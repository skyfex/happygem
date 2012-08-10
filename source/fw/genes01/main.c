#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include "services/peers/peers.h"
#include <avr/interrupt.h>
#include <stdlib.h>

#include "dna.h"

#define F_CPU 16000000
#include <util/delay.h>

#define HUG_RANGE 130

#define PAN_ID 1337
uint8_t gem_id;
uint8_t xmode = 0;

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


   // -- Get random value --
   ADMUX_struct.refs = 1; // 1.8V
   ADMUX_struct.mux = 0; // Single ended input ADC0
   ADCSRB_struct.mux5 = 0;
   // ADSCRB_struct.adts = 0b000; // Free running
   ADCSRA_struct.adate = 0; // Auto trigger
   ADCSRA_struct.adps = 0b101; // ADC clock = CPU clock / 32 = 500khz
   ADCSRA_struct.aden = 1; // ADC enable
   ADCSRA_struct.adsc = 1;
   while (ADCSRA_struct.adsc) {}
   int16_t val = ADC;
   ADCSRA_struct.aden = 0; // ADC disable

   srand(val);

   // Hello World Message
   print("((( HappyGem #"); print_uchar(gem_id); print(" )))\n");

   // system_disable_jtag();

   ANIM_UPDATE(0,0,0);
   anim_flush();


   rf_packet_t *packet;

   uint8_t hugmode = 0;
   uint16_t countdown = 0;

#define genome_n 3
#define state_size 3
 
   genome_t dna[genome_n];
   uint8_t states[genome_n][state_size];

   uint8_t i;
   for (i=0;i<genome_n;i++) {
      dna[i].type = rand();
      dna[i].speed = rand();
      dna[i].startpos  = rand();
      dna[i].size = rand();
      dna[i].r = rand();
      dna[i].g = rand();
      dna[i].b = rand();
   }
   for (i=0;i<genome_n;i++) {
      switch (dna[i].type%1) {
         case 0:
            type0_init(&dna[i], states[i]);
            break;
         case 1:
            type1_init(&dna[i], states[i]);
            break;
         case 2:
            type2_init(&dna[i], states[i]);
            break;
      }      
   }
      
   while(1) {

      
      if (tick192) {
         leds_off();
         if (xmode) break;
         if (countdown==0) {
            uint8_t addr_out;
            if (rf_handle('h', &packet)) {
               uint8_t buffer[sizeof(dna)+1];
               rf_packet_t o_packet = {
                  .req_ack = 1,
                  .dest_addr = packet->source_addr,
                  .length = sizeof(dna)+1,
                  .data = buffer
               };
               buffer[0] = 'i';
               memcpy(buffer+1, dna, sizeof(dna));
               rf_transmit(&o_packet); 

               memcpy(dna, packet->data+1, sizeof(dna)/3);

               hugmode = 2; 
               countdown = 192*2;  
               peers_reset();  
               rf_clear_all();      
            }
            else if (peers_find_hug(&addr_out, HUG_RANGE, 0)) {

               uint8_t buffer[sizeof(dna)+1];
               rf_packet_t o_packet = {
                  .req_ack = 1,
                  .dest_addr = addr_out,
                  .length = sizeof(dna)+1,
                  .data = buffer
               };
               buffer[0] = 'h';
               memcpy(buffer+1, dna, sizeof(dna));

               rf_transmit(&o_packet);
               hugmode = 1;
               countdown = 192*2;
               peers_reset();
            }
            else {
               peers_broadcast(0);
            }
         }
         else {
            if (rf_handle('i', &packet)) {
               memcpy(dna, packet->data+1, sizeof(dna)/3);
               hugmode = 3;
               rf_clear_all();
            }
            countdown--;
            if (countdown==0) {
               hugmode = 0;
            }
         }

         if (hugmode) {
            ANIM_UPDATE(hugmode==2?4:0,4,hugmode==3?4:0);
         }
         else {
            ANIM_UPDATE(0,0,0);

            for (i=0;i<genome_n;i++) {
               switch (dna[i].type%1) {
                  case 0:
                     type0(&dna[i], states[i]);
                     break;
                  case 1:
                     type1(&dna[i], states[i]);
                     break;
                  case 2:
                     type2(&dna[i], states[i]);
                     break;
               }
            }

         }

         anim_flush();

         leds_on();
      }
      tick_process();
      btns_process();
      leds_process();
   }

   ANIM_UPDATE(4,0,0);
   anim_flush();
   while (1) {
      leds_process();
   }
}


