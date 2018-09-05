/// \file StageScheduler.cpp Two stage scheduler
#include "MVFRScheduler.h"

EvaluationResult EvaluateAssignmentToMessage(int signalIndex, DataStructure *dataStructure, Message *message) {
    EvaluationResult result;
    result.penalization = -1;
    int i, j, freeBytesInSequence, bestFreeBytesInSequence, bestFoundOffset, last, currentFreeOffset;
    if (message->deadline > dataStructure->signalDeadlines[signalIndex] ||
        message->releaseTime < dataStructure->signalReleaseDates[signalIndex])
        return result;

    char *messagePositions = (char *) ccalloc(static_cast<size_t>(dataStructure->slotLength), sizeof(char));
    for (i = 0; i < (int) message->signals.size(); i++) {
        if (getMEMatrixValue(dataStructure->mutualExclusionMatrix, message->signals[i], signalIndex)) {
            for (j = dataStructure->signalInFrameOffsets[message->signals[i]];
                 j < dataStructure->signalInFrameOffsets[message->signals[i]] +
                     dataStructure->signalLengths[message->signals[i]]; j++) {
                messagePositions[j] = 1;
            }
        }
    }
    //for(i = 0; i < dataStructure->slotLength; i++)
    //	printf("%d ", messagePositions[i]);
    //printf("\n");
    bestFoundOffset = -1;
    freeBytesInSequence = 0;
    bestFreeBytesInSequence = dataStructure->slotLength + 1;
    last = 1;
    currentFreeOffset = 0;
    for (i = 0; i < dataStructure->slotLength; i++) {
        if (messagePositions[i] == 1) {
            if (freeBytesInSequence != 0 && bestFreeBytesInSequence > freeBytesInSequence &&
                freeBytesInSequence >= dataStructure->signalLengths[signalIndex]) {
                bestFreeBytesInSequence = freeBytesInSequence;
                bestFoundOffset = currentFreeOffset;
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

    if (freeBytesInSequence != 0 && bestFreeBytesInSequence == dataStructure->slotLength + 1 &&
        freeBytesInSequence >= dataStructure->signalLengths[signalIndex]) {
        bestFreeBytesInSequence = freeBytesInSequence;
        bestFoundOffset = currentFreeOffset;
    }

    if (bestFoundOffset != -1) {
        result.bestOffsetInMessage = bestFoundOffset;
        result.penalization = (bestFreeBytesInSequence - dataStructure->signalLengths[signalIndex]) +
                              10 * (abs(dataStructure->signalDeadlines[signalIndex] - message->deadline) +
                                    abs(dataStructure->signalReleaseDates[signalIndex] - message->releaseTime));
        result.message = message;
    }
    ffree(messagePositions);
    return result;
}

int PlaceSignalToMessage(int signalIndex, DataStructure *dataStructure, std::vector<Message> &messages,
                         int firstMessageWithThisPeriod) {
    int i;
    EvaluationResult tmpEvaluation, bestEvaluation;
    bestEvaluation.penalization = -1;
    for (i = firstMessageWithThisPeriod; i < (int) messages.size(); i++) {
        tmpEvaluation = EvaluateAssignmentToMessage(signalIndex, dataStructure, &messages[i]);
        if (tmpEvaluation.penalization != -1) {
            if (bestEvaluation.penalization > bestEvaluation.penalization || bestEvaluation.penalization == -1)
                bestEvaluation = tmpEvaluation;
        }
    }
    //if(bestEvaluation.penalization != -1)
    if (bestEvaluation.penalization != -1 &&
        bestEvaluation.penalization <
        (static_cast<StageParameters *>(dataStructure->schedulerParameters))->signalAssigningThreshold) { // Assign signal to best found message
        bestEvaluation.message->signals.push_back(signalIndex);
        bestEvaluation.message->deadline =
                bestEvaluation.message->deadline < dataStructure->signalDeadlines[signalIndex] ?
                bestEvaluation.message->deadline : dataStructure->signalDeadlines[signalIndex];
        bestEvaluation.message->releaseTime =
                bestEvaluation.message->releaseTime > dataStructure->signalReleaseDates[signalIndex] ?
                bestEvaluation.message->releaseTime : dataStructure->signalReleaseDates[signalIndex];
        bestEvaluation.message->length = bestEvaluation.message->length >
                                         dataStructure->signalLengths[signalIndex] + bestEvaluation.bestOffsetInMessage
                                         ?
                                         bestEvaluation.message->length : dataStructure->signalLengths[signalIndex] +
                                                                          bestEvaluation.bestOffsetInMessage;
        dataStructure->signalInFrameOffsets[signalIndex] = bestEvaluation.bestOffsetInMessage;
    }
    else { // Create new message
        Message newMessage = {};
        newMessage.signals.push_back(signalIndex);
        newMessage.deadline = dataStructure->signalDeadlines[signalIndex];
        newMessage.releaseTime = dataStructure->signalReleaseDates[signalIndex];
        newMessage.period = dataStructure->signalPeriods[signalIndex];
        newMessage.length = dataStructure->signalLengths[signalIndex];
        dataStructure->signalInFrameOffsets[signalIndex] = 0;
        messages.push_back(newMessage);
    }

    return 0;
}

int FramePacking(DataStructure *dataStructure, std::vector<Message> &messages, int firstSignal, int node) {
    int i;
    int firstMessageWithThisPeriod = 0;
    int *sortArray = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    int *sortAuxArray = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    for (i = firstSignal; i < (firstSignal + dataStructure->signalsInNodeCounts[node]); i++)
        sortArray[i] = i;
    mergeSort(sortArray, sortAuxArray, dataStructure, firstSignal,
              firstSignal + dataStructure->signalsInNodeCounts[node] - 1, mergeSortSignalComparator, LENGTHDEC);
    mergeSort(sortArray, sortAuxArray, dataStructure, firstSignal,
              firstSignal + dataStructure->signalsInNodeCounts[node] - 1, mergeSortSignalComparator, WINDOW);
    mergeSort(sortArray, sortAuxArray, dataStructure, firstSignal,
              firstSignal + dataStructure->signalsInNodeCounts[node] - 1, mergeSortSignalComparator, PERIOD);
    ffree(sortAuxArray);

    for (i = firstSignal; i < (firstSignal + dataStructure->signalsInNodeCounts[node]); i++) {
        PlaceSignalToMessage(sortArray[i], dataStructure, messages, firstMessageWithThisPeriod);
    }

    if (verboseLevel > 0)
        printf("In node %d was %d signals packed to %d messages\n", node + 1, dataStructure->signalsInNodeCounts[node],
               static_cast<int>(messages.size()));

    ffree(sortArray);
    return 0;
}

int PlaceMessageToSchedule(DataStructure *dataStructure, std::vector<Message> &messages, std::vector<int> *schedule,
                           std::vector<double> &occupiedSlotCapacity, int maxSlot,
                           int messageIndex) {
    int i, j, k;
    char admissible;
    for (i = 0; i < maxSlot; i++) // slot
    {
        if (occupiedSlotCapacity[i] + messages[messageIndex].length / (double) (messages[messageIndex].period) <=
            dataStructure->slotLength) {
            for (j = messages[messageIndex].releaseTime; j < messages[messageIndex].deadline; j++) // cycle
            {
                admissible = 1;
                for (k = 0; k < dataStructure->hyperPeriod / messages[messageIndex].period; k++) // job
                {
                    if (schedule[j + k * messages[messageIndex].period][i] + messages[messageIndex].length >
                        dataStructure->slotLength) {
                        admissible = 0;
                        break;
                    }
                }
                if (admissible) {
                    messages[messageIndex].startCycle = j;
                    messages[messageIndex].slot = i;
                    for (k = 0; k < dataStructure->hyperPeriod / messages[messageIndex].period; k++) // job
                    {
                        schedule[j + k * messages[messageIndex].period][i] += messages[messageIndex].length;
                    }
                    occupiedSlotCapacity[i] += messages[messageIndex].length / (double) messages[messageIndex].period;
                    return 1; // Message is placed
                }
            }
        }
    }
    return 0; // No free position for this message in any slot
}

int RegisterNextUsableSlot(DataStructure *dataStructure, std::vector<std::vector<int> > &slotNodeUsage, int node,
                           int firstSlot) {
    int actualSlot = -1;
    int i, j;
    for (i = firstSlot;
         i < (int) slotNodeUsage.size(); i++) // Searching for the first slot that can be used for our messages
    {
        actualSlot = i;
        for (j = 0; j < (int) slotNodeUsage[i].size(); j++) {
            if (getMEMatrixValue(dataStructure->mutualExclusionNodeMatrix, node, slotNodeUsage[i][j])) {
                actualSlot = -1;
                break;
            }
        }
        if (actualSlot != -1)
            break;
    }
    if (actualSlot == -1) // if there is no empty slot create new one
    {
        actualSlot = static_cast<int>(slotNodeUsage.size());
        std::vector<int> slotVector;
        slotVector.push_back(node);
        slotNodeUsage.push_back(slotVector);
    }
    else {
        slotNodeUsage[actualSlot].push_back(node);
    }
    return actualSlot;
}

int MessageScheduling(DataStructure *dataStructure, std::vector<Message> &messages,
                      std::vector<std::vector<int> > &slotNodeUsage, int node) {
    int i, j;
    int maxSlot = 0; // relative slot in schedule of current node
    std::vector<double> occupiedSlotCapacity;
    std::vector<int> *schedule = new std::vector<int>[dataStructure->hyperPeriod];
    std::vector<int> relativeToAbsoluteSlotTranslation;
    int *messagesIndex = (int *) mmalloc(messages.size() * sizeof(int));

    for (i = 0; i < (int) messages.size(); i++) {
        messagesIndex[i] = i;
    }
    int *messagesArrayAux = (int *) mmalloc(messages.size() * sizeof(int));
    mergeSort(messagesIndex, messagesArrayAux, &messages, 0, static_cast<int>(messages.size() - 1),
              mergeSortMessageComparator, RELEASEDATE);
    mergeSort(messagesIndex, messagesArrayAux, &messages, 0, static_cast<int>(messages.size() - 1),
              mergeSortMessageComparator, WINDOW);
    for (i = 0; i < (int) messages.size(); i++) // message
    {
        if (!PlaceMessageToSchedule(dataStructure, messages, schedule, occupiedSlotCapacity, maxSlot,
                                    messagesIndex[i])) {
            if (maxSlot == 0) {
                relativeToAbsoluteSlotTranslation.push_back(
                        RegisterNextUsableSlot(dataStructure, slotNodeUsage, node, 0));
            }
            else {
                relativeToAbsoluteSlotTranslation.push_back(
                        RegisterNextUsableSlot(dataStructure, slotNodeUsage, node,
                                               relativeToAbsoluteSlotTranslation[maxSlot - 1] + 1));
            }
            occupiedSlotCapacity.push_back(
                    messages[messagesIndex[i]].length / (double) messages[messagesIndex[i]].period);
            messages[messagesIndex[i]].slot = maxSlot;
            messages[messagesIndex[i]].startCycle = messages[messagesIndex[i]].releaseTime;
            messages[messagesIndex[i]].offsetInFrame = 0;
            for (j = 0; j < dataStructure->hyperPeriod; j++) {
                if (j % messages[messagesIndex[i]].period == messages[messagesIndex[i]].releaseTime)
                    schedule[j].push_back(messages[messagesIndex[i]].length);
                else
                    schedule[j].push_back(0);
            }
            maxSlot++;
        }
    }

    mergeSort(messagesIndex, messagesArrayAux, &messages, 0, static_cast<int>(messages.size() - 1),
              mergeSortMessageComparator, PERIOD);
    mergeSort(messagesIndex, messagesArrayAux, &messages, 0, static_cast<int>(messages.size() - 1),
              mergeSortMessageComparator, SLOT);
    for (i = 0; i < dataStructure->hyperPeriod; i++)
        for (j = 0; j < maxSlot; j++)
            schedule[i][j] = 0;
    for (i = 0; i < (int) messages.size(); i++) {
        messages[messagesIndex[i]].offsetInFrame = schedule[messages[messagesIndex[i]].startCycle][messages[messagesIndex[i]].slot];
        for (j = 0; j < dataStructure->hyperPeriod / messages[messagesIndex[i]].period; j++) {
            if (schedule[messages[messagesIndex[i]].startCycle +
                         messages[messagesIndex[i]].period * j][messages[messagesIndex[i]].slot] >
                messages[messagesIndex[i]].offsetInFrame)
                error("Infeasible scheduling algorithm!!!!!!!");
            schedule[messages[messagesIndex[i]].startCycle +
                     messages[messagesIndex[i]].period * j][messages[messagesIndex[i]].slot] =
                    messages[messagesIndex[i]].offsetInFrame + messages[messagesIndex[i]].length;
        }
        messages[messagesIndex[i]].slot = relativeToAbsoluteSlotTranslation[messages[messagesIndex[i]].slot];
    }

    ffree(messagesArrayAux);
    ffree(messagesIndex);
    delete[] schedule;
    return maxSlot;
}

void SignalConfiguring(DataStructure *dataStructure, std::vector<Message> &messages) {
    int i, j;
    for (i = 0; i < (int) messages.size(); i++) {
        for (j = 0; j < (int) messages[i].signals.size(); j++) {
            dataStructure->signalSlot[messages[i].signals[j]] = messages[i].slot;
            dataStructure->signalStartCycle[messages[i].signals[j]] = messages[i].startCycle;
            dataStructure->signalInFrameOffsets[messages[i].signals[j]] =
                    dataStructure->signalInFrameOffsets[messages[i].signals[j]]
                    + messages[i].offsetInFrame;
        }
    }
}

int NodeScheduler(DataStructure *dataStructure, int node, std::vector<std::vector<int> > &slotNodeUsage) {
    if (verboseLevel > 0)
        printf("Node %d scheduler started\n", node + 1);

    std::vector<Message> messages;
    int firstSignal = 0;
    int i;
    int scheduledSlots;
    for (i = 0; i < node; i++)
        firstSignal += dataStructure->signalsInNodeCounts[i];
    FramePacking(dataStructure, messages, firstSignal, node);
    scheduledSlots = MessageScheduling(dataStructure, messages, slotNodeUsage, node);
    SignalConfiguring(dataStructure, messages);
    return scheduledSlots;
}

int StageScheduler(DataStructure *dataStructure) {
    int i;

    // Se�azen� dle nodu
    int j, k;
    int *sortArray = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    int *sortAuxArray = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    for (i = 0; i < dataStructure->signalsCount; i++)
        sortArray[i] = i;
    mergeSort(sortArray, sortAuxArray, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
              NODE);
    ffree(sortAuxArray);
    int *signalDeadlines = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    int *signalLengths = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    int *signalNodeIDs = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    int *signalPeriods = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    int *signalReleaseDates = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    char **modelVariants = static_cast<char **>(mmalloc(sizeof(char *) * dataStructure->variantCount));
    for (i = 0; i < dataStructure->variantCount; i++)
        modelVariants[i] = static_cast<char *>(mmalloc(sizeof(char) * dataStructure->signalsCount));

    for (i = 0; i < dataStructure->signalsCount; i++) {
        signalDeadlines[i] = dataStructure->signalDeadlines[sortArray[i]];
        signalLengths[i] = dataStructure->signalLengths[sortArray[i]];
        signalNodeIDs[i] = dataStructure->signalNodeIDs[sortArray[i]];
        signalPeriods[i] = dataStructure->signalPeriods[sortArray[i]];
        signalReleaseDates[i] = dataStructure->signalReleaseDates[sortArray[i]];
        for (j = 0; j < dataStructure->variantCount; j++) {
            modelVariants[j][i] = dataStructure->modelVariants[j][sortArray[i]];
        }
    }

    for (i = 0; i < dataStructure->variantCount; i++) {
        ffree(dataStructure->modelVariants[i]);
        dataStructure->modelVariants[i] = modelVariants[i];
    }
    ffree(modelVariants);
    ffree(sortArray);
    ffree(dataStructure->signalDeadlines);
    dataStructure->signalDeadlines = signalDeadlines;
    ffree(dataStructure->signalLengths);
    dataStructure->signalLengths = signalLengths;
    ffree(dataStructure->signalNodeIDs);
    dataStructure->signalNodeIDs = signalNodeIDs;
    ffree(dataStructure->signalPeriods);
    dataStructure->signalPeriods = signalPeriods;
    ffree(dataStructure->signalReleaseDates);
    dataStructure->signalReleaseDates = signalReleaseDates;
    ffree(dataStructure->mutualExclusionMatrix);
    ffree(dataStructure->mutualExclusionNodeMatrix);

    //Create mutual exclusion matrix
    dataStructure->mutualExclusionMatrix = static_cast<char *>(ccalloc(
            static_cast<size_t>(dataStructure->signalsCount * (dataStructure->signalsCount + 1) / 2), sizeof(char)));
    dataStructure->mutualExclusionNodeMatrix = static_cast<char *>(ccalloc(
            static_cast<size_t>(dataStructure->nodeCount * (dataStructure->nodeCount + 1) / 2), sizeof(char)));
    for (i = 0; i < dataStructure->variantCount; i++) {
        for (j = 0; j < dataStructure->signalsCount; j++) {
            for (k = j + 1; k < dataStructure->signalsCount; k++) {
                if (dataStructure->modelVariants[i][j] == 1 && dataStructure->modelVariants[i][k] == 1) {
                    setMEMatrixValue(dataStructure->mutualExclusionMatrix, j, k, 1);
                    setMEMatrixValue(dataStructure->mutualExclusionNodeMatrix,
                                     dataStructure->signalNodeIDs[j] - 1,
                                     dataStructure->signalNodeIDs[k] - 1, 1);

                }
            }
        }
    }

    std::vector<std::vector<int> > slotNodeUsage; // Which nodes uses particular slot
    for (i = 0; i < dataStructure->nodeCount; i++) {
        NodeScheduler(dataStructure, i, slotNodeUsage);
    }
    dataStructure->maxSlot = static_cast<int>(slotNodeUsage.size());
    if (verboseLevel > 0)
        printf("Used %d slots in total. %d is a trivial lower bound.\n", dataStructure->maxSlot,
               ComputeBetterLowerBound(dataStructure));
    return 0;
}
