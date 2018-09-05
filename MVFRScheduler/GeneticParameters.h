/// \file GeneticParameters.h Parameters for the genetic scheduler
#ifndef GENETICPARAMETERS_H_
#define GENETICPARAMETERS_H_

/// Parameters for the genetic scheduler
struct GeneticParameters {
    int populationSize; ///< Size of the population
    int maxGenerationCount; ///< Maximal number of generations the system can evolve.
};

#endif