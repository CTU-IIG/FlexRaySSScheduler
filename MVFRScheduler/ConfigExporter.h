/// \file ConfigExporter.h Header file for the ConfigExplorer.cpp
#ifndef CONFIGEXPORTER_H_
#define CONFIGEXPORTER_H_

#define GCOLDSTARTATTEMPTS 8 // 2
#define GLISTENNOISE 2 // 15
#define GDMAXINITIALIZATIONERROR 2.648000
#define GMAXWITHOUTCLOCKCORRECTIONFATAL 2 // 15
#define GMAXWITHOUTCLOCKCORRECTIONPASSIVE 2 // 15
#define GNETWORKMANAGEMENTVECTORLENGTH 0 // 12
#define GNUMBEROFMINISLOTS 0
#define GOFFSETCORRECTIONSTART 2788
#define GSYNCNODEMAX 15
#define GDACTIONPOINTOFFSET 4
#define GDCASRXLOWMAX 87 // 67
#define GDDYNAMICSLOTIDLEPHASE 1
#define GDMINISLOT 4
#define GDMINISLOTACTIONPOINTOFFSET 2
#define GDNIT 2787
#define GDSAMPLECLOCKPERIOD 0 //0.012500
#define GDTSSTRANSMITTER 10
#define GDWAKEUPSYMBOLRXIDLE 59 // 18
#define GDWAKEUPSYMBOLRXLOW 50 // 18
#define GDWAKEUPSYMBOLRXWINDOW 301 // 76
#define GDWAKEUPSYMBOLTXIDLE 180
#define GDWAKEUPSYMBOLTXLOW 60

#define PALLOWHALTDUETOCLOCK 1 // 0
#define PALLOWPASSIVETOACTIVE 0 // 15
#define PCHANNELS 2
#define PCLUSTERDRIFTDAMPING 2 // 1
#define PDELAYCOMPENSATIONA 1 // 3
#define PDELAYCOMPENSATIONB 1 // 3
#define PEXTERNOFFSETCORRECTION 0
#define PEXTERNRATECORRECTION 0
//#define PKEYSLOTUSEDFORSTARTUP 1
//#define PKEYSLOTUSEDFORSYNC 1
#define PLATESTTX 249 // 269
#define PMACROINITIALOFFSETA 6
#define PMACROINITIALOFFSETB 6
#define PMICROINITIALOFFSETA 24
#define PMICROINITIALOFFSETB 24
#define PRATECORRECTIONOUT 205
#define POFFSETCORRECTIONOUT 337
#define PSAMPLESPERMICROTICK GDSAMPLECLOCKPERIOD
#define PSINGLESLOTENABLED 0 // 1
#define PWAKEUPCHANNEL 0
#define PWAKEUPPATTERN 33 // 2
#define PDACCEPTEDSTARTUPRANGE 212 // 129
#define PDLISTENTIMEOUT 224674
#define PDMAXDRIFT 337
#define PDECODINGCORRECTION 48 // 51

#define SYNCFRAMEPAYLOADMULTIPLEXENABLED 0
#define SECUREBUFFERS 0


#define CDFSS 1
#define CDFES 2
#define CCHANNELIDLEDELIMITER 11
#define ADBITMAX 0.10015
#define ADBIT 0.100000
#define ADPROPAGATIONDELAYMAX 2.638
#define AASSUMEDPRECISION 8.582
#define ADMAXIDLEDETECTIONDELAYAFTERHIGH 1.77515
#define GDMACROTICK 1
#define GCLOCKDEVIATIONMAX 0.0015
#define AMICROPERMACRONOM 40

#include <vector>

int ConfigExporter(DataStructure *dataStructure);

Buffer createBuffer(const DataStructure *dataStructure, std::vector<CFrame> *schedule, const int &variantID,
                    const int &slotID, const int &cycleID);

void createSetOfFrameSignals(const DataStructure *dataStructure, const std::vector<CFrame> *schedule,
                             const int &variantID, const int &slotID, const int &cycleID, std::set<int> &set);

int ComputeGdStaticSlot(DataStructure *dataStructure);

int ComputeAFrameLengthStatic(DataStructure *dataStructure);

std::string generateFrame(const DataStructure *dataStructure, const std::vector<CFrame> *schedule, const Buffer &buffer,
                          const int cppStyle);

void setBitOfBitArray(unsigned char *frame, const int &bit);

void writeClusterConfiguration(FILE *outputFile, NetworkParametersCalculator &nc, int nodeId);

void writeNodeConfiguration(FILE *outputFile, NetworkParametersCalculator &nc, int nodeId,
                            const std::vector<bool, std::allocator<bool>> &isKeyBuffer,
                            const std::vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot);

void writeBufferConfiguration(const DataStructure *dataStructure, FILE *outputFile, int nodeId,
                              const std::vector<std::vector<Buffer>> &buffers,
                              const std::vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot);

void writeStartCommunication(FILE *outputFile);

void writeDataTransmission(const DataStructure *dataStructure, int nodeId, const std::vector<CFrame> *schedule,
                           const std::vector<std::vector<Buffer>> &buffers,
                           const std::vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot,
                           FILE *outputFile);

void writeClusterConfigurationCpp(FILE *outputFile, NetworkParametersCalculator &nc, int nodeId);

void writeNodeConfigurationCpp(FILE *outputFile, NetworkParametersCalculator &nc, int nodeId,
                               const std::vector<bool, std::allocator<bool>> &isKeyBuffer,
                               const std::vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot);

void writeBufferConfigurationCpp(const DataStructure *dataStructure, FILE *outputFile, int nodeId,
                                 const std::vector<std::vector<Buffer>> &buffers,
                                 const std::vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot);

void writeStartCommunicationCpp(FILE *outputFile, const int boardId);

void writeDataTransmissionCpp(const DataStructure *dataStructure, int nodeId, const std::vector<CFrame> *schedule,
                              const std::vector<std::vector<Buffer>> &buffers,
                              const std::vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot,
                              FILE *outputFile);

void writeHeaderCpp(FILE *outputFile);

void writeRppTermination(FILE *outputFile);

void writeHeaderFileCpp(FILE *headerFile);

void ExporterCpp(const DataStructure *dataStructure, const std::vector<CFrame> *schedule,
                 const std::vector<bool> &isKeyBuffer, const std::vector<std::vector<Buffer>> &buffers,
                 const std::vector<bool> &isNodeForcedToHaveKeySlot, NetworkParametersCalculator &nc);

void ExporterRppConfig(const DataStructure *dataStructure, const std::vector<CFrame> *schedule,
                       const std::vector<bool> &isKeyBuffer, const std::vector<std::vector<Buffer>> &buffers,
                       const std::vector<bool> &isNodeForcedToHaveKeySlot, NetworkParametersCalculator &nc);

#endif