#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include "services/peers/peers.h"
#include <avr/interrupt.h>
#include <stdlib.h>


#define F_CPU 16000000
#include <util/delay.h>

#define HUG_RANGE 130

#define PAN_ID 1337
uint8_t gem_id;

void btn_handler(uint8_t btn_id)
{
   if(btn_id==3){
      print("Btn 3\n"); 
      rf_broadcast('h', 0);

   }
   if(btn_id==4){
      print("Btn 4\n");
      rf_broadcast('x', 0);
      
   }
}

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
   
   gem_id = eeprom_read(0x20);//5;
   
   // -- Drivers --
   btns_init(btn_handler);                                                                                 
   rf_init(PAN_ID, gem_id, rf_rx_handler);
   leds_init();   
   // sound_init();
   tick_timer_init();

   // -- Services --
   anim_init();

   // Enable interrupts
   system_enable_int();

   // Hello World Message
   print("((( HappyGem #"); print_uchar(gem_id); print(" )))\n");

   // system_disable_jtag();

   ANIM_UPDATE(4,0,0);
   anim_flush();


   rf_packet_t *packet;
         

   while(1) {

      
      if (tick180) {
         leds_off();

         if (rf_handle('h', &packet)) {
            rf_tx(packet->source_addr, 'g', 0);
            print("Yay\n");
            ANIM_UPDATE(0,4,0);//led_idx<8?255:0,0);
            anim_flush();
            rf_clear_all();
         }
         if (rf_handle('g', &packet)) {
            uint8_t ed = packet->ed;
            uint8_t ed1 = ed/16;
            uint8_t ed2 = (ed%16)*16;
            print_uchar(ed); print("\n");
            print_uchar(ed1); print("\n");
            print_uchar(ed2); print("\n");
            ANIM_UPDATE(0,0,4);//led_idx<=ed1 ? (led_idx==ed1 ? ed2 : 255) : 0);
            anim_flush();
            rf_clear_all();
         }

         leds_on();
      }
      tick_process();
      btns_process();
      leds_process();
   }
}