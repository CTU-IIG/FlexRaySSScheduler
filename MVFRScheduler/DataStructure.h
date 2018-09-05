/// \file DataStructure.h Basic data structures used by scheduling algorithms

#ifndef DATASTRUCTURE_H_
#define DATASTRUCTURE_H_

#include "MVFRScheduler.h"

/// Type of the algorithm to be used
enum AlgorithmType {
    TWOSTAGE, ///< Two stage algorithm (frame packing + frame scheduling)
    FIRSTFIT, ///< First fit algorithm with multischedules
    FIRSTFITVARIANT, ///< Naive first fit algorithm with naive schedules
    FIRSTFITCOLOR, ///< Improved first fit algorithm with multischedules that use graph coloring for slot scheduling
    GENETIC, ///< Genetic scheduler - Genetic framework over schedulers
    LOWERBOUND, ///< Just compute the lower bound for multivariant schedule
    LOWERBOUNDINDEPENDENT, ///< Just compute the lower bound for independent variant schedules
    INCREMENTAL, ///< Incremental scheduler
    INDEPENDENT, ///< Scheduler for comparison of independent a multivariant schedules
    BESTFIT, ///< Best fit color scheduler
    CHANNELASSIGNER, ///< Algorithm for assigning nodes to channels and scheduling for such a cluster
    EXPORT, ///< Export schedule to the FlexRay controller configuration file
    FIBEX, ///< Export schedule to the FIBEX database
    ANALYSIS, ///< Create the analysis output of the input instance
    GANTT, ///< Generate GanttChart from the schedule
    VOLUME, ///< Compute number of used slots for the preemptive messages (number of bits needed to transmitt during one hyperperiod)
    COMMON, ///< Create schedule for the case with only one variant that has all the signals (ignore variants)
};

/// Old schedule for incremental scheduling
struct OldSchedule {
    OldSchedule() : signalInFrameOffsets(nullptr), signalStartCycle(nullptr), signalSlot(nullptr) { }

    int signalsCount; ///< Number of signals
    int *signalInFrameOffsets; ///< Offset of the signal in the frame
    int *signalStartCycle; ///< Cycle ID in which the first occurrence of the signal is scheduled
    int *signalSlot; ///< Slot ID in which the signal transmission is scheduled
};

/// Statistics for optimization for incremental scheduling
struct IncremOptStatistics {
    IncremOptStatistics() : periodSignalCount(nullptr), payloadSignalCount(nullptr) { }

    int *periodSignalCount; ///< Number of signals with specified period
    int *payloadSignalCount; ///< Number of signals with specified payload
    int maximalPayload; ///< The length of the biggest signal
};

/// Basic data structure with the global attributes
struct DataStructure {
    int signalsCount; ///< Number of signals to schedule
    int cycleLength; ///< The length of the cycle
    int slotLength; ///< The length(payload) of the slot (in bits)
    int variantCount; ///< Number of variants to be generated
    int nodeCount; ///< Number of nodes
    int hyperPeriod; ///< Length of hyperperiod
    int *signalsInNodeCounts; ///< Number of signals scheduled to the particular node
    int *signalNodeIDs; ///< Transmitting Node ID of signals
    char **signalNames; ///< Names of signals
    int *signalPeriods; ///< Periods of signals
    int *signalReleaseDates; ///< Release dates of signals
    int *signalDeadlines; ///< Deadlines of signals
    int *signalLengths; ///< Payload of the signal
    char *mutualExclusionMatrix; ///< Signal Mutual Exclusion Matrix (SMEM)
    char *mutualExclusionNodeMatrix; ///< Node Mutual Exclusion Matrix (NMEM)
    char *nodesOnBothChannels; ///< Nodes that are forced to be connected to the both channels (SyncNodes)
    std::vector<char> signalFaultTolerant; ///< Signals that are forced to be submitted to both channels.
    char *nodesChannel; ///< Channel in which the node is assigned to (if node is in the both channels the value in this array has no meaning)
    int *signalInFrameOffsets; ///< Offset of the signal in the frame
    int *signalStartCycle; ///< Cycle ID where the first occurrence of the signal is scheduled
    int *signalSlot; ///< Slot ID where the signal is scheduled
    char saveResultToFile; ///< Save result to the file flag
    char *outputFile; ///< Output file for the resulting schedule
    char appendToCSV; ///< Append to the CVS file flag
    char *csvFile; ///< CVS file for statistic
    int maxSlot; ///< Number of used slots in resulting schedule
    char **modelVariants; ///< Flags for signals if they are assigned to the variants
    AlgorithmType algorithm; ///< The used algorithm
    void *schedulerParameters; ///< Parameters for particular scheduler
    float elapsedTime; ///< The time used for scheduling
    OldSchedule oldSchedule; ///< The old schedule for incremental scheduler
    IncremOptStatistics incoStatistics; ///< The statistics for incremental scheduler
    char isIncOptimized; ///< Flag if the optimization for incremental scheduling is used
    char isReceivers; ///< Should be the ECUs clustered to the channels
    std::vector<int> *signalReceivers; ///< Which ECUs receive particular signal
};

/// The method creates deep copy of DataStructure
/// @param newOne The pointer to the structure that should be created
/// @param originalOne The pointer to the structure that should be copied
void MakeDeepCopyOfDataStructure(DataStructure *newOne, DataStructure *originalOne);

/// Method for releasing all allocated memory
/// @param dataStructure The structure with global data
void FreeDataStructure(DataStructure *dataStructure);

void moveOriginalScheduleToCurrentSchedule(DataStructure *dataStructure);

void getNodeToSlotAssignment(const DataStructure *dataStructure, std::set<int> *nodeSlotAssignment);

#endif /* DATASTRUCTURE_H_ */
