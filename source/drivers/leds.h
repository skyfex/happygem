#ifndef DRIVERS_LEDS_H
#define DRIVERS_LEDS_H

#include <include/types.h>

typedef 
	union {
		struct {
		   uint16_t r;
		   uint16_t g;
		   uint16_t b;
		};
		uint16_t c[3];
   } led_t;
   
#define RGB(R,G,B) ((led_t){{R,G,B}})

void leds_set(uint8_t led_idx, uint16_t r, uint16_t g, uint16_t b);
void leds_set_all(led_t framebuffer[16]);
void leds_clear();
void leds_on(void);
void leds_off(void);
void leds_init(void);
void leds_set_brightness(uint8_t bright);
uint8_t leds_get_brightness();
void leds_process(void);

#endif