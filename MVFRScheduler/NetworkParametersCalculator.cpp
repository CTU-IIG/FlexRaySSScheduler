//
// Created by jenik on 11/2/15.
//

#include "MVFRScheduler.h"

NetworkParametersCalculator::NetworkParametersCalculator(const int Bandwidth, const int SlotPayload,
                                                         const int CycleLength,
                                                         const int SlotCount) : bandwidth(Bandwidth),
                                                                                slotPayload(SlotPayload),
                                                                                gNumberOfStaticSlots(SlotCount) {
    gdCycle = CycleLength * 1000;
    this->CalculateParameters();
}

void NetworkParametersCalculator::CalculateParameters() {
    int cChannelIdleDelimiter = 11;
    int aFrameLengthStatic;
    int aMicroPerMacroNom = 40;
    int adActionPointDifference = 1;
    int adOffsetCorrection = 9;
    int NITAbout = 40;
    const int cdFSS = 1;
    const int cdFES = 2;
    const int cdCAS = 30;
    const float dBDRxai = 0.4;
    const int gdMacrotick = (bandwidth > 2500) ? 1 : 2;
    int pDecodingCorrection = 48;
    int pDelayCompensationA = 1;
    float gdMinPropagationDelay = 0;
    float gdMaxPropagationDelay = 0.4;
    float gdBitMax = 0.10015;
    float cClockDeviationMax = 0.0015;
    float gAssumedPrecision = 2.65;
    float gOffsetCorrectionMax = 4.406;
    float pdMicrotick = 0.025;
    gdMacroTick = 1.0f;

    gdTSSTransmitter = 9;
    gdActionPointOffset = 4;

    gdMinislotActionPointOffset = static_cast<int>(ceilf((gAssumedPrecision - gdMinPropagationDelay) /
                                                         (gdMacroTick * (1 - cClockDeviationMax))));
    gPayloadLengthStatic = static_cast<int>(ceilf(slotPayload / 16.0f));
    aFrameLengthStatic = gdTSSTransmitter + cdFSS + 80 + gPayloadLengthStatic * 20 + cdFES;

    gdStaticSlot =
            2 * gdActionPointOffset + static_cast<int>(ceilf(((aFrameLengthStatic + cChannelIdleDelimiter) * gdBitMax +
                                                              gdMinPropagationDelay + gdMaxPropagationDelay) /
                                                             (gdMacroTick * (1 - cClockDeviationMax))));
    gdMiniSlot = gdMinislotActionPointOffset + static_cast<int>(ceilf((gdMaxPropagationDelay + gAssumedPrecision) /
                                                                      (gdMacroTick * (1 - cClockDeviationMax))));
    gdSymbolWindow = 15 * (2 * gdActionPointOffset + static_cast<int>(ceilf(
            (
                    (gdTSSTransmitter + cdCAS + cChannelIdleDelimiter) *
                    gdBitMax + dBDRxai + gdMinPropagationDelay + gdMaxPropagationDelay
            )
            / (gdMacrotick * (1 - cClockDeviationMax))
    )));
    gdMacroPerCycle = static_cast<int>(floorf(gdCycle / static_cast<float>(gdMacroTick)));
    gOffsetCorrectionStart = gdMacroPerCycle - adOffsetCorrection;
    gNumberOfMinislots = static_cast<int>(ceilf(
            (gdMacroPerCycle - NITAbout - gNumberOfStaticSlots * gdStaticSlot - gdSymbolWindow) /
            static_cast<float>(gdMiniSlot)));
    gdNIT = gdMacroPerCycle - gdStaticSlot * gNumberOfStaticSlots - gdMiniSlot * gNumberOfMinislots - gdSymbolWindow -
            adActionPointDifference;
    gdMicroPerCycle = gdMacroPerCycle * aMicroPerMacroNom;
    pdMaxDrift = static_cast<int>(ceilf(gdMicroPerCycle * 2 * cClockDeviationMax / (1 - cClockDeviationMax)));
    pRateCorrectionOut = static_cast<int>(ceilf((gdMicroPerCycle * 2 * cClockDeviationMax) / (1 - cClockDeviationMax)));
    pdListenTimeout = 2 * (gdMicroPerCycle + pdMaxDrift);
    pMicroInitialOffset =
            (aMicroPerMacroNom - ((pDecodingCorrection + pDelayCompensationA) % aMicroPerMacroNom)) % aMicroPerMacroNom;
    pMacroInitialOffset = gdActionPointOffset + static_cast<int>(ceilf((pDecodingCorrection + pDelayCompensationA) /
                                                                       static_cast<float>(aMicroPerMacroNom)));
    pOffsetCorrectionOut = static_cast<int>(ceilf(gOffsetCorrectionMax / (pdMicrotick * (1 - cClockDeviationMax))));
}

int NetworkParametersCalculator::getGdStaticSlot() {
    return gdStaticSlot;
}

int NetworkParametersCalculator::getGdMiniSlot() {
    return gdMiniSlot;
}

int NetworkParametersCalculator::getGdNIT() {
    return gdNIT;
}

int NetworkParametersCalculator::getGdSymbolWindow() {
    return gdSymbolWindow;
}

int NetworkParametersCalculator::getGdMacroPerCycle() {
    return gdMacroPerCycle;
}

int NetworkParametersCalculator::getGdActionPointOffset() {
    return gdActionPointOffset;
}

int NetworkParametersCalculator::getGdMinislotActionPointOffset() {
    return gdMinislotActionPointOffset;
}

int NetworkParametersCalculator::getGdTSSTransmitter() {
    return gdTSSTransmitter;
}

int NetworkParametersCalculator::getGNumberOfStaticSlots() {
    return gNumberOfStaticSlots;
}

int NetworkParametersCalculator::getGNumberOfMinislots() {
    return gNumberOfMinislots;
}

int NetworkParametersCalculator::getGPayloadLengthStatic() {
    return gPayloadLengthStatic;
}

int NetworkParametersCalculator::getGdCycle() {
    return gdCycle;
}

int NetworkParametersCalculator::getGdMicroPerCycle() {
    return gdMicroPerCycle;
}

float NetworkParametersCalculator::getGdMacroTick() {
    return gdMacroTick;
}

int NetworkParametersCalculator::getPdDriftMax() {
    return pdMaxDrift;
}

int NetworkParametersCalculator::getGOffsetCorrectionStart() {
    return gOffsetCorrectionStart;
}

int NetworkParametersCalculator::getPRateCorrectionOut() {
    return pRateCorrectionOut;
}

int NetworkParametersCalculator::getPdListenTimeout() {
    return pdListenTimeout;
}

int NetworkParametersCalculator::getPMicroInitialOffset() {
    return pMicroInitialOffset;
}

int NetworkParametersCalculator::getPMacroInitialOffset() {
    return pMacroInitialOffset;
}

int NetworkParametersCalculator::getPOffsetCorrectionOut() {
    return pOffsetCorrectionOut;
}