/// \file FirstFitScheduler.h Header file for the FirstFitScheduler.cpp
#ifndef SOLIDSCHEDULER_H_
#define SOLIDSCHEDULER_H_

#include "MVFRScheduler.h"
#include "Position.h"

/// Find first free position for the signal in the frame or signalize that it is not possible to schedule signal to the frame
/// @param dataStructure Global attributes
/// @param frame The frame where the method is looking to.
/// @param signalIndex Index of signal for which the method is looking for a place
/// @param startPosition The index where it should start to look to.
int FindFreePositionInFrame(DataStructure *dataStructure, Frame *frame, int signalIndex, int startPosition);

/// Test if it is possible to schedule signal to the frame and to all of the following frames (because of period)
/// @param dataStructure Global attributes
/// @param frame The frame where the method is looking to.
/// @param inFrameOffset Offset of the signal in the frame
/// @param signalIndex Index of the signal
int TestSignalInFrame(DataStructure *dataStructure, Frame *frame, int inFrameOffset, int signalIndex);

/// Find first Slot/Cycle position where the signal could be placed
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param signalIndex Index of the signal
/// @param startPosition The slot/cycle position where the searching should start.
Position FindFirstCyclePositionForSignal(DataStructure *dataStructure, std::vector<Frame> *schedule, int signalIndex,
                                         Position startPosition);

/// Test if the slot of the particular node (inherited from signal ID) can be scheduled to the particular slot in the multischedule
/// @param dataStructure Global attributes
/// @param frame The frame where the method is looking to.
/// @param signalIndex Index of signal for which the method is looking for a place
int TestSignalFrameNodeIDCollision(DataStructure *dataStructure, Frame *frame, int signalIndex);

/// Method just place signal to the schedule
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param position Position in multischedule where the signal should be placed to
/// @param signalIndex Index of the signal that should be placed.
int PlaceSignalToFrame(DataStructure *dataStructure, std::vector<Frame> *schedule, Position position, int signalIndex);

/// Find the first position where the signal can be placed and place it there
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param signalIndex Index of the signal that should be placed.
int PlaceFFSignalToSchedule(DataStructure *dataStructure, std::vector<Frame> *schedule, int signalIndex);

/// Entry point for the improved first fit scheduler
/// @param dataStructure Global attributes
/// @param signalOrdering Ordering of signals the first fit algorithm should follow
int FirstFitScheduler(DataStructure *dataStructure, int *signalOrdering);

#endif