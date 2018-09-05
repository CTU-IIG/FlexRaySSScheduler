/// \file Position.h Position structure - slotID/cycleID/offsetInFrame
#ifndef POSITION_H_
#define POSITION_H_

/// Position structure
struct Position {
    int slot; ///< SlotID
    int cycle; ///< CycleID
    int inFrameOffset; ///< Offset in the frame
};

#endif /* POSITION_H_ */ 