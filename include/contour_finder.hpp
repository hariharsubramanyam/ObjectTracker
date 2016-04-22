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
        
        // Filter out contours whose area is less than this size (in pixels).
        int contourSizeThreshold;
        
        // We use median filter to remove noise, this is the size of the filter.
        // It must be an odd number.
        int medianFilterSize;
    public:
        ContourFinder(int history = 1000,
                      int nMixtures = 3,
                      int contourSizeThreshold = 1000,
                      int medianFilterSize = 5);
        
        /**
         * Find contours representing the objects in the frame.
         */
        void findContours(const cv::Mat& frame,
                          std::vector<cv::Vec4i>& hierarchy,
                          std::vector<std::vector<cv::Point>>& contours);
        
        /**
         * Find the mass centers and bounding boxes for the given contours.
         */
        void getCentersAndBoundingBoxes(std::vector<std::vector<cv::Point>>& contours,
                                        std::vector<cv::Point2f>& massCenters,
                                        std::vector<cv::Rect>& boundingBoxes);
    };
}

#endif /* contour_finder_h */
