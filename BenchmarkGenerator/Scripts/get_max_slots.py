import argparse
import math
import os
from functools import reduce

def CalculateParameters(bandwidth, slotPayload, cycleLength, gNumberOfStaticSlots, outputFile):
    gdCycle = cycleLength * 1000
    cChannelIdleDelimiter = 11
    aMicroPerMacroNom = 40
    adActionPointDifference = 1
    adOffsetCorrection = 9
    NITAbout = 40
    cdFSS = 1
    cdFES = 2
    cdCAS = 30
    dBDRxai = 0.4
    gdMacrotick = 1 if bandwidth > 2500 else 2
    pDecodingCorrection = 48
    pDelayCompensationA = 1
    gdMinPropagationDelay = 0
    gdMaxPropagationDelay = 0.4
    gdBitMax = 0.10015
    cClockDeviationMax = 0.0015
    gAssumedPrecision = 2.65
    gOffsetCorrectionMax = 4.406
    pdMicrotick = 0.025
    gdMacroTick = 1.0
    gdTSSTransmitter = 9
    gdActionPointOffset = 4

    gdMinislotActionPointOffset = int(math.ceil((gAssumedPrecision - gdMinPropagationDelay) / (gdMacroTick * (1 - cClockDeviationMax))))
    gPayloadLengthStatic = int(math.ceil(slotPayload / 16.0))
    aFrameLengthStatic = gdTSSTransmitter + cdFSS + 80 + gPayloadLengthStatic * 20 + cdFES

    gdStaticSlot = 2 * gdActionPointOffset + int(math.ceil(((aFrameLengthStatic + cChannelIdleDelimiter) * gdBitMax + gdMinPropagationDelay + gdMaxPropagationDelay) / (gdMacroTick * (1 - cClockDeviationMax))))
    gdMiniSlot = gdMinislotActionPointOffset + int(math.ceil((gdMaxPropagationDelay + gAssumedPrecision) / (gdMacroTick * (1 - cClockDeviationMax))))
    gdSymbolWindow = 15 * (2 * gdActionPointOffset + int(math.ceil(((gdTSSTransmitter + cdCAS + cChannelIdleDelimiter) * gdBitMax + dBDRxai + gdMinPropagationDelay + gdMaxPropagationDelay) / (gdMacrotick * (1 - cClockDeviationMax)))))
    gdMacroPerCycle = int(math.floor(gdCycle / float(gdMacroTick)))
    gOffsetCorrectionStart = gdMacroPerCycle - adOffsetCorrection
    gNumberOfMinislots = int(math.ceil((gdMacroPerCycle - NITAbout - gNumberOfStaticSlots * gdStaticSlot - gdSymbolWindow) / float(gdMiniSlot)))
    gdNIT = gdMacroPerCycle - gdStaticSlot * gNumberOfStaticSlots - gdMiniSlot * gNumberOfMinislots - gdSymbolWindow - adActionPointDifference
    gdMicroPerCycle = gdMacroPerCycle * aMicroPerMacroNom
    pdMaxDrift = int(math.ceil(gdMicroPerCycle * 2 * cClockDeviationMax / (1 - cClockDeviationMax)))
    pRateCorrectionOut = int(math.ceil((gdMicroPerCycle * 2 * cClockDeviationMax) / (1 - cClockDeviationMax)))
    pdListenTimeout = 2 * (gdMicroPerCycle + pdMaxDrift)
    pMicroInitialOffset =(aMicroPerMacroNom - ((pDecodingCorrection + pDelayCompensationA) % aMicroPerMacroNom)) % aMicroPerMacroNom
    pMacroInitialOffset = gdActionPointOffset + int(math.ceil((pDecodingCorrection + pDelayCompensationA) / float(aMicroPerMacroNom)))
    pOffsetCorrectionOut = int(math.ceil(gOffsetCorrectionMax / (pdMicrotick * (1 - cClockDeviationMax))))
    max_static_slots = int((gNumberOfMinislots * gdMiniSlot)/gdStaticSlot) + gNumberOfStaticSlots
    if outputFile == "None":
        print(max_static_slots)
    else:
        with open(outputFile, "a") as f:
            f.write("{};\n".format(max_static_slots))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-b", help="Bandwidth [kbit/s]", type=int, default=10000)
    parser.add_argument("-s", help="Slot payload [bits]", type=int, default=64)
    parser.add_argument("-c", help="Length of cycle [ms]", type=int, default=64)
    parser.add_argument("-sc", help="Number of static slots [-]", type=int, default=1)
    parser.add_argument("-o", help="Outputfile", type=str, default="None")
    
    args = parser.parse_args()

    CalculateParameters(args.b, args.s, args.c, args.sc, args.o)
