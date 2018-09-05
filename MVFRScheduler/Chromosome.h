/// \file Chromosome.h Chromosome for the genetic algorithm

/// Chromosome for the genetic algorithm
struct Chromosome {
    int fitness; ///< Fitness value of the chromosome
    int *genes; ///< Genes of the chromosome
};