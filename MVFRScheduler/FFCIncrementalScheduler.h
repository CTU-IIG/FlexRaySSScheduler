/// \file FFCIncrementalScheduler.h Header file for FFCIncrementalScheduler.cpp
#ifndef FFCINCREMENTALSCHEDULER_H_
#define FFCINCREMENTALSCHEDULER_H_

#include "MVFRScheduler.h"

/// Node for the conflict graph
struct ConflictNode {
    int nodeID; ///< ID of the conflict graph node
    char independent; ///< Flag if the node is the part of the independent set
    std::set<int> nodeNeighbours; ///< Neighbours of the current graph node
};

/// Entry point to the incremental scheduler 
/// @param dataStructure Global attributes
int FFCIncrementalScheduler(DataStructure *dataStructure);

/// Place the signal to the original schedule. If it cause the conflict, add the signal to the conflict graph
/// @param dataStructure Global attributes
/// @param schedule The original schedule
/// @param conflictGraph Graph with the conflict signals (the signals that would cause conflict in new variant)
/// @param confMapFunction Array to map between signal ID and node in graph associated to this signal ID
/// @param scheduleNodeMap Array that map between slot ID in multischedule and and slot ID in node multischedules
/// @param signalIndex ID of the signal that should be placed
/// @param oldScheduleLength The number of slots used by original schedule
int PlaceSignalToOriginalSchedule(DataStructure *dataStructure, std::vector<CFrame> *schedule,
                                  std::vector<ConflictNode> &conflictGraph, int *confMapFunction,
                                  int *scheduleNodeMap, int signalIndex, int oldScheduleLength);

/// Method checks if slot with slot ID exists. If not, it allocates such a slot.
/// @param dataStructure Global attributes
/// @param schedule The schedule structure
/// @param slot ID of the slot
int AssureThatSlotExists(DataStructure *dataStructure, std::vector<CFrame> *schedule, int slot);

/// Method that prepare new DataStructure for slot optimization
/// @param dataStructure Global attributes
/// @param newDS New auxiliary data structure
/// @param signalMapping Signal indexes mapping function from newDS to the original dataStructure
/// @param node NodeID where the optimization occurs
/// @param slot ID of the slot for optimization
/// @param freeBand Ratio of the free bandwidth in the slot
/// @return Number of real signals form schedule
int PrepareNewIncremDataStructure(DataStructure *dataStructure, DataStructure *newDS, int *&signalMapping, int node,
                                  int slot, float freeBand);

/// Method releases all the structures prepared by the PerpareNewIncremDataStructure method
/// @param newDS New auxiliary data structure
/// @param signalMapping Signal indexes mapping function from newDS to the original dataStructure
int ReleaseNewIncremDataStructure(DataStructure *newDS, int *signalMapping);

/// Method that places slots from the temporal schedules to the final multischedule
/// @param dataStructure Global attributes
/// @param oldScheduleLength Length of the original schedule
/// @param nodeSlotSignalsCount How many signals is in the particular slots of particular ECUs
/// @param nodeSlots How many slots should be assigned for particular ECU
/// @param scheduleNodeMap Special mapping function f(node, multischedule slotID) -> temporal schedule slot ID
/// @param rSlotsAssignment Resulting assignment of slots from temporal schedules to multischedule
int FFCIncrementalSlotScheduler(DataStructure *dataStructure, int oldScheduleLength,
                                std::vector<int> *nodeSlotSignalsCount, int *nodeSlots, int *scheduleNodeMap,
                                int *rSlotsAssignment);

/// Extensibility optimization for one slot case
/// @param dataStructure Global attributes
/// @param node NodeID for which the optimization occurs
/// @param schedule Multischedule for assignment
/// @param scheduleCopy Copy of the schedule with only original signals
/// @param classicScheduling Vector of the signals which are not in original schedule
void incrementalSingleSlotExtensibilityOptimization(DataStructure *dataStructure, const int node,
                                                    std::vector<CFrame> *schedule,
                                                    const std::vector<CFrame> *scheduleCopy,
                                                    const std::vector<int> &classicScheduling);

/// Extensibility optimization for multiple slots case
/// @param dataStructure Global attributes
/// @param node NodeID for which the optimization occurs
/// @param schedule Multischedule for assignment
/// @param scheduleCopy Copy of the schedule with only original signals
/// @param classicScheduling Vector of the signals which are not in original schedule
void incrementalMultipleSlotExtensibilityOptimization(DataStructure *dataStructure, const int node,
                                                      std::vector<CFrame> *schedule,
                                                      const std::vector<CFrame> *scheduleCopy,
                                                      const std::vector<int> &classicScheduling);

#endif