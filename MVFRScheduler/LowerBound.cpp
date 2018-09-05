/// \file LowerBound.cpp Lower bound computation
#include "MVFRScheduler.h"
#include "glpk.h"

using namespace std;

int ComputeTrivialLowerBound(DataStructure *dataStructure) {
    int i, j;
    int result;
    int sum;
    int maxSum = 0;
    for (i = 0; i < dataStructure->variantCount; i++) {
        sum = 0;
        for (j = 0; j < dataStructure->signalsCount; j++) {
            if (dataStructure->modelVariants[i][j])
                sum += dataStructure->signalLengths[j] * dataStructure->hyperPeriod / dataStructure->signalPeriods[j];
        }
        if (maxSum < sum) {
            maxSum = sum;
        }
    }

    result = static_cast<int>(ceil(maxSum / static_cast<double>(dataStructure->slotLength) /
                                   static_cast<double>(dataStructure->hyperPeriod)));
    return result;
}

float ComputeSlotUtilization(DataStructure *dataStructure, int slot, std::vector<CFrame> *schedule) {
    int i, j, k, tmp;
    float result = 0;
    int *messageArray = static_cast<int *>(ccalloc(dataStructure->slotLength,
                                                   sizeof(int))); // Vyuzite bity v ramci (vyuzito protoze mame multivariantnost a tak nelze jednoduse pouzit soucet payloadu signalu)
    if (static_cast<int>(schedule[0].size()) <= slot)
        return -1;
    for (i = 0; i < dataStructure->hyperPeriod; i++) {
        for (j = 0; j < static_cast<int>(schedule[i][slot].signals.size()); j++) {
            tmp = schedule[i][slot].signals[j];
            for (k = dataStructure->signalInFrameOffsets[tmp];
                 k < dataStructure->signalInFrameOffsets[tmp] + dataStructure->signalLengths[tmp]; k++)
                messageArray[k] = 1;
        }
        for (j = 0; j < dataStructure->slotLength; j++) {
            if (messageArray[j] == 1)
                result++;
            messageArray[j] = 0;
        }
    }
    ffree(messageArray);
    return result / dataStructure->slotLength / dataStructure->hyperPeriod;
}

float ComputeMultiScheduleUtilization(DataStructure *dataStructure, std::vector<CFrame> *schedule) {
    float result = 0;
    vector<int> messageVector(dataStructure->slotLength, 0);
    for (unsigned int i = 0; i < schedule[0].size(); i++) {
        for (int j = 0; j < dataStructure->hyperPeriod; j++) {
            for (int signal : schedule[j][i].signals) {
                int signalLength = dataStructure->signalLengths[signal];
                int signalOffset = dataStructure->signalInFrameOffsets[signal];
                for (int offset = signalOffset; offset < signalOffset + signalLength; offset++)
                    messageVector[offset] = 1;
            }
            for (int &bit: messageVector) {
                if (bit == 1)
                    result++;
                bit = 0;
            }
        }
    }
    return result / dataStructure->slotLength / dataStructure->hyperPeriod;
}

int ComputeBetterLowerBound(DataStructure *dataStructure) {
    int i, j;
    int result;
    int sum;
    int maxSum = 0;
    int last = -1;
    int *signalIndexArray = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    int *aux = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    for (i = 0; i < dataStructure->signalsCount; i++)
        signalIndexArray[i] = i;
    mergeSort(signalIndexArray, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
              NODE);
    ffree(aux);
    for (i = 0; i < dataStructure->variantCount; i++) {
        sum = 0;
        for (j = 0; j < dataStructure->signalsCount; j++) {
            if (last != dataStructure->signalNodeIDs[signalIndexArray[j]]) {
                last = dataStructure->signalNodeIDs[signalIndexArray[j]];
                if ((sum % (dataStructure->hyperPeriod * dataStructure->slotLength) != 0)) // ceiling
                {
                    sum += ((dataStructure->hyperPeriod) * dataStructure->slotLength) -
                           sum % ((dataStructure->hyperPeriod) * dataStructure->slotLength);
                }
            }
            if (dataStructure->modelVariants[i][signalIndexArray[j]])
                sum += dataStructure->signalLengths[signalIndexArray[j]] *
                       dataStructure->hyperPeriod / dataStructure->signalPeriods[signalIndexArray[j]];
        }
        if (maxSum < sum) {
            maxSum = sum;
        }
        // TODO: Remove!!!
        int res = static_cast<int>(ceil((sum / static_cast<double>(dataStructure->slotLength)) /
                                        static_cast<double>(dataStructure->hyperPeriod)));
        printf("%d\n", res);
    }

    ffree(signalIndexArray);
    result = static_cast<int>(ceil((maxSum / static_cast<double>(dataStructure->slotLength)) /
                                   static_cast<double>(dataStructure->hyperPeriod)));
    return result;
}

int ComputeNodeSlots(DataStructure *dataStructure,
                     int *nodeSlots) // V�po�et kolik p�i nejmen��m mus� b�t rozvrhnuto slot� pro jednotliv� nody
{
    int i, j;
    int tmp;
    int sum;
    int last;
    int *signalIndexArray = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    int *aux = (int *) mmalloc(dataStructure->signalsCount * sizeof(int));
    for (i = 0; i < dataStructure->signalsCount; i++)
        signalIndexArray[i] = i;
    mergeSort(signalIndexArray, aux, dataStructure, 0, dataStructure->signalsCount - 1, mergeSortSignalComparator,
              NODE);
    ffree(aux);
    for (i = 0; i < dataStructure->nodeCount; i++)
        nodeSlots[i] = 0;
    for (i = 0; i < dataStructure->variantCount; i++) {
        sum = 0;
        last = -1;
        for (j = 0; j < dataStructure->signalsCount; j++) {
            if (last != dataStructure->signalNodeIDs[signalIndexArray[j]] - 1) {
                if (last != -1) {
                    tmp = static_cast<int>(ceil((sum / static_cast<double>(dataStructure->slotLength)) /
                                                static_cast<double>(dataStructure->hyperPeriod)));
                    if (tmp > nodeSlots[last]) {
                        nodeSlots[last] = tmp;
                    }
                }
                last = dataStructure->signalNodeIDs[signalIndexArray[j]] - 1;
                sum = 0;
            }
            if (dataStructure->modelVariants[i][signalIndexArray[j]]) {
                sum += dataStructure->signalLengths[signalIndexArray[j]] *
                       dataStructure->hyperPeriod / dataStructure->signalPeriods[signalIndexArray[j]];
            }
        }
        if (last != -1) {
            tmp = static_cast<int>(ceil((sum / static_cast<double>(dataStructure->slotLength)) /
                                        static_cast<double>(dataStructure->hyperPeriod)));
            if (tmp > nodeSlots[last]) {
                nodeSlots[last] = tmp;
            }
        }
    }

    ffree(signalIndexArray);
    return 1;
}

int ComputeLBForGraphColoring(DataStructure *dataStructure, int *nodesLongestVariant) // V�po�et LB pro graph coloring
{
    int result;
    int *nodeSlots = (int *) mmalloc(
            dataStructure->nodeCount * sizeof(int)); // Kolik slot� je pot�eba minim�ln� rozvrhnout
    ComputeNodeSlots(dataStructure, nodeSlots);
    result = ComputeLBForGraphColoring(dataStructure, nodesLongestVariant, nodeSlots);
    ffree(nodeSlots);
    return result;
    /*int i, j;
    int tmp;
    int sum;
    int maxSum = 0;
    int last = -1;
    int* signalIndexArray = (int*)mmalloc(dataStructure->signalsCount*sizeof(int));
    int* nodeSlots = (int*)mmalloc(dataStructure->nodeCount*sizeof(int)); // Kolik slot� je pot�eba minim�ln� rozvrhnout
    int* variantNodes = (int*)ccalloc(dataStructure->variantCount*dataStructure->nodeCount,sizeof(int)); // Zda-li varianta i obsahuje node j
    int* aux = (int*)mmalloc(dataStructure->signalsCount*sizeof(int));
    for(i = 0; i < dataStructure->signalsCount; i++)
        signalIndexArray[i] = i;
    mergeSort(signalIndexArray, aux, dataStructure, 0, dataStructure->signalsCount-1, mergeSortSignalComparator, NODE);
    ffree(aux);
    for(i = 0; i < dataStructure->variantCount; i++)
    {
        sum = 0;
        last = -1;
        for(j = 0; j < dataStructure->signalsCount; j++)
        {
            if(last != dataStructure->signalNodeIDs[signalIndexArray[j]]-1)
            {
                if(last != -1)
                {
                    tmp = (int)ceil((sum/(double)dataStructure->slotLength)/(double)(dataStructure->hyperPeriod/dataStructure->cycleLength));
                    if(tmp > nodeSlots[last])
                        nodeSlots[last] = tmp;
                }
                last = dataStructure->signalNodeIDs[signalIndexArray[j]]-1;
                sum = 0;
            }
            if(dataStructure->modelVariants[i][signalIndexArray[j]])
            {
                sum += dataStructure->signalLengths[signalIndexArray[j]] * \
                dataStructure->hyperPeriod/dataStructure->signalPeriods[signalIndexArray[j]];
                variantNodes[i*dataStructure->nodeCount+(dataStructure->signalNodeIDs[signalIndexArray[j]]-1)] = 1;
            }
        }
        if(last != -1)
        {
            tmp = (int)ceil((sum/(double)dataStructure->slotLength)/(double)(dataStructure->hyperPeriod/dataStructure->cycleLength));
            if(tmp > nodeSlots[last])
                nodeSlots[last] = tmp;
        }
    }

    for(i = 0; i < dataStructure->variantCount; i++)
    {
        sum = 0;
        for(j = 0; j < dataStructure->nodeCount; j++)
        {
            if(variantNodes[i*dataStructure->nodeCount+j])
                sum += nodeSlots[j];
        }
        if(maxSum < sum)
            maxSum = sum;
    }

    ffree(signalIndexArray);
    ffree(nodeSlots);
    ffree(variantNodes);
    return maxSum;*/
}

int ComputeLBForGraphColoring(DataStructure *dataStructure, int *nodesLongestVariant,
                              int *nodeSlots) // V�po�et LB pro graph coloring
{
    int i, j;
    int sum;
    int maxSum = 0;
    int longestVariant = 0;
    int *variantNodes = (int *) ccalloc(dataStructure->variantCount * dataStructure->nodeCount,
                                        sizeof(int)); // Zda-li varianta i obsahuje node j
    for (i = 0; i < dataStructure->variantCount; i++) {
        for (j = 0; j < dataStructure->signalsCount; j++) {
            if (dataStructure->modelVariants[i][j]) {
                variantNodes[i * dataStructure->nodeCount + (dataStructure->signalNodeIDs[j] - 1)] = 1;
            }
        }
    }

    for (i = 0; i < dataStructure->variantCount; i++) {
        sum = 0;
        for (j = 0; j < dataStructure->nodeCount; j++) {
            if (variantNodes[i * dataStructure->nodeCount + j])
                sum += nodeSlots[j];
        }
        if (maxSum < sum) {
            maxSum = sum;
            longestVariant = i;
        }
    }

    for (i = 0; i < dataStructure->nodeCount; i++) {
        if (variantNodes[longestVariant * dataStructure->nodeCount + i])
            nodesLongestVariant[i] = 1;
        else
            nodesLongestVariant[i] = 0;
    }

    ffree(variantNodes);
    return maxSum;
}


int t(int x, int y) {
    if (x == 0)
        return 0;
    else {
        if (x % 2 == 0)
            return t(x / 2, y / 2);
        else
            return t((x - 1) / 2, y / 2) + y / 2;
    }
}

int ComputeGCLowerBound(DataStructure *dataStructure) {
    int i, LB, UB, Opt;
    int *nodeSlots = (int *) mmalloc(dataStructure->nodeCount * sizeof(int));

    ComputeNodeSlots(dataStructure, nodeSlots);
    UB = 0;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        UB += nodeSlots[i];
    }
    int *sa = (int *) mmalloc(UB * sizeof(int));

    graphColoring(dataStructure, nodeSlots, &UB, &LB, &Opt, sa);

    ffree(nodeSlots);
    ffree(sa);
    return LB;
}
