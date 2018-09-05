/// \file Utility.h Header file for Utility.cpp
#ifndef UTILITY_H_
#define UTILITY_H_

/// Print the error message to the screen
/// @param message Error message
void error(const char *message);

/// Print message to the screen
/// @param message Message to be printed
void print(char *message);

/// Print debug message to the screen
/// @param message Debug message
void debug(char *message);

/// Set the value in the Mutual Exclusion Matrix
/// @param meMatrix Mutual Exclusion Matrix
/// @param i Row index in matrix
/// @param j Column index in matrix
/// @param value Value to be set
void setMEMatrixValue(char *meMatrix, int i, int j, char value);

/// Get the value in the Mutual Exclusion Matrix
/// @param meMatrix Mutual Exclusion Matrix
/// @param i Row index in matrix
/// @param j Column index in matrix
/// @return Value of the demanded cell
char getMEMatrixValue(char *meMatrix, int i, int j);

/// Allocate the memory
/// @param _Size The size of the memory
/// @return Allocated memory
void *mmalloc(size_t _Size);

/// Allocate and initialize memory
/// @param _Count Number of elements to allocate
/// @param _Size Size of the element
/// @return Allocated and initialized memory
void *ccalloc(size_t _Count, size_t _Size);

/// Release allocated memory
/// @param _Memory Memory to be released
void ffree(void *_Memory);

/// Release all allocated memory
void ffreeAll();

/// Test if the memory is allocated
/// @param _Memory Memory address to test
/// @return 1 if it is Allocated a 0 if it is not
char isAllocated(void *_Memory);

/// Normalizes parameters from the input instance
/// @param dataStructure Global attributes
int UpdateInputData(DataStructure *dataStructure);

/// Exact graph coloring algorithm based on ILP model
/// @param dataStructure Global attributes
/// @param nodeSlots Number of slots scheduled for particular node
/// @param rUB Upper bound to return
/// @param rLB Lower bound to return
/// @param rOpt Optimal value to return
/// @param rSlotsAssignment Array that assign Slot ID in resulting multischedule to the slots of nodes (slot scheduling)
int graphColoring(DataStructure *dataStructure, int *nodeSlots, int *rUB, int *rLB, int *rOpt, int *rSlotsAssignment);

/// Heuristic for graph coloring (for computing Upper bound)
/// @param dataStructure Global attributes
/// @param nodeSlots Number of slots scheduled for particular node
/// @param rSlotsAssignment Array that assign Slot ID in resulting multischedule to the slots of nodes (slot scheduling)
/// @return Upper bound
int graphColoringHeuristic(DataStructure *dataStructure, int *nodeSlots, int *rSlotsAssignment);

template <typename T>
std::string ToString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

#endif
