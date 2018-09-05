#!/usr/bin/python
import sys, os
from random import randint

# file_name = sys.argv[0]
# divider = sys.argv[1]

prefix = 'instances/'


def generateNewFileName(number, commonNodePercent, faultTolerantSignalsPermile):
    return prefix + 'Instance' + str(number).zfill(4) + '_CNP' + str(commonNodePercent).zfill(3) + '_FTSP' + str(
        faultTolerantSignalsPermile).zfill(4) + '.txt'


def generateInstances(baseNumber=1, instances=1):
    delimiter = ' '
    instanceID = 1;
    for k in range(0, 10):
        for i in range(0, 21):
            for j in range(0, 21):
                commonNodePercent = i * 5
                faultTolerantSignalsPermile = j * 50
                fileName = generateNewFileName(instanceID, commonNodePercent, faultTolerantSignalsPermile)
                print("../../BenchmarkGenerator/Binary/BenchmarkGenerator -r " + str(
                    commonNodePercent) + " -c ../../BenchmarkGenerator/Binary/SAE_1_recievers.txt -f " + str(
                    faultTolerantSignalsPermile) + " -o " + fileName)
                os.system("../../BenchmarkGenerator/Binary/BenchmarkGenerator -r " + str(
                    commonNodePercent) + " -c ../../BenchmarkGenerator/Binary/SAE_1_recievers.txt -f " + str(
                    faultTolerantSignalsPermile) + " -o " + fileName)
                instanceID = instanceID + 1


if (len(sys.argv) >= 3):
    generateInstances(int(sys.argv[1]), int(sys.argv[2]))
elif (len(sys.argv) >= 2):
    generateInstances(int(sys.argv[1]))
else:
    generateInstances()
