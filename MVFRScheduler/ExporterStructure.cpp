//
// Created by jenik on 10/27/15.
//
#include "MVFRScheduler.h"

void getExporterStruct(ExporterStruct &exporterStr, const DataStructure *dataStructure, const int &variantID) {
    int i, j, k;
    exporterStr.nodeSlotAssignment = new std::set<int>[dataStructure->nodeCount];
    exporterStr.schedule = new std::vector<CFrame>[dataStructure->hyperPeriod];
    exporterStr.isKeyBuffer.resize(dataStructure->nodeCount, false);
    exporterStr.isNodeForcedToHaveKeySlot.resize(dataStructure->nodeCount, false);
    for (i = 0; i < dataStructure->hyperPeriod; i++)
        exporterStr.schedule[i].resize(dataStructure->maxSlot);
    for (i = 0; i < dataStructure->signalsCount; i++) {
        for (j = 0; j < dataStructure->hyperPeriod / dataStructure->signalPeriods[i]; j++) {
            exporterStr.schedule[dataStructure->signalStartCycle[i] + j * dataStructure->signalPeriods[i]]
            [dataStructure->signalSlot[i]].signals.push_back(i);
        }
    }

    getNodeToSlotAssignment(dataStructure, exporterStr.nodeSlotAssignment);
    exporterStr.buffers.resize(dataStructure->nodeCount, std::vector<Buffer>()); // Generating buffers
    int numberOfKeyBuffers = 0;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        for (std::set<int>::iterator it = exporterStr.nodeSlotAssignment[i].begin();
             it != exporterStr.nodeSlotAssignment[i].end(); ++it) {
            std::set<int> cycleSet;
            cycleSet.clear();
            for (j = 0; j < dataStructure->hyperPeriod; j++) {
                if (cycleSet.find(j) == cycleSet.end()) {
                    if (exporterStr.schedule[j][*it].signals.size() > 0) {
                        Buffer newBuffer = createBuffer(dataStructure, exporterStr.schedule, variantID, *it, j);
                        if (newBuffer.period == 1) {
                            exporterStr.isKeyBuffer[i] = true;
                            numberOfKeyBuffers++;
                        }
                        exporterStr.buffers[i].push_back(newBuffer);
                        for (k = j; k < dataStructure->hyperPeriod; k = k + newBuffer.period) {
                            cycleSet.insert(k);
                        }
                    }
                }
            }
        }
        stable_sort(exporterStr.buffers[i].begin(), exporterStr.buffers[i].end(), [](Buffer a, Buffer b) {
            return a.period < b.period;
        });
    }
    for (i = 0; i < dataStructure->nodeCount; i++) {
        if (numberOfKeyBuffers >= 2) break;
        if (exporterStr.isKeyBuffer[i] == false && numberOfKeyBuffers < 2) {
            exporterStr.isNodeForcedToHaveKeySlot[i] = true;
            numberOfKeyBuffers++;
        }
    }
}