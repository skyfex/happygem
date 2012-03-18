/*
 * anims.c
 *
 * Created: 21.01.2012 19:51:58
 *  Author: fredrik
 */ 


#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"
#include <avr/interrupt.h>

#include "drivers/hg01/common.h"
#include <util/delay.h>

uint8_t c = 0;


uint8_t anim_sin(uint16_t tick)
{
	// Python: [int(round(cos(pi*(x/30.0)+pi)*8+8)) for x in range(0,60)]
	uint8_t table[60] = {
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 6, 7, 8, 9, 
		10, 10, 11, 12, 13, 13, 14, 14, 15, 15, 16, 16, 16,
		 16, 16, 16, 16, 15, 15, 14, 14, 13, 13, 12, 11, 10, 
		 10, 9, 8, 7, 6, 6, 5, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0
	};		
	return table[tick%60];
}

void win_anim(uint8_t tick)
{
	uint8_t r = anim_sin(tick*2)/2;
	ANIM_UPDATE(r,r,r);

}
void loose_anim(uint8_t tick)
{
	uint8_t r = anim_sin(tick*2)/2;
	ANIM_UPDATE(r,0,0);

}
void stage0(uint8_t tick)
{
	ANIM_UPDATE(0,7,0);

}


void stage2(uint8_t tick)
{
	uint8_t led_idx;
	for (led_idx=0;led_idx<16;led_idx++)
	{
		uint8_t x = (led_idx+tick)%4;
		if (x==0)
			anim_frame[led_idx] = (led_t){{0, 0, 0}};
		if (x==1)							    
			anim_frame[led_idx] = (led_t){{0, 1, 2}};
		if (x==2)							    
			anim_frame[led_idx] = (led_t){{7, 0, 5}};
		if (x==3)							    
			anim_frame[led_idx] = (led_t){{0, 1, 2}};
	}

}

void stage3(uint8_t tick)
{
	uint8_t d = tick%16;
	if (d >= 8) d = 16-d;
	uint8_t led_idx;
	for (led_idx=0;led_idx<16;led_idx++)
	{
		uint8_t x = (led_idx+d)%4;
		if (x==0)
			anim_frame[led_idx] = (led_t){0, 0, 0};
		if (x==1)						  
			anim_frame[led_idx] = (led_t){2, 0, 1};
		if (x==2)						  
			anim_frame[led_idx] = (led_t){7, 5, 0};
		if (x==3)						  
			anim_frame[led_idx] = (led_t){2, 0, 1};
	}

}

void stage4(uint8_t tick)
{
	char d = tick%16;
	if (d >= 14)
		d = -8+d;
	else if (d >= 12)
		d = 20-d;
	else if (d >= 10)
		d = -4+d;
	else if (d >= 8)
		d = 16-d;
	else
		d = d;
	uint8_t led_idx;
	for (led_idx=0;led_idx<16;led_idx++)
	{
		uint8_t x = (led_idx+d)%4;
		if (x==0)
			anim_frame[led_idx] = (led_t){0, 0, 0};
		if (x==1)
			anim_frame[led_idx] = (led_t){3, 2, 0};
		if (x==2)
			anim_frame[led_idx] = (led_t){5, 5, 5};
		if (x==3)
			anim_frame[led_idx] = (led_t){3, 2, 0};
	}

}

void stage1(uint8_t tick)
{
   uint8_t i;

   uint8_t a[7] = {0,1,2,3,4,5,6,7};
   uint8_t b[7] = {0,1,1,2,2,3,4,5};
   
   
   
   uint8_t t = (tick>6)?6:tick;
   
	for (i=12;i<16;i++) 
		anim_frame[i] = (led_t){0,a[t],0};
	for (i=8;i<12;i++)
		anim_frame[i] = (led_t){0,b[t],a[t]};
	for (i=4;i<8;i++)
		anim_frame[i] = (led_t){a[t],0,b[t]};
	for (i=0;i<4;i++) 
		anim_frame[i] = (led_t){a[t],b[t],0};

}


void stage5(int16_t tick)
{
   static int16_t mult;
   uint8_t i;
   
   // for (i=12;i<16;i++) 
   //    anim_frame[i] = (led_t){0,5,0};
   // for (i=8;i<12;i++)
   //    anim_frame[i] = (led_t){0,3,5};
   // for (i=4;i<8;i++)
   //    anim_frame[i] = (led_t){5,0,3};
   // for (i=0;i<4;i++) 
   //    anim_frame[i] = (led_t){5,3,0};
   // 
   
   
	for (i=12;i<16;i++) 
		anim_frame[i] = (led_t){0,7,0};
	for (i=8;i<12;i++)
		anim_frame[i] = (led_t){0,5,6};
	for (i=4;i<8;i++)
		anim_frame[i] = (led_t){7,0,5};
	for (i=0;i<4;i++) 
		anim_frame[i] = (led_t){7,5,0};
   		
   		
	//if (mult<4) {
		//if (tick%30==0)
			//mult++;
	//}	
   // mult-=tick/20; 
			
   anim_rotate(anim_frame, tick);//(tick*4)%256);			
}