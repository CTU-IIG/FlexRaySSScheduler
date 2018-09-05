//
// Created by jenik on 12/9/15.
//

#ifndef MVFRSCHEDULER_CHANNELASSIGNERTESTS_H
#define MVFRSCHEDULER_CHANNELASSIGNERTESTS_H

#include "MVFRScheduler.h"

void IterativeChannelSchedulerTest(DataStructure *dataStructure, const double alpha);

void ChannelSchedulerTest(DataStructure *dataStructure, const double alpha);

void ChannelAssignerTestOld(DataStructure *dataStructure, const double alpha);

void ChannelAssigningMethodComparison(DataStructure *dataStructure, const double alpha);

void IterativeFTChannelSchedulerTest(DataStructure *dataStructure, const double alpha);

#endif //MVFRSCHEDULER_CHANNELASSIGNERTESTS_H
