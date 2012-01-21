#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include <avr/interrupt.h>

#include "drivers/hg01/common.h"
#include <util/delay.h>

#define KILL_RANGE 35
#define GEM_ADDR 0

uint8_t range_table[16];
uint8_t points = 3;

int x = 0;

ISR(TIMER1_COMPA_vect)
{
	leds_off();
   rf_broadcast('b', 0);
   leds_on();
}

ISR(TIMER3_COMPA_vect)
{

}

void btn_handler(uint8_t btn_id)
{

}

void rf_rx_handler(rf_packet_t *packet)
{
	leds_off();
   uint8_t type = packet->data[0];
   uint8_t data = packet->data[1];
   
   uint8_t source = (uint8_t)packet->source_addr;
   if (!packet->has_source || source>15)
	  return;
   
  //if (state==0) {
	 if (type=='b') {
			uint8_t ed = packet->ed;
			if (ed>63) ed = 63;

			range_table[source] = (range_table[source]>>1) + (ed>>1);
			if (range_table[source] > KILL_RANGE) {
				
			}
	  }
	  
  // }
  leds_on();
}

void tri_pattern_green()
{
	ANIM_UPDATE(0,0,0);
	
	if (points >=3)
	{
		anim_frame[0].g = 10;
		anim_frame[1].g = 7;
		anim_frame[2].g = 3;
		anim_frame[3].g = 1;
		anim_frame[4].g = 0;
	}

	if (points >=2)
	{
		anim_frame[5].g = 10;
		anim_frame[6].g = 7;
		anim_frame[7].g = 3;
		anim_frame[8].g = 1;
		anim_frame[9].g = 0;
	}
		
	if (points >=1)
	{
		anim_frame[10].g = 10;
		anim_frame[11].g = 7;
		anim_frame[12].g = 3;
		anim_frame[13].g = 1;
		anim_frame[14].g = 0;
	}	
	//anim_frame[15].r =  (led_t){.r = 7, .g = 7, .b = 7};
}

void tri_pattern_red()
{
	ANIM_UPDATE(0,0,0);
	
	if (points >=3)
	{
	anim_frame[0].r = 10;
	anim_frame[1].r = 7;
	anim_frame[2].r = 3;
	anim_frame[3].r = 1;
	anim_frame[4].r = 0;
	}	
	if (points >=2)
	{
	anim_frame[5].r = 10;
	anim_frame[6].r = 7;
	anim_frame[7].r = 3;
	anim_frame[8].r = 1;
	anim_frame[9].r = 0;
	anim_frame[10].r = 10;
	}	
	if (points >=1)
	{
	anim_frame[11].r = 7;
	anim_frame[12].r = 3;
	anim_frame[13].r = 1;
	anim_frame[14].r = 0;
	}	
	//anim_frame[15].r =  (led_t){.r = 7, .g = 7, .b = 7};
}

void show_signal_strength()
{
	int a = range_table[(GEM_ADDR==0)?1:0]%16;
	int b = range_table[(GEM_ADDR==0)?1:0]/16;
	
	ANIM_UPDATE(led_idx<b?5:0, led_idx<a?5:0, 0);
	anim_flush();
}

void fw_main()
{
   system_init();
   usart_init();
   btns_init(btn_handler);
   leds_init();
   rf_init(1337, GEM_ADDR, rf_rx_handler);
	  
   TCCR1A_struct.wgm1 = 0;
   TCCR1B_struct.wgm1 = 1;
   OCR1AH = (6400 >> 8);
   OCR1AL = (6400 & 0xFF);
   TCCR1B_struct.cs1 = 0x05;
   TIMSK1_struct.ocie1a = 1;
   
   //TCCR3A_struct.wgm3 = 0;
   //TCCR3B_struct.wgm3 = 1;
   //OCR3AH = (16000 >> 8);
   //OCR3AL = (16000 & 0xFF);
   //TCCR3B_struct.cs3 = 0x05;
   //TIMSK3_struct.ocie3a = 1;
//
   //
   //TCCR4A_struct.wgm4 = 0;
   //TCCR4B_struct.wgm4 = 1;
   //OCR4AH = (16 >> 8);
   //OCR4AL = (16 & 0xFF);
   //TIMSK4_struct.ocie4a = 1;
   
   system_enable_int();
   
   anim_init();
   
   DDRD |= (1<<3);
   
   int i;
   for (i=0;i<16;i++)
	  range_table[i] = 0;
  
  //tri_pattern_green();
  //anim_flush();
   
   
   
   while(1) {
		show_signal_strength();
		_delay_ms(20);
		//anim_rotate_ccw();
		//anim_flush();
		//_delay_us(1500);
		
   }
}