//
// Created by jenik on 15.12.15.
//

#include "ChannelSchedulerResult.h"
#include <sstream>

std::string ChannelSchedulerResult::str() const {
    std::stringstream ss;
    ss << "A:" << this->slotsInA << " B:" << this->slotsInB << " GW_A:" << this->gatewaySlotsInA << " GW_B:" <<
    this->gatewaySlotsInB << "\n";
    return ss.str();
}