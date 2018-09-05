/// \file IndependentSet.h Header file for the IndependentSet.cpp
#ifndef INDEPENDENTSET_H_
#define INDEPENDENTSET_H_

#include "MVFRScheduler.h"

/// Method for finding the independent set in the conflict graph
/// @param dataStructure Global attributes
/// @param conflictGraph Graph with the conflict signals (the signals that would cause conflict in new variant)
/// @param conflictMapFunction Array to map between signal ID and node in graph associated to this signal ID
int findIndependentSet(DataStructure *dataStructure, std::vector<ConflictNode> &conflictGraph,
                       int *conflictMapFunction);

/// Method for finding the independent set in the conflict graph of nodes
/// @param dataStructure Global attributes
/// @param conflictGraph Graph with the conflict signals (the signals that would cause conflict in new variant)
/// @param nodeSlotSignalsCount Number of signals that particular node transmitts in the particular node
/// @param slotsNode Map array - to which node the slot is assigned
/// @param nodeSlotIndexes From which index in slotsNode begins the slots of the particular node
/// @param conflictMapFunction Array to map between signal ID and node in graph associated to this signal ID
/// @param slotDegOfFree Number of slots to which we are able to schedule slots of the particular node
/// @param oldScheduleLength The number of slots used by original schedule
int findSlotIndependentSet(DataStructure *dataStructure, std::vector<ConflictNode> &conflictGraph,
                           std::vector<int> *nodeSlotSignalsCount,
                           int *slotsNode, int *nodeSlotIndexes, int *conflictMapFunction, int *slotDegOfFree,
                           int oldScheduleLength);

#endif