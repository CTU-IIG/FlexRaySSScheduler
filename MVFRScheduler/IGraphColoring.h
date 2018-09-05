/// \file IGraphColoring.h Header file for IGraphColoring.cpp
#ifndef IGRAPHCOLORING_H_
#define IGRAPHCOLORING_H_

#include "MVFRScheduler.h"

/// Exact graph coloring algorithm for incremental scheduling
/// @param dataStructure Global attributes
/// @param nodeSlots Number of slots scheduled for particular node
/// @param rUB Upper bound to return
/// @param rLB Lower bound to return
/// @param rOpt Optimal value to return
/// @param rSlotsAssignment Array that assign Slot ID in resulting multischedule to the slots of nodes (slot scheduling)
/// @param scheduleNodeMap Array that map between slot ID in multischedule and and slot ID in node multischedules
/// @param oldScheduleLength The number of slots used by original schedule
int incremetalGraphColoring(DataStructure *dataStructure, int *nodeSlots, int *rUB, int *rLB, int *rOpt,
                            int *rSlotsAssignment, int *scheduleNodeMap, int oldScheduleLength);

/// Heuristic graph coloring algorithm for incremental scheduling (Upper bound computation)
/// @param dataStructure Global attributes
/// @param nodeSlots Number of slots scheduled for particular node
/// @param rSlotsAssignment Array that assign Slot ID in resulting multischedule to the slots of nodes (slot scheduling)
/// @return Upper bound
int incrementalGraphColoringHeuristic(DataStructure *dataStructure, int *nodeSlots, int *rSlotsAssignment);

#endif