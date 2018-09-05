/// \file GeneticScheduler.cpp Genetic scheduler - Genetic framework over the First fit scheduler
#include "MVFRScheduler.h"

void CrossOverOperatorAPOX(DataStructure *dataStructure, Chromosome parent1, Chromosome parent2, Chromosome &child) {
    //Absolute position ordered crossover
    int pp1 = 0, pp2 = 0, pch = 0;
    std::set<int> usedGenes;

    while (pch < dataStructure->signalsCount) {
        if ((rand() % (parent1.fitness + parent2.fitness)) < parent2.fitness) {
            while (usedGenes.find(parent1.genes[pp1]) != usedGenes.end())
                pp1++;
            child.genes[pch] = parent1.genes[pp1];
        }
        else {
            while (usedGenes.find(parent2.genes[pp2]) != usedGenes.end())
                pp2++;
            child.genes[pch] = parent2.genes[pp2];
        }
        usedGenes.insert(child.genes[pch]);
        pch++;
    }
}

void CrossOverOperatorOx(DataStructure *dataStructure, Chromosome parent1, Chromosome parent2, Chromosome &child) {
    // OX

    int i, j;
    int cutA = rand() % (dataStructure->signalsCount - 1);
    int cutB = cutA + rand() % (dataStructure->signalsCount - cutA - 1) + 1;

    std::set<int> usedGenes;
    for (i = cutA; i <= cutB; i++) {
        child.genes[i] = parent1.genes[i];
        usedGenes.insert(parent1.genes[i]);
    }

    i = (cutB + 1) % dataStructure->signalsCount;
    j = (cutB + 1) % dataStructure->signalsCount;
    while (j != cutA) {
        if (usedGenes.find(parent2.genes[i]) == usedGenes.end()) {
            child.genes[j] = parent2.genes[i];
            j = (j + 1) % dataStructure->signalsCount;
        }
        i = (i + 1) % dataStructure->signalsCount;
    }
}

void CrossOverOperator(DataStructure *dataStructure, Chromosome parent1, Chromosome parent2, Chromosome &child) {
    //if(rand()%2)
    //	CrossOverOperatorAPOX(dataStructure, parent1, parent2, child);
    //else
    CrossOverOperatorOx(dataStructure, parent1, parent2, child);
}

int GeneticScheduler(DataStructure *dataStructure) {
    int i, j;
    int tmpSwap, tmpSwapIndex, tmpSwapIndex2;
    int generation = 0;
    int lowerBound = ComputeBetterLowerBound(dataStructure);
    int tournament1, tournament2;
    int worstInGeneration, averageOfPopulation, bestInPopulation, bestSoFar = INT_MAX;
    int crossoverThreshold;
    int tmpPopSize;
    std::vector<Chromosome> population;
    Chromosome newBornChild;

    int *bestSoFarGenes = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    int *aux = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    srand(static_cast<unsigned int>(time(NULL)));

    // Generating of the initial population
    for (i = 0; i < ((static_cast<GeneticParameters *>(dataStructure->schedulerParameters))->populationSize); i++) {
        Chromosome ch;
        ch.fitness = -1;
        ch.genes = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
        for (j = 0; j < dataStructure->signalsCount; j++)
            ch.genes[j] = j;
        switch (i) {
            case 0:
                break;
            case 1:
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          WINDOW);
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          PERIOD);
                break;
            case 2:
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          PERIOD);
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          WINDOW);
                break;
            case 3:
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          PERIOD);
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          LENGTH);
                break;
            case 4:
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          PERIOD);
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          LENGTHDEC);
                break;
            case 5:
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          PERIOD);
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          DEADLINE);
                break;
            case 6:
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          PERIOD);
                mergeSort(ch.genes, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
                          RELEASEDATE);
                break;
            default:
                for (j = 0; j < dataStructure->signalsCount; j++) // Random swapping
                {
                    tmpSwapIndex = rand() % dataStructure->signalsCount;
                    tmpSwap = ch.genes[j];
                    ch.genes[j] = ch.genes[tmpSwapIndex];
                    ch.genes[tmpSwapIndex] = tmpSwap;
                }
                break;
        }
        population.push_back(ch);
    }
    ffree(aux);

    while (generation < (static_cast<GeneticParameters *>(dataStructure->schedulerParameters))->maxGenerationCount) {
        worstInGeneration = 0;
        averageOfPopulation = 0;
        bestInPopulation = INT_MAX;
        // Evaluate
        for (i = 0; i < (int) population.size(); i++) {
            if (population[i].fitness == -1) // if fitness is already calculated
            {
                FirstFitScheduler(dataStructure, population[i].genes);
                population[i].fitness = dataStructure->maxSlot - lowerBound;
            }
            if (population[i].fitness > worstInGeneration) {
                worstInGeneration = population[i].fitness;
            }
            if (population[i].fitness < bestInPopulation) {
                bestInPopulation = population[i].fitness;
                if (population[i].fitness < bestSoFar) {
                    bestSoFar = population[i].fitness;
                    for (j = 0; j < dataStructure->signalsCount; j++)
                        bestSoFarGenes[j] = population[i].genes[j];
                }
            }
            averageOfPopulation += population[i].fitness;
            if (population[i].fitness == 0)
                break;
        }
        if (verboseLevel > 0)
            printf("Population %d: %d, %d, %lu, %d\n", generation, bestSoFar, bestInPopulation,
                   averageOfPopulation / population.size(), worstInGeneration);
        if (bestSoFar == 0)
            break;

        // Selection
        while ((static_cast<GeneticParameters *>(dataStructure->schedulerParameters))->populationSize <
               (int) population.size()) {
            tournament1 = rand() % static_cast<int>(population.size());
            tournament2 = rand() % static_cast<int>(population.size());
            while (tournament1 == tournament2)
                tournament2 = rand() % static_cast<int>(population.size());
            if (population[tournament2].fitness > population[tournament1].fitness) {
                tournament1 = tournament2;
            }
            ffree(population[tournament1].genes);
            population.erase(population.begin() + tournament1);
        }

        tmpPopSize = static_cast<int>(population.size());
        // Crossover
        for (i = 0; i < tmpPopSize; i++) {
            crossoverThreshold = population[i].fitness;//rand() % ((worstInGeneration - bestInPopulation)>0?(worstInGeneration - bestInPopulation):1) + bestInPopulation + 1;
            j = (i + 1) % tmpPopSize;
            while ((population[j].fitness > crossoverThreshold) && (j != i))
                j = (j + 1) % tmpPopSize;
            if (j == i)
                j = rand() % tmpPopSize;
            newBornChild.fitness = -1;
            newBornChild.genes = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
            CrossOverOperator(dataStructure, population[i], population[j], newBornChild);
            population.push_back(newBornChild);
            newBornChild.genes = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
            CrossOverOperator(dataStructure, population[j], population[i], newBornChild);
            population.push_back(newBornChild);
        }

        // Mumation
        for (i = 0; i < (int) population.size(); i++) {
            if (rand() % 100 < 1) {
                tmpSwapIndex = rand() % dataStructure->signalsCount;
                tmpSwapIndex2 = rand() % dataStructure->signalsCount;
                tmpSwap = population[i].genes[tmpSwapIndex];
                population[i].genes[tmpSwapIndex] = population[i].genes[tmpSwapIndex2];
                population[i].genes[tmpSwapIndex2] = tmpSwap;
            }
        }

        generation++;
    }

    FirstFitScheduler(dataStructure, bestSoFarGenes);

    for (i = 0; i < (int) population.size(); i++) {
        ffree(population[i].genes);
    }
    ffree(bestSoFarGenes);
    return 0;
}
