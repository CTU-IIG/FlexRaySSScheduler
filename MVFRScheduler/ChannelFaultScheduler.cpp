//
// Created by jenik on 14.12.15.
//

#include "ChannelFaultScheduler.h"
#include <queue>

using namespace std;

ChannelFaultScheduler::ChannelFaultScheduler(DataStructure *dataStructure) : dataStructure(dataStructure) {

}

void ChannelFaultScheduler::InitializeSignalList(std::vector<FTSignal> &signalList) {
    for (int i = 0; i < dataStructure->signalsCount; i++) {
        FTSignal s;
        s.ID = i;
        s.node = dataStructure->signalNodeIDs[i];
        s.period = dataStructure->signalPeriods[i];
        s.payload = dataStructure->signalLengths[i];
        s.releaseDate = dataStructure->signalReleaseDates[i];
        s.deadline = dataStructure->signalDeadlines[i];
        s.isFaultTolerant = dataStructure->signalFaultTolerant[i];
        if (s.isFaultTolerant == 1) {
            s.channel = CHANNELAB;
        }
        else {
            char inA = 0, inB = 0;
            if (dataStructure->nodesOnBothChannels[dataStructure->signalNodeIDs[i] - 1] == 0) {
                if (dataStructure->nodesChannel[dataStructure->signalNodeIDs[i] - 1] == 1)
                    inA = 1;
                else if (dataStructure->nodesChannel[dataStructure->signalNodeIDs[i] - 1] == 0)
                    inB = 1;
            }
            for (int j = 0; j < static_cast<int>(dataStructure->signalReceivers[i].size()); j++) {
                if (dataStructure->nodesOnBothChannels[dataStructure->signalReceivers[i][j] - 1] == 1)
                    continue;
                if (dataStructure->nodesChannel[dataStructure->signalReceivers[i][j] - 1] == 1)
                    inA = 1;
                else if (dataStructure->nodesChannel[dataStructure->signalReceivers[i][j] - 1] == 0)
                    inB = 1;
                if ((inA == 1) && (inB == 1))
                    break;
            }
            if ((inA == 1) && (inB == 1)) {
                s.channel = CHANNELAB;
            }
            else if (inA == 1) {
                s.channel = CHANNELA;
            }
            else if (inB == 1) {
                s.channel = CHANNELB;
            }
            else {
                s.channel = CHANNELUNDEF;
            }
        }
        signalList.push_back(s);
    }
}

Position ChannelFaultScheduler::FTFindFirstCyclePositionForSignal(DataStructure *dataStructure,
                                                                  std::vector<std::vector<CFrame>> &schedule,
                                                                  int signalIndex, Position startPosition) {
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

int ChannelFaultScheduler::FTPlaceSignalToFrame(DataStructure *dataStructure,
                                                std::vector<std::vector<CFrame>> &schedule, Position position,
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

int ChannelFaultScheduler::PlaceSignalToChannelSchedule(DataStructure *channelDS, vector<vector<CFrame>> &schedule,
                                                        int signalIndex, int releaseDate) {
    int result = 0;
    Position startPosition, placePosition;
    char infeasiblePosition = 1;
    startPosition.inFrameOffset = 0;
    startPosition.cycle = releaseDate;
    startPosition.slot = 0;

    while (infeasiblePosition) {
        placePosition = FTFindFirstCyclePositionForSignal(channelDS, schedule, signalIndex, startPosition);
        if (placePosition.cycle == -1 || placePosition.inFrameOffset == -1 || placePosition.slot == -1)
            break;

        infeasiblePosition = 0;
        if (static_cast<FirstFitParameters *>(channelDS->schedulerParameters)->sortType != FFALL
            && static_cast<FirstFitParameters *>(channelDS->schedulerParameters)->sortType != FFPERIOD) {
            for (int i = 1; i < channelDS->hyperPeriod / channelDS->signalPeriods[signalIndex]; i++) {
                if (!CTestSignalInFrame(channelDS,
                                        &schedule[placePosition.cycle +
                                                  i * channelDS->signalPeriods[signalIndex]][placePosition.slot],
                                        placePosition.inFrameOffset, signalIndex)) {
                    infeasiblePosition = 1;
                    startPosition.cycle = placePosition.cycle;
                    startPosition.slot = placePosition.slot;
                    startPosition.inFrameOffset = (placePosition.inFrameOffset + 1) % channelDS->slotLength;
                    if (startPosition.inFrameOffset == 0)
                        startPosition.cycle = (startPosition.cycle + 1) % channelDS->hyperPeriod;
                    if (startPosition.cycle == 0)
                        startPosition.slot++;
                    break;
                }
            }
        }
        if (!infeasiblePosition) {
            FTPlaceSignalToFrame(channelDS, schedule, placePosition, signalIndex);
            result = placePosition.cycle;
        }
    }
    if (infeasiblePosition) // Create new slot
    {
        CFrame f = {};
        channelDS->signalStartCycle[signalIndex] = releaseDate;
        channelDS->signalSlot[signalIndex] = static_cast<int>(schedule[0].size());
        channelDS->signalInFrameOffsets[signalIndex] = 0;
        result = releaseDate;
        for (int i = 0; i < channelDS->hyperPeriod; i++) {
            schedule[i].push_back(f);
            if (i % channelDS->signalPeriods[signalIndex] == channelDS->signalReleaseDates[signalIndex])
                schedule[i][schedule[i].size() - 1].signals.push_back(signalIndex);
        }
    }

    return result;
}


ChannelSchedulerResult ChannelFaultScheduler::ChannelFTScheduler(const ChannelsFitness cfInput) {
    return ChannelFTScheduler(cfInput, false);
}

ChannelSchedulerResult ChannelFaultScheduler::ChannelFTScheduler(const ChannelsFitness cfInput, bool draw) {
    ChannelsFitness cf = cfInput;
    ChannelSchedulerResult result;
    unsigned long nodeCount = static_cast<unsigned long>(dataStructure->nodeCount);
    vector<FTSignal> SignalList;
    InitializeSignalList(SignalList);
    stable_sort(SignalList.begin(), SignalList.end(), [](FTSignal i, FTSignal j) { return (i.payload > j.payload); });
    stable_sort(SignalList.begin(), SignalList.end(),
                [](FTSignal i, FTSignal j) { return ((i.deadline - i.releaseDate) < (j.deadline - j.releaseDate)); });
    stable_sort(SignalList.begin(), SignalList.end(), [](FTSignal i, FTSignal j) { return (i.period < j.period); });
    stable_sort(SignalList.begin(), SignalList.end(), [](FTSignal i, FTSignal j) { return (i.node < j.node); });

    vector<vector<CFrame>> scheduleGW_A(static_cast<unsigned int>(dataStructure->hyperPeriod));
    vector<vector<CFrame>> scheduleGW_B(static_cast<unsigned int>(dataStructure->hyperPeriod));
    vector<vector<CFrame>> scheduleA(static_cast<unsigned int>(dataStructure->hyperPeriod));
    vector<vector<CFrame>> scheduleB(static_cast<unsigned int>(dataStructure->hyperPeriod));
    NodeSchedules faultTolerantSchedules = CreateFaultTolerantSchedules(SignalList);

    vector<int> nodeSlotsA(nodeCount); // Kolik slotù má daný node alokovaných v A
    vector<int> nodeSlotsB(nodeCount); // Kolik slotù má daný node alokovaných v B

    vector<int> occupationSlotsA(nodeCount); // Jakou šíøku pásma zabírají (kolik bitù) signály nodu v A
    vector<int> occupationSlotsB(nodeCount); // Jakou šíøku pásma zabírají (kolik bitù) signály nodu v B

    DataStructure dsA;
    DataStructure dsB;
    MakeDeepCopyOfDataStructure(&dsA, dataStructure);
    MakeDeepCopyOfDataStructure(&dsB, dataStructure);

    int last = 0;
    int sumA = 0;
    int sumB = 0;

    for (int i = 0; i < dataStructure->signalsCount; i++) {
        if (dataStructure->signalFaultTolerant[i] == 1)
            continue;
        dsA.signalSlot[i] = -1;
        dsA.signalStartCycle[i] = -1;
        dsA.signalInFrameOffsets[i] = -1;
        dsB.signalSlot[i] = -1;
        dsB.signalStartCycle[i] = -1;
        dsB.signalInFrameOffsets[i] = -1;
    }
    vector<int> nodesFirstSignal(nodeCount);
    std::queue<int> nodesQueue;
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        nodesQueue.push(i);
        nodesFirstSignal[i] = last;
        last += dataStructure->signalsInNodeCounts[i];
    }
    last = 0;
    //for(i = 0; i < dataStructure->nodeCount; i++)
    while (!nodesQueue.empty()) {
        int i = 0;
        if ((dataStructure->nodesOnBothChannels[nodesQueue.front()] == 1) && (last < dataStructure->nodeCount)) {
            nodesQueue.push(nodesQueue.front());
            nodesQueue.pop();
            last++;
            continue;
        }
        else {
            i = nodesQueue.front();
            nodesQueue.pop();
        }
        for (int j = 0; j < dataStructure->hyperPeriod; j++) {
            scheduleA[j].clear();
            scheduleB[j].clear();
        }
        scheduleA = faultTolerantSchedules[i];
        scheduleB = faultTolerantSchedules[i];

        for (int j = nodesFirstSignal[i]; j < nodesFirstSignal[i] + dataStructure->signalsInNodeCounts[i]; j++) {
            if (SignalList[j].isFaultTolerant == 1) // Fault Tolerant messages are already scheduled
            {
                continue;
            }
            if (SignalList[j].channel == CHANNELUNDEF) // Zpráva mezi nody pripojenými na oba kanály
            {
                bool toB = 1;
                if ((sumA + scheduleA[0].size() < sumB + scheduleB[0].size()) ||
                    ((sumA + scheduleA[0].size() == sumB + scheduleB[0].size()) && (cf.channelA < cf.channelB)))
                    toB = 0;
                if (toB == 1) {
                    PlaceSignalToChannelSchedule(&dsB, scheduleB, SignalList[j].ID, SignalList[j].releaseDate);
                    cf.channelB += SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
                    occupationSlotsB[i] += SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
                    SignalList[j].channel = CHANNELB;
                }
                else {
                    PlaceSignalToChannelSchedule(&dsA, scheduleA, SignalList[j].ID, SignalList[j].releaseDate);
                    cf.channelA += SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
                    occupationSlotsA[i] += SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
                    SignalList[j].channel = CHANNELA;
                }
            }
            else if (SignalList[j].channel == CHANNELA) { // Zprava jen v kanalu A
                PlaceSignalToChannelSchedule(&dsA, scheduleA, SignalList[j].ID, SignalList[j].releaseDate);
                occupationSlotsA[i] += SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
            }
            else if (SignalList[j].channel == CHANNELB) { // Zprava jen v kanalu B
                PlaceSignalToChannelSchedule(&dsB, scheduleB, SignalList[j].ID, SignalList[j].releaseDate);
                occupationSlotsB[i] += SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
            }
            else if (SignalList[j].channel == CHANNELAB) { // Zprava v obou kanalech
                int gwReleaseDate;
                if (dataStructure->nodesOnBothChannels[i] == 1) {
                    PlaceSignalToChannelSchedule(&dsA, scheduleA, SignalList[j].ID, SignalList[j].releaseDate);
                    occupationSlotsA[i] += SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
                    PlaceSignalToChannelSchedule(&dsB, scheduleB, SignalList[j].ID, SignalList[j].releaseDate);
                    occupationSlotsB[i] += SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
                }
                else {
                    if (dataStructure->nodesChannel[i] == 1) {
                        gwReleaseDate = PlaceSignalToChannelSchedule(&dsA, scheduleA, SignalList[j].ID,
                                                                     SignalList[j].releaseDate);
                        occupationSlotsA[i] +=
                                SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
                        dsB.signalReleaseDates[SignalList[j].ID] = gwReleaseDate;
                        PlaceSignalToChannelSchedule(&dsB, scheduleGW_B, SignalList[j].ID, gwReleaseDate);
                        dsB.signalNodeIDs[SignalList[j].ID] = dataStructure->nodeCount + 1;
                    }
                    else {
                        gwReleaseDate = PlaceSignalToChannelSchedule(&dsB, scheduleB, SignalList[j].ID,
                                                                     SignalList[j].releaseDate);
                        occupationSlotsB[i] +=
                                SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
                        dsA.signalReleaseDates[SignalList[j].ID] = gwReleaseDate;
                        PlaceSignalToChannelSchedule(&dsA, scheduleGW_A, SignalList[j].ID, gwReleaseDate);
                        dsA.signalNodeIDs[SignalList[j].ID] = dataStructure->nodeCount + 1;
                    }
                }
            }
        }

        nodeSlotsA[i] = static_cast<int>(scheduleA[0].size());
        nodeSlotsB[i] = static_cast<int>(scheduleB[0].size());
        sumA += scheduleA[0].size();
        sumB += scheduleB[0].size();

        last++;
    }
    sumA += scheduleGW_A[0].size();
    sumB += scheduleGW_B[0].size();

    // Slot scheduling
    // slotA_mapping[i][j] - jake slotID ma slot nodu i v rozvrhu pro kanal A ktery byl v temporalnim rozvrhu oznacen jako slot j
    int **slotA_mapping = new int *[dataStructure->nodeCount + 1];
    int **slotB_mapping = new int *[dataStructure->nodeCount + 1];
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        if (nodeSlotsA[i] > 0)
            slotA_mapping[i] = new int[nodeSlotsA[i]];
        if (nodeSlotsB[i] > 0)
            slotB_mapping[i] = new int[nodeSlotsB[i]];
    }
    // Add gateway slots
    slotA_mapping[dataStructure->nodeCount] = new int[scheduleGW_A[0].size()];
    slotB_mapping[dataStructure->nodeCount] = new int[scheduleGW_B[0].size()];

    // Assuring, that the slots with the fault tolerant signals will be mappet to the same slot in both channels
    int slotFTCounter = 0;
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        for (int j = 0; j < faultTolerantSchedules[i][0].size(); j++) {
            slotA_mapping[i][j] = slotFTCounter;
            slotB_mapping[i][j] = slotFTCounter;
            slotFTCounter++;
        }
    }

    int slotCounter = slotFTCounter; // Map not-FT slots after FT slots
    // Pro kanál A
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        for (int j = static_cast<int>(faultTolerantSchedules[i][0].size()); j < nodeSlotsA[i]; j++) {
            slotA_mapping[i][j] = slotCounter;
            slotCounter++;
        }
    }
    for (int j = 0; j < static_cast<int>(scheduleGW_A[0].size()); j++) {
        slotA_mapping[dataStructure->nodeCount][j] = slotCounter;
        slotCounter++;
    }

    slotCounter = slotFTCounter;
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        for (int j = static_cast<int>(faultTolerantSchedules[i][0].size()); j < nodeSlotsB[i]; j++) {
            slotB_mapping[i][j] = slotCounter;
            slotCounter++;
        }
    }
    for (int j = 0; j < static_cast<int>(scheduleGW_B[0].size()); j++) {
        slotB_mapping[dataStructure->nodeCount][j] = slotCounter;
        slotCounter++;
    }

    vector<int> slotAThroughGW(static_cast<unsigned long>(sumA));
    vector<int> slotBThroughGW(static_cast<unsigned long>(sumB));

    // Prvotní mapování
    for (int i = 0; i < dataStructure->signalsCount; i++) {
        switch (SignalList[i].channel) {
            case CHANNELA:
                dsA.signalSlot[SignalList[i].ID] = slotA_mapping[SignalList[i].node -
                                                                 1][dsA.signalSlot[SignalList[i].ID]];
                break;
            case CHANNELB:
                dsB.signalSlot[SignalList[i].ID] = slotB_mapping[SignalList[i].node -
                                                                 1][dsB.signalSlot[SignalList[i].ID]];
                break;
            case CHANNELAB:
                if (dataStructure->nodesOnBothChannels[SignalList[i].node - 1] == 1) {
                    dsA.signalSlot[SignalList[i].ID] = slotA_mapping[SignalList[i].node -
                                                                     1][dsA.signalSlot[SignalList[i].ID]];
                    dsB.signalSlot[SignalList[i].ID] = slotB_mapping[SignalList[i].node -
                                                                     1][dsB.signalSlot[SignalList[i].ID]];
                }
                else {
                    if (dataStructure->nodesChannel[SignalList[i].node - 1] == 1) {
                        dsA.signalSlot[SignalList[i].ID] = slotA_mapping[SignalList[i].node -
                                                                         1][dsA.signalSlot[SignalList[i].ID]];
                        slotAThroughGW[dsA.signalSlot[SignalList[i].ID]] = 1;
                        dsB.signalSlot[SignalList[i].ID] = slotB_mapping[dataStructure->nodeCount][dsB.signalSlot[SignalList[i].ID]]; // Mapping z GW
                    }
                    else {
                        dsB.signalSlot[SignalList[i].ID] = slotB_mapping[SignalList[i].node -
                                                                         1][dsB.signalSlot[SignalList[i].ID]];
                        slotBThroughGW[dsB.signalSlot[SignalList[i].ID]] = 1;
                        dsA.signalSlot[SignalList[i].ID] = slotA_mapping[dataStructure->nodeCount][dsA.signalSlot[SignalList[i].ID]]; // Mapping z GW
                    }
                }
                break;
            case CHANNELUNDEF:
                error("CHANNEL is still UNDEF!");
                break;
            default:
                error("Channel has unknows value");
                break;
        }
    }
    for (int i = 0; i < dataStructure->nodeCount + 1; i++) {
        if (i < dataStructure->nodeCount) {
            if (nodeSlotsA[i] > 0)
                delete[] slotA_mapping[i];
            if (nodeSlotsB[i] > 0)
                delete[] slotB_mapping[i];
        }
        else {
            if (scheduleGW_A[0].size() > 0)
                delete[] slotA_mapping[i];
            if (scheduleGW_B[0].size() > 0)
                delete[] slotB_mapping[i];
        }

    }

    delete[] slotA_mapping;
    delete[] slotB_mapping;

    // Druhé mapování pro maximalizaci možného zpoždìní na GW
    /*
    vector<int> slotA_optMapping(static_cast<unsigned long>(sumA));
    vector<int> slotB_optMapping(static_cast<unsigned long>(sumB));
    int mapCounter = 0;
    int lastThroughtA = -1, lastThroughtB = -1;
    for (int i = 0; i < sumA; i++) {
        if (slotAThroughGW[i] == 1) {
            slotA_optMapping[i] = mapCounter;
            lastThroughtA = mapCounter;
            mapCounter++;
        }
    }
    for (int i = 0; i < sumA; i++) {
        if (slotAThroughGW[i] == 0) {
            slotA_optMapping[i] = mapCounter;
            mapCounter++;
        }
    }

    mapCounter = 0;
    for (int i = 0; i < sumB; i++) {
        if (slotBThroughGW[i] == 1) {
            slotB_optMapping[i] = mapCounter;
            lastThroughtB = mapCounter;
            mapCounter++;
        }
    }
    for (int i = 0; i < sumB; i++) {
        if (slotBThroughGW[i] == 0) {
            slotB_optMapping[i] = mapCounter;
            mapCounter++;
        }
    }
    int sumTmp = sumA;
    for (int i = static_cast<int>(sumTmp - scheduleGW_A[0].size()); i < sumTmp; i++) {
        slotA_optMapping[i] = slotA_optMapping[i] +
                              std::max<int>(0, static_cast<int>(lastThroughtB + 1 - sumTmp + scheduleGW_A[0].size()));
        if (slotA_optMapping[i] > sumA - 1)
            sumA = slotA_optMapping[i] + 1;
    }

    sumTmp = sumB;
    for (int i = static_cast<int>(sumTmp - scheduleGW_B[0].size()); i < sumTmp; i++) {
        slotB_optMapping[i] = slotB_optMapping[i] +
                              std::max<int>(0, static_cast<int>(lastThroughtA + 1 - sumTmp + scheduleGW_B[0].size()));
        if (slotB_optMapping[i] > sumB - 1)
            sumB = slotB_optMapping[i] + 1;
    }

    for (int i = 0; i < dataStructure->signalsCount; i++) {
        if (dsA.signalSlot[i] != -1)
            dsA.signalSlot[i] = slotA_optMapping[dsA.signalSlot[i]];
        if (dsB.signalSlot[i] != -1)
            dsB.signalSlot[i] = slotB_optMapping[dsB.signalSlot[i]];
    }
    */
    // GW musí být za poslední slot through
    // znovu pøemapovat signály
    // nový výpoèet sumA/sumB

    dataStructure->maxSlot = (sumA > sumB) ? sumA : sumB;

    result.gatewaySlotsInA = static_cast<int>(scheduleGW_A[0].size());
    result.gatewaySlotsInB = static_cast<int>(scheduleGW_B[0].size());
    result.slotsInA = sumA;
    result.slotsInB = sumB;

    printf("Length of the schedule: %d \n", dataStructure->maxSlot);
    if (draw) {
        char ofA[6] = "A.svg";
        char ofB[6] = "B.svg";
        dsA.outputFile = ofA;
        dsB.outputFile = ofB;
        dsA.maxSlot = dataStructure->maxSlot;
        dsB.maxSlot = dataStructure->maxSlot;
        dsA.nodeCount = dsA.nodeCount + 1; // Add gateway node
        dsB.nodeCount = dsB.nodeCount + 1;
        createGanttChartSVGForVariant(&dsA, 0);
        createGanttChartSVGForVariant(&dsB, 0);
    }
    FreeDataStructure(&dsA);
    FreeDataStructure(&dsB);

    return result;
}

NodeSchedules ChannelFaultScheduler::CreateFaultTolerantSchedules(vector<ChannelFaultScheduler::FTSignal> signalList) {
    NodeSchedules schedules(static_cast<unsigned long>(dataStructure->nodeCount));
    stable_sort(signalList.begin(), signalList.end(),
                [](FTSignal i, FTSignal j) { return (i.isFaultTolerant > j.isFaultTolerant); });

    // Schedules initialization
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        schedules[i].resize(static_cast<unsigned long>(dataStructure->hyperPeriod));
    }
    int signalIndex;
    for (signalIndex = 0; signalList[signalIndex].isFaultTolerant == 1; signalIndex++) {
        FTSignal &signal = signalList[signalIndex];
        PlaceSignalToChannelSchedule(dataStructure, schedules[signal.node - 1], signal.ID, signal.releaseDate);
    }
    return schedules;
}
