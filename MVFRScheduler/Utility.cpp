/// \file Utility.cpp Utilities for memory handling and printing
#include "MVFRScheduler.h"
#include "glpk.h"
#include "gurobi_c++.h"

using namespace std;

char verboseAllocInfo = 0; // 0 = no, 1 = only errors, 2 = errors + free, 3 = all

void error(const char *message) {
    printf("Error: %s\n", message);
}

void print(char *message) {
    printf("%s", message);
}

void debug(char *message) {
    printf("%s", message);
}

char getMEMatrixValue(char *meMatrix, int i, int j) {
    if (i == j)
        return 0;
    if (i > j)
        return meMatrix[i * (i + 1) / 2 + j];
    else
        return meMatrix[j * (j + 1) / 2 + i];
}

void setMEMatrixValue(char *meMatrix, int i, int j, char value) {
    if (i == j)
        return;
    if (i > j) {
        meMatrix[i * (i + 1) / 2 + j] = value;
    }
    else {
        meMatrix[j * (j + 1) / 2 + i] = value;
    }
}

std::set<void *> allocatedMemory;

void *mmalloc(size_t _Size) {
    void *memory = malloc(_Size);
    if (verboseAllocInfo > 2)
        printf("ma%p ", memory);
    allocatedMemory.insert(memory);
    return memory;
};

void *ccalloc(size_t _Count, size_t _Size) {
    void *memory = calloc(_Count, _Size);
    if (verboseAllocInfo > 2)
        printf("a%p ", memory);
    allocatedMemory.insert(memory);
    return memory;
}

void ffree(void *_Memory) {
    std::set<void *>::iterator it;
    it = allocatedMemory.find(_Memory);
    if (it != allocatedMemory.end()) {
        if (verboseAllocInfo > 1)
            printf("f%p ", *it);
        free(_Memory);
        allocatedMemory.erase(it);
    }
    else {
        if (verboseAllocInfo > 0)
            printf("fe%p ", _Memory);
    }
}

void ffreeAll() {
    std::set<void *>::iterator it;
    for (it = allocatedMemory.begin(); it != allocatedMemory.end(); ++it) {
        if (verboseAllocInfo > 0)
            printf("fa%p ", *it);
        free(*it);
    }
}

char isAllocated(void *_Memory) {
    std::set<void *>::iterator it;
    if (_Memory == nullptr)
        return false;
    it = allocatedMemory.find(_Memory);
    return it != allocatedMemory.end();
}

int UpdateInputData(DataStructure *dataStructure) {
    int i;
    int release, deadline;
    for (i = 0; i < dataStructure->signalsCount; i++) {
        dataStructure->signalPeriods[i] = dataStructure->signalPeriods[i] / dataStructure->cycleLength;
        release = dataStructure->signalReleaseDates[i];
        dataStructure->signalReleaseDates[i] = (int) ceil(
                dataStructure->signalReleaseDates[i] / (double) dataStructure->cycleLength);
        if (dataStructure->signalReleaseDates[i] > 0) {
            if (dataStructure->signalReleaseDates[i] >= dataStructure->signalPeriods[i]) {
                --dataStructure->signalReleaseDates[i];
            }
        }
        if (dataStructure->signalReleaseDates[i] >= dataStructure->signalPeriods[i]) {
            printf("The release time for %s is not allowed (ReleaseTime:%d/Period%d)", dataStructure->signalNames[i],
                   release, dataStructure->signalPeriods[i] * dataStructure->cycleLength);
            return 1;
        }
        deadline = dataStructure->signalDeadlines[i];
        dataStructure->signalDeadlines[i] = (int) floor(
                (dataStructure->signalDeadlines[i] + 1) / (double) dataStructure->cycleLength);
        if (dataStructure->signalDeadlines[i] > dataStructure->signalPeriods[i])
            dataStructure->signalDeadlines[i] = dataStructure->signalPeriods[i];
        if (dataStructure->signalReleaseDates[i] > dataStructure->signalDeadlines[i]) {
            printf("The release time is greater than deadline for %s (ReleaseTime:%d/Deadline%d)",
                   dataStructure->signalNames[i],
                   release, deadline);
            return 1;
        }
    }
    dataStructure->hyperPeriod = dataStructure->hyperPeriod / dataStructure->cycleLength;
    return 0;
}

int graphColoringHeuristic(DataStructure *dataStructure, int *nodeSlots, int *rSlotsAssignment) {
    int i, j, k, sum, UB, tmp;
    // Compute UB - Sekven�n� heuristika
    sum = 0;
    UB = 0;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        sum += nodeSlots[i];
    }
    //int* slotsAssignment = (int*)mmalloc(sum * sizeof(int)); // do jak�ho SlotID je slot p�i�azen
    int *slotsNode = (int *) ccalloc(static_cast<size_t>(sum), sizeof(int)); // K jak�mu nodu slot pat��
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
            rSlotsAssignment[tmp] = -1;
            colours[tmp] = tmp;
            tmp++;
        }
    }

    for (i = 0; i < sum; i++) {
        possibleColours.clear();
        possibleColours.insert(colours, colours + sum);
        for (j = 0; j < dataStructure->nodeCount; j++) {
            if ((slotsNode[i] == j) || (getMEMatrixValue(dataStructure->mutualExclusionNodeMatrix, slotsNode[i], j))) {
                for (k = nodeSlotIndexes[j]; k < nodeSlotIndexes[j] + nodeSlots[j]; k++) {
                    possibleColours.erase(rSlotsAssignment[k]);
                }
            }
        }
        it = possibleColours.begin();
        rSlotsAssignment[i] = *it;
        if (*it > UB) UB = *it;
    }
    UB = UB + 1;
    ffree(slotsNode);
    ffree(nodeSlotIndexes);
    ffree(colours);
    return UB;
}

int graphColoring(DataStructure *dataStructure, int *nodeSlots, int *rUB, int *rLB, int *rOpt, int *rSlotsAssignment) {
    int i, j, tmp, UB, LB;
    UB = 0;
    LB = 0;

    int *nodeSlotIndexes = (int *) mmalloc(
            dataStructure->nodeCount * sizeof(int)); // Od jak�ho indexu v slotsNode za��naj� sloty dan�ho nodu
    int *nodesLongestVariant = (int *) mmalloc(
            dataStructure->nodeCount * sizeof(int)); // Bin�rn� pole ur�uj�c� kter� nody obsahuje nejde�� z variant

    tmp = 0;
    for (i = 0; i < dataStructure->nodeCount; i++)// p�edpo��t�v�n� struktur
    {
        nodeSlotIndexes[i] = tmp;
        for (j = 0; j < nodeSlots[i]; j++) {
            tmp++;
        }
    }

    UB = graphColoringHeuristic(dataStructure, nodeSlots, rSlotsAssignment);

    LB = ComputeLBForGraphColoring(dataStructure, nodesLongestVariant, nodeSlots);

    *rUB = UB;
    *rLB = LB;
    *rOpt = UB;

    printf("LB. %d, UB: %d\n", LB, UB);
    if (LB == UB) {
        ffree(nodeSlotIndexes);
        ffree(nodesLongestVariant);
        return 1;
    }
    printf("LB. %d, UB: %d\n", LB, UB);

    // ILP reseni specialniho graph coloring problemu
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
                    printf("f%d to %d\n");
                }
            }
        }
    }

    // symmetry breaking
    unsigned int currentIndex = 0;
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        if (nodesLongestVariant[i] == 1) {
            for (int j = 0; j < nodeSlots[i]; j++) {
                model.addConstr(wik[i][currentIndex] == 1, "symmetry breaking");
                currentIndex++;
            }
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
            if (x == 1) {
                rSlotsAssignment[nodeSlotIndexes[i]] = k;
                printf("To node %d slot %d assign slotID %d\n", i, nodeSlotIndexes[i], k);
                nodeSlotIndexes[i]++;
            }
        }
    }

#else
    glp_prob *mip = glp_create_prob();
    glp_set_prob_name(mip, "GraphColoring");
    glp_set_obj_dir(mip, GLP_MIN);

    int line = 1;
    int t;
    glp_add_cols(mip, UB * dataStructure->nodeCount + 1);
    for (i = 0; i < dataStructure->nodeCount; i++) // s_{i,t} + s_{j,t} <= 1 \forall i,j,t | i<j ; NMEM(i,j)==1
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

    { // Symmetry breaking - nejdelsi varianta bude v reseni striktne nastavena
        double values[2];
        int indices[2];
        tmp = 0;
        values[0] = indices[0] = 0;
        values[1] = 1;
        for (i = 0; i < dataStructure->nodeCount; i++) {
            if (nodesLongestVariant[i]) {
                for (j = 0; j < nodeSlots[i]; j++) {
                    indices[1] = i * UB + tmp + 1;
                    glp_add_rows(mip, 1);
                    glp_set_mat_row(mip, line, 1, indices, values);
                    glp_set_row_bnds(mip, line, GLP_FX, 1, 1);
                    line++;
                    tmp++;
                }
            }
        }
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
    settings.tm_lim = 300000; // Time limit - 1 hour.
    settings.mip_gap = 0.0; // Relative tolerance. (1 ~ 100 %)

    int ret_code = glp_intopt(mip, &settings);

    if (ret_code != 0 && ret_code != GLP_ETMLIM && ret_code != GLP_EMIPGAP) {
        return UB;
    }
    else {
        int status = glp_mip_status(mip);
        if (status == GLP_OPT || status == GLP_FEAS) {
            *rOpt = (int) glp_mip_obj_val(mip);

            for (i = 0; i < UB * dataStructure->nodeCount; ++i) {
                int x = (int) glp_mip_col_val(mip, i + 1);
                if (x == 1) {
                    rSlotsAssignment[nodeSlotIndexes[i / UB]] = i % UB;
                    printf("To node %d slot %d assign slotID %d\n", i / UB, nodeSlotIndexes[i / UB], i % UB);
                    nodeSlotIndexes[i / UB]++;
                }
            }
        }
    }

    glp_delete_prob(mip);
    glp_free_env();
#endif

    ffree(nodeSlotIndexes);
    ffree(nodesLongestVariant);

    return 1;
}
