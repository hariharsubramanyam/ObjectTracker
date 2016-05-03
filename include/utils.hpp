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
        
        /**
         * Resize the image so that neither # rows nor # cols exceed maxDimension.
         * Preserve the aspect ratio though.
         * Set maxDimension = -1 if you don't want to do any scaling.
         */
        void scale(cv::Mat& img, int maxDimension) {
            if (maxDimension == -1) {
                return;
            }
            if (maxDimension >= img.rows && maxDimension >= img.cols) {
                return;
            }
            
            double scale = (1.0 * maxDimension) / img.rows;
            if (img.cols > img.rows) {
                scale = (1.0 * maxDimension) / img.cols;
            }
            
            int newRows = img.rows * scale;
            int newCols = img.cols * scale;
            
            cv::resize(img, img, cv::Size(newCols, newRows));
        }
    }
}

#endif /* utils_h */
