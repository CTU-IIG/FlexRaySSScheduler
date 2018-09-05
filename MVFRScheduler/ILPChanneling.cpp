/// \file ILPChanneling.cpp ILP formulation of the FlexRay channel assigning problem
#include "MVFRScheduler.h"
#include "glpk.h"

/*ChannelsFitness computeChannelAssignmentUtilityILP(DataStructure* dataStructure, CAMessage* messages, int messagesCount, glp_prob* mip)
{
	int i, j;
	char inA, inB;
	ChannelsFitness cfResult;
	cfResult.channelA = cfResult.channelB = cfResult.gateway = 0;
	int A = 0, B = 0;
	for(i = 0; i < messagesCount; i++)
	{
		inA = inB = 0;
		for(j = 0; j < static_cast<int>(messages[i].nodes.size()); j++)
		{
			if(dataStructure->nodesOnBothChannels[messages[i].nodes[j]] == 1)
				continue;
			if(dataStructure->nodesChannel[messages[i].nodes[j]] == 1)
				inA = 1;
			else
				inB = 1;
			if((inA==1)&&(inB==1))
				break;
		}
		if(inA == 1)
		{
			if(static_cast<char>(glp_mip_col_val(mip, dataStructure->nodeCount + i + 1)) != 1)
				printf("%d A\n", i);
			else
				A += messages[i].payload;
			cfResult.channelA += messages[i].payload;
		}
		if(inB == 1)
		{
			if(static_cast<char>(glp_mip_col_val(mip, dataStructure->nodeCount + messagesCount + i + 1)) != 1)
				printf("%d B\n", i);
			else
				B += messages[i].payload;
			cfResult.channelB += messages[i].payload;
		}
		if((inA==1)&&(inB==1))
			cfResult.gateway += messages[i].payload;
	}
	printf("%d, %d \n", A, B);
	return cfResult;
}*/

ChannelsFitness ILPChanneling(DataStructure *dataStructure, const double alpha) {
    int i, j;
    int messageCount;
    double alphaCoef = alpha;
    ChannelsFitness cfResult;
    CAMessage *messages;
    messageCount = AggregateMessages(dataStructure, messages);

    int payloadsSum = 0;
    for (i = 0; i < messageCount; i++) {
        for (j = 0; j < static_cast<int>(messages[i].nodes.size()); j++) {
            if (dataStructure->nodesOnBothChannels[messages[i].nodes[j]] != 1) {
                payloadsSum += messages[i].payload;
                break;
            }
        }
    }
    if (alphaCoef < 0)
        alphaCoef = 1.0 / payloadsSum;

    // ILP �e�en� speci�ln�ho graph coloring probl�mu
    glp_prob *mip = glp_create_prob();
    glp_set_prob_name(mip, "ILPChanneling");
    glp_set_obj_dir(mip, GLP_MIN);

    int line = 1;
    glp_add_cols(mip, dataStructure->nodeCount + 2 * messageCount + 4);

    // Nastaven� prom�nn�ch a kriteria
    int number_variables = glp_get_num_cols(mip);
    for (i = 1; i <= number_variables; i++) {
        if (i <= dataStructure->nodeCount) {
            glp_set_col_kind(mip, i, GLP_BV);
        }
        else if (i <= dataStructure->nodeCount + 2 * messageCount) {
            glp_set_col_kind(mip, i, GLP_CV);
            glp_set_col_bnds(mip, i, GLP_DB, 0, 1);
        }
        else if (i <= dataStructure->nodeCount + 2 * messageCount + 2) {
            glp_set_col_kind(mip, i, GLP_CV);
            glp_set_col_bnds(mip, i, GLP_LO, 0, 0);
        }
        else if (i <= dataStructure->nodeCount + 2 * messageCount + 3) {
            glp_set_col_kind(mip, i, GLP_CV);
            glp_set_col_bnds(mip, i, GLP_LO, 0, 0);
            glp_set_obj_coef(mip, i, alphaCoef); // 0.0001
        }
        else {
            glp_set_col_kind(mip, i, GLP_CV);
            glp_set_col_bnds(mip, i, GLP_LO, 0, 0);
            glp_set_obj_coef(mip, i, 1);
        }
    }

    // pomocn� prom�nn�
    int AIndice = dataStructure->nodeCount + 2 * messageCount + 1;
    int BIndice = dataStructure->nodeCount + 2 * messageCount + 2;
    int GWIndice = dataStructure->nodeCount + 2 * messageCount + 3;
    int zIndice = dataStructure->nodeCount + 2 * messageCount + 4;

    // A <= z
    {
        double values[] = {0, 1, -1};
        int indices[] = {0, AIndice, zIndice};
        glp_add_rows(mip, 1);
        glp_set_mat_row(mip, line, 2, indices, values);
        glp_set_row_bnds(mip, line, GLP_UP, 0, 0);
        line++;
    }

    //B <= z
    {
        double values[] = {0, 1, -1};
        int indices[] = {0, BIndice, zIndice};
        glp_add_rows(mip, 1);
        glp_set_mat_row(mip, line, 2, indices, values);
        glp_set_row_bnds(mip, line, GLP_UP, 0, 0);
        line++;
    }

    // GW = A+B-\sum{payload_m}
    {
        double values[] = {0, 1, -1, -1};
        int indices[] = {0, GWIndice, AIndice, BIndice};

        glp_add_rows(mip, 1);
        glp_set_mat_row(mip, line, 3, indices, values);
        glp_set_row_bnds(mip, line, GLP_FX, -payloadsSum, -payloadsSum);
        line++;
    }

    // A = \sum{w_m * y_{m,A}}
    {
        double *values = new double[messageCount + 2];
        int *indices = new int[messageCount + 2];
        values[0] = 0;
        indices[0] = 0;
        values[1] = 1;
        indices[1] = AIndice;
        for (i = 0; i < messageCount; i++) {
            values[i + 2] = -messages[i].payload;
            indices[i + 2] = dataStructure->nodeCount + 1 + i;
        }
        glp_add_rows(mip, 1);
        glp_set_mat_row(mip, line, messageCount + 1, indices, values);
        glp_set_row_bnds(mip, line, GLP_FX, 0, 0);
        line++;
        delete[] values;
        delete[] indices;
    }

    // B = \sum{w_m * y_{m,B}}
    {
        double *values = new double[messageCount + 2];
        int *indices = new int[messageCount + 2];
        values[0] = 0;
        indices[0] = 0;
        values[1] = 1;
        indices[1] = BIndice;
        for (i = 0; i < messageCount; i++) {
            values[i + 2] = -messages[i].payload;
            indices[i + 2] = dataStructure->nodeCount + messageCount + 1 + i;
        }
        glp_add_rows(mip, 1);
        glp_set_mat_row(mip, line, messageCount + 1, indices, values);
        glp_set_row_bnds(mip, line, GLP_FX, 0, 0);
        line++;
        delete[] values;
        delete[] indices;
    }

    /*{ // Zak�zat um�st�n� v�ech ECU do jednoho kan�lu
        double* values = new double[dataStructure->nodeCount + 1];
        int* indices = new int[dataStructure->nodeCount + 1];
        values[0] = 0;
        indices[0] = 0;
        for(i = 0; i < dataStructure->nodeCount; i++)
        {
            values[i+1] = 1;
            indices[i+1] = i+1;
        }
        glp_add_rows(mip,1);
        glp_set_mat_row(mip,line,dataStructure->nodeCount,indices,values);
        glp_set_row_bnds(mip,line,GLP_LO ,1,0);
        line++;
        glp_add_rows(mip,1);
        glp_set_mat_row(mip,line,dataStructure->nodeCount,indices,values);
        glp_set_row_bnds(mip,line,GLP_UP ,0,dataStructure->nodeCount - 1);
        line++;
        delete[] values;
        delete[] indices;
    }*/

    // x_i - y_{m,A} <= 0 \forall m, i \in ECUs_m
    for (i = 0; i < messageCount; i++) {
        for (j = 0; j < static_cast<int>(messages[i].nodes.size()); j++) {
            if (dataStructure->nodesOnBothChannels[messages[i].nodes[j]] != 1) {
                double values[] = {0, 1, -1};
                int indices[] = {0, messages[i].nodes[j] + 1, dataStructure->nodeCount + i + 1};
                glp_add_rows(mip, 1);
                glp_set_mat_row(mip, line, 2, indices, values);
                glp_set_row_bnds(mip, line, GLP_UP, 0, 0);
                line++;
            }
        }
    }

    // -x_i - y_{m,B} <= -1 \forall m, i \in ECUs_m
    for (i = 0; i < messageCount; i++) {
        for (j = 0; j < static_cast<int>(messages[i].nodes.size()); j++) {
            if (dataStructure->nodesOnBothChannels[messages[i].nodes[j]] != 1) {
                double values[] = {0, -1, -1};
                int indices[] = {0, messages[i].nodes[j] + 1, dataStructure->nodeCount + messageCount + i + 1};
                glp_add_rows(mip, 1);
                glp_set_mat_row(mip, line, 2, indices, values);
                glp_set_row_bnds(mip, line, GLP_UP, 0, -1);
                line++;
            }
        }
    }

    // nody co jsou v AB rovou p�i�ad�me jedno kam
    {
        for (i = 0; i < dataStructure->nodeCount; i++) {
            if (dataStructure->nodesOnBothChannels[i] == 1) {
                double values[] = {0, 1};
                int indices[] = {0, i + 1};
                glp_add_rows(mip, 1);
                glp_set_mat_row(mip, line, 1, indices, values);
                glp_set_row_bnds(mip, line, GLP_FX, 1, 1);
                line++;
            }
        }
    }

    // x_0 = 1 // prvn� node co nen� v AB nastav do A�ka
    {
        for (i = 0; i < dataStructure->nodeCount; i++) {
            if (dataStructure->nodesOnBothChannels[i] != 1) {
                double values[] = {0, 1};
                int indices[] = {0, i + 1};
                glp_add_rows(mip, 1);
                glp_set_mat_row(mip, line, 1, indices, values);
                glp_set_row_bnds(mip, line, GLP_FX, 1, 1);
                line++;
                break;
            }
        }
    }

    glp_iocp settings;
    glp_init_iocp(&settings);
    settings.presolve = GLP_ON;
    settings.msg_lev = GLP_MSG_ALL; //GLP_MSG_OFF ; //GLP_MSG_ALL;
    settings.br_tech = GLP_BR_MFV;
    settings.bt_tech = GLP_BT_BPH;
    settings.mir_cuts = GLP_ON;
    settings.tm_lim = 3600000; // Time limit - 1 hour.
    settings.mip_gap = 0.0; // Relative tolerance. (1 ~ 100 %)

    int ret_code = glp_intopt(mip, &settings);

    if (ret_code != 0 && ret_code != GLP_ETMLIM && ret_code != GLP_EMIPGAP) {
        cfResult.channelA = cfResult.channelB = cfResult.gateway = -1;
        return cfResult;
    }

    int status = glp_mip_status(mip);
    if (status == GLP_OPT || status == GLP_FEAS) {

        for (i = 0; i < dataStructure->nodeCount; i++)
            dataStructure->nodesChannel[i] = static_cast<char>(glp_mip_col_val(mip, i + 1));

        /*for(i = 0; i < number_variables; i++)
        {
            printf("%d ", static_cast<int>(glp_mip_col_val(mip,i+1)));
        }*/
        cfResult.channelA = static_cast<int>(floor(glp_mip_col_val(mip, AIndice) + 0.5));
        cfResult.channelB = static_cast<int>(floor(glp_mip_col_val(mip, BIndice) + 0.5));
        cfResult.gateway = static_cast<int>(floor(glp_mip_col_val(mip, GWIndice) + 0.5));
        printf("/ %d, %d, %d", cfResult.channelA, cfResult.channelB, cfResult.gateway);
    }
    glp_delete_prob(mip);
    glp_free_env();

    return cfResult;
}
