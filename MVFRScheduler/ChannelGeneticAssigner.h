//
// Created by jenik on 12/7/15.
//

#ifndef MVFRSCHEDULER_CHANNELGENETICASSIGNER_H
#define MVFRSCHEDULER_CHANNELGENETICASSIGNER_H

#include "MVFRScheduler.h"
#include <vector>

class ChannelGeneticAssigner {
private:
    struct Individual {
        std::vector<int> chromosones;
        ChannelsFitness fitness;

        Individual(int chromosonesCount);

        ChannelsFitness EvaluateIndividual(const std::vector<CAMessage> &messages,
                                           const std::set<int> nodesInBothChannels, const double alpha);
    };

    struct Population {
        std::vector<Individual> individuals;
    };
    int populationNumber;
    int populationMaxNumber;
    int populationSize;
    int chromosomesCount;
    double alpha;
    bool verbose;
    Individual bestIndividual;
    std::vector<CAMessage> messages;
    std::set<int> nodesInBothChannels;

    Population population;

    void GenerateInitialPopulation();

    void EvaluatePopulation();

    void Selection();

    void CrossOver();

    void Mutation();

    void ClearPopulation();

    int StopCriterion();

    void CrossOverBinaryOperator(const Individual &parentA, const Individual &parentB, Individual &child);

    int CompareFitnesses(const ChannelsFitness &cf1, const ChannelsFitness &cf2) const;

    double ComputeObjectiveValue(const ChannelsFitness &cf) const;

public:
    ChannelGeneticAssigner(const int populationSize, const int populationMaxNumber, const int chromosomesCount,
                           const CAMessage *const messages, const int messagesCount,
                           const char *const nodesOnBothChannels, const double alpha, const bool verbose);

    ChannelsFitness StartGeneticAlgorithm();

    double ComputeObjectiveValue() const;
};


#endif //MVFRSCHEDULER_CHANNELGENETICASSIGNER_H
