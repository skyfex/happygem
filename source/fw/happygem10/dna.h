#include <avr/io.h>
#include "services/anim/anim.h"

#define GENOME_MAX_SIZE 8
#define STATE_SIZE 8
#define GENE_PADDING 8

// typedef struct {
// 	uint8_t r;
// 	uint8_t g;
// 	uint8_t b;
// } dna_color_t;

#define GENE_BASE       \
	uint8_t		type;   \
	pix_t		color;  \
	uint8_t		offset; \
	uint8_t		stride; \
	uint8_t     duration; \


typedef struct {
	GENE_BASE
	uint8_t data[GENE_PADDING];
} gene_t;

typedef struct {
	uint8_t data[STATE_SIZE];
} gene_state_t;

void dna_init();
void dna_anim();

// ---------------

#define debug_type 1

typedef struct {
	GENE_BASE
} debug_gene_t;

typedef struct {
} debug_state_t;

void debug_gene_init(debug_gene_t *g, debug_state_t *s);
void debug_gene(debug_gene_t *g, debug_state_t *s, pix_t* frame);

//

#define wave_type 2

typedef struct {
	GENE_BASE
} wave_gene_t;

typedef struct {
} wave_state_t;

void wave_gene_init(wave_gene_t *g, wave_state_t *s);
void wave_gene(wave_gene_t *g, wave_state_t *s, pix_t* frame);

//

#define wings_type 3

typedef struct {
	GENE_BASE
} wings_gene_t;

typedef struct {
} wings_state_t;

void wings_gene_init(wings_gene_t *g, wings_state_t *s);
void wings_gene(wings_gene_t *g, wings_state_t *s, pix_t* frame);

//

#define dot_type 4

typedef struct {
	GENE_BASE
	uint8_t pos;
	uint8_t mirror;
} dot_gene_t;

typedef struct {
} dot_state_t;

void dot_gene_init(dot_gene_t *g, dot_state_t *s);
void dot_gene(dot_gene_t *g, dot_state_t *s, pix_t* frame);



