/// \file FirstFitVariantScheduler.h Header file for the Native first fit scheduler
#include "VariantFrame.h"

/// Find first free position for the signal in the frame or signalize that it is not possible to schedule signal to the frame
/// @param dataStructure Global attributes
/// @param frame The frame where the method is looking to.
/// @param signalIndex Index of signal for which the method is looking for a place
/// @param startPosition The index where it should start to look to.
int VariantFindFreePositionInFrame(DataStructure *dataStructure, VariantFrame *frame, int signalIndex,
                                   int startPosition);

/// Test if it is possible to schedule signal to the frame and to all of the following frames (because of period)
/// @param dataStructure Global attributes
/// @param frame The frame where the method is looking to.
/// @param inFrameOffset Offset of the signal in the frame
/// @param signalIndex Index of the signal
int VariantTestSignalInFrame(DataStructure *dataStructure, VariantFrame *frame, int inFrameOffset, int signalIndex);

/// Find first Slot/Cycle position where the signal could be placed
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param signalIndex Index of the signal
/// @param startPosition The slot/cycle position where the searching should start.
Position VariantFindFirstCyclePositionForSignal(DataStructure *dataStructure, std::vector<VariantFrame> *schedule,
                                                int signalIndex,
                                                Position startPosition);

/// The method checks if there is enough slots in the schedule. If not it allocates necessary slots.
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param slots Number of necessary slots
void assureScheduleHasEnoughSlots(DataStructure *dataStructure, std::vector<VariantFrame> *schedule, int slots);

/// Method just place signal to the schedule
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param position Position in multischedule where the signal should be placed to
/// @param signalIndex Index of the signal that should be placed.
int VariantPlaceSignalToFrame(DataStructure *dataStructure, std::vector<VariantFrame> **schedule, Position position,
                              int signalIndex);

/// Gets the variant ID of the variant that uses particular signal and has allocated greatest number of slots
/// @param dataStructure Global attributes
/// @param signalIndex Index of the signal that should be placed.
/// @param schedule Multischedule for assignment
int getVariantUsingSignalWithMaxSlotUsed(DataStructure *dataStructure, int signalIndex,
                                         std::vector<VariantFrame> **schedule);

/// Find the first position where the signal can be placed and place it there
/// @param dataStructure Global attributes
/// @param schedule Multischedule for assignment
/// @param signalIndex Index of the signal that should be placed.
int PlaceFFVSignalToSchedule(DataStructure *dataStructure, std::vector<VariantFrame> **schedule, int signalIndex);

/// Entry point for the improved first fit scheduler
/// @param dataStructure Global attributes
/// @param signalOrdering Ordering of signals the first fit algorithm should follow
int FirstFitVariantScheduler(DataStructure *dataStructure, int *signalOrdering);