#!/bin/bash

python3 make_MV_Approaches_plot.py --instances 30 ConfigFilesApproaches &
python3 make_Incremental_plot.py --instances 30 --iterations 10 Results_Incremental/results.csv &
python3 make_MV_plot.py --steps 50 --instances 30 porscheConfigFile_1.txt Results_MV/results.csv &
python3 make_MV_vs_Incremental_plot.py --steps 50 --iterations 10 --instances 30 porscheConfigFile_1.txt Results_MV_Incremental/results.csv &
