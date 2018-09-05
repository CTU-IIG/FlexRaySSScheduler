/// \file BPosition.h Position structure for best-fit search - slotID/cycleID/offsetInFrame/evaluation
#ifndef BPOSITION_H_
#define BPOSITION_H_

/// Position structure
struct BPosition {
    int slot; ///< SlotID
    int cycle; ///< CycleID
    int inFrameOffset; ///< Offset in the frame
    int evaluation; ///< Evaluation (fitness) of the position
};

#endif /* BPOSITION_H_ */ 