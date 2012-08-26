#include "sex.h"

#include "dna.h"

#include <stdlib.h>
#include <string.h>

#define CROSSOVER_AVG_CHUNK ( 2 )


void sex(uint16_t addr_native, uint16_t addr_foreign, 
	uint8_t* genome_size_native, uint8_t* genome_size_foreign, 
	gene_t* genome_native, gene_t* genome_foreign)
{
	*genome_size_native = *genome_size_foreign;
	memcpy(genome_native, genome_foreign, *genome_size_foreign*sizeof(gene_t)); 

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
void crossover_crude(gene_t* genome_native, gene_t* genome_foreign)
{
	gene_t genome_new[GENOME_MAX_SIZE];

	uint8_t gn_count = GENOME_SIZE - 1;
	uint8_t gf_count = GENOME_SIZE - 1;

	uint8_t gn_prob = 1;
	uint8_t gf_prob = 1;

	uint8_t i;
	for (i = GENOME_SIZE - 1; i > 0; i--)
	{
		if (rand()%(gn_prob + gf_prob) < gn_prob)
		{
			if (crossover_crude_is_visible((pattern_gene_t*)&genome_native[i])) {
				genome_new[i] = genome_native[gn_count];
				gn_count -= 1;
				gf_prob += 2*gn_prob;
			}
			else if (crossover_crude_is_visible((pattern_gene_t*)&genome_foreign[i])) {
				genome_new[i] = genome_foreign[gf_count];
				gf_count -= 1;
				gn_prob += 2*gf_prob;
			}
			else {
				genome_new[i] = genome_native[gn_count];
				gn_count -= 1;
			}
		}
		else
		{
			if (crossover_crude_is_visible((pattern_gene_t*)&genome_foreign[i])) {
				genome_new[i] = genome_foreign[gf_count];
				gf_count -= 1;
				gn_prob += 2*gf_prob;
			}
			else if (crossover_crude_is_visible((pattern_gene_t*)&genome_native[i])) {
				genome_new[i] = genome_native[gn_count];
				gn_count -= 1;
				gf_prob += 2*gn_prob;
			}
			else { 
				genome_new[i] = genome_foreign[gf_count];
				gf_count -= 1;
			}
		}
	}

	for (i = 0; i < GENOME_SIZE; i++)
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

