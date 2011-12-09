#include "anim.h"

struct {
   led_t frame[16];
   } anim;
   
led_t *anim_frame;
   
void anim_init()
{
   anim_frame = anim.frame;
   int i;
   for (i=0;i<16;i++) {
      led_t l = {0,0,0};
      anim.frame[i] = l;
   }
}

void anim_rotate_cw()
{
   led_t led15 = anim.frame[15];
   int i;
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

