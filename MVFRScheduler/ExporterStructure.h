//
// Created by jenik on 10/27/15.
//

#ifndef MVFRSCHEDULER_EXPORTERSTRUCTURE_H
#define MVFRSCHEDULER_EXPORTERSTRUCTURE_H

#include "MVFRScheduler.h"

typedef struct exporterStruct {
private:

public:
    std::vector<std::vector<Buffer>> buffers;
    std::vector<CFrame> *schedule;
    std::set<int> *nodeSlotAssignment;
    std::vector<bool> isKeyBuffer;
    std::vector<bool> isNodeForcedToHaveKeySlot;

} ExporterStruct;

void getExporterStruct(ExporterStruct &exporterStr, const DataStructure *dataStructure, const int &variantID);

#endif //MVFRSCHEDULER_EXPORTERSTRUCTURE_H
