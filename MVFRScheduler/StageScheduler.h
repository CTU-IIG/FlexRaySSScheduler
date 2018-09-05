/// \file StageScheduler.h Header file for the StageScheduler.cpp
#ifndef STAGESCHEDULER_H_
#define STAGESCHEDULER_H_

#include "Message.h"

/// Structure for result from signal message assignment evaluation
struct EvaluationResult {
    int penalization; ///< Penalization of assignment signal to message
    int bestOffsetInMessage; ///< Best position in message where the signal can be placed
    Message *message; ///< Pointer to that message
};

/// Evaluation of penalization of Signal to Message assignment
/// @param signalIndex Index of the signal to be placed
/// @param dataStructure Global attributes
/// @param message Pointer to the message to which the signal should be placed
EvaluationResult EvaluateAssignmentToMessage(int signalIndex, DataStructure *dataStructure, Message *message);

/// Method tries to place signal to some message. If it is not possible it creates a new message and place it there.
/// @param signalIndex Index of the signal to be placed
/// @param dataStructure Global attributes
/// @param messages The vector of messages
/// @param firstMessageWithThisPeriod Index of the first message in the messages which has the same period
int PlaceSignalToMessage(int signalIndex, DataStructure *dataStructure,
                         std::vector<Message> &messages, int firstMessageWithThisPeriod);

/// Method tries to find feasible position for the message and place it there.
/// @param dataStructure Global attributes
/// @param messages The vector of messages
/// @param schedule The schedule where the message should be placed
/// @param occupiedSlotCapacity The ratio of occupancy of the particular slot
/// @param maxSlot The number of already allocated slots
/// @param messageIndex Index of the message to by placed to the schedule
int PlaceMessageToSchedule(DataStructure *dataStructure, std::vector<Message> &messages, std::vector<int> *schedule,
                           std::vector<double> &occupiedSlotCapacity, int maxSlot,
                           int messageIndex);

/// Frame packer (First stage)
/// @param dataStructure Global attributes
/// @param messages The vector of messages
/// @param firstSignal ID of the first signal in the node (for which the Frame packing is called)
/// @param node ID of the node for scheduling
int FramePacking(DataStructure *dataStructure, std::vector<Message> &messages, int firstSignal, int node);

/// Message scheduler (Second stage)
/// @param dataStructure Global attributes
/// @param messages The vector of messages
/// @param slotNodeUsage Array that say which slot is used by which node
/// @param node ID of the node for scheduling
int MessageScheduling(DataStructure *dataStructure, std::vector<Message> &messages,
                      std::vector<std::vector<int> > &slotNodeUsage, int node);

/// Configure the signals parameters according to the message parameters
/// @param dataStructure Global attributes
/// @param messages The vector of messages
void SignalConfiguring(DataStructure *dataStructure, std::vector<Message> &messages);

/// Find the slot after "firstSlot" that could be used by particular node or alocate the new one
/// @param dataStructure Global attributes
/// @param slotNodeUsage Array that say which slot is used by which node
/// @param node ID of the node for scheduling
/// @param firstSlot Slot ID where the searching should start
int RegisterNextUsableSlot(DataStructure *dataStructure, std::vector<std::vector<int> > &slotNodeUsage, int node,
                           int firstSlot);

/// Method that schedule one node independently
/// @param dataStructure Global attributes
/// @param node ID of the node for scheduling
/// @param slotNodeUsage Array that say which slot is used by which node
int NodeScheduler(DataStructure *dataStructure, int node, std::vector<std::vector<int> > &slotNodeUsage);

/// Entry point to the Two Stage scheduler
/// @param dataStructure Global attributes
int StageScheduler(DataStructure *dataStructure);

#endif
