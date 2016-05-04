#ifndef ground_truth_mode_h
#define ground_truth_mode_h

#include "cmdparser.hpp"

/**
 * This mode lets you collect ground truth positions for an object by following
 * it with your mouse. Click to toggle the recording of the positions (when the
 * positions are recording, you'll see text printed to the console).
 */
namespace OT {
    namespace Mode {
        namespace GroundTruth {
            void run(const cli::Parser& parser);
        }
    }
}


#endif /* ground_truth_mode_h */
