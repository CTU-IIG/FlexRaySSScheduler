/// \file Frame.h Data structure for the frame in clasic first fit scheduling algorithm
#ifndef FRAME_H_
#define FRAME_H_

/// Multiframe structure
struct Frame {
    std::vector<int> signals; ///< Vector of signals scheduled in the multiframe
    std::set<int> nodes; ///< Vector of nodes that transmits in the multiframe
};

#endif /* FRAME_H_ */ 