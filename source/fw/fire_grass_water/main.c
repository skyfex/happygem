#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include <avr/interrupt.h>

#include "drivers/hg01/common.h"
// #include <util/delay.h>

#define KILL_RANGE 35
#define GEM_ADDR 4

uint8_t donald;

uint8_t state;
uint8_t counter;
uint8_t points;
uint8_t my_color;

#define SOUND_BASE 16
#define SOUND_INTERVAL 1
uint8_t sound_counter;
uint8_t sound_up;

#define COUNTDOWN_TIME 30

void update_color();

uint8_t range_table[16];

ISR(TIMER1_COMPA_vect)
{
   if (sound_counter!=0) {
      TCCR4B_struct.cs4 = 0x05;

      sound_counter--;
      
      uint16_t num = SOUND_BASE;
      if (sound_up)
         num += SOUND_INTERVAL*sound_counter;
      else
         num -= SOUND_INTERVAL*sound_counter;
         
      OCR4AH = (num >> 8);
      OCR4AL = (num & 0xFF);
   }
   else
      TCCR4B_struct.cs4 = 0x00;
      
   if (state==0)
      rf_broadcast('b', my_color);
   // putc('.');
}

ISR(TIMER3_COMPA_vect)
{
   if (counter!=0) {
      counter--;
      if (counter==0) {
         state = 1;
      }
   }
   if (state==0) {
      donald = 1;
   }
   if (state==30)
   state = 31;
   if (state==32)
   state = 33;
}

ISR(TIMER4_COMPA_vect)
{
   PIND = (1<<3);
}

void btn_handler(uint8_t btn_id)
{
   // if(btn_id==3){
   //    print("Btn 3\n");
   //    state = 30;
   //    counter = 0;
   //    my_color = GEM_ADDR%3;
   //    points = 0;
   //    rf_broadcast('r', 0);
   // }
   // if(btn_id==4){
   //    print("Btn 4\n");
   //    counter = COUNTDOWN_TIME;
   //    rf_broadcast('s', 0);
   // }
}

void do_win()
{
   points++;
   state = 10;
   counter = 5;
   sound_counter = 3;
   sound_up = 1;
}
void do_lose()
{  
   state = 20;
   counter = 5;
   sound_counter = 3;
   sound_up = 0;
}

void rf_rx_handler(rf_packet_t *packet)
{
   uint8_t type = packet->data[0];
   uint8_t data = packet->data[1];
   
   uint8_t source = (uint8_t)packet->source_addr;
   if (!packet->has_source || source>15)
      return;
   
  if (state==0) {
     if (type=='b') {
            uint8_t ed = packet->ed;
            if (ed>63) ed = 63;
            range_table[source] = (range_table[source]>>1) + (ed>>1);
            if (range_table[source] > KILL_RANGE) {
               if (my_color==0) {
                  if (data==1) {
                     rf_tx(packet->source_addr, 'c', 0);
                     my_color = 2;
                     do_win();
                  }
               }
               else if (my_color==1) {
                  if (data==2) {
                     rf_tx(packet->source_addr, 'c', 0);
                     my_color = 0;
                     do_win();
                  }               
               }
               else {
                  if (data==0) {
                     rf_tx(packet->source_addr, 'c', 0);
                     my_color = 1;
                     do_win();
                  }               
               }
            }
      }
      if (type=='c') {
         if (my_color==0) {
            my_color = 1;
            do_lose();
         }
         else if (my_color==1) {
            my_color = 2;
            do_lose();
         }
         else {
            my_color = 0;
            do_lose();            
         }      
      }
   }
   if (type=='r') {
      state = 30;
      counter = 0;
      my_color = GEM_ADDR%3;
      points = 0;
   }
   if (type=='s') {
      counter = COUNTDOWN_TIME;
   }
}

void show_points()
{
   ANIM_UPDATE(0,0,0);
   anim_flush();
   int i;
   for (i=0;i<points;i++) {
      if (i<16) 
         anim_frame[i%16].r = 5;
      else if (i<32)
         anim_frame[i%16].g = 4;
      else if (i<48)
         anim_frame[i%16].r = 0;
      else if (i<64)
         anim_frame[i%16].b = 4;
      else if (i<80)
         anim_frame[i%16].g = 0;
      else if (i<96)
         anim_frame[i%16].r = 5;
      else
         break;
      anim_flush();
   }
}

void show_fail()
{
   ANIM_UPDATE(0,0,0);
   anim_flush();
   anim_frame[0].r = 5;
   anim_flush();
   anim_frame[1].r = 5; anim_frame[15].r = 5;
   anim_flush();
   anim_frame[2].r = 5; anim_frame[14].r = 5;
   anim_flush();
   anim_frame[3].r = 5; anim_frame[13].r = 5;
   anim_flush();
   anim_frame[4].r = 5; anim_frame[12].r = 5;
   anim_flush();
   anim_frame[5].r = 5; anim_frame[11].r = 5; anim_frame[0].r = 0;
   anim_flush();
   anim_frame[6].r = 5; anim_frame[10].r = 5; anim_frame[1].r = 0; anim_frame[15].r = 0;
   anim_flush();
   anim_frame[7].r = 5; anim_frame[9].r = 5; anim_frame[2].r = 0; anim_frame[14].r = 0;
   anim_flush();
   anim_frame[8].r = 5; anim_frame[3].r = 0; anim_frame[13].r = 0;
   anim_flush();
   anim_frame[4].r = 0; anim_frame[12].r = 0;
   anim_flush();
   anim_frame[5].r = 0; anim_frame[11].r = 0;
   anim_flush();
   anim_frame[6].r = 0; anim_frame[10].r = 0;
   anim_flush();
   anim_frame[7].r = 0; anim_frame[9].r = 0;
   anim_flush();
   anim_frame[8].r = 0;
   anim_flush();
}

void update_color()
{
   ANIM_UPDATE(my_color==0? 5:0, my_color==1? 4:0, my_color==2? 4:0);
   anim_flush();
}

void fw_main()
{
   system_init();
   usart_init();
   btns_init(btn_handler);
   leds_init();
   rf_init(1337, GEM_ADDR, rf_rx_handler);
      
   // system_disable_jtag();
   
   // OCR0A = 160;
   // TIMSK0_struct.toie0 = 1;
   // TCCR0B_struct.cs0 = 0x05; // clk/1024
   
   TCCR1A_struct.wgm1 = 0;
   TCCR1B_struct.wgm1 = 1;
   OCR1AH = (6400 >> 8);
   OCR1AL = (6400 & 0xFF);
   TCCR1B_struct.cs1 = 0x05;
   TIMSK1_struct.ocie1a = 1;
   
   TCCR3A_struct.wgm3 = 0;
   TCCR3B_struct.wgm3 = 1;
   OCR3AH = (16000 >> 8);
   OCR3AL = (16000 & 0xFF);
   TCCR3B_struct.cs3 = 0x05;
   TIMSK3_struct.ocie3a = 1;

   
   TCCR4A_struct.wgm4 = 0;
   TCCR4B_struct.wgm4 = 1;
   OCR4AH = (16 >> 8);
   OCR4AL = (16 & 0xFF);
   TIMSK4_struct.ocie4a = 1;
   
   system_enable_int();
   
   anim_init();
   
   DDRD |= (1<<3);
   
   int i;
   for (i=0;i<16;i++)
      range_table[i] = 0;
   
   points = 0;
   state = 0;
   counter = 0;
   donald = 0;
   my_color = GEM_ADDR%3;
   update_color();
      
   print("Hello World\n");
   
   
   while(1) {
      if (state == 0 && donald==1) {
         update_color();
         donald = 0;
      }
      
      if (state==31) {
         ANIM_UPDATE(3, 2, 0);
         anim_flush();
         state = 32;
      }
      if (state==33) {
         ANIM_UPDATE(0,0,0);
         anim_flush();
         state = 30;
      }
      if (state==10) {
         show_points();
         state = 11;
      }
      if (state==20) {
         show_fail();
         state = 12;
      }
      if (state==1) {
         update_color();
         state = 0;
      }

         
      btns_process();
   }
}