//
// Created by jenik on 6/7/16.
//

#include "MVFRScheduler.h"

using namespace std;

int findIndependentSetGurobi(DataStructure *dataStructure, std::vector<ConflictNode> &conflictGraph,
                             int *conflictMapFunction) {
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    //model.getEnv().set(GRB_DoubleParam_TimeLimit, 5);
    model.set(GRB_IntAttr_ModelSense, -1);

    vector<GRBVar> Xi(conflictGraph.size());
    for (int i = 0; i < static_cast<int>(conflictGraph.size()); i++) {
        Xi[i] = model.addVar(0.0, 1.0, conflictGraph.size() * dataStructure->hyperPeriod +
                                       dataStructure->hyperPeriod /
                                       dataStructure->signalPeriods[conflictGraph[i].nodeID], GRB_BINARY, "Xi");
        //Xi[i] = model.addVar(0.0, 1.0, dataStructure->signalLengths[conflictGraph[i].nodeID]*
        //                               dataStructure->hyperPeriod /
        //                               dataStructure->signalPeriods[conflictGraph[i].nodeID], GRB_BINARY, "Xi");
    }
    model.update();
    for (int i = 0; i < static_cast<int>(conflictGraph.size()); i++) // v_i + v_j \leg 1 \forall vi_, v_j \in E
    {
        std::set<int>::iterator it;
        for (it = conflictGraph[i].nodeNeighbours.begin(); it != conflictGraph[i].nodeNeighbours.end(); ++it) {
            if (conflictGraph[i].nodeID < *it) {
                model.addConstr(Xi[i] + Xi[conflictMapFunction[*it]] <= 1);
            }
        }
    }

    model.optimize();
    switch (model.get(GRB_IntAttr_Status)) {
        case GRB_INFEASIBLE:
        case GRB_UNBOUNDED:
            return FAIL;
        case GRB_TIME_LIMIT:
            if (model.get(GRB_IntAttr_SolCount) <= 0) {
                return FAIL;
            }
            break;
        default:
            break;
    }

    for (int i = 0; i < (int) conflictGraph.size(); ++i) {
        conflictGraph[i].independent = static_cast<char>(Xi[i].get(GRB_DoubleAttr_X));
    }

    return SUCCESS;

}

int findSlotIndependentSetGurobi(DataStructure *dataStructure, std::vector<ConflictNode> &conflictGraph,
                                 std::vector<int> *nodeSlotSignalsCount, int *slotsNode, int *nodeSlotIndexes,
                                 int *conflictMapFunction, int *slotDegOfFree, int oldScheduleLength) {
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    model.set(GRB_IntAttr_ModelSense, -1);

    vector<GRBVar> xi(conflictGraph.size());
    for (int i = 0; i < conflictGraph.size(); i++) {
        xi[i] = model.addVar(0.0, 1.0, 1 + (conflictGraph.size() + 1) * slotDegOfFree[i] +
                                       (3 * oldScheduleLength * conflictGraph.size() * conflictGraph.size() +
                                        1) *
                                       nodeSlotSignalsCount[slotsNode[conflictGraph[i].nodeID]][
                                               conflictGraph[i].nodeID -
                                               nodeSlotIndexes[slotsNode[conflictGraph[i].nodeID]]],
                             GRB_BINARY, "xi");
    }
    model.update();

    for (int i = 0; i < conflictGraph.size(); i++) {
        for (set<int>::iterator it = conflictGraph[i].nodeNeighbours.begin();
             it != conflictGraph[i].nodeNeighbours.end(); ++it) {
            if (conflictGraph[i].nodeID < *it) {
                model.addConstr(xi[i] + xi[conflictMapFunction[*it]] <= 1);
            }
        }
    }

    model.optimize();

    switch (model.get(GRB_IntAttr_Status)) {
        case GRB_INFEASIBLE:
        case GRB_UNBOUNDED:
            return FAIL;
        case GRB_TIME_LIMIT:
            if (model.get(GRB_IntAttr_SolCount) <= 0) {
                return FAIL;
            }
            break;
        default:
            break;
    }

    for (int i = 0; i < (int) conflictGraph.size(); ++i) {
        conflictGraph[i].independent = static_cast<char>(xi[i].get(GRB_DoubleAttr_X));
    }

    return SUCCESS;
}