/*
 * File: fray_basic_demo_rpp.c
 *
 * Target selection: rpp.tlc
 * Embedded hardware selection: 32-bit Generic
 */

#include "fray_basic_demo_rpp.h"
#include "fray_basic_demo_rpp_private.h"

/* Block signals (auto storage) */
BlockIO_fray_basic_demo_rpp fray_basic_demo_rpp_B;

/* External outputs (root outports fed by signals with auto storage) */
ExternalOutputs_fray_basic_demo fray_basic_demo_rpp_Y;

/* Real-time model */
RT_MODEL_fray_basic_demo_rpp fray_basic_demo_rpp_M_;
RT_MODEL_fray_basic_demo_rpp *const fray_basic_demo_rpp_M =
        &fray_basic_demo_rpp_M_;

static const Fr_TMS570LS_ClusterConfigType fray_cluster_cfg = {
        .gColdStartAttempts = 8U,
        .gListenNoise = 2U,
        .gMacroPerCycle = 5000U,
        .gMaxWithoutClockCorrectionFatal = 2U,
        .gMaxWithoutClockCorrectionPassive = 2U,
        .gNetworkManagementVectorLength = 0U,
        .gNumberOfMinislots = 262U,
        .gNumberOfStaticSlots = 127U,
        .gOffsetCorrectionStart = 4991U,
        .gPayloadLengthStatic = 2U,
        .gSyncNodeMax = 15U,
        .gdActionPointOffset = 4U,
        .gdCASRxLowMax = 87U,
        .gdDynamicSlotIdlePhase = 1U,
        .gdMinislot = 7U,
        .gdMinislotActionPointOffset = 3U,
        .gdNIT = 4966U,
        .gdSampleClockPeriod = 0U,
        .gdStaticSlot = 23U,
        .gdTSSTransmitter = 9U,
        .gdWakeupSymbolRxIdle = 59U,
        .gdWakeupSymbolRxLow = 50U,
        .gdWakeupSymbolRxWindow = 301U,
        .gdWakeupSymbolTxIdle = 180U,
        .gdWakeupSymbolTxLow = 60U,
};

static const Fr_TMS570LS_NodeConfigType fray_node_cfg = {
        .pAllowHaltDueToClock = 1U,
        .pAllowPassiveToActive = 0U,
        .pChannels = FR_CHANNEL_AB,
        .pClusterDriftDamping = 2U,
        .pDelayCompensationA = 1U,
        .pDelayCompensationB = 1U,
        .pExternOffsetCorrection = 0U,
        .pExternRateCorrection = 0U,
        .pKeySlotUsedForStartup = 1U,
        .pKeySlotUsedForSync = 1U,
        .pLatestTx = 249U,
        .pMacroInitialOffsetA = 6U,
        .pMacroInitialOffsetB = 6U,
        .pMicroInitialOffsetA = 31U,
        .pMicroInitialOffsetB = 31U,
        .pMicroPerCycle = 200000U,
        .pRateCorrectionOut = 601U,
        .pOffsetCorrectionOut = 177U,
        .pSamplesPerMicrotick = 0U,
        .pSingleSlotEnabled = 0U,
        .pWakeupChannel = FR_CHANNEL_A,
        .pWakeupPattern = 33U,
        .pdAcceptedStartupRange = 212U,
        .pdListenTimeout = 401202U,
        .pdMaxDrift = 601U,
        .pDecodingCorrection = 48U,
};

#define FRAY_STATIC_BUFFER_COUNT       53
static const Fr_TMS570LS_BufferConfigType
        fray_static_buffers_cfg[FRAY_STATIC_BUFFER_COUNT] = {{
                                                                     // Buffer 0U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 1U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 84U
                                                             },
                                                             {
                                                                     // Buffer 1U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 1U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 85U
                                                             },
                                                             {
                                                                     // Buffer 2U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 1U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 86U
                                                             },
                                                             {
                                                                     // Buffer 3U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 1U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 87U
                                                             },
                                                             {
                                                                     // Buffer 4U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 1U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 88U
                                                             },
                                                             {
                                                                     // Buffer 5U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 1U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 89U
                                                             },
                                                             {
                                                                     // Buffer 6U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 1U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 90U
                                                             },
                                                             {
                                                                     // Buffer 7U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 2U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 91U
                                                             },
                                                             {
                                                                     // Buffer 8U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 3U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 91U
                                                             },
                                                             {
                                                                     // Buffer 9U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 2U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 92U
                                                             },
                                                             {
                                                                     // Buffer 10U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 3U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 92U
                                                             },
                                                             {
                                                                     // Buffer 11U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 2U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 93U
                                                             },
                                                             {
                                                                     // Buffer 12U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 3U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 93U
                                                             },
                                                             {
                                                                     // Buffer 13U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 2U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 94U
                                                             },
                                                             {
                                                                     // Buffer 14U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 5U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 94U
                                                             },
                                                             {
                                                                     // Buffer 15U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 7U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 94U
                                                             },
                                                             {
                                                                     // Buffer 16U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 4U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 95U
                                                             },
                                                             {
                                                                     // Buffer 17U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 9U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 95U
                                                             },
                                                             {
                                                                     // Buffer 18U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 10U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 95U
                                                             },
                                                             {
                                                                     // Buffer 19U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 11U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 95U
                                                             },
                                                             {
                                                                     // Buffer 20U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 13U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 95U
                                                             },
                                                             {
                                                                     // Buffer 21U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 14U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 95U
                                                             },
                                                             {
                                                                     // Buffer 22U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 15U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 95U
                                                             },
                                                             {
                                                                     // Buffer 23U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 8U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 96U
                                                             },
                                                             {
                                                                     // Buffer 24U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 9U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 96U
                                                             },
                                                             {
                                                                     // Buffer 25U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 10U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 96U
                                                             },
                                                             {
                                                                     // Buffer 26U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 11U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 96U
                                                             },
                                                             {
                                                                     // Buffer 27U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 12U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 96U
                                                             },
                                                             {
                                                                     // Buffer 28U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 13U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 96U
                                                             },
                                                             {
                                                                     // Buffer 29U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 14U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 96U
                                                             },
                                                             {
                                                                     // Buffer 30U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 15U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 96U
                                                             },
                                                             {
                                                                     // Buffer 31U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 8U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 97U
                                                             },
                                                             {
                                                                     // Buffer 32U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 9U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 97U
                                                             },
                                                             {
                                                                     // Buffer 33U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 10U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 97U
                                                             },
                                                             {
                                                                     // Buffer 34U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 11U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 97U
                                                             },
                                                             {
                                                                     // Buffer 35U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 12U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 97U
                                                             },
                                                             {
                                                                     // Buffer 36U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 21U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 97U
                                                             },
                                                             {
                                                                     // Buffer 37U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 22U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 97U
                                                             },
                                                             {
                                                                     // Buffer 38U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 23U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 97U
                                                             },
                                                             {
                                                                     // Buffer 39U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 29U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 97U
                                                             },
                                                             {
                                                                     // Buffer 40U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 30U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 97U
                                                             },
                                                             {
                                                                     // Buffer 41U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 31U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 97U
                                                             },
                                                             {
                                                                     // Buffer 42U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 16U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 98U
                                                             },
                                                             {
                                                                     // Buffer 43U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 33U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 98U
                                                             },
                                                             {
                                                                     // Buffer 44U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 34U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 98U
                                                             },
                                                             {
                                                                     // Buffer 45U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 35U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 98U
                                                             },
                                                             {
                                                                     // Buffer 46U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 49U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 98U
                                                             },
                                                             {
                                                                     // Buffer 47U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 68U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 98U
                                                             },
                                                             {
                                                                     // Buffer 48U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 69U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 98U
                                                             },
                                                             {
                                                                     // Buffer 49U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 70U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 98U
                                                             },
                                                             {
                                                                     // Buffer 50U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 71U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 98U
                                                             },
                                                             {
                                                                     // Buffer 51U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 100U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 98U
                                                             },
                                                             {
                                                                     // Buffer 52U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 101U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 98U
                                                             },
};

static const Fr_TMS570LS_MsgRAMConfig fray_msg_ram_cfg = {
        .syncFramePayloadMultiplexEnabled = 0U,
        .secureBuffers = FR_SB_RECONFIG_ENABLED,
        .statSegmentBufferCount = FRAY_STATIC_BUFFER_COUNT,
        .dynSegmentBufferCount = 0,
        .fifoBufferCount = 0
};

static const Fr_ConfigType flexray_cfg = {
        .clusterConfiguration = &fray_cluster_cfg,
        .nodeConfiguration = &fray_node_cfg,
        .msgRAMConfig = &fray_msg_ram_cfg,
        .staticBufferConfigs = fray_static_buffers_cfg,
        .dynamicBufferConfigs = NULL,
        .fifoBufferConfigs = NULL,
};

/* Model initialize function */
void fray_basic_demo_rpp_initialize(void) {
    /* Registration code */

    /* initialize error status */
    rtmSetErrorStatus(fray_basic_demo_rpp_M, (NULL));

    /* block I/O */
    (void) memset(((void *) &fray_basic_demo_rpp_B), 0,
                  sizeof(BlockIO_fray_basic_demo_rpp));

    /* external outputs */
    fray_basic_demo_rpp_Y.Out1 = FALSE;

    /* Start for S-Function (sfunction_frayconfig): '<Root>/Configure FlexRay node1' */
    int8_t retVal;
    uint32_t error;
    rpp_sci_printf("FlexRay board 3 started!\n");
    retVal = rpp_fr_init_driver(&flexray_cfg, &error);
    if (retVal == FAILURE) {
        rpp_sci_printf("FlexRay driver init error.\n");
        fray_basic_demo_rpp_B.ConfigureFlexRaynode1 = TRUE;
    } else {
        rpp_sci_printf("FlexRay driver initialized.\n");
    }

    retVal = rpp_fr_init_controller(0, &error);
    if (retVal == FAILURE) {
        rpp_sci_printf("FlexRay controller init error: 0x92.\n", error);
        fray_basic_demo_rpp_B.ConfigureFlexRaynode1 = TRUE;
    } else {
        rpp_sci_printf("FlexRay controller initialized.\n");
    }

    retVal = rpp_fr_start_communication(0, &error);
    if (retVal == FAILURE) {
        rpp_sci_printf("FlexRay start communication failed: 0xa2c4c4c.\n", error);
        fray_basic_demo_rpp_B.ConfigureFlexRaynode1 = TRUE;
    } else {
        rpp_sci_printf("FlexRay communication started.\n");
    }

    retVal = rpp_fr_all_slots(0);
    if (retVal == FAILURE) {
        rpp_sci_printf("FlexRay all slots failed: 0x4c4c554e.\n", error);
        fray_basic_demo_rpp_B.ConfigureFlexRaynode1 = TRUE;
    } else {
        rpp_sci_printf("FlexRay is communicating on all slots.\n");
    }
    fray_basic_demo_rpp_initialize_transmission();
}

void fray_basic_demo_rpp_initialize_transmission() {
    int8_t retVal;
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 84 from buffer 0.\n");
        retVal = rpp_fr_transmit_lpdu(0, 0U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 0 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 85 from buffer 1.\n");
        retVal = rpp_fr_transmit_lpdu(0, 1U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 1 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 86 from buffer 2.\n");
        retVal = rpp_fr_transmit_lpdu(0, 2U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 2 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 87 from buffer 3.\n");
        retVal = rpp_fr_transmit_lpdu(0, 3U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 3 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 88 from buffer 4.\n");
        retVal = rpp_fr_transmit_lpdu(0, 4U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 4 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x08, 0x42, 0x12, 0x49};
        rpp_sci_printf("sending message [ 08 42 12 49] on slot 89 from buffer 5.\n");
        retVal = rpp_fr_transmit_lpdu(0, 5U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 5 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x95, 0x55, 0x55};
        rpp_sci_printf("sending message [ 24 95 55 55] on slot 90 from buffer 6.\n");
        retVal = rpp_fr_transmit_lpdu(0, 6U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 6 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x57, 0xFF, 0xFF, 0xC1};
        rpp_sci_printf("sending message [ 57 FF FF C1] on slot 91 from buffer 7.\n");
        retVal = rpp_fr_transmit_lpdu(0, 7U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 7 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x57, 0xFF, 0xFF, 0xE1};
        rpp_sci_printf("sending message [ 57 FF FF E1] on slot 91 from buffer 8.\n");
        retVal = rpp_fr_transmit_lpdu(0, 8U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 8 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 92 from buffer 9.\n");
        retVal = rpp_fr_transmit_lpdu(0, 9U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 9 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 92 from buffer 10.\n");
        retVal = rpp_fr_transmit_lpdu(0, 10U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 10 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x02, 0x04, 0x10, 0x41};
        rpp_sci_printf("sending message [ 02 04 10 41] on slot 93 from buffer 11.\n");
        retVal = rpp_fr_transmit_lpdu(0, 11U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 11 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x89, 0x25};
        rpp_sci_printf("sending message [ 04 10 89 25] on slot 93 from buffer 12.\n");
        retVal = rpp_fr_transmit_lpdu(0, 12U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 12 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x55, 0x55, 0x5F, 0xFF};
        rpp_sci_printf("sending message [ 55 55 5F FF] on slot 94 from buffer 13.\n");
        retVal = rpp_fr_transmit_lpdu(0, 13U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 13 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xFC, 0x04, 0x09};
        rpp_sci_printf("sending message [ FF FC 04 09] on slot 94 from buffer 14.\n");
        retVal = rpp_fr_transmit_lpdu(0, 14U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 14 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xF8, 0x10, 0x41};
        rpp_sci_printf("sending message [ FF F8 10 41] on slot 94 from buffer 15.\n");
        retVal = rpp_fr_transmit_lpdu(0, 15U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 15 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x42, 0x23};
        rpp_sci_printf("sending message [ 04 10 42 23] on slot 95 from buffer 16.\n");
        retVal = rpp_fr_transmit_lpdu(0, 16U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 16 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x12, 0xAA, 0xFE, 0x0B};
        rpp_sci_printf("sending message [ 12 AA FE 0B] on slot 95 from buffer 17.\n");
        retVal = rpp_fr_transmit_lpdu(0, 17U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 17 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x18, 0x08, 0x2B};
        rpp_sci_printf("sending message [ 04 18 08 2B] on slot 95 from buffer 18.\n");
        retVal = rpp_fr_transmit_lpdu(0, 18U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 18 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x15, 0x82, 0xE1};
        rpp_sci_printf("sending message [ 04 15 82 E1] on slot 95 from buffer 19.\n");
        retVal = rpp_fr_transmit_lpdu(0, 19U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 19 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x12, 0xAA, 0xFE, 0x03};
        rpp_sci_printf("sending message [ 12 AA FE 03] on slot 95 from buffer 20.\n");
        retVal = rpp_fr_transmit_lpdu(0, 20U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 20 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x24, 0xAC, 0x81};
        rpp_sci_printf("sending message [ 01 24 AC 81] on slot 95 from buffer 21.\n");
        retVal = rpp_fr_transmit_lpdu(0, 21U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 21 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 95 from buffer 22.\n");
        retVal = rpp_fr_transmit_lpdu(0, 22U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 22 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 96 from buffer 23.\n");
        retVal = rpp_fr_transmit_lpdu(0, 23U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 23 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 96 from buffer 24.\n");
        retVal = rpp_fr_transmit_lpdu(0, 24U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 24 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 96 from buffer 25.\n");
        retVal = rpp_fr_transmit_lpdu(0, 25U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 25 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x03};
        rpp_sci_printf("sending message [ 01 01 01 03] on slot 96 from buffer 26.\n");
        retVal = rpp_fr_transmit_lpdu(0, 26U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 26 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x02, 0x08, 0x20, 0x83};
        rpp_sci_printf("sending message [ 02 08 20 83] on slot 96 from buffer 27.\n");
        retVal = rpp_fr_transmit_lpdu(0, 27U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 27 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 96 from buffer 28.\n");
        retVal = rpp_fr_transmit_lpdu(0, 28U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 28 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x44, 0x49};
        rpp_sci_printf("sending message [ 04 10 44 49] on slot 96 from buffer 29.\n");
        retVal = rpp_fr_transmit_lpdu(0, 29U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 29 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x92, 0x49, 0x25};
        rpp_sci_printf("sending message [ 24 92 49 25] on slot 96 from buffer 30.\n");
        retVal = rpp_fr_transmit_lpdu(0, 30U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 30 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x92, 0x49, 0x25};
        rpp_sci_printf("sending message [ 24 92 49 25] on slot 97 from buffer 31.\n");
        retVal = rpp_fr_transmit_lpdu(0, 31U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 31 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x95, 0x55, 0x55};
        rpp_sci_printf("sending message [ 24 95 55 55] on slot 97 from buffer 32.\n");
        retVal = rpp_fr_transmit_lpdu(0, 32U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 32 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x55, 0x55, 0x55, 0x55};
        rpp_sci_printf("sending message [ 55 55 55 55] on slot 97 from buffer 33.\n");
        retVal = rpp_fr_transmit_lpdu(0, 33U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 33 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x55, 0xFF, 0xFF, 0xFF};
        rpp_sci_printf("sending message [ 55 FF FF FF] on slot 97 from buffer 34.\n");
        retVal = rpp_fr_transmit_lpdu(0, 34U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 34 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF};
        rpp_sci_printf("sending message [ FF FF FF FF] on slot 97 from buffer 35.\n");
        retVal = rpp_fr_transmit_lpdu(0, 35U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 35 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xE0, 0x20, 0x20, 0x93};
        rpp_sci_printf("sending message [ E0 20 20 93] on slot 97 from buffer 36.\n");
        retVal = rpp_fr_transmit_lpdu(0, 36U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 36 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x54, 0x98, 0x08, 0x51};
        rpp_sci_printf("sending message [ 54 98 08 51] on slot 97 from buffer 37.\n");
        retVal = rpp_fr_transmit_lpdu(0, 37U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 37 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 97 from buffer 38.\n");
        retVal = rpp_fr_transmit_lpdu(0, 38U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 38 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xE0, 0x20, 0x20, 0x21};
        rpp_sci_printf("sending message [ E0 20 20 21] on slot 97 from buffer 39.\n");
        retVal = rpp_fr_transmit_lpdu(0, 39U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 39 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x05};
        rpp_sci_printf("sending message [ 01 01 01 05] on slot 97 from buffer 40.\n");
        retVal = rpp_fr_transmit_lpdu(0, 40U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 40 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x11};
        rpp_sci_printf("sending message [ 04 10 41 11] on slot 97 from buffer 41.\n");
        retVal = rpp_fr_transmit_lpdu(0, 41U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 41 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x92, 0x55, 0x55};
        rpp_sci_printf("sending message [ 24 92 55 55] on slot 98 from buffer 42.\n");
        retVal = rpp_fr_transmit_lpdu(0, 42U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 42 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x5F, 0xFF, 0xFC, 0x15};
        rpp_sci_printf("sending message [ 5F FF FC 15] on slot 98 from buffer 43.\n");
        retVal = rpp_fr_transmit_lpdu(0, 43U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 43 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 98 from buffer 44.\n");
        retVal = rpp_fr_transmit_lpdu(0, 44U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 44 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x40, 0x41, 0x04, 0x25};
        rpp_sci_printf("sending message [ 40 41 04 25] on slot 98 from buffer 45.\n");
        retVal = rpp_fr_transmit_lpdu(0, 45U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 45 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x5F, 0xFF, 0xFC, 0x00};
        rpp_sci_printf("sending message [ 5F FF FC 00] on slot 98 from buffer 46.\n");
        retVal = rpp_fr_transmit_lpdu(0, 46U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 46 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0xAA, 0xFC, 0x05};
        rpp_sci_printf("sending message [ 24 AA FC 05] on slot 98 from buffer 47.\n");
        retVal = rpp_fr_transmit_lpdu(0, 47U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 47 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0xC0, 0x96, 0x03};
        rpp_sci_printf("sending message [ 01 C0 96 03] on slot 98 from buffer 48.\n");
        retVal = rpp_fr_transmit_lpdu(0, 48U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 48 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x02, 0x13};
        rpp_sci_printf("sending message [ 01 01 02 13] on slot 98 from buffer 49.\n");
        retVal = rpp_fr_transmit_lpdu(0, 49U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 49 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x92, 0x5F, 0xC0};
        rpp_sci_printf("sending message [ 24 92 5F C0] on slot 98 from buffer 50.\n");
        retVal = rpp_fr_transmit_lpdu(0, 50U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 50 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0xAA, 0xFC, 0x00};
        rpp_sci_printf("sending message [ 24 AA FC 00] on slot 98 from buffer 51.\n");
        retVal = rpp_fr_transmit_lpdu(0, 51U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 51 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0xC0, 0x00, 0x00};
        rpp_sci_printf("sending message [ 01 C0 00 00] on slot 98 from buffer 52.\n");
        retVal = rpp_fr_transmit_lpdu(0, 52U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 52 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
}

/* Model step function */
void fray_basic_demo_rpp_step(void) {
    int8_t retVal = ERR_PARAM_NO_ERROR;
    Fr_POCStatusType status;
    uint32_t error = ERR_PARAM_NO_ERROR;
    char *StateStrings[] = {
            "CONFIG", "DEFAULT_CONFIG", "HALT", "NORMAL_ACTIVE",
            "NORMAL_PASSIVE", "READY", "STARTUP", "LOOPBACK",
            "MONITOR", "WAKEUP"
    };

    retVal = rpp_fr_get_poc_status(0, &status);
    rpp_sci_printf("State: %s\n", StateStrings[status.State]);
    if (retVal == SUCCESS && status.State == 2) {
        rpp_sci_printf("Connection halted! \r\nTrying to reestablish connection!\r\n");
        retVal = rpp_fr_abort_communication(0);
        if (retVal == SUCCESS) {
            rpp_sci_printf("FlexRay node communication stopped.\r\n");
            retVal = rpp_fr_init_controller(0, &error);
            if (retVal == SUCCESS) {
                rpp_sci_printf("FlexRay controller reinitialized. \r\nStarting new communication!\r\n");
                retVal = rpp_fr_start_communication(0, &error);
                if (retVal == SUCCESS) {
                    fray_basic_demo_rpp_initialize_transmission();
                    rpp_sci_printf("FlexRay communication is running.\r\n");
                }
                else {
                    if (error & FR_STARTUP_ERR_SW_STUP_FOLLOW)
                        rpp_sci_printf("Can not switch POC to RUN state.\r\n");
                    else if (error & FR_STARTUP_ERR_CSINH_DIS)
                        rpp_sci_printf("Cold start inhibit disabled error.\r\n");
                    else if (error & FR_STARTUP_ERR_SW_STUP_READY)
                        rpp_sci_printf("Can not switch back to READY from STARTUP.\r\n");
                    else if (error & FR_STARTUP_ERR_SW_STUP_AS_NCOLD)
                        rpp_sci_printf("Can not switch to STARTUP as non-coldstarter.\r\n");
                    else
                        rpp_sci_printf("General error.\r\n");
                }
                return;
            }
            else {
                rpp_sci_printf("FlexRay needs to be configured before initialization.\r\n");
                return;
            }
        }
        else {
            rpp_sci_printf("General error.\r\n");
            return;
        }
    }  /* (no terminate code required) */
}

/* Model terminate function */
void fray_basic_demo_rpp_terminate(void) {
    /* (no terminate code required) */
}
