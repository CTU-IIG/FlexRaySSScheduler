/// \file FFCIncrementalScheduler.cpp Incremental Scheduler
#include "MVFRScheduler.h"
#include <stack>

int AssureThatSlotExists(DataStructure *dataStructure, std::vector<CFrame> *schedule, int slot) {
    int i;
    while ((int) schedule[0].size() <= slot) {
        for (i = 0; i < dataStructure->hyperPeriod; i++) {
            CFrame frame;
            schedule[i].push_back(frame);
        }
    }
    return 0;
}

int PlaceSignalToOriginalSchedule(DataStructure *dataStructure, std::vector<CFrame> *schedule,
                                  std::vector<ConflictNode> &conflictGraph, int *confMapFunction,
                                  int *scheduleNodeMap, int signalIndex, int oldScheduleLength) {
    int i, j, a1, a2, b1, b2, cycle, slot;
    dataStructure->signalSlot[signalIndex] = scheduleNodeMap[
            (dataStructure->signalNodeIDs[signalIndex] - 1) * (oldScheduleLength + 1)
            + dataStructure->oldSchedule.signalSlot[signalIndex]];
    dataStructure->signalStartCycle[signalIndex] = dataStructure->oldSchedule.signalStartCycle[signalIndex];
    dataStructure->signalInFrameOffsets[signalIndex] = dataStructure->oldSchedule.signalInFrameOffsets[signalIndex];
    a1 = dataStructure->signalInFrameOffsets[signalIndex];
    a2 = a1 + dataStructure->signalLengths[signalIndex];
    slot = dataStructure->signalSlot[signalIndex];
    AssureThatSlotExists(dataStructure, schedule, slot);
    for (i = 0; i < dataStructure->hyperPeriod / dataStructure->signalPeriods[signalIndex]; i++) {
        cycle = dataStructure->signalStartCycle[signalIndex] + i * dataStructure->signalPeriods[signalIndex];
        for (j = 0; j < (int) schedule[cycle][slot].signals.size(); j++) {
            if (getMEMatrixValue(dataStructure->mutualExclusionMatrix, signalIndex, schedule[cycle][slot].signals[j])) {
                b1 = dataStructure->signalInFrameOffsets[schedule[cycle][slot].signals[j]];
                b2 = b1 + dataStructure->signalLengths[schedule[cycle][slot].signals[j]];
                if (((b1 >= a1) && (b1 < a2)) // Test zda li signály kolidují
                    || ((a1 >= b1) && (a1 < b2))
                    || ((b2 > a1) && (b2 <= a2))
                    || ((a2 > b1) && (a2 <= b2))) {
                    if (confMapFunction[signalIndex] == -1) // Pokud se jedná o první konflikt prvního signálu
                    {
                        ConflictNode node;
                        node.nodeID = signalIndex;
                        conflictGraph.push_back(node);
                        confMapFunction[signalIndex] = static_cast<int>(conflictGraph.size() - 1);
                    }
                    if (confMapFunction[schedule[cycle][slot].signals[j]] ==
                        -1) // Pokud se jedná o první konflikt druhého signálu
                    {
                        ConflictNode node;
                        node.nodeID = schedule[cycle][slot].signals[j];
                        conflictGraph.push_back(node);
                        confMapFunction[schedule[cycle][slot].signals[j]] = static_cast<int>(conflictGraph.size() - 1);
                    }

                    conflictGraph[confMapFunction[signalIndex]].nodeNeighbours.insert(schedule[cycle][slot].signals[j]);
                    conflictGraph[confMapFunction[schedule[cycle][slot].signals[j]]].nodeNeighbours.insert(signalIndex);
                }
            }
        }
        schedule[cycle][slot].signals.push_back(signalIndex);
    }
    return 0;
}

int FFCIncrementalSlotScheduler(DataStructure *dataStructure, int oldScheduleLength,
                                std::vector<int> *nodeSlotSignalsCount, int *nodeSlots, int *scheduleNodeMap,
                                int *rSlotsAssignment) {
    int i, j, k, l, tmp;
    int slotsCount;
    std::vector<int> classicSlotScheduling; // Sloty, které se budou muset rozvrhnout pomocí graph coloringu
    std::vector<ConflictNode> conflictGraph; // Graf konfliktù
    int *nodeSlotIndexes = (int *) mmalloc(
            dataStructure->nodeCount * sizeof(int)); // Od jakého indexu v slotsNode zaèínají sloty daného nodu

    slotsCount = 0;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        slotsCount += nodeSlots[i];
    }

    int *slotsNode = (int *) ccalloc(slotsCount, sizeof(int)); // K jakému nodu slot patøí
    int *confMapFunction = (int *) mmalloc(slotsCount * sizeof(int));

    tmp = 0;
    for (i = 0; i < dataStructure->nodeCount; i++)// pøedpoèítávání struktur
    {
        nodeSlotIndexes[i] = tmp;
        for (j = 0; j < nodeSlots[i]; j++) {
            slotsNode[tmp] = i;
            rSlotsAssignment[tmp] = -1;
            tmp++;
        }
    }

    // slotSchedule[i][j] øíká, který slot je v rozvrhu ve slot ID i rozvrhnut jako j_tý slot
    std::vector<int> *slotSchedule = new std::vector<int>[oldScheduleLength];

    j = 0;
    for (i = 0; i < slotsCount; i++) // Rozvrhni sloty dle pùvodního rozvrhu
    {
        confMapFunction[i] = -1;
        if ((i == 0) || (slotsNode[i - 1] != slotsNode[i]))
            j = 0;
        if (i - nodeSlotIndexes[slotsNode[i]] < scheduleNodeMap[(oldScheduleLength + 1) * slotsNode[i] +
                                                                oldScheduleLength]) { // Pokud je slot v pùvodním rozvrhu
            while ((scheduleNodeMap[(oldScheduleLength + 1) * slotsNode[i] + j] != i - nodeSlotIndexes[slotsNode[i]]) &&
                   (j < oldScheduleLength))
                j++;
            if (j < oldScheduleLength) {
                rSlotsAssignment[i] = j;
                slotSchedule[j].push_back(i);
                j++;
            }
            else {
                error("Fatal error: Graph coloring for incremental scheduling failure!");
                return 0;
            }
        }
        else {
            classicSlotScheduling.push_back(i);
        }
    }

    // Najdi kolize
    for (i = 0; i < oldScheduleLength; i++) {
        for (j = 0; j < (int) slotSchedule[i].size(); j++) {
            for (k = j + 1; k < (int) slotSchedule[i].size(); k++) {
                if (getMEMatrixValue(dataStructure->mutualExclusionNodeMatrix, slotsNode[slotSchedule[i][j]],
                                     slotsNode[slotSchedule[i][k]])) {
                    if (confMapFunction[slotSchedule[i][j]] == -1) // Pokud se jedná o první konflikt prvního signálu
                    {
                        ConflictNode node;
                        node.nodeID = slotSchedule[i][j];
                        conflictGraph.push_back(node);
                        confMapFunction[slotSchedule[i][j]] = static_cast<int>(conflictGraph.size() - 1);
                    }
                    if (confMapFunction[slotSchedule[i][k]] == -1) // Pokud se jedná o první konflikt druhého signálu
                    {
                        ConflictNode node;
                        node.nodeID = slotSchedule[i][k];
                        conflictGraph.push_back(node);
                        confMapFunction[slotSchedule[i][k]] = static_cast<int>(conflictGraph.size() - 1);
                    }

                    conflictGraph[confMapFunction[slotSchedule[i][j]]].nodeNeighbours.insert(slotSchedule[i][k]);
                    conflictGraph[confMapFunction[slotSchedule[i][k]]].nodeNeighbours.insert(slotSchedule[i][j]);
                }
            }
        }
    }

    if (conflictGraph.size() > 0) {
        int *slotsDegOfFree = (int *) ccalloc(dataStructure->nodeCount, sizeof(int));
        //int degOfFree; // Poèet slotID, do kterých lze ještì slot rozvrhnout
        int freeFlag; // Pøíznak, zda-li je možné slot do daného SlotID pøiøadit
        for (i = 0; i < oldScheduleLength; i++) {
            for (j = 0; j < dataStructure->nodeCount; j++) {
                freeFlag = 1;
                for (k = 0; k < (int) slotSchedule[i].size(); k++) {
                    if ((slotsNode[slotSchedule[i][k]] == j) ||
                        getMEMatrixValue(dataStructure->mutualExclusionNodeMatrix,
                                         j, slotsNode[slotSchedule[i][k]])) {
                        freeFlag = 0;
                        break;
                    }
                }
                slotsDegOfFree[j] += freeFlag;
            }
        }
#ifdef GUROBI
        findSlotIndependentSetGurobi(dataStructure, conflictGraph, nodeSlotSignalsCount, slotsNode,
                                     nodeSlotIndexes, confMapFunction, slotsDegOfFree, oldScheduleLength);
#else
        findSlotIndependentSet(dataStructure, conflictGraph, nodeSlotSignalsCount, slotsNode,
                               nodeSlotIndexes, confMapFunction, slotsDegOfFree, oldScheduleLength);
#endif
        ffree(slotsDegOfFree);
    }

    // Smaž sloty, které nejsou v nezávislé množinì a pøidej je k rozvržení
    for (j = 0; j < (int) conflictGraph.size(); j++) {
        if (!conflictGraph[j].independent) // Pokud nepatøí slot do nejvìtší nezávislé množiny, smaž jej z pùvodního rozvrhu
            // a pøidej do množiny slotù k rozvržení
        {
            // Najdi èíslo slotu kde se v pùvodním rozvrhu "slot" nacházel
            k = rSlotsAssignment[conflictGraph[j].nodeID]; // k = èíslo multislotu ke kterému je slot pøiøazen
            l = 0; // l = index "slotu" v multislotu
            while ((l < (int) slotSchedule[k].size()) && (slotSchedule[k][l] != conflictGraph[j].nodeID))
                l++;

            slotSchedule[k].erase(slotSchedule[k].begin() + l);
            rSlotsAssignment[conflictGraph[j].nodeID] = -1;
            classicSlotScheduling.push_back(conflictGraph[j].nodeID);
        }
    }

    int UB, LB, Opt, swap, last;
    incremetalGraphColoring(dataStructure, nodeSlots, &UB, &LB, &Opt, rSlotsAssignment, scheduleNodeMap,
                            oldScheduleLength);

    // Update signalSlot
    dataStructure->maxSlot = Opt;
    last = nodeSlots[0];
    nodeSlots[0] = 0;
    for (i = 1; i <
                dataStructure->nodeCount; i++) // Kumulativní souèet - v nodeSlots poté index prvního slotu daného nodu v slotAssignment
    {
        swap = nodeSlots[i];
        nodeSlots[i] = nodeSlots[i - 1] + last;
        last = swap;
    }

    for (i = 0; i < dataStructure->signalsCount; i++) {
        dataStructure->signalSlot[i] = rSlotsAssignment[nodeSlots[dataStructure->signalNodeIDs[i] - 1] +
                                                        dataStructure->signalSlot[i]];
    }

    ffree(nodeSlotIndexes);
    ffree(slotsNode);
    ffree(confMapFunction);
    delete[] slotSchedule;
    return 0;
}

int PrepareNewIncremDataStructure(DataStructure *dataStructure, DataStructure *newDS, int *&signalMapping, int node,
                                  int slot, float freeBand) {
    int i, j, tmp;
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
        printf("");
    }

    tmp = 0;
    for (i = 0; i < dataStructure->signalsCount; i++)
        if ((dataStructure->signalNodeIDs[i] - 1 == node) && ((dataStructure->signalSlot[i] == slot) || (slot < 0)))
            tmp++;

    newDS->signalsCount = tmp + static_cast<int>(newSignalsPayload.size());
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
        if ((dataStructure->signalNodeIDs[i] - 1 == node) && ((dataStructure->signalSlot[i] == slot) || (slot < 0))) {
            signalMapping[tmp] = i;
            newDS->signalDeadlines[tmp] = dataStructure->signalDeadlines[i];
            newDS->signalLengths[tmp] = dataStructure->signalLengths[i];
            newDS->signalNodeIDs[tmp] = dataStructure->signalNodeIDs[i];
            newDS->signalPeriods[tmp] = dataStructure->signalPeriods[i];
            newDS->signalReleaseDates[tmp] = dataStructure->signalReleaseDates[i];
            newDS->signalInFrameOffsets[tmp] = dataStructure->signalInFrameOffsets[i];
            newDS->signalSlot[tmp] = dataStructure->signalSlot[i];
            newDS->signalStartCycle[tmp] = dataStructure->signalStartCycle[i];
            tmp++;
        }
    }
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
    return tmp;
}

int ReleaseNewIncremDataStructure(DataStructure *newDS, int *signalMapping) {
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

int FFCIncrementalScheduler(DataStructure *dataStructure) {
    int i, j, k, l, max;
    // Init fáze
    max = -1; // Zjištìní poètu slotù v pùvodním rozvrhu
    for (i = 0; i < dataStructure->oldSchedule.signalsCount; i++) {
        if (dataStructure->oldSchedule.signalSlot[i] > max)
            max = dataStructure->oldSchedule.signalSlot[i];
    }
    max++;

    // Matice pocet_nodu*pocet_slotu. Na indexu [i,j] je -1 pokud node i ve slotu j nevysila jinak je tam poøadové
    // èíslo jeho slotu (1 = první, 2 = druhý) který v daném slotu vysílá.
    int *scheduleNodeMap = (int *) mmalloc(
            (max + 1) * dataStructure->nodeCount * sizeof(int)); // Poslední pole je pro èítaè
    for (i = 0; i < (max + 1) * dataStructure->nodeCount; i++) {
        if (i % (max + 1) == max)
            scheduleNodeMap[i] = 0;
        else
            scheduleNodeMap[i] = -1;
    }

    for (i = 0; i < dataStructure->oldSchedule.signalsCount; i++) // Najde využíté sloty daných nodù
    {
        if (scheduleNodeMap[(dataStructure->signalNodeIDs[i] - 1) * (max + 1) +
                            dataStructure->oldSchedule.signalSlot[i]] == -1) {
            scheduleNodeMap[(dataStructure->signalNodeIDs[i] - 1) * (max + 1) +
                            dataStructure->oldSchedule.signalSlot[i]] = 1;
            scheduleNodeMap[(dataStructure->signalNodeIDs[i] - 1) * (max + 1) + max]++;
        }
    }


    for (i = 0; i < dataStructure->nodeCount; i++) // Vytvoøí mapovací matici
    {
        k = 0;
        for (j = 0; j < max; j++) {
            if (scheduleNodeMap[i * (max + 1) + j] != -1) {
                scheduleNodeMap[i * (max + 1) + j] = k;
                k++;
            }
        }
    }

    // 1. a 2. fáze - hledání konfliktù a vytváøení rozvrhù v rámci nodu
    int sum, last;
    std::vector<CFrame> *schedule = new std::vector<CFrame>[dataStructure->hyperPeriod];
    std::vector<CFrame> *scheduleCopy = new std::vector<CFrame>[dataStructure->hyperPeriod];
    std::vector<int> *nodeSlotSignalsCount = new std::vector<int>[dataStructure->nodeCount]; // Kolik signálù je rozvržených v daném slotu daného nodu
    int *aux = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    int *nodeSlots = (int *) mmalloc(dataStructure->nodeCount * sizeof(int));
    int *signalOrdering = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));

    for (i = 0; i < dataStructure->signalsCount; i++)
        signalOrdering[i] = i;

    mergeSort(signalOrdering, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
              LENGTHDEC);
    mergeSort(signalOrdering, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
              WINDOW);
    mergeSort(signalOrdering, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
              PERIOD);
    mergeSort(signalOrdering, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator, NODE);
    ffree(aux);

    std::vector<int> classicScheduling; // Signály, které bude tøeba pøerozvrhnout
    std::vector<ConflictNode> conflictGraph;
    int *conflictMapFunction = (int *) mmalloc(
            dataStructure->signalsCount * sizeof(int)); // Pro každý signál urèuje pøípadnou pozici v grafu kolizí
    for (i = 0; i < dataStructure->signalsCount; i++)
        conflictMapFunction[i] = -1;
    sum = 0; // celkový počet rozvrhovaných slotù
    last = 0;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        classicScheduling.clear();
        conflictGraph.clear();
        for (j = 0; j < dataStructure->hyperPeriod; j++) // Inicializuj nový rozvrh
            schedule[j].clear();

        for (j = last;
             j < last + dataStructure->signalsInNodeCounts[i]; j++) // Sestroj pùvodní rozvrh a nalezni konflikty
        {
            if (signalOrdering[j] >= dataStructure->oldSchedule.signalsCount)
                classicScheduling.push_back(signalOrdering[j]);
            else
                PlaceSignalToOriginalSchedule(dataStructure, schedule, conflictGraph, conflictMapFunction,
                                              scheduleNodeMap, signalOrdering[j], max);
        }

        // Solve independent sets in conflictGraph
        if (conflictGraph.size() > 0)
#ifdef GUROBI
            findIndependentSetGurobi(dataStructure, conflictGraph, conflictMapFunction);
#else
            findIndependentSet(dataStructure, conflictGraph, conflictMapFunction);
#endif

        // Delete signals that are not in the independent set form schedule and add them to the classicScheduling
        for (j = 0; j < (int) conflictGraph.size(); j++) {
            if (!conflictGraph[j].independent) // Pokud nepatøí signál do nejvìtší nezávislé množiny, smaž jej z pùvodního rozvrhu
                // a pøidej do množiny signálù k rozvržení
            {
                for (k = dataStructure->signalStartCycle[conflictGraph[j].nodeID];
                     k < dataStructure->hyperPeriod; k = k + dataStructure->signalPeriods[conflictGraph[j].nodeID]) {
                    for (l = 0;
                         l < (int) schedule[k][dataStructure->signalSlot[conflictGraph[j].nodeID]].signals.size(); l++)
                        if (schedule[k][dataStructure->signalSlot[conflictGraph[j].nodeID]].signals[l] ==
                            conflictGraph[j].nodeID) {
                            schedule[k][dataStructure->signalSlot[conflictGraph[j].nodeID]].signals.erase(
                                    schedule[k][dataStructure->signalSlot[conflictGraph[j].nodeID]].signals.begin() +
                                    l);
                            break;
                        }
                }
                classicScheduling.push_back(conflictGraph[j].nodeID);
            }
        }

        // vytvor kopii rozvrhu pro ucely nasledujiciho prerozvrzeni
        for (j = 0; j < dataStructure->hyperPeriod; j++) {
            scheduleCopy[j] = schedule[j];
        }

        // Schedule signals in the classicScheduliung to the roster
        for (j = 0; j < (int) classicScheduling.size(); j++) {
            PlaceFFCSignalToSchedule(dataStructure, schedule, classicScheduling[j]);
        }

        switch (dataStructure->isIncOptimized) {
            case 1:
                incrementalSingleSlotExtensibilityOptimization(dataStructure, i, schedule, scheduleCopy,
                                                               classicScheduling);
                break;
            case 2:
                incrementalMultipleSlotExtensibilityOptimization(dataStructure, i, schedule, scheduleCopy,
                                                                 classicScheduling);
                break;
            default:
                break;
        }

        for (j = 0; j < (int) schedule[0].size(); j++)
            nodeSlotSignalsCount[i].push_back(0);
        nodeSlots[i] = static_cast<int>(schedule[0].size());
        sum += schedule[0].size();
        last = last + dataStructure->signalsInNodeCounts[i];
    }

    for (i = 0; i < dataStructure->signalsCount; i++) {
        nodeSlotSignalsCount[dataStructure->signalNodeIDs[i] - 1][dataStructure->signalSlot[i]]++;
    }

    int *rSlotsAssignment = (int *) mmalloc(sum * sizeof(int));
    FFCIncrementalSlotScheduler(dataStructure, max, nodeSlotSignalsCount, nodeSlots, scheduleNodeMap, rSlotsAssignment);

    ffree(rSlotsAssignment);
    ffree(nodeSlots);
    ffree(signalOrdering);
    ffree(scheduleNodeMap);
    ffree(conflictMapFunction);
    delete[] schedule;
    delete[] nodeSlotSignalsCount;
    return 0;
}

void incrementalSingleSlotExtensibilityOptimization(DataStructure *dataStructure, const int node,
                                                    std::vector<CFrame> *schedule, const std::vector<CFrame> *scheduleCopy,
                                                    const std::vector<int> &classicScheduling) {
    for (unsigned long j = 0; j < schedule[0].size(); j++) {
        float tmp = ComputeSlotUtilization(dataStructure, j, schedule);
        while (tmp < 1) {
            int *signalMapping;
            int *ordering;
            int *aux2;
            std::vector<int> signalMappingReverse(dataStructure->signalsCount);
            int toScheduleFromOrig; // Kolik signálù z pùvodního slotu se má pøerozvrhnout
            std::vector<CFrame> *slotSchedule = new std::vector<CFrame>[dataStructure->hyperPeriod];
            std::set<int> schedulingSet(classicScheduling.begin(), classicScheduling.end());
            std::vector<int> signalsToSchedule; // Signály k pøerozvržení (vèetnì novì vytvoøených)
            if (verboseLevel == 2) printf("%d, %d - %f\n", node, j, tmp);
            DataStructure ds;
            PrepareNewIncremDataStructure(dataStructure, &ds, signalMapping, node, j, 1 - tmp);
            toScheduleFromOrig = 0;
            for (int k = 0; k < ds.signalsCount; k++) {
                if (schedulingSet.find(signalMapping[k]) != schedulingSet.end()) {
                    toScheduleFromOrig++;
                    signalsToSchedule.push_back(k);
                }
                else {
                    if (signalMapping[k] == -1)
                        signalsToSchedule.push_back(k);
                }
            }
            if (toScheduleFromOrig < 1) {
                ReleaseNewIncremDataStructure(&ds, signalMapping);
                if (verboseLevel == 2) printf("Nothing to reschedule \n");
                break;
            }
            for (int k = 0; k < ds.signalsCount; k++) {
                if (signalMapping[k] != -1) {
                    signalMappingReverse[signalMapping[k]] = k;
                }
            }
            ordering = static_cast<int *>(mmalloc(signalsToSchedule.size() * sizeof(int)));
            aux2 = static_cast<int *>(mmalloc(signalsToSchedule.size() * sizeof(int)));
            for (unsigned long k = 0; k < signalsToSchedule.size(); k++)
                ordering[k] = signalsToSchedule[k];
            mergeSort(ordering, aux2, &ds, 0, toScheduleFromOrig - 1, mergeSortSignalComparator, LENGTHDEC);
            mergeSort(ordering, aux2, &ds, 0, toScheduleFromOrig - 1, mergeSortSignalComparator, WINDOW);
            mergeSort(ordering, aux2, &ds, 0, static_cast<int>(signalsToSchedule.size() - 1),
                      mergeSortSignalComparator, PERIOD);
            for (int k = 0; k < dataStructure->hyperPeriod; k++) {
                slotSchedule[k].clear();
                if (j < static_cast<int>(scheduleCopy[k].size())) {
                    slotSchedule[k].push_back(scheduleCopy[k][j]);
                    for (unsigned long l = 0; l < slotSchedule[k][0].signals.size(); l++) {
                        slotSchedule[k][0].signals[l] = signalMappingReverse[slotSchedule[k][0].signals[l]];
                    }
                }
            }

            for (unsigned long k = 0; k < signalsToSchedule.size(); k++)
                PlaceFFCSignalToSchedule(&ds, slotSchedule, ordering[k]);

            if (verboseLevel == 2)
                printf("newut: %f, %d\n", ComputeSlotUtilization(&ds, 0, slotSchedule),
                       static_cast<int>(slotSchedule[0].size()));
            ffree(aux2);

            if (slotSchedule[0].size() > 1)
                tmp = tmp * 1.05f;
            else {
                // update original signals;
                for (int k = 0; k < ds.signalsCount; k++) {
                    if (signalMapping[k] != -1) {
                        dataStructure->signalStartCycle[signalMapping[k]] = ds.signalStartCycle[k];
                        dataStructure->signalInFrameOffsets[signalMapping[k]] = ds.signalInFrameOffsets[k];
                    }
                }
                tmp = 1;
            }
            ffree(ordering);
            ReleaseNewIncremDataStructure(&ds, signalMapping);
        }
    }
}

void incrementalMultipleSlotExtensibilityOptimization(DataStructure *dataStructure, const int node,
                                                      std::vector<CFrame> *schedule, const std::vector<CFrame> *scheduleCopy,
                                                      const std::vector<int> &classicScheduling) {
    std::set<int> schedulingSet(classicScheduling.begin(), classicScheduling.end()); // Set of signals to be scheduled
    float tmp = ComputeMultiScheduleUtilization(dataStructure, schedule);
    const unsigned long scheduleLength = schedule[0].size();
    while (tmp < scheduleLength) {
        int *signalMapping;
        int *ordering;
        int *aux2;
        std::vector<int> signalMappingReverse(dataStructure->signalsCount);
        int toScheduleFromOrig; // Kolik signálù z pùvodního slotu se má pøerozvrhnout
        std::vector<CFrame> *slotSchedule = new std::vector<CFrame>[dataStructure->hyperPeriod];
        std::vector<int> signalsToSchedule; // Signály k pøerozvržení (vèetnì novì vytvoøených)
        if (verboseLevel == 2) printf("%d - %f\n", node, tmp);
        DataStructure ds;
        PrepareNewIncremDataStructure(dataStructure, &ds, signalMapping, node, -1, 1 - tmp);
        toScheduleFromOrig = 0;
        for (int k = 0; k < ds.signalsCount; k++) {
            if (schedulingSet.find(signalMapping[k]) != schedulingSet.end()) {
                toScheduleFromOrig++;
                signalsToSchedule.push_back(k);
            }
            else {
                if (signalMapping[k] == -1)
                    signalsToSchedule.push_back(k);
            }
        }
        if (toScheduleFromOrig < 1) {
            ReleaseNewIncremDataStructure(&ds, signalMapping);
            if (verboseLevel == 2) printf("Nothing to reschedule \n");
            break;
        }
        for (int k = 0; k < ds.signalsCount; k++) {
            if (signalMapping[k] != -1) {
                signalMappingReverse[signalMapping[k]] = k;
            }
        }
        ordering = static_cast<int *>(mmalloc(signalsToSchedule.size() * sizeof(int)));
        aux2 = static_cast<int *>(mmalloc(signalsToSchedule.size() * sizeof(int)));
        for (unsigned long k = 0; k < signalsToSchedule.size(); k++)
            ordering[k] = signalsToSchedule[k];
        mergeSort(ordering, aux2, &ds, 0, toScheduleFromOrig - 1, mergeSortSignalComparator, LENGTHDEC);
        mergeSort(ordering, aux2, &ds, 0, toScheduleFromOrig - 1, mergeSortSignalComparator, WINDOW);
        mergeSort(ordering, aux2, &ds, 0, static_cast<int>(signalsToSchedule.size() - 1),
                  mergeSortSignalComparator, PERIOD);
        for (int k = 0; k < dataStructure->hyperPeriod; k++) { // Překopírování a mapování původního rozvrhu
            slotSchedule[k].clear();
            for (unsigned long j = 0; j < scheduleCopy[k].size(); j++) {
                slotSchedule[k].push_back(scheduleCopy[k][j]);
                for (unsigned long l = 0; l < slotSchedule[k][j].signals.size(); l++) {
                    slotSchedule[k][j].signals[l] = signalMappingReverse[slotSchedule[k][j].signals[l]];
                }
            }
        }

        for (unsigned long k = 0; k < signalsToSchedule.size(); k++)
            PlaceFFCSignalToSchedule(&ds, slotSchedule, ordering[k]);

        if (verboseLevel == 2)
            printf("newut: %f, %d\n", ComputeSlotUtilization(&ds, 0, slotSchedule),
                   static_cast<int>(slotSchedule[0].size()));
        ffree(aux2);

        if (slotSchedule[0].size() > scheduleLength)
            tmp = tmp * 1.05f;
        else {
            // update original signals;
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
        ReleaseNewIncremDataStructure(&ds, signalMapping);
    }
}