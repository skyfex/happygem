#include "anim.h"
#include <drivers/system.h>
#include <stdlib.h>

struct {
   pix_t frame[16];
   pix_t frame1[16];
   pix_t frame2[16];
   } anim;
   
pix_t *anim_frame;
   
void anim_init()
{
   anim_frame = anim.frame;
   uint8_t i;
   for (i=0;i<16;i++) {
      pix_t l = {{0,0,0,255}};
      anim.frame[i] = l;
   }
}

pix_t *anim_tempframe1()
{
   return anim.frame1;
}
pix_t *anim_tempframe2()
{
   return anim.frame2;
}

pix_t *anim_alloc(void)
{
	return malloc(16*sizeof(pix_t));
}

void anim_clear(pix_t *frame)
{
   uint8_t i,j;
   for (i=0;i<16;i++) {
      for (j=0;j<4;j++) {
         frame[i].c[j] = 0;
      }
   }
}

void anim_free(pix_t *frame)
{
	free(frame);
}

void anim_rotate(pix_t *frame, uint8_t angle)
{
	//angle = (angle%(16*4));
	//if (angle < 0) angle = 16*3 + angle;
	uint8_t angle_l = angle/16;
	uint8_t angle_h = angle/16+1;
	uint16_t angle_d = angle%16;
	
	//print_uchar(angle_l); print(" "); print_uchar(angle_d); print("\n");
	
	uint8_t i;
	for (i=0;i<16;i++) {
      anim.frame1[i] = frame[(i+angle_l)%16];
      anim.frame2[i] = frame[(i+angle_h)%16];
   }
   
   for (i=0;i<16;i++) {
	  uint8_t j;
	  for (j=0;j<3;j++) {
		  
         uint16_t l = anim.frame1[i].c[j];
         uint16_t h = anim.frame2[i].c[j];
         l = l * (16-angle_d);
         h = h * angle_d;
         frame[i].c[j] = (l+h)/16;
	  }		
   }
}

void anim_rotate_cw()
{
   pix_t led15 = anim.frame[15];
   uint8_t i;
   for(i=15;i>0;i--) {
      anim.frame[i] = anim.frame[i-1];
   }
   anim.frame[0] = led15;
}

void anim_rotate_ccw()
{
   pix_t led0 = anim.frame[0];
   uint8_t i;
   for(i=0;i<15;i++) {
      anim.frame[i] = anim.frame[i+1];
   }
   anim.frame[15] = led0;
}

void anim_gamma_correct(led_t *output, pix_t *input)
{
   // TODO: Decrease lookup-table size by storing a quarter of the values and interpolating
/*
Python: 
-------
gamma = 2.5; max_r = 2048; max_g = 1024; max_b = 4095;
print ','.join([str(int(round(max_r*((x/256.0)**(gamma))))) for x in range(0,257,8)]);
print ','.join([str(int(round(max_g*((x/256.0)**(gamma))))) for x in range(0,257,8)]);
print ','.join([str(int(round(max_b*((x/256.0)**(gamma))))) for x in range(0,257,8)]);




*/
   uint16_t lut[3][33] = {
      {
         0,0,2,6,11,20,31,46,64,86,112,142,176,215,259,308,362,421,486,556,632,715,803,897,998,1105,1219,1339,1467,1601,1743,1892,2048
      },
      {
         0,0,1,3,6,10,16,23,32,43,56,71,88,108,130,154,181,211,243,278,316,357,401,448,499,552,609,670,733,801,871,946,1024   
      },
      {
         0,1,4,11,23,40,62,92,128,172,224,284,353,431,518,616,724,842,972,1112,1265,1429,1605,1793,1995,2209,2437,2678,2933,3202,3485,3783,4095
      }
   };
   uint8_t i,j; 
   for (i=0;i<16;i++) {
      for (j=0;j<3;j++) {
         uint8_t in = input[i].c[j];
         uint16_t out1 = lut[j][in/8];
         uint16_t out2 = lut[j][in/8+1];
         uint16_t out = (out1*(7-in%8) + (out2*(in%8)))/8;
         output[i].c[j] = out;
      }
      // output[i].r = lut_r[input[i].r];
      // output[i].g = lut_g[input[i].g];
      // output[i].b = lut_b[input[i].b];
   }
   
}

void anim_comp_over(pix_t *dest, pix_t *source)
{
   // Note: ignores destination alpha
   uint8_t i,j;
   for (i=0;i<16;i++) {
      uint16_t a = source[i].a;
      for (j=0;j<3;j++) {
         uint16_t d = dest[i].c[j];
         uint16_t s = source[i].c[j];
         uint16_t r = (d*(255-a) + s*a)/255;
         dest[i].c[j] = r;
      }
   }
}
void anim_repeat_vert(pix_t *target)
{
   uint8_t i;
   for (i=1;i<8;i++) {
      target[i+8] = target[i];       
   }
}
void anim_mirror_vert(pix_t *target)
{
   uint8_t i;
   for (i=1;i<8;i++) {
      target[8-i] = target[i];       
   }
}
void anim_mirror_hori(pix_t *target)
{
   uint8_t i;
   for (i=1;i<8;i++) {
      target[(i+12)%16] = target[i+4];       
   }
}

void anim_mirror_both(pix_t *target)
{
   target[8] = target[0];
   target[12] = target[4];

   target[5] = target[3];
   target[6] = target[2];
   target[7] = target[1];

   target[11] = target[3];
   target[10] = target[2];
   target[ 9] = target[1];

   target[13] = target[3];
   target[14] = target[2];
   target[15] = target[1];
}

void anim_flush()
{
   led_t output[16];
   anim_gamma_correct(output, anim.frame);
	leds_set_all(output);
}

// void anim_flush_frame(pix_t *frame)
// {
//    leds_set_all(frame);
// }

uint8_t anim_sin(uint16_t t)
{
   
   // Python: [int(round(cos(pi*(x/32.0)+pi)*127.5+127.5)) for x in range(0,64)]
   uint8_t table[64] = {
      0, 1, 2, 5, 10, 15, 21, 29, 37, 47, 57, 67, 79, 90, 103, 
      115, 127, 140, 152, 165, 176, 188, 198, 208, 218, 226, 234, 
      240, 245, 250, 253, 254, 255, 254, 253, 250, 245, 240, 234, 
      226, 218, 208, 198, 188, 176, 165, 152, 140, 128, 115, 103, 
      90, 79, 67, 57, 47, 37, 29, 21, 15, 10, 5, 2, 1
   };
   return table[t%64];
}

// Not used by anything right now
pix_t anim_merge_pix(pix_t *a, pix_t *b, uint8_t fader)
{
   pix_t result;
   result.r = ((uint16_t)a->r * (255-(uint16_t)fader) + (uint16_t)b->r * (uint16_t)fader)/255;
   result.g = ((uint16_t)a->g * (255-(uint16_t)fader) + (uint16_t)b->g * (uint16_t)fader)/255;
   result.b = ((uint16_t)a->b * (255-(uint16_t)fader) + (uint16_t)b->b * (uint16_t)fader)/255;
   return result;
}
