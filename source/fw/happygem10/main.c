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
#include "sex.h"


#define GEM_ID_ADDR 0x20
#define PEERS_EEPROM_ADDR  0x400
#define DNA_EEPROM_ADDR  0x800
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

   gem_id = eeprom_read(GEM_ID_ADDR);
#ifdef GEM_ID
   if (gem_id != GEM_ID) {
      eeprom_write(GEM_ID_ADDR, GEM_ID);
   }
#endif

   srand(gem_id);
   
   // -- Drivers --
   btns_init(app_btn_handler);                                                                                 
   rf_init(PAN_ID, gem_id, rf_rx_handler);
   leds_init();   
   tick_timer_init();

   // -- Services --
   anim_init();

   // Enable interrupts
   system_enable_int();

   // Hello World Message
   print("((( HappyGem #"); print_uchar(gem_id); print(" )))\n");

   ANIM_UPDATE(0, 0, 0);
   anim_flush();

   peers_init(PEERS_EEPROM_ADDR);
   app_init();
   dna_init(DNA_EEPROM_ADDR);

 		
   while(1) {

      if (tick192) {
         app_process();
         leds_process();
      }
      tick_process();
      btns_process();
   }
}