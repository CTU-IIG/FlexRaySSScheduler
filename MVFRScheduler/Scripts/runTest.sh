#!/bin/bash
./MVFRScheduler -aca -o HW_schedule.txt HW_testbench.txt 
./MVFRScheduler -ae 0 HW_schedule.txt -o HW_BoardConfig.txt HW_testbench.txt
./MVFRScheduler -ax 0 HW_schedule.txt -o Fibex_BoardDB.txt HW_testbench.txt
./MVFRScheduler -at 0 HW_schedule.txt -o HW_BoardConfig.svg HW_testbench.txt
 