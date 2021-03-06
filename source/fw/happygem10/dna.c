#include "dna.h"
#include "sex.h"

#include "drivers/all.h"
#include "services/anim/anim.h"
#include "sex.h"

#include <stdlib.h>
#include <string.h>

#define DNA_MAGIC 42

static unsigned int dna_eeprom_addr;

uint8_t genome_size;
gene_t genome[GENOME_MAX_SIZE];
static gene_state_t state[GENOME_MAX_SIZE];

static int8_t beat_t;     // Position in beat (0-16)
static int8_t beat_count; // beat count  (0-32)
static int16_t pattern_beat;

pix_t dna_frame[16];
uint8_t dna_tx_buffer[sizeof(genome)+2];
gene_t genome_foreign[GENOME_MAX_SIZE];
uint8_t genome_size_foreign;

void dna_anim_gene(gene_t *g, gene_state_t *s, pix_t *frame);

// ------------

void dna_init(unsigned int eeprom_addr)
{
	dna_eeprom_addr = eeprom_addr;
	if(sizeof(genome) > 80) {
		print_uchar(sizeof(genome)); print("\n");
		print("DNA size too large\n");
	}

	genome_size = GENOME_SIZE;

	dna_reset_beat();
	dna_new_pattern();

	dna_load();
}

void dna_reset_beat()
{
	pattern_beat = 0;
	beat_t = 0;
	beat_count = 0;	
}

void dna_new_pattern()
{


	uint8_t i = 0;
	while (i < genome_size-1) {
		pattern_gene_init((pattern_gene_t*)&genome[i], (pattern_state_t*)&state[i]);
		if (crossover_crude_is_visible((pattern_gene_t*)&genome[i]))
			i++;
	}
	if (rand()%2 == 0)	
		swirl_gene_init((swirl_gene_t*)&genome[genome_size-1], (swirl_state_t*)&state[genome_size-1]);
	else
		wings_gene_init((wings_gene_t*)&genome[genome_size-1], (wings_state_t*)&state[genome_size-1]);		
}

void dna_save()
{
	uint8_t i;
	leds_off();
	eeprom_write(dna_eeprom_addr, DNA_MAGIC); // Magic number
	eeprom_write(dna_eeprom_addr+1, genome_size);
	uint8_t *data = (uint8_t*)genome;
	for (i = 0; i < GENOME_MAX_SIZE*sizeof(gene_t); i++) {
		eeprom_write(dna_eeprom_addr + 2 + i, data[i]);
	}
	leds_on();
}
void dna_load()
{
	uint8_t i;
	uint8_t magic = eeprom_read(dna_eeprom_addr);
	if (magic != DNA_MAGIC) return;
	genome_size = eeprom_read(dna_eeprom_addr+1);
	uint8_t *data = (uint8_t*)genome;
	for (i = 0; i < GENOME_MAX_SIZE*sizeof(gene_t); i++) {
		data[i] = eeprom_read(dna_eeprom_addr + 2 + i);
	}	
}
void dna_delete()
{
	eeprom_write(dna_eeprom_addr, 255);
}


void dna_transmit(uint8_t port, uint16_t addr)
{
	rf_packet_t o_packet = {
	   .req_ack = 1,
	   .dest_addr = addr,
	   .length = sizeof(dna_tx_buffer),
	   .data = dna_tx_buffer
	};
	dna_tx_buffer[0] = port;
	dna_tx_buffer[1] = genome_size;
	memcpy(dna_tx_buffer+2, genome, sizeof(genome));
	rf_transmit(&o_packet);

}

void dna_recieve(rf_packet_t *packet)
{
	uint8_t *data = packet->data;
	genome_size_foreign = data[1];
	memcpy(genome_foreign, &data[2], genome_size_foreign * sizeof(gene_t)); 
	sex(packet->dest_addr, packet->source_addr,
		&genome_size, &genome_size_foreign,
		genome, genome_foreign);
}

int8_t dna_beat_count()
{
	return beat_count;
}
int8_t dna_beat_t()
{
	return beat_t;
}

void dna_anim()
{

	ANIM_UPDATE(0,0,0);
	uint8_t i;
	for (i=0; i<genome_size; i++) {
		anim_clear(dna_frame);
		dna_anim_gene(&genome[i], &state[i], dna_frame);
		anim_comp_over(anim_frame, dna_frame);
	}
	beat_t++;
	if (tick16)
		pattern_beat++;
	if (pattern_beat > 16*32) {
		pattern_beat = 0;
	}
	if (beat_t==16) {
		beat_t = 0;
		beat_count++;
		if (beat_count==32)
			beat_count = 0;
	}
}

#define DNA_ANIM_GENE_HANDLE(X)	\
	case X##_type:				\
		X##_gene((X##_gene_t*)g, (X##_state_t*)s, frame);	\
		break; 	

void dna_anim_gene(gene_t *g, gene_state_t *s, pix_t *frame)
{
	switch(g->type) {
		case 0:
			// reserved for blank gene
			break;

		DNA_ANIM_GENE_HANDLE(debug)
		DNA_ANIM_GENE_HANDLE(wave)
		DNA_ANIM_GENE_HANDLE(wings)
		DNA_ANIM_GENE_HANDLE(dot)
		DNA_ANIM_GENE_HANDLE(pattern)
		DNA_ANIM_GENE_HANDLE(swirl)

		default:
			break;
	}
}

void dna_random_color_full(pix_t *p)
{
	p->r = rand()%255;
	p->g = rand()%255;
	p->b = rand()%255;
	p->a = 255;
}

void dna_random_color_true(pix_t *p)
{
	uint16_t juice = DNA_COLOR_STRENGTH;
	uint16_t color[3];
	bool color_set[3];
	
	color_set[0] = false;
	color_set[1] = false;
	color_set[2] = false;

	uint8_t select;
	uint8_t colors_set = 0;
	while (colors_set < 2)
	{
		select = rand()%3;
		if (color_set[select] == false)
		{
			color_set[select] = true;
			color[select] = rand()%juice;
			juice -= color[select];
			colors_set++;
		}
	}
	if (!color_set[0])
	{
		color[0] = juice;
	}
	if (!color_set[1])
	{
		color[1] = juice;
	}
	if (!color_set[2])
	{
		color[2] = juice;
	}

	p->r = color[0];
	p->g = color[1];
	p->b = color[2];
	p->a = 255;
}

void dna_random_color(pix_t *p)
{
	uint16_t hue = rand()%(256*3);
	uint16_t r,g,b;
	if (hue < 256) {
		r = 256-hue;
		g = hue;
		b = 0;
	}
	else 
	if (hue < 256*2)
	{
		hue -= 256;
		r = 0;
		g = 256-hue;
		b = hue;
	}
	else {
		hue -= 256*2;
		r = hue;
		g = 0;
		b = 256-hue;
	}
	p->r = r;
	p->g = g;
	p->b = b;
	p->a = 255;
}

// -------------


void debug_gene_init(debug_gene_t *g, debug_state_t *s)
{
	g->type = debug_type;
}

void debug_gene(debug_gene_t *g, debug_state_t *s, pix_t* frame)
{
	frame[0].a = 255;
	frame[1].a = 255;
	frame[2].a = 255;
	frame[3].a = 255;
	if (beat_count==0 && beat_t<8)
		frame[0] = (pix_t){{0,255,0,255}};		
	if (beat_count%8==0 && beat_t<8)
		frame[1] = (pix_t){{0,0,255,255}};	
	if (beat_count%4==0 && beat_t<8)
		frame[2] = (pix_t){{255,0,0,255}};		
	if (beat_count%2==0 && beat_t<8) {
		frame[3] = (pix_t){{0,128,128,255}};
	}
	// if (beat_t<8)
	// 	frame[1] = (pix_t){{255,0,0,255}};
}

// -------------


void wave_gene_init(wave_gene_t *g, wave_state_t *s)
{
	g->type = wave_type;
	dna_random_color_full(&g->color);

	g->stride = 1<<(rand()%4+1);
	g->duration = 1<<(rand()%3+1);
	if (g->duration > g->stride)
		g->duration = 2;
	g->offset = 0;
	// if (rand()%2==0)
	// 	g->offset += g->duration;
}

void wave_gene(wave_gene_t *g, wave_state_t *s, pix_t* frame)
{
	int8_t t;
	t = beat_count - g->offset;
	if (t < 0) return;
	t = t % g->stride;
	if (t >= g->duration) return;
	t = t*16 + beat_t;

	if (g->duration == 1)
		t *= 4;
	else if (g->duration == 2)
		t *= 2;
	else if (g->duration == 8)
		t /= 2;

	uint8_t i;
	for (i=0;i<16;i++) {
		frame[i] = g->color;
		frame[i].a = anim_sin(t);			

	}
}


// --------------


void wings_gene_init(wings_gene_t *g, wings_state_t *s)
{
	g->type = wings_type;
	dna_random_color(&g->color);

	g->stride = 1<<(rand()%3+2);
	g->duration = 1<<(rand()%2+2);
	if (g->duration >= g->stride)
		g->duration = g->stride/2;
	// g->offset = 0;
	// if (rand()%2==0)
	// 	g->offset += g->duration;
	g->flip = rand()%2;
}

void wings_gene(wings_gene_t *g, wings_state_t *s, pix_t* frame)
{
	int8_t t;
	t = beat_count - g->offset;
	if (t < 0) return;
	t = t % g->stride;
	if (t >= g->duration) return;
	t = t*16 + beat_t;

	if (g->duration == 2)
		t *= 2;
	else if (g->duration == 8)
		t /= 2;

	if (t>32) t = 64-t;

	uint8_t t1 = t/6;
	uint8_t t2 = t%6;

	uint8_t i;
	for (i=0;i<t1;i++) {
		frame[4-i] = g->color;
		if (i == t1-1)
			frame[4-i].a = t2*32; 
	}
	anim_mirror_both(frame);

	if (g->flip) {
		anim_rotate(frame, 64);
	}
}

// --------------


void dot_gene_init(dot_gene_t *g, dot_state_t *s)
{
	g->type = dot_type;
	dna_random_color_full(&g->color);

	g->stride = 1<<(rand()%4+1);
	g->duration = 1<<(rand()%4);
	if (g->duration > g->stride)
		g->duration = 2;
	g->offset = 0;
	if (rand()%2==0)
		g->offset += g->duration;

	g->pos = rand()%4;
	g->mirror = rand()%4;
}

void dot_gene(dot_gene_t *g, dot_state_t *s, pix_t* frame)
{
	int8_t t;
	t = beat_count - g->offset;
	if (t < 0) return;
	t = t % g->stride;
	if (t >= g->duration) return;
	t = t*16 + beat_t;

	if (g->duration == 1)
		t *= 4;
	else if (g->duration == 2)
		t *= 2;
	else if (g->duration == 8)
		t /= 2;

	pix_t c = g->color;
	c.a = 255-anim_sin(t);

	frame[g->pos] = c;

	if (g->mirror == 0 || g->mirror==3)
		frame[8+g->pos] = c;
	if (g->mirror == 1 || g->mirror==3)
		frame[8-g->pos] = c;
	if (g->mirror == 2 || g->mirror==3)
		if (g->pos!=0)
			frame[16-g->pos] = c;		

}

void pattern_gene_init(pattern_gene_t *g, pattern_state_t *s)
{
	//set general
	g->type = pattern_type;
	g->stride = rand()%PATTERN_GENE_MAX_STRIDE;
	// if (g->stride < PATTERN_GENE_MIN_STRIDE)
	// 	g->stride = PATTERN_GENE_MIN_STRIDE;

	//set leap
	g->leap = rand()%PATTERN_GENE_MAX_LEAP + 1;
	// if (g->leap < PATTERN_GENE_MIN_LEAP)
	// 	g->leap = PATTERN_GENE_MIN_LEAP;

	//set pattern
	g->length = PATTERN_GENE_PATTERN_MAX_LENGTH;

	uint8_t i;
	for (i = 0; i < PATTERN_GENE_PATTERN_MAX_LENGTH; ++i)
	{
		if (rand()%2)
			g->pattern[i] = 1;
		else
			g->pattern[i] = -1;
	}

	//set color
	dna_random_color_true(&g->color);
}

void pattern_gene(pattern_gene_t *g, pattern_state_t *s, pix_t* frame)
{		
		uint8_t beat;
		//adjust beat_count to gene stride
		beat = pattern_beat / (int16_t)(16>>g->stride);

		//adjust beat_count to gene leap
		beat = beat * g->leap;

		//write two-way symmetric patterns
		uint8_t i;
		for (i = 0; i < g->length; ++i)
		{
			if (g->pattern[i] > -1)
			{
				frame[(+ beat + i) % 16] = g->color;
				frame[16 - (+ beat + i) % 16] = g->color;
				frame[(+ beat + i + 8) % 16] = g->color;
				frame[16 - (+ beat + i + 8) % 16] = g->color;
			}
		}
}

// --------------


void swirl_gene_init(swirl_gene_t *g, swirl_state_t *s)
{
	g->type = swirl_type;
	dna_random_color(&g->color);

	g->stride = 1<<(rand()%3+2);
	g->duration = 1<<(rand()%2+2);
	if (g->duration > g->stride)
		g->duration = 2;
	// g->offset = 0;
	// if (rand()%2==0)
	// 	g->offset += g->duration;
	g->direction = (rand()%2)?1:-1;
	g->flip = rand()%2;
}

void swirl_gene(swirl_gene_t *g, swirl_state_t *s, pix_t* frame)
{
	int8_t t;
	t = beat_count - g->offset;
	if (t < 0) return;
	t = t % g->stride;
	if (t >= g->duration) return;
	t = t*16 + beat_t;

	if (g->duration == 2)
		t *= 2;
	else if (g->duration == 8)
		t /= 2;

	pix_t color = g->color;
	int8_t x;
	if (g->direction<0)
		x = 64 - t;
	else
		x = -32 + t;		
	uint8_t i;
	for (i=0; i < 32; i++) {
		color.a = i * (256/32);
		if (x >= 0 && x <= 15) {
			frame[x] = color;
		}
		if (x == 16)
			frame[0] = color;			
		x += g->direction;
	}
	if (g->flip) {
		anim_rotate(frame, 128);
	}
	// while (i != t) {
	// 	i++;
	// 	x += g->direction;
	// 	frame[x%16] = ;
	// }

}

// --------------
