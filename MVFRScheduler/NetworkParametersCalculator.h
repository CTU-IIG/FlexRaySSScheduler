//
// Created by jenik on 11/2/15.
//

#ifndef MVFRSCHEDULER_NETWORKPARAMETERSCALCULATOR_H
#define MVFRSCHEDULER_NETWORKPARAMETERSCALCULATOR_H


class NetworkParametersCalculator {
private:
    int bandwidth;
    int slotPayload; // Slot payload
    int gdCycle; // Cycle length
    int gNumberOfStaticSlots; // Slot count
    int gNumberOfMinislots;
    int gPayloadLengthStatic;
    int gOffsetCorrectionStart;

    int gdStaticSlot;
    int gdMiniSlot;
    int gdNIT; // 2 - 805
    int gdSymbolWindow;
    int gdMacroPerCycle;
    int gdActionPointOffset;
    int gdMinislotActionPointOffset;
    int gdTSSTransmitter;
    int gdMicroPerCycle;
    float gdMacroTick;
    int pdMaxDrift;
    int pRateCorrectionOut;
    int pdListenTimeout;
    int pMicroInitialOffset;
    int pMacroInitialOffset;
    int pOffsetCorrectionOut;

    void CalculateParameters();

public:
    NetworkParametersCalculator(const int Bandwidth, const int SlotPayload, const int CycleLength, const int SlotCount);

    int getGdStaticSlot();

    int getGdMiniSlot();

    int getGdNIT();

    int getGdSymbolWindow();

    int getGdMacroPerCycle();

    int getGdActionPointOffset();

    int getGdMinislotActionPointOffset();

    int getGdTSSTransmitter();

    int getGNumberOfStaticSlots();

    int getGNumberOfMinislots();

    int getGPayloadLengthStatic();

    int getGdCycle();

    int getGdMicroPerCycle();

    float getGdMacroTick();

    int getPdDriftMax();

    int getGOffsetCorrectionStart();

    int getPRateCorrectionOut();

    int getPdListenTimeout();

    int getPMicroInitialOffset();

    int getPMacroInitialOffset();

    int getPOffsetCorrectionOut();
};


#endif //MVFRSCHEDULER_NETWORKPARAMETERSCALCULATOR_H
