#ifndef SERVICES_ANIM_H
#define SERVICES_ANIM_H

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

void anim_init(void);

led_t *anim_alloc(void);
void anim_free(led_t *frame);

void anim_rotate(led_t *frame, char angle);

void anim_rotate_cw(void);
void anim_rotate_ccw(void);

void anim_flush(void);

#endif