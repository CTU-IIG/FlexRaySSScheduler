#!/bin/bash

python3 generateApproachesTestInstances.py --instances 30 ConfigFilesApproaches
python3 generateApproachesTestInstances.py --instances 30 --runtest ConfigFilesApproaches

python3 generateIncrementalTestInstances.py --instances 30 --iterations 10 porscheConfigFile_1.txt
python3 generateIncrementalTestInstances.py --instances 30 --iterations 10 --runtest Instances_Incremental Results_Incremental porscheConfigFile_1.txt

python3 generateMVTestInstances.py --steps 50 --instances 30 porscheConfigFile_1.txt
python3 generateMVTestInstances.py --steps 50 --instances 30 --runtest Instances_MV Results_MV porscheConfigFile_1.txt

python3 generateMVIncrementalTestInstances.py --steps 50 --instances 30 --iterations 10 porscheConfigFile_1.txt
python3 generateMVIncrementalTestInstances.py --steps 50 --instances 30 --iterations 10 --runtest Instances_MV_Incremental Results_MV_Incremental porscheConfigFile_1.txt

python3 make_MV_Approaches_plot.py --instances 30 ConfigFilesApproaches &
python3 make_Incremental_plot.py --instances 30 --iterations 10 Results_Incremental/results.csv &
python3 make_MV_plot.py --steps 50 --instances 30 porscheConfigFile_1.txt Results_MV/results.csv &
python3 make_MV_vs_Incremental_plot.py --steps 500 --iterations 10 --instances 30 porscheConfigFile_1.txt Results_MV_Incremental/results.csv &
