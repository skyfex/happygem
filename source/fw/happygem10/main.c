#include <avr/io.h>
#include "drivers/all.h"
#include "drivers/hg01/common.h"
#include "services/anim/anim.h"
#include "services/peers/peers.h"
#include <avr/interrupt.h>
#include <stdlib.h>
#define F_CPU 16000000
#include <util/delay.h>

#include "app.h"
#include "dna.h"


#define PAN_ID 1337
uint8_t gem_id;

uint8_t mode = 0;


bool rf_rx_handler(rf_packet_t *packet)
{
   uint8_t type = packet->data[0];
   if (type=='p') {
      return peers_rf_handler(packet);
   }
   return false;
}




void fw_main()
{

   // Init drivers
   system_init();
   usart_init();
   
   gem_id = eeprom_read(0x20);
   
   // -- Drivers --
   btns_init(app_btn_handler);                                                                                 
   rf_init(PAN_ID, gem_id, rf_rx_handler);
   leds_init();   
   tick_timer_init();

   // system_srand();
   // srand(100);

   // -- Services --
   anim_init();

   // Enable interrupts
   system_enable_int();

   // Hello World Message
   print("((( HappyGem #"); print_uchar(gem_id); print(" )))\n");

   ANIM_UPDATE(0, 0, 0);
   anim_flush();

   app_init();
   dna_init();

 		
   while(1) {

      if (tick192) {
<<<<<<< HEAD
         app_process();
=======

         if (tick64) {





            if (mode==0) {


               rf_packet_t *packet;
               uint8_t addr_out;

               if (rf_handle('h', &packet)) {
                  mode = 4; rot = 0; 
                  peers_reset();  
                  rf_clear_all();      
               }
               else if (peers_find_hug(&addr_out, HUG_RANGE, 0)) {

                  uint8_t buffer[1];
                  rf_packet_t o_packet = {
                     .req_ack = 1,
                     .dest_addr = addr_out,
                     .length = 1,//sizeof(dna)+1,
                     .data = buffer
                  };
                  buffer[0] = 'h';
                  // memcpy(buffer+1, dna, sizeof(dna));

                  rf_transmit(&o_packet);
                  mode = 4; rot = 0;
                  peers_reset();
               }
               else {
                  if (tick32)
                     dna_anim();
                  peers_broadcast(0);
               }


               anim_flush();
            }
            if (mode==1) {

               anim_flush();
            }
            if (mode==2) {
               draw_rainbow(anim_frame);
               anim_rotate(anim_frame, rot);
               rot+=1;
               anim_flush();
            }
            if (mode==3) {
               ANIM_UPDATE(255,255,255);
               anim_flush();
            }
            if (mode==4) {
               rot++;
               uint8_t i;
               for (i=0;i<16;i++) {
                  anim_frame[i] = (pix_t){{0,anim_sin(rot*2),0,255}};
               }
               if (rot==254) {
                 mode = 0;
                 ANIM_UPDATE(0,0,0); 
               } 
               anim_flush();
            }
         }

>>>>>>> e87dc4520bfba4aeb68b9c1879c8db7f5a8deb77
         leds_process();
      }
      tick_process();
      btns_process();
   }
}