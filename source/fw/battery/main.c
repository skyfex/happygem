#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include <avr/interrupt.h>
#include <stdlib.h>


#define F_CPU 16000000
#include <util/delay.h>

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
   return false;
}

ISR(TIMER4_COMPA_vect)
{
   PIND = (1<<3);
}



void fw_main()
{

   // Init drivers
   system_init();
   usart_init();
   
   gem_id = eeprom_read(0x20);
   
   print("(((O))) HappyGem #"); print_uchar(gem_id); print(" (((O)))\n");
   
   btns_init(btn_handler);                                                                                 
   rf_init(1337, gem_id, rf_rx_handler);
 
   leds_init();   
   
   // Init services
   anim_init();

   sound_init();
   
   // ANIM_UPDATE(0,1,0);
   // anim_flush();

   // Enable interrupts
   system_enable_int();
   
   tick_timer_init();
   
   uint8_t tick_prev = 0;

   uint8_t i;

   uint8_t rotation = 0;

   system_disable_jtag();

   DDRF &= ~(1<<7);
   
   ADCSRA_struct.aden = 1;
   ADCSRA_struct.adps = 0b111; // Clock division: 128
   ADMUX_struct.refs = 0b01; // 1.8V reference
   ADMUX_struct.adlar = 0; // Right adjust    
   ADMUX_struct.mux = 0b00111; // Single ended Channel 7

   ADCSRA_struct.adsc = 1; // Start conversion
   		
   uint32_t acc = 0;
   uint8_t acc_n = 0;

   while(1) {

      
      if (tick!=tick_prev) {
         leds_off();
         
         for (i=12;i<16;i++) 
      		anim_frame[i] = (led_t){0,7,0};
      	for (i=8;i<12;i++)
      		anim_frame[i] = (led_t){0,5,6};
      	for (i=4;i<8;i++)
      		anim_frame[i] = (led_t){7,0,5};
      	for (i=0;i<4;i++) 
      		anim_frame[i] = (led_t){7,5,0};

         rotation += 4;
         anim_rotate(anim_frame, rotation);
         anim_flush();
          
         leds_on();

         if (ADCSRA_struct.adsc==0) {
            acc += ADC;
            acc_n++;
            if (acc_n==10) {
               acc *= 1000;
               print_ushort(acc/3413);
               print("\n");
               acc_n = 0;
               acc = 0;
            }
            ADCSRA_struct.adsc = 1;
         }
         
         tick_prev = tick;
      }
      

      btns_process();
      leds_process();
   }
}