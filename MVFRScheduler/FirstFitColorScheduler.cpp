/// \file FirstFitColorScheduler.cpp Improved First Fit Scheduler that uses Graph coloring for Slot scheduling
#include "MVFRScheduler.h"


int CFindFreePositionInFrame(DataStructure *dataStructure, CFrame *frame, int signalIndex, int startPosition) {
    int i, j, freeBytesInSequence, bestFoundOffset, last, currentFreeOffset;
    std::vector<char> messagePositions(dataStructure->slotLength, 0);
    for (i = 0; i < static_cast<int>(frame->signals.size()); i++) // For each signal in frame
    {
        if (getMEMatrixValue(dataStructure->mutualExclusionMatrix, frame->signals[i],
                             signalIndex)) // If signal can have conflict
        {
            for (j = dataStructure->signalInFrameOffsets[frame->signals[i]];
                 j < dataStructure->signalInFrameOffsets[frame->signals[i]] +
                     dataStructure->signalLengths[frame->signals[i]]; j++) {
                messagePositions[j] = 1;
            }
        }
    }
    bestFoundOffset = -1;
    freeBytesInSequence = 0;
    last = 1;
    currentFreeOffset = 0;
    for (i = startPosition; i < dataStructure->slotLength; i++) {
        if (messagePositions[i] == 1) {
            if (freeBytesInSequence >= dataStructure->signalLengths[signalIndex]) {
                bestFoundOffset = currentFreeOffset;
                break;
            }
            freeBytesInSequence = 0;
            last = 1;
        }
        else {
            if (last == 1)
                currentFreeOffset = i;
            freeBytesInSequence++;
            last = 0;
        }
    }

    if (freeBytesInSequence >= dataStructure->signalLengths[signalIndex] && bestFoundOffset == -1) {
        bestFoundOffset = currentFreeOffset;
    }
    return bestFoundOffset;
}

int CTestSignalInFrame(DataStructure *dataStructure, CFrame *frame, int inFrameOffset, int signalIndex) {
    int i;
    for (i = 0; i < static_cast<int>(frame->signals.size()); i++) {
        if (getMEMatrixValue(dataStructure->mutualExclusionMatrix, frame->signals[i],
                             signalIndex)) // If signal can have conflict
        {
            // If there is a collision
            if (!((inFrameOffset >= dataStructure->signalInFrameOffsets[frame->signals[i]] +
                                    dataStructure->signalLengths[frame->signals[i]]) ||
                  (inFrameOffset + dataStructure->signalLengths[signalIndex] <=
                   dataStructure->signalInFrameOffsets[frame->signals[i]])))
                return 0;
        }
    }
    return 1;
}

Position CFindFirstCyclePositionForSignal(DataStructure *dataStructure, std::vector<CFrame> *schedule, int signalIndex,
                                          Position startPosition) {
    int i, j;
    int inFramePosition;
    char init = 1;
    Position resultPosition;
    resultPosition.cycle = -1;
    resultPosition.inFrameOffset = -1;
    resultPosition.slot = -1;
    for (i = startPosition.slot; i < static_cast<int>(schedule[0].size()); i++) // Slots
    {
        for (j = init ? startPosition.cycle : dataStructure->signalReleaseDates[signalIndex];
             j < dataStructure->signalDeadlines[signalIndex]; j++) // Cycles
        {
            inFramePosition = CFindFreePositionInFrame(dataStructure, &schedule[j][i],
                                                       signalIndex, init ? startPosition.inFrameOffset : 0);
            if (inFramePosition != -1) {
                resultPosition.inFrameOffset = inFramePosition;
                resultPosition.cycle = j;
                resultPosition.slot = i;
                return resultPosition;
            }
        }
        init = 0;
    }
    return resultPosition;
}

int CPlaceSignalToFrame(DataStructure *dataStructure, std::vector<CFrame> *schedule, Position position,
                        int signalIndex) {
    int i;
    dataStructure->signalSlot[signalIndex] = position.slot;
    dataStructure->signalStartCycle[signalIndex] = position.cycle;
    dataStructure->signalInFrameOffsets[signalIndex] = position.inFrameOffset;
    for (i = 0; i < dataStructure->hyperPeriod / dataStructure->signalPeriods[signalIndex]; i++) {
        schedule[position.cycle + i * dataStructure->signalPeriods[signalIndex]][position.slot].signals.push_back(
                signalIndex);
    }
    return 0;
}

int PlaceFFCSignalToSchedule(DataStructure *dataStructure, std::vector<CFrame> *schedule, int signalIndex) {
    int i;
    Position startPosition, placePosition;
    char infeasiblePosition = 1;
    startPosition.inFrameOffset = 0;
    startPosition.cycle = dataStructure->signalReleaseDates[signalIndex];
    startPosition.slot = 0;

    while (infeasiblePosition) {
        placePosition = CFindFirstCyclePositionForSignal(dataStructure, schedule, signalIndex, startPosition);
        if (placePosition.cycle == -1 || placePosition.inFrameOffset == -1 || placePosition.slot == -1)
            break;

        infeasiblePosition = 0;
        if (static_cast<FirstFitParameters *>(dataStructure->schedulerParameters)->sortType != FFALL
            && static_cast<FirstFitParameters *>(dataStructure->schedulerParameters)->sortType != FFPERIOD) {
            for (i = 1; i < dataStructure->hyperPeriod / dataStructure->signalPeriods[signalIndex]; i++) {
                if (!CTestSignalInFrame(dataStructure,
                                        &schedule[placePosition.cycle +
                                                  i * dataStructure->signalPeriods[signalIndex]][placePosition.slot],
                                        placePosition.inFrameOffset, signalIndex)) {
                    infeasiblePosition = 1;
                    startPosition.cycle = placePosition.cycle;
                    startPosition.slot = placePosition.slot;
                    startPosition.inFrameOffset = (placePosition.inFrameOffset + 1) % dataStructure->slotLength;
                    if (startPosition.inFrameOffset == 0)
                        startPosition.cycle = (startPosition.cycle + 1) % dataStructure->hyperPeriod;
                    if (startPosition.cycle == 0)
                        startPosition.slot++;
                    break;
                }
            }
        }
        if (!infeasiblePosition) {
            CPlaceSignalToFrame(dataStructure, schedule, placePosition, signalIndex);
        }
    }
    if (infeasiblePosition) // Create new slot
    {
        CFrame f;
        dataStructure->signalStartCycle[signalIndex] = dataStructure->signalReleaseDates[signalIndex];
        dataStructure->signalSlot[signalIndex] = static_cast<int>(schedule[0].size());
        dataStructure->signalInFrameOffsets[signalIndex] = 0;
        for (i = 0; i < dataStructure->hyperPeriod; i++) {
            schedule[i].push_back(f);
            if (i % dataStructure->signalPeriods[signalIndex] == dataStructure->signalReleaseDates[signalIndex])
                schedule[i][schedule[i].size() - 1].signals.push_back(signalIndex);
        }
    }

    return 1;
}

int PrepareNewDataStructure(DataStructure *dataStructure, DataStructure *newDS, int *&signalMapping, int node, int slot,
                            float freeBand) {
    int i, j, tmp, result;
    std::vector<int> newSignalsPeriod;
    std::vector<int> newSignalsPayload;

    // Make copy of signals from the old
    newDS->algorithm = dataStructure->algorithm;
    newDS->cycleLength = dataStructure->cycleLength;
    newDS->hyperPeriod = dataStructure->hyperPeriod;
    newDS->nodeCount = dataStructure->nodeCount;
    newDS->schedulerParameters = dataStructure->schedulerParameters;
    newDS->slotLength = dataStructure->slotLength;

    tmp = 0;
    for (i = 0; i < 7; i++) {
        tmp += dataStructure->incoStatistics.periodSignalCount[i];
    }

    for (i = 0; i < 7; i++) {
        int maxLength = static_cast<int>(
                (freeBand * dataStructure->incoStatistics.periodSignalCount[i] / static_cast<float>(tmp)) *
                dataStructure->slotLength * (static_cast<int>(pow(2.f, static_cast<float>(i)))));
        for (j = 0; j < maxLength; j = j + dataStructure->incoStatistics.maximalPayload) {
            newSignalsPeriod.push_back(static_cast<int>(pow(2.f, static_cast<float>(i))));
            if (maxLength - j < dataStructure->incoStatistics.maximalPayload)
                newSignalsPayload.push_back(maxLength - j);
            else
                newSignalsPayload.push_back(dataStructure->incoStatistics.maximalPayload);
        }
    }

    tmp = 0;
    for (i = 0; i < dataStructure->signalsCount; i++)
        if ((dataStructure->signalNodeIDs[i] - 1 == node) && ((slot < 0) || (dataStructure->signalSlot[i] == slot)))
            tmp++;

    newDS->signalsCount = static_cast<int>(tmp + newSignalsPayload.size());
    newDS->signalDeadlines = static_cast<int *>(mmalloc((tmp + newSignalsPayload.size()) * sizeof(int)));
    newDS->signalInFrameOffsets = static_cast<int *>(mmalloc((tmp + newSignalsPayload.size()) * sizeof(int)));
    newDS->signalLengths = static_cast<int *>(mmalloc((tmp + newSignalsPayload.size()) * sizeof(int)));
    newDS->signalNodeIDs = static_cast<int *>(mmalloc((tmp + newSignalsPayload.size()) * sizeof(int)));
    newDS->signalPeriods = static_cast<int *>(mmalloc((tmp + newSignalsPayload.size()) * sizeof(int)));
    newDS->signalReleaseDates = static_cast<int *>(mmalloc((tmp + newSignalsPayload.size()) * sizeof(int)));
    newDS->signalSlot = static_cast<int *>(mmalloc((tmp + newSignalsPayload.size()) * sizeof(int)));
    newDS->signalStartCycle = static_cast<int *>(mmalloc((tmp + newSignalsPayload.size()) * sizeof(int)));
    signalMapping = static_cast<int *>(mmalloc((tmp + newSignalsPayload.size()) * sizeof(int)));

    tmp = 0;
    for (i = 0; i < dataStructure->signalsCount; i++) {
        if ((dataStructure->signalNodeIDs[i] - 1 == node) && ((slot < 0) || (dataStructure->signalSlot[i] == slot))) {
            signalMapping[tmp] = i;
            newDS->signalDeadlines[tmp] = dataStructure->signalDeadlines[i];
            newDS->signalLengths[tmp] = dataStructure->signalLengths[i];
            newDS->signalNodeIDs[tmp] = dataStructure->signalNodeIDs[i];
            newDS->signalPeriods[tmp] = dataStructure->signalPeriods[i];
            newDS->signalReleaseDates[tmp] = dataStructure->signalReleaseDates[i];
            tmp++;
        }
    }
    result = tmp;
    for (i = 0; i < static_cast<int>(newSignalsPayload.size()); i++) {
        newDS->signalPeriods[tmp] = newSignalsPeriod[i];
        newDS->signalDeadlines[tmp] = newDS->signalPeriods[tmp];
        newDS->signalLengths[tmp] = newSignalsPayload[i];
        newDS->signalNodeIDs[tmp] = node;
        newDS->signalReleaseDates[tmp] = 0;
        signalMapping[tmp] = -1;
        tmp++;
    }

    // Create mutual matrix
    newDS->mutualExclusionMatrix = static_cast<char *>(ccalloc(newDS->signalsCount * (newDS->signalsCount + 1) / 2,
                                                               sizeof(char)));
    for (i = 0; i < newDS->signalsCount; i++) {
        for (j = i + 1; j < newDS->signalsCount; j++) {
            if (signalMapping[i] == -1 || signalMapping[j] == -1)
                setMEMatrixValue(newDS->mutualExclusionMatrix, i, j, 1);
            else
                setMEMatrixValue(newDS->mutualExclusionMatrix, i, j,
                                 getMEMatrixValue(dataStructure->mutualExclusionMatrix, signalMapping[i],
                                                  signalMapping[j]));
        }
    }
    return result;
}

int ReleaseNewDataStructure(DataStructure *newDS, int *signalMapping) {
    ffree(newDS->signalDeadlines);
    ffree(newDS->signalInFrameOffsets);
    ffree(newDS->signalLengths);
    ffree(newDS->signalNodeIDs);
    ffree(newDS->signalPeriods);
    ffree(newDS->signalReleaseDates);
    ffree(newDS->mutualExclusionMatrix);
    ffree(newDS->signalSlot);
    ffree(newDS->signalStartCycle);
    ffree(signalMapping);
    return 1;
}


int FirstFitColorScheduler(DataStructure *dataStructure, int *signalOrdering) {
    int i, j, k, sum, last;
    std::vector<CFrame> *schedule = new std::vector<CFrame>[dataStructure->hyperPeriod];
    int *aux = static_cast<int *>(mmalloc(dataStructure->signalsCount * sizeof(int)));
    int *nodeSlots = static_cast<int *>(mmalloc(dataStructure->nodeCount * sizeof(int)));

    mergeSort(signalOrdering, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator, NODE);
    ffree(aux);

    sum = 0;
    last = 0;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        for (j = 0; j < dataStructure->hyperPeriod; j++)
            schedule[j].clear();

        for (j = last; j < last + dataStructure->signalsInNodeCounts[i]; j++) {
            PlaceFFCSignalToSchedule(dataStructure, schedule, signalOrdering[j]);
        }

        nodeSlots[i] = static_cast<int>(schedule[0].size());
        sum += schedule[0].size();

        switch (dataStructure->isIncOptimized) {
            case 1:
                oneSlotExtensibilityOptimization(dataStructure, i, schedule);
                break;
            case 2:
                multiSlotExtensibilityOptimization(dataStructure, i, schedule);
                break;
            default:
                break;
        }
        last = last + dataStructure->signalsInNodeCounts[i];
    }

    int UB, LB, Opt, swap;
    int *slotAssignment = static_cast<int *>(mmalloc(sum * sizeof(int)));
    graphColoring(dataStructure, nodeSlots, &UB, &LB, &Opt, slotAssignment);
    dataStructure->maxSlot = Opt;
    last = nodeSlots[0];
    nodeSlots[0] = 0;
    for (i = 1; i <
                dataStructure->nodeCount; i++) // Kumulativn� sou�et - v nodeSlots pot� index prvn�ho slotu dan�ho nodu v slotAssignment
    {
        swap = nodeSlots[i];
        nodeSlots[i] = nodeSlots[i - 1] + last;
        last = swap;
    }

    // Update solution
    for (i = 0; i < dataStructure->signalsCount; i++) {
        dataStructure->signalSlot[i] = slotAssignment[nodeSlots[dataStructure->signalNodeIDs[i] - 1] +
                                                      dataStructure->signalSlot[i]];
    }
    if (verboseLevel > 0)
        printf("%d slots in schedule\n", Opt);
    ffree(slotAssignment);
    ffree(nodeSlots);
    delete[] schedule;
    return 1;
}

void oneSlotExtensibilityOptimization(DataStructure *dataStructure, const int node, std::vector<CFrame> *schedule) {
    std::vector<CFrame> *auxSchedule = new std::vector<CFrame>[dataStructure->hyperPeriod];
    for (int j = 0; j < static_cast<int>(schedule[0].size()); j++) {
        float tmp = ComputeSlotUtilization(dataStructure, j, schedule);
        while (tmp < 1) {
            int realSignals;
            int *signalMapping; // Mapovani indexu mezi puvodni DS a novou DS (z nove do puvodni)
            int *ordering;
            int *aux2;
            if (verboseLevel == 2) printf("%d, %d - %f\n", node, j, tmp);
            // Prepare new dataStructure for new OneSlot scheduling problem
            DataStructure ds;
            realSignals = PrepareNewDataStructure(dataStructure, &ds, signalMapping, node, j, 1 - tmp);
            if (realSignals > 0) {
                ordering = static_cast<int *>(mmalloc(ds.signalsCount * sizeof(int)));
                aux2 = static_cast<int *>(mmalloc(ds.signalsCount * sizeof(int)));
                for (int k = 0; k < ds.signalsCount; k++)
                    ordering[k] = k;
                mergeSort(ordering, aux2, &ds, 0, realSignals - 1, mergeSortSignalComparator, LENGTHDEC);
                mergeSort(ordering, aux2, &ds, 0, realSignals - 1, mergeSortSignalComparator, WINDOW);
                mergeSort(ordering, aux2, &ds, 0, ds.signalsCount - 1, mergeSortSignalComparator, PERIOD);
                for (int k = 0; k < dataStructure->hyperPeriod; k++)
                    auxSchedule[k].clear();
                // Create new OneSlot schedule
                for (int k = 0; k < ds.signalsCount; k++)
                    PlaceFFCSignalToSchedule(&ds, auxSchedule, ordering[k]);

                if (verboseLevel == 2)
                    printf("newut: %f, %d\n", ComputeSlotUtilization(&ds, 0, auxSchedule),
                           static_cast<int>(auxSchedule[0].size()));
                ffree(aux2);

                if (auxSchedule[0].size() > 1)
                    tmp = tmp * 1.05f;
                else {
                    // update original signals if the optimization is feasible;
                    for (int k = 0; k < ds.signalsCount; k++) {
                        if (signalMapping[k] != -1) {
                            dataStructure->signalStartCycle[signalMapping[k]] = ds.signalStartCycle[k];
                            dataStructure->signalInFrameOffsets[signalMapping[k]] = ds.signalInFrameOffsets[k];
                        }
                    }
                    tmp = 1;
                }
                ffree(ordering);
                ReleaseNewDataStructure(&ds, signalMapping);
            }
            else {
                ReleaseNewDataStructure(&ds, signalMapping);
                tmp = 1;
            }
        }
    }
}

void multiSlotExtensibilityOptimization(DataStructure *dataStructure, const int node, std::vector<CFrame> *schedule) {
    std::vector<CFrame> *auxSchedule = new std::vector<CFrame>[dataStructure->hyperPeriod];
    float tmp = ComputeMultiScheduleUtilization(dataStructure, schedule);
    const unsigned long scheduleLength = schedule[0].size();
    while (tmp < scheduleLength) {
        int realSignals;
        int *signalMapping; // Mapovani indexu mezi puvodni DS a novou DS (z nove do puvodni)
        int *ordering;
        int *aux2;
        if (verboseLevel == 2) printf("%d - %f\n", node, tmp);
        // Prepare new dataStructure for new OneSlot scheduling problem
        DataStructure ds;
        realSignals = PrepareNewDataStructure(dataStructure, &ds, signalMapping, node, -1, scheduleLength - tmp);
        if (realSignals > 0) {
            ordering = static_cast<int *>(mmalloc(ds.signalsCount * sizeof(int)));
            aux2 = static_cast<int *>(mmalloc(ds.signalsCount * sizeof(int)));
            for (int k = 0; k < ds.signalsCount; k++)
                ordering[k] = k;
            mergeSort(ordering, aux2, &ds, 0, realSignals - 1, mergeSortSignalComparator, LENGTHDEC);
            mergeSort(ordering, aux2, &ds, 0, realSignals - 1, mergeSortSignalComparator, WINDOW);
            mergeSort(ordering, aux2, &ds, 0, ds.signalsCount - 1, mergeSortSignalComparator, PERIOD);
            for (int k = 0; k < dataStructure->hyperPeriod; k++)
                auxSchedule[k].clear();
            // Create new OneSlot schedule
            for (int k = 0; k < ds.signalsCount; k++)
                PlaceFFCSignalToSchedule(&ds, auxSchedule, ordering[k]);

            if (verboseLevel == 2)
                printf("newut: %f, %d\n", ComputeSlotUtilization(&ds, 0, auxSchedule),
                       static_cast<int>(auxSchedule[0].size()));
            ffree(aux2);

            if (auxSchedule[0].size() > scheduleLength)
                tmp = tmp * 1.05f;
            else {
                // update original signals if the optimization is feasible;
                for (int k = 0; k < ds.signalsCount; k++) {
                    if (signalMapping[k] != -1) {
                        dataStructure->signalSlot[signalMapping[k]] = ds.signalSlot[k];
                        dataStructure->signalStartCycle[signalMapping[k]] = ds.signalStartCycle[k];
                        dataStructure->signalInFrameOffsets[signalMapping[k]] = ds.signalInFrameOffsets[k];
                    }
                }
                tmp = scheduleLength;
            }
            ffree(ordering);
            ReleaseNewDataStructure(&ds, signalMapping);
        }
        else {
            ReleaseNewDataStructure(&ds, signalMapping);
            tmp = scheduleLength;
        }
    }
}