/// \file GDS.h Structures used for benchmark instance generator.

#ifndef GDS_H_ // GeneratorDataStructure
#define GDS_H_

#include<vector>


/// Additional parameters for the incremental scheduler.
struct IncrementalParameters
{
	int addedSignalsCount; ///< Number of new signals in the instance
	int addedNodesCount; ///< Number of new nodes in the instance
};

/// Generated attributes of the benchmark instance.
struct BenchmarkInstance
{
	int* signalPeriods; ///< Period of the signal
	int* signalReleaseDates; ///< Release date of the signal
	int* signalDeadlines; ///< Deadline of the signals
	int* signalPayloads; ///< Payload of the signals
	int* signalNodeIDs; ///< Node ID of the signals
	int* signalsInNodeCounts; ///< Number of signals transmitted by one node
	int** modelVariants; ///< Matrix of signals assigned to the model variant
	std::vector<int> *signalReceivers; ///< Which ECUs receive particular signal
	char* nodesOnBothChannels; ///< Synchronization nodes that are connected to the both channels
};

/// Generator data structure - global attributes for the generator.
struct GDS
{
	int cycleLength; ///< Length of cycle
	int slotLength; ///< Length of slot
	int signalsCount; ///< Number of signals
	int variantsCount; ///< Number of variants
	int* periodProbabilities; ///< Probability of specified period
	int* payloadProbabilities; ///< Probability of specified payload
	int*receiversProbabilities; ///< Probability of the signal that has given number of receivers
	int releasePortion; ///< Portion of signals with release date
	int deadlinePortion; ///< Portion of signals with deadline
	int nodeCount; ///< Number of nodes
	int maxSignalPayload; ///< Maximal signal payload
	int commonNodesPortion; ///< Portion of common nodes
	int specificNodesPortion; ///< Portion of specific nodes
	int commonSignalsPortion; ///< Portion of common signals
	int specificSignalsPortion; ///< Portion of specific signals
    int isMultiVariantTestingMode; ///< Flag if we are going to generate multivariant testing instances
    int isMultiVariantIncrementalTestingMode; ///< Flag if we are going to generate instances for multivariant vs incremental testing
	BenchmarkInstance benchmarkInstance; ///< Generated benchmark instance
	IncrementalParameters incrementalParameters; ///< Parameters for incremental instance generator
	unsigned int seed; ///< Seed for random generator
	int syncNodePercent; ///< How many percent of nodes are sync nodes
	int faultTolerantPromile; ///< How many signals in the sync nodes will be Fault tolerant - for channel scheduling
	char isIncremental; ///< Is the scheduling Incremental
	char isReceivers; ///< Should be generated also receivers of the message
	char isFaultTolerant; ///< Should be some signals fault tolerant
	char* configFileName; ///< File name of the configuration file
	char* outputFileName; ///< File name of the output file
    char *origBenchInstFileName; ///< File name of the original benchmark instance for incremental scheduling
};

#endif