
#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include "services/peers/peers.h"
#include "app.h"
#include "misc.h"

#define HUG_RANGE 80

#define BTN_BRIGHTNESS 2
#define BTN_FLASHLIGHT 3

#define MODE_DEFAULT    0
#define MODE_BRIGHTNESS 3
#define MODE_FLASHLIGHT 4

uint8_t timer;
uint8_t mode;

uint8_t brightness;


void app_init()
{
	mode = 0;
}

void app_btn_handler(uint8_t btn_id)
{
   if (btn_id==1) {
      print("Btn 1\n"); 
      mode = 0;
      // dna_init();
      uint16_t bat = battery_measure();
      print_ushort(bat); print("\n");
   }
   if (btn_id==BTN_BRIGHTNESS) {
      print("Brightness\n"); 
      brightness += 1;
      if (brightness >= 16)
      	brightness = 0;
      leds_set_brightness(brightness);
      print_uchar(brightness); print("\n");
      timer = 0;
      mode = MODE_BRIGHTNESS;
   }
   if(btn_id==BTN_FLASHLIGHT){
      print("Flashlight\n"); 
      mode = 2;
   }
   if(btn_id==4){
      print("Btn 4\n");
      mode = 3;
   }
}

void app_process()
{
    if (tick64) {

    	if (btn_is_down(BTN_FLASHLIGHT)) {
    		mode = MODE_FLASHLIGHT;
    		timer = 0;
    	}

    	switch (mode) {
    		case MODE_DEFAULT:
	    		ANIM_UPDATE(0,0,0);
	    		anim_flush();
    			break;
    		case MODE_BRIGHTNESS:
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
    		case MODE_FLASHLIGHT:
    			ANIM_UPDATE(255, 255, 255);
    			anim_flush();
    			timer++;
    			if (timer==128) {
      				timer = 0;
    					mode = MODE_DEFAULT;
  				}
    			break;
    		default:
    			mode = MODE_DEFAULT;
    			break;
    	}



		// if (btn_is_down(2)) {
		// 	uint8_t brightness = leds_get_brightness();
		// 	brightness += 1;
		// 	print_ushort(brightness); print("\n");
		// 	leds_set_brightness(brightness);            
		// }



  //       if (mode==0) {

  //          // rf_packet_t *packet;
  //          // uint8_t addr_out;

  //          // if (rf_handle('h', &packet)) {
  //          //    mode = 4; rot = 0; 
  //          //    peers_reset();  
  //          //    rf_clear_all();      
  //          // }
  //          // else if (peers_find_hug(&addr_out, HUG_RANGE, 0)) {

  //          //    uint8_t buffer[1];
  //          //    rf_packet_t o_packet = {
  //          //       .req_ack = 1,
  //          //       .dest_addr = addr_out,
  //          //       .length = 1,//sizeof(dna)+1,
  //          //       .data = buffer
  //          //    };
  //          //    buffer[0] = 'h';
  //          //    // memcpy(buffer+1, dna, sizeof(dna));

  //          //    rf_transmit(&o_packet);
  //          //    mode = 4; rot = 0;
  //          //    peers_reset();
  //          // }
  //          // else {
  //          //    dna_anim();
  //          //    peers_broadcast(0);

  //          anim_flush();
  //       }
  //       if (mode==1) {

  //          anim_flush();
  //       }
  //       if (mode==2) {
  //          draw_rainbow(anim_frame);
  //          anim_rotate(anim_frame, rot);
  //          rot+=1;
  //          anim_flush();
  //       }
  //       if (mode==3) {
  //          ANIM_UPDATE(255,255,255);
  //          anim_flush();
  //       }
  //       if (mode==4) {
  //          rot++;
  //          uint8_t i;
  //          for (i=0;i<16;i++) {
  //             anim_frame[i] = (pix_t){{0,anim_sin(rot*2),0,255}};
  //          }
  //          if (rot==254) {
  //            mode = 0;
  //            ANIM_UPDATE(0,0,0); 
  //          } 
  //          anim_flush();
  //       }
  //    }
    }
}