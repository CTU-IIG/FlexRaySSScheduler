/// \file io.h Header file for io.cpp

#ifndef IO_H_
#define IO_H_

#include "MVFRScheduler.h"

/// Parse input arguments
/// @param argc Number of input arguments
/// @param argv Array of input arguments
/// @param dataStructure Global attributes
int parseInput(int argc, char *argv[], DataStructure *dataStructure);

/// Reading input file (benchmark instance)
/// @param fileName Name of the input file
/// @param dataStructure Global attributes
int readInputFile(char *fileName, DataStructure *dataStructure);

/// Saving the resulting schedule to the output file
/// @param dataStructure Global attributes
int saveToOutputFile(DataStructure *dataStructure);

int AppendNLToCSV(const DataStructure *dataStructure);

/// Appending the given value to the CVS file
/// @param dataStructure Global attributes
/// @param value Value that should be appended
int AppendToCSV(const DataStructure *dataStructure, int value);

/// Appending the given value to the CVS file
/// @param dataStructure Global attributes
/// @param value Value that should be appended
int AppendToCSV(const DataStructure *dataStructure, float value);

/// Appending the values of resulting schedule (number of used slots and time used for scheduling) to the CVS file
/// @param dataStructure Global attributes
int AppendResultsToCSV(DataStructure *dataStructure);

/// Appending the values of used slots in multivariant and independent schedule to the CVS file
/// @param multiVariant Number of used slots in multivariant scheduling
/// @param independent Number of used slots in independent scheduling
/// @param fileName Name of the file to append values to
int AppendIndependentTestResults(int multiVariant, int independent, char *fileName);

/// Reading of the old roaster for independent scheduling
/// @param fileName Name of the file with original roaster for the independent scheduler
/// @param dataStructure Global attributes
int readOldRoaster(char *fileName, DataStructure *dataStructure);


#endif