#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include "services/peers/peers.h"
#include <avr/interrupt.h>
#include <stdlib.h>


#define F_CPU 16000000
#include <util/delay.h>

#define PAN_ID 1337
uint8_t gem_id;

void btn_handler(uint8_t btn_id)
{
   if(btn_id==3){
      print("Btn 3\n"); 
      

   }
   if(btn_id==4){
      print("Btn 4\n");

      
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
   
   gem_id = eeprom_read(0x20);
   
   // -- Drivers --
   btns_init(btn_handler);                                                                                 
   rf_init(PAN_ID, gem_id, rf_rx_handler);
   leds_init();   
   sound_init();
   tick_timer_init();

   // -- Services --
   anim_init();

   // Enable interrupts
   system_enable_int();

   // Hello World Message
   print("((( HappyGem #"); print_uchar(gem_id); print(" )))\n");

   system_disable_jtag();
   
   rf_packet_t *packet;
   		
   uint8_t rot=0;

   while(1) {

      
      if (tick60) {
         leds_off();

         if (tick1) {
            uint8_t bat = battery_measure();
            print_uchar(bat);
            print("\n");
         }

         ANIM_UPDATE(led_idx<tick?8:0,0,0);
         anim_flush();

         rot+=4;

         sound_process();
         tick_process();
         leds_on();
      }
      
      btns_process();
      leds_process();
   }
}