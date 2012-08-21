#include <avr/io.h>
#include "services/anim/anim.h"

//General
#define GENOME_MAX_SIZE 8
#define STATE_SIZE 16
#define GENE_PADDING 16

#define DNA_COLOR_STRENGTH 125

#define GENE_BASE_BASIC \
	uint8_t		type;	\
	uint8_t		stride;	\

#define GENE_BASE_COMMON 	\
	GENE_BASE_BASIC			\
	pix_t		color;		\
	uint8_t		offset;		\
	uint8_t     duration;	\

//Gene specific
#define PATTERN_GENE_PATTERN_MAX_LENGTH 5
#define PATTERN_GENE_MAX_COLORS 2
#define PATTERN_GENE_MAX_STRIDE 3
#define PATTERN_GENE_MAX_LEAP 3

typedef struct {
	GENE_BASE_BASIC
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
	GENE_BASE_COMMON
} debug_gene_t;

typedef struct {
} debug_state_t;

void debug_gene_init(debug_gene_t *g, debug_state_t *s);
void debug_gene(debug_gene_t *g, debug_state_t *s, pix_t* frame);

//

#define wave_type 2

typedef struct {
	GENE_BASE_COMMON
} wave_gene_t;

typedef struct {
} wave_state_t;

void wave_gene_init(wave_gene_t *g, wave_state_t *s);
void wave_gene(wave_gene_t *g, wave_state_t *s, pix_t* frame);

//

#define wings_type 3

typedef struct {
	GENE_BASE_COMMON
} wings_gene_t;

typedef struct {
} wings_state_t;

void wings_gene_init(wings_gene_t *g, wings_state_t *s);
void wings_gene(wings_gene_t *g, wings_state_t *s, pix_t* frame);

//

#define dot_type 4

typedef struct {
	GENE_BASE_COMMON
	uint8_t pos;
	uint8_t mirror;
} dot_gene_t;

typedef struct {
} dot_state_t;

void dot_gene_init(dot_gene_t *g, dot_state_t *s);
void dot_gene(dot_gene_t *g, dot_state_t *s, pix_t* frame);


#define pattern_type 5

//Pattern Gene
typedef struct {
	GENE_BASE_BASIC
	int8_t pattern[PATTERN_GENE_PATTERN_MAX_LENGTH];
	uint8_t length;
	uint8_t leap;
	pix_t color[PATTERN_GENE_MAX_COLORS];
} pattern_gene_t;

//Pattern State:
typedef struct {

} pattern_state_t;

void pattern_gene_init(pattern_gene_t *g, pattern_state_t *s);
void pattern_gene(pattern_gene_t *g, pattern_state_t *s, pix_t* frame);
