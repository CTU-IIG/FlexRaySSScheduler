/// \file Analysis.cpp The input instance analyzer
#include "MVFRScheduler.h"


void FRAnalyzer(DataStructure *dataStructure) {
    int i, j;
    int ***analyticalMatrix = new int **[dataStructure->nodeCount];
    char *usedLengths = new char[dataStructure->slotLength]();
    FILE *outputFile;

    if (dataStructure->saveResultToFile) {
        outputFile = fopen(dataStructure->outputFile, "w");
    }
    else {
        outputFile = stdout;
    }

    if (outputFile == NULL) {
        outputFile = stdout;
        printf("Output file not found. It was set to standard output");
    }
    for (i = 0; i < dataStructure->nodeCount; i++) {
        analyticalMatrix[i] = new int *[dataStructure->slotLength];
        for (j = 0; j < dataStructure->slotLength; j++) {
            analyticalMatrix[i][j] = new int[7]();
        }
    }

    for (i = 0; i < dataStructure->signalsCount; i++) {
        int period = (dataStructure->hyperPeriod / dataStructure->signalPeriods[i]);
        switch (period) {
            case 1:
                period = 6;
                break;
            case 2:
                period = 5;
                break;
            case 4:
                period = 4;
                break;
            case 8:
                period = 3;
                break;
            case 16:
                period = 2;
                break;
            case 32:
                period = 1;
                break;
            case 64:
                period = 0;
                break;
            default:
                period = 0;
                break;
        }
        analyticalMatrix[dataStructure->signalNodeIDs[i] - 1][dataStructure->signalLengths[i] - 1][period]++;
        //if((dataStructure->signalLengths[i] == 1)||(dataStructure->signalLengths[i] == 2)||(dataStructure->signalLengths[i] == 4)||(dataStructure->signalLengths[i] == 8)||
        //   (dataStructure->signalLengths[i] == 16)||(dataStructure->signalLengths[i] == 32))
        usedLengths[dataStructure->signalLengths[i] - 1] = 1;
    }

    for (i = 0; i < dataStructure->nodeCount; i++) {
        fprintf(outputFile, "ECU %d:\n", i + 1);
        fprintf(outputFile, "      1   2   4   8  16  32  64\n");
        fprintf(outputFile, "   ----------------------------\n");

        for (j = 0; j < dataStructure->slotLength; j++) {
            if (usedLengths[j] != 0) {
                fprintf(outputFile, "%2d: %3d %3d %3d %3d %3d %3d %3d\n", j + 1, analyticalMatrix[i][j][6],
                        analyticalMatrix[i][j][5], analyticalMatrix[i][j][4],
                        analyticalMatrix[i][j][3], analyticalMatrix[i][j][2], analyticalMatrix[i][j][1],
                        analyticalMatrix[i][j][0]);
            }
            delete[] analyticalMatrix[i][j];
        }
        fprintf(outputFile, "\n");
        delete[] analyticalMatrix[i];
    }
    delete[] analyticalMatrix;
    delete[] usedLengths;

    if (outputFile != stdout)
        fclose(outputFile);
    return;
}
