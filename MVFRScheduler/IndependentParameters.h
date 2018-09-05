/// \file IndependentParameters.h Parameters for the Independent scheduler (for comparing multivariant and independent schedules)
#ifndef INDEPENDENTPARAMETERS_H_
#define INDEPENDENTPARAMETERS_H_

/// Parameters for the Independent scheduler
struct IndependentParameters {
    char *saveResultToFile; ///< Name of the file where the resulting values should be saved
    FirstFitSortType sortType; ///< What ordering should be used for the signals
};

#endif