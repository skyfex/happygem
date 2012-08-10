#ifndef SERVICES_ANIM_H
#define SERVICES_ANIM_H

#include <drivers/leds.h>

typedef 
	union {
		struct {
		   uint8_t r;
		   uint8_t g;
		   uint8_t b;
		   uint8_t a;
		};
		uint8_t c[4];
   } pix_t;

extern pix_t *anim_frame;

#define ANIM_UPDATE(R_EXP, G_EXP, B_EXP)  \
{ int led_idx;                            \
   for (led_idx=0;led_idx<16;led_idx++) { \
      anim_frame[led_idx].r = R_EXP;      \
      anim_frame[led_idx].g = G_EXP;      \
      anim_frame[led_idx].b = B_EXP;      \
   }                                      \
}

void anim_init(void);

pix_t *anim_alloc(void);
void anim_clear(pix_t *frame);
void anim_free(pix_t *frame);

void anim_rotate(pix_t *frame, uint8_t angle);

void anim_rotate_cw(void);
void anim_rotate_ccw(void);

void anim_comp_over(pix_t *dest, pix_t *source);

void anim_mirror_vert(pix_t *target);
void anim_mirror_hori(pix_t *target);
void anim_mirror_both(pix_t *target);

void anim_flush(void);

uint8_t anim_sin(uint16_t tick);

#endif