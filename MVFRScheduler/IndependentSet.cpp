/// \file IndependentSet.cpp Algorithm for computing the independent sets [Incremental scheduler]
#include "MVFRScheduler.h"
#include "glpk.h"

int findIndependentSet(DataStructure *dataStructure, std::vector<ConflictNode> &conflictGraph,
                       int *conflictMapFunction) {
    int i;
    // ILP �e�en� hled�n� v�en� nejv�t�� nez�visl� mno�iny
    glp_prob *mip = glp_create_prob();
    glp_set_prob_name(mip, "IndeoendentSet");
    glp_set_obj_dir(mip, GLP_MAX);

    int line = 1;
    glp_add_cols(mip, conflictGraph.size());
    double values[] = {0, 1, 1};
    int indices[3];
    indices[0] = 0;
    std::set<int>::iterator it;
    for (i = 0; i < (int) conflictGraph.size(); i++) // v_i + v_j \leg 1 \forall vi_, v_j \in E
    {
        for (it = conflictGraph[i].nodeNeighbours.begin(); it != conflictGraph[i].nodeNeighbours.end(); ++it) {
            if (conflictGraph[i].nodeID < *it) {
                indices[1] = i + 1;
                indices[2] = conflictMapFunction[*it] + 1;
                glp_add_rows(mip, 1);
                glp_set_mat_row(mip, line, 2, indices, values);
                glp_set_row_bnds(mip, line, GLP_UP, 0, 1);
                line++;
            }
        }
    }

    //int number_variables = glp_get_num_cols(mip);
    for (i = 1; i <= (int) conflictGraph.size(); i++) {
        glp_set_col_kind(mip, i, GLP_BV);
        glp_set_obj_coef(mip, i, conflictGraph.size() * dataStructure->hyperPeriod +
                                 dataStructure->hyperPeriod /
                                 dataStructure->signalPeriods[conflictGraph[i - 1].nodeID]);
    }

    glp_iocp settings;
    glp_init_iocp(&settings);
    settings.presolve = GLP_ON;
    //settings.msg_lev = GLP_MSG_OFF; //GLP_MSG_ALL;
    settings.msg_lev = GLP_MSG_ALL;
    settings.br_tech = GLP_BR_FFV;
    settings.bt_tech = GLP_BT_BPH;
    settings.mir_cuts = GLP_ON;
    //settings.tm_lim = 3600000; // Time limit - 1 hour.
    //settings.tm_lim = 5000; // Time limit - 5 sec.
    settings.mip_gap = 0.0; // Relative tolerance. (1 ~ 100 %)

    int ret_code = glp_intopt(mip, &settings);

    if (ret_code != 0 && ret_code != GLP_ETMLIM && ret_code != GLP_EMIPGAP) {
        return -1;
    }
    else {
        int status = glp_mip_status(mip);
        if (status == GLP_OPT || status == GLP_FEAS) {
            for (i = 0; i < (int) conflictGraph.size(); ++i) {
                conflictGraph[i].independent = (int) glp_mip_col_val(mip, i + 1);
            }
        }
    }

    glp_delete_prob(mip);
    glp_free_env();

    return 0;
}

int findSlotIndependentSet(DataStructure *dataStructure, std::vector<ConflictNode> &conflictGraph,
                           std::vector<int> *nodeSlotSignalsCount, int *slotsNode, int *nodeSlotIndexes,
                           int *conflictMapFunction, int *slotDegOfFree, int oldScheduleLength) {
    int i;
    // ILP �e�en� hled�n� v�en� nejv�t�� nez�visl� mno�iny
    glp_prob *mip = glp_create_prob();
    glp_set_prob_name(mip, "IndeoendentSet");
    glp_set_obj_dir(mip, GLP_MAX);

    int line = 1;
    glp_add_cols(mip, conflictGraph.size());
    double values[] = {0, 1, 1};
    int indices[3];
    indices[0] = 0;
    std::set<int>::iterator it;
    for (i = 0; i < (int) conflictGraph.size(); i++) // v_i + v_j \leg 1 \forall vi_, v_j \in E
    {
        for (it = conflictGraph[i].nodeNeighbours.begin(); it != conflictGraph[i].nodeNeighbours.end(); ++it) {
            if (conflictGraph[i].nodeID < *it) {
                indices[1] = i + 1;
                indices[2] = conflictMapFunction[*it] + 1;
                glp_add_rows(mip, 1);
                glp_set_mat_row(mip, line, 2, indices, values);
                glp_set_row_bnds(mip, line, GLP_UP, 0, 1);
                line++;
            }
        }
    }

    //int number_variables = glp_get_num_cols(mip);
    for (i = 1; i <= (int) conflictGraph.size(); i++) {
        glp_set_col_kind(mip, i, GLP_BV);
        //glp_set_obj_coef(mip,i,conflictGraph.size()*dataStructure->signalsCount +
        //	nodeSlotSignalsCount[slotsNode[conflictGraph[i-1].nodeID]][conflictGraph[i-1].nodeID - nodeSlotIndexes[slotsNode[conflictGraph[i-1].nodeID]]]);

        //glp_set_obj_coef(mip,i,1 + conflictGraph.size()*
        //	nodeSlotSignalsCount[slotsNode[conflictGraph[i-1].nodeID]][conflictGraph[i-1].nodeID - nodeSlotIndexes[slotsNode[conflictGraph[i-1].nodeID]]]);

        glp_set_obj_coef(mip, i, 1 + (conflictGraph.size() + 1) * slotDegOfFree[i - 1] +
                                 (3 * oldScheduleLength * conflictGraph.size() * conflictGraph.size() + 1) *
                                 nodeSlotSignalsCount[slotsNode[conflictGraph[i - 1].nodeID]][
                                         conflictGraph[i - 1].nodeID -
                                         nodeSlotIndexes[slotsNode[conflictGraph[i - 1].nodeID]]]);
    }

    glp_iocp settings;
    glp_init_iocp(&settings);
    settings.presolve = GLP_ON;
    settings.msg_lev = GLP_MSG_OFF; //GLP_MSG_ALL;
    settings.br_tech = GLP_BR_FFV;
    settings.bt_tech = GLP_BT_BPH;
    settings.mir_cuts = GLP_ON;
    settings.tm_lim = 3600000; // Time limit - 1 hour.
    settings.mip_gap = 0.0; // Relative tolerance. (1 ~ 100 %)

    int ret_code = glp_intopt(mip, &settings);

    if (ret_code != 0 && ret_code != GLP_ETMLIM && ret_code != GLP_EMIPGAP) {
        return -1;
    }
    else {
        int status = glp_mip_status(mip);
        if (status == GLP_OPT || status == GLP_FEAS) {
            for (i = 0; i < (int) conflictGraph.size(); ++i) {
                conflictGraph[i].independent = (int) glp_mip_col_val(mip, i + 1);
            }
        }
    }

    glp_delete_prob(mip);
    glp_free_env();

    return 0;
}