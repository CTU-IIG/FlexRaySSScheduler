/// \file IGraphColoring.cpp Graph coloring algorithms for incremental scheduling
#include "IGraphColoring.h"
#include "glpk.h"
#include "gurobi_c++.h"

using namespace std;

int incrementalGraphColoringHeuristic(DataStructure *dataStructure, int *nodeSlots, int *rSlotsAssignment) {
    int i, j, k, sum, UB, tmp;
    // Compute UB - Sekven�n� heuristika
    sum = 0;
    UB = 0;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        sum += nodeSlots[i];
    }

    int *slotsNode = (int *) ccalloc(sum, sizeof(int)); // K jak�mu nodu slot pat��
    int *nodeSlotIndexes = (int *) mmalloc(
            dataStructure->nodeCount * sizeof(int)); // Od jak�ho indexu v slotsNode za��naj� sloty dan�ho nodu
    int *colours = (int *) mmalloc(sum * sizeof(int)); // barvy k obarven�

    tmp = 0;
    std::set<int> possibleColours; // mno�ina barev, kter� m��eme uzlu p�i�adit
    std::set<int>::iterator it;
    for (i = 0; i < dataStructure->nodeCount; i++)// p�edpo��t�v�n� struktur
    {
        nodeSlotIndexes[i] = tmp;
        for (j = 0; j < nodeSlots[i]; j++) {
            slotsNode[tmp] = i;
            colours[tmp] = tmp;
            tmp++;
        }
    }

    for (i = 0; i < sum; i++) {
        if (rSlotsAssignment[i] == -1) // Pokud je�t� nen� slot rozvr�en
        {
            possibleColours.clear();
            possibleColours.insert(colours, colours + sum);
            for (j = 0; j < dataStructure->nodeCount; j++) {
                if ((slotsNode[i] == j) ||
                    (getMEMatrixValue(dataStructure->mutualExclusionNodeMatrix, slotsNode[i], j))) {
                    for (k = nodeSlotIndexes[j]; k < nodeSlotIndexes[j] + nodeSlots[j]; k++) {
                        possibleColours.erase(rSlotsAssignment[k]);
                    }
                }
            }
            it = possibleColours.begin();
            rSlotsAssignment[i] = *it;
        }
        if (rSlotsAssignment[i] > UB) UB = rSlotsAssignment[i];
    }
    UB = UB + 1;
    ffree(slotsNode);
    ffree(nodeSlotIndexes);
    ffree(colours);
    return UB;
}

int incremetalGraphColoring(DataStructure *dataStructure, int *nodeSlots, int *rUB, int *rLB, int *rOpt,
                            int *rSlotsAssignment, int *scheduleNodeMap, int oldScheduleLength) {
    int i, j, tmp, UB, LB;
    UB = 0;
    LB = 0;

    int *nodeSlotIndexes = (int *) mmalloc(
            dataStructure->nodeCount * sizeof(int)); // Od jak�ho indexu v slotsNode za��naj� sloty dan�ho nodu
    int *nodesLongestVariant = (int *) mmalloc(
            dataStructure->nodeCount * sizeof(int)); // Bin�rn� pole ur�uj�c� kter� nody obsahuje nejde�� z variant
    std::set<int> *assignedSlots = new std::set<int>[dataStructure->nodeCount];

    tmp = 0;
    for (i = 0; i < dataStructure->nodeCount; i++)// p�edpo��t�v�n� struktur
    {
        nodeSlotIndexes[i] = tmp;
        for (j = 0; j < nodeSlots[i]; j++) {
            if (rSlotsAssignment[tmp] != -1)
                assignedSlots[i].insert(rSlotsAssignment[tmp]);
            tmp++;
        }
    }

    int *rSlotsAssignmentTmp = (int *) mmalloc(tmp * sizeof(int));
    for (i = 0; i < tmp; i++)
        rSlotsAssignmentTmp[i] = rSlotsAssignment[i];

    UB = incrementalGraphColoringHeuristic(dataStructure, nodeSlots, rSlotsAssignmentTmp);

    LB = ComputeLBForGraphColoring(dataStructure, nodesLongestVariant, nodeSlots);

    if (LB < oldScheduleLength)
        LB = oldScheduleLength;

    *rUB = UB;
    *rLB = LB;
    *rOpt = UB;

    printf("LB: %d, UB: %d\n", LB, UB);
    if (LB == UB) {
        ffree(nodeSlotIndexes);
        ffree(nodesLongestVariant);
        for (i = 0; i < tmp; i++)
            rSlotsAssignment[i] = rSlotsAssignmentTmp[i];
        ffree(rSlotsAssignmentTmp);
        return 1;
    }


#ifdef GUROBI
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);

    // Generate variables
    vector<vector<GRBVar>> wik(static_cast<unsigned long>(dataStructure->nodeCount),
                               vector<GRBVar>(static_cast<unsigned int>(UB)));
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        for (int k = 0; k < UB; k++) {
            wik[i][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "wik");
        }
    }
    GRBVar z = model.addVar(LB, UB, 0.0, GRB_INTEGER, "z");
    model.update();
    model.setObjective(0.0 + z, GRB_MINIMIZE);
    // Generate constraints
    // k*wik <= z \forall i,k
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        for (int k = 0; k < UB; k++) {
            model.addConstr(k * wik[i][k] <= z, "k*w_{i,k} \\leq z");
        }
    }

    // \sum_{k=1...UB}{w_{i,k}} = L_i \forall i
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        model.addConstr(std::accumulate(wik[i].begin(), wik[i].end(), GRBLinExpr()) == nodeSlots[i],
                        "\\sum_{k=1...UB}{w_{i,k}} = L_i");
    }

    // w_{i,k} + w_{j,k} \leg 1 \forall i,j,k | EEM_{i,j} = 1
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        for (int j = 0; j < dataStructure->nodeCount; j++) {
            if (getMEMatrixValue(dataStructure->mutualExclusionNodeMatrix, i, j) == 1) {
                for (int k = 0; k < UB; k++) {
                    model.addConstr(wik[i][k] + wik[j][k] <= 1, "w_{i,k} + w_{j,k} \\leg 1");
                }
            }
        }
    }

    tmp = 0; // s_{i,j} = 1 if it is already known(from original schedule)
    for (i = 0; i < dataStructure->nodeCount; i++) {
        j = tmp;
        while (tmp < j + nodeSlots[i]) {
            if (rSlotsAssignment[tmp] != -1) {
                model.addConstr(wik[i][rSlotsAssignment[tmp]] == 1);
            }
            tmp++;
        }
    }

    model.optimize();
    switch (model.get(GRB_IntAttr_Status)) {
        case GRB_INFEASIBLE:
        case GRB_UNBOUNDED:
            ffree(nodeSlotIndexes);
            ffree(nodesLongestVariant);
            return 1;
        case GRB_TIME_LIMIT:
            if (model.get(GRB_IntAttr_SolCount) <= 0) {
                ffree(nodeSlotIndexes);
                ffree(nodesLongestVariant);
                return 1;
            }
        default:
            break;
    }
    for (int i = 0; i < dataStructure->nodeCount; ++i) {
        for (int k = 0; k < UB; k++) {
            int x = static_cast<int>(wik[i][k].get(GRB_DoubleAttr_X));
            if ((x == 1) &&
                (assignedSlots[i].find(k) == assignedSlots[i].end())) // Prirazujeme jen pokud jiz nebylo prirazeno
            { // Jiz nemusi byt sloty jednoho nodu prirazeny serazene, ale muze se objevit i poradi na preskacku
                // Drive sloty obsazovaly slotID 2 a 4, nyni je treba rozvrhnout dalsi slot a to muze byt i slot 3 i kdyz v rSlotAssigment
                // je az za sloty se slotID 2 a 4.
                for (j = nodeSlotIndexes[i]; j < nodeSlotIndexes[i] + nodeSlots[i]; j++) {
                    if (rSlotsAssignment[j] == -1) {
                        rSlotsAssignment[j] = k;
                        printf("To node %d slot %d assign slotID %d\n", i, j - nodeSlotIndexes[i], k);
                        break;
                    }
                }
            }
        }
    }

#else
    // ILP �e�en� speci�ln�ho graph coloring probl�mu
    glp_prob *mip = glp_create_prob();
    glp_set_prob_name(mip, "GraphColoring");
    glp_set_obj_dir(mip, GLP_MIN);

    int line = 1;
    int t;
    glp_add_cols(mip, UB * dataStructure->nodeCount + 1);
    for (i = 0;
         i < dataStructure->nodeCount; i++) // s_{i,t} + s_{j,t} <= 1 \forall i,j,t | i<j ; NMEM(i,j)==1 or N_i == N_j
    {
        double values[] = {0, 1, 1};
        int indices[3];
        indices[0] = 0;
        for (j = i + 1; j < dataStructure->nodeCount; j++) {
            if (getMEMatrixValue(dataStructure->mutualExclusionNodeMatrix, i, j))
                for (t = 0; t < UB; t++) {
                    indices[1] = i * UB + t + 1;
                    indices[2] = j * UB + t + 1;
                    glp_add_rows(mip, 1);
                    glp_set_mat_row(mip, line, 2, indices, values);
                    glp_set_row_bnds(mip, line, GLP_UP, 0, 1);
                    line++;
                }
        }
    }

    tmp = 0; // s_{i,j} = 1 if it is already known(from original schedule)
    for (i = 0; i < dataStructure->nodeCount; i++) {
        double values[] = {0, 1};
        int indices[2];
        indices[0] = 0;
        j = tmp;
        while (tmp < j + nodeSlots[i]) {
            if (rSlotsAssignment[tmp] != -1) {
                indices[1] = i * UB + rSlotsAssignment[tmp] + 1;
                glp_add_rows(mip, 1);
                glp_set_mat_row(mip, line, 1, indices, values);
                glp_set_row_bnds(mip, line, GLP_FX, 1, 1);
                line++;
            }
            tmp++;
        }
    }

    for (i = 0; i < dataStructure->nodeCount; i++) // t*s_{i,z} - z <= 0 \forall i,t
    {
        double values[] = {0, 1, -1};
        int indices[3];
        indices[2] = dataStructure->nodeCount * UB + 1;
        indices[0] = 0;
        for (t = 0; t < UB; t++) {
            values[1] = t + 1;
            indices[1] = i * UB + t + 1;
            glp_add_rows(mip, 1);
            glp_set_mat_row(mip, line, 2, indices, values);
            glp_set_row_bnds(mip, line, GLP_UP, 0, 0);
            line++;
        }
    }

    glp_add_rows(mip, dataStructure->nodeCount);

    {
        double *values = (double *) mmalloc((UB + 1) * sizeof(double));
        int *indices = (int *) mmalloc((UB + 1) * sizeof(int));
        indices[0] = 0;
        values[0] = 0;
        for (i = 0; i < dataStructure->nodeCount; i++) // \sum_{t} s_{i,t} = |i| \forall i
        {
            for (j = 0; j < UB; j++) {
                indices[j + 1] = i * UB + j + 1;
                values[j + 1] = 1;
            }
            glp_set_mat_row(mip, line, UB, indices, values);
            glp_set_row_bnds(mip, line, GLP_FX, nodeSlots[i], nodeSlots[i]);
            line++;
        }
        ffree(values);
        ffree(indices);
    }

    { // LB <= z <= UB
        double values[2];
        int indices[2];
        values[0] = indices[0] = 0;
        values[1] = 1;
        indices[1] = dataStructure->nodeCount * UB + 1;
        glp_add_rows(mip, 1);
        glp_set_mat_row(mip, line, 1, indices, values);
        glp_set_row_bnds(mip, line, GLP_DB, LB, UB);
        line++;
    }

    int number_variables = glp_get_num_cols(mip);
    for (i = 1; i <= number_variables; i++) {
        if (i <= UB * dataStructure->nodeCount) {
            glp_set_col_kind(mip, i, GLP_BV);
        }
        else {
            glp_set_col_kind(mip, i, GLP_IV);
            glp_set_col_bnds(mip, i, GLP_LO, 0, 0);
            glp_set_obj_coef(mip, i, 1);
        }
    }

    glp_iocp settings;
    glp_init_iocp(&settings);
    settings.presolve = GLP_ON;
    settings.msg_lev = GLP_MSG_OFF; //GLP_MSG_ALL;
    settings.br_tech = GLP_BR_FFV;
    settings.bt_tech = GLP_BT_BPH;
    settings.mir_cuts = GLP_ON;
    settings.tm_lim = 5000; // Time limit - 1 hour.
    settings.mip_gap = 0.0; // Relative tolerance. (1 ~ 100 %)

    int ret_code = glp_intopt(mip, &settings);

    if (ret_code != 0 && ret_code != GLP_ETMLIM && ret_code != GLP_EMIPGAP) {
        for (i = 0; i < tmp; i++)
            rSlotsAssignment[i] = rSlotsAssignmentTmp[i];
        ffree(rSlotsAssignmentTmp);
        ffree(nodeSlotIndexes);
        ffree(nodesLongestVariant);
        glp_delete_prob(mip);
        glp_free_env();
        return UB;
    }
    else {
        int status = glp_mip_status(mip);
        if (status == GLP_OPT || status == GLP_FEAS) {
            *rOpt = (int) glp_mip_obj_val(mip);

            for (i = 0; i < UB * dataStructure->nodeCount; ++i) {
                int x = (int) glp_mip_col_val(mip, i + 1);
                if ((x == 1) && (assignedSlots[i / UB].find(i % UB) ==
                                 assignedSlots[i / UB].end())) // P�i�azujeme jen pokud ji� nebylo p�i�azeno
                { // Ji� nemus� b�t sloty jednoho nodu p�i�azeny se�azen�, ale m��e se objevit i po�ad� na p�esk��ku
                    // D��v� sloty obsazovaly slotID 2 a 4, nyn� je t�eba rozvrhnout dal�� slot a to m��e b�t i slot 3 i kdy� v rSlotAssigment
                    // je a� za sloty se slotID 2 a 4.
                    for (j = nodeSlotIndexes[i / UB]; j < nodeSlotIndexes[i / UB] + nodeSlots[i / UB]; j++) {
                        if (rSlotsAssignment[j] == -1) {
                            rSlotsAssignment[j] = i % UB;
                            printf("To node %d slot %d assign slotID %d\n", i / UB, j - nodeSlotIndexes[i / UB], i % UB);
                            break;
                        }
                    }
                }
            }
        }
        else {
            for (i = 0; i < tmp; i++)
                rSlotsAssignment[i] = rSlotsAssignmentTmp[i];
            ffree(rSlotsAssignmentTmp);
            ffree(nodeSlotIndexes);
            ffree(nodesLongestVariant);
            glp_delete_prob(mip);
            glp_free_env();
            return UB;
        }
    }

    glp_delete_prob(mip);
    glp_free_env();
#endif

    ffree(rSlotsAssignmentTmp);
    ffree(nodeSlotIndexes);
    ffree(nodesLongestVariant);

    return 1;
}
