/// \file MergeSort.cpp Implementation of the MergeSort
#include "MVFRScheduler.h"

void mergeSort(int array[], int aux[], void *withRespectTo, int left, int right,
               int (*comparator)(int, int, void *, SortType), SortType type) {
    if (left == right) return;
    int middleIndex = (left + right) / 2;
    mergeSort(array, aux, withRespectTo, left, middleIndex, comparator, type);
    mergeSort(array, aux, withRespectTo, middleIndex + 1, right, comparator, type);
    merge(array, aux, withRespectTo, left, right, comparator, type);

    for (int i = left; i <= right; i++) {
        array[i] = aux[i];
    }
}

void merge(int array[], int aux[], void *withRespectTo, int left, int right,
           int (*comparator)(int, int, void *, SortType), SortType type) {
    int middleIndex = (left + right) / 2;
    int leftIndex = left;
    int rightIndex = middleIndex + 1;
    int auxIndex = left;
    while (leftIndex <= middleIndex && rightIndex <= right) {
        if (comparator(array[leftIndex], array[rightIndex], withRespectTo, type)) {
            aux[auxIndex] = array[leftIndex++];
        }
        else {
            aux[auxIndex] = array[rightIndex++];
        }
        auxIndex++;
    }
    while (leftIndex <= middleIndex) {
        aux[auxIndex] = array[leftIndex++];
        auxIndex++;
    }
    while (rightIndex <= right) {
        aux[auxIndex] = array[rightIndex++];
        auxIndex++;
    }
}

int mergeSortSignalComparator(int first, int second, void *withRespectTo, SortType type) {
    DataStructure *dataStructure = static_cast<DataStructure *>(withRespectTo);
    switch (type) {
        case PERIODDEC:
            return dataStructure->signalPeriods[first] >= dataStructure->signalPeriods[second];
        case WINDOWDEC:
            return (dataStructure->signalDeadlines[first] - dataStructure->signalReleaseDates[first])
                   >= (dataStructure->signalDeadlines[second] - dataStructure->signalReleaseDates[second]);
        case WINDOW:
            return (dataStructure->signalDeadlines[first] - dataStructure->signalReleaseDates[first])
                   <= (dataStructure->signalDeadlines[second] - dataStructure->signalReleaseDates[second]);
        case DEADLINEDEC:
            return dataStructure->signalDeadlines[first] >= dataStructure->signalDeadlines[second];
        case DEADLINE:
            return dataStructure->signalDeadlines[first] <= dataStructure->signalDeadlines[second];
        case RELEASEDATEDEC:
            return dataStructure->signalReleaseDates[first] >= dataStructure->signalReleaseDates[second];
        case RELEASEDATE:
            return dataStructure->signalReleaseDates[first] <= dataStructure->signalReleaseDates[second];
        case LENGTHDEC:
            return dataStructure->signalLengths[first] >= dataStructure->signalLengths[second];
        case LENGTH:
            return dataStructure->signalLengths[first] <= dataStructure->signalLengths[second];
        case NODE:
            return dataStructure->signalNodeIDs[first] <= dataStructure->signalNodeIDs[second];
        case NODEDEC:
            return dataStructure->signalNodeIDs[first] >= dataStructure->signalNodeIDs[second];
        default:
            return dataStructure->signalPeriods[first] <= dataStructure->signalPeriods[second];
    }
}

int mergeSortMessageComparator(int first, int second, void *withRespectTo, SortType type) {
    std::vector<Message> *messages = (std::vector<Message> *) withRespectTo;
    switch (type) {
        case PERIODDEC:
            return (*messages)[first].period >= (*messages)[second].period;
        case WINDOWDEC:
            return ((*messages)[first].deadline - (*messages)[first].releaseTime)
                   >= ((*messages)[second].deadline - (*messages)[second].releaseTime);
        case WINDOW:
            return ((*messages)[first].deadline - (*messages)[first].releaseTime)
                   <= ((*messages)[second].deadline - (*messages)[second].releaseTime);
        case DEADLINEDEC:
            return (*messages)[first].deadline >= (*messages)[second].deadline;
        case DEADLINE:
            return (*messages)[first].deadline <= (*messages)[second].deadline;
        case RELEASEDATEDEC:
            return (*messages)[first].releaseTime >= (*messages)[second].releaseTime;
        case RELEASEDATE:
            return (*messages)[first].releaseTime <= (*messages)[second].releaseTime;
        case LENGTHDEC:
            return (*messages)[first].length >= (*messages)[second].length;
        case LENGTH:
            return (*messages)[first].length <= (*messages)[second].length;
        case SLOT:
            return (*messages)[first].slot >= (*messages)[second].slot;
        case SLOTDEC:
            return (*messages)[first].slot <= (*messages)[second].slot;
        default:
            return (*messages)[first].period <= (*messages)[second].period;
    }
}
