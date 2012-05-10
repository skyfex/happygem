#include <avr/io.h>
#include "drivers/all.h"
#include "drivers/hg01/common.h"
#include "services/anim/anim.h"
#include "services/peers/peers.h"
#include <avr/interrupt.h>
#include <stdlib.h>


#define F_CPU 16000000
#include <util/delay.h>

#define PAN_ID 1337
uint8_t gem_id;

uint8_t mode = 0;

void btn_handler(uint8_t btn_id)
{
   if (btn_id==1) {
      print("Btn 1\n"); 
      mode = 0;

   }
   if (btn_id==2) {
      print("Btn 2\n"); 
      mode = 1;
   }
   if(btn_id==3){
      print("Btn 3\n"); 
      mode = 0;
   }
   if(btn_id==4){
      print("Btn 4\n");
      mode = 1;
      
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
   tick_timer_init();

   // -- Services --
   anim_init();

   // Enable interrupts
   system_enable_int();

   // Hello World Message
   print("((( HappyGem #"); print_uchar(gem_id); print(" )))\n");


   ADMUX_struct.refs = 1; // 1.8V

   ADMUX_struct.mux = 0; // Single ended input ADC0
   ADCSRB_struct.mux5 = 0;
   // ADSCRB_struct.adts = 0b000; // Free running
   ADCSRA_struct.adate = 0; // Auto trigger
   ADCSRA_struct.adps = 0b101; // ADC clock = CPU clock / 32 = 500khz
   ADCSRA_struct.aden = 1; // ADC enable


   int16_t lastval = 0;
   uint8_t rot = 0;

   int16_t list[16];
   uint8_t list_idx = 0;

   while(1) {

      if (tick180) {

         if (mode == 0) {
            if (tick60) {
            anim_frame[ 0] =  (led_t){{255,0,0}};
            anim_frame[ 1] =  (led_t){{64,2,0}};
            anim_frame[ 2] =  (led_t){{32,4,0}};
            anim_frame[ 3] =  (led_t){{16,8,0}};
            anim_frame[ 4] =  (led_t){{8,16,0}};
            anim_frame[ 5] =  (led_t){{4,64,0}};

            anim_frame[ 6] =  (led_t){{0,255,0}};
            anim_frame[ 7] =  (led_t){{0,64,4}};
            anim_frame[ 8] =  (led_t){{0,16,8}};
            anim_frame[ 9] =  (led_t){{0,8,16}};
            anim_frame[10] =  (led_t){{0,4,64}};

            anim_frame[11] =  (led_t){{0,0,255}};
            anim_frame[12] =  (led_t){{8,0,64}};
            anim_frame[13] =  (led_t){{16,0,16}};
            anim_frame[14] =  (led_t){{32,0,8}};
            anim_frame[15] =  (led_t){{64,0,4}};
            anim_rotate(anim_frame, rot);
            rot++;
            anim_flush();
            }
         }
         else {

            ADCSRA_struct.adsc = 1;
            while (ADCSRA_struct.adsc) {}
            
            int16_t val = ADC;

            list[list_idx] = val;
            list_idx = (list_idx+1)%16;
            int16_t avg = 0;
            for (uint8_t i=0;i<16;i++) {
               avg += list[i];
            }
            avg = avg/16;

            val -= avg;
            if (val<0) val = -val;

            ANIM_UPDATE(val, 0, 0);
            anim_flush();

            print_ushort(val);
            print("  ");

         }

         leds_process();
      }
      tick_process();
      btns_process();
   }
}