#ifndef SEX_H
#define SEX_H

#include "dna.h"

void crossover_simple(gene_t* genome_native, gene_t* genome_foreign);
void crossover_simple_splice(gene_t* gene_native, gene_t* gene_foreign);

void crossover_crude(gene_t* genome_native, gene_t* genome_foreign);
bool crossover_crude_is_visible(pattern_gene_t* gene);
gene_t* crossover_crude_next_nonempty(gene_t* g);

#endif