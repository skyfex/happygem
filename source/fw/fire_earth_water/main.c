#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include <avr/interrupt.h>
// #include <util/delay.h>

#define KILL_RANGE 35
#define GEM_ADDR 2

uint8_t my_color;
void update_color(uint8_t color);

uint8_t range_table[16];

ISR(TIMER1_COMPA_vect)
{
   rf_broadcast('b', my_color);
   // putc('.');
}

ISR(TIMER3_COMPA_vect)
{
   int i;
   // for (i=0;i<16;i++)
   //    anim_frame[i].r = range_table[i]/4;
   // print_uchar(range_table[1]); putc('\n');
   // anim_flush();
   // putc('x');
}

void btn_handler(uint8_t btn_id)
{
   // if(btn_id==3){
   //    print("Btn 3\n");
   //    rf_tx(1, 1, 0);
   //    rf_broadcast(1, 0);
   // }
   // if(btn_id==4){
   //    print("Btn 4\n");
   //    rf_tx(3, 1, 0);
   //    rf_broadcast(1, 0);
   // }
}

void rf_rx_handler(rf_packet_t *packet)
{
   uint8_t type = packet->data[0];
   uint8_t data = packet->data[1];
   
   uint8_t source = (uint8_t)packet->source_addr;
   if (!packet->has_source || source>15)
      return;
   
  if (type=='b') {
         uint8_t ed = packet->ed;
         if (ed>63) ed = 63;
         range_table[source] = (range_table[source]>>1) + (ed>>1);
         if (range_table[source] > KILL_RANGE) {
            if (my_color==0) {
               if (data==1) {
                  rf_tx(packet->source_addr, 'c', 0);
                  update_color(2);
               }
            }
            else if (my_color==1) {
               if (data==2) {
                  rf_tx(packet->source_addr, 'c', 0);
                  update_color(0);
               }               
            }
            else {
               if (data==0) {
                  rf_tx(packet->source_addr, 'c', 0);
                  update_color(1);
               }               
            }
         }
   }
   if (type=='c') {
      if (my_color==0) {
         update_color(1);
      }
      else if (my_color==1) {
         update_color(2);
      }
      else {
         update_color(0);             
      }      
   }
   
   // print("Length:"); print_uchar(packet->length); putc('\n');
   // print("ED:"); print_uchar(packet->ed); putc('\n');
   // print("Seq:"); print_uchar(packet->seq); putc('\n');
   // if (packet->has_source) {
   //    print("Source Addr:"); print_ushort(packet->source_addr); putc('\n'); 
   // }
   // if (packet->has_dest) {
   //    print("Dest Addr:"); print_ushort(packet->dest_addr); putc('\n'); 
   // }
   // print_uchar(packet->data[0]); putc('\n');
   // print_uchar(packet->data[1]); putc('\n');
   // putc('\n');
}

void update_color(uint8_t color)
{
   my_color = color;
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
   OCR1AH = (3200 >> 8);
   OCR1AL = (3200 & 0xFF);
   TCCR1B_struct.cs1 = 0x05;
   TIMSK1_struct.ocie1a = 1;
   
   TCCR3A_struct.wgm3 = 0;
   TCCR3B_struct.wgm3 = 1;
   OCR3AH = (16000 >> 8);
   OCR3AL = (16000 & 0xFF);
   TCCR3B_struct.cs3 = 0x05;
   TIMSK3_struct.ocie3a = 1;

   
   
   system_enable_int();
   
   anim_init();
   
   int i;
   for (i=0;i<16;i++)
      range_table[i] = 0;
   
   update_color(GEM_ADDR%3);
   print("Hello World\n");
   
   
   while(1) {
      btns_process();
   }
}