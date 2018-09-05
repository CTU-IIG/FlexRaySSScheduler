/// \file FirstFitColorScheduler.h Header file for the FirstFitColorScheduler.cpp
#ifndef FIRSTFITCOLORSCHEDULER_H_
#define FIRSTFITCOLORSCHEDULER_H_

#include "MVFRScheduler.h"
#include "Position.h"
#include "CFrame.h" // Why???

/// Extensibility optimization for one slot case
/// @param dataStructure Global attributes
/// @param node NodeID for which the optimization occurs
/// @param schedule Multischedule for assignment
void oneSlotExtensibilityOptimization(DataStructure *dataStructure, const int node, std::vector<CFrame> *schedule);

/// Extensibility optimization for multiple slot case
/// @param dataStructure Global attributes
/// @param node NodeID for which the optimization occurs
/// @param schedule Multischedule for assignment
void multiSlotExtensibilityOptimization(DataStructure *dataStructure, const int node, std::vector<CFrame> *schedule);

/// Find first free position for the signal in the frame or signalize that it is not possible to schedule signal to the frame
/// @param dataStructure Global attributes
/// @param frame The frame where the method is looking to.
/// @param signalIndex Index of signal for which the method is looking for a place
/// @param startPosition The index where it should start to look to.
int CFindFreePositionInFrame(DataStructure *dataStructure, CFrame *frame, int signalIndex, int startPosition);

/// Test if it is possible to schedule signal to the frame and to all of the following frames (because of period)
/// @param dataStructure Global attributes
/// @param frame The frame where the method is looking to.
/// @param inFrameOffset Offset of the signal in the frame
/// @param signalIndex Index of the signal
int CTestSignalInFrame(DataStructure *dataStructure, CFrame *frame, int inFrameOffset, int signalIndex);

/// Find first Slot/Cycle position where the signal could be placed
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param signalIndex Index of the signal
/// @param startPosition The slot/cycle position where the searching should start.
Position CFindFirstCyclePositionForSignal(DataStructure *dataStructure, std::vector<CFrame> *schedule, int signalIndex,
                                          Position startPosition);

/// Method just place signal to the schedule
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param position Position in multischedule where the signal should be placed to
/// @param signalIndex Index of the signal that should be placed.
int CPlaceSignalToFrame(DataStructure *dataStructure, std::vector<CFrame> *schedule, Position position,
                        int signalIndex);

/// Find the first position where the signal can be placed and place it there
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param signalIndex Index of the signal that should be placed.
int PlaceFFCSignalToSchedule(DataStructure *dataStructure, std::vector<CFrame> *schedule, int signalIndex);

/// Method that prepare new DataStructure for slot optimization
/// @param dataStructure Global attributes
/// @param newDS New auxiliary data structure
/// @param signalMapping Signal indexes mapping function from newDS to the original dataStructure
/// @param node NodeID where the optimization occurs
/// @param slot ID of the slot for optimization
/// @param freeBand Ratio of the free bandwidth in the slot
/// @return Number of real signals form schedule
int PrepareNewDataStructure(DataStructure *dataStructure, DataStructure *newDS, int *&signalMapping, int node, int slot,
                            float freeBand);

/// Method releases all the structures prepared by the PerpareNewIncremDataStructure method
/// @param newDS New auxiliary data structure
/// @param signalMapping Signal indexes mapping function from newDS to the original dataStructure
int ReleaseNewDataStructure(DataStructure *newDS, int *signalMapping);

/// Entry point for the improved first fit scheduler
/// @param dataStructure Global attributes
/// @param signalOrdering Ordering of signals the first fit algorithm should follow
int FirstFitColorScheduler(DataStructure *dataStructure, int *signalOrdering);

#endif