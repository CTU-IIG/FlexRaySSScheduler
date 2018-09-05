//
// Created by jenik on 12/3/15.
//

#ifndef MVFRSCHEDULER_ILPCHANNELINGGUROBI_H
#define MVFRSCHEDULER_ILPCHANNELINGGUROBI_H

#include "gurobi_c++.h"
#include "MVFRScheduler.h"

template<class InputIt, class InputIt2, class T, class TernaryOperation>
T linAccum(InputIt first, InputIt last, InputIt2 first2, InputIt2 last2, T init,
           TernaryOperation op) {
    for (; first != last || first2 != last2; ++first, ++first2) {
        init = op(init, first, first2);
    }
    return init;
}

template<class InputIt, class InputIt2, class T>
T linSum(InputIt first, InputIt last, InputIt2 first2, InputIt2 last2, T init) {
    return linAccum(first, last, first2, last2, init,
                    [](T init, InputIt first, InputIt2 second) { return init + *first * *second; });

}

template<class InputIt, class InputIt2>
GRBLinExpr expSum(std::vector<InputIt> first, std::vector<InputIt2> second) {
    return linSum(first.begin(), first.end(), second.begin(), second.end(), GRBLinExpr());
}

ChannelsFitness ILPChannelingGurobi(DataStructure *dataStructure, double const alpha);

#endif //MVFRSCHEDULER_ILPCHANNELINGGUROBI_H
