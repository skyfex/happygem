#include "sex.h"

#include "dna.h"

#include <stdlib.h>
#include <string.h>

#define CROSSOVER_AVG_CHUNK ( 2 )


void sex(uint16_t addr_native, uint16_t addr_foreign, 
	uint8_t* genome_size_native, uint8_t* genome_size_foreign, 
	gene_t* genome_native, gene_t* genome_foreign)
{
	if (*genome_size_native != *genome_size_foreign)
		return;

	uint8_t genome_size = *genome_size_native;

	uint16_t addr_min;
	uint16_t addr_max;
	gene_t *genome_min;
	gene_t *genome_max;

	if (addr_native < addr_foreign) {
		addr_min = addr_native;
		addr_max = addr_foreign;
		genome_min = genome_native;
		genome_max = genome_foreign;
	}
	else {
		addr_min = addr_foreign;
		addr_max = addr_native;
		genome_min = genome_foreign;
		genome_max = genome_native;
	}	

	uint16_t addr_a;
	uint16_t addr_b;
	gene_t *genome_a;
	gene_t *genome_b;

	srand(addr_min*addr_max);

	if ((rand()%2)==0) {
		addr_a = addr_min;
		addr_b = addr_max;
		genome_a = genome_min;
		genome_b = genome_max;
	}
	else {
		addr_a = addr_max;
		addr_b = addr_min;
		genome_a = genome_max;
		genome_b = genome_min;
	}


	crossover_crude(genome_native, genome_a, genome_b, genome_size-1);

	if (rand()%4 == 0) {
		uint8_t idx = rand()%(genome_size-2);
		pattern_gene_init((pattern_gene_t*)&genome_native[idx], NULL);
	}

	uint8_t l = genome_size-1; // last
	uint8_t r = rand()%3;
	if (r==0) {
		genome_native[l] = genome_a[l];
	}
	else if (r==1) {
		genome_native[l] = genome_b[l];
	}
	else {
		if (rand()%2 == 0)	
			swirl_gene_init((swirl_gene_t*)&genome_native[l], NULL);
		else
			wings_gene_init((wings_gene_t*)&genome_native[l], NULL);	
	}
	// *genome_size_native = *genome_size_foreign;
	// memcpy(genome_native, genome_foreign, *genome_size_foreign*sizeof(gene_t)); 

	dna_save();
	dna_reset_beat();
}

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
//discard non-visible genes.
void crossover_crude(gene_t* genome_native, gene_t* genome_a, gene_t* genome_b, uint8_t genome_size)
{
	gene_t genome_new[GENOME_MAX_SIZE];

	uint8_t gn_count = genome_size - 1;
	uint8_t gf_count = genome_size - 1;

	uint8_t i;
	for (i = genome_size - 1; i > 0; i--)
	{
		if (rand()%((gn_count * gn_count + gf_count * gf_count) > gn_count * gn_count))
		{
			if (crossover_crude_is_visible((pattern_gene_t*)&genome_a[i])) {
				genome_new[i] = genome_a[i];
				gn_count -= 1;
			}
			else if (crossover_crude_is_visible((pattern_gene_t*)&genome_b[i])) {
				genome_new[i] = genome_b[i];
				gf_count -= 1;
			}
			else {
				genome_new[i] = genome_a[i];
			}
		}
		else
		{
			if (crossover_crude_is_visible((pattern_gene_t*)&genome_b[i])) {
				genome_new[i] = genome_b[i];
				gf_count -= 1;
			}
			else if (crossover_crude_is_visible((pattern_gene_t*)&genome_a[i])) {
				genome_new[i] = genome_a[i];
				gn_count -= 1;
			}
			else { 
				genome_new[i] = genome_b[i];
			}
		}
	}

	for (i = 0; i < genome_size; i++)
	{
		genome_native[i] = genome_new[i];
	}
}

bool crossover_crude_is_visible(pattern_gene_t* g)
{
	uint8_t i;
	for (i = 0; i < g->length; ++i)
	{
		if (g->pattern[i] != -1)
			return true;
	}
	return false;
}

