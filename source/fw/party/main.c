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

uint8_t peer_pings[16];
uint16_t peer_times[16];

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
   // if (type=='p') {
   //    return peers_rf_handler(packet);
   // }
   if (type=='i') {
      uint16_t addr = packet->source_addr;
      uint16_t* data = (uint16_t*)&packet->data[1];
      if (addr < 16) {
         if (peer_pings[addr]<4)
            peer_pings[addr]++;
         peer_times[addr] = *data;
      }

      return true;
   }
   return false;
}



void animate(uint16_t time)
{
   uint8_t i;
   #define band_count 13
   led_t band_colors[band_count] = {
      {{8,5,0}}, {{0,5,8}}, {{5,5,0}}, {{5,0,5}},
      {{0,8,5}}, {{2,5,8}}, {{5,8,0}}, {{5,5,0}},
      {{8,5,2}}, {{8,5,2}}, {{5,0,8}}, {{8,0,5}},
      {{6,6,6}}
   };
   uint8_t band_widths[band_count] = {
      8, 16, 32, 16,
      32, 16, 32, 64,
      32, 16, 8, 16,
      32
   };
   uint16_t band_sum = 0;
   for (i=0;i<band_count;i++) {
      band_sum += band_widths[i];
   }
   uint16_t t = time%band_sum;

   int16_t band_x = t;
   uint8_t band_i;
   for(band_i=0; band_i<band_count; band_i++) {
      band_x -= band_widths[band_i];
      if (band_x < 0) {
         band_x += band_widths[band_i];
         break;
      }
   }  

   for (i=0;i<7;i++) {
      led_t color = band_colors[band_i];
      switch(i) {
         case 0:
            anim_frame[13] = color;
            anim_frame[12] = color;
            anim_frame[11] = color;
            break;
         case 1:
            anim_frame[14] = color;
            anim_frame[10] = color;
            break;
         case 2:
            anim_frame[15] = color;
            anim_frame[9] = color;
            break;
         case 3:
            anim_frame[0] = color;
            anim_frame[8] = color;
            break;
         case 4:
            anim_frame[1] = color;
            anim_frame[7] = color;
            break;
         case 5:
            anim_frame[2] = color;
            anim_frame[6] = color;
            break;
         case 6:
            anim_frame[3] = color;
            anim_frame[4] = color;
            anim_frame[5] = color;
            break;
      }
      band_x++;
      if (band_x >= band_widths[band_i]) {
         band_x = 0;
         band_i++;
         if (band_i>=band_count)
            band_i = 0;
      }
   }

}

void fw_main()
{

   // _delay_ms(1000);
   // eeprom_write(0x20, 9);
   // while(1) {}

   // Init drivers
   system_init();
   usart_init();
   
   gem_id = eeprom_read(0x20);
   
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

   ANIM_UPDATE(0,4,0);
   anim_flush();


   uint8_t i;
   rf_packet_t *packet;
   uint16_t time = 0;
   uint16_t time_delta = 14;

   while(1) {

      
      if (tick180) {
         leds_off();

         if (tick5) {
            rf_broadcast_16('i', time);
         }
         if (tick4) {
            uint16_t prev_time = 0;
            bool has_predecessor=false;
            for (i=0;i<16;i++) {
               if (peer_pings[i]>0) {
                  peer_pings[i]--;
                  if (i<gem_id) {
                     prev_time = peer_times[i];
                     has_predecessor = true;
                  }
               }

            }
            if (has_predecessor) {
               uint16_t my_time = prev_time-time_delta;
               int16_t delta_error = time-my_time-time_delta;
               if (delta_error < -10 || delta_error > 10) 
                  time = my_time;
            }
         }
         if (tick30) {
            animate(time/2);
            anim_flush();
            time++;
         }


         leds_on();
      }
      tick_process();
      btns_process();
      leds_process();
   }
}