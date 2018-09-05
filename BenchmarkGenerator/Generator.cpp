///
/// \mainpage MultiVariant FlexRay Static Segment Scheduler Benchmark Instances Generator
///
/// The source code for the MultiVariant FlexRay Static Segment Scheduler Benchmark Instances Generator.
/// Resulting instances are used for MultiVariant FlexRay Static Segment Scheduler testing.
///

/// \file Generator.cpp The main source file for generating benchmark instances.
#include "Generator.h"


int readConfigFile(GDS *gds) {
    int i;
    FILE *fConfigFile;
    int readingPhase = 0;
    if (gds->configFileName == NULL) {
        printf("You have to set also a config file!");
        return FAIL;
    }

    fConfigFile = fopen(gds->configFileName, "r");
    if (fConfigFile == NULL) {
        printf("The specified config file does not exists!");
        return FAIL;
    }
    if (ferror(fConfigFile) || feof(fConfigFile)) {
        printf("There is a problem with reading of the config file!");
        return FAIL;
    }

    fscanf(fConfigFile, "%d\n", &gds->cycleLength);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    fscanf(fConfigFile, "%d\n", &gds->slotLength);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    fscanf(fConfigFile, "%d\n", &gds->signalsCount);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    gds->periodProbabilities = nnew<int>(7);
    readingPhase++;
    fscanf(fConfigFile, "%d %d %d %d %d %d %d\n", &gds->periodProbabilities[0], &gds->periodProbabilities[1],
           &gds->periodProbabilities[2], &gds->periodProbabilities[3], &gds->periodProbabilities[4],
           &gds->periodProbabilities[5], &gds->periodProbabilities[6]);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    fscanf(fConfigFile, "%d\n", &gds->maxSignalPayload);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    gds->payloadProbabilities = nnew<int>(gds->maxSignalPayload);
    readingPhase++;
    for (i = 0; i < gds->maxSignalPayload; i++)
        fscanf(fConfigFile, "%d ", &gds->payloadProbabilities[i]);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    fscanf(fConfigFile, "%d\n", &gds->releasePortion);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    fscanf(fConfigFile, "%d\n", &gds->deadlinePortion);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    fscanf(fConfigFile, "%d\n", &gds->nodeCount);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    fscanf(fConfigFile, "%d\n", &gds->variantsCount);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    fscanf(fConfigFile, "%d\n", &gds->commonNodesPortion);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    fscanf(fConfigFile, "%d\n", &gds->specificNodesPortion);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    fscanf(fConfigFile, "%d\n", &gds->commonSignalsPortion);
    if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
    fscanf(fConfigFile, "%d\n", &gds->specificSignalsPortion);
    if (gds->isReceivers) {
        if (feof(fConfigFile) || ferror(fConfigFile)) goto Error;
        gds->receiversProbabilities = nnew<int>(gds->nodeCount - 1);
        gds->benchmarkInstance.signalReceivers = nnew<std::vector<int>>(gds->signalsCount);
        gds->benchmarkInstance.nodesOnBothChannels = nnewi<char>(gds->nodeCount);
        for (i = 0; i < gds->nodeCount - 1; i++)
            fscanf(fConfigFile, "%d ", &gds->receiversProbabilities[i]);
        int tmp = 100;
        for (i = 0; i < gds->nodeCount - 1; i++) {
            gds->receiversProbabilities[i] = tmp - gds->receiversProbabilities[i];
            tmp = gds->receiversProbabilities[i];
        }
    }
    if (ferror(fConfigFile)) goto Error;
    fclose(fConfigFile);
    return SUCCESS;

    Error:
    printf("The config file is badly structured!");
    switch (readingPhase) {
        case 2:
            ddelete(gds->payloadProbabilities);
        case 1:
            ddelete(gds->periodProbabilities);
        default:
            break;
    }
    fclose(fConfigFile);
    return FAIL;
}

int readOrigBenchInstFile(GDS *gds) {
    int i, j, tmp;
    char exception = 0;
    FILE *inputFile;
    inputFile = fopen(gds->origBenchInstFileName, "r");
    if (inputFile == NULL) {
        printf("Input file does not exist!");
        return 0;
    }
    while (1) {
        fscanf(inputFile, "%d", &(gds->signalsCount)); // Read the number of signals
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }
        // Add new signals for the roastering
        gds->incrementalParameters.addedSignalsCount = rand() % (1 + gds->signalsCount * INCREM_PERC / 100);
        gds->signalsCount = gds->signalsCount + gds->incrementalParameters.addedSignalsCount;
        fscanf(inputFile, "%d", &tmp); // Read the duration of cycle
        if (ferror(inputFile) || feof(inputFile) || tmp != gds->cycleLength) {
            exception = 1;
            break;
        }
        fscanf(inputFile, "%d", &tmp); // Read the length if static slot
        if (ferror(inputFile) || feof(inputFile) || tmp != gds->slotLength) {
            exception = 1;
            break;
        }
        fscanf(inputFile, "%d", &(gds->variantsCount)); // Read the number of variants
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }
        gds->variantsCount++;
        fscanf(inputFile, "%d", &(gds->nodeCount)); // Read the number of nodes
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }
        tmp = rand() % 100;
        if (tmp < 1 && gds->incrementalParameters.addedSignalsCount >= 3)
            gds->incrementalParameters.addedNodesCount = 3;
        else if (tmp < 10 && gds->incrementalParameters.addedSignalsCount >= 2)
            gds->incrementalParameters.addedNodesCount = 2;
        else if (tmp < 20 && gds->incrementalParameters.addedSignalsCount >= 1)
            gds->incrementalParameters.addedNodesCount = 1;
        else
            gds->incrementalParameters.addedNodesCount = 0;

        gds->nodeCount += gds->incrementalParameters.addedNodesCount;

        // Read the number of signals per node
        gds->benchmarkInstance.signalsInNodeCounts = nnewi<int>(gds->nodeCount);
        for (i = 0; i < gds->nodeCount - gds->incrementalParameters.addedNodesCount; i++) {
            fscanf(inputFile, "%d", &(gds->benchmarkInstance.signalsInNodeCounts[i]));
            gds->benchmarkInstance.signalsInNodeCounts[i] = 0;
        }
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read the node assigment for signal
        gds->benchmarkInstance.signalNodeIDs = nnew<int>(gds->signalsCount);
        for (i = 0; i < gds->signalsCount - gds->incrementalParameters.addedSignalsCount; i++)
            fscanf(inputFile, "%d", &(gds->benchmarkInstance.signalNodeIDs[i]));
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read the signal names
        //gds->benchmarkInstance.signalNames = static_cast<char **>(mmalloc(sizeof(char*)*dataStructure->signalsCount));
        char *word = new char[30];
        for (i = 0; i < gds->signalsCount - gds->incrementalParameters.addedSignalsCount; i++) {
            fscanf(inputFile, "%s", word);
            //	dataStructure->signalNames[i] = static_cast<char*>(mmalloc((strlen(word)+1)*sizeof(char)));
            //	strcpy(dataStructure->signalNames[i], word);
        }
        //if(ferror(inputFile) || feof(inputFile))
        //{
        //	exception = 1;
        //	break;
        //}

        // Read signal periods
        gds->benchmarkInstance.signalPeriods = nnew<int>(gds->signalsCount);
        for (i = 0; i < gds->signalsCount - gds->incrementalParameters.addedSignalsCount; i++) {
            fscanf(inputFile, "%d", &(gds->benchmarkInstance.signalPeriods[i]));
        }
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read release times of signals
        gds->benchmarkInstance.signalReleaseDates = nnew<int>(gds->signalsCount);
        for (i = 0; i < gds->signalsCount - gds->incrementalParameters.addedSignalsCount; i++)
            fscanf(inputFile, "%d", &(gds->benchmarkInstance.signalReleaseDates[i]));
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read deadlines of signals
        gds->benchmarkInstance.signalDeadlines = nnew<int>(gds->signalsCount);
        for (i = 0; i < gds->signalsCount - gds->incrementalParameters.addedSignalsCount; i++)
            fscanf(inputFile, "%d", &(gds->benchmarkInstance.signalDeadlines[i]));
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read the number of bytes of signals
        gds->benchmarkInstance.signalPayloads = nnew<int>(gds->signalsCount);
        for (i = 0; i < gds->signalsCount - gds->incrementalParameters.addedSignalsCount; i++)
            fscanf(inputFile, "%d", &(gds->benchmarkInstance.signalPayloads[i]));
        if (ferror(inputFile) || feof(inputFile)) {
            exception = 1;
            break;
        }

        // Read the model variants
        gds->benchmarkInstance.modelVariants = nnew<int *>(gds->variantsCount); // Temporal array for variants reading
        int inputChar;
        for (i = 0; i < gds->variantsCount - 1; i++) {
            gds->benchmarkInstance.modelVariants[i] = nnewi<int>(gds->signalsCount);
            for (j = 0; j < gds->signalsCount - gds->incrementalParameters.addedSignalsCount; j++) {
                fscanf(inputFile, "%d", &inputChar);
                gds->benchmarkInstance.modelVariants[i][j] = inputChar;
            }
        }
        if (ferror(inputFile) || (feof(inputFile) && !gds->isReceivers)) {
            exception = 1;
            break;
        }
        if (gds->isReceivers) {
            int ABNodeCount = 0;
            fscanf(inputFile, "%d", &ABNodeCount);
            for (i = 0; i < ABNodeCount; i++) {
                int ABNode;
                fscanf(inputFile, "%d", &ABNode);
                gds->benchmarkInstance.nodesOnBothChannels[ABNode - 1] = 1;
            }


            for (i = 0; i < gds->signalsCount - gds->incrementalParameters.addedSignalsCount; i++) {
                int recCount, value;
                fscanf(inputFile, "%d", &recCount);
                for (j = 0; j < recCount; j++) {
                    fscanf(inputFile, "%d", &value);
                    gds->benchmarkInstance.signalReceivers[i].push_back(value);
                }
            }
        }

        if (ferror(inputFile)) {
            exception = 1;
            break;
        }
        gds->benchmarkInstance.modelVariants[gds->variantsCount - 1] = nnewi<int>(gds->signalsCount);

        break;
    }
    fclose(inputFile);
    if (exception) {
        printf("Error raised during reading input file!");
        return 0;
    }
    return 1;
}

int readInput(GDS *gds, int argc, char **argv) {
    int i;
    gds->seed = static_cast<unsigned int>(time(NULL));
    gds->configFileName = NULL;
    gds->outputFileName = NULL;
    gds->origBenchInstFileName = NULL;
    gds->isIncremental = FALSE;
    gds->isReceivers = FALSE;
    gds->isFaultTolerant = FALSE;
    gds->faultTolerantPromile = 0;
    gds->syncNodePercent = 100;
    gds->isMultiVariantTestingMode = 0;
    if (argc < 2)
        printf("No input file set - It is necessary to determine at least input file!");
    else {
        for (i = 1; i < argc; i++) {
            if (i != argc - 1) {
                switch (argv[i][0]) {
                    case '-':
                        switch (argv[i][1]) {
                            case 's':
                                i++;
                                gds->seed = static_cast<unsigned int>(strtoul(argv[i], nullptr, 10));
                                break;
                            case 'c':
                                i++;
                                gds->configFileName = argv[i];
                                break;
                            case 'i':
                                i++;
                                gds->isIncremental = TRUE;
                                gds->origBenchInstFileName = argv[i];
                                break;
                            case 'o':
                                i++;
                                gds->outputFileName = argv[i];
                                break;
                            case 'r':
                                gds->isReceivers = TRUE;
                                i++;
                                gds->syncNodePercent = atoi(argv[i]);
                                break;
                            case 'f':
                                gds->isFaultTolerant = TRUE;
                                i++;
                                gds->faultTolerantPromile = atoi(argv[i]);
                                break;
                            case 'm':
                                gds->isMultiVariantTestingMode = 1;
                                break;
                            case 'n':
                                gds->isMultiVariantIncrementalTestingMode = 1;
                                break;
                            default:
                                break;
                        }
                    default:
                        break;
                }
            }
        }
    }

    srand(gds->seed);
    return readConfigFile(gds) && ((gds->isIncremental) ? readOrigBenchInstFile(gds) : TRUE);
}

int testConfigFile(GDS *gds) {
    if (gds->outputFileName == NULL) {
        printf("The output file is not specified! You can do it by -o option.\n");
        return FAIL;
    }

    // Kdy� je d�lka cyklu men�� ne� 1
    if (gds->cycleLength < 1) {
        printf("The length of cycle must be bigger than 1!\n");
        return FAIL;
    }

    // Mus� b�t v�c uzl� ne� nod�
    if (gds->nodeCount > gds->signalsCount) {
        printf("Number of signals must be bigger or equal to the number of nodes!\n");
        return FAIL;
    }
    // Kdy� je po�et specifick�ch ECU vy��� ne� po�et specifick�ch sign�l�
    if (((gds->nodeCount * gds->specificNodesPortion) / 100) >
        ((gds->signalsCount * gds->specificSignalsPortion) / 100)) {
        printf("Number of specific signals must be bigger or equal to the number of specific nodes!\n");
        return FAIL;
    }

    // Pokud jsou sd�len� sign�ly, pak mus� b�t i sd�len� node
    if ((((gds->nodeCount * gds->commonNodesPortion) / 100) == 0) &&
        (((gds->signalsCount * gds->commonSignalsPortion) / 100) > 0)) {
        printf("There must be at least one common node if there are some common signals!\n");
        return FAIL;
    }

    // Pokud se maj� generovat i p��jemci, je t�eba m�t n�jak� sd�len� node, kter� bude p�ipojen na A i B a bude slou�it jako
    // synchroniza�n� ECU
    if ((gds->isReceivers == TRUE) && (((gds->nodeCount * gds->commonNodesPortion) / 100) == 0)) {
        printf("There must be at least one common node that will be connected to the A and B for synchronisation purposes!\n");
        return FAIL;
    }

    return SUCCESS;
}

int testBenchmarkInstance(GDS *gds) {
    int i;
    int *usedNode = nnewi<int>(gds->nodeCount);

    for (i = 0; i < gds->signalsCount; i++) {
        usedNode[gds->benchmarkInstance.signalNodeIDs[i] - 1] = 1;
    }

    for (i = 0; i < gds->nodeCount; i++) {
        if (usedNode[i] == 0) {
            printf("Generator was not able to assign signals to all the nodes!");
            return FAIL;
        }
    }
    ddelete(usedNode);
    return SUCCESS;
}

int generateInstances(GDS *gds) {
    // Nody: na za��tku jsou spole�n� nody, pak v�eobecn� nody a na konci jsou specifick�(nejvy��� nodeID)
    // Sign�ly: na za��tku jsou spole�n� sign�ly, pak pecifick� a na konci ostatn� sign�ly
    int i, j, k, tmp;
    int cNodeCount, sNodeCount, cSignalCount, sSignalCount;
    gds->benchmarkInstance.modelVariants = nnew<int *>(gds->variantsCount);
    for (i = 0; i < gds->variantsCount; i++)
        gds->benchmarkInstance.modelVariants[i] = nnew<int>(gds->signalsCount);
    gds->benchmarkInstance.signalDeadlines = nnew<int>(gds->signalsCount);
    gds->benchmarkInstance.signalPayloads = nnewi<int>(gds->signalsCount);
    gds->benchmarkInstance.signalPeriods = nnew<int>(gds->signalsCount);
    gds->benchmarkInstance.signalReleaseDates = nnewi<int>(gds->signalsCount);
    gds->benchmarkInstance.signalNodeIDs = nnew<int>(gds->signalsCount);

    cNodeCount = (gds->nodeCount * gds->commonNodesPortion) / 100;
    sNodeCount = (gds->nodeCount * gds->specificNodesPortion) / 100;
    cSignalCount = (gds->signalsCount * gds->commonSignalsPortion) / 100;
    sSignalCount = (gds->signalsCount * gds->specificSignalsPortion) / 100;

    // P�epo�et hustoty pravd�podobnosti do distribu�n� funkce
    for (i = 1; i < 7; i++)
        gds->periodProbabilities[i] = gds->periodProbabilities[i] + gds->periodProbabilities[i - 1];
    for (i = 1; i < gds->maxSignalPayload; i++)
        gds->payloadProbabilities[i] = gds->payloadProbabilities[i] + gds->payloadProbabilities[i - 1];
    for (i = 0; i < gds->signalsCount; i++) {
        // generov�n� periody
        tmp = rand() % 100;
        gds->benchmarkInstance.signalPeriods[i] = gds->cycleLength;
        for (j = 0; j < 7; j++) {
            if (tmp < gds->periodProbabilities[j]) break;
            gds->benchmarkInstance.signalPeriods[i] = gds->benchmarkInstance.signalPeriods[i] * 2;
        }

        // generov�n� payload
        tmp = rand() % 100;
        gds->benchmarkInstance.signalPayloads[i] = 1;
        for (j = 0; j < gds->maxSignalPayload; j++) {
            if (tmp < gds->payloadProbabilities[j]) break;
            gds->benchmarkInstance.signalPayloads[i]++;
        }

        // generov�n� deadlines
        gds->benchmarkInstance.signalDeadlines[i] = gds->benchmarkInstance.signalPeriods[i] - 1;
        if (rand() % 100 < gds->deadlinePortion) {
            gds->benchmarkInstance.signalDeadlines[i] -=
                    (rand() % (1 + gds->benchmarkInstance.signalPeriods[i] / (gds->cycleLength * 3)))
                    * gds->cycleLength;
            if (gds->benchmarkInstance.signalDeadlines[i] < gds->cycleLength)
                gds->benchmarkInstance.signalDeadlines[i] = gds->cycleLength; // Ale nejsp� by nem�lo nastat
        }

        // generov�n� release date
        if (rand() % 100 < gds->releasePortion) {
            tmp = (rand() % (1 + gds->benchmarkInstance.signalDeadlines[i] / gds->cycleLength)) * gds->cycleLength;
            if (tmp < 5 * gds->cycleLength)
                gds->benchmarkInstance.signalReleaseDates[i] = tmp;
            else
                gds->benchmarkInstance.signalReleaseDates[i] = 5 * gds->cycleLength;
        }
    }

    // Nastaven� ECUs pro z�kladn� sign�ly
    if (cNodeCount > 0) {
        int *signalsCountInCommonNodes = nnew<int>(cNodeCount);
        tmp = 0;
        // Ur�en� kolik spol. sign�l� v kter�m spol. nodu
        for (i = 0; i < cNodeCount; i++) {
            signalsCountInCommonNodes[i] = 20 + rand() % 80;
            tmp += signalsCountInCommonNodes[i];
        }
        for (i = 0; i < cNodeCount; i++) {
            signalsCountInCommonNodes[i] = signalsCountInCommonNodes[i] * cSignalCount / tmp;
        }

        tmp = 0;
        for (i = 0; i < cNodeCount; i++)
            tmp += signalsCountInCommonNodes[i];
        for (i = 0; i < cSignalCount - tmp; i++)
            signalsCountInCommonNodes[rand() % cNodeCount]++;

        tmp = 0;
        for (i = 0; i < cNodeCount; i++) {
            for (j = 0; j < signalsCountInCommonNodes[i]; j++) {
                gds->benchmarkInstance.signalNodeIDs[tmp] = i + 1;
                tmp++;
            }
        }
        ddelete(signalsCountInCommonNodes);
    }

    // Nastaven� ECUs pro specifick� sign�ly
    if (sNodeCount > 0) {
        int *signalsCountInSpecificNodes = nnew<int>(sNodeCount); // Kolik sign�l� je v jednotliv�ch specifick�ch nodech
        //int specSigInSpecNodesCount = sNodeCount + rand()%(1+sSignalCount-sNodeCount);
        tmp = 0;
        for (i = 0; i < sNodeCount; i++) {
            signalsCountInSpecificNodes[i] = 20 + rand() % 80;
            tmp += signalsCountInSpecificNodes[i];
        }
        for (i = 0; i < sNodeCount; i++) {
            signalsCountInSpecificNodes[i] = signalsCountInSpecificNodes[i] * sSignalCount / tmp;
        }
        tmp = 0;
        for (i = 0; i < sNodeCount; i++)
            tmp += signalsCountInSpecificNodes[i];
        for (i = 0; i < sSignalCount - tmp; i++)
            signalsCountInSpecificNodes[rand() % (sNodeCount + 1)]++;
        tmp = 0;
        for (i = 0; i < sNodeCount; i++) {
            for (j = 0; j < signalsCountInSpecificNodes[i]; j++) {
                gds->benchmarkInstance.signalNodeIDs[tmp + cSignalCount] = gds->nodeCount - i;
                tmp++;
            }
        }
        tmp += cSignalCount;
        ddelete(signalsCountInSpecificNodes);
    } else {
        tmp = cSignalCount;
    }

    // Nastavit ID Nodu v�em ostatn�m sign�l�m
    for (i = tmp; i < gds->signalsCount; i++) {
        if (i - tmp < gds->nodeCount - cNodeCount - sNodeCount)
            gds->benchmarkInstance.signalNodeIDs[i] =
                    cNodeCount + i - tmp + 1; // Zaji�t�n�, aby v�echny ECU se v rozvrhu objevily
        else
            gds->benchmarkInstance.signalNodeIDs[i] = (rand() % (gds->nodeCount - sNodeCount)) + 1;

    }

    // V�po�et kolik sign�l� vys�laj� jednotil� ECU
    gds->benchmarkInstance.signalsInNodeCounts = nnewi<int>(gds->nodeCount);
    for (i = 0; i < gds->signalsCount; i++) {
        gds->benchmarkInstance.signalsInNodeCounts[gds->benchmarkInstance.signalNodeIDs[i] - 1]++;
    }

    // Generov�n� variant
    int *variantNodes = nnewi<int>(gds->nodeCount * gds->variantsCount); // Jak� nody dan� varianta pou��v�

    for (i = 0; i < gds->variantsCount; i++) // Ur�i jak� nody budou v jak�ch variant�ch
    {
        tmp = 30 + rand() % 40;
        for (j = 0; j < gds->nodeCount; j++) // Ur�en� jak� nody dan� varianta vyu��v�
        {
            if (j < cNodeCount) // V�echny spole�n� nody mus� b�t ve v�ech variant�ch
                variantNodes[i * gds->nodeCount + j] = 1;
            else if (j < gds->nodeCount - sNodeCount) {
                if (rand() % 100 < tmp)
                    variantNodes[i * gds->nodeCount + j] = 1; // Pro klasick� nody to ur�i pravd�podobnostn�
            } else if (j == gds->nodeCount - sNodeCount + i)
                variantNodes[i * gds->nodeCount + j] = 1; // P�i�azen� pro specifick� nod
        }
    }

    for (j = cNodeCount;
         j < gds->nodeCount - sNodeCount; j++) // Pokud n�jak� node nen� rozvr�en nikde, p�i�a� jej n�jak� variant�
    {
        tmp = 0;
        for (i = 0; i < gds->variantsCount; i++) {
            if (variantNodes[i * gds->nodeCount + j] == 1) {
                tmp = 1;
                break;
            }
        }
        if (tmp == 0) {
            for (i = 0; i < 1 + rand() % 3; i++) {
                variantNodes[rand() % gds->variantsCount * gds->nodeCount + j] = 1;
            }
        }
    }

    if (gds->isMultiVariantTestingMode | gds->isMultiVariantIncrementalTestingMode == 0 || gds->nodeCount > 1) {
        int *isSpecSignalScheduled = nnewi<int>(
                sSignalCount); // Ur�en� zda-li je specifick� sign�l ji� rozvr�en do n�jak� varianty
        for (i = 0; i < gds->variantsCount; i++) {
            tmp = 30 + rand() % 40;
            for (j = 0; j < gds->signalsCount; j++) {
                if (j < cSignalCount) {
                    gds->benchmarkInstance.modelVariants[i][j] = 1;
                } else if (j < cSignalCount + sSignalCount) {
                    if (isSpecSignalScheduled[j - cSignalCount] == 0) {
                        if (variantNodes[i * gds->nodeCount + gds->benchmarkInstance.signalNodeIDs[j] - 1] == 1) {
                            if (gds->benchmarkInstance.signalNodeIDs[j] >
                                gds->nodeCount - sNodeCount) // Je-li node rozvr�en pro tuto variantu
                            {
                                gds->benchmarkInstance.modelVariants[i][j] = 1;
                                isSpecSignalScheduled[j - cSignalCount] = 1;
                            } else {
                                if (rand() % (gds->variantsCount - i) == 0) {
                                    isSpecSignalScheduled[j - cSignalCount] = 1;
                                    gds->benchmarkInstance.modelVariants[i][j] = 1;
                                } else {
                                    gds->benchmarkInstance.modelVariants[i][j] = 0;
                                }
                            }
                        } else {
                            gds->benchmarkInstance.modelVariants[i][j] = 0;
                        }
                    } else {
                        gds->benchmarkInstance.modelVariants[i][j] = 0;
                    }
                } else {
                    if (variantNodes[i * gds->nodeCount + gds->benchmarkInstance.signalNodeIDs[j] - 1] == 1)
                        gds->benchmarkInstance.modelVariants[i][j] = ((rand() % 100) < tmp) ? 1 : 0;
                    else
                        gds->benchmarkInstance.modelVariants[i][j] = 0;
                }
            }
        }
        ddelete(isSpecSignalScheduled);

        // Oprava v p��pad�, �e st�le existuje sign�l, kter� nen� rozvr�en do ��dn� varianty
        for (j = cSignalCount + sSignalCount; j < gds->signalsCount; j++) {
            tmp = 0;
            for (i = 0; i < gds->variantsCount; i++) {
                if (gds->benchmarkInstance.modelVariants[i][j] == 1) {
                    tmp = 1;
                    break;
                }
            }

            if (tmp == 0) {
                for (i = 0; i < 1 + rand() % 3; i++) {
                    tmp = rand() % gds->variantsCount;
                    while (variantNodes[tmp * gds->nodeCount + gds->benchmarkInstance.signalNodeIDs[j] - 1] ==
                           0) // TODO: Predelat. Tohle muze trvat nekonecne dlouho. Coz je kekel.
                        tmp = rand() % gds->variantsCount;
                    gds->benchmarkInstance.modelVariants[tmp][j] = 1;
                }
            }
        }
    } else if (gds->isMultiVariantTestingMode)// Multivariant testing mode
    {
        for (j = 0; j < gds->signalsCount; j++) {
            tmp = rand() % 100;
            if (tmp < gds->commonSignalsPortion) // Generate common signals
            {
                for (i = 0; i < gds->variantsCount; i++)
                    gds->benchmarkInstance.modelVariants[i][j] = 1;
            } else if (tmp < gds->commonSignalsPortion + gds->specificSignalsPortion) // Generate specific signals
            {
                int used_variant = rand() % gds->variantsCount;
                for (i = 0; i < gds->variantsCount; i++)
                    gds->benchmarkInstance.modelVariants[i][j] = (i == used_variant) ? 1 : 0;
            } else {
                for (i = 0; i < gds->variantsCount; i++)
                    gds->benchmarkInstance.modelVariants[i][j] = 1;
                int unused_number = rand() % (gds->variantsCount - 2) + 1;
                unused_number = ((unused_number < gds->variantsCount / 2) && (gds->variantsCount > 3))
                                ? gds->variantsCount / 2 : unused_number;
                for (i = 0; i < unused_number; i++) {
                    gds->benchmarkInstance.modelVariants[rand() % gds->variantsCount][j] = 0;
                }
            }
        }
    } else // MultiVariant vs Incremental testing mode
    {
        for (j = 0; j < gds->signalsCount; j++) {
            tmp = rand() % 100;
            if (tmp < gds->specificSignalsPortion) {
                int used_variant = rand() % gds->variantsCount;
                for (i = 0; i < gds->variantsCount; i++)
                    gds->benchmarkInstance.modelVariants[i][j] = (i == used_variant) ? 1 : 0;
            } else {
                tmp = rand() % 2;
                if (tmp == 0) {
                    for (i = 0; i < gds->variantsCount; i++)
                        gds->benchmarkInstance.modelVariants[i][j] = 1;
                } else {
                    for (i = 0; i < gds->variantsCount; i++)
                        gds->benchmarkInstance.modelVariants[i][j] = 1;
                    int unused_number = rand() % (gds->variantsCount - 2) + 1;
                    unused_number = ((unused_number < gds->variantsCount / 2) && (gds->variantsCount > 3))
                                    ? gds->variantsCount / 2 : unused_number;
                    for (i = 0; i < unused_number; i++) {
                        gds->benchmarkInstance.modelVariants[rand() % gds->variantsCount][j] = 0;
                    }
                }
            }
        }
    }

    // Nastaveni prijimacu daného signálu
    if (gds->isReceivers == TRUE) {
        // Generov�n� ECU kter� budou p�ipojeny na oba kan�ly
        {
            //tmp = (rand() % cNodeCount) + 1;
            tmp = (gds->nodeCount * gds->syncNodePercent) / 100;
            if ((tmp < 2) && (gds->nodeCount >= 2)) tmp = 2;
            //if (tmp > 3) tmp = 3;
            //for(i = 0; i < tmp; i++)
            //{
            //	gds->benchmarkInstance.nodesOnBothChannels[i] = 1;
            //}
            //i = 0;
            while (tmp > 0) {
                if ((gds->benchmarkInstance.nodesOnBothChannels[i] == 0) && (rand() % 100 <= gds->syncNodePercent)) {
                    gds->benchmarkInstance.nodesOnBothChannels[i] = 1;
                    tmp--;
                }
                i = (i + 1) % gds->nodeCount;
            }
        }
        int *probabilityIncidenceMatrix = nnew<int>(gds->nodeCount * gds->nodeCount);
        // Napocitavani pravdepodobnostni incidencni matice
        for (i = 0; i < gds->nodeCount * gds->nodeCount; i++) {
            tmp = rand() % 100;
            probabilityIncidenceMatrix[i] = tmp;
            // Normalizace pres radek
            if ((i + 1) % gds->nodeCount == 0) {
                const int firstRowItem = (i / gds->nodeCount) * gds->nodeCount;
                float sum = 0;
                for (j = firstRowItem; j < firstRowItem + gds->nodeCount; j++) {
                    if (gds->benchmarkInstance.nodesOnBothChannels[j] == 1)
                        probabilityIncidenceMatrix[j] *= 3;
                    sum += probabilityIncidenceMatrix[j];
                }
                sum = sum / 100;

                for (j = firstRowItem; j < firstRowItem + gds->nodeCount; j++) {
                    probabilityIncidenceMatrix[j] = static_cast<int>(probabilityIncidenceMatrix[j] / sum);
                    if (probabilityIncidenceMatrix[j] < 1)
                        probabilityIncidenceMatrix[j] = 1;
                }
            }
        }
        for (i = 0; i < gds->signalsCount; i++) {
            tmp = rand() % 100;
            int *possibleRecievers = nnewi<int>(gds->nodeCount);
            for (j = 0; j < gds->nodeCount - 1; j++) {
                if (tmp >= gds->receiversProbabilities[j]) {
                    tmp = j + 1;
                    break;
                }
            }
            for (j = 0; j < gds->variantsCount; j++) {
                for (k = 0; k < gds->nodeCount; k++) {
                    if ((k != gds->benchmarkInstance.signalNodeIDs[i] - 1) &&
                        (variantNodes[j * gds->nodeCount + k] == 1))
                        possibleRecievers[k] = 1;
                }
            }
            if (i < cSignalCount) {
                int commonNode = rand() % cNodeCount;
                if (commonNode == gds->benchmarkInstance.signalNodeIDs[i] - 1)
                    commonNode = (commonNode + 1) % cNodeCount;
                if (commonNode != gds->benchmarkInstance.signalNodeIDs[i] -
                                  1) // kdyby byl jen jeden sd�lenej node taky by se to mohlo st�le rovnat
                {
                    if (commonNode == gds->benchmarkInstance.signalNodeIDs[i] - 1)
                        printf("");
                    gds->benchmarkInstance.signalReceivers[i].push_back(commonNode);
                    possibleRecievers[commonNode] = 0;
                    tmp--;
                }
            }
            int possibleCount = 0;
            for (j = 0; j < gds->nodeCount - 1; j++) // kolik je moznych nodu
            {
                if (possibleRecievers[j] == 1)
                    possibleCount++;
            }
            if (possibleCount < tmp) {
                tmp = possibleCount;
            }
            if (possibleCount == tmp) {
                for (j = 0; j < gds->nodeCount - 1; j++) {
                    if (possibleRecievers[j] == 1) {
                        if (j == gds->benchmarkInstance.signalNodeIDs[i] - 1)
                            printf("");
                        gds->benchmarkInstance.signalReceivers[i].push_back(j);
                    }
                }
            }
            if (possibleCount > tmp) {
                int *probabilitiesRec = nnewi<int>(
                        gds->nodeCount); // S jakou pravdepodobnosti by se mel objevit dany node jako reciever
                float sum = 0;
                for (j = 0; j < gds->nodeCount; j++) {
                    if (possibleRecievers[j] == 1) {
                        probabilitiesRec[j] = probabilityIncidenceMatrix[
                                (gds->benchmarkInstance.signalNodeIDs[i] - 1) * gds->nodeCount + j];
                        sum += probabilitiesRec[j];
                    }
                }
                for (j = 0; j < gds->nodeCount; j++) {
                    if (possibleRecievers[j] == 1) {
                        probabilitiesRec[j] = static_cast<int>(probabilitiesRec[j] / sum);
                    }
                }
                j = 0;
                while (tmp > 0) {
                    if ((rand() % 100 <= probabilitiesRec[j]) && (possibleRecievers[j] == 1)) {
                        possibleRecievers[j] = 0;
                        gds->benchmarkInstance.signalReceivers[i].push_back(j);
                        tmp--;
                    }
                    j = (j + 1) % gds->nodeCount;
                }
                ddelete(probabilitiesRec);
            }
            ddelete(possibleRecievers);
        }
        ddelete(probabilityIncidenceMatrix);
    }

    ddelete(variantNodes);
    if (testBenchmarkInstance(gds) == FAIL)
        return FAIL;
    return SUCCESS;
}

int generateIncremInstances(GDS *gds) {
    int i, j, tmp;

    // Prepocet hustoty pravdepodobnosti do distribucni funkci
    for (i = 1; i < 7; i++)
        gds->periodProbabilities[i] = gds->periodProbabilities[i] + gds->periodProbabilities[i - 1];
    for (i = 1; i < gds->maxSignalPayload; i++)
        gds->payloadProbabilities[i] = gds->payloadProbabilities[i] + gds->payloadProbabilities[i - 1];
    for (i = gds->signalsCount - gds->incrementalParameters.addedSignalsCount; i < gds->signalsCount; i++) {
        // generovani periody
        tmp = rand() % 100;
        gds->benchmarkInstance.signalPeriods[i] = gds->cycleLength;
        for (j = 0; j < 7; j++) {
            if (tmp < gds->periodProbabilities[j]) break;
            gds->benchmarkInstance.signalPeriods[i] = gds->benchmarkInstance.signalPeriods[i] * 2;
        }

        // generovani payload
        tmp = rand() % 100;
        gds->benchmarkInstance.signalPayloads[i] = 1;
        for (j = 0; j < 8; j++) {
            if (tmp < gds->payloadProbabilities[j]) break;
            gds->benchmarkInstance.signalPayloads[i]++;
        }

        // generovani deadlines
        gds->benchmarkInstance.signalDeadlines[i] = gds->benchmarkInstance.signalPeriods[i] - 1;
        if (rand() % 100 < gds->deadlinePortion) {
            gds->benchmarkInstance.signalDeadlines[i] -=
                    (rand() % (1 + gds->benchmarkInstance.signalPeriods[i] / (gds->cycleLength * 3)))
                    * gds->cycleLength;
            if (gds->benchmarkInstance.signalDeadlines[i] < gds->cycleLength)
                gds->benchmarkInstance.signalDeadlines[i] = gds->cycleLength; // Ale nejsp� by nem�lo nastat
        }

        // generovani release date
        if (rand() % 100 < gds->releasePortion) {
            tmp = (rand() % (1 + gds->benchmarkInstance.signalDeadlines[i] / gds->cycleLength)) * gds->cycleLength;
            if (tmp < 5 * gds->cycleLength)
                gds->benchmarkInstance.signalReleaseDates[i] = tmp;
            else
                gds->benchmarkInstance.signalReleaseDates[i] = 5 * gds->cycleLength;
        } else {
            gds->benchmarkInstance.signalReleaseDates[i] = 0;
        }
    }

    // Nastavit ID Nodu novym signalem
    for (i = gds->signalsCount - gds->incrementalParameters.addedSignalsCount; i < gds->signalsCount; i++) {
        if (i < gds->signalsCount - gds->incrementalParameters.addedSignalsCount
                + gds->incrementalParameters.addedNodesCount)
            gds->benchmarkInstance.signalNodeIDs[i] =
                    i - gds->signalsCount + gds->incrementalParameters.addedSignalsCount
                    + gds->nodeCount - gds->incrementalParameters.addedNodesCount +
                    1; // Zaji�t�n�, aby v�echny ECU se v rozvrhu objevily
        else {
            if (gds->incrementalParameters.addedNodesCount == 0) {
                gds->benchmarkInstance.signalNodeIDs[i] = (rand() % gds->nodeCount) + 1;
            } else {
                if (rand() % 100 < 70)
                    gds->benchmarkInstance.signalNodeIDs[i] =
                            gds->nodeCount - gds->incrementalParameters.addedNodesCount
                            + (rand() % gds->incrementalParameters.addedNodesCount) + 1;
                else
                    gds->benchmarkInstance.signalNodeIDs[i] = (rand() % (gds->nodeCount)) + 1;
            }
        }
    }

    // Vypocet kolik signalu vysilaji jednotile ECU
    //gds->benchmarkInstance.signalsInNodeCounts = nnewi<int>(gds->nodeCount);
    for (i = 0; i < gds->signalsCount; i++) {
        gds->benchmarkInstance.signalsInNodeCounts[gds->benchmarkInstance.signalNodeIDs[i] - 1]++;
    }

    // Generovani varianty
    tmp = rand() % (gds->variantsCount - 1); // Zkopíruj náhodně nějakou jinou variantu
    int *isNodeUsed = nnewi<int>(gds->nodeCount - gds->incrementalParameters.addedNodesCount);
    for (i = 0; i < gds->signalsCount; i++) {
        if (gds->benchmarkInstance.modelVariants[tmp][i] == 1)
            isNodeUsed[gds->benchmarkInstance.signalNodeIDs[i] - 1] = 1;
        if (i < gds->signalsCount - gds->incrementalParameters.addedSignalsCount)
            gds->benchmarkInstance.modelVariants[gds->variantsCount - 1][i] =
                    gds->benchmarkInstance.modelVariants[tmp][i]; //Zkop�ruj sign�ly ze star� varianty
        else
            gds->benchmarkInstance.modelVariants[gds->variantsCount - 1][i] = 1; //Nov� sign�ly mus� b�t v nov� variant�
    }

    int begin = 1;
    for (i = 0; i < gds->signalsCount - gds->incrementalParameters.addedSignalsCount; i++) {
        if (begin == 1 && gds->benchmarkInstance.modelVariants[gds->variantsCount - 1][i] == 1) {
            continue;
        } else {
            begin = 0;
        }
        if (rand() % 100 < 30) {
            if (gds->benchmarkInstance.modelVariants[gds->variantsCount - 1][i] == 1) {
                if (rand() % 100 < 35) {
                    gds->benchmarkInstance.modelVariants[gds->variantsCount - 1][i] = 0;
                }
            } else {
                if (isNodeUsed[gds->benchmarkInstance.signalNodeIDs[i] - 1] == 1) {
                    if (rand() % 100 < 65) {
                        gds->benchmarkInstance.modelVariants[gds->variantsCount - 1][i] = 1;
                    }
                } else {
                    if (rand() % 300 < 1) {
                        gds->benchmarkInstance.modelVariants[gds->variantsCount - 1][i] = 1;
                        isNodeUsed[gds->benchmarkInstance.signalNodeIDs[i] - 1] = 1;
                    }
                }
            }
        }
    }


    ddelete(isNodeUsed);
    if (testBenchmarkInstance(gds) == FAIL)
        return FAIL;
    return SUCCESS;
}

int saveToOutputFile(GDS *gds) {
    int i, j;
    FILE *fOutputFile;
    fOutputFile = fopen(gds->outputFileName, "w");
    if (fOutputFile == NULL || ferror(fOutputFile)) {
        printf("Error occured during saving the result to the output file!");
        if (fOutputFile != NULL)
            fclose(fOutputFile);
        return FAIL;
    }
    fprintf(fOutputFile, "%d\n", gds->signalsCount);
    fprintf(fOutputFile, "%d\n", gds->cycleLength);
    fprintf(fOutputFile, "%d\n", gds->slotLength);
    fprintf(fOutputFile, "%d\n", gds->variantsCount);
    fprintf(fOutputFile, "%d\n", gds->nodeCount);
    for (i = 0; i < gds->nodeCount; i++)
        fprintf(fOutputFile, "%d ", gds->benchmarkInstance.signalsInNodeCounts[i]);
    fprintf(fOutputFile, "\n");
    for (i = 0; i < gds->signalsCount; i++)
        fprintf(fOutputFile, "%d ", gds->benchmarkInstance.signalNodeIDs[i]);
    fprintf(fOutputFile, "\n");
    for (i = 0; i < gds->signalsCount; i++)
        fprintf(fOutputFile, "signal%d ", i);
    fprintf(fOutputFile, "\n");
    for (i = 0; i < gds->signalsCount; i++)
        fprintf(fOutputFile, "%d ", gds->benchmarkInstance.signalPeriods[i]);
    fprintf(fOutputFile, "\n");
    for (i = 0; i < gds->signalsCount; i++)
        fprintf(fOutputFile, "%d ", gds->benchmarkInstance.signalReleaseDates[i]);
    fprintf(fOutputFile, "\n");
    for (i = 0; i < gds->signalsCount; i++)
        fprintf(fOutputFile, "%d ", gds->benchmarkInstance.signalDeadlines[i]);
    fprintf(fOutputFile, "\n");
    for (i = 0; i < gds->signalsCount; i++)
        fprintf(fOutputFile, "%d ", gds->benchmarkInstance.signalPayloads[i]);
    fprintf(fOutputFile, "\n");
    for (i = 0; i < gds->variantsCount; i++) {
        for (j = 0; j < gds->signalsCount; j++) {
            fprintf(fOutputFile, "%d ", gds->benchmarkInstance.modelVariants[i][j]);
        }
        fprintf(fOutputFile, "\n");
    }
    if (gds->isReceivers) {
        // Nody ktere jsou pripojeny na A i B
        int nodeSum = 0;
        for (i = 0; i < gds->nodeCount; i++) {
            if (gds->benchmarkInstance.nodesOnBothChannels[i] == 1)
                nodeSum++;
        }
        fprintf(fOutputFile, "%d ", nodeSum);
        for (i = 0; i < gds->nodeCount; i++) {
            if (gds->benchmarkInstance.nodesOnBothChannels[i] == 1)
                fprintf(fOutputFile, "%d ", i + 1);
        }
        fprintf(fOutputFile, "\n");

        // P��jemci zpr�v
        for (i = 0; i < gds->signalsCount; i++) {
            fprintf(fOutputFile, "%lu ", gds->benchmarkInstance.signalReceivers[i].size());
            for (j = 0; j < static_cast<int>(gds->benchmarkInstance.signalReceivers[i].size()); j++) {
                fprintf(fOutputFile, "%d ", gds->benchmarkInstance.signalReceivers[i][j] + 1);
            }
            fprintf(fOutputFile, "\n");
        }

        // Generovani fault-tolerant messages
        for (i = 0; i < gds->signalsCount; i++) {
            if ((gds->benchmarkInstance.nodesOnBothChannels[gds->benchmarkInstance.signalNodeIDs[i] - 1] == 1) &&
                gds->isFaultTolerant) {
                if (rand() % 1000 < gds->faultTolerantPromile) {
                    fprintf(fOutputFile, "1\n");
                } else {
                    fprintf(fOutputFile, "0\n");
                }
            } else {
                fprintf(fOutputFile, "0\n");
            }
        }
    }
    fclose(fOutputFile);
    return SUCCESS;
}

int releaseDataStructures(GDS *gds) {
    int i;
    ddelete(gds->benchmarkInstance.signalsInNodeCounts);
    ddelete(gds->benchmarkInstance.signalReleaseDates);
    ddelete(gds->benchmarkInstance.signalPeriods);
    ddelete(gds->benchmarkInstance.signalPayloads);
    ddelete(gds->benchmarkInstance.signalNodeIDs);
    ddelete(gds->benchmarkInstance.signalDeadlines);
    //if (allocatedMemory.find(static_cast<void *>(gds->benchmarkInstance.signalReceivers)) != allocatedMemory.end())
    if (isAllocated(gds->benchmarkInstance.signalReceivers))
        ddelete(gds->benchmarkInstance.signalReceivers);
    //if (allocatedMemory.find(static_cast<void *>(gds->benchmarkInstance.modelVariants)) !=
    //    allocatedMemory.end())
    if (isAllocated(gds->benchmarkInstance.modelVariants)) {
        for (i = 0; i < gds->variantsCount; i++) {
            ddelete(gds->benchmarkInstance.modelVariants[i]);
        }
    }
    if (gds->isReceivers) {
        ddelete(gds->benchmarkInstance.nodesOnBothChannels);
        ddelete(gds->receiversProbabilities);
    }
    ddelete(gds->benchmarkInstance.modelVariants);
    ddelete(gds->payloadProbabilities);
    ddelete(gds->periodProbabilities);
    return SUCCESS;
}

int main(int argc, char **argv) {
    printf("Benchmark instances generator:\n");
    GDS gds;
    if (readInput(&gds, argc, argv) == SUCCESS) {
        if (testConfigFile(&gds) == SUCCESS) {
            if (!gds.isIncremental) {
                if (generateInstances(&gds) == SUCCESS) {
                    saveToOutputFile(&gds);
                }
            } else {
                if (generateIncremInstances(&gds) == SUCCESS) {
                    saveToOutputFile(&gds);
                }
            }
        }
        releaseDataStructures(&gds);
    }
    ddeleteAll();
#ifdef _DEBUG
    getchar();
#endif
    return 0;
}