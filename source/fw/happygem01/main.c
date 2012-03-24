#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include "services/peers/peers.h"
#include <avr/interrupt.h>
#include <stdlib.h>

#define HUG_RANGE 130

#define F_CPU 16000000
#include <util/delay.h>

sound_t win_s[2] =  {
   {B3, 5},
   {F4, 40}
};
sound_pattern_t win_p = {
   2, win_s, 1, 1
};

sound_t loose_s[4] =  {
   {G3, 40},
   {Cs3, 40},
   {C3, 40}
};
sound_pattern_t loose_p = {
   3, loose_s, 1, 1
};

enum States { S_HUGSEARCH, S_COUNTDOWN, S_WINLOOSE, S_GAMEOVER};

uint8_t peer_table[16][2];
uint8_t data_table[16];

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
   uint8_t data = packet->data[1];
   uint8_t addr = packet->source_addr;
   if (type=='p') {
      return peers_rf_handler(packet);
   }
   else {
      putc(type);
   }
   return false;
}

bool find_hug(uint8_t *addr_out, uint8_t data)
{
   for(uint8_t i=0; i<16; i++) {
      if ((peer_table[i][0]+peer_table[i][1])>HUG_RANGE) {
         if (data_table[i]==data) {
            *addr_out = i;
            return 1;
         }        
      }
   }
   return 0;
}


void fw_main()
{

   // Init drivers
   system_init();
   usart_init();
   
   gem_id = eeprom_read(0x20);
   
   print("((( HappyGem #"); print_uchar(gem_id); print(" )))\n");
   
   btns_init(btn_handler);                                                                                 
   rf_init(1337, gem_id, rf_rx_handler);
 
   leds_init();   
   
   // Init services
   anim_init();

   sound_init();
   


   // Enable interrupts
   system_enable_int();
   
   tick_timer_init();
   
   uint8_t tick_prev = 0;

   uint8_t i;

   int8_t level = 0;
   uint8_t rotation = 0;
   uint8_t counter = 0; 
   uint8_t did_win = 0;
   uint8_t state = S_HUGSEARCH;

   rf_packet_t *packet;
   		
   while(1) {

      
      if (tick!=tick_prev) {
         leds_off();


         switch(state) {
            case S_HUGSEARCH: ;
               uint8_t hug_addr;
               if (rf_handle('h', &packet)) {
                  did_win = packet->data[1];
                  print_uchar(did_win);
                  counter = 120;
                  state = S_COUNTDOWN;
                  rf_clear_all();
               }
               else if (find_hug(&hug_addr, level)) {
                  did_win = rand()%2;
                  rf_tx(hug_addr, 'h', !did_win, true);
                  peers_reset();
                  counter = 120;
                  state = S_COUNTDOWN;
                  rf_clear_all();
                  print("Hugging "); print_ushort(hug_addr); print("\n");
                  print_uchar(did_win);
               } 
               else {
                  rf_broadcast('p', level);
               }
               /// --- Animation ---
               if (level==0) {
                  ANIM_UPDATE(2,0,0);

                  //
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
                  //

                  anim_flush();
               }
               else {
                  ANIM_UPDATE(0,0,0);
                  for (i=12;i<16;i++) 
                     anim_frame[i] = (led_t){0,7,0};
                  if (level>1)
                  for (i=8;i<12;i++)
                     anim_frame[i] = (led_t){0,5,6};
                  if (level>2)
                  for (i=4;i<8;i++)
                     anim_frame[i] = (led_t){7,0,5};
                  if (level>3)
                  for (i=0;i<4;i++) 
                     anim_frame[i] = (led_t){7,5,0};
                  rotation += 4;
                  anim_rotate(anim_frame, rotation);
                  anim_flush();
               }

            break;
            case S_COUNTDOWN: ;
               uint8_t x = counter/7;
               ANIM_UPDATE(0,led_idx<x?5:0,led_idx<x?6:0);
               anim_flush();
               if (counter==0) {
                  if (did_win) 
                     sound_play(&win_p);
                  else
                     sound_play(&loose_p);
                  state = S_WINLOOSE;
                  counter = 60;
               }
               else 
                  counter--;

            break;
            case S_WINLOOSE: ;
                  uint8_t y = anim_sin(counter*2)/2;
                 ANIM_UPDATE(did_win?0:y,did_win?y:0,0);
                 anim_flush();
                 counter--;
                 if (counter==0) {
                     if (did_win)
                        level++;
                     else
                        level--;
                     if (level<0) level = 0;
                     if (level==5)
                           state=S_GAMEOVER;
                     else
                        state = S_HUGSEARCH;
                 } 
            break;
            case S_GAMEOVER: ;
               uint8_t z = anim_sin(tick*2)/2;
               ANIM_UPDATE(z,z,z);
               anim_flush();
            break;
         }

         sound_process();
         
         leds_on();
         
         tick_prev = tick;
      }
      

      btns_process();
      leds_process();
   }
}