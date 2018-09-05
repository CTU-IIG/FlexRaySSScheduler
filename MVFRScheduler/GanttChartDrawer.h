//
// Created by jenik on 9/25/15.
//

#ifndef MVFRSCHEDULER_GANTTCHARTDRAWER_H
#define MVFRSCHEDULER_GANTTCHARTDRAWER_H

#include "MVFRScheduler.h"

#define BITWIDTH 3
#define LINEWIDTH 1
#define CYCLEHEIGHT 30
#define LMARGIN 60
#define BMARGIN 60
#define RMARGIN 15
#define TMARGIN 15
#define TASKDELIMITER 4
#define FONTSIZE 26
#define FONTWIDTH 12
#define LABELMARGIN 8

int createGanttChart(DataStructure *dataStructure);

int createGanttChartSVG(DataStructure *dataStructure);

void drawEnvironment(DataStructure *dataStructure, int xDim, int yDim, const int variantID, svg::Document &doc);

void drawTask(svg::Document &doc, DataStructure *dataStructure, int signal);

void drawArrows(int xDim, int yDim, svg::Document &doc);

void createGanttChartSVGForVariant(DataStructure *dataStructure, const int &variantID);

#endif //MVFRSCHEDULER_GANTTCHARTDRAWER_H
