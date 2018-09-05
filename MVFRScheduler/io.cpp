/// \file io.cpp Methods for input and output operations
#include "MVFRScheduler.h"

int parseInput(int argc, char *argv[], DataStructure *dataStructure) {
    int i;
    dataStructure->saveResultToFile = 0;
    dataStructure->appendToCSV = 0;
    dataStructure->algorithm = TWOSTAGE;
    dataStructure->isIncOptimized = 0;
    static FirstFitParameters firstFitParameters;
    static GeneticParameters geneticParameters;
    static StageParameters stageParameters;
    static IndependentParameters independentParameters;
    static ExporterParameters exporterParameters;
    geneticParameters.populationSize = 9;
    geneticParameters.maxGenerationCount = 100;
    stageParameters.signalAssigningThreshold = INT_MAX;
    dataStructure->isReceivers = FALSE;
    if (argc < 2)
        error("No input file set - It is necessary to determine at least input file!");
    else {
        for (i = 1; i < argc; i++) {
            if (i != argc - 1) {
                switch (argv[i][0]) {
                    case '-':
                        switch (argv[i][1]) {
                            case 'v':
                                switch (argv[i][2]) {
                                    case '1':
                                        verboseAllocInfo = 0;
                                        verboseLevel = 1;
                                        break;
                                    case '2':
                                        verboseAllocInfo = 1;
                                        verboseLevel = 2;
                                        break;
                                    case '3':
                                        verboseAllocInfo = 3;
                                        verboseLevel = 3;
                                        break;
                                    default :
                                        verboseAllocInfo = 0;
                                        verboseLevel = 0;
                                        break;
                                }
                                break;
                            case 'o':
                                dataStructure->saveResultToFile = 1;
                                dataStructure->outputFile = argv[i + 1];
                                i++;
                                break;
                            case 't':
                                if (argv[i][2] == 0)
                                    dataStructure->isIncOptimized = 1;
                                else {
                                    if (argv[i][2] == 's') {
                                        dataStructure->isIncOptimized = 1;
                                    }
                                    else {
                                        dataStructure->isIncOptimized = 2;
                                    }
                                }
                                break;
                            case 'a':
                                switch (argv[i][2]) {
                                    case 's':
                                        dataStructure->algorithm = TWOSTAGE;
                                        dataStructure->schedulerParameters = &stageParameters;
                                        break;
                                    case 'v':
                                        dataStructure->schedulerParameters = &firstFitParameters;
                                        dataStructure->algorithm = FIRSTFITVARIANT;
                                        switch (argv[i][3]) {
                                            case 0:
                                                firstFitParameters.sortType = FFNONE;
                                                break;
                                            case 'p':
                                                firstFitParameters.sortType = FFPERIOD;
                                                break;
                                            case 'w':
                                                firstFitParameters.sortType = FFWINDOW;
                                                break;
                                            case 'l':
                                                firstFitParameters.sortType = FFLENGTHDEC;
                                                break;
                                            case 'a':
                                                firstFitParameters.sortType = FFALL;
                                                break;
                                            default:
                                                error("The options are not supported!");
                                                break;
                                        }
                                        break;
                                    case 'c':
                                        dataStructure->schedulerParameters = &firstFitParameters;
                                        dataStructure->algorithm = FIRSTFITCOLOR;
                                        switch (argv[i][3]) {
                                            case 0:
                                                firstFitParameters.sortType = FFNONE;
                                                break;
                                            case 'p':
                                                firstFitParameters.sortType = FFPERIOD;
                                                break;
                                            case 'w':
                                                firstFitParameters.sortType = FFWINDOW;
                                                break;
                                            case 'l':
                                                firstFitParameters.sortType = FFLENGTHDEC;
                                                break;
                                            case 'a':
                                                firstFitParameters.sortType = FFALL;
                                                break;
                                            default:
                                                error("The options are not supported!");
                                                break;
                                        }
                                        break;
                                    case 'h':
                                        dataStructure->algorithm = CHANNELASSIGNER;
                                        dataStructure->schedulerParameters = &firstFitParameters;
                                        firstFitParameters.sortType = FFALL;
                                        break;
                                    case 'b':
                                        dataStructure->schedulerParameters = &firstFitParameters;
                                        dataStructure->algorithm = BESTFIT;
                                        switch (argv[i][3]) {
                                            case 0:
                                                firstFitParameters.sortType = FFNONE;
                                                break;
                                            case 'p':
                                                firstFitParameters.sortType = FFPERIOD;
                                                break;
                                            case 'w':
                                                firstFitParameters.sortType = FFWINDOW;
                                                break;
                                            case 'l':
                                                firstFitParameters.sortType = FFLENGTHDEC;
                                                break;
                                            case 'a':
                                                firstFitParameters.sortType = FFALL;
                                                break;
                                            default:
                                                error("The options are not supported!");
                                                break;
                                        }
                                        break;
                                    case 'f':
                                        dataStructure->schedulerParameters = &firstFitParameters;
                                        dataStructure->algorithm = FIRSTFIT;
                                        switch (argv[i][3]) {
                                            case 0:
                                                firstFitParameters.sortType = FFNONE;
                                                break;
                                            case 'p':
                                                firstFitParameters.sortType = FFPERIOD;
                                                break;
                                            case 'w':
                                                firstFitParameters.sortType = FFWINDOW;
                                                break;
                                            case 'l':
                                                firstFitParameters.sortType = FFLENGTHDEC;
                                                break;
                                            case 'a':
                                                firstFitParameters.sortType = FFALL;
                                                break;
                                            default:
                                                error("The options are not supported!");
                                                break;
                                        }
                                        break;
                                    case 'g':
                                        dataStructure->algorithm = GENETIC;
                                        dataStructure->schedulerParameters = &geneticParameters;
                                        break;
                                    case 'l':
                                        switch (argv[i][3]) {
                                            case 'i':
                                                dataStructure->algorithm = LOWERBOUNDINDEPENDENT;
                                                break;
                                            case 'm':
                                                dataStructure->algorithm = LOWERBOUND;
                                                break;
                                            default:
                                                dataStructure->algorithm = LOWERBOUND;
                                                error("The options are not supported! Chose algorithm for the Lowerbound of multivariant schedules.");
                                        }
                                        dataStructure->schedulerParameters = nullptr;
                                        break;
                                    case 'i':
                                        dataStructure->algorithm = INCREMENTAL;
                                        firstFitParameters.sortType = FFNONE;
                                        dataStructure->schedulerParameters = &firstFitParameters;
                                        readOldRoaster(argv[i + 1], dataStructure);
                                        i++;
                                        break;
                                    case 'e':
                                        dataStructure->algorithm = EXPORT;
                                        switch (argv[i][3]) {
                                            case 'c':
                                                exporterParameters.exportType = ExporterEngineType::CPP_FILE;
                                                break;
                                            case 'r':
                                                exporterParameters.exportType = ExporterEngineType::RPP_CONFIG;
                                            case 0:
                                            default:
                                                exporterParameters.exportType = ExporterEngineType::RPP_CONFIG;
                                                break;
                                        }
                                        exporterParameters.variantID = atoi(argv[i + 1]);
                                        dataStructure->schedulerParameters = &exporterParameters;
                                        readOldRoaster(argv[i + 2], dataStructure);
                                        i = i + 2;
                                        break;
                                    case 'x':
                                        dataStructure->algorithm = FIBEX;
                                        exporterParameters.variantID = atoi(argv[i + 1]);
                                        dataStructure->schedulerParameters = &exporterParameters;
                                        readOldRoaster(argv[i + 2], dataStructure);
                                        i = i + 2;
                                        break;
                                    case 'd':
                                        dataStructure->algorithm = INDEPENDENT;
                                        switch (argv[i][3]) {
                                            case 0:
                                                independentParameters.sortType = FFNONE;
                                                break;
                                            case 'p':
                                                independentParameters.sortType = FFPERIOD;
                                                break;
                                            case 'w':
                                                independentParameters.sortType = FFWINDOW;
                                                break;
                                            case 'l':
                                                independentParameters.sortType = FFLENGTHDEC;
                                                break;
                                            case 'a':
                                                independentParameters.sortType = FFALL;
                                                break;
                                            default:
                                                error("The options are not supported!");
                                                break;
                                        }
                                        if (argc > i + 2) {
                                            independentParameters.saveResultToFile = argv[i + 1];
                                            i++;
                                        }
                                        else
                                            independentParameters.saveResultToFile = nullptr;
                                        dataStructure->schedulerParameters = &independentParameters;
                                        break;
                                    case 'y':
                                        dataStructure->algorithm = ANALYSIS;
                                        break;
                                    case 't':
                                        dataStructure->algorithm = GANTT;
                                        exporterParameters.variantID = atoi(argv[i + 1]);
                                        dataStructure->schedulerParameters = &exporterParameters;
                                        readOldRoaster(argv[i + 2], dataStructure);
                                        i = i + 2;
                                        break;
                                    case 'w':
                                        dataStructure->algorithm = VOLUME;
                                        break;
                                    case 'o':
                                        dataStructure->algorithm = COMMON;
                                        dataStructure->schedulerParameters = &firstFitParameters;
                                        firstFitParameters.sortType = FFALL;
                                        break;

                                    default:
                                        error("The options are not supported!");
                                        break;
                                }
                                break;
                            case 'c':
                                dataStructure->appendToCSV = 1;
                                dataStructure->csvFile = argv[i + 1];
                                if (argv[i][2] == 'b') dataStructure->appendToCSV = 2;
                                i++;
                                break;
                            case 'p':
                                switch (argv[i][2]) {
                                    case 's':
                                        stageParameters.signalAssigningThreshold = atoi(argv[i + 1]);
                                        i++;
                                        break;
                                    case 'g':
                                        switch (argv[i][3]) {
                                            case 'p':
                                                geneticParameters.populationSize = atoi(argv[i + 1]);
                                                i++;
                                                break;
                                            case 'g':
                                                geneticParameters.maxGenerationCount = atoi(argv[i + 1]);
                                            default:
                                                error("The options are not supported!");
                                                break;
                                        }
                                    default:
                                        error("The options are not supported!");
                                        break;
                                }
                                break;
                            case 'r':
                                dataStructure->isReceivers = TRUE;
                                break;
                            default:
                                error("The options are not supported!");
                                break;
                        }
                        break;
                    default:
                        error("The options are not supported!");
                        break;
                }
            }
            else {
                if (verboseLevel > 0)
                    printf("Reading input file\n");
                return readInputFile(argv[i], dataStructure);
            }
        }
    }
    printf("Input parameters are formulated incorrectly!\n");
    return 0;
}

int readOldRoaster(char *fileName, DataStructure *dataStructure) {
    int tmp;
    std::fstream fp;
    fp.open(fileName, std::ios::in | std::ios::binary);
    //FILE* inputFile = fopen(fileName, "r");
    std::vector<int> buffer;

    //if(inputFile == NULL)
    if (!fp.is_open()) {
        error("Original schedule file does not exist!");
        return 0;
    }
    while (fp >> tmp) {
        //tmp2 = fscanf(inputFile, "%d", &tmp);
        if (fp.fail()) {
            error("Error occured during original schedule reading!");
            //fclose(inputFile);
            fp.close();
            return 0;
        }
        buffer.push_back(tmp);
    }
    //fclose(inputFile);
    fp.close();
    if (buffer.size() % 3 != 0) {
        error("Old schedule file has a wrong structure!");
        return 0;
    }
    dataStructure->oldSchedule.signalsCount = static_cast<int>(buffer.size() / 3);
    dataStructure->oldSchedule.signalInFrameOffsets = static_cast<int *>(mmalloc(buffer.size() / 3 * sizeof(int)));
    dataStructure->oldSchedule.signalSlot = static_cast<int *>(mmalloc(buffer.size() / 3 * sizeof(int)));
    dataStructure->oldSchedule.signalStartCycle = static_cast<int *>(mmalloc(buffer.size() / 3 * sizeof(int)));
    for (tmp = 0; tmp < static_cast<int>(buffer.size()); tmp++) {
        switch ((tmp * 3) / buffer.size()) {
            case 0:
                dataStructure->oldSchedule.signalSlot[tmp % (buffer.size() / 3)] = buffer[tmp];
                break;
            case 1:
                dataStructure->oldSchedule.signalStartCycle[tmp % (buffer.size() / 3)] = buffer[tmp];
                break;
            case 2:
                dataStructure->oldSchedule.signalInFrameOffsets[tmp % (buffer.size() / 3)] = buffer[tmp];
                break;
            default:
                error("Algorithm error!!!");
        }
    }
    return 0;
}

int readInputFile(char *fileName, DataStructure *dataStructure) {
    int i, j, k;
    char exception = 0;
    FILE *inputFile;
    char word[50];
    dataStructure->signalNames = nullptr;
    inputFile = fopen(fileName, "r");
    if (inputFile == NULL) {
        error("Input file does not exist!");
        return 0;
    }
    while (1) {
        fscanf(inputFile, "%d", &(dataStructure->signalsCount)); // Read the number of signals
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }
        fscanf(inputFile, "%d", &(dataStructure->cycleLength)); // Read the duration of cycle
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }
        if (dataStructure->cycleLength == 0) {
            error("Length of the cycle is not allowed to be zero!");
            exception = 1;
            break;
        }
        fscanf(inputFile, "%d", &(dataStructure->slotLength)); // Read the length if static slot
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }
        fscanf(inputFile, "%d", &(dataStructure->variantCount)); // Read the number of variants
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }
        fscanf(inputFile, "%d", &(dataStructure->nodeCount)); // Read the number of nodes
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read the number of signals per node
        dataStructure->signalsInNodeCounts = static_cast<int *>(mmalloc(sizeof(int) * dataStructure->nodeCount));
        for (i = 0; i < dataStructure->nodeCount; i++)
            fscanf(inputFile, "%d", &(dataStructure->signalsInNodeCounts[i]));
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read the node assignment for signal
        dataStructure->signalNodeIDs = static_cast<int *>(mmalloc(sizeof(int) * dataStructure->signalsCount));
        for (i = 0; i < dataStructure->signalsCount; i++)
            fscanf(inputFile, "%d", &(dataStructure->signalNodeIDs[i]));
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read the signal names
        dataStructure->signalNames = static_cast<char **>(mmalloc(sizeof(char *) * dataStructure->signalsCount));
        for (i = 0; i < dataStructure->signalsCount; i++) {
            fscanf(inputFile, "%s", word);
            dataStructure->signalNames[i] = static_cast<char *>(mmalloc((strlen(word) + 1) * sizeof(char)));
            strcpy(dataStructure->signalNames[i], word);
        }
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read signal periods
        dataStructure->signalPeriods = static_cast<int *>(mmalloc(sizeof(int) * dataStructure->signalsCount));
        dataStructure->hyperPeriod = 0;
        for (i = 0; i < dataStructure->signalsCount; i++) {
            fscanf(inputFile, "%d", &(dataStructure->signalPeriods[i]));
            if (dataStructure->signalPeriods[i] > 64 * dataStructure->cycleLength)
                dataStructure->signalPeriods[i] = 64 * dataStructure->cycleLength;
            if (dataStructure->signalPeriods[i] > dataStructure->hyperPeriod)
                dataStructure->hyperPeriod = dataStructure->signalPeriods[i];
        }
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read release times of signals
        dataStructure->signalReleaseDates = static_cast<int *>(mmalloc(sizeof(int) * dataStructure->signalsCount));
        for (i = 0; i < dataStructure->signalsCount; i++)
            fscanf(inputFile, "%d", &(dataStructure->signalReleaseDates[i]));
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read deadlines of signals
        dataStructure->signalDeadlines = static_cast<int *>(mmalloc(sizeof(int) * dataStructure->signalsCount));
        for (i = 0; i < dataStructure->signalsCount; i++)
            fscanf(inputFile, "%d", &(dataStructure->signalDeadlines[i]));
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read the number of bits of signals
        dataStructure->signalLengths = static_cast<int *>(mmalloc(sizeof(int) * dataStructure->signalsCount));
        dataStructure->incoStatistics.maximalPayload = 0;
        for (i = 0; i < dataStructure->signalsCount; i++) {
            fscanf(inputFile, "%d", &(dataStructure->signalLengths[i]));
            if (dataStructure->signalLengths[i] > dataStructure->incoStatistics.maximalPayload)
                dataStructure->incoStatistics.maximalPayload = dataStructure->signalLengths[i];
        }
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read the model variants
        dataStructure->modelVariants = static_cast<char **>(mmalloc(
                sizeof(char *) * dataStructure->variantCount)); // Temporal array for variants reading
        int *signalCountsInVariants = static_cast<int *>(ccalloc(dataStructure->variantCount,
                                                                 sizeof(int))); // Counting number of signals is particular variants
        int inputChar;
        for (i = 0; i < dataStructure->variantCount; i++) {
            dataStructure->modelVariants[i] = static_cast<char *>(mmalloc(sizeof(char) * dataStructure->signalsCount));
            for (j = 0; j < dataStructure->signalsCount; j++) {
                fscanf(inputFile, "%d", &inputChar);
                dataStructure->modelVariants[i][j] = static_cast<char>((dataStructure->algorithm == COMMON) ? 1
                                                                                                            : inputChar);
                if (dataStructure->modelVariants[i][j]) signalCountsInVariants[i]++;
            }
        }
        if (ferror(inputFile) || (feof(inputFile) && !dataStructure->isReceivers)) {
            exception = 1;
            break;
        }

        // Na�ten� ��sti pro ur�en� kret� ECU dan� zpr�vy �tou
        if (dataStructure->isReceivers == TRUE) {
            dataStructure->nodesChannel = static_cast<char *>(ccalloc(dataStructure->nodeCount, sizeof(char)));
            dataStructure->nodesOnBothChannels = static_cast<char *>(ccalloc(dataStructure->nodeCount, sizeof(char)));
            dataStructure->signalReceivers = new std::vector<int>[dataStructure->signalsCount]();
            dataStructure->signalFaultTolerant.resize(static_cast<unsigned int>(dataStructure->signalsCount), 0);
            int ABNodeCount = 0;
            fscanf(inputFile, "%d", &ABNodeCount);
            for (i = 0; i < ABNodeCount; i++) {
                int ABNode;
                fscanf(inputFile, "%d", &ABNode);
                dataStructure->nodesOnBothChannels[ABNode - 1] = 1;
            }
            for (i = 0; i <
                        dataStructure->signalsCount; i++) // All nodes which has some Fault tolerant signals must be connected to both channels.
            {
                if (dataStructure->signalFaultTolerant[i] == 1)
                    dataStructure->nodesOnBothChannels[dataStructure->signalNodeIDs[i] - 1] = 1;
            }

            for (i = 0; i < dataStructure->signalsCount; i++) {
                int recCount, value;
                fscanf(inputFile, "%d", &recCount);
                for (j = 0; j < recCount; j++) {
                    fscanf(inputFile, "%d", &value);
                    dataStructure->signalReceivers[i].push_back(value);
                }
            }

            for (i = 0; i < dataStructure->signalsCount; i++) {
                int numReturned, value;
                numReturned = fscanf(inputFile, "%d", &value);
                if (numReturned == 0 || ferror(inputFile) || feof(inputFile)) {
                    clearerr(inputFile);
                    break;
                }
                if (value == 0)
                    dataStructure->signalFaultTolerant[i] = 0;
                else
                    dataStructure->signalFaultTolerant[i] = 1;
            }
        }

        if (ferror(inputFile)) {
            exception = 1;
            break;
        }

        //Create mutual exclusion matrix
        dataStructure->mutualExclusionMatrix = static_cast<char *>(ccalloc(
                dataStructure->signalsCount * (dataStructure->signalsCount + 1) / 2, sizeof(char)));
        dataStructure->mutualExclusionNodeMatrix = static_cast<char *>(ccalloc(
                dataStructure->nodeCount * (dataStructure->nodeCount + 1) / 2, sizeof(char)));
        int **shortedModelVariants = static_cast<int **>(mmalloc(sizeof(int *) * dataStructure->variantCount));
        int counter;
        for (i = 0; i < dataStructure->variantCount; i++) {
            shortedModelVariants[i] = static_cast<int *>(mmalloc(sizeof(int) * signalCountsInVariants[i]));
            counter = 0;
            for (j = 0; j < dataStructure->signalsCount; j++) {
                if (dataStructure->modelVariants[i][j]) {
                    shortedModelVariants[i][counter] = j;
                    counter++;
                }
            }
        }
        for (i = 0; i < dataStructure->variantCount; i++) {
            for (j = 0; j < signalCountsInVariants[i]; j++) {
                for (k = j + 1; k < signalCountsInVariants[i]; k++) {
                    // mutual exclusion metrix: 1 - there is confict, 0 - there is no conflict
                    setMEMatrixValue(dataStructure->mutualExclusionMatrix, shortedModelVariants[i][j],
                                     shortedModelVariants[i][k], 1);
                    setMEMatrixValue(dataStructure->mutualExclusionNodeMatrix,
                                     dataStructure->signalNodeIDs[shortedModelVariants[i][j]] - 1,
                                     dataStructure->signalNodeIDs[shortedModelVariants[i][k]] - 1, 1);
                }
            }
        }

        // Set up statistics for incremental scheduling
        dataStructure->incoStatistics.periodSignalCount = static_cast<int *>(ccalloc(sizeof(int), 7));
        dataStructure->incoStatistics.payloadSignalCount = static_cast<int *>(ccalloc(sizeof(int),
                                                                                      dataStructure->incoStatistics.maximalPayload));
        for (i = 0; i < dataStructure->signalsCount; i++) {
            dataStructure->incoStatistics.payloadSignalCount[dataStructure->signalLengths[i] - 1]++;
            switch (dataStructure->signalPeriods[i] / dataStructure->cycleLength) {
                case 1:
                    dataStructure->incoStatistics.periodSignalCount[0]++;
                    break;
                case 2:
                    dataStructure->incoStatistics.periodSignalCount[1]++;
                    break;
                case 4:
                    dataStructure->incoStatistics.periodSignalCount[2]++;
                    break;
                case 8:
                    dataStructure->incoStatistics.periodSignalCount[3]++;
                    break;
                case 16:
                    dataStructure->incoStatistics.periodSignalCount[4]++;
                    break;
                case 32:
                    dataStructure->incoStatistics.periodSignalCount[5]++;
                    break;
                case 64:
                    dataStructure->incoStatistics.periodSignalCount[6]++;
                    break;
                default:
                    printf("Some signal pariods are too high: signal %d, period %d\n", i,
                           dataStructure->signalPeriods[i]);
            }
        }
        for (i = 0; i <
                    7; i++) // Přepočet, protože můžeme do prostoru pro signál s periodou 1 vložit i signály s periodou 2 atd.
        {
            for (j = i + 1; j < 7; j++) {
                dataStructure->incoStatistics.periodSignalCount[i] += dataStructure->incoStatistics.periodSignalCount[j];
            }
            dataStructure->incoStatistics.periodSignalCount[i] /= static_cast<int>(pow(static_cast<float>(2),
                                                                                       static_cast<float>(i)));
        }

        for (i = 0; i < dataStructure->variantCount; i++) {
            ffree(shortedModelVariants[i]);
        }
        ffree(shortedModelVariants);
        ffree(signalCountsInVariants);
        break;
    }
    dataStructure->signalSlot =
            static_cast<int *>(ccalloc(static_cast<unsigned long>(dataStructure->signalsCount), sizeof(int)));
    dataStructure->signalStartCycle =
            static_cast<int *>(ccalloc(static_cast<unsigned long>(dataStructure->signalsCount), sizeof(int)));
    dataStructure->signalInFrameOffsets =
            static_cast<int *>(ccalloc(static_cast<unsigned long>(dataStructure->signalsCount), sizeof(int)));
    if (ferror(inputFile) || feof(inputFile) || exception) {
        error("Error raised during reading input file!");
        return 0;
    }
    fclose(inputFile);
    return 1;
}

int saveToOutputFile(DataStructure *dataStructure) {
    int i;
    FILE *outputFile;

    outputFile = fopen(dataStructure->outputFile, "w");
    if (outputFile != NULL) {
        for (i = 0; i < dataStructure->signalsCount - 1; i++) {
            fprintf(outputFile, "%d ", dataStructure->signalSlot[i]);
        }
        fprintf(outputFile, "%d\n", dataStructure->signalSlot[dataStructure->signalsCount - 1]);
        for (i = 0; i < dataStructure->signalsCount - 1; i++) {
            fprintf(outputFile, "%d ", dataStructure->signalStartCycle[i]);
        }
        fprintf(outputFile, "%d\n", dataStructure->signalStartCycle[dataStructure->signalsCount - 1]);
        for (i = 0; i < dataStructure->signalsCount - 1; i++) {
            fprintf(outputFile, "%d ", dataStructure->signalInFrameOffsets[i]);
        }
        fprintf(outputFile, "%d", dataStructure->signalInFrameOffsets[dataStructure->signalsCount - 1]);

        if (ferror(outputFile) || feof(outputFile)) {
            error("Error raised while writing to ouput file!");
            return 0;
        }
        fclose(outputFile);
    }
    else {
        printf("%d\n", errno);
        error("It is not possible to open output file!\n");
        return 0;
    }
    return 1;
}

int AppendToCSV(const DataStructure *dataStructure, int value) {
    FILE *outputCSV;
    outputCSV = fopen(dataStructure->csvFile, "a");

    if (outputCSV != NULL) {
        fprintf(outputCSV, "%d; ", value);
        fclose(outputCSV);
    }
    else {
        fclose(outputCSV);
        printf("%d\n", errno);
        error("It is not possible to open output cvs file!\n");
        return 0;
    }
    return 1;
}

int AppendNLToCSV(const DataStructure *dataStructure) {
    FILE *outputCSV;
    outputCSV = fopen(dataStructure->csvFile, "a");

    if (outputCSV != NULL) {
        fprintf(outputCSV, "\n");
        fclose(outputCSV);
    }
    else {
        fclose(outputCSV);
        printf("%d\n", errno);
        error("It is not possible to open output cvs file!\n");
        return 0;
    }
    return 1;
}

int AppendToCSV(const DataStructure *dataStructure, float value) {
    FILE *outputCSV;
    outputCSV = fopen(dataStructure->csvFile, "a");

    if (outputCSV != NULL) {
        fprintf(outputCSV, "%.31f; ", value);
        fclose(outputCSV);
    }
    else {
        fclose(outputCSV);
        printf("%d\n", errno);
        error("It is not possible to open output cvs file!\n");
        return 0;
    }
    return 1;
}


int AppendResultsToCSV(DataStructure *dataStructure) {
    FILE *outputCSV;
    outputCSV = fopen(dataStructure->csvFile, "a");

    if (outputCSV != NULL) {
        if (dataStructure->appendToCSV == 1)
            fprintf(outputCSV, "%d; %.3lf; ", dataStructure->maxSlot, dataStructure->elapsedTime);
        else
            fprintf(outputCSV, "%d; %.3lf\n", dataStructure->maxSlot, dataStructure->elapsedTime);
        fclose(outputCSV);
    }
    else {
        printf("%d\n", errno);
        error("It is not possible to open output cvs file!\n");
        return 0;
    }
    return 1;
}

int AppendIndependentTestResults(int multiVariant, int independent, char *fileName) {
    FILE *outputFile;
    outputFile = fopen(fileName, "a");

    if (outputFile != NULL) {
        fprintf(outputFile, "%d; %d; ", independent, multiVariant);
        fclose(outputFile);
    }
    else {
        printf("%d\n", errno);
        error("It is not possible to open output cvs file!\n");
        return 0;
    }
    return 1;
}
