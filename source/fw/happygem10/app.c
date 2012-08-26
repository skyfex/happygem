
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

static uint16_t timer;
static uint8_t mode;
static uint8_t mode_priority;

static uint8_t brightness;

static uint16_t bat_level;

void app_init()
{
  brightness = 12;
  leds_set_brightness(4);
  mode_priority = 0;
	mode = MODE_BAT_SAMPLE;
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
   if(btn_id==4){
      print("Btn 4\n");
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
            timer = 0;
            mode = MODE_HUG_ANIMATION;

            rf_tx(packet->source_addr, 'H', 0);

            peers_reset();  
            rf_clear_all();      
          }
          else if (peers_find_hug(&addr_out)) {

            dna_transmit('h',  addr_out);

            // uint8_t buffer[1];
            // rf_packet_t o_packet = {
            //    .req_ack = 1,
            //    .dest_addr = addr_out,
            //    .length = 1,//sizeof(dna)+1,
            //    .data = buffer
            // };
            // buffer[0] = 'h';
            // // memcpy(buffer+1, dna, sizeof(dna));

            // rf_transmit(&o_packet);
            peers_reset();

            timer = 0;
            mode = MODE_HUG_ACK_WAIT;
          }

          // rf_clear_old();


    			break;







        case MODE_SLEEP:
          mode_priority = 9;

          ANIM_UPDATE(0,255,0);
          anim_flush();

          // Todo: Sleep RF
          leds_off();
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
            // print_ushort(bat_level); print("\n");
            leds_set_brightness(brightness);
            timer = 0;
            mode = MODE_BAT_SHOW;
          }
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
            uint8_t x = t<indicator ? t : indicator;
            if (i<indicator && i<timer/2)
              anim_frame[i] = (pix_t){{(15-x)*16,x*16,0}};
            else
              anim_frame[i] = (pix_t){{0,0,0}};
          }
          anim_flush();

          timer++;

          if (timer==128) {
            timer = 0;
            mode = MODE_DEFAULT;
          }

          break;

        // Show brightness
    		case MODE_BRIGHTNESS:
          mode_priority = 0;

    			for (uint8_t i=0; i<16;i++) {
    				if (i <= brightness) {
    					anim_frame[i] = (pix_t){{0,128,192}};
    				}
    				else {
    					anim_frame[i] = (pix_t){{0,0,0}};
    				}
    			}
    			anim_flush();
    			timer++;
    			if (timer==128) {
    				timer = 0;
  					mode = MODE_DEFAULT;
  				}
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
          break;

        case MODE_HUG_ACK_WAIT:
          mode_priority = 0;

          anim_clear(anim_frame);
          anim_flush();

          if (rf_handle('H', &packet)) {
            timer = 0;
            mode = MODE_HUG_ANIMATION;
          }
          else {
            timer++;
            if (timer >= 32) {
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

          break;

    		default:
    			mode = MODE_DEFAULT;
    			break;
    	}
    }
}