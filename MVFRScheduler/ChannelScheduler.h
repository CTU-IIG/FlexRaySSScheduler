/// \file ChannelScheduler.h The algorithm responsible for scheduling signals to the channel schedules (for A and B) according to the channel assignment. (Just for one variant)

#ifndef CHANNELSCHEDULER_H_
#define CHANNELSCHEDULER_H_

#include "ChannelAssigner.h"

struct Signal {
    int ID;
    int node;
    int period;
    int payload;
    int releaseDate;
    int deadline;
    int channel;
};

#endif