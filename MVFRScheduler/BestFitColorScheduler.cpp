/// \file BestFitColorScheduler.cpp Improved First Fit Scheduler that uses Graph coloring for Slot scheduling and best-fit searching policy
#include "MVFRScheduler.h"


int BFindFreePositionInFrame(DataStructure *dataStructure, CFrame *frame, int signalIndex, int startPosition,
                             int *evaluation) {
    int i, j, freeBytesInSequence, bestFoundOffset, last, currentFreeOffset;
    char *messagePositions = static_cast<char *>(ccalloc(static_cast<size_t>(dataStructure->slotLength), sizeof(char)));
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

    // Výpočet evaluační funkce
    int emptyOverall = 0;
    int biggestSpace = 0;
    int smallestSpace = dataStructure->slotLength;
    if (bestFoundOffset != -1) {
        freeBytesInSequence = 0;
        last = 1;
        for (i = bestFoundOffset; i < bestFoundOffset + dataStructure->signalLengths[signalIndex]; i++)
            messagePositions[i] = 1;
        for (i = 0; i < dataStructure->slotLength; i++) {
            if (messagePositions[i] == 1) {
                if (freeBytesInSequence > biggestSpace) {
                    biggestSpace = freeBytesInSequence;
                }
                if (last == 0 && freeBytesInSequence < smallestSpace) {
                    smallestSpace = freeBytesInSequence;
                }
                freeBytesInSequence = 0;
                last = 1;
            }
            else {
                emptyOverall++;
                freeBytesInSequence++;
                last = 0;
            }
        }
        if (freeBytesInSequence > biggestSpace) biggestSpace = freeBytesInSequence;
        if (freeBytesInSequence < smallestSpace && last == 0) smallestSpace = freeBytesInSequence;
    }
    *evaluation =
            10 * biggestSpace + 3 * emptyOverall + 12 * smallestSpace + dataStructure->slotLength - bestFoundOffset;
    ffree(messagePositions);
    return bestFoundOffset;
}

int BTestSignalInFrame(DataStructure *dataStructure, CFrame *frame, int inFrameOffset, int signalIndex) {
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

BPosition BFindFirstCyclePositionForSignal(DataStructure *dataStructure, std::vector<CFrame> *schedule, int signalIndex,
                                           BPosition startPosition) {
    int i, j;
    int inFramePosition;
    char init = 1;
    int evaluation;
    BPosition resultPosition;
    resultPosition.cycle = -1;
    resultPosition.inFrameOffset = -1;
    resultPosition.slot = -1;
    resultPosition.evaluation = -1;
    for (i = startPosition.slot; i < static_cast<int>(schedule[0].size()); i++) // Slots
    {
        for (j = init ? startPosition.cycle : dataStructure->signalReleaseDates[signalIndex];
             j < dataStructure->signalDeadlines[signalIndex]; j++) // Cycles
        {
            inFramePosition = BFindFreePositionInFrame(dataStructure, &schedule[j][i],
                                                       signalIndex, init ? startPosition.inFrameOffset : 0,
                                                       &evaluation);
            if (inFramePosition != -1) {
                resultPosition.inFrameOffset = inFramePosition;
                resultPosition.cycle = j;
                resultPosition.slot = i;
                resultPosition.evaluation = evaluation + 20 * (dataStructure->hyperPeriod - resultPosition.cycle) +
                                            40 * (static_cast<int>(schedule[0].size()) - resultPosition.slot);
                return resultPosition;
            }
        }
        init = 0;
    }
    return resultPosition;
}

int BPlaceSignalToFrame(DataStructure *dataStructure, std::vector<CFrame> *schedule, BPosition position,
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

int PlaceBFCSignalToSchedule(DataStructure *dataStructure, std::vector<CFrame> *schedule, int signalIndex) {
    int i;
    BPosition startPosition, placePosition, bestPosition;
    char infeasiblePosition;
    startPosition.inFrameOffset = 0;
    startPosition.cycle = dataStructure->signalReleaseDates[signalIndex];
    startPosition.slot = 0;
    startPosition.evaluation = 0;
    bestPosition.evaluation = -1;

    while (1) {
        placePosition = BFindFirstCyclePositionForSignal(dataStructure, schedule, signalIndex, startPosition);
        if (placePosition.cycle == -1 || placePosition.inFrameOffset == -1 || placePosition.slot == -1)
            break;

        infeasiblePosition = 0;
        if (static_cast<FirstFitParameters *>(dataStructure->schedulerParameters)->sortType != FFALL
            && static_cast<FirstFitParameters *>(dataStructure->schedulerParameters)->sortType != FFPERIOD) {
            for (i = 1; i < dataStructure->hyperPeriod / dataStructure->signalPeriods[signalIndex]; i++) {
                if (!BTestSignalInFrame(dataStructure,
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
            if (placePosition.evaluation > bestPosition.evaluation) {
                bestPosition.cycle = placePosition.cycle;
                bestPosition.evaluation = placePosition.evaluation;
                bestPosition.inFrameOffset = placePosition.inFrameOffset;
                bestPosition.slot = placePosition.slot;
            }
            startPosition.inFrameOffset = 0;
            startPosition.cycle = (placePosition.cycle + 1) % dataStructure->hyperPeriod;
            startPosition.slot = placePosition.slot;
            if (startPosition.cycle == 0)
                startPosition.slot++;
        }
    }
    if (bestPosition.evaluation != -1) {

        CFrame cF;
        BPlaceSignalToFrame(dataStructure, schedule, bestPosition, signalIndex);
    }
    else // Create new slot
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

int BestFitColorScheduler(DataStructure *dataStructure, int *signalOrdering) {
    int i, j, sum, last;
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

        for (j = last; j < last + dataStructure->signalsInNodeCounts[i]; j++)
            PlaceBFCSignalToSchedule(dataStructure, schedule, signalOrdering[j]);

        nodeSlots[i] = static_cast<int>(schedule[0].size());
        sum += schedule[0].size();
        last = last + dataStructure->signalsInNodeCounts[i];
    }

    int UB, LB, Opt, swap;
    int *slotAssignment = static_cast<int *>(mmalloc(sum * sizeof(int)));
    graphColoring(dataStructure, nodeSlots, &UB, &LB, &Opt, slotAssignment);
    dataStructure->maxSlot = Opt;
    last = nodeSlots[0];
    nodeSlots[0] = 0;
    for (i = 1; i <
                dataStructure->nodeCount; i++) // Kumulativní součet - v nodeSlots poté index prvního slotu daného nodu v slotAssignment
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
    delete[] schedule;
    return 1;
}
