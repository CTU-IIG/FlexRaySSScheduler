/// \file BestFitColorScheduler.h Header file for the BestFitColorScheduler.cpp
#ifndef BESTFITCOLORSCHEDULER_H_
#define BESTFITCOLORSCHEDULER_H_

#include "MVFRScheduler.h"
#include "Position.h"
#include "CFrame.h" // Why???

/// Find first free position for the signal in the frame or signalize that it is not possible to schedule signal to the frame
/// @param dataStructure Global attributes
/// @param frame The frame where the method is looking to.
/// @param signalIndex Index of signal for which the method is looking for a place
/// @param startPosition The index where it should start to look to.
/// @param evaluation The resultin evaluation value for given assigment
int BFindFreePositionInFrame(DataStructure *dataStructure, CFrame *frame, int signalIndex, int startPosition,
                             int *evaluation);

/// Test if it is possible to schedule signal to the frame and to all of the following frames (because of period)
/// @param dataStructure Global attributes
/// @param frame The frame where the method is looking to.
/// @param inFrameOffset Offset of the signal in the frame
/// @param signalIndex Index of the signal
int BTestSignalInFrame(DataStructure *dataStructure, CFrame *frame, int inFrameOffset, int signalIndex);

/// Find first Slot/Cycle position where the signal could be placed
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param signalIndex Index of the signal
/// @param startPosition The slot/cycle position where the searching should start.
BPosition BFindFirstCyclePositionForSignal(DataStructure *dataStructure, std::vector<CFrame> *schedule, int signalIndex,
                                           BPosition startPosition);

/// Method just place signal to the schedule
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param position Position in multischedule where the signal should be placed to
/// @param signalIndex Index of the signal that should be placed.
int BPlaceSignalToFrame(DataStructure *dataStructure, std::vector<CFrame> *schedule, BPosition position,
                        int signalIndex);

/// Find the first position where the signal can be placed and place it there
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param signalIndex Index of the signal that should be placed.
int PlaceBFCSignalToSchedule(DataStructure *dataStructure, std::vector<CFrame> *schedule, int signalIndex);

/// Entry point for the best fit scheduler
/// @param dataStructure Global attributes
/// @param signalOrdering Ordering of signals the first fit algorithm should follow
int BestFitColorScheduler(DataStructure *dataStructure, int *signalOrdering);

#endif