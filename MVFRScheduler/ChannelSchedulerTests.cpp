//
// Created by jenik on 12/9/15.
//

#include "ChannelSchedulerTests.h"
#include <sys/time.h>

void ChannelSchedulerTest(DataStructure *dataStructure, const double alpha) {
    int mode = 0;
    mode = 2;
    switch (mode) {
        case 1:
            IterativeChannelSchedulerTest(dataStructure, alpha);
            break;
        case 2:
            ChannelAssigningMethodComparison(dataStructure, alpha);
            break;
        case 3:
            IterativeFTChannelSchedulerTest(dataStructure, alpha);
            break;
        case 4:
            AppendToCSV(dataStructure, static_cast<float>(alpha));
            break;
        default:
            ChannelAssignerTestOld(dataStructure, alpha);
            break;
    }
}

void IterativeFTChannelSchedulerTest(DataStructure *dataStructure, const double alpha) {
    ChannelFaultScheduler chfs(dataStructure);
    ChannelsFitness cf = ChannelAssigner(dataStructure, alpha, 1.0, 1000);
    chfs.ChannelFTScheduler(cf);
}

void ChannelAssigningMethodComparison(DataStructure *dataStructure, const double alpha) {
    clock_t startTime, endTime;
    timeval startTimeTV, endTimeTV;

    AppendToCSV(dataStructure, static_cast<float>(alpha));

    //startTime = clock();
    gettimeofday(&startTimeTV, NULL);
    printf("Gurobi ILP run\n");
#ifdef GUROBI
    ChannelsFitness cfILP = ILPChannelingGurobi(dataStructure, alpha);
#else
    ChannelsFitness cfILP = ILPChanneling(dataStructure, alpha);
#endif
    //ChannelsFitness cfILP2 = ILPChanneling(dataStructure, alpha);
    //endTime = clock();
    gettimeofday(&endTimeTV, NULL);
    if (dataStructure->appendToCSV != 0) {
        AppendToCSV(dataStructure, cfILP.channelA);
        AppendToCSV(dataStructure, cfILP.channelB);
        AppendToCSV(dataStructure, cfILP.gateway);
        //AppendToCSV(dataStructure,
        //            ((static_cast<float>(endTime) - static_cast<float>(startTime)) / 1000000.0F) * 1000);
        AppendToCSV(dataStructure,
                    static_cast<float>(1000 * (endTimeTV.tv_sec - startTimeTV.tv_sec) +
                                       (endTimeTV.tv_usec - startTimeTV.tv_usec) / 1000.0));
    }

    startTime = clock();
    ChannelsFitness cf = ChannelAssigner(dataStructure, alpha, 1.0, 10000);
    endTime = clock();
    if (dataStructure->appendToCSV != 0) {
        AppendToCSV(dataStructure, cf.channelA);
        AppendToCSV(dataStructure, cf.channelB);
        AppendToCSV(dataStructure, cf.gateway);
        AppendToCSV(dataStructure,
                    ((static_cast<float>(endTime) - static_cast<float>(startTime)) / 1000000.0F) * 1000);
    }

    startTime = clock();
    CAMessage *messages;
    int messagesCount = AggregateMessages(dataStructure, messages);
    ChannelGeneticAssigner ChGA(100, 100, dataStructure->nodeCount, messages, messagesCount,
                                dataStructure->nodesOnBothChannels, alpha, 1);
    cf = ChGA.StartGeneticAlgorithm();
    endTime = clock();
    if (dataStructure->appendToCSV != 0) {
        AppendToCSV(dataStructure, cf.channelA);
        AppendToCSV(dataStructure, cf.channelB);
        AppendToCSV(dataStructure, cf.gateway);
        AppendToCSV(dataStructure,
                    ((static_cast<float>(endTime) - static_cast<float>(startTime)) / 1000000.0F) * 1000);
    }

    //AppendNLToCSV(dataStructure);
}

void IterativeChannelSchedulerTest(DataStructure *dataStructure, const double alpha) {
    //timeval startTimeTV, endTimeTV;
    IterativeChannelScheduler scheduler(dataStructure, 100);
    //gettimeofday(&startTimeTV, NULL);
    scheduler.CreateSchedule(alpha);
    //gettimeofday(&endTimeTV, NULL);
    //AppendToCSV(dataStructure,
    //            ((static_cast<float>(endTime) - static_cast<float>(startTime)) / 1000000.0F) * 1000);
}

void ChannelAssignerTestOld(DataStructure *dataStructure, const double alpha) {
    clock_t startTime, endTime;
    startTime = clock();
#ifdef GUROBI
    ChannelsFitness cfILP = ILPChannelingGurobi(dataStructure, alpha);
#else
    ChannelsFitness cfILP = ILPChanneling(dataStructure, alpha);
#endif
    endTime = clock();
    if (dataStructure->appendToCSV != 0) {
        AppendToCSV(dataStructure, cfILP.channelA);
        AppendToCSV(dataStructure, cfILP.channelB);
        AppendToCSV(dataStructure, cfILP.gateway);
        AppendToCSV(dataStructure,
                    ((static_cast<float>(endTime) - static_cast<float>(startTime)) / 1000000.0F) * 1000);
    }

    startTime = clock();
    ChannelsFitness cf = ChannelAssigner(dataStructure, alpha, 1.0, 1000);
    endTime = clock();
    if (dataStructure->appendToCSV != 0) {
        AppendToCSV(dataStructure, cf.channelA);
        AppendToCSV(dataStructure, cf.channelB);
        AppendToCSV(dataStructure, cf.gateway);
        AppendToCSV(dataStructure,
                    ((static_cast<float>(endTime) - static_cast<float>(startTime)) / 1000000.0F) * 1000);
    }

    startTime = clock();
    CAMessage *messages;
    int messagesCount = AggregateMessages(dataStructure, messages);
    ChannelGeneticAssigner ChGA(100, 100, dataStructure->nodeCount, messages, messagesCount,
                                dataStructure->nodesOnBothChannels, alpha, 1);
    cf = ChGA.StartGeneticAlgorithm();
    endTime = clock();
    if (dataStructure->appendToCSV != 0) {
        AppendToCSV(dataStructure, cf.channelA);
        AppendToCSV(dataStructure, cf.channelB);
        AppendToCSV(dataStructure, cf.gateway);
        AppendToCSV(dataStructure,
                    ((static_cast<float>(endTime) - static_cast<float>(startTime)) / 1000000.0F) * 1000);
    }
    startTime = clock();
    ChannelScheduler(dataStructure, cf);
    endTime = clock();
    if (dataStructure->appendToCSV != 0) {
        AppendToCSV(dataStructure, dataStructure->maxSlot);
        AppendToCSV(dataStructure,
                    ((static_cast<float>(endTime) - static_cast<float>(startTime)) / 1000000.0F) * 1000);
    }
    int i;
    cf.channelA = 0;
    cf.channelB = 0;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        dataStructure->nodesChannel[i] = 1;
        dataStructure->nodesOnBothChannels[i] = 0;
    }

    ChannelScheduler(dataStructure, cf);
}