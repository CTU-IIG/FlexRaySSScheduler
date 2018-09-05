/// \file FirstFitParameters.h Parameters for the first fit schedulers
enum FirstFitSortType {
    FFPERIOD, ///< Increasing period
    FFWINDOW, ///< Increasing window
    FFDEADLINE, ///< Increasing deadline
    FFRELEASETIME, ///< Increasing release date
    FFLENGTH, ///< Increasing length/payload
    FFPERIODDEC, ///< Decreasing period
    FFWINDOWDEC, ///< Decreasing window
    FFDEADLINEDEC, ///< Decreasing deadline
    FFRELEASETIMEDEC, ///< Decreasing release date
    FFLENGTHDEC, ///< Decreasing length
    FFNODE, ///< Increasing slot ID
    FFNODEDEC, ///< Decreasing slot ID
    FFALL, ///< Combination of orderingd
    FFLINFUNC, ///< Use linear function [not used]
    FFNONE ///< No ordering is used
};

/// Sorting coeficients for soring if linear function is used [not used]
struct FirstFitSortingCoeficients {
    int period; ///< Weight of period value
    int window; ///< Weight of window size
    int deadline; ///< Weight of deadline value
    int release; ///< Weight of release date value
    int length; ///< Weight of length value
    int node; ///< Weight of node ID value
};

/// Struct for parameters used by First fit schedulers
struct FirstFitParameters {
    FirstFitSortType sortType; ///< Type of the signal ordering for the scheduler
};