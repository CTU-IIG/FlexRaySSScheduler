/// \file LowerBound.h Header file for LowerBound.cpp
#ifndef LOWERBOUND_H_
#define LOWERBOUND_H_

#include "MVFRScheduler.h"

/// Compute the trivial lower bound
/// @param dataStructure Global attributes
int ComputeTrivialLowerBound(DataStructure *dataStructure);

/// Compute Utilization of the slot
/// @param dataStructure Global attributes
/// @param slot ID of the slot for which utilization shoud be computed
/// @param schedule The schedule for utilization computation.
float ComputeSlotUtilization(DataStructure *dataStructure, int slot, std::vector<CFrame> *schedule);

/// Compute Utilization of the schedule
/// @param dataStructure Global attributes
/// @param schedule The schedule for utilization computation.
float ComputeMultiScheduleUtilization(DataStructure *dataStructure, std::vector<CFrame> *schedule);

/// Compute better lower bound (take different nodes into account)
/// @param dataStructure Global attributes
int ComputeBetterLowerBound(DataStructure *dataStructure);

/// Compute lower bound for graph coloring algorithm
/// @param dataStructure Global attributes
/// @param nodesLongestVariant Binary array of flags that says wich nodes are used in the longest variant
/// @param nodeSlots Number of slots scheduled for particular node 
int ComputeLBForGraphColoring(DataStructure *dataStructure, int *nodesLongestVariant, int *nodeSlots);

/// Compute lower bound for graph coloring algorithm
/// @param dataStructure Global attributes
/// @param nodesLongestVariant Binary array of flags that says wich nodes are used in the longest variant
int ComputeLBForGraphColoring(DataStructure *dataStructure, int *nodesLongestVariant);

/// Compute lower bound for number of used slots for all the nodes through all the variants
/// @param dataStructure Global attributes
/// @param nodeSlots Number of slots scheduled for particular node 
int ComputeNodeSlots(DataStructure *dataStructure, int *nodeSlots);

/// Compute lower bound using graph coloring algorithm
/// @param dataStructure Global attributes
int ComputeGCLowerBound(DataStructure *dataStructure);

/// Transforamtion from FlexRay scheduling to binpacking [not used]
/// @param x Cycle ID
/// @param y Hyperperiod
int t(int x, int y);

#endif