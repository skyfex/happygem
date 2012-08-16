#include "sex.h"

#include "dna.h"

#include <stdlib.h>

#define CROSSOVER_AVG_CHUNK ( 2 )

//Simple Crossover - all genetypes:
//selects chunks of avg size CROSSOVER_AVG_CHUNK from each genome.
void crossover_simple(gene_t* genome_native, gene_t* genome_foreign)
{
	uint8_t i;
	for (i = 0; i < GENOME_MAX_SIZE; i++)
	{
		crossover_simple_splice(&genome_native[i], &genome_foreign[i]);
	}
}

void crossover_simple_splice(gene_t* gene_native, gene_t* gene_foreign)
{
	bool native = rand()%2;

	uint8_t i;
	for (i = 0; i < GENE_SIZE; i++)
	{
		if (!native)
			gene_native->amino[i] = gene_foreign->amino[i];

		if (rand()%CROSSOVER_AVG_CHUNK + 1 == CROSSOVER_AVG_CHUNK)
		{
			native = native? false : true;
		}

	}
}

//Crude Crossover - pattern gene only:
//selects [1, GENOME_MAX_SIZE] genes from each genome.
//(similar to simple crossover w/CROSSOVER_AVG_CHUNK = GENE_SIZE)
void crossover_crude(gene_t* genome_native, gene_t* genome_foreign)
{
	
}

void crossover_crude_is_visible(gene_t* gene)
{
	uint8_t i;
	for (i = 0; i < PATTERN_GENE_PATTERN_MAX_LENGTH; ++i)
	{

	}
}