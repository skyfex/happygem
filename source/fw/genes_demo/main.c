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
      rf_broadcast('x', 0);

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

#define red (led_t){{15,0,0}}
#define red2 (led_t){{2,0,0}}
#define orange  (led_t){{15,8,0}}
#define orange2  (led_t){{8,4,0}}
#define turk  (led_t){{0,12,8}}
#define turk2  (led_t){{0,6,3}}

void anim1()
{
   static uint8_t t = 0;

   if (tick60) {

      led_t frame[16];

      char i;
      for (i=0;i<16;i++) {
         frame[i] = (led_t){{0,0,anim_sin(tick/3)/2}};
      }
      frame[0] = red;
      frame[8] = red;

      anim_rotate(frame, t);

      anim_flush_frame(frame);

      t++;
   }
}



void anim2()
{
   static uint8_t t = 0;



   if (tick5) {



      led_t frame[16];
      uint8_t t2 = t;
      if (t2>=8) t2 = 16-t2;

      char i;
      for (i=0;i<16;i++) {
         frame[i] = (led_t){{0,0,0}};
      }
      frame[t2] = orange;
      if (t2!=0)
         frame[16-t2] = orange;

      if (t2==0) {
         frame[15] = orange2;
         frame[1] = orange2;
      }
      if (t2==8) {
         frame[7] = orange2;
         frame[9] = orange2;
      }

      if (t2==0 || t2==1 || t2==7 || t2==8) {
         frame[3] = turk;
         frame[4] = turk;
         frame[5] = turk;
         frame[3+8] = turk;
         frame[4+8] = turk;
         frame[5+8] = turk;
      }
      if (t2==3 || t2==4 || t2==5) {
         frame[0] = turk;
         frame[8] = turk;
      }

      anim_flush_frame(frame);

      t++;
      if (t==16) t = 0;
   }
}

void anim3() 
{
   static uint8_t t = 0;

   if (tick60) {

      uint8_t t2 = t;
      if (t2>=8) t2 = 16-t2;

      led_t frame[16];

      char i;
      for (i=0;i<16;i++) {
         uint8_t x = anim_sin(tick/3);
         if (x>12) x = 12;
         frame[i] = (led_t){{0,x,x>8?8:x}};
      }


    if (t2==0 || t2==1 || t2==7 || t2==8) {
         frame[3] = orange;
         frame[4] = orange;
         frame[5] = orange;
         frame[3+8] = orange;
         frame[4+8] = orange;
         frame[5+8] = orange;
      }
      if (t2==3 || t2==4 || t2==5) {
         frame[0] = orange;
         frame[8] = orange;
      }

      anim_flush_frame(frame);

      if (tick5) {
         t++;
         if (t==16) t = 0;
      }
   }
}

void anim4()
{
   static uint8_t t = 0;


  if (tick60) {

      uint8_t t2 = t;
      if (t2>=8) t2 = 16-t2;

      led_t frame[16];

      char i;
      for (i=0;i<16;i++) {
         uint8_t x = anim_sin(tick/3);
         if (x>12) x = 12;
         frame[i] = (led_t){{0,x,x>8?8:x}};
      }

      frame[t2] = red;
      if (t2!=0)
      frame[16-t2] = red;
      if (t2==0) {
         frame[15] = red2;
         frame[0] = red;
         frame[1] = red2;
      }
      if (t2==8) {
         frame[7] = red2;
         frame[8] = red;
         frame[9] = red2;
      }
      if (t2==1) {
         frame[15] = red;
         frame[0] = red2;
         frame[1] = red;
      }
      if (t2==7) {
         frame[7] = red;
         frame[8] = red2;
         frame[9] = red;
      }

      anim_flush_frame(frame);

      if (tick5) {
         t++;
         if (t==16) t = 0;
      }
   }

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

   ANIM_UPDATE(0,0,0);
   anim_flush();


   rf_packet_t *packet;
   		
   uint8_t a = 1;
   uint16_t count = 0;

   while (1) {
      if (tick180) {
         // anim4();
         if (rf_handle('x', &packet)) {
            
            rf_clear_all();
            break;
         }
      }
      btns_process();
      leds_process();
      tick_process();
   }

   while(1) {

      
      if (tick180) {
         leds_off();

         if (a<3) {
         if (tick30) {
            uint8_t hug_addr;
            if (rf_handle('h', &packet)) {
               a+=2;
               count = 179;
               // peers_reset();
               rf_clear_all();
            }
            else if (peers_find_hug(&hug_addr, HUG_RANGE, 0)) {
               a+=2;
               count = 179;
               rf_tx(hug_addr, 'h', 0, true);
               peers_reset();
               rf_clear_all();

            } 
            else {
               rf_broadcast('p', 0);
            }
         }
         }
         if (count==0) {

            switch(a) {
               case 1: anim1(); break;
               case 2: anim2(); break;
               case 3: anim3(); break;
               case 4: anim4(); break;
               default: 
                  ANIM_UPDATE(1,0,0);
                  anim_flush();
                  break;
            }
         }
         else {
            ANIM_UPDATE((180-count)/12,(180-count)/12,(180-count)/12);
            anim_flush();
            count--;
         }

         leds_on();
      }
      tick_process();
      btns_process();
      leds_process();
   }
}