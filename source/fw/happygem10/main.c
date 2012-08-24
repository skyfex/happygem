#include <avr/io.h>
#include "drivers/all.h"
#include "drivers/hg01/common.h"
#include "services/anim/anim.h"
#include "services/peers/peers.h"
#include <avr/interrupt.h>
#include <stdlib.h>
#define F_CPU 16000000
#include <util/delay.h>

#include "dna.h"
#include "sex.h"

#define PAN_ID 1337
uint8_t gem_id;

uint8_t mode = 0;
uint8_t brightness = 8;

//start sex testcode
uint8_t test_seq = 0;

void set_genome(gene_t* genome, gene_t* genome_new)
{
   uint8_t i;
   for (i = 0; i < GENOME_SIZE; i++)
   {
      genome[i] = genome_new[i];
   }
}

pattern_gene_t rg = {.type = 5,
                     .stride = 0,
                     .length = PATTERN_GENE_PATTERN_MAX_LENGTH,
                     .leap = 0,
                     .pattern = { 0, 0, 0, 0 },
                     .color[0].c[0] = 255,
                     .color[0].c[1] = 0,
                     .color[0].c[2] = 0,
                     .color[0].c[3] = 255};
pattern_gene_t bg = {.type = 5,
                     .stride = 0,
                     .length = PATTERN_GENE_PATTERN_MAX_LENGTH,
                     .leap = 0,
                     .pattern = { 0, 0, 0, 0 },
                     .color[0].c[0] = 0,
                     .color[0].c[1] = 0,
                     .color[0].c[2] = 150,
                     .color[0].c[3] = 255};
pattern_gene_t sg = {.type = 5,
                     .stride = 0,
                     .length = PATTERN_GENE_PATTERN_MAX_LENGTH,
                     .leap = 0,
                     .pattern = { 0, 0, 0, 0 },
                     .color[0].c[0] = 130,
                     .color[0].c[1] = 0,
                     .color[0].c[2] = 0,
                     .color[0].c[3] = 255}; 
//end sex testcode



void btn_handler(uint8_t btn_id)
{
   if (btn_id==1) {
      print("Btn 1\n"); 
      mode = 0;

//start sex testcode
      switch (test_seq)
      {
      pattern_gene_t* pg;
      case 0:
         genome_old_store();
         dna_init();

         test_seq++;
         break;
      case 1:
         crossover_crude(get_genome(), get_genome_old());
         // if (crossover_crude_is_visible((pattern_gene_t*)get_genome()))
         //    set_genome(get_genome(), (gene_t*)&bg);
         // else
         //    set_genome(get_genome(), (gene_t*)&rg);

         test_seq = 0;
         break;
      case 2:
         
         pg = (pattern_gene_t*)get_genome();
         uint8_t i;
         for (i = 0; i < PATTERN_GENE_PATTERN_MAX_LENGTH; ++i)
         {
            if (i >= pg->length)
               pg->pattern[i] = -1;
            else {
               if (pg->pattern[i] == -1)
                  sg.pattern[i] = -1;
               else
                  sg.pattern[i] = 0;
            }
         }
         set_genome(get_genome(), (gene_t*)&sg);

         test_seq = 0;
         break;
      default:
         break;
      }
//end sex testcode
//start old
      // dna_init();
//end old
   }
   if (btn_id==2) {
      print("Btn 2\n"); 
      if (brightness==0)
         brightness = 8;
      else
         brightness--;
      leds_set_brightness(brightness);

      // mode = 1;
   }
   if(btn_id==3){
      print("Btn 3\n"); 
      mode = 2;
   }
   if(btn_id==4){
      print("Btn 4\n");
      mode = 3;
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


void draw_rainbow(pix_t *frame)
{
   uint16_t i;
   {
      pix_t from = (pix_t){{255,0,0}};
      pix_t to = (pix_t){{0,0,255}};
      for (i=0;i<5;i++) {
         frame[i].r = (uint16_t)from.r*(5-i)/5 + (uint16_t)to.r*i/5;
         frame[i].g = (uint16_t)from.g*(5-i)/5 + (uint16_t)to.g*i/5;
         frame[i].b = (uint16_t)from.b*(5-i)/5 + (uint16_t)to.b*i/5;
      }
   }
   {
      pix_t from = (pix_t){{0,0,255}};
      pix_t to = (pix_t){{0,255,0}};
      for (i=0;i<5;i++) {
         frame[5+i].r = (uint16_t)from.r*(5-i)/5 + (uint16_t)to.r*i/5;
         frame[5+i].g = (uint16_t)from.g*(5-i)/5 + (uint16_t)to.g*i/5;
         frame[5+i].b = (uint16_t)from.b*(5-i)/5 + (uint16_t)to.b*i/5;
      }
   }
   {
      pix_t from = (pix_t){{0,255,0}};
      pix_t to = (pix_t){{255,0,0}};
      for (i=0;i<6;i++) {
         frame[10+i].r = (uint16_t)from.r*(6-i)/6 + (uint16_t)to.r*i/6;
         frame[10+i].g = (uint16_t)from.g*(6-i)/6 + (uint16_t)to.g*i/6;
         frame[10+i].b = (uint16_t)from.b*(6-i)/6 + (uint16_t)to.b*i/6;
      }
   }
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

   // system_srand();
   // srand(100);

   // -- Services --
   anim_init();

   // Enable interrupts
   system_enable_int();

   // Hello World Message
   print("((( HappyGem #"); print_uchar(gem_id); print(" )))\n");

   ANIM_UPDATE(0, 0, 0);
   anim_flush();

   uint8_t rot;

   pix_t frame[16];

   dna_init();

   #define HUG_RANGE 80

 		
   while(1) {

      if (tick192) {

         if (tick64) {





            if (mode==0) {


               rf_packet_t *packet;
               uint8_t addr_out;

               if (rf_handle('h', &packet)) {
                  mode = 4; rot = 0; 
                  peers_reset();  
                  rf_clear_all();      
               }
               else if (peers_find_hug(&addr_out, HUG_RANGE, 0)) {

                  uint8_t buffer[1];
                  rf_packet_t o_packet = {
                     .req_ack = 1,
                     .dest_addr = addr_out,
                     .length = 1,//sizeof(dna)+1,
                     .data = buffer
                  };
                  buffer[0] = 'h';
                  // memcpy(buffer+1, dna, sizeof(dna));

                  rf_transmit(&o_packet);
                  mode = 4; rot = 0;
                  peers_reset();
               }
               else {
                  if (tick32)
                     dna_anim();
                  peers_broadcast(0);
               }


               anim_flush();
            }
            if (mode==1) {

               anim_flush();
            }
            if (mode==2) {
               draw_rainbow(anim_frame);
               anim_rotate(anim_frame, rot);
               rot+=1;
               anim_flush();
            }
            if (mode==3) {
               ANIM_UPDATE(255,255,255);
               anim_flush();
            }
            if (mode==4) {
               rot++;
               uint8_t i;
               for (i=0;i<16;i++) {
                  anim_frame[i] = (pix_t){{0,anim_sin(rot*2),0,255}};
               }
               if (rot==254) {
                 mode = 0;
                 ANIM_UPDATE(0,0,0); 
               } 
               anim_flush();
            }
         }

         leds_process();
      }
      tick_process();
      btns_process();
   }
}