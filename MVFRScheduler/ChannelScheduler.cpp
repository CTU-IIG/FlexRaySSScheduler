#include "MVFRScheduler.h"
#include <queue>
#include <algorithm>

int PlaceSignalToChannelSchedule(DataStructure *dataStructure, std::vector<CFrame> *schedule, int signalIndex,
                                 int releaseDate) {
    int i;
    int result = 0;
    Position startPosition, placePosition;
    char infeasiblePosition = 1;
    startPosition.inFrameOffset = 0;
    startPosition.cycle = releaseDate;
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
            result = placePosition.cycle;
        }
    }
    if (infeasiblePosition) // Create new slot
    {
        CFrame f;
        dataStructure->signalStartCycle[signalIndex] = releaseDate;
        result = releaseDate;
        dataStructure->signalSlot[signalIndex] = static_cast<int>(schedule[0].size());
        dataStructure->signalInFrameOffsets[signalIndex] = 0;
        for (i = 0; i < dataStructure->hyperPeriod; i++) {
            schedule[i].push_back(f);
            if (i % dataStructure->signalPeriods[signalIndex] == dataStructure->signalReleaseDates[signalIndex])
                schedule[i][schedule[i].size() - 1].signals.push_back(signalIndex);
        }
    }

    return result;
}


void InitializeSignalList(DataStructure *dataStructure, std::vector<Signal> &signalList) {
    int i, j;
    for (i = 0; i < dataStructure->signalsCount; i++) {
        Signal s;
        s.ID = i;
        s.node = dataStructure->signalNodeIDs[i];
        s.period = dataStructure->signalPeriods[i];
        s.payload = dataStructure->signalLengths[i];
        s.releaseDate = dataStructure->signalReleaseDates[i];
        s.deadline = dataStructure->signalDeadlines[i];

        char inA = 0, inB = 0;
        if (dataStructure->nodesOnBothChannels[dataStructure->signalNodeIDs[i] - 1] == 0) {
            if (dataStructure->nodesChannel[dataStructure->signalNodeIDs[i] - 1] == 1)
                inA = 1;
            else if (dataStructure->nodesChannel[dataStructure->signalNodeIDs[i] - 1] == 0)
                inB = 1;
        }
        for (j = 0; j < static_cast<int>(dataStructure->signalReceivers[i].size()); j++) {
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
        signalList.push_back(s);
    }
}

bool cmpByPeriodInc(Signal i, Signal j) { return (i.period < j.period); }

bool cmpByPayloadDec(Signal i, Signal j) { return (i.payload > j.payload); }

bool cmpByWindowInc(Signal i, Signal j) { return ((i.deadline - i.releaseDate) < (j.deadline - j.releaseDate)); }

bool cmpByNodeInc(Signal i, Signal j) { return (i.node < j.node); }
//bool cmpByABAssignment (Signal i,Signal j) { return ((i.channel == CHANNELAB) && (j.channel != CHANNELAB)); } // Hodí AB zprávy na zaèátek

void CheckChannelSchedules(std::vector<Signal> signalList, DataStructure *dsA, DataStructure *dsB) {
    int i;
    for (i = 0; i < dsA->signalsCount; i++) {
        if (signalList[i].channel == CHANNELAB) {
            if (dsA->nodesOnBothChannels[signalList[i].node - 1] == 1)
                continue;
            if (dsA->nodesChannel[signalList[i].node - 1] == 1) {
                if (dsA->signalStartCycle[signalList[i].ID] > dsB->signalStartCycle[signalList[i].ID])
                    error("GW_B transmitts unrecieved signal");
                if ((dsA->signalStartCycle[signalList[i].ID] == dsB->signalStartCycle[signalList[i].ID]) &&
                    dsA->signalSlot[signalList[i].ID] > dsB->signalSlot[signalList[i].ID])
                    error("GW_B transmitts unrecieved signal");
            }
            else {
                if (dsB->signalStartCycle[signalList[i].ID] > dsA->signalStartCycle[signalList[i].ID])
                    error("GW_A transmitts unrecieved signal");
                if ((dsB->signalStartCycle[signalList[i].ID] == dsA->signalStartCycle[signalList[i].ID]) &&
                    dsB->signalSlot[signalList[i].ID] > dsA->signalSlot[signalList[i].ID])
                    error("GW_A transmitts unrecieved signal");
            }
        }
    }
}

int ChannelScheduler(DataStructure *dataStructure, ChannelsFitness cf) {
    int i, j;
    std::vector<Signal> SignalList;
    InitializeSignalList(dataStructure, SignalList);
    stable_sort(SignalList.begin(), SignalList.end(), cmpByPayloadDec);
    stable_sort(SignalList.begin(), SignalList.end(), cmpByWindowInc);
    stable_sort(SignalList.begin(), SignalList.end(), cmpByPeriodInc);
    stable_sort(SignalList.begin(), SignalList.end(), cmpByNodeInc);
    //std::stable_sort(SignalList.begin(), SignalList.end(), cmpByABAssignment);

    std::vector<CFrame> *scheduleGW_A = new std::vector<CFrame>[dataStructure->hyperPeriod];
    std::vector<CFrame> *scheduleGW_B = new std::vector<CFrame>[dataStructure->hyperPeriod];
    std::vector<CFrame> *scheduleA = new std::vector<CFrame>[dataStructure->hyperPeriod];
    std::vector<CFrame> *scheduleB = new std::vector<CFrame>[dataStructure->hyperPeriod];

    int *nodeSlotsA = new int[dataStructure->nodeCount](); // Kolik slotù má daný node alokovaných v A
    int *nodeSlotsB = new int[dataStructure->nodeCount](); // Kolik slotù má daný node alokovaných v B

    int *occupationSlotsA = new int[dataStructure->nodeCount](); // Jakou šíøku pásma zabírají (kolik bitù) signály nodu v A
    int *occupationSlotsB = new int[dataStructure->nodeCount](); // Jakou šíøku pásma zabírají (kolik bitù) signály nodu v B

    DataStructure dsA;
    DataStructure dsB;
    MakeDeepCopyOfDataStructure(&dsA, dataStructure);
    MakeDeepCopyOfDataStructure(&dsB, dataStructure);

    int last = 0;
    int sumA = 0;
    int sumB = 0;

    for (i = 0; i < dataStructure->signalsCount; i++) {
        dsA.signalSlot[i] = -1;
        dsA.signalStartCycle[i] = -1;
        dsA.signalInFrameOffsets[i] = -1;
        dsB.signalSlot[i] = -1;
        dsB.signalStartCycle[i] = -1;
        dsB.signalInFrameOffsets[i] = -1;
    }
    int *nodesFirstSignal = new int[dataStructure->nodeCount];
    std::queue<int> nodesQueue;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        nodesQueue.push(i);
        nodesFirstSignal[i] = last;
        last += dataStructure->signalsInNodeCounts[i];
    }
    last = 0;
    //for(i = 0; i < dataStructure->nodeCount; i++)
    while (!nodesQueue.empty()) {
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
        for (j = 0; j < dataStructure->hyperPeriod; j++) {
            scheduleA[j].clear();
            scheduleB[j].clear();
        }

        for (j = nodesFirstSignal[i]; j < nodesFirstSignal[i] + dataStructure->signalsInNodeCounts[i]; j++) {
            if (SignalList[j].channel == CHANNELUNDEF) // Zpráva mezi nody pøipojenými na oba kanály
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
            else if (SignalList[j].channel == CHANNELA) {
                PlaceSignalToChannelSchedule(&dsA, scheduleA, SignalList[j].ID, SignalList[j].releaseDate);
                occupationSlotsA[i] += SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
            }
            else if (SignalList[j].channel == CHANNELB) {
                PlaceSignalToChannelSchedule(&dsB, scheduleB, SignalList[j].ID, SignalList[j].releaseDate);
                occupationSlotsB[i] += SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
            }
            else if (SignalList[j].channel == CHANNELAB) {
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
                    }
                    else {
                        gwReleaseDate = PlaceSignalToChannelSchedule(&dsB, scheduleB, SignalList[j].ID,
                                                                     SignalList[j].releaseDate);
                        occupationSlotsB[i] +=
                                SignalList[j].payload * dataStructure->hyperPeriod / SignalList[j].period;
                        dsA.signalReleaseDates[SignalList[j].ID] = gwReleaseDate;
                        PlaceSignalToChannelSchedule(&dsA, scheduleGW_A, SignalList[j].ID, gwReleaseDate);
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
    int **slotA_mapping = new int *[dataStructure->nodeCount +
                                    1]; // slotA_mapping[i][j] - jaké slotID má slot nodu i v rozvrhu pro kanál A který byl v temporálním rozvrhu oznaèen jako slot j
    int **slotB_mapping = new int *[dataStructure->nodeCount + 1];
    for (i = 0; i < dataStructure->nodeCount; i++) {
        if (nodeSlotsA[i] > 0)
            slotA_mapping[i] = new int[nodeSlotsA[i]];
        if (nodeSlotsB[i] > 0)
            slotB_mapping[i] = new int[nodeSlotsB[i]];
    }
    slotA_mapping[dataStructure->nodeCount] = new int[scheduleGW_A[0].size()];
    slotB_mapping[dataStructure->nodeCount] = new int[scheduleGW_B[0].size()];

    int slotCounter = 0;
    // Pro kanál A
    for (i = 0; i < dataStructure->nodeCount; i++) {
        for (j = 0; j < nodeSlotsA[i]; j++) {
            slotA_mapping[i][j] = slotCounter;
            slotCounter++;
        }
    }
    for (j = 0; j < static_cast<int>(scheduleGW_A[0].size()); j++) {
        slotA_mapping[dataStructure->nodeCount][j] = slotCounter;
        slotCounter++;
    }

    slotCounter = 0;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        for (j = 0; j < nodeSlotsB[i]; j++) {
            slotB_mapping[i][j] = slotCounter;
            slotCounter++;
        }
    }
    for (j = 0; j < static_cast<int>(scheduleGW_B[0].size()); j++) {
        slotB_mapping[dataStructure->nodeCount][j] = slotCounter;
        slotCounter++;
    }

    int *slotAThroughGW = new int[sumA]();
    int *slotBThroughGW = new int[sumB]();

    // Prvotní mapování
    for (i = 0; i < dataStructure->signalsCount; i++) {
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
    for (i = 0; i < dataStructure->nodeCount + 1; i++) {
        if (nodeSlotsA[i] > 0)
            delete[] slotA_mapping[i];
        if (nodeSlotsB[i] > 0)
            delete[] slotB_mapping[i];
    }
    delete[] slotA_mapping;
    delete[] slotB_mapping;

    // Druhé mapování pro maximalizaci možného zpoždìní na GW
    int *slotA_optMapping = new int[sumA];
    int *slotB_optMapping = new int[sumB];
    int mapCounter = 0;
    int lastThroughtA = -1, lastThroughtB = -1;
    for (i = 0; i < sumA; i++) {
        if (slotAThroughGW[i] == 1) {
            slotA_optMapping[i] = mapCounter;
            lastThroughtA = mapCounter;
            mapCounter++;
        }
    }
    for (i = 0; i < sumA; i++) {
        if (slotAThroughGW[i] == 0) {
            slotA_optMapping[i] = mapCounter;
            mapCounter++;
        }
    }

    mapCounter = 0;
    for (i = 0; i < sumB; i++) {
        if (slotBThroughGW[i] == 1) {
            slotB_optMapping[i] = mapCounter;
            lastThroughtB = mapCounter;
            mapCounter++;
        }
    }
    for (i = 0; i < sumB; i++) {
        if (slotBThroughGW[i] == 0) {
            slotB_optMapping[i] = mapCounter;
            mapCounter++;
        }
    }
    int sumTmp = sumA;
    for (i = static_cast<int>(sumTmp - scheduleGW_A[0].size()); i < sumTmp; i++) {
        slotA_optMapping[i] = slotA_optMapping[i] +
                              std::max<int>(0, static_cast<int>(lastThroughtB + 1 - sumTmp + scheduleGW_A[0].size()));
        if (slotA_optMapping[i] > sumA - 1)
            sumA = slotA_optMapping[i] + 1;
    }

    sumTmp = sumB;
    for (i = static_cast<int>(sumTmp - scheduleGW_B[0].size()); i < sumTmp; i++) {
        slotB_optMapping[i] = slotB_optMapping[i] +
                              std::max<int>(0, static_cast<int>(lastThroughtA + 1 - sumTmp + scheduleGW_B[0].size()));
        if (slotB_optMapping[i] > sumB - 1)
            sumB = slotB_optMapping[i] + 1;
    }

    for (i = 0; i < dataStructure->signalsCount; i++) {
        if (dsA.signalSlot[i] != -1)
            dsA.signalSlot[i] = slotA_optMapping[dsA.signalSlot[i]];
        if (dsB.signalSlot[i] != -1)
            dsB.signalSlot[i] = slotB_optMapping[dsB.signalSlot[i]];
    }
    // GW musí být za poslední slot through
    // znovu pøemapovat signály
    // nový výpoèet sumA/sumB

    CheckChannelSchedules(SignalList, &dsA, &dsB);
    dataStructure->maxSlot = (sumA > sumB) ? sumA : sumB;

    if (dataStructure->appendToCSV != 0) {
        AppendToCSV(dataStructure, static_cast<int>(scheduleGW_A[0].size()));
        AppendToCSV(dataStructure, static_cast<int>(scheduleGW_B[0].size()));
        AppendToCSV(dataStructure, sumA);
        AppendToCSV(dataStructure, sumB);
    }

    printf("Length of the schedule: %d \n", dataStructure->maxSlot);
    FreeDataStructure(&dsA);
    FreeDataStructure(&dsB);

    delete[] occupationSlotsA;
    delete[] occupationSlotsB;
    delete[] nodeSlotsA;
    delete[] nodeSlotsB;
    delete[] scheduleGW_A;
    delete[] scheduleGW_B;
    delete[] scheduleA;
    delete[] scheduleB;
    delete[] nodesFirstSignal;
    delete[] slotAThroughGW;
    delete[] slotBThroughGW;
    delete[] slotA_optMapping;
    delete[] slotB_optMapping;
    return SUCCESS;
}
