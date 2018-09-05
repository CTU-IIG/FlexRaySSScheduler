/// \file VariantFrame.h Frame structure used by Naive First Fit scheduler
#ifndef VARIANTFRAME_H_
#define VARIANTFRAME_H_

/// Frame structure for Naive First Fit scheduler
struct VariantFrame {
    std::vector<int> signals; ///< Vector of signals placed to the frame
    int node; ///< Node ID which transmits this frame.
};

#endif /*VARIANTFRAME_H_*/