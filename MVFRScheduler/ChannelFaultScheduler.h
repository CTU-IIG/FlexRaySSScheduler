//
// Created by jenik on 14.12.15.
//

#ifndef MVFRSCHEDULER_CHANNELFAULTSCHEDULER_H
#define MVFRSCHEDULER_CHANNELFAULTSCHEDULER_H

#include "MVFRScheduler.h"

typedef std::vector<std::vector<std::vector<CFrame>>> NodeSchedules;

class ChannelFaultScheduler {
private:
    struct FTSignal {
        int ID;
        int node;
        int period;
        int payload;
        int releaseDate;
        int deadline;
        int channel;
        int isFaultTolerant;
    };

    DataStructure *dataStructure;

    int PlaceSignalToChannelSchedule(DataStructure *channelDS, std::vector<std::vector<CFrame>> &schedule,
                                     int signalIndex, int releaseDate);

    Position FTFindFirstCyclePositionForSignal(DataStructure *dataStructure,
                                               std::vector<std::vector<CFrame>> &schedule,
                                               int signalIndex, Position startPosition);

    int FTPlaceSignalToFrame(DataStructure *dataStructure, std::vector<std::vector<CFrame>> &schedule,
                             Position position,
                             int signalIndex);

    void InitializeSignalList(std::vector<FTSignal> &signalList);

public:
    ChannelFaultScheduler(DataStructure *dataStructure);

    ChannelSchedulerResult ChannelFTScheduler(const ChannelsFitness cfInput);

    ChannelSchedulerResult ChannelFTScheduler(const ChannelsFitness cfInput, bool draw);

    NodeSchedules CreateFaultTolerantSchedules(std::vector<FTSignal> signalList);
};


#endif //MVFRSCHEDULER_CHANNELFAULTSCHEDULER_H
