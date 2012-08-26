#ifndef SEX_H
#define SEX_H

#include "dna.h"

void sex(uint16_t addr_native, uint16_t addr_foreign, 
	uint8_t* genome_size_native, uint8_t* genome_size_foreign, 
	gene_t* genome_native, gene_t* genome_foreign);

void crossover_simple(gene_t* genome_native, gene_t* genome_foreign);
void crossover_simple_splice(gene_t* gene_native, gene_t* gene_foreign);

void crossover_crude(gene_t* genome_native, gene_t* genome_a, gene_t* genome_b, uint8_t genome_size);
bool crossover_crude_is_visible(pattern_gene_t* gene);

#endif
