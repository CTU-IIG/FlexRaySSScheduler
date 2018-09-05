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

#define FRAY_STATIC_BUFFER_COUNT       109
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
                                                                     .slotId = 27U
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
                                                                     .slotId = 28U
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
                                                                     .slotId = 29U
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
                                                                     .slotId = 30U
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
                                                                     .slotId = 31U
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
                                                                     .slotId = 32U
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
                                                                     .slotId = 33U
                                                             },
                                                             {
                                                                     // Buffer 7U
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
                                                                     .slotId = 34U
                                                             },
                                                             {
                                                                     // Buffer 8U
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
                                                                     .slotId = 35U
                                                             },
                                                             {
                                                                     // Buffer 9U
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
                                                                     .slotId = 36U
                                                             },
                                                             {
                                                                     // Buffer 10U
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
                                                                     .slotId = 37U
                                                             },
                                                             {
                                                                     // Buffer 11U
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
                                                                     .slotId = 38U
                                                             },
                                                             {
                                                                     // Buffer 12U
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
                                                                     .slotId = 39U
                                                             },
                                                             {
                                                                     // Buffer 13U
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
                                                                     .slotId = 40U
                                                             },
                                                             {
                                                                     // Buffer 14U
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
                                                                     .slotId = 41U
                                                             },
                                                             {
                                                                     // Buffer 15U
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
                                                                     .slotId = 41U
                                                             },
                                                             {
                                                                     // Buffer 16U
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
                                                                     .slotId = 42U
                                                             },
                                                             {
                                                                     // Buffer 17U
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
                                                                     .slotId = 42U
                                                             },
                                                             {
                                                                     // Buffer 18U
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
                                                                     .slotId = 43U
                                                             },
                                                             {
                                                                     // Buffer 19U
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
                                                                     .slotId = 43U
                                                             },
                                                             {
                                                                     // Buffer 20U
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
                                                                     .slotId = 44U
                                                             },
                                                             {
                                                                     // Buffer 21U
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
                                                                     .slotId = 44U
                                                             },
                                                             {
                                                                     // Buffer 22U
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
                                                                     .slotId = 45U
                                                             },
                                                             {
                                                                     // Buffer 23U
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
                                                                     .slotId = 45U
                                                             },
                                                             {
                                                                     // Buffer 24U
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
                                                                     .slotId = 46U
                                                             },
                                                             {
                                                                     // Buffer 25U
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
                                                                     .slotId = 46U
                                                             },
                                                             {
                                                                     // Buffer 26U
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
                                                                     .slotId = 47U
                                                             },
                                                             {
                                                                     // Buffer 27U
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
                                                                     .slotId = 47U
                                                             },
                                                             {
                                                                     // Buffer 28U
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
                                                                     .slotId = 48U
                                                             },
                                                             {
                                                                     // Buffer 29U
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
                                                                     .slotId = 48U
                                                             },
                                                             {
                                                                     // Buffer 30U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 6U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 48U
                                                             },
                                                             {
                                                                     // Buffer 31U
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
                                                                     .slotId = 48U
                                                             },
                                                             {
                                                                     // Buffer 32U
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
                                                                     .slotId = 49U
                                                             },
                                                             {
                                                                     // Buffer 33U
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
                                                                     .slotId = 49U
                                                             },
                                                             {
                                                                     // Buffer 34U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 6U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 49U
                                                             },
                                                             {
                                                                     // Buffer 35U
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
                                                                     .slotId = 49U
                                                             },
                                                             {
                                                                     // Buffer 36U
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
                                                                     .slotId = 50U
                                                             },
                                                             {
                                                                     // Buffer 37U
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
                                                                     .slotId = 50U
                                                             },
                                                             {
                                                                     // Buffer 38U
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
                                                                     .slotId = 50U
                                                             },
                                                             {
                                                                     // Buffer 39U
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
                                                                     .slotId = 50U
                                                             },
                                                             {
                                                                     // Buffer 40U
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
                                                                     .slotId = 50U
                                                             },
                                                             {
                                                                     // Buffer 41U
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
                                                                     .slotId = 50U
                                                             },
                                                             {
                                                                     // Buffer 42U
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
                                                                     .slotId = 50U
                                                             },
                                                             {
                                                                     // Buffer 43U
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
                                                                     .slotId = 50U
                                                             },
                                                             {
                                                                     // Buffer 44U
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
                                                                     .slotId = 51U
                                                             },
                                                             {
                                                                     // Buffer 45U
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
                                                                     .slotId = 51U
                                                             },
                                                             {
                                                                     // Buffer 46U
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
                                                                     .slotId = 51U
                                                             },
                                                             {
                                                                     // Buffer 47U
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
                                                                     .slotId = 51U
                                                             },
                                                             {
                                                                     // Buffer 48U
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
                                                                     .slotId = 51U
                                                             },
                                                             {
                                                                     // Buffer 49U
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
                                                                     .slotId = 51U
                                                             },
                                                             {
                                                                     // Buffer 50U
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
                                                                     .slotId = 51U
                                                             },
                                                             {
                                                                     // Buffer 51U
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
                                                                     .slotId = 51U
                                                             },
                                                             {
                                                                     // Buffer 52U
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
                                                                     .slotId = 52U
                                                             },
                                                             {
                                                                     // Buffer 53U
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
                                                                     .slotId = 52U
                                                             },
                                                             {
                                                                     // Buffer 54U
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
                                                                     .slotId = 52U
                                                             },
                                                             {
                                                                     // Buffer 55U
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
                                                                     .slotId = 52U
                                                             },
                                                             {
                                                                     // Buffer 56U
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
                                                                     .slotId = 52U
                                                             },
                                                             {
                                                                     // Buffer 57U
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
                                                                     .slotId = 52U
                                                             },
                                                             {
                                                                     // Buffer 58U
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
                                                                     .slotId = 52U
                                                             },
                                                             {
                                                                     // Buffer 59U
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
                                                                     .slotId = 52U
                                                             },
                                                             {
                                                                     // Buffer 60U
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
                                                                     .slotId = 53U
                                                             },
                                                             {
                                                                     // Buffer 61U
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
                                                                     .slotId = 53U
                                                             },
                                                             {
                                                                     // Buffer 62U
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
                                                                     .slotId = 53U
                                                             },
                                                             {
                                                                     // Buffer 63U
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
                                                                     .slotId = 53U
                                                             },
                                                             {
                                                                     // Buffer 64U
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
                                                                     .slotId = 53U
                                                             },
                                                             {
                                                                     // Buffer 65U
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
                                                                     .slotId = 53U
                                                             },
                                                             {
                                                                     // Buffer 66U
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
                                                                     .slotId = 53U
                                                             },
                                                             {
                                                                     // Buffer 67U
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
                                                                     .slotId = 53U
                                                             },
                                                             {
                                                                     // Buffer 68U
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
                                                                     .slotId = 54U
                                                             },
                                                             {
                                                                     // Buffer 69U
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
                                                                     .slotId = 54U
                                                             },
                                                             {
                                                                     // Buffer 70U
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
                                                                     .slotId = 54U
                                                             },
                                                             {
                                                                     // Buffer 71U
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
                                                                     .slotId = 54U
                                                             },
                                                             {
                                                                     // Buffer 72U
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
                                                                     .slotId = 54U
                                                             },
                                                             {
                                                                     // Buffer 73U
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
                                                                     .slotId = 54U
                                                             },
                                                             {
                                                                     // Buffer 74U
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
                                                                     .slotId = 54U
                                                             },
                                                             {
                                                                     // Buffer 75U
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
                                                                     .slotId = 54U
                                                             },
                                                             {
                                                                     // Buffer 76U
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
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 77U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 17U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 78U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 18U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 79U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 19U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 80U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 20U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 81U
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
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 82U
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
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 83U
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
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 84U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 25U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 85U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 26U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 86U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 27U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 87U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 28U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 88U
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
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 89U
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
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 90U
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
                                                                     .slotId = 55U
                                                             },
                                                             {
                                                                     // Buffer 91U
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
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 92U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 17U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 93U
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
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 94U
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
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 95U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 36U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 96U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 37U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 97U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 38U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 98U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 39U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 99U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 50U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 100U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 72U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 101U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 73U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 102U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 74U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 103U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 75U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 104U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 76U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 105U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 77U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 106U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 78U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 107U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 79U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
                                                             },
                                                             {
                                                                     // Buffer 108U
                                                                     .channel = FR_CHANNEL_AB,
                                                                     .cycleCounterFiltering = 104U,
                                                                     .fidMask = 0,
                                                                     .isTx = TRUE,
                                                                     .maxPayload = 2U,
                                                                     .msgBufferInterrupt = 1U,
                                                                     .payloadPreambleIndicatorTr = 0U,
                                                                     .rejectNullFrames = FALSE,
                                                                     .rejectStaticSegment = FALSE,
                                                                     .singleTransmit = 0U,
                                                                     .slotId = 56U
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
    rpp_sci_printf("FlexRay board 1 started!\n");
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
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 27 from buffer 0.\n");
        retVal = rpp_fr_transmit_lpdu(0, 0U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 0 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 28 from buffer 1.\n");
        retVal = rpp_fr_transmit_lpdu(0, 1U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 1 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 29 from buffer 2.\n");
        retVal = rpp_fr_transmit_lpdu(0, 2U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 2 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 30 from buffer 3.\n");
        retVal = rpp_fr_transmit_lpdu(0, 3U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 3 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 31 from buffer 4.\n");
        retVal = rpp_fr_transmit_lpdu(0, 4U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 4 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x03};
        rpp_sci_printf("sending message [ 01 01 01 03] on slot 32 from buffer 5.\n");
        retVal = rpp_fr_transmit_lpdu(0, 5U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 5 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x02, 0x04, 0x08, 0x11};
        rpp_sci_printf("sending message [ 02 04 08 11] on slot 33 from buffer 6.\n");
        retVal = rpp_fr_transmit_lpdu(0, 6U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 6 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x02, 0x04, 0x08, 0x11};
        rpp_sci_printf("sending message [ 02 04 08 11] on slot 34 from buffer 7.\n");
        retVal = rpp_fr_transmit_lpdu(0, 7U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 7 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x02, 0x08, 0x20, 0x83};
        rpp_sci_printf("sending message [ 02 08 20 83] on slot 35 from buffer 8.\n");
        retVal = rpp_fr_transmit_lpdu(0, 8U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 8 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x09};
        rpp_sci_printf("sending message [ 04 10 41 09] on slot 36 from buffer 9.\n");
        retVal = rpp_fr_transmit_lpdu(0, 9U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 9 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x08, 0x44, 0x92, 0x49};
        rpp_sci_printf("sending message [ 08 44 92 49] on slot 37 from buffer 10.\n");
        retVal = rpp_fr_transmit_lpdu(0, 10U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 10 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x95, 0x55, 0x55};
        rpp_sci_printf("sending message [ 24 95 55 55] on slot 38 from buffer 11.\n");
        retVal = rpp_fr_transmit_lpdu(0, 11U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 11 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x55, 0x55, 0x55, 0x57};
        rpp_sci_printf("sending message [ 55 55 55 57] on slot 39 from buffer 12.\n");
        retVal = rpp_fr_transmit_lpdu(0, 12U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 12 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF};
        rpp_sci_printf("sending message [ FF FF FF FF] on slot 40 from buffer 13.\n");
        retVal = rpp_fr_transmit_lpdu(0, 13U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 13 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xFF, 0x80, 0x81};
        rpp_sci_printf("sending message [ FF FF 80 81] on slot 41 from buffer 14.\n");
        retVal = rpp_fr_transmit_lpdu(0, 14U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 14 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xFF, 0x81, 0x07};
        rpp_sci_printf("sending message [ FF FF 81 07] on slot 41 from buffer 15.\n");
        retVal = rpp_fr_transmit_lpdu(0, 15U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 15 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 42 from buffer 16.\n");
        retVal = rpp_fr_transmit_lpdu(0, 16U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 16 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x40, 0x41};
        rpp_sci_printf("sending message [ 04 10 40 41] on slot 42 from buffer 17.\n");
        retVal = rpp_fr_transmit_lpdu(0, 17U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 17 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 43 from buffer 18.\n");
        retVal = rpp_fr_transmit_lpdu(0, 18U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 18 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 43 from buffer 19.\n");
        retVal = rpp_fr_transmit_lpdu(0, 19U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 19 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 44 from buffer 20.\n");
        retVal = rpp_fr_transmit_lpdu(0, 20U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 20 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 44 from buffer 21.\n");
        retVal = rpp_fr_transmit_lpdu(0, 21U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 21 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x02, 0x04, 0x08, 0x11};
        rpp_sci_printf("sending message [ 02 04 08 11] on slot 45 from buffer 22.\n");
        retVal = rpp_fr_transmit_lpdu(0, 22U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 22 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 45 from buffer 23.\n");
        retVal = rpp_fr_transmit_lpdu(0, 23U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 23 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x09};
        rpp_sci_printf("sending message [ 04 10 41 09] on slot 46 from buffer 24.\n");
        retVal = rpp_fr_transmit_lpdu(0, 24U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 24 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x08, 0x42, 0x11, 0x25};
        rpp_sci_printf("sending message [ 08 42 11 25] on slot 46 from buffer 25.\n");
        retVal = rpp_fr_transmit_lpdu(0, 25U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 25 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0xAA, 0xAA, 0xAB};
        rpp_sci_printf("sending message [ 24 AA AA AB] on slot 47 from buffer 26.\n");
        retVal = rpp_fr_transmit_lpdu(0, 26U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 26 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x55, 0x5F, 0xFF, 0xFF};
        rpp_sci_printf("sending message [ 55 5F FF FF] on slot 47 from buffer 27.\n");
        retVal = rpp_fr_transmit_lpdu(0, 27U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 27 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xFF, 0xC1, 0x21};
        rpp_sci_printf("sending message [ FF FF C1 21] on slot 48 from buffer 28.\n");
        retVal = rpp_fr_transmit_lpdu(0, 28U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 28 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xC0, 0x40, 0x40, 0x41};
        rpp_sci_printf("sending message [ C0 40 40 41] on slot 48 from buffer 29.\n");
        retVal = rpp_fr_transmit_lpdu(0, 29U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 29 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xFF, 0xC2, 0x03};
        rpp_sci_printf("sending message [ FF FF C2 03] on slot 48 from buffer 30.\n");
        retVal = rpp_fr_transmit_lpdu(0, 30U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 30 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x40, 0x40, 0x40, 0x41};
        rpp_sci_printf("sending message [ 40 40 40 41] on slot 48 from buffer 31.\n");
        retVal = rpp_fr_transmit_lpdu(0, 31U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 31 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x02, 0x04, 0x09};
        rpp_sci_printf("sending message [ 01 02 04 09] on slot 49 from buffer 32.\n");
        retVal = rpp_fr_transmit_lpdu(0, 32U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 32 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x02, 0x08, 0x20, 0x83};
        rpp_sci_printf("sending message [ 02 08 20 83] on slot 49 from buffer 33.\n");
        retVal = rpp_fr_transmit_lpdu(0, 33U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 33 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 49 from buffer 34.\n");
        retVal = rpp_fr_transmit_lpdu(0, 34U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 34 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x92, 0x49, 0x25};
        rpp_sci_printf("sending message [ 04 92 49 25] on slot 49 from buffer 35.\n");
        retVal = rpp_fr_transmit_lpdu(0, 35U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 35 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x55, 0x57, 0xFF, 0xFF};
        rpp_sci_printf("sending message [ 55 57 FF FF] on slot 50 from buffer 36.\n");
        retVal = rpp_fr_transmit_lpdu(0, 36U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 36 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x38, 0x08, 0x09};
        rpp_sci_printf("sending message [ 01 38 08 09] on slot 50 from buffer 37.\n");
        retVal = rpp_fr_transmit_lpdu(0, 37U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 37 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x80, 0x80, 0x80, 0x83};
        rpp_sci_printf("sending message [ 80 80 80 83] on slot 50 from buffer 38.\n");
        retVal = rpp_fr_transmit_lpdu(0, 38U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 38 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x40, 0x40, 0x40, 0x65};
        rpp_sci_printf("sending message [ 40 40 40 65] on slot 50 from buffer 39.\n");
        retVal = rpp_fr_transmit_lpdu(0, 39U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 39 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x55, 0x57, 0xFF, 0xFF};
        rpp_sci_printf("sending message [ 55 57 FF FF] on slot 50 from buffer 40.\n");
        retVal = rpp_fr_transmit_lpdu(0, 40U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 40 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x05, 0x57, 0x83};
        rpp_sci_printf("sending message [ 01 05 57 83] on slot 50 from buffer 41.\n");
        retVal = rpp_fr_transmit_lpdu(0, 41U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 41 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x12, 0x80, 0x83};
        rpp_sci_printf("sending message [ 24 12 80 83] on slot 50 from buffer 42.\n");
        retVal = rpp_fr_transmit_lpdu(0, 42U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 42 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x60, 0x20, 0x20, 0x21};
        rpp_sci_printf("sending message [ 60 20 20 21] on slot 50 from buffer 43.\n");
        retVal = rpp_fr_transmit_lpdu(0, 43U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 43 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0xAB, 0xE0, 0x23};
        rpp_sci_printf("sending message [ 04 AB E0 23] on slot 51 from buffer 44.\n");
        retVal = rpp_fr_transmit_lpdu(0, 44U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 44 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 51 from buffer 45.\n");
        retVal = rpp_fr_transmit_lpdu(0, 45U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 45 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 51 from buffer 46.\n");
        retVal = rpp_fr_transmit_lpdu(0, 46U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 46 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 51 from buffer 47.\n");
        retVal = rpp_fr_transmit_lpdu(0, 47U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 47 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 51 from buffer 48.\n");
        retVal = rpp_fr_transmit_lpdu(0, 48U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 48 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 51 from buffer 49.\n");
        retVal = rpp_fr_transmit_lpdu(0, 49U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 49 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 51 from buffer 50.\n");
        retVal = rpp_fr_transmit_lpdu(0, 50U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 50 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 51 from buffer 51.\n");
        retVal = rpp_fr_transmit_lpdu(0, 51U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 51 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 52 from buffer 52.\n");
        retVal = rpp_fr_transmit_lpdu(0, 52U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 52 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 52 from buffer 53.\n");
        retVal = rpp_fr_transmit_lpdu(0, 53U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 53 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 52 from buffer 54.\n");
        retVal = rpp_fr_transmit_lpdu(0, 54U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 54 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 52 from buffer 55.\n");
        retVal = rpp_fr_transmit_lpdu(0, 55U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 55 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 52 from buffer 56.\n");
        retVal = rpp_fr_transmit_lpdu(0, 56U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 56 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x02, 0x04, 0x08, 0x11};
        rpp_sci_printf("sending message [ 02 04 08 11] on slot 52 from buffer 57.\n");
        retVal = rpp_fr_transmit_lpdu(0, 57U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 57 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x02, 0x04, 0x08, 0x11};
        rpp_sci_printf("sending message [ 02 04 08 11] on slot 52 from buffer 58.\n");
        retVal = rpp_fr_transmit_lpdu(0, 58U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 58 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x02, 0x04, 0x08, 0x11};
        rpp_sci_printf("sending message [ 02 04 08 11] on slot 52 from buffer 59.\n");
        retVal = rpp_fr_transmit_lpdu(0, 59U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 59 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 53 from buffer 60.\n");
        retVal = rpp_fr_transmit_lpdu(0, 60U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 60 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 53 from buffer 61.\n");
        retVal = rpp_fr_transmit_lpdu(0, 61U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 61 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 53 from buffer 62.\n");
        retVal = rpp_fr_transmit_lpdu(0, 62U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 62 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 53 from buffer 63.\n");
        retVal = rpp_fr_transmit_lpdu(0, 63U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 63 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 53 from buffer 64.\n");
        retVal = rpp_fr_transmit_lpdu(0, 64U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 64 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 53 from buffer 65.\n");
        retVal = rpp_fr_transmit_lpdu(0, 65U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 65 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x11, 0x12, 0x49};
        rpp_sci_printf("sending message [ 04 11 12 49] on slot 53 from buffer 66.\n");
        retVal = rpp_fr_transmit_lpdu(0, 66U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 66 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x92, 0x49, 0x25};
        rpp_sci_printf("sending message [ 24 92 49 25] on slot 53 from buffer 67.\n");
        retVal = rpp_fr_transmit_lpdu(0, 67U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 67 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x92, 0x49, 0x25};
        rpp_sci_printf("sending message [ 24 92 49 25] on slot 54 from buffer 68.\n");
        retVal = rpp_fr_transmit_lpdu(0, 68U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 68 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x92, 0x49, 0x25};
        rpp_sci_printf("sending message [ 24 92 49 25] on slot 54 from buffer 69.\n");
        retVal = rpp_fr_transmit_lpdu(0, 69U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 69 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x92, 0x49, 0x55};
        rpp_sci_printf("sending message [ 24 92 49 55] on slot 54 from buffer 70.\n");
        retVal = rpp_fr_transmit_lpdu(0, 70U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 70 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x55, 0x55, 0x55, 0x55};
        rpp_sci_printf("sending message [ 55 55 55 55] on slot 54 from buffer 71.\n");
        retVal = rpp_fr_transmit_lpdu(0, 71U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 71 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x55, 0x55, 0x55, 0x55};
        rpp_sci_printf("sending message [ 55 55 55 55] on slot 54 from buffer 72.\n");
        retVal = rpp_fr_transmit_lpdu(0, 72U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 72 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x55, 0x55, 0x55, 0x57};
        rpp_sci_printf("sending message [ 55 55 55 57] on slot 54 from buffer 73.\n");
        retVal = rpp_fr_transmit_lpdu(0, 73U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 73 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF};
        rpp_sci_printf("sending message [ FF FF FF FF] on slot 54 from buffer 74.\n");
        retVal = rpp_fr_transmit_lpdu(0, 74U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 74 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF};
        rpp_sci_printf("sending message [ FF FF FF FF] on slot 54 from buffer 75.\n");
        retVal = rpp_fr_transmit_lpdu(0, 75U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 75 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF};
        rpp_sci_printf("sending message [ FF FF FF FF] on slot 55 from buffer 76.\n");
        retVal = rpp_fr_transmit_lpdu(0, 76U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 76 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xC1, 0x04, 0xDD};
        rpp_sci_printf("sending message [ FF C1 04 DD] on slot 55 from buffer 77.\n");
        retVal = rpp_fr_transmit_lpdu(0, 77U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 77 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x03};
        rpp_sci_printf("sending message [ 01 01 01 03] on slot 55 from buffer 78.\n");
        retVal = rpp_fr_transmit_lpdu(0, 78U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 78 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x80, 0x80, 0x80, 0x81};
        rpp_sci_printf("sending message [ 80 80 80 81] on slot 55 from buffer 79.\n");
        retVal = rpp_fr_transmit_lpdu(0, 79U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 79 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x60, 0x20, 0x20, 0x21};
        rpp_sci_printf("sending message [ 60 20 20 21] on slot 55 from buffer 80.\n");
        retVal = rpp_fr_transmit_lpdu(0, 80U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 80 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x08, 0x92, 0x55};
        rpp_sci_printf("sending message [ 01 08 92 55] on slot 55 from buffer 81.\n");
        retVal = rpp_fr_transmit_lpdu(0, 81U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 81 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x80, 0x80, 0x80, 0x81};
        rpp_sci_printf("sending message [ 80 80 80 81] on slot 55 from buffer 82.\n");
        retVal = rpp_fr_transmit_lpdu(0, 82U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 82 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 55 from buffer 83.\n");
        retVal = rpp_fr_transmit_lpdu(0, 83U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 83 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0x80, 0x80, 0x81};
        rpp_sci_printf("sending message [ FF 80 80 81] on slot 55 from buffer 84.\n");
        retVal = rpp_fr_transmit_lpdu(0, 84U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 84 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 55 from buffer 85.\n");
        retVal = rpp_fr_transmit_lpdu(0, 85U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 85 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 55 from buffer 86.\n");
        retVal = rpp_fr_transmit_lpdu(0, 86U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 86 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 55 from buffer 87.\n");
        retVal = rpp_fr_transmit_lpdu(0, 87U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 87 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 55 from buffer 88.\n");
        retVal = rpp_fr_transmit_lpdu(0, 88U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 88 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x08, 0x44, 0x92, 0x49};
        rpp_sci_printf("sending message [ 08 44 92 49] on slot 55 from buffer 89.\n");
        retVal = rpp_fr_transmit_lpdu(0, 89U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 89 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x95, 0x55, 0x55};
        rpp_sci_printf("sending message [ 24 95 55 55] on slot 55 from buffer 90.\n");
        retVal = rpp_fr_transmit_lpdu(0, 90U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 90 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x55, 0x55, 0x57, 0xFF};
        rpp_sci_printf("sending message [ 55 55 57 FF] on slot 56 from buffer 91.\n");
        retVal = rpp_fr_transmit_lpdu(0, 91U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 91 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF};
        rpp_sci_printf("sending message [ FF FF FF FF] on slot 56 from buffer 92.\n");
        retVal = rpp_fr_transmit_lpdu(0, 92U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 92 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xE0, 0x92, 0x09, 0x41};
        rpp_sci_printf("sending message [ E0 92 09 41] on slot 56 from buffer 93.\n");
        retVal = rpp_fr_transmit_lpdu(0, 93U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 93 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 56 from buffer 94.\n");
        retVal = rpp_fr_transmit_lpdu(0, 94U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 94 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 01 01 01 01] on slot 56 from buffer 95.\n");
        retVal = rpp_fr_transmit_lpdu(0, 95U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 95 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0x04, 0x04, 0x05};
        rpp_sci_printf("sending message [ 24 04 04 05] on slot 56 from buffer 96.\n");
        retVal = rpp_fr_transmit_lpdu(0, 96U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 96 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x02, 0x04, 0x09};
        rpp_sci_printf("sending message [ 01 02 04 09] on slot 56 from buffer 97.\n");
        retVal = rpp_fr_transmit_lpdu(0, 97U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 97 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x21, 0x25, 0x55};
        rpp_sci_printf("sending message [ 04 21 25 55] on slot 56 from buffer 98.\n");
        retVal = rpp_fr_transmit_lpdu(0, 98U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 98 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xE0, 0x00, 0x00, 0x00};
        rpp_sci_printf("sending message [ E0 00 00 00] on slot 56 from buffer 99.\n");
        retVal = rpp_fr_transmit_lpdu(0, 99U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 99 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x57, 0xFF, 0xFF, 0x01};
        rpp_sci_printf("sending message [ 57 FF FF 01] on slot 56 from buffer 100.\n");
        retVal = rpp_fr_transmit_lpdu(0, 100U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 100 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x54, 0x04, 0x10, 0x4B};
        rpp_sci_printf("sending message [ 54 04 10 4B] on slot 56 from buffer 101.\n");
        retVal = rpp_fr_transmit_lpdu(0, 101U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 101 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x7D, 0x01, 0x01, 0x01};
        rpp_sci_printf("sending message [ 7D 01 01 01] on slot 56 from buffer 102.\n");
        retVal = rpp_fr_transmit_lpdu(0, 102U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 102 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x01, 0x04, 0x10, 0x41};
        rpp_sci_printf("sending message [ 01 04 10 41] on slot 56 from buffer 103.\n");
        retVal = rpp_fr_transmit_lpdu(0, 103U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 103 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x41, 0x05};
        rpp_sci_printf("sending message [ 04 10 41 05] on slot 56 from buffer 104.\n");
        retVal = rpp_fr_transmit_lpdu(0, 104U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 104 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x04, 0x10, 0x44, 0x49};
        rpp_sci_printf("sending message [ 04 10 44 49] on slot 56 from buffer 105.\n");
        retVal = rpp_fr_transmit_lpdu(0, 105U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 105 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x24, 0xAA, 0xAF, 0xFF};
        rpp_sci_printf("sending message [ 24 AA AF FF] on slot 56 from buffer 106.\n");
        retVal = rpp_fr_transmit_lpdu(0, 106U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 106 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0xFF, 0xC0, 0x00, 0x00};
        rpp_sci_printf("sending message [ FF C0 00 00] on slot 56 from buffer 107.\n");
        retVal = rpp_fr_transmit_lpdu(0, 107U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 107 failed.\n");
            fray_basic_demo_rpp_B.ConfigureTXbuffer = TRUE;
        }
    }
    {
        uint8_t data[] = {0x57, 0xFF, 0xFE, 0x00};
        rpp_sci_printf("sending message [ 57 FF FE 00] on slot 56 from buffer 108.\n");
        retVal = rpp_fr_transmit_lpdu(0, 108U, data, 4);
        if (retVal == FAILURE) {
            rpp_sci_printf("Sending a message from buffer 108 failed.\n");
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
