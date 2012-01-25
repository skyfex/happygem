#ifndef DRIVERS_LEDS_H
#define DRIVERS_LEDS_H

#include <include/types.h>

typedef struct {
	union {
		struct {
		   uint8_t r;
		   uint8_t g;
		   uint8_t b;
		};
		uint8_t c[3];
	};		   
   } led_t;
   

void leds_set(uint8_t led_idx, uint8_t r, uint8_t g, uint8_t b);
void leds_set_all(led_t framebuffer[16]);
void leds_clear();
void leds_on(void);
void leds_off(void);
void leds_init(void);
void leds_process(void);

#endif