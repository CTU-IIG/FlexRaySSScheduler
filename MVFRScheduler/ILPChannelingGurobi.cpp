//
// Created by jenik on 12/3/15.
//

#include "ILPChannelingGurobi.h"

using namespace std;

ChannelsFitness ILPChannelingGurobi(DataStructure *dataStructure, double const alpha) {
    try {
        unsigned int messageCount;
        double alphaCoef = alpha;
        GRBEnv env = GRBEnv();
        GRBModel model = GRBModel(env);
        //model.getEnv().set(GRB_DoubleParam_TimeLimit, 3600);
        ChannelsFitness cfResult;
        CAMessage *messages;
        messageCount = static_cast<unsigned int>(AggregateMessages(dataStructure, messages));

        vector<int> w(messageCount);
        transform(messages, messages + messageCount, w.begin(), [](CAMessage &message) { return message.payload; });
        int sumW = 0;
        for (int i = 0; i < messageCount; i++) {
            for (int j = 0; j < static_cast<int>(messages[i].nodes.size()); j++) {
                if (dataStructure->nodesOnBothChannels[messages[i].nodes[j]] != 1) {
                    sumW += messages[i].payload;
                    break;
                }
            }
        }
        if (alphaCoef < 0)
            alphaCoef = 1.0 / sumW;

        GRBVar z = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "z");
        GRBVar Pa = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "Pa");
        GRBVar Pb = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "Pb");
        GRBVar Pg = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "Pg");

        vector<GRBVar> Uea(messageCount);
        vector<GRBVar> Ueb(messageCount);
        vector<GRBVar> xi(static_cast<unsigned int>(dataStructure->nodeCount));

        generate_n(Uea.begin(), messageCount,
                   [&model]() { return model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "Uea"); });
        generate_n(Ueb.begin(), messageCount,
                   [&model]() { return model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "Ueb"); });
        generate_n(xi.begin(), dataStructure->nodeCount,
                   [&model]() { return model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "xi"); });

        model.update();

        model.setObjective(z + alphaCoef * Pg, GRB_MINIMIZE);

        model.addConstr(Pa <= z);
        model.addConstr(Pb <= z);
        model.addConstr(Pa + Pb - sumW == Pg);
        model.addConstr(expSum(Uea, w) == Pa);
        model.addConstr(expSum(Ueb, w) == Pb);
        for (int e = 0; e < messageCount; e++) {
            for (int i = 0; i < static_cast<int>(messages[e].nodes.size()); i++) // Problem!!!
            {
                if (dataStructure->nodesOnBothChannels[messages[e].nodes[i]] != 1) {
                    model.addConstr(xi[messages[e].nodes[i]] - Uea[e] <= 0);
                    model.addConstr(xi[messages[e].nodes[i]] + Ueb[e] >= 1);
                }
            }
        }

        model.addConstr(xi[0] == 1);
        model.optimize();
        switch (model.get(GRB_IntAttr_Status)) {
            case GRB_INFEASIBLE:
            case GRB_UNBOUNDED:
                cfResult.channelA = cfResult.channelB = cfResult.gateway = -1;
                return cfResult;
            case GRB_TIME_LIMIT:
                if (model.get(GRB_IntAttr_SolCount) <= 0) {
                    cfResult.channelA = cfResult.channelB = cfResult.gateway = -1;
                    return cfResult;
                }
            default:
                break;
        }


        for (int i = 0; i < dataStructure->nodeCount; i++)
            dataStructure->nodesChannel[i] = static_cast<char>(xi[i].get(GRB_DoubleAttr_X));

        /*for(i = 0; i < number_variables; i++)
        {
            printf("%d ", static_cast<int>(glp_mip_col_val(mip,i+1)));
        }*/
        cfResult.channelA = static_cast<int>(floor(Pa.get(GRB_DoubleAttr_X) + 0.5));
        cfResult.channelB = static_cast<int>(floor(Pb.get(GRB_DoubleAttr_X) + 0.5));
        cfResult.gateway = static_cast<int>(floor(Pg.get(GRB_DoubleAttr_X) + 0.5));
        for (int i = 0; i < dataStructure->nodeCount; i++) {
            printf("%d ", static_cast<int>(xi[i].get(GRB_DoubleAttr_X)));
        }
        printf("\n");
        printf("/ %d, %d, %d\n", cfResult.channelA, cfResult.channelB, cfResult.gateway);

        delete[] messages;
        return cfResult;
    } catch (GRBException e) {
        cout << e.getMessage() << "\n";
    }
}