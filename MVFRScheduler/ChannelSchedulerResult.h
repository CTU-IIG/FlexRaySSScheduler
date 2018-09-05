//
// Created by jenik on 15.12.15.
//

#ifndef MVFRSCHEDULER_CHANNELSCHEDULERRESULT_H
#define MVFRSCHEDULER_CHANNELSCHEDULERRESULT_H

#include <string>

struct ChannelSchedulerResult {
    int slotsInA;
    int slotsInB;
    int gatewaySlotsInA;
    int gatewaySlotsInB;

    std::string str() const;
};

#endif //MVFRSCHEDULER_CHANNELSCHEDULERRESULT_H
