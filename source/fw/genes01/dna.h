#include <avr/io.h>

typedef struct {
   uint8_t type;
   int8_t speed;
   uint8_t startpos;
   uint8_t size;
   uint8_t r;
   uint8_t g;
   uint8_t b;
} genome_t;


void type0(genome_t *gene, uint8_t *state);
void type0_init(genome_t *gene, uint8_t *state);

void type1(genome_t *gene, uint8_t *state);
void type1_init(genome_t *gene, uint8_t *state);

void type2(genome_t *gene, uint8_t *state);
void type2_init(genome_t *gene, uint8_t *state);