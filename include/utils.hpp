#ifndef utils_h
#define utils_h

#include <opencv2/opencv.hpp>

namespace OT {
    namespace Utils {
        /**
         * Check if there's another frame in the video capture. We do this by first checking if the user has quit (i.e. pressed
         * the "Q" key) and then trying to retrieve the next frame of the video.
         */
        bool hasFrame(cv::VideoCapture& capture) {
            bool hasNotQuit = ((char) cv::waitKey(1)) != 'q';
            bool hasAnotherFrame = capture.grab();
            return hasNotQuit && hasAnotherFrame;
        }
    }
}

#endif /* utils_h */
