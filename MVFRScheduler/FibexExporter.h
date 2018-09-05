//
// Created by jenik on 10/27/15.
//

#ifndef MVFRSCHEDULER_FIBEXEXPORTER_H
#define MVFRSCHEDULER_FIBEXEXPORTER_H
#define FIBEXINDENT 2

int FibexExporter(DataStructure *dataStructure);

int FibexExporterWrite(const DataStructure *dataStructure, std::ostream &sout, const ExporterStruct &exporterStr,
                       const int variantId);

void FibexPrintChannels(const DataStructure *dataStructure, std::ostream &sout, int indent,
                        const ExporterStruct &exporterStr);

std::string IndentSpaces(const int &indent);

void FibexPrintChannel(const DataStructure *dataStructure, std::ostream &sout, const int &channel, const int &indent,
                       const ExporterStruct &exporterStr);

void FibexPrintFrameTriggerings(const DataStructure *dataStructure, std::ostream &sout, const int &channel,
                                const int &indent,
                                const ExporterStruct &exporterStr);

void FibexPrintFrameTriggering(std::ostream &sout, const int indent, const ExporterStruct &exporterStr, const int &node,
                               const int &bufferId, const int channel);

void FibexPrintTimings(std::ostream &sout, const int indent, const ExporterStruct &exporterStr, const int &node,
                       const int &bufferId);

void FibexPrintAbsolutelyScheduledTiming(std::ostream &sout, const int indent, const ExporterStruct &exporterStr,
                                         const int &node, const int &bufferId);

void FibexPrintEcus(const DataStructure *dataStructure, std::ostream &sout, const int &indent,
                    const ExporterStruct &exporterStr);

void FibexPrintEcu(const DataStructure *dataStructure, std::ostream &sout, const int &indent,
                   const ExporterStruct &exporterStr, const int &node);

void FibexPrintControllers(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                           const ExporterStruct &exporterStr, const int &node);

void FibexPrintConnectors(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                          const ExporterStruct &exporterStr, const int &node);

void FibexPrintConnector(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                         const ExporterStruct &exporterStr, const int &node, const int &channel);

void FibexPrintInputs(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                      const ExporterStruct &exporterStr, const int &node, const int &channel);

void FibexPrintOutputs(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                       const ExporterStruct &exporterStr, const int node, const int channel);

void FibexPrintOutputPort(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                          const ExporterStruct &exporterStr, const int node, const int channel, const int bufferId);

void FibexPrintProcessingInformation(const DataStructure *dataStructure, std::ostream &sout, const int indent);

void FibexPrintCodings(const DataStructure *dataStructure, std::ostream &sout, const int indent);

void FibexPrintCoding(const DataStructure *dataStructure, std::ostream &sout, const int indent, const int signalId);

void FibexPrintCodedType(const DataStructure *dataStructure, std::ostream &sout, const int indent, const int signalId);

void FibexPrintSignals(const DataStructure *dataStructure, std::ostream &sout, const int indent, const int variantId);

void FibexPrintSignal(const DataStructure *dataStructure, std::ostream &sout, const int indent, const int signalId);

void FibexPrintFrames(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                      const ExporterStruct &exporterStr, const int variantId);

void FibexPrintFrame(const DataStructure *dataStructure, std::ostream &sout, int indent,
                     const ExporterStruct &exporterStr, const int channel, const int node, const int bufferId,
                     const int variantId);

void FibexPrintPduInstance(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                           const ExporterStruct &exporterStr, const int channel, const int slotId, const int cycleId);

void FibexPrintPdus(const DataStructure *dataStructure, std::ostream &sout, const int indent, const int variantId,
                    const ExporterStruct &exporterStr);

void FibexPrintPdu(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                   const ExporterStruct &exporterStr, const int node, const int bufferId, const int variantId);

void FibexPrintProject(std::ostream &sout, const int indent, const int variantId);

void FibexPrintElements(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                        const ExporterStruct &exporterStr, const int variantId);

void FibexPrintClusters(const DataStructure *dataStructure, std::ostream &sout, const int indent);

void FibexPrintCluster(const DataStructure *dataStructure, std::ostream &sout, const int indent);

void FibexPrintSignalInstance(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                              const int slotId, const int cycleId, const int signalId);

void FibexPrintInputPort(const DataStructure *dataStructure, std::ostream &sout, const int indent,
                         const ExporterStruct &exporterStr, const int node, const int channel, const int bufferId);

#define XMLHEADER \
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"

#define FIBEXHEADER \
"<fx:FIBEX VERSION=\"3.1.0\" xmlns:all=\"http://www.asam.net/xml/fbx/all\" xmlns:can=\"http://www.asam.net/xml/fbx/can\" xmlns:flexray=\"http://www.asam.net/xml/fbx/flexray\" xmlns:fx=\"http://www.asam.net/xml/fbx\" xmlns:ho=\"http://www.asam.net/xml\" xmlns:lin=\"http://www.asam.net/xml/fbx/lin\" xmlns:vi=\"http://www.vector-informatik.com/xml/fbx/vector-flexray\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.asam.net/xml/fbx/all http://www.asam.net/xml/fbx/3_1_0/xsd/fibex4multiplatform.xsd\">\n"

#endif //MVFRSCHEDULER_FIBEXEXPORTER_H