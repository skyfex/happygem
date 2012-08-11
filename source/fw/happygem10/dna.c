#include "dna.h"

#include "drivers/all.h"
#include "services/anim/anim.h"

#include <stdlib.h>

uint8_t genome_size;
gene_t genome[GENOME_MAX_SIZE];
static gene_state_t state[GENOME_MAX_SIZE];

static int8_t beat_t;     // Position in beat (0-16)
static int8_t beat_count; // beat count  (0-32)

void dna_anim_gene(gene_t *g, gene_state_t *s, pix_t *frame);

// ------------

void dna_init()
{
	beat_t = 0;
	beat_count = 0;
	genome_size = 1;

	pattern_gene_init((pattern_gene_t*)&genome[0], (pattern_state_t*)&state[0]);
	// debug_gene_init((debug_gene_t*)&genome[1], (debug_state_t*)&state[1]);
}

void dna_anim()
{
	ANIM_UPDATE(0,0,0);
	uint8_t i;
	pix_t frame[16];
	for (i=0; i<genome_size; i++) {
		anim_clear(frame);
		dna_anim_gene(&genome[i], &state[i], frame);
		anim_comp_over(anim_frame, frame);
	}
	beat_t++;
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
	if (rand()%2==0)
		g->offset += g->duration;
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
	dna_random_color_full(&g->color);

	g->stride = 1<<(rand()%4+1);
	g->duration = 1<<(rand()%2+2);
	if (g->duration > g->stride)
		g->duration = 2;
	g->offset = 0;
	if (rand()%2==0)
		g->offset += g->duration;
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
	g->type = pattern_type;
	g->length = rand()%(PATTERN_GENE_PATTERN_MAX_LENGTH + 1);

	uint8_t i;
	for (i = 0; i < PATTERN_GENE_PATTERN_MAX_LENGTH; ++i)
	{
		if (rand()%2 == 1)
			g->pattern[i] = rand()%PATTERN_GENE_MAX_COLORS;
		else
			g->pattern[i] = -1;
	}

	for (i = 0; i < PATTERN_GENE_MAX_COLORS; ++i)
	{
		dna_random_color_true(&g->color[i]);
	}
}

void pattern_gene(pattern_gene_t *g, pattern_state_t *s, pix_t* frame)
{
	// uint8_t i;
	// for (i = 0; i < PATTERN_GENE_MAX_COLORS; ++i)
	// {
	// 	frame[i] = g->color[i];
	// }
	uint8_t i;
	for (i = 0; i < g->length; ++i)
	{
		if (g->pattern[i] > -1)
			frame[i] = g->color[g->pattern[i]];
	}



}
