/// \file FirstFitVariantScheduler.cpp Naive First fit scheduler
#include "MVFRScheduler.h"


int VariantFindFreePositionInFrame(DataStructure *dataStructure, VariantFrame *frame, int signalIndex,
                                   int startPosition) {
    int i, j, freeBytesInSequence, bestFoundOffset, last, currentFreeOffset;
    if ((frame->node != dataStructure->signalNodeIDs[signalIndex]) && (frame->node != -1))
        return -1;
    char *messagePositions = (char *) ccalloc(dataStructure->slotLength, sizeof(char));
    for (i = 0; i < (int) frame->signals.size(); i++) // For each signal in frame
    {
        for (j = dataStructure->signalInFrameOffsets[frame->signals[i]];
             j < dataStructure->signalInFrameOffsets[frame->signals[i]] +
                 dataStructure->signalLengths[frame->signals[i]]; j++) {
            messagePositions[j] = 1;
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
    ffree(messagePositions);
    return bestFoundOffset;
}

int VariantTestSignalInFrame(DataStructure *dataStructure, VariantFrame *frame, int inFrameOffset, int signalIndex) {
    int i;
    if ((frame->node != dataStructure->signalNodeIDs[signalIndex]) && (frame->node != -1))
        return 0;
    for (i = 0; i < (int) frame->signals.size(); i++) {
        if (!((inFrameOffset >= dataStructure->signalInFrameOffsets[frame->signals[i]] +
                                dataStructure->signalLengths[frame->signals[i]]) ||
              (inFrameOffset + dataStructure->signalLengths[signalIndex] <=
               dataStructure->signalInFrameOffsets[frame->signals[i]])))
            return 0;
    }
    return 1;
}

Position VariantFindFirstCyclePositionForSignal(DataStructure *dataStructure, std::vector<VariantFrame> *schedule,
                                                int signalIndex,
                                                Position startPosition) {
    int i, j;
    int inFramePosition;
    char init = 1;
    Position resultPosition;
    resultPosition.cycle = -1;
    resultPosition.inFrameOffset = -1;
    resultPosition.slot = -1;
    for (i = startPosition.slot; i < (int) schedule[0].size(); i++) // Slots
    {
        for (j = init ? startPosition.cycle : dataStructure->signalReleaseDates[signalIndex];
             j < dataStructure->signalDeadlines[signalIndex]; j++) // Cycles
        {
            inFramePosition = VariantFindFreePositionInFrame(dataStructure, &schedule[j][i],
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

void assureScheduleHasEnoughSlots(DataStructure *dataStructure, std::vector<VariantFrame> *schedule, int slots) {
    int i;
    VariantFrame vf;
    vf.node = -1;
    while (slots >= (int) schedule[0].size()) {
        for (i = 0; i < dataStructure->hyperPeriod; i++) {
            schedule[i].push_back(vf);
        }
    }
}

int VariantPlaceSignalToFrame(DataStructure *dataStructure, std::vector<VariantFrame> **schedule, Position position,
                              int signalIndex) {
    int i, j;
    dataStructure->signalSlot[signalIndex] = position.slot;
    dataStructure->signalStartCycle[signalIndex] = position.cycle;
    dataStructure->signalInFrameOffsets[signalIndex] = position.inFrameOffset;
    for (i = 0; i < dataStructure->variantCount; i++) {
        if (dataStructure->modelVariants[i][signalIndex]) {
            assureScheduleHasEnoughSlots(dataStructure, schedule[i], position.slot);
            for (j = 0; j < dataStructure->hyperPeriod / dataStructure->signalPeriods[signalIndex]; j++) {
                schedule[i][position.cycle +
                            j * dataStructure->signalPeriods[signalIndex]][position.slot].signals.push_back(
                        signalIndex);
            }
            if (schedule[i][0][position.slot].node == -1) {
                for (j = 0; j < dataStructure->hyperPeriod; j++) {
                    schedule[i][j][position.slot].node = dataStructure->signalNodeIDs[signalIndex];
                }
            }
        }
    }
    return 0;
}

int getVariantUsingSignalWithMaxSlotUsed(DataStructure *dataStructure, int signalIndex,
                                         std::vector<VariantFrame> **schedule) {
    int i;
    int result = -1;
    int used = -1;
    for (i = 0; i < dataStructure->variantCount; i++) {
        if (dataStructure->modelVariants[i][signalIndex]) {
            if (used < (int) schedule[i][0].size()) {
                used = (int) schedule[i][0].size();
                result = i;
            }
        }
    }
    return result;
}

int PlaceFFVSignalToSchedule(DataStructure *dataStructure, std::vector<VariantFrame> **schedule, int signalIndex) {
    int i, j;
    Position startPosition, placePosition;
    char infeasiblePosition = 1;
    int firstVariant = getVariantUsingSignalWithMaxSlotUsed(dataStructure, signalIndex, schedule);
    if (firstVariant == -1)
        return 1;
    startPosition.inFrameOffset = 0;
    startPosition.cycle = dataStructure->signalReleaseDates[signalIndex];
    startPosition.slot = 0;

    while (infeasiblePosition) {
        //if(signalIndex == 34 && startPosition.slot == 1)
        //printf("g");
        placePosition = VariantFindFirstCyclePositionForSignal(dataStructure, schedule[firstVariant], signalIndex,
                                                               startPosition);
        if (placePosition.cycle == -1 || placePosition.inFrameOffset == -1 || placePosition.slot == -1)
            break;

        infeasiblePosition = 0;
        for (j = firstVariant; (j < dataStructure->variantCount) && (!infeasiblePosition); j++) {
            if (placePosition.slot >= (int) schedule[j][0].size())
                continue;
            if (dataStructure->modelVariants[j][signalIndex]) {
                for (i = ((j == firstVariant) ? 1 : 0);
                     i < dataStructure->hyperPeriod / dataStructure->signalPeriods[signalIndex]; i++) {
                    if (!VariantTestSignalInFrame(dataStructure,
                                                  &schedule[j][placePosition.cycle + i *
                                                                                     dataStructure->signalPeriods[signalIndex]][placePosition.slot],
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
        }
        if (!infeasiblePosition) {
            VariantPlaceSignalToFrame(dataStructure, schedule, placePosition, signalIndex);
        }
    }
    if (infeasiblePosition) // Create new slot
    {
        VariantFrame vf;
        dataStructure->signalStartCycle[signalIndex] = dataStructure->signalReleaseDates[signalIndex];
        dataStructure->signalSlot[signalIndex] = static_cast<int>(schedule[firstVariant][0].size());
        dataStructure->signalInFrameOffsets[signalIndex] = 0;
        for (j = 0; j < dataStructure->variantCount; j++) {
            if (dataStructure->modelVariants[j][signalIndex]) {
                assureScheduleHasEnoughSlots(dataStructure, schedule[j], dataStructure->signalSlot[signalIndex] - 1);
                for (i = 0; i < dataStructure->hyperPeriod; i++) {
                    schedule[j][i].push_back(vf);
                    schedule[j][i][schedule[j][i].size() - 1].node = dataStructure->signalNodeIDs[signalIndex];
                    if (i % dataStructure->signalPeriods[signalIndex] == dataStructure->signalReleaseDates[signalIndex])
                        schedule[j][i][schedule[j][i].size() - 1].signals.push_back(signalIndex);
                }
            }
        }
    }

    return 1;
}

int FirstFitVariantScheduler(DataStructure *dataStructure, int *signalOrdering) {
    int i;
    std::vector<VariantFrame> **schedule = new std::vector<VariantFrame> *[dataStructure->variantCount];
    for (i = 0; i < dataStructure->variantCount; i++)
        schedule[i] = new std::vector<VariantFrame>[dataStructure->hyperPeriod];

    for (i = 0; i < dataStructure->signalsCount; i++)
        PlaceFFVSignalToSchedule(dataStructure, schedule, signalOrdering[i]);

    for (i = 0; i < dataStructure->variantCount; i++) {
        dataStructure->maxSlot = (dataStructure->maxSlot < static_cast<int>(schedule[i][0].size()))
                                 ? static_cast<int>(schedule[i][0].size()) : dataStructure->maxSlot;
        delete[] schedule[i];
    }

    if (verboseLevel > 0)
        printf("%d slots in schedule\n", dataStructure->maxSlot);

    delete[] schedule;
    return 1;
}
