//
// Created by jenik on 12/8/15.
//

#ifndef MVFRSCHEDULER_ITERATIVECHANNELSCHEDULER_H
#define MVFRSCHEDULER_ITERATIVECHANNELSCHEDULER_H

#include "MVFRScheduler.h"
#include <set>
#include <string>

class IterativeChannelScheduler {
private:
    int iterationNumber;
    int maxIterationCount;
    std::set<std::size_t> cycleCheckSet;

    DataStructure *dataStructure;

    ChannelSchedulerResult ChannelSchedulerIt(const ChannelsFitness cfInput);

    void InitializeSignalList(std::vector<Signal> &signalList);

    int PlaceSignalToChannelSchedule(DataStructure *channelDS, std::vector<CFrame> *schedule,
                                     int signalIndex, int releaseDate);

    bool TerminationCondition(const ChannelsFitness &cf);

public:
    IterativeChannelScheduler(DataStructure *dataStructure, const int maxIterationCount);

    void CreateSchedule(const double alpha);

    double UpdateBeta(const ChannelSchedulerResult &result);
};


#endif //MVFRSCHEDULER_ITERATIVECHANNELSCHEDULER_H
