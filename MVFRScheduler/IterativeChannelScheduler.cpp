//
// Created by jenik on 12/8/15.
//

#include "IterativeChannelScheduler.h"
#include <queue>
#include <math.h>
#include <sys/time.h>

using namespace std;

IterativeChannelScheduler::IterativeChannelScheduler(DataStructure *dataStructure, const int maxIterationCount)
        : dataStructure(dataStructure), maxIterationCount(maxIterationCount), iterationNumber(0) { }

void IterativeChannelScheduler::CreateSchedule(const double alpha) {
    timeval startTime, endTime;
    vector<char> bestNodeChannel(static_cast<unsigned int>(dataStructure->nodeCount));
    gettimeofday(&startTime, NULL);
    ChannelFaultScheduler cfs(dataStructure);
    double beta = 1.0;
    int maxAssignerIterations = 1000;
    iterationNumber = 0;
    ChannelSchedulerResult csr, bestCsr;
    ChannelsFitness bestCf;
    ChannelsFitness cf = ChannelAssigner(dataStructure, alpha, beta, maxAssignerIterations);
    csr = cfs.ChannelFTScheduler(cf);
    bestCsr = csr;
    bestCf = cf;
    copy(dataStructure->nodesChannel, dataStructure->nodesChannel + dataStructure->nodeCount, bestNodeChannel.begin());
    gettimeofday(&endTime, NULL);
    if (dataStructure->appendToCSV != 0) {
        AppendToCSV(dataStructure, bestCf.channelA);
        AppendToCSV(dataStructure, bestCf.channelB);
        AppendToCSV(dataStructure, bestCf.gateway);
        AppendToCSV(dataStructure, max(bestCsr.slotsInA, bestCsr.slotsInB));
        AppendToCSV(dataStructure, min(bestCsr.slotsInA, bestCsr.slotsInB));
        AppendToCSV(dataStructure,
                    bestCsr.slotsInA >= bestCsr.slotsInB ? bestCsr.gatewaySlotsInA : bestCsr.gatewaySlotsInB);
        AppendToCSV(dataStructure,
                    bestCsr.slotsInA < bestCsr.slotsInB ? bestCsr.gatewaySlotsInA : bestCsr.gatewaySlotsInB);
        AppendToCSV(dataStructure,
                    static_cast<float>(1000 * (endTime.tv_sec - startTime.tv_sec) +
                                       (endTime.tv_usec - startTime.tv_usec) / 1000.0));
    }
    iterationNumber++;
    if (verboseLevel > 0)
        cout << csr.str() << endl;
    while (true) {
        beta = UpdateBeta(csr);
        cf = ChannelAssigner(dataStructure, alpha, beta, maxAssignerIterations);
        if (!TerminationCondition(cf))
            break;
        //csr = ChannelSchedulerIt(cf);
        csr = cfs.ChannelFTScheduler(cf);
        if ((max(csr.slotsInA, csr.slotsInB) < max(bestCsr.slotsInA, bestCsr.slotsInB)) ||
            ((max(csr.slotsInA, csr.slotsInB) == max(bestCsr.slotsInA, bestCsr.slotsInB)) &&
             (min(csr.slotsInA, csr.slotsInB) < min(bestCsr.slotsInA, bestCsr.slotsInB)))) {
            bestCsr = csr;
            bestCf = cf;
            copy(dataStructure->nodesChannel, dataStructure->nodesChannel + dataStructure->nodeCount,
                 bestNodeChannel.begin());
            cout << "B: " << cf.str() << endl;
        }
        iterationNumber++;
        if (verboseLevel > 0)
            cout << csr.str() << endl;
    }
    gettimeofday(&endTime, NULL);
    if (verboseLevel > 0) {
        cout << "Best found: " << bestCsr.str() << "/" << bestCf.str() << endl;
        copy(bestNodeChannel.begin(), bestNodeChannel.end(), dataStructure->nodesChannel);
        csr = cfs.ChannelFTScheduler(bestCf, true);
        cout << "Generated: " << csr.str() << "/" << bestCf.str() << endl;
    }
    if (dataStructure->appendToCSV != 0) {
        AppendToCSV(dataStructure, bestCf.channelA);
        AppendToCSV(dataStructure, bestCf.channelB);
        AppendToCSV(dataStructure, bestCf.gateway);
        AppendToCSV(dataStructure, max(bestCsr.slotsInA, bestCsr.slotsInB));
        AppendToCSV(dataStructure, min(bestCsr.slotsInA, bestCsr.slotsInB));
        AppendToCSV(dataStructure,
                    bestCsr.slotsInA >= bestCsr.slotsInB ? bestCsr.gatewaySlotsInA : bestCsr.gatewaySlotsInB);
        AppendToCSV(dataStructure,
                    bestCsr.slotsInA < bestCsr.slotsInB ? bestCsr.gatewaySlotsInA : bestCsr.gatewaySlotsInB);
        AppendToCSV(dataStructure,
                    static_cast<float>(1000 * (endTime.tv_sec - startTime.tv_sec) +
                                       (endTime.tv_usec - startTime.tv_usec) / 1000.0));
    }
}

void IterativeChannelScheduler::InitializeSignalList(std::vector<Signal> &signalList) {
    for (int i = 0; i < dataStructure->signalsCount; i++) {
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
        signalList.push_back(s);
    }
}

int IterativeChannelScheduler::PlaceSignalToChannelSchedule(DataStructure *channelDS, std::vector<CFrame> *schedule,
                                                            int signalIndex, int releaseDate) {
    int result = 0;
    Position startPosition, placePosition;
    char infeasiblePosition = 1;
    startPosition.inFrameOffset = 0;
    startPosition.cycle = releaseDate;
    startPosition.slot = 0;

    while (infeasiblePosition) {
        placePosition = CFindFirstCyclePositionForSignal(channelDS, schedule, signalIndex, startPosition);
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
            CPlaceSignalToFrame(channelDS, schedule, placePosition, signalIndex);
            result = placePosition.cycle;
        }
    }
    if (infeasiblePosition) // Create new slot
    {
        CFrame f;
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

ChannelSchedulerResult IterativeChannelScheduler::ChannelSchedulerIt(const ChannelsFitness cfInput) {
    ChannelsFitness cf = cfInput;
    ChannelSchedulerResult result;
    unsigned long nodeCount = static_cast<unsigned long>(dataStructure->nodeCount);
    vector<Signal> SignalList;
    InitializeSignalList(SignalList);
    stable_sort(SignalList.begin(), SignalList.end(), [](Signal i, Signal j) { return (i.payload > j.payload); });
    stable_sort(SignalList.begin(), SignalList.end(),
                [](Signal i, Signal j) { return ((i.deadline - i.releaseDate) < (j.deadline - j.releaseDate)); });
    stable_sort(SignalList.begin(), SignalList.end(), [](Signal i, Signal j) { return (i.period < j.period); });
    stable_sort(SignalList.begin(), SignalList.end(), [](Signal i, Signal j) { return (i.node < j.node); });

    vector<CFrame> *scheduleGW_A = new std::vector<CFrame>[dataStructure->hyperPeriod];
    vector<CFrame> *scheduleGW_B = new std::vector<CFrame>[dataStructure->hyperPeriod];
    vector<CFrame> *scheduleA = new std::vector<CFrame>[dataStructure->hyperPeriod];
    vector<CFrame> *scheduleB = new std::vector<CFrame>[dataStructure->hyperPeriod];

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

        for (int j = nodesFirstSignal[i]; j < nodesFirstSignal[i] + dataStructure->signalsInNodeCounts[i]; j++) {
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
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        if (nodeSlotsA[i] > 0)
            slotA_mapping[i] = new int[nodeSlotsA[i]];
        if (nodeSlotsB[i] > 0)
            slotB_mapping[i] = new int[nodeSlotsB[i]];
    }
    slotA_mapping[dataStructure->nodeCount] = new int[scheduleGW_A[0].size()];
    slotB_mapping[dataStructure->nodeCount] = new int[scheduleGW_B[0].size()];

    int slotCounter = 0;
    // Pro kanál A
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        for (int j = 0; j < nodeSlotsA[i]; j++) {
            slotA_mapping[i][j] = slotCounter;
            slotCounter++;
        }
    }
    for (int j = 0; j < static_cast<int>(scheduleGW_A[0].size()); j++) {
        slotA_mapping[dataStructure->nodeCount][j] = slotCounter;
        slotCounter++;
    }

    slotCounter = 0;
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        for (int j = 0; j < nodeSlotsB[i]; j++) {
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
    // GW musí být za poslední slot through
    // znovu pøemapovat signály
    // nový výpoèet sumA/sumB

    dataStructure->maxSlot = (sumA > sumB) ? sumA : sumB;

    result.gatewaySlotsInA = static_cast<int>(scheduleGW_A[0].size());
    result.gatewaySlotsInB = static_cast<int>(scheduleGW_B[0].size());
    result.slotsInA = sumA;
    result.slotsInB = sumB;

    printf("Length of the schedule: %d \n", dataStructure->maxSlot);
    FreeDataStructure(&dsA);
    FreeDataStructure(&dsB);


    delete[] scheduleGW_A;
    delete[] scheduleGW_B;
    delete[] scheduleA;
    delete[] scheduleB;
    return result;
}

bool IterativeChannelScheduler::TerminationCondition(const ChannelsFitness &cf) {
    iterationNumber++;
    hash<string> hash_fn;
    std::size_t hashValue = hash_fn(cf.str());

    if (iterationNumber >= maxIterationCount)
        return false;
    if (cycleCheckSet.find(hashValue) != cycleCheckSet.end())
        return false;

    cycleCheckSet.insert(hashValue);
    return true;
}

double IterativeChannelScheduler::UpdateBeta(const ChannelSchedulerResult &result) {
    return sqrt(result.slotsInA / (1.0 * result.slotsInB));
}
