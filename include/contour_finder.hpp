#ifndef contour_finder_h
#define contour_finder_h

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

namespace OT {
    /**
     * This class will find blobs representing objects in a frame. It uses
     * background subtraction to isolate the foreground, does some preprocessing, finds
     * contours, and removes small contours.
     */
    class ContourFinder {
    private:
        // The background subtractor that isolates the foreground.
        cv::Ptr<cv::BackgroundSubtractorMOG2> bg;
        
        // The foreground of the frame that should contain the blobs.
        cv::Mat foreground;
        
        // Remove contours that are too small.
        void filterOutBadContours(std::vector<std::vector<cv::Point>>& contours);
    public:
        ContourFinder();
        
        /**
         * Find contours representing the objects in the frame.
         */
        void findContours(const cv::Mat& frame,
                          std::vector<cv::Vec4i>& hierarchy,
                          std::vector<std::vector<cv::Point>>& contours);
    };
}

#endif /* contour_finder_h */
