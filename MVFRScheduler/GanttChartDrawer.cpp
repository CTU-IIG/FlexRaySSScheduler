//
// Created by jenik on 9/25/15.
//

#include "MVFRScheduler.h"

using namespace svg;

class JetColorMap {
private:
    static double interpolate(double val, double y0, double x0, double y1, double x1);

    static double base(double val);

public:
    static double red(double gray);

    static double green(double gray);

    static double blue(double gray);

    static svg::Color getColour(double gray);
};


int createGanttChart(DataStructure *dataStructure) {

    if (dataStructure->algorithm == GANTT)
        moveOriginalScheduleToCurrentSchedule(dataStructure);
    createGanttChartSVG(dataStructure);
}

void drawTask(Document &doc, DataStructure *dataStructure, int signal) {
    int i;
    const int &cycleID = dataStructure->signalStartCycle[signal];
    const int &slotID = dataStructure->signalSlot[signal];
    const int &period = dataStructure->signalPeriods[signal];
    const int &offsetInFrame = dataStructure->signalInFrameOffsets[signal];
    const int &payload = dataStructure->signalLengths[signal];
    const int &hyperperiod = dataStructure->hyperPeriod;
    const int &slotPayload = dataStructure->slotLength;
    if (slotID < 0) // Skip non scheduled tasks in given channel - For channel scheduling
        return;
    for (i = cycleID; i < hyperperiod; i += period) {
        Polygon task(Fill(JetColorMap::getColour(signal / static_cast<double>(dataStructure->signalsCount))),
                     Stroke(LINEWIDTH, Color::Black), CRISPEDGES);
        int x = LMARGIN + (slotID * slotPayload + offsetInFrame) * BITWIDTH;
        int y = BMARGIN + (hyperperiod - i - 1) * CYCLEHEIGHT;
        task << Point(x, y) << Point(x + payload * BITWIDTH, y) <<
        Point(x + payload * BITWIDTH, y + CYCLEHEIGHT - TASKDELIMITER) <<
        Point(x, y + CYCLEHEIGHT - TASKDELIMITER);
        doc << task;
    }
}

int createGanttChartSVG(DataStructure *dataStructure) {
    int i;

    int &variantID = (static_cast<ExporterParameters *>(dataStructure->schedulerParameters))->variantID;
    if (variantID < -1 || variantID >= dataStructure->variantCount) {
        for (i = 0; i < dataStructure->variantCount; i++) {
            createGanttChartSVGForVariant(dataStructure, i);
        }
        variantID = -1;
    }
    else {
        createGanttChartSVGForVariant(dataStructure, variantID);
    }

}

void createGanttChartSVGForVariant(DataStructure *dataStructure, const int &variantID) {
    int i;
    int xDim = LMARGIN + dataStructure->maxSlot * dataStructure->slotLength * BITWIDTH + RMARGIN;
    int yDim = BMARGIN + dataStructure->hyperPeriod * CYCLEHEIGHT + TMARGIN;
    Dimensions dimensions(xDim + 2, yDim + 2);
    std::string docFileName(dataStructure->outputFile);
    std::stringstream ssFileName;
    size_t dotPosition = docFileName.find_last_of('.');
    ssFileName << docFileName.substr(0, dotPosition) << "_variant" << variantID << ".svg";
    docFileName = ssFileName.str();
    Document doc(docFileName, Layout(dimensions, Layout::BottomLeft));

    doc << EmbeddedFont("csr10.svgf");

    for (i = 0; i < dataStructure->signalsCount; i++) {
        if (variantID == -1 || dataStructure->modelVariants[variantID][i] == 1) {
            drawTask(doc, dataStructure, i);
        }
    }

    drawEnvironment(dataStructure, xDim, yDim, variantID, doc);

    doc.save();
}

void drawEnvironment(DataStructure *dataStructure, int xDim, int yDim, const int variantID, Document &doc) {

    const int &slotPayload = dataStructure->slotLength;
    std::set<int> *nodeSlotAssignment = new std::set<int>[dataStructure->nodeCount];
    getNodeToSlotAssignment(dataStructure, nodeSlotAssignment);

    int i;
    // Draw axes
    Line xAxes(Point(LMARGIN - 8, BMARGIN), Point(xDim, BMARGIN), Stroke(LINEWIDTH * 2, Color::Black), CRISPEDGES);
    doc << xAxes;
    Line yAxes(Point(LMARGIN, BMARGIN - 8), Point(LMARGIN, yDim), Stroke(LINEWIDTH * 2, Color::Black), CRISPEDGES);
    doc << yAxes;

    drawArrows(xDim, yDim, doc);

    // Draw slot delimiters
    const unsigned int dash[] = {2, 4, 4, 2};
    for (i = 0; i < dataStructure->maxSlot; i++) {
        Line delimiter(Point(LMARGIN + (i + 1) * slotPayload * BITWIDTH, BMARGIN - 15),
                       Point(LMARGIN + (i + 1) * slotPayload * BITWIDTH, yDim),
                       Stroke(LINEWIDTH, Color::Black,
                              std::vector<unsigned int>(dash, dash + sizeof(dash) / sizeof(dash[0]))), CRISPEDGES);
        doc << delimiter;
    }

    // Draw row labels
    int xTextPosition;
    int yTextPosition;
    for (i = 0; i < dataStructure->hyperPeriod; i++) {
        if (i < 10) {
            xTextPosition = LMARGIN - LABELMARGIN - FONTWIDTH;
        }
        else {
            xTextPosition = LMARGIN - LABELMARGIN - 2 * FONTWIDTH;
        }
        yTextPosition = BMARGIN + (dataStructure->hyperPeriod - i - 1) * CYCLEHEIGHT + (CYCLEHEIGHT - FONTSIZE);
        doc << Text(Point(xTextPosition, yTextPosition), ToString(i), Color::Black, Font(FONTSIZE, "csr10"));
    }

    std::vector<bool> isNodeUsedInThisVariant(static_cast<unsigned long>(dataStructure->nodeCount), false);
    for (int i = 0; i < dataStructure->signalsCount; i++) {
        if (variantID == -1 || dataStructure->modelVariants[variantID][i] == 1)
            isNodeUsedInThisVariant[dataStructure->signalNodeIDs[i] - 1] = true;
    }

    for (i = 0; i < dataStructure->nodeCount; i++) {
        if (!isNodeUsedInThisVariant[i])
            continue;
        int slotOrder = 0;
        std::set<int>::iterator it;
        for (it = nodeSlotAssignment[i].begin(); it != nodeSlotAssignment[i].end(); it++) {
            if (*it < 0) // Signal is not scheduled in given channel - for ChannelScheduling
                continue;
            xTextPosition = LMARGIN + ((2 * (*it) + 1) * dataStructure->slotLength * BITWIDTH) / 2 - FONTSIZE / 2;
            doc << Text(Point(xTextPosition, BMARGIN - FONTSIZE - LABELMARGIN), ToString(i + 1),
                        Color::Black, Font(FONTSIZE, "csr10"));
            if (i < 9) {
                doc << Text(Point(xTextPosition + FONTWIDTH + 1, BMARGIN - FONTSIZE - LABELMARGIN - 4),
                            ToString(slotOrder), Color::Black, Font(2 * FONTSIZE / 3, "csr10"));
            }
            else {
                doc << Text(Point(xTextPosition + 2 * FONTWIDTH + 1, BMARGIN - FONTSIZE - LABELMARGIN - 4),
                            ToString(slotOrder), Color::Black, Font(2 * FONTSIZE / 3, "csr10"));
            }
            slotOrder++;
        }
    }

    delete[] nodeSlotAssignment;
}

void drawArrows(int xDim, int yDim, Document &doc) {
    Polygon xArrow(Fill(Color::Black), Stroke(1, Color::Black));
    xArrow << Point(xDim - 4, BMARGIN - 2) << Point(xDim, BMARGIN) << Point(xDim - 4, BMARGIN + 2);
    doc << xArrow;

    Polygon yArrow(Fill(Color::Black), Stroke(1, Color::Black));
    yArrow << Point(LMARGIN - 2, yDim - 4) << Point(LMARGIN, yDim) << Point(LMARGIN + 2, yDim - 4);
    doc << yArrow;
}

double JetColorMap::interpolate(double val, double y0, double x0, double y1, double x1) {
    return (val - x0) * (y1 - y0) / (x1 - x0) + y0;
}

double JetColorMap::base(double val) {
    if (val <= -0.75) return 0;
    else if (val <= -0.25) return interpolate(val, 0.0, -0.75, 1.0, -0.25);
    else if (val <= 0.25) return 1.0;
    else if (val <= 0.75) return interpolate(val, 1.0, 0.25, 0.0, 0.75);
    else return 0.0;
}

double JetColorMap::red(double gray) {
    return base(gray - 0.5);
}

double JetColorMap::green(double gray) {
    return base(gray);
}

double JetColorMap::blue(double gray) {
    return base(gray + 0.5);
}

svg::Color JetColorMap::getColour(double gray) {
    double grayScale = gray * 2 - 1;
    return Color(static_cast<int>(red(grayScale) * 256),
                 static_cast<int>(green(grayScale) * 256),
                 static_cast<int>(blue(grayScale) * 256));
}