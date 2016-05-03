#ifndef tracking_mode_h
#define tracking_mode_h

#include "cmdparser.hpp"

/**
 * This program is the main object tracker.
 */
namespace OT {
    namespace Mode {
        namespace Tracking {
            void run(const cli::Parser& parser);
        }
    }
}


#endif /* tracking_mode_h */
