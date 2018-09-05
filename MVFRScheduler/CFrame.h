/// \file CFrame.h Frame declaration for improved first fit scheduler
#ifndef CFRAME_H_
#define CFRAME_H_

/// Multiframe structure
struct CFrame {
    std::vector<int> signals; ///< Vector of signals scheduled in the frame
};

#endif /* CFRAME_H_ */ 