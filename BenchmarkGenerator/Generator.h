/// \file Generator.h Header file for Generator.cpp

#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "GDS.h"
#include "Utility.h"
#include <string>

/// Value that means that the method calling was successfull
#define SUCCESS 1 
/// Value that means that the method calling failed
#define FAIL 0 
/// Value that means that some boolean logic is true
#define TRUE 1
/// Value that means that some boolean logic is false
#define FALSE 0 

/// How many percent of the instance occupies the new signals
#define INCREM_PERC 4

/// Parse the arguments and read the input
/// @param gds Generator data structure
/// @param argc Number of arguments
/// @param argv Array of arguments
/// @return SUCCESS or FAIL
int readInput(GDS* gds, int argc, char** argv);
/// Reads the configuration files
/// @param gds Generator data structure
/// @return SUCCESS or FAIL
int readConfigFile(GDS* gds);
/// Checking if the configuration file is consistent
/// @param gds Generator data structure
/// @return SUCCESS or FAIL
int testConfigFile(GDS* gds);
/// Checking if the generated benchmark instance is consistent
/// @param gds Generator data structure
/// @return SUCCESS or FAIL
int testBenchmarkInstance(GDS* gds);
/// Instance generator for multivariant scheduling
/// @param gds Generator data structure
/// @return SUCCESS or FAIL
int generateInstances(GDS* gds);
/// Saves the benchmark instance to the file
/// @param gds Generator data structure
/// @return SUCCESS or FAIL
int saveToOutputFile(GDS* gds);
/// Frees all the structures allocated by GDS
/// @param gds Generator data structure
/// @return SUCCESS or FAIL
int releaseDataStructures(GDS* gds);
/// Generates the benchmark instance for incremental multivariant scheduling
/// @param gds Generator data structure
/// @return SUCCESS or FAIL
int generateIncremInstances(GDS* gds);
/// Entry point to the application
/// @param argc Number of arguments
/// @param argv Array of arguments
int main(int argc, char** argv);

#endif