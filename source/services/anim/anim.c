#include "anim.h"
#include <drivers/system.h>
#include <stdlib.h>

struct {
   led_t frame[16];
   led_t frame1[16];
   led_t frame2[16];
   } anim;
   
led_t *anim_frame;
   
void anim_init()
{
   anim_frame = anim.frame;
   uint8_t i;
   for (i=0;i<16;i++) {
      led_t l = {0,0,0};
      anim.frame[i] = l;
   }
}


led_t *anim_alloc(void)
{
	return malloc(16*sizeof(led_t));
}
void anim_free(led_t *frame)
{
	free(frame);
}

void anim_rotate(led_t *frame, char angle)
{
	//angle = (angle%(16*4));
	//if (angle < 0) angle = 16*3 + angle;
	char angle_l = angle/16;
	char angle_h = angle/16+1;
	char angle_d = angle%16;
	
	//print_uchar(angle_l); print(" "); print_uchar(angle_d); print("\n");
	
	uint8_t i;
	for (i=0;i<16;i++) {
      anim.frame1[i] = frame[(i+angle_l)%16];
	  anim.frame2[i] = frame[(i+angle_h)%16];
   }
   
   	for (i=0;i<16;i++) {
	  uint8_t j;
	  for (j=0;j<3;j++) {
		  
		 char l = anim.frame1[i].c[j]*(16-angle_d);
		 char h = anim.frame2[i].c[j]*angle_d;
		frame[i].c[j] = (l+h)/16; //anim.frame2[i].c[j];
	  }		
   }
}

void anim_rotate_cw()
{
   led_t led15 = anim.frame[15];
   uint8_t i;
   for(i=15;i>0;i--) {
      anim.frame[i] = anim.frame[i-1];
   }
   anim.frame[0] = led15;
}

void anim_rotate_ccw()
{
   led_t led0 = anim.frame[0];
   int i;
   for(i=0;i<15;i++) {
      anim.frame[i] = anim.frame[i+1];
   }
   anim.frame[15] = led0;
}

void anim_flush()
{
	leds_set_all(anim.frame);
}

