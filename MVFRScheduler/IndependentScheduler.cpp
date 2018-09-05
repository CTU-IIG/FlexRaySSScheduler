/// \file IndependentScheduler.cpp Independent scheduler for comparing number of used slots by independent scheduling and multivariant scheduling
#include "IndependentScheduler.h"

int IndependentScheduler(DataStructure *dataStructure) {
    int i, j, k;
    int signalsCount, signalIndex, nodeIndex;
    int maxIndependent = 0;
    int *nodes = static_cast<int *>(mmalloc(dataStructure->nodeCount * sizeof(int)));
    int *signalInNodeCount = static_cast<int *>(ccalloc(dataStructure->nodeCount, sizeof(int)));
    for (i = 0; i < dataStructure->variantCount; i++) {
        DataStructure ds;
        ds.algorithm = dataStructure->algorithm;
        ds.cycleLength = dataStructure->cycleLength;
        ds.hyperPeriod = dataStructure->hyperPeriod;
        ds.maxSlot = dataStructure->maxSlot;
        ds.schedulerParameters = dataStructure->schedulerParameters;
        ds.slotLength = dataStructure->slotLength;
        ds.variantCount = 1;
        signalsCount = 0;
        for (j = 0; j < dataStructure->nodeCount; j++) {
            nodes[j] = 0;
        }
        for (j = 0; j < dataStructure->signalsCount; j++) // count number of signals in variant
        {
            if (dataStructure->modelVariants[i][j] == 1) {
                signalInNodeCount[dataStructure->signalNodeIDs[j] - 1] += 1;
                nodes[dataStructure->signalNodeIDs[j] - 1] = 1;
                signalsCount++;
            }
        }
        ds.signalsCount = signalsCount;
        for (j = 0; j < dataStructure->nodeCount; j++)
            nodes[j] = 1 - nodes[j];
        for (j = 1; j < dataStructure->nodeCount; j++)
            nodes[j] = nodes[j] + nodes[j - 1];

        ds.nodeCount = dataStructure->nodeCount - nodes[dataStructure->nodeCount - 1];
        ds.signalsInNodeCounts = static_cast<int *>(ccalloc(ds.nodeCount, sizeof(int)));

        // alloc data structures
        ds.signalDeadlines = static_cast<int *>(mmalloc(sizeof(int) * ds.signalsCount));
        ds.signalInFrameOffsets = static_cast<int *>(mmalloc(sizeof(int) * ds.signalsCount));
        ds.signalLengths = static_cast<int *>(mmalloc(sizeof(int) * ds.signalsCount));
        ds.signalNodeIDs = static_cast<int *>(mmalloc(sizeof(int) * ds.signalsCount));
        ds.signalPeriods = static_cast<int *>(mmalloc(sizeof(int) * ds.signalsCount));
        ds.signalReleaseDates = static_cast<int *>(mmalloc(sizeof(int) * ds.signalsCount));
        ds.signalSlot = static_cast<int *>(mmalloc(sizeof(int) * ds.signalsCount));
        ds.signalStartCycle = static_cast<int *>(mmalloc(sizeof(int) * ds.signalsCount));
        ds.modelVariants = static_cast<char **>(mmalloc(sizeof(char *)));
        ds.modelVariants[0] = static_cast<char *>(mmalloc(sizeof(char) * ds.signalsCount));
        int *signalMapper = static_cast<int *>(mmalloc(sizeof(int) * ds.signalsCount));
        int *nodeMapper = static_cast<int *>(mmalloc(
                sizeof(int) * ds.nodeCount)); // Id nodu v nové instanci je jaké ID v původní instanci?

        nodeIndex = 0;
        if (nodes[0] == 0) {
            nodeMapper[0] = 1;
            nodeIndex++;
        }
        for (j = 1; j < dataStructure->nodeCount; j++) {
            if (nodes[j] == nodes[j - 1]) {
                nodeMapper[nodeIndex] = j + 1;
                nodeIndex++;
            }
        }

        signalIndex = 0;
        for (j = 0; j < dataStructure->signalsCount; j++) // set signals values
        {
            if (dataStructure->modelVariants[i][j] == 1) {
                signalMapper[signalIndex] = j;
                ds.signalDeadlines[signalIndex] = dataStructure->signalDeadlines[j];
                ds.signalLengths[signalIndex] = dataStructure->signalLengths[j];
                ds.signalNodeIDs[signalIndex] =
                        dataStructure->signalNodeIDs[j] - nodes[dataStructure->signalNodeIDs[j] - 1];
                ds.signalPeriods[signalIndex] = dataStructure->signalPeriods[j];
                ds.signalReleaseDates[signalIndex] = dataStructure->signalReleaseDates[j];
                ds.modelVariants[0][signalIndex] = 1;
                signalIndex++;
            }
        }

        for (j = 0; j < ds.signalsCount; j++)
            ds.signalsInNodeCounts[ds.signalNodeIDs[j] - 1] += 1;

        ds.mutualExclusionMatrix = static_cast<char *>(ccalloc(ds.signalsCount * (ds.signalsCount + 1) / 2,
                                                               sizeof(char)));
        ds.mutualExclusionNodeMatrix = static_cast<char *>(ccalloc(ds.nodeCount * (ds.nodeCount + 1) / 2,
                                                                   sizeof(char)));
        for (j = 0; j < ds.signalsCount; j++) {
            for (k = j + 1; k < ds.signalsCount; k++) {
                // if (getMEMatrixValue(dataStructure->mutualExclusionMatrix, signalMapper[j], signalMapper[k]))
                setMEMatrixValue(ds.mutualExclusionMatrix, j, k, 1);
            }
        }

        for (j = 0; j < ds.nodeCount; j++) {
            for (k = j + 1; k < ds.nodeCount; k++) {
                // if (getMEMatrixValue(dataStructure->mutualExclusionNodeMatrix, nodeMapper[j], nodeMapper[k]))
                setMEMatrixValue(ds.mutualExclusionNodeMatrix, j, k, 1);
            }
        }

        int *aux = static_cast<int *>(mmalloc(sizeof(int) * ds.signalsCount));
        int *ordering = static_cast<int *>(mmalloc(ds.signalsCount * sizeof(int)));
        for (j = 0; j < ds.signalsCount; j++)
            ordering[j] = j;
        mergeSort(ordering, aux, &ds, 0, ds.signalsCount - 1, mergeSortSignalComparator, LENGTHDEC);
        mergeSort(ordering, aux, &ds, 0, ds.signalsCount - 1, mergeSortSignalComparator, WINDOW);
        mergeSort(ordering, aux, &ds, 0, ds.signalsCount - 1, mergeSortSignalComparator, PERIOD);
        mergeSort(ordering, aux, &ds, 0, ds.signalsCount - 1, mergeSortSignalComparator, NODE);
        FirstFitScheduler(&ds, ordering);
        if (maxIndependent < ds.maxSlot)
            maxIndependent = ds.maxSlot;
        ffree(aux);
        ffree(ordering);
        ffree(ds.mutualExclusionMatrix);
        ffree(ds.mutualExclusionNodeMatrix);
        ffree(ds.signalDeadlines);
        ffree(ds.signalInFrameOffsets);
        ffree(ds.signalLengths);
        ffree(ds.signalNodeIDs);
        ffree(ds.signalPeriods);
        ffree(ds.signalReleaseDates);
        ffree(ds.signalsInNodeCounts);
        ffree(ds.signalSlot);
        ffree(ds.signalStartCycle);
        ffree(ds.modelVariants[0]);
        ffree(ds.modelVariants);
        ffree(signalMapper);
        ffree(nodeMapper);
    }
    ffree(nodes);
    ffree(signalInNodeCount);

    int *aux2 = static_cast<int *>(mmalloc(sizeof(int) * dataStructure->signalsCount));
    int *ordering2 = static_cast<int *>(mmalloc(dataStructure->signalsCount * sizeof(int)));
    for (i = 0; i < dataStructure->signalsCount; i++)
        ordering2[i] = i;
    mergeSort(ordering2, aux2, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator, LENGTHDEC);
    mergeSort(ordering2, aux2, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator, WINDOW);
    mergeSort(ordering2, aux2, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator, PERIOD);
    mergeSort(ordering2, aux2, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator, NODE);
    FirstFitScheduler(dataStructure, ordering2);
    printf("%d/%d\n", maxIndependent, dataStructure->maxSlot);
    IndependentParameters *params = static_cast<IndependentParameters *>(dataStructure->schedulerParameters);
    if (params->saveResultToFile != nullptr)
        AppendIndependentTestResults(dataStructure->maxSlot, maxIndependent, params->saveResultToFile);
    ffree(aux2);
    ffree(ordering2);
    return 1;
}