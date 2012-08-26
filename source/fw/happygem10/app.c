
#include "app.h"

#include <stdlib.h>

#include "drivers/all.h"
#include "services/anim/anim.h"
#include "services/peers/peers.h"

#include "dna.h"
#include "misc.h"



#define HUG_RANGE 80

#define BTN_SLEEP      3
#define BTN_BRIGHTNESS 1
#define BTN_FLASHLIGHT 2
#define BTN_KILL 4

#define MODE_DEFAULT    0
#define MODE_SLEEP   1
#define MODE_BAT_SAMPLE  2
#define MODE_BAT_SHOW   3
#define MODE_BRIGHTNESS 4
#define MODE_FLASHLIGHT 5
#define MODE_LED_FAILURE_WAIT 6
#define MODE_LED_FAILURE_BLINK 7
#define MODE_HUG_ACK_WAIT 8
#define MODE_HUG_ANIMATION 9
#define MODE_RESET 10

static uint16_t timer;
static uint8_t mode;
static uint8_t mode_priority;
static uint8_t btn_hold_timer;
static uint8_t brightness;

static uint16_t bat_level;

static uint16_t last_hug_addr;

void app_init()
{
  last_hug_addr = 0;
  btn_hold_timer = 0;
  brightness = 12;
  leds_set_brightness(brightness);
  mode_priority = 0;
	mode = MODE_DEFAULT;//MODE_BAT_SAMPLE;
}

void app_btn_handler(uint8_t btn_id)
{
   if (btn_id==BTN_SLEEP) {
      if (mode_priority < 9) {
        print("Sleep\n"); 
        timer = 0;
        mode = MODE_SLEEP;
      }
   }
   if (btn_id==BTN_BRIGHTNESS) {
      if (mode_priority < 5) {
        print("Brightness\n"); 
        brightness += 1;
        if (brightness >= 16)
        	brightness = 0;
        leds_set_brightness(brightness);
        print_uchar(brightness); print("\n");
        timer = 0;
        mode = MODE_BRIGHTNESS;
      }
   }
   if(btn_id==BTN_FLASHLIGHT){
      print("Flashlight\n"); 
      // Handled in main loop
   }
   if(btn_id==BTN_KILL){
      print("Kill\n");
   }
}

void app_process()
{
  uint8_t i;

  rf_packet_t *packet;
  uint16_t addr_out;

    if (tick64) {

      if (leds_failure_detected()) {
        brightness = 0;
        leds_set_brightness(0);
        leds_off();
        mode = MODE_LED_FAILURE_WAIT;
        timer = 0;
      }
    	else if (btn_is_down(BTN_FLASHLIGHT)) {
        if (mode_priority < 5) {
      		mode = MODE_FLASHLIGHT;
      		timer = 0;
        }
    	}
      else if (btn_is_down(BTN_KILL)) {
        if (mode != MODE_RESET)
          btn_hold_timer++;
        if (btn_hold_timer == 128) {
          btn_hold_timer = 0;
          peers_unhugged_reset();
          timer = 0;
          mode = MODE_RESET;
        }
      }
      else {
        btn_hold_timer = 0;
      }

    	switch (mode) {

    		case MODE_DEFAULT:
          mode_priority = 0;

          peers_process();

          dna_anim();

          pix_t *overlay = anim_tempframe1();
          if (peers_unhugged_is_close()) {
            if (dna_beat_count()%2==0) {
              for (uint8_t i=0;i<16;i++) {
                overlay[i] = (pix_t){{255,255,255,anim_sin(dna_beat_t()*2)}};
              }
              anim_comp_over(anim_frame, overlay);
            }
          }
          else if (peers_unhugged_in_range()) {
            if (dna_beat_count()%8==0) {
              for (uint8_t i=0;i<16;i++) {
                overlay[i] = (pix_t){{255,255,255,anim_sin(dna_beat_t()*2)}};
              }
              anim_comp_over(anim_frame, overlay);
            }
          }
          anim_flush();

          if (rf_handle('h', &packet)) {
            rf_clear_all();      

            timer = 0;
            mode = MODE_HUG_ANIMATION;

            dna_transmit('H', packet->source_addr);

            last_hug_addr = packet->source_addr;
            dna_recieve(packet);
            peers_do_hug(packet->source_addr);

            peers_reset();  
          }
          else if (peers_find_hug(&addr_out)) {
            if (addr_out != last_hug_addr) {
              rf_clear_all();      

              dna_transmit('h',  addr_out);

              peers_reset();

              timer = 0;
              mode = MODE_HUG_ACK_WAIT;
            }
          }

          // rf_clear_old();


    			break;






        case MODE_SLEEP:
          mode_priority = 9;

          ANIM_UPDATE(0,255,0);
          anim_flush();

          // Todo: Sleep RF
          leds_off();
          rf_clear_all();      
          rf_sleep();

          system_sleep();
          print("Waking up\n");

          rf_wake();
          leds_on();

          timer = 0;
          mode = MODE_BAT_SAMPLE;
          break;

        // Measure Battery Level
        case MODE_BAT_SAMPLE:
          mode_priority = 9;

          if (timer==0) {
            leds_set_brightness(0);
            ANIM_UPDATE(255,255,255);
            anim_flush();

            // leds_off();
            bat_level = 0;
          }
          uint16_t meas = battery_measure();
          srand(rand()*meas); // Get some real randomness going
          bat_level += meas;
          timer++;
          if (timer==8) {
            bat_level /= 8;
             dna_init(); // Todo: remove
            // print_ushort(bat_level); print("\n");
            leds_set_brightness(brightness);
            timer = 0;
            mode = MODE_BAT_SHOW;
          }
          rf_clear_all();      
          break;

        // Show Battery Level
        case MODE_BAT_SHOW:
          mode_priority = 9;

          uint16_t indicator;
          indicator = bat_level - 100;
          indicator = indicator / 56;
          if (indicator > 15) indicator = 15;

          for (i=0; i<16; i++) {
            uint8_t t = timer/2;
            // uint8_t x = t<indicator ? t : indicator;
            if (i < indicator && i < t)
              anim_frame[i] = (pix_t){{(15-i)*16,i*16,0}};
            else
              anim_frame[i] = (pix_t){{0,0,0}};
          }
          anim_flush();

          timer++;

          if (timer==128) {
            timer = 0;
            mode = MODE_DEFAULT;
          }
          rf_clear_all(); 
          break;

        // Show brightness
    		case MODE_BRIGHTNESS:
          mode_priority = 0;

    			for (uint8_t i=0; i<16;i++) {
    				if (i <= brightness) {
    					anim_frame[i] = (pix_t){{0,100,150}};
    				}
    				else {
    					anim_frame[i] = (pix_t){{200,50,0}};
    				}
    			}
    			anim_flush();
    			timer++;
    			if (timer==128) {
    				timer = 0;
  					mode = MODE_DEFAULT;
  				}
          rf_clear_all(); 
    			break;

        // Pure white light for 4 seconds
    		case MODE_FLASHLIGHT:
          mode_priority = 0;

    			ANIM_UPDATE(255, 255, 255);
    			anim_flush();
    			timer++;
    			if (timer==255) {
      				timer = 0;
    					mode = MODE_DEFAULT;
  				}
          rf_clear_all(); 
    			break;

        // Handle low-power failure
        case MODE_LED_FAILURE_WAIT:
          mode_priority = 10;

          ANIM_UPDATE(0,0,0);
          anim_flush();
          timer++;
          if (timer==255) {
            timer = 0;
            leds_on();
            mode = MODE_LED_FAILURE_BLINK;
          }
          rf_clear_all(); 
          break;  
        case MODE_LED_FAILURE_BLINK: ;
          mode_priority = 10;

          uint8_t x = anim_sin(timer);
          ANIM_UPDATE(x,0,0);
          anim_flush();
          timer++;
          if (timer==255) {
            timer = 0;
            mode = MODE_DEFAULT;
          }
          rf_clear_all(); 
          break;

        case MODE_HUG_ACK_WAIT:
          mode_priority = 0;

          anim_clear(anim_frame);
          anim_flush();

          if (rf_handle('H', &packet)) {
            rf_clear_all(); 
            dna_recieve(packet);
            last_hug_addr = packet->source_addr;
            peers_do_hug(packet->source_addr);
            timer = 0;
            mode = MODE_HUG_ANIMATION;
          }
          else {
            timer++;
            if (timer >= 64) {
              timer = 0;
              mode = MODE_DEFAULT;
            }            
          }

          break;

        case MODE_HUG_ANIMATION: ;
          mode_priority = 0;

          draw_rainbow(anim_frame);
          anim_rotate(anim_frame, timer);
          anim_flush();

          timer+=6;

          if (timer >= 64*6*3) {
            mode = MODE_DEFAULT;
            timer = 0;
          }
          rf_clear_all(); 
          break;

        case MODE_RESET: ;
          mode_priority = 9;
          draw_rainbow(anim_frame);
          anim_rotate(anim_frame, timer*6);
          anim_flush();

          timer++;

          if (timer >= 64*3) {
            mode = MODE_DEFAULT;
            timer = 0;
          }
          rf_clear_all(); 
          break;

    		default:
    			mode = MODE_DEFAULT;
    			break;
    	}



    }
}