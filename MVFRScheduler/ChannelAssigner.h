/// \file ChannelAssigner.h The algorithm for assigning ECUs to the particular A/B channel of the FlexRay bus. Objective is to improve assignment to minimize number of allocated slots and communication through the GW in resulting schedule.

#ifndef CHANNELASSIGNER_H_
#define CHANNELASSIGNER_H_

#include "MVFRScheduler.h"

struct CAMessage {
    std::vector<int> nodes;
    int payload;
    int channel;
};

struct ChannelsFitness {
    int channelA;
    int channelB;
    int gateway;

    std::string str() const;
};

struct NodeList {
    int channel;
    std::vector<CAMessage *> messages;
};

long ComputeNodeHash(std::vector<int> nodes);

int AggregateMessages(const DataStructure *dataStructure, CAMessage *&messages);

ChannelsFitness computeChannelAssignmentUtility(DataStructure *dataStructure, CAMessage *messages, int messagesCount);

ChannelsFitness EvaluateAddingNodeToChanel(int channel, int node, NodeList *node_list, ChannelsFitness cf);

void AssignNodeToChannel(int channel, int node, NodeList *node_list);

ChannelsFitness AssignNodesToChannels(const DataStructure *dataStructure, NodeList *node_list, CAMessage *messages,
                                      int messageCount, const double alpha, const double beta);

ChannelsFitness EvaluateNodesSwap(DataStructure *dataStructure, NodeList *node_list, std::vector<int> &nodes,
                                  ChannelsFitness cf);

ChannelsFitness EvaluateNodeSwap(NodeList *node_list, unsigned int node, ChannelsFitness cf);

bool IsNewCFBetter(ChannelsFitness newCF, ChannelsFitness oldCF, const double alpha, const double beta);

void SwapNode(DataStructure *dataStructure, NodeList *node_list, unsigned int node);

bool TOSOptimize(DataStructure *dataStructure, NodeList *node_list, CAMessage *messages, int messagesCount,
                 ChannelsFitness &cfResult, const double alpha, const double beta);

ChannelsFitness TwoOptSearch(DataStructure *dataStructure, NodeList *node_list, CAMessage *messages, int messageCount,
                             ChannelsFitness cf, const double alpha, const double beta);

ChannelsFitness ChannelAssigner(DataStructure *dataStructure, const double alpha, const double beta,
                                const int maxIteration);

int ChannelScheduler(DataStructure *dataStructure, ChannelsFitness cf);

#endif