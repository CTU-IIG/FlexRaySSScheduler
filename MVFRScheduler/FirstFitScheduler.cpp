/// \file FirstFitScheduler.cpp First Fit Scheduler
#include "MVFRScheduler.h"

int FindFreePositionInFrame(DataStructure *dataStructure, Frame *frame, int signalIndex, int startPosition) {
    int i, j, freeBytesInSequence, bestFoundOffset, last, currentFreeOffset;
    if (TestSignalFrameNodeIDCollision(dataStructure, frame, signalIndex))
        return -1;
    char *messagePositions = static_cast<char *>(ccalloc(dataStructure->slotLength, sizeof(char)));
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
    ffree(messagePositions);
    return bestFoundOffset;
}

int TestSignalFrameNodeIDCollision(DataStructure *dataStructure, Frame *frame, int signalIndex) {
    for (std::set<int>::iterator it = frame->nodes.begin(); it != frame->nodes.end(); ++it) {
        if (getMEMatrixValue(dataStructure->mutualExclusionNodeMatrix, *it - 1,
                             dataStructure->signalNodeIDs[signalIndex] - 1)) {
            return 1;
        }
    }
    return 0;
}

int TestSignalInFrame(DataStructure *dataStructure, Frame *frame, int inFrameOffset, int signalIndex) {
    int i;
    if (TestSignalFrameNodeIDCollision(dataStructure, frame, signalIndex))
        return 0;
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

Position FindFirstCyclePositionForSignal(DataStructure *dataStructure, std::vector<Frame> *schedule, int signalIndex,
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
            inFramePosition = FindFreePositionInFrame(dataStructure, &schedule[j][i],
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

int PlaceSignalToFrame(DataStructure *dataStructure, std::vector<Frame> *schedule, Position position, int signalIndex) {
    int i;
    dataStructure->signalSlot[signalIndex] = position.slot;
    dataStructure->signalStartCycle[signalIndex] = position.cycle;
    dataStructure->signalInFrameOffsets[signalIndex] = position.inFrameOffset;
    for (i = 0; i < dataStructure->hyperPeriod; i++) {
        schedule[i][position.slot].nodes.insert(dataStructure->signalNodeIDs[signalIndex]);
        if (i % dataStructure->signalPeriods[signalIndex] == position.cycle)
            schedule[i][position.slot].signals.push_back(signalIndex);
    }
    return 0;
}

int PlaceFFSignalToSchedule(DataStructure *dataStructure, std::vector<Frame> *schedule, int signalIndex) {
    int i;
    Position startPosition, placePosition;
    char infeasablePosition = 1;
    startPosition.inFrameOffset = 0;
    startPosition.cycle = dataStructure->signalReleaseDates[signalIndex];
    startPosition.slot = 0;

    while (infeasablePosition) {
        placePosition = FindFirstCyclePositionForSignal(dataStructure, schedule, signalIndex, startPosition);
        if (placePosition.cycle == -1 || placePosition.inFrameOffset == -1 || placePosition.slot == -1)
            break;

        infeasablePosition = 0;
        for (i = 1; i < dataStructure->hyperPeriod / dataStructure->signalPeriods[signalIndex]; i++) {
            if (!TestSignalInFrame(dataStructure,
                                   &schedule[placePosition.cycle +
                                             i * dataStructure->signalPeriods[signalIndex]][placePosition.slot],
                                   placePosition.inFrameOffset, signalIndex)) {
                infeasablePosition = 1;
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
        if (!infeasablePosition) {
            PlaceSignalToFrame(dataStructure, schedule, placePosition, signalIndex);
        }
    }
    if (infeasablePosition) // Create new slot
    {
        Frame f;
        dataStructure->signalStartCycle[signalIndex] = dataStructure->signalReleaseDates[signalIndex];
        dataStructure->signalSlot[signalIndex] = static_cast<int>(schedule[0].size());
        dataStructure->signalInFrameOffsets[signalIndex] = 0;
        for (i = 0; i < dataStructure->hyperPeriod; i++) {
            schedule[i].push_back(f);
            schedule[i][schedule[i].size() - 1].nodes.insert(dataStructure->signalNodeIDs[signalIndex]);
            if (i % dataStructure->signalPeriods[signalIndex] == dataStructure->signalReleaseDates[signalIndex])
                schedule[i][schedule[i].size() - 1].signals.push_back(signalIndex);
        }
    }

    return 1;
}

int FirstFitScheduler(DataStructure *dataStructure, int *signalOrdering) {
    int i;
    std::vector<Frame> *schedule = new std::vector<Frame>[dataStructure->hyperPeriod];
    for (i = 0; i < dataStructure->signalsCount; i++)
        PlaceFFSignalToSchedule(dataStructure, schedule, signalOrdering[i]);
    dataStructure->maxSlot = static_cast<int>(schedule[0].size());
    if (verboseLevel > 0)
        printf("%d slots in schedule\n", static_cast<int>(schedule[0].size()));
    delete[] schedule;
    return 1;
}
