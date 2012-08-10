#include <avr/io.h>
#include "drivers/all.h"
#include "services/anim/anim.h"

#include "dna.h"

void type0_init(genome_t *gene, uint8_t *state)
{
	uint8_t *pos = &state[0];
	*pos = (gene->startpos%4)<<2;
}
void type0(genome_t *gene, uint8_t *state)
{
	uint8_t *pos = &state[0];
	int8_t speed = gene->speed%3;
	int8_t speedx;
	if (speed==0) 	   speedx = 12;
	else if (speed==1) speedx = 24;
	else if (speed==2) speedx = 48;
	else speedx = 192;

	led_t led = (led_t){{gene->r%10, gene->g%10, gene->b%10}};

	int8_t size = gene->size%3;
	anim_frame[*pos] = led;
	if (size>=1)
		anim_frame[(*pos+1)%16] = led;
	if (size>=2)
		anim_frame[(*pos+2)%16] = led;



	if (tick%speedx==0) {
		if (gene->speed >= 0)
			*pos += 1;
		else
			*pos -= 1;
	}
	*pos = *pos%16;

}
void type1_init(genome_t *gene, uint8_t *state)
{
	uint8_t *pos = &state[0];
	uint8_t *on = &state[1];

	*pos = (gene->startpos%4)*4;
	*on = 1;
}
void type1(genome_t *gene, uint8_t *state)
{
	uint8_t *pos = &state[0];
	uint8_t *on = &state[1];

	int8_t size = gene->size%3;
	int8_t speed = gene->speed%3;

	int8_t speedx;
	if (speed==0) 	   speedx = 24;
	else if (speed==1) speedx = 48;
	else if (speed==2) speedx = 96;
	else speedx = 192;

	led_t led = (led_t){{gene->r%10, gene->g%10, gene->b%10}};

	if (*on) {
		anim_frame[*pos] = led;
		anim_frame[(*pos+8)%16] = led;
		if (size>=1) {
			anim_frame[(*pos+1)%16] = led;
			anim_frame[(*pos+9)%16] = led;
		}
		if (size>=2) {
			anim_frame[(*pos-1)%16] = led;
			anim_frame[(*pos+7)%16] = led;
		}
	}


	if (tick%speedx==0) {
		*on = !*on;
	}
}

void type2_init(genome_t *gene, uint8_t *state)
{
	uint8_t *pos = &state[0];
	*pos = (gene->startpos%4)<<2;
}
void type2(genome_t *gene, uint8_t *state)
{
	uint8_t *pos = &state[0];
	anim_frame[*pos] = (led_t){{gene->r%10, gene->g%10, gene->b%10}};

}