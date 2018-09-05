/// \file ScheduleChecker.h Header file fot ScheduleChecker.cpp
#ifndef SCHEDULECHECKER_H_
#define SCHEDULECHECKER_H_

#include "MVFRScheduler.h"

/// Method for checking the resulting schedule (to be sure that there is no constraint violated)
/// @param dataStructure Global attributes
int CheckSchedule(DataStructure *dataStructure);

#endif