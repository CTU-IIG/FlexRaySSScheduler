/// \file MergeSort.h Header file for the MergeSort.cpp
#ifndef MERGESORT_H_
#define MERGESORT_H_

/// Parameter according which the MergeSort should sort the signals
enum SortType {
    PERIOD, ///< Increasing period
    WINDOW, ///< Increasing window
    DEADLINE, ///< Increasing deadline
    RELEASEDATE, ///< Increasing release date
    LENGTH, ///< Increasing length/payload
    PERIODDEC, ///< Decreasing period
    WINDOWDEC, ///< Decreasing window
    DEADLINEDEC, ///< Decreasing deadline
    RELEASEDATEDEC, ///< Decreasing release date
    LENGTHDEC, ///< Decreasing length
    SLOT, ///< Increasing slot ID
    SLOTDEC, ///< Decreasing slot ID
    NODE, ///< Increasing transmitting node ID
    NODEDEC /// Decreasing transmitting node ID
};

/// Merge sort algorithm
/// @param array Array to be sorted
/// @param aux Aux array for merge sort
/// @param withRespectTo Structure with parameters by which the array should be sorted
/// @param left Index of the first element to sort
/// @param right Index of the last element to sort
/// @param f Comparator function
/// @param type Parameter according which the MergeSort should sort the signals
void mergeSort(int array[], int aux[], void *withRespectTo, int left, int right, int (*f)(int, int, void *, SortType),
               SortType type);

/// Merge two sorted arrays to the one sorted
/// @param array Array to be sorted
/// @param aux Aux array for merge sort
/// @param withRespectTo Structure with parameters by which the array should be sorted
/// @param left Index of the first element to sort
/// @param right Index of the last element to sort
/// @param comparator Comparator function
/// @param type Parameter according which the MergeSort should sort the signals
void merge(int array[], int aux[], void *withRespectTo, int left, int right,
           int (*comparator)(int, int, void *, SortType), SortType type);

/// Comparator for Signal sorting
/// @param first Value of the first element in comparison
/// @param second Value of the second element in comparison
/// @param withRespectTo Structure with parameters by which the array should be sorted
/// @param type Parameter according which the MergeSort should sort the signals
int mergeSortSignalComparator(int first, int second, void *withRespectTo, SortType type);

/// Comparator for Message sorting
/// @param first Value of the first element in comparison
/// @param second Value of the second element in comparison
/// @param withRespectTo Structure with parameters by which the array should be sorted
/// @param type Parameter according which the MergeSort should sort the signals
int mergeSortMessageComparator(int first, int second, void *withRespectTo, SortType type);

#endif