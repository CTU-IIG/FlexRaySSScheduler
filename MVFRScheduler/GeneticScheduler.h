/// \file GeneticScheduler.h Header file for the GeneticScheduler.cpp
#ifndef GENETICSCHEDULER_H_
#define GENETICSCHEDULER_H_

#include "Chromosome.h"

/// APOX crossover operator
/// @param dataStructure Global attributes
/// @param parent1 First parent to crossover
/// @param parent2 Second parent to crossover
/// @param child The resulting child
void CrossOverOperatorAPOX(DataStructure *dataStructure, Chromosome parent1, Chromosome parent2, Chromosome &child);

/// OX crossover operator
/// @param dataStructure Global attributes
/// @param parent1 First parent to crossover
/// @param parent2 Second parent to crossover
/// @param child The resulting child
void CrossOverOperatorOx(DataStructure *dataStructure, Chromosome parent1, Chromosome parent2, Chromosome &child);

/// Crossover operator that combine APOX and OX together
/// @param dataStructure Global attributes
/// @param parent1 First parent to crossover
/// @param parent2 Second parent to crossover
/// @param child The resulting child
void CrossOverOperator(DataStructure *dataStructure, Chromosome parent1, Chromosome parent2, Chromosome &child);

/// Entry point for the genetic scheduler
/// @param dataStructure Global attributes
int GeneticScheduler(DataStructure *dataStructure);

#endif