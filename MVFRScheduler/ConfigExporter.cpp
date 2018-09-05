/// \file ConfigExporter.cpp Code for exporting schedule to the FlexRay Configuration
#include "MVFRScheduler.h"

using namespace std;

int ConfigExporter(DataStructure *dataStructure) {
    int i, j, k, tmp;

    ExporterParameters &exporterParameters = *(static_cast<ExporterParameters *>(dataStructure->schedulerParameters));
    int variantID = exporterParameters.variantID;

    int gNumberOfStaticSlots;
    int gPayloadLengthStatic = (dataStructure->slotLength + 15) / 16;
    int gdStaticSlot = ComputeGdStaticSlot(dataStructure);
    int gMacroPerCycle = dataStructure->cycleLength * 1000;
    int pMicroPerCycle = gMacroPerCycle * AMICROPERMACRONOM;

    if (variantID < 0 || variantID >= dataStructure->variantCount) {
        error("Variant identifier is out of the range!");
        return FAIL;
    }

    if (dataStructure->algorithm == EXPORT) {
        moveOriginalScheduleToCurrentSchedule(dataStructure);
    }

    gNumberOfStaticSlots = dataStructure->maxSlot;

    if (dataStructure->maxSlot == -1)
        return 0;
    std::vector<CFrame> *schedule = new std::vector<CFrame>[dataStructure->hyperPeriod];
    for (i = 0; i < dataStructure->hyperPeriod; i++)
        schedule[i].resize(dataStructure->maxSlot);
    std::set<int> *nodeSlotAssignment = new std::set<int>[dataStructure->nodeCount];

    for (i = 0; i < dataStructure->signalsCount; i++) {
        for (j = 0; j < dataStructure->hyperPeriod / dataStructure->signalPeriods[i]; j++) {
            schedule[dataStructure->signalStartCycle[i] + j * dataStructure->signalPeriods[i]]
            [dataStructure->signalSlot[i]].signals.push_back(i);
        }
    }

    getNodeToSlotAssignment(dataStructure, nodeSlotAssignment);
    std::vector<bool> isKeyBuffer(dataStructure->nodeCount, false);

    std::vector<std::vector<Buffer>> buffers(dataStructure->nodeCount, std::vector<Buffer>()); // Generating buffers
    int numberOfKeyBuffers = 0;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        for (std::set<int>::iterator it = nodeSlotAssignment[i].begin(); it != nodeSlotAssignment[i].end(); ++it) {
            std::set<int> cycleSet;
            cycleSet.clear();
            for (j = 0; j < dataStructure->hyperPeriod; j++) {
                if (cycleSet.find(j) == cycleSet.end()) {
                    if (schedule[j][*it].signals.size() > 0) {
                        Buffer newBuffer = createBuffer(dataStructure, schedule, variantID, *it, j);
                        if (newBuffer.period == 1) {
                            isKeyBuffer[i] = true;
                            numberOfKeyBuffers++;
                        }
                        buffers[i].push_back(newBuffer);
                        for (k = j; k < dataStructure->hyperPeriod; k = k + newBuffer.period) {
                            cycleSet.insert(k);
                        }
                    }
                }
            }
        }
        std::stable_sort(buffers[i].begin(), buffers[i].end(), [](Buffer a, Buffer b) {
            return a.period < b.period;
        });
    }

    std::vector<bool> isNodeForcedToHaveKeySlot(dataStructure->nodeCount, false);
    for (i = 0; i < dataStructure->nodeCount; i++) {
        if (numberOfKeyBuffers >= 2) break;
        if (isKeyBuffer[i] == false && numberOfKeyBuffers < 2) {
            isNodeForcedToHaveKeySlot[i] = true;
            numberOfKeyBuffers++;
        }
    }

    int numberOfAdditionalSlots = std::accumulate(isNodeForcedToHaveKeySlot.begin(), isNodeForcedToHaveKeySlot.end(),
                                                  0,
                                                  [](int sum, bool additional) { return additional ? sum + 1 : sum; });
    NetworkParametersCalculator nc = NetworkParametersCalculator(10000, dataStructure->slotLength,
                                                                 dataStructure->cycleLength,
                                                                 dataStructure->maxSlot + numberOfAdditionalSlots);
    if (dataStructure->appendToCSV != 0) {
        AppendToCSV(dataStructure, nc.getGNumberOfStaticSlots());
    }
    if (exporterParameters.exportType == ExporterEngineType::CPP_FILE) {
        ExporterCpp(dataStructure, schedule, isKeyBuffer, buffers, isNodeForcedToHaveKeySlot, nc);
    }
    else if (exporterParameters.exportType == ExporterEngineType::RPP_CONFIG) {
        ExporterRppConfig(dataStructure, schedule, isKeyBuffer, buffers, isNodeForcedToHaveKeySlot, nc);
    }

    delete[] schedule;
    delete[] nodeSlotAssignment;
    return SUCCESS;
}

void ExporterRppConfig(const DataStructure *dataStructure, const vector<CFrame> *schedule,
                       const vector<bool> &isKeyBuffer, const vector<vector<Buffer>> &buffers,
                       const vector<bool> &isNodeForcedToHaveKeySlot,
                       NetworkParametersCalculator &nc) {
    int i;
    FILE *outputFile = fopen(dataStructure->outputFile, "w");
    if (outputFile != NULL) {
        for (i = 0; i < dataStructure->nodeCount; i++) {
            writeClusterConfiguration(outputFile, nc, i);
            writeNodeConfiguration(outputFile, nc, i, isKeyBuffer, isNodeForcedToHaveKeySlot);
            writeBufferConfiguration(dataStructure, outputFile, i, buffers, isNodeForcedToHaveKeySlot);
            writeStartCommunication(outputFile);
            writeDataTransmission(dataStructure, i, schedule, buffers, isNodeForcedToHaveKeySlot, outputFile);
        }

        if (i + 1 < dataStructure->nodeCount) {
            fprintf(outputFile, "\n\n");
        }
    }
    fclose(outputFile);
}

void ExporterCpp(const DataStructure *dataStructure, const vector<CFrame> *schedule,
                 const vector<bool> &isKeyBuffer, const vector<vector<Buffer>> &buffers,
                 const vector<bool> &isNodeForcedToHaveKeySlot, NetworkParametersCalculator &nc) {
    int i;
    for (i = 0; i < dataStructure->nodeCount; i++) {
        stringstream ss;
        ss << dataStructure->outputFile;
        ss << i << ".c";
        FILE *outputFile = fopen(ss.str().c_str(), "w");
        if (outputFile != NULL) {
            writeHeaderCpp(outputFile);
            writeClusterConfigurationCpp(outputFile, nc, 0);
            writeNodeConfigurationCpp(outputFile, nc, i, isKeyBuffer, isNodeForcedToHaveKeySlot);
            writeBufferConfigurationCpp(dataStructure, outputFile, i, buffers, isNodeForcedToHaveKeySlot);
            writeStartCommunicationCpp(outputFile, i);
            writeDataTransmissionCpp(dataStructure, i, schedule, buffers, isNodeForcedToHaveKeySlot, outputFile);
            writeRppTermination(outputFile);
        }
        fclose(outputFile);
    }
    stringstream ss;
    ss << dataStructure->outputFile << ".h";
    FILE *headerFile = fopen(ss.str().c_str(), "w");
    if (headerFile != NULL) {
        writeHeaderFileCpp(headerFile);
    }
    fclose(headerFile);
}

void writeHeaderFileCpp(FILE *headerFile) {
    fprintf(headerFile, "/*\n"
            " * File: fray_basic_demo_rpp.h\n"
            " *\n"
            " * Target selection: rpp.tlc\n"
            " * Embedded hardware selection: 32-bit Generic\n"
            " */\n"
            "\n"
            "#ifndef RTW_HEADER_fray_basic_demo_rpp_h_\n"
            "#define RTW_HEADER_fray_basic_demo_rpp_h_\n"
            "#ifndef fray_basic_demo_rpp_COMMON_INCLUDES_\n"
            "# define fray_basic_demo_rpp_COMMON_INCLUDES_\n"
            "#include <base.h>\n"
            "#include <rpp/sci.h>\n"
            "#include <stddef.h>\n"
            "#include <string.h>\n"
            "#include \"rtwtypes.h\"\n"
            "#include \"rpp/rpp.h\"\n"
            "#endif                                 /* fray_basic_demo_rpp_COMMON_INCLUDES_ */\n"
            "\n"
            "#include \"fray_basic_demo_rpp_types.h\"\n"
            "\n"
            "/* Macros for accessing real-time model data structure */\n"
            "#ifndef rtmGetErrorStatus\n"
            "# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)\n"
            "#endif\n"
            "\n"
            "#ifndef rtmSetErrorStatus\n"
            "# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))\n"
            "#endif\n"
            "\n"
            "#ifndef rtmGetStopRequested\n"
            "# define rtmGetStopRequested(rtm)      ((void*) 0)\n"
            "#endif\n"
            "\n"
            "/* Block signals (auto storage) */\n"
            "typedef struct {\n"
            "  uint8_T ConfigureRXbuffer_o2[6];     /* '<Root>/Configure RX buffer' */\n"
            "  uint8_T ConfigureRXbuffer_o3;        /* '<Root>/Configure RX buffer' */\n"
            "  boolean_T ConfigureFlexRaynode1;     /* '<Root>/Configure FlexRay node1' */\n"
            "  boolean_T ConfigureTXbuffer;         /* '<Root>/Configure TX buffer' */\n"
            "  boolean_T ConfigureRXbuffer_o1;      /* '<Root>/Configure RX buffer' */\n"
            "  boolean_T ConfigureRXbuffer_o4;      /* '<Root>/Configure RX buffer' */\n"
            "  boolean_T SerialSend;                /* '<Root>/Serial Send' */\n"
            "  boolean_T SerialSend1;               /* '<Root>/Serial Send1' */\n"
            "  boolean_T SerialSend2;               /* '<Root>/Serial Send2' */\n"
            "  boolean_T SerialSend3;               /* '<Root>/Serial Send3' */\n"
            "  boolean_T SerialSend4;               /* '<Root>/Serial Send4' */\n"
            "  boolean_T SerialSend5;               /* '<Root>/Serial Send5' */\n"
            "} BlockIO_fray_basic_demo_rpp;\n"
            "\n"
            "/* External outputs (root outports fed by signals with auto storage) */\n"
            "typedef struct {\n"
            "  boolean_T Out1;                      /* '<Root>/Out1' */\n"
            "} ExternalOutputs_fray_basic_demo;\n"
            "\n"
            "/* Parameters (auto storage) */\n"
            "struct Parameters_fray_basic_demo_rpp_ {\n"
            "  uint8_T Constant_Value[6];           /* Computed Parameter: Constant_Value\n"
            "                                        * Referenced by: '<Root>/Constant'\n"
            "                                        */\n"
            "};\n"
            "\n"
            "/* Real-time Model Data Structure */\n"
            "struct tag_RTM_fray_basic_demo_rpp {\n"
            "  const char_T *errorStatus;\n"
            "};\n"
            "\n"
            "/* Block parameters (auto storage) */\n"
            "extern Parameters_fray_basic_demo_rpp fray_basic_demo_rpp_P;\n"
            "\n"
            "/* Block signals (auto storage) */\n"
            "extern BlockIO_fray_basic_demo_rpp fray_basic_demo_rpp_B;\n"
            "\n"
            "/* External outputs (root outports fed by signals with auto storage) */\n"
            "extern ExternalOutputs_fray_basic_demo fray_basic_demo_rpp_Y;\n"
            "\n"
            "/* Model entry point functions */\n"
            "extern void fray_basic_demo_rpp_initialize(void);\n"
            "extern void fray_basic_demo_rpp_step(void);\n"
            "extern void fray_basic_demo_rpp_terminate(void);\n"
            "extern void fray_basic_demo_rpp_initialize_transmission(void);\n"
            "\n"
            "/* Real-time Model object */\n"
            "extern RT_MODEL_fray_basic_demo_rpp *const fray_basic_demo_rpp_M;\n"
            "\n"
            "/*-\n"
            " * The generated code includes comments that allow you to trace directly\n"
            " * back to the appropriate location in the model.  The basic format\n"
            " * is <system>/block_name, where system is the system number (uniquely\n"
            " * assigned by Simulink) and block_name is the name of the block.\n"
            " *\n"
            " * Use the MATLAB hilite_system command to trace the generated code back\n"
            " * to the model.  For example,\n"
            " *\n"
            " * hilite_system('<S3>')    - opens system 3\n"
            " * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3\n"
            " *\n"
            " * Here is the system hierarchy for this model\n"
            " *\n"
            " * '<Root>' : 'fray_basic_demo_rpp'\n"
            " */\n"
            "#endif                                 /* RTW_HEADER_fray_basic_demo_rpp_h_ */\n");
}

void writeRppTermination(FILE *outputFile) {
    fprintf(outputFile, "/* Model step function */\n"
            "void fray_basic_demo_rpp_step(void)\n"
            "{\n"
            "    int8_t retVal = ERR_PARAM_NO_ERROR;\n"
            "    Fr_POCStatusType status;\n"
            "    uint32_t error = ERR_PARAM_NO_ERROR;\n"
            "    char *StateStrings[]  = {\n"
            "        \"CONFIG\", \"DEFAULT_CONFIG\", \"HALT\", \"NORMAL_ACTIVE\",\n"
            "        \"NORMAL_PASSIVE\", \"READY\", \"STARTUP\", \"LOOPBACK\",\n"
            "        \"MONITOR\", \"WAKEUP\"\n"
            "    };\n"
            "\n");
    fprintf(outputFile, "    retVal = rpp_fr_get_poc_status(0, &status);\n");
    fprintf(outputFile, "    rpp_sci_printf(\"State: %%s\\n\", StateStrings[status.State]);\n");
    fprintf(outputFile, "    if (retVal == SUCCESS && status.State == 2) {\n");
    fprintf(outputFile,
            "        rpp_sci_printf(\"Connection halted! \\r\\nTrying to reestablish connection!\\r\\n\");\n"
                    "        retVal = rpp_fr_abort_communication(0);\n"
                    "        if (retVal == SUCCESS){\n"
                    "            rpp_sci_printf(\"FlexRay node communication stopped.\\r\\n\");\n");
    fprintf(outputFile, "            retVal = rpp_fr_init_controller(0, &error);\n"
            "            if (retVal == SUCCESS) {\n"
            "                rpp_sci_printf(\"FlexRay controller reinitialized. \\r\\nStarting new communication!\\r\\n\");\n"
            "                retVal = rpp_fr_start_communication(0, &error);\n"
            "                if (retVal == SUCCESS){\n"
            "                    fray_basic_demo_rpp_initialize_transmission();\n"
            "                    rpp_sci_printf(\"FlexRay communication is running.\\r\\n\");\n"
            "                }\n"
            "                else {\n");
    fprintf(outputFile, "                    if (error & FR_STARTUP_ERR_SW_STUP_FOLLOW)\n"
            "                        rpp_sci_printf(\"Can not switch POC to RUN state.\\r\\n\");\n"
            "                    else if (error & FR_STARTUP_ERR_CSINH_DIS)\n"
            "                        rpp_sci_printf(\"Cold start inhibit disabled error.\\r\\n\");\n"
            "                    else if (error & FR_STARTUP_ERR_SW_STUP_READY)\n"
            "                        rpp_sci_printf(\"Can not switch back to READY from STARTUP.\\r\\n\");\n"
            "                    else if (error & FR_STARTUP_ERR_SW_STUP_AS_NCOLD)\n"
            "                        rpp_sci_printf(\"Can not switch to STARTUP as non-coldstarter.\\r\\n\");\n"
            "                    else\n"
            "                        rpp_sci_printf(\"General error.\\r\\n\");\n"
            "                }\n"
            "                return;\n"
            "            }\n");
    fprintf(outputFile, "            else {\n"
            "                rpp_sci_printf(\"FlexRay needs to be configured before initialization.\\r\\n\");\n"
            "                return;\n"
            "            }\n"
            "        }\n"
            "        else{\n"
            "            rpp_sci_printf(\"General error.\\r\\n\");\n"
            "            return;\n"
            "        }\n"
            "    }"
            "  /* (no terminate code required) */\n"
            "}\n");

    fprintf(outputFile, "/* Model terminate function */\n"
            "void fray_basic_demo_rpp_terminate(void)\n"
            "{\n"
            "  /* (no terminate code required) */\n"
            "}\n");
}

void writeHeaderCpp(FILE *outputFile) {
    fprintf(outputFile, "/*\n"
            " * File: fray_basic_demo_rpp.c\n"
            " *\n"
            " * Target selection: rpp.tlc\n"
            " * Embedded hardware selection: 32-bit Generic\n"
            " */\n"
            "\n"
            "#include \"fray_basic_demo_rpp.h\"\n"
            "#include \"fray_basic_demo_rpp_private.h\"\n"
            "\n"
            "/* Block signals (auto storage) */\n"
            "BlockIO_fray_basic_demo_rpp fray_basic_demo_rpp_B;\n"
            "\n"
            "/* External outputs (root outports fed by signals with auto storage) */\n"
            "ExternalOutputs_fray_basic_demo fray_basic_demo_rpp_Y;\n"
            "\n"
            "/* Real-time model */\n"
            "RT_MODEL_fray_basic_demo_rpp fray_basic_demo_rpp_M_;\n"
            "RT_MODEL_fray_basic_demo_rpp *const fray_basic_demo_rpp_M =\n"
            "    &fray_basic_demo_rpp_M_;\n\n");
}

void writeDataTransmission(const DataStructure *dataStructure, int nodeId, const vector<CFrame> *schedule,
                           const vector<vector<Buffer>> &buffers,
                           const vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot,
                           FILE *outputFile) {
    for (int j = 0; j < buffers[nodeId].size(); j++) {
        int bufferID = (isNodeForcedToHaveKeySlot[nodeId]) ? j + 1 : j;
        fprintf(outputFile, "frbttransmit%d%s\n", bufferID,
                generateFrame(dataStructure, schedule, buffers[nodeId][j], 0).c_str());
    }
}

void writeDataTransmissionCpp(const DataStructure *dataStructure, int nodeId, const vector<CFrame> *schedule,
                              const vector<vector<Buffer>> &buffers,
                              const vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot,
                              FILE *outputFile) {
    fprintf(outputFile, "void fray_basic_demo_rpp_initialize_transmission(){\n    int8_t retVal;\n");
    for (int j = 0; j < buffers[nodeId].size(); j++) {
        int bufferID = (isNodeForcedToHaveKeySlot[nodeId]) ? j + 1 : j;
        fprintf(outputFile, "    {\n"
                        "        uint8_t data[] = {%s};\n",
                generateFrame(dataStructure, schedule, buffers[nodeId][j], 1).c_str()
        );
        fprintf(outputFile,
                "        rpp_sci_printf(\"sending message [%s] on slot %d from buffer %d.\\n\");\n"
                        "        retVal = rpp_fr_transmit_lpdu(0, %dU, data, %d);\n"
                        "        if (retVal == FAILURE) {\n"
                        "            rpp_sci_printf(\"Sending a message from buffer %d failed.\\n\");\n"
                        "            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;\n"
                        "        }\n"
                        "    }\n",
                generateFrame(dataStructure, schedule, buffers[nodeId][j], 0).c_str(),
                buffers[nodeId][j].slotID + 1,
                bufferID,
                bufferID,
                (dataStructure->slotLength + 7) / 8,
                bufferID
        );
    }
    fprintf(outputFile, "}\n");
}

void writeStartCommunication(FILE *outputFile) {
    fprintf(outputFile, "frbtinitU\n");
    fprintf(outputFile, "frbtstart\n\n");
    fprintf(outputFile, "frbtallslots\n");
}

void writeStartCommunicationCpp(FILE *outputFile, const int boardId) {
    string secure[4] = {"FR_SB_RECONFIG_ENABLED", "FR_SB_STAT_REC_DISABLED_STAT_TR_DISABLED", "FR_SB_ALL_REC_DISABLED",
                        "FR_SB_ALL_REC_DISABLED_STAT_TR_DISABLED"};
    fprintf(outputFile,
            "static const Fr_TMS570LS_MsgRAMConfig fray_msg_ram_cfg = {\n"
                    "    .syncFramePayloadMultiplexEnabled = %dU,\n"
                    "    .secureBuffers = %s,\n"
                    "    .statSegmentBufferCount = FRAY_STATIC_BUFFER_COUNT,\n"
                    "    .dynSegmentBufferCount = 0,\n"
                    "    .fifoBufferCount = 0\n"
                    "};\n"
                    "\n"
                    "static const Fr_ConfigType flexray_cfg = {\n"
                    "    .clusterConfiguration = &fray_cluster_cfg,\n"
                    "    .nodeConfiguration = &fray_node_cfg,\n"
                    "    .msgRAMConfig = &fray_msg_ram_cfg,\n"
                    "    .staticBufferConfigs = fray_static_buffers_cfg,\n"
                    "    .dynamicBufferConfigs = NULL,\n"
                    "    .fifoBufferConfigs = NULL,\n"
                    "};\n\n",
            SYNCFRAMEPAYLOADMULTIPLEXENABLED,
            secure[SECUREBUFFERS].c_str());
    fprintf(outputFile,
            "/* Model initialize function */\n"
                    "void fray_basic_demo_rpp_initialize(void)\n"
                    "{\n"
                    "    /* Registration code */\n"
                    "\n"
                    "    /* initialize error status */\n"
                    "    rtmSetErrorStatus(fray_basic_demo_rpp_M, (NULL));\n"
                    "\n"
                    "    /* block I/O */\n"
                    "    (void) memset(((void *) &fray_basic_demo_rpp_B), 0,\n"
                    "                  sizeof(BlockIO_fray_basic_demo_rpp));\n"
                    "\n"
                    "    /* external outputs */\n"
                    "    fray_basic_demo_rpp_Y.Out1 = FALSE;\n"
                    "\n"
                    "    /* Start for S-Function (sfunction_frayconfig): '<Root>/Configure FlexRay node1' */\n"
                    "    int8_t retVal;\n"
                    "    uint32_t error;\n"
                    "    rpp_sci_printf(\"FlexRay board %d started!\\n\");\n"
                    "    retVal = rpp_fr_init_driver(&flexray_cfg, &error);\n"
                    "    if (retVal == FAILURE) {\n"
                    "        rpp_sci_printf(\"FlexRay driver init error.\\n\");\n"
                    "        fray_basic_demo_rpp_B.ConfigureFlexRaynode1 = TRUE;\n"
                    "    } else {\n"
                    "        rpp_sci_printf(\"FlexRay driver initialized.\\n\");\n"
                    "    }\n"
                    "\n"
                    "    retVal = rpp_fr_init_controller(0, &error);\n"
                    "    if (retVal == FAILURE) {\n"
                    "        rpp_sci_printf(\"FlexRay controller init error: %#x.\\n\", error);\n"
                    "        fray_basic_demo_rpp_B.ConfigureFlexRaynode1 = TRUE;\n"
                    "    } else {\n"
                    "        rpp_sci_printf(\"FlexRay controller initialized.\\n\");\n"
                    "    }\n"
                    "\n"
                    "    retVal = rpp_fr_start_communication(0, &error);\n"
                    "    if (retVal == FAILURE) {\n"
                    "        rpp_sci_printf(\"FlexRay start communication failed: %#x.\\n\", error);\n"
                    "        fray_basic_demo_rpp_B.ConfigureFlexRaynode1 = TRUE;\n"
                    "    } else {\n"
                    "        rpp_sci_printf(\"FlexRay communication started.\\n\");\n"
                    "    }\n"
                    "\n"
                    "    retVal = rpp_fr_all_slots(0);\n"
                    "    if (retVal == FAILURE) {\n"
                    "      rpp_sci_printf(\"FlexRay all slots failed: %#x.\\n\", error);\n"
                    "      fray_basic_demo_rpp_B.ConfigureFlexRaynode1 = TRUE;\n"
                    "    } else {\n"
                    "      rpp_sci_printf(\"FlexRay is communicating on all slots.\\n\");\n"
                    "    }\n"
                    "    fray_basic_demo_rpp_initialize_transmission();\n"
                    "}\n\n", boardId);
}

void writeBufferConfiguration(const DataStructure *dataStructure, FILE *outputFile, int nodeId,
                              const vector<vector<Buffer>> &buffers,
                              const vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot) {
    if (isNodeForcedToHaveKeySlot[nodeId]) {
        fprintf(outputFile, "frbtcfgbufS0 slot%d AB cyc0 tx max%d continuous ppi0 int1\n",
                dataStructure->maxSlot + 1 + nodeId, (dataStructure->slotLength + 15) / 16);
    }
    for (int j = 0; j < buffers[nodeId].size(); j++) {
        int bufferID = (isNodeForcedToHaveKeySlot[nodeId]) ? j + 1 : j;
        fprintf(outputFile, "frbtcfgbufS%d slot%d AB cyc%d tx max%d continuous ppi0 int1\n", bufferID,
                buffers[nodeId][j].slotID + 1, buffers[nodeId][j].period + buffers[nodeId][j].cycleID,
                (dataStructure->slotLength + 15) / 16);
    }
}

void writeBufferConfigurationCpp(const DataStructure *dataStructure, FILE *outputFile, int nodeId,
                                 const vector<vector<Buffer>> &buffers,
                                 const vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot) {
    fprintf(outputFile, "#define FRAY_STATIC_BUFFER_COUNT       %d\n"
                    "static const Fr_TMS570LS_BufferConfigType\n"
                    "fray_static_buffers_cfg[FRAY_STATIC_BUFFER_COUNT] = {",
            buffers[nodeId].size());

    if (isNodeForcedToHaveKeySlot[nodeId]) {
        fprintf(outputFile,
                "    {\n"
                        "        // Buffer 0U\n"
                        "        .channel = FR_CHANNEL_AB,\n"
                        "        .cycleCounterFiltering = 0U,\n"
                        "        .fidMask = 0,\n"
                        "        .isTx = TRUE,\n"
                        "        .maxPayload = %dU,\n"
                        "        .msgBufferInterrupt = 1U,\n"
                        "        .payloadPreambleIndicatorTr = 0U,\n"
                        "        .rejectNullFrames = FALSE,\n"
                        "        .rejectStaticSegment = FALSE,\n"
                        "        .singleTransmit = 0U,\n"
                        "        .slotId = %dU\n"
                        "    },",
                (dataStructure->slotLength + 15) / 16,
                dataStructure->maxSlot + 1 + nodeId
        );
    }
    for (int j = 0; j < buffers[nodeId].size(); j++) {
        int bufferID = (isNodeForcedToHaveKeySlot[nodeId]) ? j + 1 : j;
        fprintf(outputFile, "    {\n"
                        "        // Buffer %dU\n"
                        "        .channel = FR_CHANNEL_AB,\n"
                        "        .cycleCounterFiltering = %dU,\n"
                        "        .fidMask = 0,\n"
                        "        .isTx = TRUE,\n"
                        "        .maxPayload = %dU,\n"
                        "        .msgBufferInterrupt = 1U,\n"
                        "        .payloadPreambleIndicatorTr = 0U,\n"
                        "        .rejectNullFrames = FALSE,\n"
                        "        .rejectStaticSegment = FALSE,\n"
                        "        .singleTransmit = 0U,\n"
                        "        .slotId = %dU\n"
                        "    },\n",
                bufferID,
                buffers[nodeId][j].period + buffers[nodeId][j].cycleID,
                (dataStructure->slotLength + 15) / 16,
                buffers[nodeId][j].slotID + 1
        );
    }
    fprintf(outputFile, "};\n\n");

}

void writeNodeConfiguration(FILE *outputFile, NetworkParametersCalculator &nc, int nodeId,
                            const vector<bool, std::allocator<bool>> &isKeyBuffer,
                            const vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot) {
    fprintf(outputFile, "frbtconfignode 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X "
                    "0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",
            PALLOWHALTDUETOCLOCK,
            PALLOWPASSIVETOACTIVE,
            PCHANNELS,
            PCLUSTERDRIFTDAMPING,
            PDELAYCOMPENSATIONA,
            PDELAYCOMPENSATIONB,
            PEXTERNOFFSETCORRECTION,
            PEXTERNRATECORRECTION,
            (isNodeForcedToHaveKeySlot[nodeId] || isKeyBuffer[nodeId]) ? 1 : 0,
            (isNodeForcedToHaveKeySlot[nodeId] || isKeyBuffer[nodeId]) ? 1 : 0,
            PLATESTTX,
            nc.getPMacroInitialOffset(), //PMACROINITIALOFFSETA,
            nc.getPMacroInitialOffset(), //PMACROINITIALOFFSETB,
            nc.getPMicroInitialOffset(), //PMICROINITIALOFFSETA,
            nc.getPMicroInitialOffset(), //PMICROINITIALOFFSETB,
            nc.getGdMicroPerCycle(), //pMicroPerCycle,
            nc.getPRateCorrectionOut(), //PRATECORRECTIONOUT,
            nc.getPOffsetCorrectionOut(), //POFFSETCORRECTIONOUT,
            PSAMPLESPERMICROTICK,
            PSINGLESLOTENABLED,
            PWAKEUPCHANNEL,
            PWAKEUPPATTERN,
            PDACCEPTEDSTARTUPRANGE,
            nc.getPdListenTimeout(), //PDLISTENTIMEOUT,
            nc.getPdDriftMax(), //PDMAXDRIFT,
            PDECODINGCORRECTION,
            SYNCFRAMEPAYLOADMULTIPLEXENABLED,
            SECUREBUFFERS
    );
}

void writeNodeConfigurationCpp(FILE *outputFile, NetworkParametersCalculator &nc, int nodeId,
                               const vector<bool, std::allocator<bool>> &isKeyBuffer,
                               const vector<bool, std::allocator<bool>> &isNodeForcedToHaveKeySlot) {
    fprintf(outputFile, "static const Fr_TMS570LS_NodeConfigType fray_node_cfg = {\n"
                    "    .pAllowHaltDueToClock = %dU,\n"
                    "    .pAllowPassiveToActive = %dU,\n"
                    "    .pChannels = %s,\n"
                    "    .pClusterDriftDamping = %dU,\n"
                    "    .pDelayCompensationA = %dU,\n"
                    "    .pDelayCompensationB = %dU,\n"
                    "    .pExternOffsetCorrection = %dU,\n"
                    "    .pExternRateCorrection = %dU,\n"
                    "    .pKeySlotUsedForStartup = %dU,\n"
                    "    .pKeySlotUsedForSync = %dU,\n"
                    "    .pLatestTx = %dU,\n"
                    "    .pMacroInitialOffsetA = %dU,\n"
                    "    .pMacroInitialOffsetB = %dU,\n"
                    "    .pMicroInitialOffsetA = %dU,\n"
                    "    .pMicroInitialOffsetB = %dU,\n"
                    "    .pMicroPerCycle = %dU,\n"
                    "    .pRateCorrectionOut = %dU,\n"
                    "    .pOffsetCorrectionOut = %dU,\n"
                    "    .pSamplesPerMicrotick = %dU,\n"
                    "    .pSingleSlotEnabled = %dU,\n"
                    "    .pWakeupChannel = %s,\n"
                    "    .pWakeupPattern = %dU,\n"
                    "    .pdAcceptedStartupRange = %dU,\n"
                    "    .pdListenTimeout = %dU,\n"
                    "    .pdMaxDrift = %dU,\n"
                    "    .pDecodingCorrection = %dU,\n"
                    "};\n\n",
            PALLOWHALTDUETOCLOCK,
            PALLOWPASSIVETOACTIVE,
            (PCHANNELS == 2) ? "FR_CHANNEL_AB" : "FR_CHANNEL_A",
            PCLUSTERDRIFTDAMPING,
            PDELAYCOMPENSATIONA,
            PDELAYCOMPENSATIONB,
            PEXTERNOFFSETCORRECTION,
            PEXTERNRATECORRECTION,
            (isNodeForcedToHaveKeySlot[nodeId] || isKeyBuffer[nodeId]) ? 1 : 0,
            (isNodeForcedToHaveKeySlot[nodeId] || isKeyBuffer[nodeId]) ? 1 : 0,
            PLATESTTX,
            nc.getPMacroInitialOffset(), //PMACROINITIALOFFSETA,
            nc.getPMacroInitialOffset(), //PMACROINITIALOFFSETB,
            nc.getPMicroInitialOffset(), //PMICROINITIALOFFSETA,
            nc.getPMicroInitialOffset(), //PMICROINITIALOFFSETB,
            nc.getGdMicroPerCycle(), //pMicroPerCycle,
            nc.getPRateCorrectionOut(), //PRATECORRECTIONOUT,
            nc.getPOffsetCorrectionOut(), //POFFSETCORRECTIONOUT,
            PSAMPLESPERMICROTICK,
            PSINGLESLOTENABLED,
            (PWAKEUPCHANNEL == 0) ? "FR_CHANNEL_A" : "FR_CHANNEL_B",
            PWAKEUPPATTERN,
            PDACCEPTEDSTARTUPRANGE,
            nc.getPdListenTimeout(), //PDLISTENTIMEOUT,
            nc.getPdDriftMax(), //PDMAXDRIFT,
            PDECODINGCORRECTION
    );
}

void writeClusterConfiguration(FILE *outputFile, NetworkParametersCalculator &nc, int nodeId) {
    fprintf(outputFile, "#The configuration of the node %d#\n", nodeId + 1);
    fprintf(outputFile, "frbtconfigcluster 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X"
                    " 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",
            GCOLDSTARTATTEMPTS,
            GLISTENNOISE,
            nc.getGdMacroPerCycle(), //gMacroPerCycle, !!!
            GMAXWITHOUTCLOCKCORRECTIONFATAL,
            GMAXWITHOUTCLOCKCORRECTIONPASSIVE,
            GNETWORKMANAGEMENTVECTORLENGTH,
            nc.getGNumberOfMinislots(), //GNUMBEROFMINISLOTS,
            nc.getGNumberOfStaticSlots(), //gNumberOfStaticSlots + dataStructure->nodeCount,
            nc.getGOffsetCorrectionStart(), //GOFFSETCORRECTIONSTART,
            nc.getGPayloadLengthStatic(), //gPayloadLengthStatic,
            GSYNCNODEMAX,
            nc.getGdActionPointOffset(), //GDACTIONPOINTOFFSET,
            GDCASRXLOWMAX,
            GDDYNAMICSLOTIDLEPHASE,
            nc.getGdMiniSlot(), //GDMINISLOT,
            nc.getGdMinislotActionPointOffset(), //GDMINISLOTACTIONPOINTOFFSET,
            nc.getGdMacroPerCycle() - nc.getGdNIT(), //GDNIT,
            GDSAMPLECLOCKPERIOD,
            nc.getGdStaticSlot(), //gdStaticSlot,
            nc.getGdTSSTransmitter(), //GDTSSTRANSMITTER,
            GDWAKEUPSYMBOLRXIDLE,
            GDWAKEUPSYMBOLRXLOW,
            GDWAKEUPSYMBOLRXWINDOW,
            GDWAKEUPSYMBOLTXIDLE,
            GDWAKEUPSYMBOLTXLOW
    );
}

void writeClusterConfigurationCpp(FILE *outputFile, NetworkParametersCalculator &nc, int nodeId) {
    fprintf(outputFile, "static const Fr_TMS570LS_ClusterConfigType fray_cluster_cfg = {\n"
                    "    .gColdStartAttempts = %dU,\n"
                    "    .gListenNoise = %dU,\n"
                    "    .gMacroPerCycle = %dU,\n"
                    "    .gMaxWithoutClockCorrectionFatal = %dU,\n"
                    "    .gMaxWithoutClockCorrectionPassive = %dU,\n"
                    "    .gNetworkManagementVectorLength = %dU,\n"
                    "    .gNumberOfMinislots = %dU,\n"
                    "    .gNumberOfStaticSlots = %dU,\n"
                    "    .gOffsetCorrectionStart = %dU,\n"
                    "    .gPayloadLengthStatic = %dU,\n"
                    "    .gSyncNodeMax = %dU,\n"
                    "    .gdActionPointOffset = %dU,\n"
                    "    .gdCASRxLowMax = %dU,\n"
                    "    .gdDynamicSlotIdlePhase = %dU,\n"
                    "    .gdMinislot = %dU,\n"
                    "    .gdMinislotActionPointOffset = %dU,\n"
                    "    .gdNIT = %dU,\n"
                    "    .gdSampleClockPeriod = %dU,\n"
                    "    .gdStaticSlot = %dU,\n"
                    "    .gdTSSTransmitter = %dU,\n"
                    "    .gdWakeupSymbolRxIdle = %dU,\n"
                    "    .gdWakeupSymbolRxLow = %dU,\n"
                    "    .gdWakeupSymbolRxWindow = %dU,\n"
                    "    .gdWakeupSymbolTxIdle = %dU,\n"
                    "    .gdWakeupSymbolTxLow = %dU,\n"
                    "};\n\n",
            GCOLDSTARTATTEMPTS,
            GLISTENNOISE,
            nc.getGdMacroPerCycle(), //gMacroPerCycle, !!!
            GMAXWITHOUTCLOCKCORRECTIONFATAL,
            GMAXWITHOUTCLOCKCORRECTIONPASSIVE,
            GNETWORKMANAGEMENTVECTORLENGTH,
            nc.getGNumberOfMinislots(), //GNUMBEROFMINISLOTS,
            nc.getGNumberOfStaticSlots(), //gNumberOfStaticSlots + dataStructure->nodeCount,
            nc.getGOffsetCorrectionStart(), //GOFFSETCORRECTIONSTART,
            nc.getGPayloadLengthStatic(), //gPayloadLengthStatic,
            GSYNCNODEMAX,
            nc.getGdActionPointOffset(), //GDACTIONPOINTOFFSET,
            GDCASRXLOWMAX,
            GDDYNAMICSLOTIDLEPHASE,
            nc.getGdMiniSlot(), //GDMINISLOT,
            nc.getGdMinislotActionPointOffset(), //GDMINISLOTACTIONPOINTOFFSET,
            nc.getGdMacroPerCycle() - nc.getGdNIT(), //GDNIT,
            GDSAMPLECLOCKPERIOD,
            nc.getGdStaticSlot(), //gdStaticSlot,
            nc.getGdTSSTransmitter(), //GDTSSTRANSMITTER,
            GDWAKEUPSYMBOLRXIDLE,
            GDWAKEUPSYMBOLRXLOW,
            GDWAKEUPSYMBOLRXWINDOW,
            GDWAKEUPSYMBOLTXIDLE,
            GDWAKEUPSYMBOLTXLOW
    );
}

std::string generateFrame(const DataStructure *dataStructure, const vector<CFrame> *schedule, const Buffer &buffer,
                          const int cppStyle) {
    std::string result;
    std::stringstream stream;
    unsigned char *frame = new unsigned char[(dataStructure->slotLength + 7) / 8]();
    const CFrame &cFrame = schedule[buffer.cycleID][buffer.slotID];
    int i;
    for (i = 0; i < cFrame.signals.size(); i++) {
        int &offset = dataStructure->signalInFrameOffsets[cFrame.signals[i]];
        int &payload = dataStructure->signalLengths[cFrame.signals[i]];
        setBitOfBitArray(frame, offset + payload - 1);
    }
    for (i = 0; i < (dataStructure->slotLength + 7) / 8; i++) {
        if (cppStyle == 0) {
            stream << " ";
            stream << std::setfill('0');
            stream << std::hex << std::setw(2) << std::uppercase << static_cast<unsigned int>(frame[i]);
        }
        else {
            stream << " 0x";
            stream << std::setfill('0');
            stream << std::hex << std::setw(2) << std::uppercase << static_cast<unsigned int>(frame[i]);
            if (i + 1 < (dataStructure->slotLength + 7) / 8) {
                stream << ",";
            }
        }
    }
    delete[] frame;
    return stream.str();
}

void setBitOfBitArray(unsigned char *frame, const int &bit) {
    int byte = bit / 8;
    unsigned char offsetInByte = static_cast<unsigned char>(7 - bit % 8);
    const unsigned char one = 1;
    frame[byte] = frame[byte] | (one << offsetInByte);
}

int ComputeGdStaticSlot(DataStructure *dataStructure) {
    return 2 * GDACTIONPOINTOFFSET +
           static_cast<int>(ceil(
                   (
                           (
                                   ComputeAFrameLengthStatic(dataStructure) - CDFES / (double) 2 + CCHANNELIDLEDELIMITER
                           ) * ADBITMAX +
                           ADPROPAGATIONDELAYMAX + AASSUMEDPRECISION + ADMAXIDLEDETECTIONDELAYAFTERHIGH
                   )
                   *
                   (
                           GDMACROTICK / (1 + GCLOCKDEVIATIONMAX)
                   )
           ));
}

int ComputeAFrameLengthStatic(DataStructure *dataStructure) {
    return GDTSSTRANSMITTER + CDFSS + 80 + ((dataStructure->slotLength + 15) / 16) * 20 + CDFES;
}

Buffer createBuffer(const DataStructure *dataStructure, std::vector<CFrame> *schedule, const int &variantID,
                    const int &slotID, const int &cycleID) {
    int powers[7] = {1, 2, 4, 8, 16, 32, 64};
    int startPowerIndex;
    int i, j;
    Buffer resultingBuffer;
    resultingBuffer.cycleID = cycleID;
    resultingBuffer.slotID = slotID;
    for (i = 0; i < 7; i++) {
        if (cycleID < powers[i]) {
            startPowerIndex = i;
            break;
        }
    }

    std::set<int> firstFrameSignals;
    createSetOfFrameSignals(dataStructure, schedule, variantID, slotID, cycleID, firstFrameSignals);

    char isValid;
    for (i = startPowerIndex; powers[i] <= dataStructure->hyperPeriod; i++) {
        isValid = true;
        for (j = cycleID + powers[i]; j < dataStructure->hyperPeriod; j = j + powers[i]) {
            std::set<int> occurenceFrameSignals;
            createSetOfFrameSignals(dataStructure, schedule, variantID, slotID, j, occurenceFrameSignals);
            if (firstFrameSignals.size() != occurenceFrameSignals.size()) {
                isValid = false;
                break;
            }
            else {
                if (!std::equal(firstFrameSignals.begin(), firstFrameSignals.end(), occurenceFrameSignals.begin())) {
                    isValid = false;
                    break;
                }
            }
        }
        if (isValid) {
            resultingBuffer.period = powers[i];
            return resultingBuffer;
        }
    }
}

void createSetOfFrameSignals(const DataStructure *dataStructure, const std::vector<CFrame> *schedule,
                             const int &variantID, const int &slotID, const int &cycleID, std::set<int> &set) {
    int i;
    for (i = 0; i < schedule[cycleID][slotID].signals.size(); i++) {
        if (dataStructure->modelVariants[variantID][schedule[cycleID][slotID].signals[i]] == 1) {
            set.insert(schedule[cycleID][slotID].signals[i]);
        }
    }
}
