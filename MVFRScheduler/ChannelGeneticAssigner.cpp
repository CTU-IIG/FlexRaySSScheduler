//
// Created by jenik on 12/7/15.
//

#include "ChannelGeneticAssigner.h"

using namespace std;

ChannelGeneticAssigner::Individual::Individual(int chromosonesCount) {
    this->chromosones.clear();
    this->chromosones.reserve(chromosonesCount);
    for (int i = 0; i < chromosonesCount; i++) {
        this->chromosones.push_back(rand() % 2);
    }
}

ChannelGeneticAssigner::ChannelGeneticAssigner(const int populationSize, const int populationMaxNumber,
                                               const int chromosomesCount,
                                               const CAMessage *const messages, const int messagesCount,
                                               const char *const nodesOnBothChannels, const double alpha,
                                               const bool verbose = false) :
        populationSize(populationSize), populationMaxNumber(populationMaxNumber),
        chromosomesCount(chromosomesCount), alpha(alpha),
        bestIndividual(Individual(chromosomesCount)), verbose(verbose) {
    if (this->alpha < 0) {
        int payloadsSum = 0;
        for (int i = 0; i < messagesCount; i++) {
            for (int j = 0; j < static_cast<int>(messages[i].nodes.size()); j++) {
                if (nodesOnBothChannels[messages[i].nodes[j]] != 1) {
                    payloadsSum += messages[i].payload;
                    break;
                }
            }
        }
        this->alpha = 1.0 / payloadsSum;
    }
    this->messages.clear();
    this->messages.resize(messagesCount);
    copy(messages, messages + messagesCount, this->messages.begin());
    for (int i = 0; i < chromosomesCount; i++) {
        if (nodesOnBothChannels[i] == 1) this->nodesInBothChannels.insert(i);
    }
    bestIndividual.fitness.channelA = std::numeric_limits<int>::max();
    bestIndividual.fitness.channelB = std::numeric_limits<int>::max();
    bestIndividual.fitness.gateway = std::numeric_limits<int>::max();
}

void ChannelGeneticAssigner::ClearPopulation() {
    this->population.individuals.clear();
    populationNumber = 0;
}

void ChannelGeneticAssigner::GenerateInitialPopulation() {
    ClearPopulation();
    this->population.individuals.resize(populationSize, Individual(chromosomesCount));
    generate_n(this->population.individuals.begin(), populationSize,
               [this]() { return Individual(this->chromosomesCount); });
}

int ChannelGeneticAssigner::StopCriterion() {
    if (this->populationNumber < populationMaxNumber)
        return 1;
    else
        return 0;
}

ChannelsFitness ChannelGeneticAssigner::Individual::EvaluateIndividual(const std::vector<CAMessage> &messages,
                                                                       const std::set<int> nodesInBothChannels,
                                                                       const double alpha) {
    bool A, B;
    this->fitness.channelA = 0;
    this->fitness.channelB = 0;
    this->fitness.gateway = 0;
    for (CAMessage message : messages) {
        A = false;
        B = false;
        for (int node : message.nodes) {
            if (nodesInBothChannels.find(node) == nodesInBothChannels.end()) {
                if (this->chromosones[node] == 0)
                    A = true;
                else
                    B = true;
            }
        }
        if (A) this->fitness.channelA += message.payload;
        if (B) this->fitness.channelB += message.payload;
        if (A && B) this->fitness.gateway += message.payload;
    }
    return this->fitness;
}

void ChannelGeneticAssigner::EvaluatePopulation() {
    ChannelsFitness populationBestQuality = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max(),
                                             std::numeric_limits<int>::max()};
    for (Individual &individual : population.individuals) {
        individual.EvaluateIndividual(messages, nodesInBothChannels, alpha);
        if (CompareFitnesses(individual.fitness, populationBestQuality) < 0)
            populationBestQuality = individual.fitness;
        if (CompareFitnesses(individual.fitness, bestIndividual.fitness) < 0)
            bestIndividual = individual;
    }
}

void ChannelGeneticAssigner::Selection() {
    while (population.individuals.size() > populationSize) {
        int tournament1 = rand() % static_cast<int>(population.individuals.size());
        int tournament2 = rand() % static_cast<int>(population.individuals.size());
        while (tournament1 == tournament2) {
            tournament2 = rand() % static_cast<int>(population.individuals.size());
        }
        if (CompareFitnesses(population.individuals[tournament2].fitness, population.individuals[tournament1].fitness) >
            0)
            tournament1 = tournament2;
        population.individuals.erase(population.individuals.begin() + tournament1);
    }
}

void ChannelGeneticAssigner::CrossOver() {
    int tmpPopSize = static_cast<int>(population.individuals.size());
    ChannelsFitness crossoverThreshold;
    for (int i = 0; i < tmpPopSize; i++) {
        crossoverThreshold = population.individuals[i].fitness;
        int j = (i + 1) % tmpPopSize;
        while (CompareFitnesses(population.individuals[j].fitness, crossoverThreshold) > 0 && (j != i))
            j = (j + 1) % tmpPopSize;
        if (j == i)
            j = rand() % tmpPopSize;
        Individual newBornChild1(chromosomesCount);
        CrossOverBinaryOperator(population.individuals[i], population.individuals[j], newBornChild1);
        population.individuals.push_back(newBornChild1);

        Individual newBornChild2(chromosomesCount);
        CrossOverBinaryOperator(population.individuals[j], population.individuals[i], newBornChild2);
        population.individuals.push_back(newBornChild2);
    }
}


ChannelsFitness ChannelGeneticAssigner::StartGeneticAlgorithm() {
    GenerateInitialPopulation();
    while (StopCriterion()) {
        EvaluatePopulation();
        if (verbose)
            cout << "Population " << populationNumber << ": " << ComputeObjectiveValue(bestIndividual.fitness) << endl;
        Selection();
        CrossOver();
        Mutation();
        populationNumber++;
    }
    EvaluatePopulation();
    if (verbose)
        cout << "Last population: " << ComputeObjectiveValue(bestIndividual.fitness) << endl;
    return bestIndividual.fitness;
}

void ChannelGeneticAssigner::CrossOverBinaryOperator(const ChannelGeneticAssigner::Individual &parentA,
                                                     const ChannelGeneticAssigner::Individual &parentB,
                                                     ChannelGeneticAssigner::Individual &child) {
    int splitPoint = rand() % (static_cast<int>(parentA.chromosones.size()) - 2) + 1;
    if (splitPoint > parentA.chromosones.size() << splitPoint < 0) {
        child = parentA;
        return;
    }
    child.chromosones.resize(parentA.chromosones.size());
    for (int i = 0; i < parentA.chromosones.size(); i++) {
        if (i < splitPoint)
            child.chromosones[i] = parentA.chromosones[i];
        else
            child.chromosones[i] = parentB.chromosones[i];
    }
}

void ChannelGeneticAssigner::Mutation() {
    for (int i = 0; i < population.individuals.size(); i++) {
        if (rand() % 100 < 1) {
            int tmpSwapIndex = rand() % chromosomesCount;
            int tmpSwapIndex2 = rand() % chromosomesCount;
            int tmpSwap = population.individuals[i].chromosones[tmpSwapIndex];
            population.individuals[i].chromosones[tmpSwapIndex] = population.individuals[i].chromosones[tmpSwapIndex2];
            population.individuals[i].chromosones[tmpSwapIndex2] = tmpSwap;
        }
    }
}

double ChannelGeneticAssigner::ComputeObjectiveValue(const ChannelsFitness &cf) const {
    return std::max(cf.channelA, cf.channelB) + alpha * cf.gateway;
}

double ChannelGeneticAssigner::ComputeObjectiveValue() const {
    return ComputeObjectiveValue(bestIndividual.fitness);
}

int ChannelGeneticAssigner::CompareFitnesses(const ChannelsFitness &cf1, const ChannelsFitness &cf2) const {
    double cf1val = ComputeObjectiveValue(cf1);
    double cf2val = ComputeObjectiveValue(cf2);
    if (cf1val < cf2val)
        return -1;
    if (cf1val > cf2val)
        return 1;
    return 0;
}