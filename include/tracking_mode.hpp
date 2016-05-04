#ifndef tracking_mode_h
#define tracking_mode_h

#include "cmdparser.hpp"

/**
 * Plots points listed in the (timestamp, x, y, frame) CSV overlaid on a video.
 * If you have a (timestamp, x, y) file, you can get the frame number by using the
 * script in scripts/timestamp_to_frame.py.
 */
namespace OT {
    namespace Mode {
        namespace Tracking {
            void run(const cli::Parser& parser);
        }
    }
}


#endif /* tracking_mode_h */
