/// \file DataStructure.cpp Data structure handling
#include "MVFRScheduler.h"

void MakeDeepCopyOfDataStructure(DataStructure *newOne, DataStructure *originalOne) {
    newOne->signalsCount = originalOne->signalsCount;
    newOne->cycleLength = originalOne->cycleLength;
    newOne->slotLength = originalOne->slotLength;
    newOne->variantCount = originalOne->variantCount;
    newOne->nodeCount = originalOne->nodeCount;
    newOne->hyperPeriod = originalOne->hyperPeriod;
    newOne->incoStatistics.maximalPayload = originalOne->incoStatistics.maximalPayload;
    newOne->signalsInNodeCounts = static_cast<int *>(mmalloc(sizeof(int) * newOne->nodeCount));
    newOne->signalNodeIDs = static_cast<int *>(mmalloc(sizeof(int) * newOne->signalsCount));
    newOne->signalPeriods = static_cast<int *>(mmalloc(sizeof(int) * newOne->signalsCount));
    newOne->signalReleaseDates = static_cast<int *>(mmalloc(sizeof(int) * newOne->signalsCount));
    newOne->signalDeadlines = static_cast<int *>(mmalloc(sizeof(int) * newOne->signalsCount));
    newOne->signalLengths = static_cast<int *>(mmalloc(sizeof(int) * newOne->signalsCount));
    newOne->nodesOnBothChannels = static_cast<char *>(mmalloc(sizeof(char) * newOne->nodeCount));
    newOne->nodesChannel = static_cast<char *>(mmalloc(sizeof(char) * newOne->nodeCount));
    newOne->signalInFrameOffsets = static_cast<int *>(mmalloc(sizeof(int) * newOne->signalsCount));
    newOne->signalStartCycle = static_cast<int *>(mmalloc(sizeof(int) * newOne->signalsCount));
    newOne->signalSlot = static_cast<int *>(mmalloc(sizeof(int) * newOne->signalsCount));
    newOne->incoStatistics.periodSignalCount = static_cast<int *>(mmalloc(sizeof(int) * 7));
    newOne->incoStatistics.payloadSignalCount = static_cast<int *>(mmalloc(
            sizeof(int) * newOne->incoStatistics.maximalPayload));
    newOne->saveResultToFile = originalOne->saveResultToFile;
    newOne->signalNames = nullptr;
    newOne->maxSlot = 0;
    newOne->modelVariants = static_cast<char **>(mmalloc(sizeof(char *) * newOne->variantCount));
    newOne->mutualExclusionMatrix = static_cast<char *>(ccalloc(newOne->signalsCount * (newOne->signalsCount + 1) / 2,
                                                                sizeof(char)));
    newOne->mutualExclusionNodeMatrix = static_cast<char *>(ccalloc(newOne->nodeCount * (newOne->nodeCount + 1) / 2,
                                                                    sizeof(char)));
    newOne->algorithm = originalOne->algorithm;
    newOne->schedulerParameters = originalOne->schedulerParameters;
    newOne->isReceivers = originalOne->isReceivers;
    newOne->signalReceivers = new std::vector<int>[newOne->signalsCount];
    int i, j;
    for (i = 0; i < 7; i++)
        newOne->incoStatistics.periodSignalCount[i] = originalOne->incoStatistics.periodSignalCount[i];
    for (i = 0; i < newOne->incoStatistics.maximalPayload; i++)
        newOne->incoStatistics.payloadSignalCount[i] = originalOne->incoStatistics.payloadSignalCount[i];
    for (i = 0; i < newOne->variantCount; i++) {
        newOne->modelVariants[i] = static_cast<char *>(mmalloc(sizeof(char) * newOne->signalsCount));
        for (j = 0; j < newOne->signalsCount; j++) {
            newOne->modelVariants[i][j] = originalOne->modelVariants[i][j];
        }
    }
    for (i = 0; i < newOne->signalsCount * (newOne->signalsCount + 1) / 2; i++)
        newOne->mutualExclusionMatrix[i] = originalOne->mutualExclusionMatrix[i];
    for (i = 0; i < newOne->nodeCount * (newOne->nodeCount + 1) / 2; i++)
        newOne->mutualExclusionNodeMatrix[i] = originalOne->mutualExclusionNodeMatrix[i];
    for (i = 0; i < newOne->nodeCount; i++) {
        newOne->signalsInNodeCounts[i] = originalOne->signalsInNodeCounts[i];
        newOne->nodesOnBothChannels[i] = originalOne->nodesOnBothChannels[i];
        newOne->nodesChannel[i] = originalOne->nodesChannel[i];
    }
    for (i = 0; i < newOne->signalsCount; i++) {
        newOne->signalReceivers[i] = originalOne->signalReceivers[i];
        newOne->signalNodeIDs[i] = originalOne->signalNodeIDs[i];
        newOne->signalPeriods[i] = originalOne->signalPeriods[i];
        newOne->signalReleaseDates[i] = originalOne->signalReleaseDates[i];
        newOne->signalDeadlines[i] = originalOne->signalDeadlines[i];
        newOne->signalLengths[i] = originalOne->signalLengths[i];
        newOne->signalInFrameOffsets[i] = originalOne->signalInFrameOffsets[i];
        newOne->signalSlot[i] = originalOne->signalSlot[i];
        newOne->signalStartCycle[i] = originalOne->signalStartCycle[i];
    }
}

void FreeDataStructure(DataStructure *dataStructure) {
    int i;
    ffree(dataStructure->signalDeadlines);
    ffree(dataStructure->signalInFrameOffsets);
    ffree(dataStructure->signalLengths);
    if (dataStructure->signalNames != nullptr) {
        if (isAllocated(dataStructure->signalNames)) {
            for (i = 0; i < dataStructure->signalsCount; i++)
                ffree(dataStructure->signalNames[i]);
            ffree(dataStructure->signalNames);
        }
    }
    if (dataStructure->isReceivers) {
        delete[] dataStructure->signalReceivers;
        ffree(dataStructure->nodesOnBothChannels);
        ffree(dataStructure->nodesChannel);
    }
    ffree(dataStructure->signalNodeIDs);
    ffree(dataStructure->signalPeriods);
    ffree(dataStructure->signalReleaseDates);
    ffree(dataStructure->signalsInNodeCounts);
    ffree(dataStructure->mutualExclusionMatrix);
    ffree(dataStructure->mutualExclusionNodeMatrix);
    ffree(dataStructure->signalSlot);
    ffree(dataStructure->signalStartCycle);
    ffree(dataStructure->incoStatistics.periodSignalCount);
    ffree(dataStructure->incoStatistics.payloadSignalCount);
    if (isAllocated(dataStructure->modelVariants)) {
        for (i = 0; i < dataStructure->variantCount; i++) {
            ffree(dataStructure->modelVariants[i]);
        }
        ffree(dataStructure->modelVariants);
    }
    if (isAllocated(dataStructure->oldSchedule.signalInFrameOffsets))
        ffree(dataStructure->oldSchedule.signalInFrameOffsets);
    if (isAllocated(dataStructure->oldSchedule.signalSlot))
        ffree(dataStructure->oldSchedule.signalSlot);
    if (isAllocated(dataStructure->oldSchedule.signalStartCycle))
        ffree(dataStructure->oldSchedule.signalStartCycle);
}

void moveOriginalScheduleToCurrentSchedule(DataStructure *dataStructure) {
    int i;
    ffree(dataStructure->signalInFrameOffsets);
    ffree(dataStructure->signalSlot);
    ffree(dataStructure->signalStartCycle);
    dataStructure->signalSlot = dataStructure->oldSchedule.signalSlot;
    dataStructure->signalInFrameOffsets = dataStructure->oldSchedule.signalInFrameOffsets;
    dataStructure->signalStartCycle = dataStructure->oldSchedule.signalStartCycle;
    dataStructure->signalsCount = dataStructure->oldSchedule.signalsCount;
    dataStructure->maxSlot = -1;
    for (i = 0; i < dataStructure->signalsCount; i++) {
        dataStructure->maxSlot =
                (dataStructure->signalSlot[i] > dataStructure->maxSlot) ? dataStructure->signalSlot[i]
                                                                        : dataStructure->maxSlot;
    }
    dataStructure->maxSlot++;
}

void getNodeToSlotAssignment(const DataStructure *dataStructure, std::set<int> *nodeSlotAssignment) {
    int i;
    for (i = 0; i < dataStructure->signalsCount; i++) // find out which slots a given node uses
    {
        nodeSlotAssignment[dataStructure->signalNodeIDs[i] - 1].insert(dataStructure->signalSlot[i]);
    }
}
