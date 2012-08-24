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

void anim_random_explosion(pix_t* frame, pix_t* dna_frame, uint16_t* frame_counter, bool* anim_continue)
{
   uint16_t max_frames = 80;

   uint8_t i;
   for (i = 0; i<16; i++)
   {
      frame[i].r = (rand()%255 * (max_frames - *frame_counter) + dna_frame[i].r * (*frame_counter)) / max_frames;
      frame[i].g = (rand()%255 * (max_frames - *frame_counter) + dna_frame[i].g * (*frame_counter)) / max_frames;
      frame[i].b = (rand()%255 * (max_frames - *frame_counter) + dna_frame[i].b * (*frame_counter)) / max_frames;
      frame[i].a = 255;
   }

   *frame_counter += 1;

   if (*frame_counter > max_frames)
      *anim_continue = false;
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
print ','.join([str(int(round(max_r*((x/255.0)**(gamma))))) for x in range(0,256)]);
print ','.join([str(int(round(max_g*((x/255.0)**(gamma))))) for x in range(0,256)]);
print ','.join([str(int(round(max_b*((x/255.0)**(gamma))))) for x in range(0,256)]);

*/
   uint16_t lut_r[256] = {
0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,3,3,4,4,4,5,6,6,7,7,8,9,10,11,11,12,13,14,15,16,18,19,20,21,23,24,25,27,28,30,31,33,35,37,38,40,42,44,46,48,51,53,55,57,60,62,65,67,70,72,75,78,81,84,87,90,93,96,99,103,106,109,113,116,120,124,128,131,135,139,143,147,152,156,160,165,169,173,178,183,188,192,197,202,207,212,218,223,228,234,239,245,250,256,262,268,274,280,286,292,298,305,311,318,324,331,338,345,351,358,366,373,380,387,395,402,410,418,425,433,441,449,457,466,474,482,491,499,508,517,526,534,544,553,562,571,580,590,600,609,619,629,639,649,659,669,679,690,700,711,722,732,743,754,765,776,788,799,811,822,834,845,857,869,881,894,906,918,931,943,956,969,981,994,1007,1021,1034,1047,1061,1074,1088,1102,1116,1130,1144,1158,1172,1187,1201,1216,1231,1246,1260,1276,1291,1306,1321,1337,1352,1368,1384,1400,1416,1432,1448,1465,1481,1498,1514,1531,1548,1565,1582,1600,1617,1634,1652,1670,1688,1705,1724,1742,1760,1778,1797,1815,1834,1853,1872,1891,1910,1930,1949,1969,1988,2008,2028,2048
   };
   uint16_t lut_g[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,5,5,6,6,7,7,8,8,9,9,10,11,11,12,13,13,14,15,16,17,17,18,19,20,21,22,23,24,25,26,27,29,30,31,32,34,35,36,38,39,40,42,43,45,46,48,50,51,53,55,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,87,89,91,94,96,99,101,104,106,109,111,114,117,120,122,125,128,131,134,137,140,143,146,149,152,156,159,162,165,169,172,176,179,183,186,190,194,197,201,205,209,213,217,221,225,229,233,237,241,245,250,254,258,263,267,272,276,281,286,290,295,300,305,309,314,319,324,329,335,340,345,350,355,361,366,372,377,383,388,394,400,405,411,417,423,429,435,441,447,453,459,465,472,478,484,491,497,504,510,517,524,530,537,544,551,558,565,572,579,586,593,601,608,615,623,630,638,645,653,661,668,676,684,692,700,708,716,724,732,741,749,757,766,774,783,791,800,808,817,826,835,844,853,862,871,880,889,898,908,917,927,936,946,955,965,975,984,994,1004,1014,1024
   };
   uint16_t lut_b[256] = {
0,0,0,0,0,0,0,1,1,1,1,2,2,2,3,3,4,5,5,6,7,8,9,10,11,12,14,15,16,18,19,21,23,25,27,29,31,33,35,37,40,42,45,48,51,54,57,60,63,66,70,73,77,81,85,88,93,97,101,105,110,115,119,124,129,134,140,145,150,156,162,168,173,180,186,192,199,205,212,219,226,233,240,248,255,263,270,278,286,295,303,312,320,329,338,347,356,365,375,385,394,404,414,425,435,446,456,467,478,489,500,512,524,535,547,559,572,584,596,609,622,635,648,662,675,689,703,717,731,745,760,775,789,805,820,835,851,866,882,898,915,931,948,964,981,998,1016,1033,1051,1069,1087,1105,1123,1142,1161,1180,1199,1218,1238,1257,1277,1297,1317,1338,1358,1379,1400,1421,1443,1464,1486,1508,1530,1552,1575,1598,1621,1644,1667,1691,1714,1738,1762,1787,1811,1836,1861,1886,1911,1937,1962,1988,2014,2041,2067,2094,2121,2148,2176,2203,2231,2259,2287,2315,2344,2373,2402,2431,2461,2490,2520,2550,2581,2611,2642,2673,2704,2736,2767,2799,2831,2863,2896,2929,2962,2995,3028,3062,3096,3130,3164,3198,3233,3268,3303,3339,3374,3410,3446,3483,3519,3556,3593,3630,3668,3705,3743,3781,3820,3858,3897,3936,3976,4015,4055,4095
   };

   uint8_t i; 
   for (i=0;i<16;i++) {
      output[i].r = lut_r[input[i].r];
      output[i].g = lut_g[input[i].g];
      output[i].b = lut_b[input[i].b];
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
