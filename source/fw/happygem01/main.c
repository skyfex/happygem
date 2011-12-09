#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include <avr/interrupt.h>
// #include <util/delay.h>

#define GEM_ADDR 0

void btn_handler(uint8_t btn_id)
{
   if(btn_id==3){
      print("Btn 3\n");
      ANIM_UPDATE(0,3,0);
      anim_flush();
   }
   if(btn_id==4){
      print("Btn 4\n");
      ANIM_UPDATE(0,0,3);
      anim_flush();
   }
}

void rf_rx_handler(rf_packet_t *packet)
{
   uint8_t type = packet->data[0];
   uint8_t data = packet->data[1];
}

void fw_main()
{
   // Init drivers
   system_init();
   usart_init();
   btns_init(btn_handler);
   leds_init();
   rf_init(1337, GEM_ADDR, rf_rx_handler);

   // Enable interrupts
   system_enable_int();
   
   // Init services
   anim_init();

   print("Hello World\n");
   
   ANIM_UPDATE(4,0,0);
   anim_flush();
   
   while(1) {
      btns_process();
   }
}