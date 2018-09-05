/// \file ScheduleChecker.cpp Code for schedule checker - scheduler error detector
#include "MVFRScheduler.h"

int CheckSchedule(DataStructure *dataStructure) {
    int i, j, k, l;
    int *pByte;
    int result = 0;
    if (dataStructure->maxSlot == -1)
        return 0;
    int ***schedule = (int ***) mmalloc(dataStructure->variantCount * sizeof(int **));
    for (i = 0; i < dataStructure->variantCount; i++) {
        schedule[i] = (int **) mmalloc(dataStructure->hyperPeriod * sizeof(int *));
        for (j = 0; j < dataStructure->hyperPeriod; j++) {
            schedule[i][j] = (int *) ccalloc(dataStructure->slotLength * dataStructure->maxSlot, sizeof(int));
        }
    }
    for (i = 0; i < dataStructure->signalsCount; i++) // sign�l
    {
        for (j = 0; j < dataStructure->hyperPeriod / dataStructure->signalPeriods[i]; j++) // opakov�n�
        {
            if (dataStructure->signalStartCycle[i] + dataStructure->signalPeriods[i] * j >=
                dataStructure->hyperPeriod ||
                dataStructure->signalStartCycle[i] + dataStructure->signalPeriods[i] * j < 0)
                printf("Schedule infeasible - signal %d is located in infeasible start cycle", i);
            for (k = 0; k < dataStructure->signalLengths[i]; k++) // byte sign�lu
            {
                for (l = 0; l < dataStructure->variantCount; l++) {
                    if (dataStructure->modelVariants[l][i]) {
                        pByte = &schedule[l][dataStructure->signalStartCycle[i] + dataStructure->signalPeriods[i] * j]
                        [dataStructure->signalSlot[i] * dataStructure->slotLength +
                         dataStructure->signalInFrameOffsets[i] + k];
                        if (*pByte != 0) {
                            printf("Schedule infeasible - signal: %d, in cycle %d, slot %d, offset (%d, %d), length %d, occupied already by signal %d\n",
                                   i, dataStructure->signalStartCycle[i] + dataStructure->signalPeriods[i] * j,
                                   dataStructure->signalSlot[i],
                                   dataStructure->signalInFrameOffsets[i],
                                   dataStructure->signalInFrameOffsets[i] + k, dataStructure->signalLengths[i],
                                   *pByte - 1);
                            getchar(); // Remove
                            result = 1;
                        }
                        else {
                            *pByte = i + 1;
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < dataStructure->variantCount; i++) {
        for (j = 0; j < dataStructure->hyperPeriod; j++) {
            ffree(schedule[i][j]);
        }
        ffree(schedule[i]);
    }
    ffree(schedule);
    return result;
}
