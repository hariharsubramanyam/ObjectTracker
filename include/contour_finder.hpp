#ifndef contour_finder_h
#define contour_finder_h

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

namespace OT {
    class ContourFinder {
    private:
        cv::Ptr<cv::BackgroundSubtractorMOG2> bg;
        cv::Mat foreground;
        
        void filterOutBadContours(std::vector<std::vector<cv::Point>>& contours);
    public:
        ContourFinder();
        void findContours(const cv::Mat& frame,
                          std::vector<cv::Vec4i>& hierarchy,
                          std::vector<std::vector<cv::Point>>& contours);
    };
}

#endif /* contour_finder_h */
