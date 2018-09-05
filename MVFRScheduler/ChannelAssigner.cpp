#include "MVFRScheduler.h"
#include <map>
#include <algorithm>

long ComputeNodeHash(std::vector<int> nodes) {
    int i;
    long hashAddress = 5381;
    for (i = 0; i < static_cast<int>(nodes.size()); i++) {
        hashAddress = ((hashAddress << 5) + hashAddress) + nodes[i];
    }
    return hashAddress;
}

int AggregateMessages(const DataStructure *dataStructure, CAMessage *&messages) {
    int i, j;
    std::set<long> hashes;
    std::set<long>::iterator it;
    // Compute receiver list hashes
    for (i = 0; i < dataStructure->signalsCount; i++) {
        if (dataStructure->signalFaultTolerant[i] ==
            1) // Remove the signals that are fault tolerant (transmitted to both channels)
            continue;
        for (j = 0; j <
                    static_cast<int>(dataStructure->signalReceivers[i].size()); j++) // Pouze kontrola zda-li neni nejaky problem se vstupni instanci
            if (dataStructure->signalReceivers[i][j] == dataStructure->signalNodeIDs[i])
                printf("Reciever is the same as the transmitter - %d!!!\n", i);

        std::vector<int> messNodes = dataStructure->signalReceivers[i];
        messNodes.push_back(dataStructure->signalNodeIDs[i]);
        sort(messNodes.begin(), messNodes.end());
        for (j = 1; j < static_cast<int>(messNodes.size()); j++)
            if (messNodes[j] == messNodes[j - 1])
                printf("The same reciever assigned twice - %d!!\n", i);
        hashes.insert(ComputeNodeHash(messNodes));
    }

    // Map hash to integer
    std::map<long, int> hashMap;
    i = 0;
    for (it = hashes.begin(); it != hashes.end(); ++it) {
        hashMap[*it] = i;
        i++;
    }

    // Initialize messages
    messages = new CAMessage[hashes.size()];
    for (i = 0; i < static_cast<int>(hashes.size()); i++) {
        messages[i].payload = 0;
    }

    // Fill the messages up
    for (i = 0; i < dataStructure->signalsCount; i++) {
        if (dataStructure->signalFaultTolerant[i] == 1)
            continue;
        std::vector<int> messNodes = dataStructure->signalReceivers[i];
        messNodes.push_back(dataStructure->signalNodeIDs[i]);
        sort(messNodes.begin(), messNodes.end());
        int messID = hashMap[ComputeNodeHash(messNodes)];
        if (messages[messID].nodes.size() == 0) {
            for (j = 0; j < static_cast<int>(messNodes.size()); j++)
                messages[messID].nodes.push_back(messNodes[j] - 1);
        }

        if (messages[messID].nodes.size() != messNodes.size()) {
            if (verboseLevel > 1) printf("Inaccuracy because the collision in hash");
        }
        else {
            int recOK = TRUE;
            for (j = 0; j < static_cast<int>(messages[messID].nodes.size()); j++) {

                if (messages[messID].nodes[j] != messNodes[j] - 1) {
                    recOK = FALSE;
                    break;
                }
            }
            if (recOK == FALSE) {
                if (verboseLevel > 1) printf("Inaccuracy because the collision in hash");
                return -1;
            }
            else {
                messages[messID].payload = messages[messID].payload + dataStructure->signalLengths[i] *
                                                                      (dataStructure->hyperPeriod /
                                                                       dataStructure->signalPeriods[i]);
            }
        }
    }
    return static_cast<int>(hashes.size());
}

ChannelsFitness computeChannelAssignmentUtility(DataStructure *dataStructure, CAMessage *messages, int messagesCount) {
    int i, j;
    char inA, inB;
    ChannelsFitness cfResult;
    cfResult.channelA = cfResult.channelB = cfResult.gateway = 0;
    for (i = 0; i < messagesCount; i++) {
        inA = inB = 0;
        for (j = 0; j < static_cast<int>(messages[i].nodes.size()); j++) {
            if (dataStructure->nodesOnBothChannels[messages[i].nodes[j]] == 1)
                continue;
            if (dataStructure->nodesChannel[messages[i].nodes[j]] == 1)
                inA = 1;
            else
                inB = 1;
            if ((inA == 1) && (inB == 1))
                break;
        }
        if (inA == 1)
            cfResult.channelA += messages[i].payload;
        if (inB == 1)
            cfResult.channelB += messages[i].payload;
        if ((inA == 1) && (inB == 1))
            cfResult.gateway += messages[i].payload;
        if ((inA == 1) && (inB == 1)) {
            if (messages[i].channel != CHANNELAB) {
                printf("EAB");
            }
        }
        else if (inA == 1) {
            if (messages[i].channel != CHANNELA)
                printf("EA");
        }
        else if (inB == 1) {
            if (messages[i].channel != CHANNELB)
                printf("EB %d,", messages[i].channel);
        }

    }
    return cfResult;
}

ChannelsFitness EvaluateAddingNodeToChanel(int channel, int node, NodeList *node_list, ChannelsFitness cf) {
    unsigned int i;
    ChannelsFitness result;
    result.channelA = cf.channelA;
    result.channelB = cf.channelB;
    result.gateway = cf.gateway;
    for (i = 0; i < node_list[node].messages.size(); i++) {
        if (node_list[node].messages[i]->channel == CHANNELUNDEF) {
            if (channel == CHANNELA)
                result.channelA += node_list[node].messages[i]->payload;
            else if (channel == CHANNELB)
                result.channelB += node_list[node].messages[i]->payload;
        }
        else if ((node_list[node].messages[i]->channel == CHANNELA) && (channel == CHANNELB)) {
            result.channelB += node_list[node].messages[i]->payload;
            result.gateway += node_list[node].messages[i]->payload;
        }
        else if ((node_list[node].messages[i]->channel == CHANNELB) && (channel == CHANNELA)) {
            result.channelA += node_list[node].messages[i]->payload;
            result.gateway += node_list[node].messages[i]->payload;
        }
    }
    return result;
}

void AssignNodeToChannel(int channel, int node, NodeList *node_list) {
    unsigned int i;
    node_list[node].channel = channel;
    //dataStructure->nodesChannel[node] = channel;//
    for (i = 0; i < node_list[node].messages.size(); i++) {
        if (node_list[node].messages[i]->channel == CHANNELUNDEF) {
            node_list[node].messages[i]->channel = channel;
        }
        else if ((node_list[node].messages[i]->channel == CHANNELA) && (channel == CHANNELB)) {
            node_list[node].messages[i]->channel = CHANNELAB;
        }
        else if ((node_list[node].messages[i]->channel == CHANNELB) && (channel == CHANNELA)) {
            node_list[node].messages[i]->channel = CHANNELAB;
        }
    }
}

ChannelsFitness AssignNodesToChannels(const DataStructure *dataStructure, NodeList *node_list, CAMessage *messages,
                                      int messageCount, const double alpha, const double beta) {
    int i, j;
    ChannelsFitness cf;
    cf.channelA = 0;
    cf.channelB = 0;
    cf.gateway = 0;
    int *ordering = new int[dataStructure->nodeCount];
    for (i = 0; i < dataStructure->nodeCount; i++)
        ordering[i] = i;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        int tmp = ordering[i];
        int new_position = rand() % dataStructure->nodeCount;
        ordering[i] = ordering[new_position];
        ordering[new_position] = tmp;
    }

    // Všem zprávám a nodum nastavíme neznámý kanál
    for (i = 0; i < messageCount; i++)
        messages[i].channel = CHANNELUNDEF;
    for (i = 0; i < dataStructure->nodeCount; i++)
        node_list[i].channel = CHANNELUNDEF;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        if (dataStructure->nodesOnBothChannels[i] == 1)
            node_list[i].channel = CHANNELAB;
    }

    // Pripravit node_list
    for (i = 0; i < messageCount; i++) {
        for (j = 0; j < static_cast<int>(messages[i].nodes.size()); j++) {
            node_list[messages[i].nodes[j]].messages.push_back(&messages[i]);
        }
    }

    // Prirazování
    for (i = 0; i < dataStructure->nodeCount; i++) {
        int nodeID = ordering[i];
        if (node_list[nodeID].channel != CHANNELUNDEF) {
            continue;
        }

        //dataStructure->nodesOnBothChannels[nodeID] = ABnodes[nodeID];
        ChannelsFitness cfA = EvaluateAddingNodeToChanel(CHANNELA, nodeID, node_list, cf);
        ChannelsFitness cfB = EvaluateAddingNodeToChanel(CHANNELB, nodeID, node_list, cf);
        if (std::max(beta * cfA.channelA, 1.0 * cfA.channelB) + alpha * cfA.gateway <
            std::max(beta * cfB.channelA, 1.0 * cfB.channelB) + alpha * cfB.gateway) {
            AssignNodeToChannel(CHANNELA, nodeID, node_list);
            cf.channelA = cfA.channelA;
            cf.channelB = cfA.channelB;
            cf.gateway = cfA.gateway;
        }
        else {
            AssignNodeToChannel(CHANNELB, nodeID, node_list);
            cf.channelA = cfB.channelA;
            cf.channelB = cfB.channelB;
            cf.gateway = cfB.gateway;
        }
    }
    delete[] ordering;
    return cf;
}

ChannelsFitness EvaluateNodesSwap(DataStructure *dataStructure, NodeList *node_list, std::vector<int> &nodes,
                                  ChannelsFitness cf) {
    int i, j;
    ChannelsFitness cfResult = cf;
    std::set<CAMessage *> updateMessagesSet;
    std::set<CAMessage *>::iterator umsIt;
    int *newNodesChannel = new int[dataStructure->nodeCount];
    for (i = 0; i < dataStructure->nodeCount; i++)
        newNodesChannel[i] = dataStructure->nodesChannel[i];
    for (i = 0; i < static_cast<int>(nodes.size()); i++) {
        newNodesChannel[nodes[i]] = 1 - newNodesChannel[nodes[i]];
        for (j = 0; j < static_cast<int>(node_list[nodes[i]].messages.size()); j++) {
            updateMessagesSet.insert(node_list[nodes[i]].messages[j]);
        }
    }
    for (umsIt = updateMessagesSet.begin(); umsIt != updateMessagesSet.end(); ++umsIt) {
        int origChannel = (*umsIt)->channel;
        int newChannel = 0;
        int inA, inB;
        inA = inB = 0;
        for (j = 0; j < static_cast<int>((*umsIt)->nodes.size()); j++) {
            if (dataStructure->nodesOnBothChannels[(*umsIt)->nodes[j]] == 1)
                continue;
            if (newNodesChannel[(*umsIt)->nodes[j]] == 1)
                inA = 1;
            else
                inB = 1;
            if ((inA == 1) && (inB == 1))
                break;
        }
        if ((inA == 1) && (inB == 1)) {
            newChannel = CHANNELAB;
        }
        else if (inA == 1) {
            newChannel = CHANNELA;
        }
        else if (inB == 1) {
            newChannel = CHANNELB;
        }
        if (newChannel != origChannel) {
            switch (origChannel) {
                case CHANNELAB:
                    if (newChannel == CHANNELA) {
                        cfResult.channelB -= (*umsIt)->payload;
                        cfResult.gateway -= (*umsIt)->payload;
                    }
                    else if (newChannel == CHANNELB) {
                        cfResult.channelA -= (*umsIt)->payload;
                        cfResult.gateway -= (*umsIt)->payload;
                    }
                    break;
                case CHANNELA:
                    if (newChannel == CHANNELAB) {
                        cfResult.channelB += (*umsIt)->payload;
                        cfResult.gateway += (*umsIt)->payload;
                    }
                    else if (newChannel == CHANNELB) {
                        cfResult.channelA -= (*umsIt)->payload;
                        cfResult.channelB += (*umsIt)->payload;
                    }
                    break;
                case CHANNELB:
                    if (newChannel == CHANNELAB) {
                        cfResult.channelA += (*umsIt)->payload;
                        cfResult.gateway += (*umsIt)->payload;
                    }
                    else if (newChannel == CHANNELA) {
                        cfResult.channelA += (*umsIt)->payload;
                        cfResult.channelB -= (*umsIt)->payload;
                    }
                    break;
                default:
                    printf("Error: EvaluateNodesSwap!!!");
                    break;
            }
        }
    }

    delete[] newNodesChannel;
    return cfResult;
}

ChannelsFitness EvaluateNodeSwap(NodeList *node_list, unsigned int node, ChannelsFitness cf) {
    int i, j;
    ChannelsFitness cfResult;
    cfResult.channelA = cf.channelA;
    cfResult.channelB = cf.channelB;
    cfResult.gateway = cf.gateway;
    int newChannel;
    if (node_list[node].channel == CHANNELAB)
        return cfResult;
    if (node_list[node].channel == CHANNELA)
        newChannel = CHANNELB;
    else
        newChannel = CHANNELA;

    for (i = 0; i < static_cast<int>(node_list[node].messages.size()); i++) {
        if (node_list[node].messages[i]->channel ==
            node_list[node].channel) // Pokud je zpráva zasílána jen v kanále nodu
        {
            char isAnotherNodeInSameChannel = FALSE;
            // Kontrola zda-li i po zmìnì zùstane nìco v daném kanálu
            for (j = 0; j < static_cast<int>(node_list[node].messages[i]->nodes.size()); j++) {
                if (node_list[node_list[node].messages[i]->nodes[j]].channel == node_list[node].channel) {
                    if (node_list[node].messages[i]->nodes[j] != static_cast<int>(node)) {
                        isAnotherNodeInSameChannel = TRUE;
                        break;
                    }
                }
            }
            if (isAnotherNodeInSameChannel == TRUE) {
                // stane se z ní zpráva na AB
                if (newChannel == CHANNELA) {
                    cfResult.channelA += node_list[node].messages[i]->payload;
                }
                else {
                    cfResult.channelB += node_list[node].messages[i]->payload;
                }
                cfResult.gateway += node_list[node].messages[i]->payload;
            }
            else {
                if (newChannel == CHANNELA) {
                    cfResult.channelA += node_list[node].messages[i]->payload;
                    cfResult.channelB -= node_list[node].messages[i]->payload;
                }
                else {
                    cfResult.channelA -= node_list[node].messages[i]->payload;
                    cfResult.channelB += node_list[node].messages[i]->payload;
                }
            }
        }
        else // Zpráva je v AB
        {
            char allInOneNode = true;
            for (j = 0; j < static_cast<int>(node_list[node].messages[i]->nodes.size()); j++) {
                if (node_list[node].messages[i]->nodes[j] == static_cast<int>(node))
                    continue;
                if ((node_list[node_list[node].messages[i]->nodes[j]].channel != newChannel) &&
                    (node_list[node_list[node].messages[i]->nodes[j]].channel) != CHANNELAB) {
                    allInOneNode = false;
                    break;
                }
            }
            if (allInOneNode) {
                if (newChannel == CHANNELA) {
                    cfResult.channelB -= node_list[node].messages[i]->payload;
                }
                else {
                    cfResult.channelA -= node_list[node].messages[i]->payload;
                }
                cfResult.gateway -= node_list[node].messages[i]->payload;
            }
        }
    }

    return cfResult;
}

bool IsNewCFBetter(ChannelsFitness newCF, ChannelsFitness oldCF, const double alpha, const double beta) {
    return ((std::max(beta * newCF.channelA, 1.0 * newCF.channelB) + alpha * newCF.gateway <
             std::max(beta * oldCF.channelA, 1.0 * oldCF.channelB) + alpha * oldCF.gateway) ||
            oldCF.channelA == INT_MAX);
}

void SwapNode(DataStructure *dataStructure, NodeList *node_list, unsigned int node) {
    int i, j, inA, inB;
    std::vector<CAMessage *> &messages = node_list[node].messages;
    dataStructure->nodesChannel[node] = static_cast<char>(1 - dataStructure->nodesChannel[node]);
    if (node_list[node].channel == CHANNELA)
        node_list[node].channel = CHANNELB;
    else if (node_list[node].channel == CHANNELB)
        node_list[node].channel = CHANNELA;
    else {
        printf("Error in node swapping");
        return;
    }

    for (i = 0; i < static_cast<int>(messages.size()); i++) {
        inA = inB = 0;
        for (j = 0; j < static_cast<int>(messages[i]->nodes.size()); j++) {
            if (dataStructure->nodesOnBothChannels[messages[i]->nodes[j]] == 1)
                continue;
            if (dataStructure->nodesChannel[messages[i]->nodes[j]] == 1)
                inA = 1;
            else
                inB = 1;
            if ((inA == 1) && (inB == 1))
                break;
        }
        if ((inA == 1) && (inB == 1)) {
            messages[i]->channel = CHANNELAB;
            continue;
        }
        if (inA == 1) {
            messages[i]->channel = CHANNELA;
        }
        else if (inB == 1) {
            messages[i]->channel = CHANNELB;
        }
        else {
            printf("Error signal swapping!!!");
        }
    }
}

bool TOSOptimize(DataStructure *dataStructure, NodeList *node_list, CAMessage *messages, int messagesCount,
                 ChannelsFitness &cfResult, const double alpha, const double beta) {
    int i;
    ChannelsFitness cfTmp;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        if (dataStructure->nodesOnBothChannels[i] != 1) {
            cfTmp = EvaluateNodeSwap(node_list, i, cfResult);
            if (IsNewCFBetter(cfTmp, cfResult, alpha, beta)) {
                SwapNode(dataStructure, node_list, i);
                cfTmp = computeChannelAssignmentUtility(dataStructure, messages,
                                                        messagesCount); //TODO: Check if it is necessary
                cfResult = cfTmp;
                return true;
            }
        }
    }

    return false;
    /*std::vector<int> nodes;
    for(i = 0; i < dataStructure->nodeCount; i++)
    {
        if(dataStructure->nodesOnBothChannels[i] == 1)
            continue;
        nodes.push_back(i);
        for(j = i+1; j < dataStructure->nodeCount; j++)
        {
            if(dataStructure->nodesOnBothChannels[i] == 1)
                continue;
            nodes.push_back(j);
            cfTmp = EvaluateNodesSwap(dataStructure, node_list, nodes, cfResult);
            if(IsNewCFBetter(cfTmp, cfResult))
            {
                SwapNode(dataStructure, node_list, nodes[0]);
                SwapNode(dataStructure, node_list, nodes[1]);
                cfTmp = computeChannelAssignmentUtility(dataStructure, messages, messagesCount);
                cfResult = cfTmp;
                return true;
            }
            nodes.pop_back();
        }
        nodes.pop_back();
    }
    return false;
     */
}

ChannelsFitness TwoOptSearch(DataStructure *dataStructure, NodeList *node_list, CAMessage *messages, int messageCount,
                             ChannelsFitness cf, const double alpha, const double beta) {
    ChannelsFitness cfResult;
    cfResult = cf;
    while (1) {
        if (!TOSOptimize(dataStructure, node_list, messages, messageCount, cfResult, alpha, beta))
            break;
    }
    return cfResult;
}

ChannelsFitness ChannelAssigner(DataStructure *dataStructure, const double alpha, const double beta,
                                const int maxIteration) {
    const unsigned int seed = static_cast<unsigned int>(time(NULL));
    double alphaCoef = alpha;
    printf("Channer assigner seed - %d\n", seed);
    srand(seed);
    //srand(0);
    CAMessage *messages;
    ChannelsFitness cf;
    ChannelsFitness bestCf;
    bestCf.channelA = INT_MAX;
    bestCf.channelB = INT_MAX;
    bestCf.gateway = INT_MAX;
    int messageCount;
    int i, j;
    NodeList *node_list = new NodeList[dataStructure->nodeCount]; // List of nodes
    char *nodeChannelBest = new char[dataStructure->nodeCount]; // Known best channel assignment
    messageCount = AggregateMessages(dataStructure, messages);
    if (alphaCoef < 0) {
        int payloadsSum = 0;
        for (i = 0; i < messageCount; i++) {
            for (j = 0; j < static_cast<int>(messages[i].nodes.size()); j++) {
                if (dataStructure->nodesOnBothChannels[messages[i].nodes[j]] != 1) {
                    payloadsSum += messages[i].payload;
                    break;
                }
            }
        }
        alphaCoef = 1.0 / payloadsSum;
    }
    for (j = 0; j < maxIteration; j++) {
        for (i = 0; i < dataStructure->nodeCount; i++)
            node_list[i].messages.clear();
        cf = AssignNodesToChannels(dataStructure, node_list, messages, messageCount, alphaCoef, beta);
        for (i = 0; i < dataStructure->nodeCount; i++)
            if (node_list[i].channel == CHANNELB)
                dataStructure->nodesChannel[i] = 0;
            else
                dataStructure->nodesChannel[i] = 1;
        cf = TwoOptSearch(dataStructure, node_list, messages, messageCount, cf, alphaCoef, beta);
        if (IsNewCFBetter(cf, bestCf, alphaCoef, beta)) {
            for (i = 0; i < dataStructure->nodeCount; i++) {
                if (node_list[i].channel == CHANNELB)
                    nodeChannelBest[i] = 0;
                else
                    nodeChannelBest[i] = 1;
            }
            bestCf.channelA = cf.channelA;
            bestCf.channelB = cf.channelB;
            bestCf.gateway = cf.gateway;
        }
    }
    for (i = 0; i < dataStructure->nodeCount; i++) {
        dataStructure->nodesChannel[i] = nodeChannelBest[i];
    }
    printf("/ %d, %d, %d\n", bestCf.channelA, bestCf.channelB, bestCf.gateway);
    delete[] node_list;
    delete[] messages;
    delete[] nodeChannelBest;
    return bestCf;
}

std::string ChannelsFitness::str() const {
    std::stringstream ss;
    ss << "A:" << this->channelA << " B:" << this->channelB << " GW:" << this->gateway << std::endl;
    return ss.str();
}
