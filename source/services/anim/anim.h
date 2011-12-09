
#include <drivers/leds.h>

extern led_t *anim_frame;

#define ANIM_UPDATE(R_EXP, G_EXP, B_EXP)  \
{ int led_idx;                            \
   for (led_idx=0;led_idx<16;led_idx++) { \
      anim_frame[led_idx].r = R_EXP;      \
      anim_frame[led_idx].g = G_EXP;      \
      anim_frame[led_idx].b = B_EXP;      \
   }                                      \
}

void anim_init();
void anim_rotate_cw();
void anim_rotate_ccw();
void anim_flush();