///
/// \mainpage MultiVariant FlexRay Static Segment Scheduler
/// The source file of the MultiVariant FlexRay Static Segment Scheduler with Time Constraints
///
/// \section usage Usage of application
/// <pre>
/// MVFRScheduler [-v1|-v2|-v3] [-a[s|v|f|c|g|l|i|d]] [-p[s|g[p|g]] paramValue] [-c CVSFile] [-o OutputFile] benchmarkInstance
/// </pre>
/// \section options Options
/// <pre>
/// -v	verbosity level {0, 1, 2, 3}
/// -a	Used algorithm
///		-as			Two stage algorithm
///		-avp		Native First Fit algorithm - signals ordered by increasing period
///		-avw		Native First Fit algorithm - signals ordered by increasing window
///		-avl		Native First Fit algorithm - signals ordered by decreasing length
///		-ava		Native First Fit algorithm - signals ordered by combination of parameters
///		-af[pwla]	First Fit algorithm - signals ordered by increasing period
///		-ac[pwla]	Improved First Fit algorithm - signals ordered by increasing period
///		-ag			Genetic scheduler - Genetic framework over the first fit scheduler
///		-al			Lower bound computation
///		-ai			Incremental scheduler - signals ordered by combination of parameters
///		-ad[pwla]	Scheduler for comparison of independent a multivariant schedules
///	-p	Schedulers parameters
///		-ps			Signal assigning threshold for Two stage algorithm
///		-pgp		The size of the population in the Genetic scheduler
///		-pgg		The maximal number of generation for Genetic scheduler
/// -c	Save resulting values to the CVS file for statistic
/// -o	Save resulting schedule to the output file
/// </pre>
///

/// \file MVFRScheduler.cpp Entry point to the MVFRScheduler.

#include "MVFRScheduler.h"
using namespace std;

char verboseLevel = 0; //0 - nothing, 1 - classical output, // 2 - detailed, // 3 - all

int runAlgorithm(DataStructure *dataStructure) {
    int *ordering;
    int *aux;
    clock_t startTime, endTime;

    switch (dataStructure->algorithm) {
        case TWOSTAGE:
            StageScheduler(dataStructure);
            break;
        case COMMON:
        case FIRSTFIT:
        case FIRSTFITCOLOR:
        case BESTFIT:
        case FIRSTFITVARIANT: {
            ordering = static_cast<int *>(mmalloc(dataStructure->signalsCount * sizeof(int)));
            aux = static_cast<int *>(mmalloc(dataStructure->signalsCount * sizeof(int)));
            for (int i = 0; i < dataStructure->signalsCount; i++)
                ordering[i] = i;
            switch (static_cast<FirstFitParameters *>(dataStructure->schedulerParameters)->sortType) {
                case FFPERIOD:
                    mergeSort(ordering, aux, dataStructure, 0, dataStructure->signalsCount - 1,
                              mergeSortSignalComparator, PERIOD);
                    break;
                case FFLENGTHDEC:
                    mergeSort(ordering, aux, dataStructure, 0, dataStructure->signalsCount - 1,
                              mergeSortSignalComparator, LENGTHDEC);
                    break;
                case FFWINDOW:
                    mergeSort(ordering, aux, dataStructure, 0, dataStructure->signalsCount - 1,
                              mergeSortSignalComparator, PERIOD);
                    mergeSort(ordering, aux, dataStructure, 0, dataStructure->signalsCount - 1,
                              mergeSortSignalComparator, WINDOW);
                    break;
                case FFALL:
                    mergeSort(ordering, aux, dataStructure, 0, dataStructure->signalsCount - 1,
                              mergeSortSignalComparator, LENGTHDEC);
                    mergeSort(ordering, aux, dataStructure, 0, dataStructure->signalsCount - 1,
                              mergeSortSignalComparator, WINDOW);
                    mergeSort(ordering, aux, dataStructure, 0, dataStructure->signalsCount - 1,
                              mergeSortSignalComparator, PERIOD);
                    mergeSort(ordering, aux, dataStructure, 0, dataStructure->signalsCount - 1,
                              mergeSortSignalComparator, NODE);
                default:
                    break;
            }
            if (dataStructure->algorithm == FIRSTFITVARIANT)
                FirstFitVariantScheduler(dataStructure, ordering);
            else if (dataStructure->algorithm == FIRSTFITCOLOR || dataStructure->algorithm == COMMON)
                FirstFitColorScheduler(dataStructure, ordering);
            else if (dataStructure->algorithm == FIRSTFIT)
                FirstFitScheduler(dataStructure, ordering);
            else if (dataStructure->algorithm == BESTFIT)
                BestFitColorScheduler(dataStructure, ordering);
            ffree(ordering);
            ffree(aux);
            break;
        }
        case GENETIC:
            GeneticScheduler(dataStructure);
            break;
        case LOWERBOUND:
            dataStructure->maxSlot = ComputeGCLowerBound(dataStructure);
            break;
        case LOWERBOUNDINDEPENDENT:
            dataStructure->maxSlot = ComputeBetterLowerBound(dataStructure);
            break;
        case INCREMENTAL:
            FFCIncrementalScheduler(dataStructure);
            break;
        case INDEPENDENT:
            IndependentScheduler(dataStructure);
            break;
        case CHANNELASSIGNER:
            if (dataStructure->isReceivers) {
                CAMessage *messages;
                unsigned int messageCount = static_cast<unsigned int>(AggregateMessages(dataStructure, messages));
                int sumW = 0;
                for (int i = 0; i < messageCount; i++) {
                    for (int j = 0; j < static_cast<int>(messages[i].nodes.size()); j++) {
                        if (dataStructure->nodesOnBothChannels[messages[i].nodes[j]] != 1) {
                            sumW += messages[i].payload;
                            break;
                        }
                    }
                }
                ChannelSchedulerTest(dataStructure, 1.0 / sumW);
                //ChannelSchedulerTest(dataStructure, 0.0001);
                delete[] messages;
            }
            else {
                error("For channel assigner and scheduler, the recievers should be specified!");
            }
            break;
        case ANALYSIS:
            FRAnalyzer(dataStructure);
            break;
        case VOLUME: {
            vector<int> bitCounts(dataStructure->variantCount, 0);
            for (int i = 0; i < dataStructure->signalsCount; i++) {
                for (int j = 0; j < dataStructure->variantCount; j++) {
                    if (dataStructure->modelVariants[j][i] == 1)
                        bitCounts[j] += dataStructure->signalLengths[i] * dataStructure->hyperPeriod /
                                        dataStructure->signalPeriods[i];
                }
            }
            dataStructure->maxSlot = *std::max_element(bitCounts.begin(), bitCounts.end());
            break;
        }
        default:
            break;
    }
    return 1;
}


int main(int argc, char *argv[]) {
    int allRight = 0;
    clock_t startTime, endTime;
    DataStructure dataStructure;
    if (parseInput(argc, argv, &dataStructure)) {
        allRight = 1;
        if (UpdateInputData(&dataStructure))
            return 0;
        if (verboseLevel > 0)
            printf("Starting scheduling algorithm\n");
        startTime = clock();
        dataStructure.maxSlot = -1;
        runAlgorithm(&dataStructure);
        endTime = clock();
        dataStructure.elapsedTime = ((static_cast<float>(endTime) - static_cast<float>(startTime)) / 1000000.0F) * 1000;
        printf("%.3lf\n", dataStructure.elapsedTime);
        if (verboseLevel > 0)
            printf("Scheduling finished\n");
        if (dataStructure.saveResultToFile) {
            if (dataStructure.algorithm != LOWERBOUND) {
                if (dataStructure.algorithm != ANALYSIS) {
                    if (verboseLevel > 0)
                        printf("Saving results to output file %s\n", dataStructure.outputFile);
                    switch (dataStructure.algorithm) {
                        case EXPORT:
                            printf("Hi baj\n");
                            allRight = ConfigExporter(&dataStructure);
                            break;
                        case FIBEX:
                            allRight = FibexExporter(&dataStructure);
                            break;
                        case GANTT:
                            allRight = createGanttChart(&dataStructure);
                            break;
                        default:
                            allRight = saveToOutputFile(&dataStructure);
                    }
                }
            }
            else {
                printf("There is no feasible schedule to be saved if using lower bound computing algorithm!\n");
            }
        }
        if (dataStructure.appendToCSV) {
            if (verboseLevel > 0)
                printf("Saving results to CSV file %s\n", dataStructure.csvFile);
            allRight = AppendResultsToCSV(&dataStructure);
        }
        if (dataStructure.algorithm != LOWERBOUND && dataStructure.algorithm != VOLUME &&
            dataStructure.algorithm != LOWERBOUNDINDEPENDENT && !dataStructure.isReceivers)
            CheckSchedule(&dataStructure);
        if (allRight && (verboseLevel > 0)) {
            printf("All is done, press any key to continue!\n");
        }
    }

    FreeDataStructure(&dataStructure);
    verboseAllocInfo = 1;
    ffreeAll();
    if (verboseLevel > 0) {
        getchar();
    }
    return 0;
}