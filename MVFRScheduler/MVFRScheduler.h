/// \file MVFRScheduler.h Header file for the MVFRScheduler.cpp
#ifndef MVFRSCHEDULER_H_
#define MVFRSCHEDULER_H_

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <set>
#include <math.h>
#include <ctime>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <numeric>
#include "simple_svg_1.0.0.h"
#include "DataStructure.h"
#include "ChannelSchedulerResult.h"
#include "Buffer.h"
#include "CFrame.h"
#include "ExporterStructure.h"
#include "NetworkParametersCalculator.h"
#include "io.h"
#include "Utility.h"
#include "StageScheduler.h"
#include "StageParameters.h"
#include "Message.h"
#include "MergeSort.h"
#include "ScheduleChecker.h"
#include "LowerBound.h"
#include "Frame.h"
#include "FirstFitScheduler.h"
#include "FirstFitParameters.h"
#include "Position.h"
#include "GeneticScheduler.h"
#include "GeneticParameters.h"
#include "FirstFitVariantScheduler.h"
#include "VariantFrame.h"
#include "FirstFitColorScheduler.h"
#include "FFCIncrementalScheduler.h"
#include "IndependentSet.h"
#include "IndependentSetGurobi.h"
#include "IGraphColoring.h"
#include "IndependentScheduler.h"
#include "IndependentParameters.h"
#include "BPosition.h"
#include "BestFitColorScheduler.h"
#include "ExporterParameters.h"
#include "ConfigExporter.h"
#include "ChannelAssigner.h"
#include "ChannelScheduler.h"
#include "ILPChanneling.h"
#include "Analysis.h"
#include "GanttChartDrawer.h"
#include "FibexExporter.h"
#include "IterativeChannelScheduler.h"
#include "ILPChannelingGurobi.h"
#include "ChannelGeneticAssigner.h"
#include "ChannelSchedulerTests.h"
#include "ChannelFaultScheduler.h"

#define nullptr NULL

#define SUCCESS 1
#define FAIL 0
#define TRUE 1
#define FALSE 0
#define CHANNELA 1
#define CHANNELB 2
#define CHANNELAB 3
#define CHANNELUNDEF 0
#define GUROBI

extern char verboseAllocInfo; ///< Verbosity level for allocation messages
extern char verboseLevel; ///< Verbosity level of application

/// Method for calling the particular scheduling algorithm
/// @param dataStructure Global attributes
int runAlgorithm(DataStructure *dataStructure);

/// Entry point to the application
/// @param argc Number of attributes
/// @param argv Array of attributes
int main(int argc, char *argv[]);

#endif
