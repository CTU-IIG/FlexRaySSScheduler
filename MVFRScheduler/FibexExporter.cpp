//
// Created by jenik on 10/27/15.
//

#include "MVFRScheduler.h"

int FibexExporter(DataStructure *dataStructure) {
    int variantId = (static_cast<ExporterParameters *>(dataStructure->schedulerParameters))->variantID;
    ExporterStruct exporterStr;
    moveOriginalScheduleToCurrentSchedule(dataStructure);
    getExporterStruct(exporterStr, dataStructure, variantId);

    std::fstream fout;
    if (strcmp(dataStructure->outputFile, "-") != 0) {
        fout.open(dataStructure->outputFile, std::fstream::out | std::fstream::trunc);
        FibexExporterWrite(dataStructure, fout, exporterStr, variantId);
        fout.close();
    }
    else {
        FibexExporterWrite(dataStructure, std::cout, exporterStr, variantId);
    }
    return SUCCESS;
}

int FibexExporterWrite(const DataStructure *dataStructure, std::ostream &sout, const ExporterStruct &exporterStr,
                       const int variantId) {
    sout << XMLHEADER;
    sout << FIBEXHEADER;
    FibexPrintProject(sout, 1, variantId);
    FibexPrintElements(dataStructure, sout, 1, exporterStr, variantId);
    FibexPrintProcessingInformation(dataStructure, sout, 1);
    sout << "</fx:FIBEX>" << std::endl;
    return SUCCESS;
}

void FibexPrintElements(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                        const ExporterStruct &exporterStr, const int variantId) {
    sout << IndentSpaces(indent) << "<fx:ELEMENTS>" << std::endl;
    FibexPrintClusters(dataStructure, sout, indent + 1);
    FibexPrintChannels(dataStructure, sout, indent + 1, exporterStr);
    FibexPrintEcus(dataStructure, sout, indent + 1, exporterStr);
    FibexPrintPdus(dataStructure, sout, indent + 1, variantId, exporterStr);
    FibexPrintFrames(dataStructure, sout, indent + 1, exporterStr, variantId);
    FibexPrintSignals(dataStructure, sout, indent + 1, variantId);
    sout << IndentSpaces(indent) << "</fx:ELEMENTS>" << std::endl;
}

void FibexPrintClusters(const DataStructure *dataStructure, std::ostream &sout, const int indent) {
    sout << IndentSpaces(indent) << "<fx:CLUSTERS>" << std::endl;
    FibexPrintCluster(dataStructure, sout, indent + 1);
    sout << IndentSpaces(indent) << "</fx:CLUSTERS>" << std::endl;
}

void FibexPrintCluster(const DataStructure *dataStructure, std::ostream &sout, const int indent) {
    NetworkParametersCalculator nc(10000, dataStructure->slotLength, dataStructure->cycleLength,
                                   dataStructure->maxSlot);
    sout << IndentSpaces(indent) << "<fx:CLUSTER ID=\"Cluster_1\" xsi:type=\"flexray:CLUSTER-TYPE\">" << std::endl;
    sout << IndentSpaces(indent + 1) << "<ho:SHORT-NAME>Cluster_One</ho:SHORT-NAME>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:SPEED>10000000</fx:SPEED>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:IS-HIGH-LOW-BIT-ORDER>true</fx:IS-HIGH-LOW-BIT-ORDER>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:BIT-COUNTING-POLICY>SAWTOOTH</fx:BIT-COUNTING-POLICY>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:PROTOCOL xsi:type=\"flexray:PROTOCOL-TYPE\">FlexRay</fx:PROTOCOL>" <<
    std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:PROTOCOL-VERSION>2.1</fx:PROTOCOL-VERSION>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:CHANNEL-REFS>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<fx:CHANNEL-REF ID-REF=\"Channel_1\"/>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<fx:CHANNEL-REF ID-REF=\"Channel_2\"/>" << std::endl;
    sout << IndentSpaces(indent + 1) << "</fx:CHANNEL-REFS>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:MEDIUM>ELECTRICAL</fx:MEDIUM>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:COLD-START-ATTEMPTS>" << GCOLDSTARTATTEMPTS <<
    "</flexray:COLD-START-ATTEMPTS>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:ACTION-POINT-OFFSET>" << nc.getGdActionPointOffset() <<
    "</flexray:ACTION-POINT-OFFSET>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:DYNAMIC-SLOT-IDLE-PHASE>" << GDDYNAMICSLOTIDLEPHASE <<
    "</flexray:DYNAMIC-SLOT-IDLE-PHASE>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:MINISLOT>" << nc.getGdMiniSlot() << "</flexray:MINISLOT>" <<
    std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:MINISLOT-ACTION-POINT-OFFSET>" <<
    nc.getGdMinislotActionPointOffset() << "</flexray:MINISLOT-ACTION-POINT-OFFSET>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:N-I-T>" << nc.getGdNIT() << "</flexray:N-I-T>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:SAMPLE-CLOCK-PERIOD>0.012500</flexray:SAMPLE-CLOCK-PERIOD>" <<
    std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:STATIC-SLOT>" << nc.getGdStaticSlot() << "</flexray:STATIC-SLOT>" <<
    std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:SYMBOL-WINDOW>" << nc.getGdSymbolWindow() <<
    "</flexray:SYMBOL-WINDOW>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:T-S-S-TRANSMITTER>" << nc.getGdTSSTransmitter() <<
    "</flexray:T-S-S-TRANSMITTER>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:WAKE-UP>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:WAKE-UP-SYMBOL-RX-IDLE>" << GDWAKEUPSYMBOLRXIDLE <<
    "</flexray:WAKE-UP-SYMBOL-RX-IDLE>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:WAKE-UP-SYMBOL-RX-LOW>" << GDWAKEUPSYMBOLRXLOW <<
    "</flexray:WAKE-UP-SYMBOL-RX-LOW>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:WAKE-UP-SYMBOL-RX-WINDOW>" << GDWAKEUPSYMBOLRXWINDOW <<
    "</flexray:WAKE-UP-SYMBOL-RX-WINDOW>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:WAKE-UP-SYMBOL-TX-IDLE>" << GDWAKEUPSYMBOLTXIDLE <<
    "</flexray:WAKE-UP-SYMBOL-TX-IDLE>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:WAKE-UP-SYMBOL-TX-LOW>" << GDWAKEUPSYMBOLTXLOW <<
    "</flexray:WAKE-UP-SYMBOL-TX-LOW>" << std::endl;
    sout << IndentSpaces(indent + 1) << "</flexray:WAKE-UP>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:LISTEN-NOISE>" << GLISTENNOISE << "</flexray:LISTEN-NOISE>" <<
    std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:MACRO-PER-CYCLE>" << nc.getGdMacroPerCycle() <<
    "</flexray:MACRO-PER-CYCLE>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:MACROTICK>" << GDMACROTICK << "</flexray:MACROTICK>" << std::endl;
    sout << IndentSpaces(indent + 1) <<
    "<flexray:MAX-INITIALIZATION-ERROR>" << GDMAXINITIALIZATIONERROR << "</flexray:MAX-INITIALIZATION-ERROR>" <<
    std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:MAX-WITHOUT-CLOCK-CORRECTION-FATAL>" <<
    GMAXWITHOUTCLOCKCORRECTIONFATAL << "</flexray:MAX-WITHOUT-CLOCK-CORRECTION-FATAL>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:MAX-WITHOUT-CLOCK-CORRECTION-PASSIVE>" <<
    GMAXWITHOUTCLOCKCORRECTIONPASSIVE << "</flexray:MAX-WITHOUT-CLOCK-CORRECTION-PASSIVE>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:NETWORK-MANAGEMENT-VECTOR-LENGTH>" <<
    GNETWORKMANAGEMENTVECTORLENGTH << "</flexray:NETWORK-MANAGEMENT-VECTOR-LENGTH>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:NUMBER-OF-MINISLOTS>" << nc.getGNumberOfMinislots() <<
    "</flexray:NUMBER-OF-MINISLOTS>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:NUMBER-OF-STATIC-SLOTS>" << nc.getGNumberOfStaticSlots() <<
    "</flexray:NUMBER-OF-STATIC-SLOTS>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:OFFSET-CORRECTION-START>" << nc.getGOffsetCorrectionStart() <<
    "</flexray:OFFSET-CORRECTION-START>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:PAYLOAD-LENGTH-STATIC>" << nc.getGPayloadLengthStatic() <<
    "</flexray:PAYLOAD-LENGTH-STATIC>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:SYNC-NODE-MAX>" << GSYNCNODEMAX << "</flexray:SYNC-NODE-MAX>" <<
    std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:CAS-RX-LOW-MAX>" << GDCASRXLOWMAX << "</flexray:CAS-RX-LOW-MAX>" <<
    std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:BIT>" << ADBIT << "</flexray:BIT>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:CYCLE>" << nc.getGdCycle() << "</flexray:CYCLE>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<flexray:CLUSTER-DRIFT-DAMPING>" << PCLUSTERDRIFTDAMPING <<
    "</flexray:CLUSTER-DRIFT-DAMPING>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:CLUSTER>" << std::endl;
}


void FibexPrintProject(std::ostream &sout, const int indent, const int variantId) {
    sout << IndentSpaces(indent) << "<fx:PROJECT ID=\"Project_Variant_" << variantId << "\">" << std::endl;
    sout << IndentSpaces(indent) << "<ho:SHORT-NAME>Vehicle_variant_" << variantId << "</ho:SHORT-NAME>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:PROJECT>" << std::endl;
}

void FibexPrintPdus(const DataStructure *dataStructure, std::ostream &sout, const int indent, const int variantId,
                    const ExporterStruct &exporterStr) {
    sout << IndentSpaces(indent) << "<fx:PDUS>" << std::endl;
    for (int node = 0; node < dataStructure->nodeCount; ++node) {
        for (int bufferId = 0; bufferId < exporterStr.buffers[node].size(); ++bufferId) {
            FibexPrintPdu(dataStructure, sout, indent + 1, exporterStr, node, bufferId, variantId);
        }
    }
    sout << IndentSpaces(indent) << "</fx:PDUS>" << std::endl;
}

void FibexPrintPdu(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                   const ExporterStruct &exporterStr, const int node, const int bufferId, const int variantId) {
    const Buffer &buffer = exporterStr.buffers[node][bufferId];
    sout << IndentSpaces(indent) << "<fx:PDU ID=\"PDU_" << buffer.slotID + 1 << "_" << buffer.cycleID + 1 <<
    "\">" << std::endl;
    sout << IndentSpaces(indent + 1) << "<ho:SHORT-NAME>New_PDU_" << buffer.slotID + 1 << "_" << buffer.cycleID + 1 <<
    "</ho:SHORT-NAME>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:BYTE-LENGTH>" << dataStructure->slotLength / 8 <<
    "</fx:BYTE-LENGTH>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:PDU-TYPE>APPLICATION</fx:PDU-TYPE>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:SIGNAL-INSTANCES>" << std::endl;
    for (auto signalId :exporterStr.schedule[buffer.cycleID][buffer.slotID].signals) {
        if (dataStructure->modelVariants[variantId][signalId]) {
            FibexPrintSignalInstance(dataStructure, sout, indent + 2, buffer.slotID, buffer.cycleID, signalId);
        }
    }

    sout << IndentSpaces(indent + 1) << "</fx:SIGNAL-INSTANCES>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:PDU>" << std::endl;
}

void FibexPrintSignalInstance(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                              const int slotId, const int cycleId, const int signalId) {
    sout << IndentSpaces(indent) << "<fx:SIGNAL-INSTANCE ID=\"SignalInstance_" << slotId + 1 << "_" << cycleId + 1 <<
    "_" << signalId << "\">" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:BIT-POSITION>" <<
    (7 - dataStructure->signalInFrameOffsets[signalId] % 8) + (dataStructure->signalInFrameOffsets[signalId] / 8) * 8 <<
    "</fx:BIT-POSITION>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:IS-HIGH-LOW-BYTE-ORDER>true</fx:IS-HIGH-LOW-BYTE-ORDER>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:SIGNAL-REF ID-REF=\"Signal_" << signalId << "\"/>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:SIGNAL-INSTANCE>" << std::endl;
}

void FibexPrintFrames(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                      const ExporterStruct &exporterStr, const int variantId) {
    sout << IndentSpaces(indent) << "<fx:FRAMES>" << std::endl;
    for (int node = 0; node < dataStructure->nodeCount; ++node) {
        for (int bufferId = 0; bufferId < exporterStr.buffers[node].size(); ++bufferId) {
            FibexPrintFrame(dataStructure, sout, indent + 1, exporterStr, 0, node, bufferId, variantId);
            FibexPrintFrame(dataStructure, sout, indent + 1, exporterStr, 1, node, bufferId, variantId);
        }
    }
    sout << IndentSpaces(indent) << "</fx:FRAMES>" << std::endl;
}

void FibexPrintFrame(const DataStructure *dataStructure, std::ostream &sout, int indent,
                     const ExporterStruct &exporterStr, const int channel, const int node, const int bufferId,
                     const int variantId) {
    const Buffer &buffer = exporterStr.buffers[node][bufferId];
    sout << IndentSpaces(indent) << "<fx:FRAME ID=\"Frame_" << buffer.slotID + 1 << "_" << buffer.cycleID + 1 <<
    "_" << (channel ? "B" : "A") << "\">" << std::endl;
    sout << IndentSpaces(indent + 1) << "<ho:SHORT-NAME>New_Frame_" << buffer.slotID + 1 << "_" << buffer.cycleID + 1 <<
    "_" << (channel ? "B" : "A") << "</ho:SHORT-NAME>" << std::endl;
    sout << IndentSpaces(indent) << "<fx:BYTE-LENGTH>" << dataStructure->slotLength / 8 << "</fx:BYTE-LENGTH>" <<
    std::endl;
    sout << IndentSpaces(indent) << "<fx:FRAME-TYPE>APPLICATION</fx:FRAME-TYPE>" << std::endl;
    sout << IndentSpaces(indent) << "<fx:PDU-INSTANCES>" << std::endl;
    FibexPrintPduInstance(dataStructure, sout, indent + 1, exporterStr, channel, buffer.slotID, buffer.cycleID);
    sout << IndentSpaces(indent) << "</fx:PDU-INSTANCES>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:FRAME>" << std::endl;
}

void FibexPrintPduInstance(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                           const ExporterStruct &exporterStr, const int channel, const int slotId, const int cycleId) {
    sout << IndentSpaces(indent) << "<fx:PDU-INSTANCE ID=\"PDUInstance_" << slotId + 1 << "_" << cycleId + 1 << "_" <<
    (channel ? "B" : "A") << "\">" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:PDU-REF ID-REF=\"PDU_" << slotId + 1 << "_" << cycleId + 1 <<
    "\"/>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:BIT-POSITION>" << dataStructure->slotLength - 8 << "</fx:BIT-POSITION>" <<
    std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:IS-HIGH-LOW-BYTE-ORDER>true</fx:IS-HIGH-LOW-BYTE-ORDER>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:PDU-INSTANCE>" << std::endl;

}

void FibexPrintSignals(const DataStructure *dataStructure, std::ostream &sout, const int indent, const int variantId) {
    sout << IndentSpaces(indent) << "<fx:SIGNALS>" << std::endl;
    for (int signalId = 0; signalId < dataStructure->signalsCount; signalId++) {
        if (dataStructure->modelVariants[variantId][signalId]) {
            FibexPrintSignal(dataStructure, sout, indent + 1, signalId);
        }
    }
    sout << IndentSpaces(indent) << "</fx:SIGNALS>" << std::endl;
}

void FibexPrintSignal(const DataStructure *dataStructure, std::ostream &sout, const int indent, const int signalId) {
    sout << IndentSpaces(indent) << "<fx:SIGNAL ID=\"Signal_" << signalId << "\">" << std::endl;
    sout << IndentSpaces(indent + 1) << "<ho:SHORT-NAME>New_Signal_" << signalId << "</ho:SHORT-NAME>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:CODING-REF ID-REF=\"Coding_" << signalId << "\"/>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:SIGNAL>" << std::endl;

}

void FibexPrintProcessingInformation(const DataStructure *dataStructure, std::ostream &sout, const int indent) {
    sout << IndentSpaces(indent) << "<fx:PROCESSING-INFORMATION>" << std::endl;
    FibexPrintCodings(dataStructure, sout, indent + 1);
    sout << IndentSpaces(indent) << "</fx:PROCESSING-INFORMATION>" << std::endl;

}

void FibexPrintCodings(const DataStructure *dataStructure, std::ostream &sout, const int indent) {
    sout << IndentSpaces(indent + 1) << "<fx:CODINGS>" << std::endl;
    for (int signalId = 0; signalId < dataStructure->signalsCount; signalId++) {
        FibexPrintCoding(dataStructure, sout, indent + 1, signalId);
    }
    sout << IndentSpaces(indent + 1) << "</fx:CODINGS>" << std::endl;
}

void FibexPrintCoding(const DataStructure *dataStructure, std::ostream &sout, const int indent, const int signalId) {
    sout << IndentSpaces(indent) << "<fx:CODING ID=\"Coding_" << signalId << "\">" << std::endl;
    sout << IndentSpaces(indent + 1) << "<ho:SHORT-NAME>New_Coding_" << signalId << "</ho:SHORT-NAME>" << std::endl;
    FibexPrintCodedType(dataStructure, sout, indent + 1, signalId);
    sout << IndentSpaces(indent) << "</fx:CODING>" << std::endl;
}

void FibexPrintCodedType(const DataStructure *dataStructure, std::ostream &sout, const int indent, const int signalId) {
    int uintcoding = 8;
    if (dataStructure->signalLengths[signalId] > 8) uintcoding = 16;
    if (dataStructure->signalLengths[signalId] > 16) uintcoding = 32;
    if (dataStructure->signalLengths[signalId] > 32) uintcoding = 64;
    sout << IndentSpaces(indent) <<
    "<ho:CODED-TYPE CATEGORY=\"STANDARD-LENGTH-TYPE\" ho:BASE-DATA-TYPE=\"A_UINT" << uintcoding << "\">" <<
    std::endl;
    sout << IndentSpaces(indent + 1) << "<ho:BIT-LENGTH>" << dataStructure->signalLengths[signalId] <<
    "</ho:BIT-LENGTH>" << std::endl;
    sout << IndentSpaces(indent) << "</ho:CODED-TYPE>" << std::endl;
}

void FibexPrintEcus(const DataStructure *dataStructure, std::ostream &sout, const int &indent,
                    const ExporterStruct &exporterStr) {
    sout << IndentSpaces(indent) << "<fx:ECUS>" << std::endl;
    for (int node = 0; node < dataStructure->nodeCount; node++) {
        FibexPrintEcu(dataStructure, sout, indent + 1, exporterStr, node);
    }
    sout << IndentSpaces(indent) << "</fx:ECUS>" << std::endl;
}

void FibexPrintEcu(const DataStructure *dataStructure, std::ostream &sout, const int &indent,
                   const ExporterStruct &exporterStr, const int &node) {
    sout << IndentSpaces(indent) << "<fx:ECU ID=\"ECU_" << node + 1 << "\">" << std::endl;
    sout << IndentSpaces(indent + 1) << "<ho:SHORT-NAME>" << node + 1 << "</ho:SHORT-NAME>" << std::endl;
    FibexPrintControllers(dataStructure, sout, indent + 1, exporterStr, node);
    FibexPrintConnectors(dataStructure, sout, indent + 1, exporterStr, node);
    sout << IndentSpaces(indent) << "</fx:ECU>" << std::endl;
}

void FibexPrintConnectors(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                          const ExporterStruct &exporterStr, const int &node) {
    sout << IndentSpaces(indent) << "<fx:CONNECTORS>" << std::endl;
    FibexPrintConnector(dataStructure, sout, indent + 1, exporterStr, node, 0);
    FibexPrintConnector(dataStructure, sout, indent + 1, exporterStr, node, 1);
    sout << IndentSpaces(indent) << "</fx:CONNECTORS>" << std::endl;

}

void FibexPrintConnector(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                         const ExporterStruct &exporterStr, const int &node, const int &channel) {
    sout << IndentSpaces(indent) << "<fx:CONNECTOR ID=\"Connector_" << node + 1 << "_" << channel + 1 <<
    "\" xsi:type=\"flexray:CONNECTOR-TYPE\">" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:CHANNEL-REF ID-REF=\"Channel_" << channel + 1 << "\"/>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:CONTROLLER-REF ID-REF=\"Controller_" << node + 1 << "\"/>" << std::endl;
    FibexPrintInputs(dataStructure, sout, indent + 1, exporterStr, node, channel);
    FibexPrintOutputs(dataStructure, sout, indent + 1, exporterStr, node, channel);
    sout << IndentSpaces(indent + 1) << "<flexray:WAKE-UP-CHANNEL>false</flexray:WAKE-UP-CHANNEL>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:CONNECTOR>" << std::endl;
}

void FibexPrintOutputs(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                       const ExporterStruct &exporterStr, const int node, const int channel) {
    if (exporterStr.buffers[node].size() > 0) {
        sout << IndentSpaces(indent) << "<fx:OUTPUTS>" << std::endl;
        for (int bufferId = 0; bufferId < exporterStr.buffers[node].size(); bufferId++) {
            FibexPrintOutputPort(dataStructure, sout, indent + 1, exporterStr, node, channel, bufferId);
        }
        sout << IndentSpaces(indent) << "</fx:OUTPUTS>" << std::endl;
    }
}

void FibexPrintOutputPort(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                          const ExporterStruct &exporterStr, const int node, const int channel, const int bufferId) {
    const Buffer &buffer = exporterStr.buffers[node][bufferId];
    sout << IndentSpaces(indent) << "<fx:OUTPUT-PORT ID=\"OutputPort_" << node + 1 << "_" << bufferId + 1 << "_" <<
    (channel ? "B" : "A") << "\">" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:FRAME-TRIGGERING-REF ID-REF=\"FrameTriggering_" << buffer.slotID + 1 <<
    "_" <<
    buffer.cycleID + 1 << "_" << (channel ? "B" : "A") << "\"/>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:COMPLETE-FRAME/>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:OUTPUT-PORT>" << std::endl;
}

void FibexPrintInputs(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                      const ExporterStruct &exporterStr, const int &node, const int &channel) {
    if (node == 0 || node == 1) {
        sout << IndentSpaces(indent) << "<fx:INPUTS>" << std::endl;
        if (node == 0) {
            for (int transmittingNode = 1; transmittingNode < dataStructure->nodeCount; ++transmittingNode) {
                for (int bufferId = 0; bufferId < exporterStr.buffers[transmittingNode].size(); bufferId++) {
                    FibexPrintInputPort(dataStructure, sout, indent + 1, exporterStr, transmittingNode, channel,
                                        bufferId);
                }
            }
        }
        else {
            for (int bufferId = 0; bufferId < exporterStr.buffers[0].size(); bufferId++) {
                FibexPrintInputPort(dataStructure, sout, indent + 1, exporterStr, 0, channel,
                                    bufferId);
            }
        }
        sout << IndentSpaces(indent) << "</fx:INPUTS>" << std::endl;
    }

    // TODO: Refactore Inputs
}

void FibexPrintInputPort(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                         const ExporterStruct &exporterStr, const int node, const int channel, const int bufferId) {
    const Buffer &buffer = exporterStr.buffers[node][bufferId];
    sout << IndentSpaces(indent) << "<fx:INPUT-PORT ID=\"InputPort_" << node + 1 << "_" << bufferId + 1 << "_" <<
    (channel ? "B" : "A") << "\">" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:FRAME-TRIGGERING-REF ID-REF=\"FrameTriggering_" << buffer.slotID + 1 <<
    "_" << buffer.cycleID + 1 << "_" << (channel ? "B" : "A") << "\"/>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:COMPLETE-FRAME/>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:INPUT-PORT>" << std::endl;

}

void FibexPrintControllers(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                           const ExporterStruct &exporterStr, const int &node) {
    NetworkParametersCalculator nc(10000, dataStructure->slotLength, dataStructure->cycleLength,
                                   dataStructure->maxSlot);
    sout << IndentSpaces(indent) << "<fx:CONTROLLERS>" << std::endl;
    sout << IndentSpaces(indent + 1) <<
    "<fx:CONTROLLER ID=\"Controller_" << node + 1 << "\" xsi:type=\"flexray:CONTROLLER-TYPE\">" << std::endl;
    sout << IndentSpaces(indent + 2) << "<ho:SHORT-NAME>" << node + 1 << "</ho:SHORT-NAME>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:KEY-SLOT-USAGE>" << std::endl;
    if (exporterStr.isKeyBuffer[node] || exporterStr.isNodeForcedToHaveKeySlot[node]) {
        sout << IndentSpaces(indent + 3) << "<flexray:STARTUP-SYNC>" << exporterStr.buffers[node][0].slotID + 1 <<
        "</flexray:STARTUP-SYNC>" << std::endl;
    }
    else {
        sout << IndentSpaces(indent + 3) << "<flexray:NONE/>" << std::endl;
    }
    sout << IndentSpaces(indent + 2) << "</flexray:KEY-SLOT-USAGE>" << std::endl;
    sout << IndentSpaces(indent + 2) <<
    "<flexray:MAX-DYNAMIC-PAYLOAD-LENGTH>127</flexray:MAX-DYNAMIC-PAYLOAD-LENGTH>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:CLUSTER-DRIFT-DAMPING>" << PCLUSTERDRIFTDAMPING <<
    "</flexray:CLUSTER-DRIFT-DAMPING>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:DECODING-CORRECTION>" << PDECODINGCORRECTION <<
    "</flexray:DECODING-CORRECTION>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:LISTEN-TIMEOUT>" << nc.getPdListenTimeout() <<
    "</flexray:LISTEN-TIMEOUT>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:MAX-DRIFT>" << nc.getPdDriftMax() << "</flexray:MAX-DRIFT>" <<
    std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:EXTERN-OFFSET-CORRECTION>" << PEXTERNOFFSETCORRECTION <<
    "</flexray:EXTERN-OFFSET-CORRECTION>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:EXTERN-RATE-CORRECTION>" << PEXTERNRATECORRECTION <<
    "</flexray:EXTERN-RATE-CORRECTION>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:LATEST-TX>" << PLATESTTX << "</flexray:LATEST-TX>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:MICRO-PER-CYCLE>" << nc.getGdMicroPerCycle() <<
    "</flexray:MICRO-PER-CYCLE>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:OFFSET-CORRECTION-OUT>" << nc.getPOffsetCorrectionOut() <<
    "</flexray:OFFSET-CORRECTION-OUT>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:RATE-CORRECTION-OUT>" << nc.getPRateCorrectionOut() <<
    "</flexray:RATE-CORRECTION-OUT>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:SAMPLES-PER-MICROTICK>2</flexray:SAMPLES-PER-MICROTICK>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:DELAY-COMPENSATION-A>" << PDELAYCOMPENSATIONA <<
    "</flexray:DELAY-COMPENSATION-A>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:DELAY-COMPENSATION-B>" << PDELAYCOMPENSATIONB <<
    "</flexray:DELAY-COMPENSATION-B>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:WAKE-UP-PATTERN>" << PWAKEUPPATTERN <<
    "</flexray:WAKE-UP-PATTERN>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:ALLOW-HALT-DUE-TO-CLOCK>" <<
    (PALLOWHALTDUETOCLOCK ? "true" : "false") <<
    "</flexray:ALLOW-HALT-DUE-TO-CLOCK>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:ALLOW-PASSIVE-TO-ACTIVE>" << PALLOWPASSIVETOACTIVE <<
    "</flexray:ALLOW-PASSIVE-TO-ACTIVE>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:ACCEPTED-STARTUP-RANGE>" << PDACCEPTEDSTARTUPRANGE <<
    "</flexray:ACCEPTED-STARTUP-RANGE>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:MACRO-INITIAL-OFFSET-A>" << nc.getPMacroInitialOffset() <<
    "</flexray:MACRO-INITIAL-OFFSET-A>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:MACRO-INITIAL-OFFSET-B>" << nc.getPMacroInitialOffset() <<
    "</flexray:MACRO-INITIAL-OFFSET-B>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:MICRO-INITIAL-OFFSET-A>" << nc.getPMicroInitialOffset() <<
    "</flexray:MICRO-INITIAL-OFFSET-A>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:MICRO-INITIAL-OFFSET-B>" << nc.getPMicroInitialOffset() <<
    "</flexray:MICRO-INITIAL-OFFSET-B>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:SINGLE-SLOT-ENABLED>" << (PSINGLESLOTENABLED ? "true" : "false") <<
    "</flexray:SINGLE-SLOT-ENABLED>" << std::endl;
    sout << IndentSpaces(indent + 2) << "<flexray:MICROTICK>0.025</flexray:MICROTICK>" << std::endl;
    sout << IndentSpaces(indent + 1) << "</fx:CONTROLLER>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:CONTROLLERS>" << std::endl;
}

void FibexPrintChannels(const DataStructure *dataStructure, std::ostream &sout, int indent,
                        const ExporterStruct &exporterStr) {
    sout << IndentSpaces(indent) << "<fx:CHANNELS>" << std::endl;
    FibexPrintChannel(dataStructure, sout, 0, indent + 1, exporterStr);
    FibexPrintChannel(dataStructure, sout, 1, indent + 1, exporterStr);
    sout << IndentSpaces(indent) << "</fx:CHANNELS>" << std::endl;
}

void FibexPrintChannel(const DataStructure *dataStructure, std::ostream &sout, const int &channel, const int &indent,
                       const ExporterStruct &exporterStr) {
    sout << IndentSpaces(indent) << "<fx:CHANNEL ID=\"Channel_" << channel + 1 <<
    "\" xsi:type=\"flexray:CHANNEL-TYPE\">" << std::endl;

    sout << IndentSpaces(indent + 1) << "<ho:SHORT-NAME>Channel_" << ((channel == 0) ? "A" : "B") <<
    "</ho:SHORT-NAME>" << std::endl;

    FibexPrintFrameTriggerings(dataStructure, sout, channel, indent + 1, exporterStr);
    sout << IndentSpaces(indent + 1) << "<flexray:FLEXRAY-CHANNEL-NAME>" << (channel ? "B" : "A") <<
    "</flexray:FLEXRAY-CHANNEL-NAME>" << std::endl;

    sout << IndentSpaces(indent) << "</fx:CHANNEL>" << std::endl;
}

void FibexPrintFrameTriggerings(const DataStructure *dataStructure, std::ostream &sout, const int &channel,
                                const int &indent,
                                const ExporterStruct &exporterStr) {
    sout << IndentSpaces(indent) << "<fx:FRAME-TRIGGERINGS>" << std::endl;
    for (int i = 0; i < dataStructure->nodeCount; i++) {
        for (int j = 0; j < exporterStr.buffers[i].size(); j++) {
            FibexPrintFrameTriggering(sout, indent + 1, exporterStr, i, j, channel);
        }
    }
    sout << IndentSpaces(indent) << "</fx:FRAME-TRIGGERINGS>" << std::endl;
}

void FibexPrintFrameTriggering(std::ostream &sout, const int indent, const ExporterStruct &exporterStr, const int &node,
                               const int &bufferId, const int channel) {
    const Buffer &buffer = exporterStr.buffers[node][bufferId];
    sout << IndentSpaces(indent) << "<fx:FRAME-TRIGGERING ID=\"FrameTriggering_" << buffer.slotID + 1 << "_" <<
    buffer.cycleID + 1 << "_" << (channel ? "B" : "A") << "\">" << std::endl;
    FibexPrintTimings(sout, indent + 1, exporterStr, node, bufferId);
    sout << IndentSpaces(indent + 1) << "<fx:FRAME-REF ID-REF=\"Frame_" << buffer.slotID + 1 << "_" <<
    buffer.cycleID + 1 << "_" << (channel ? "B" : "A") << "\"/>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:FRAME-TRIGGERING>" << std::endl;
}

void FibexPrintTimings(std::ostream &sout, const int indent, const ExporterStruct &exporterStr, const int &node,
                       const int &bufferId) {
    sout << IndentSpaces(indent) << "<fx:TIMINGS>" << std::endl;
    FibexPrintAbsolutelyScheduledTiming(sout, indent + 1, exporterStr, node, bufferId);
    sout << IndentSpaces(indent) << "</fx:TIMINGS>" << std::endl;
}

void FibexPrintAbsolutelyScheduledTiming(std::ostream &sout, const int indent, const ExporterStruct &exporterStr,
                                         const int &node, const int &bufferId) {
    sout << IndentSpaces(indent) << "<fx:ABSOLUTELY-SCHEDULED-TIMING>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:SLOT-ID xsi:type=\"flexray:SLOT-ID-TYPE\">" <<
    exporterStr.buffers[node][bufferId].slotID + 1 << "</fx:SLOT-ID>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:BASE-CYCLE xsi:type=\"flexray:CYCLE-COUNTER-TYPE\">" <<
    exporterStr.buffers[node][bufferId].cycleID << "</fx:BASE-CYCLE>" << std::endl;
    sout << IndentSpaces(indent + 1) << "<fx:CYCLE-REPETITION xsi:type=\"flexray:CYCLE-REPETITION-TYPE\">" <<
    exporterStr.buffers[node][bufferId].period << "</fx:CYCLE-REPETITION>" << std::endl;
    sout << IndentSpaces(indent) << "</fx:ABSOLUTELY-SCHEDULED-TIMING>" << std::endl;
}

std::string IndentSpaces(const int &indent) {
    std::stringstream ssResult;
    for (int i = 0; i < 2 * indent; i++)
        ssResult << " ";
    return ssResult.str();
}
