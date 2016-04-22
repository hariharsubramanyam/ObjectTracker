#include "contour_finder.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

namespace OT {
    ContourFinder::ContourFinder(int history,
                                 int nMixtures,
                                 bool detectShadows,
                                 double shadowThreshold,
                                 int contourSizeThreshold,
                                 int medianFilterSize) {
        this->bg = cv::createBackgroundSubtractorMOG2();
        this->bg->setHistory(history);
        this->bg->setNMixtures(nMixtures);
        this->bg->setDetectShadows(detectShadows);
        this->bg->setShadowThreshold(shadowThreshold);
        this->contourSizeThreshold = contourSizeThreshold;
        this->medianFilterSize = medianFilterSize;
    }
    
    /**
     * Remove contours if they are are too small.
     */
    void ContourFinder::filterOutBadContours(std::vector<std::vector<cv::Point>>& contours) {
        int threshold = this->contourSizeThreshold;
        // Remove contours that have a size less than the contour size threshold.
        auto removeThese = std::remove_if(contours.begin(), contours.end(), [threshold](std::vector<cv::Point> contour) {
            return cv::contourArea(contour) <= threshold;
        });
        contours.erase(removeThese, contours.end());
    }
    
    void ContourFinder::findContours(const cv::Mat &frame,
                                     std::vector<cv::Vec4i> &hierarchy,
                                     std::vector<std::vector<cv::Point> > &contours) {
        // First clear the conotour and hierarchy objects.
        contours.clear();
        hierarchy.clear();
        
        // Find the foreground.
        this->bg->apply(frame, this->foreground);
        
        // Get rid little specks of noise by doing a median blur.
        // The median blur is good for salt-and-pepper noise, not Gaussian noise.
        cv::medianBlur(this->foreground, this->foreground, this->medianFilterSize);
        
        // Dilate the image to make the blobs larger.
        cv::dilate(this->foreground, this->foreground, cv::Mat());
        
        // Find the contours.
        cv::findContours(this->foreground, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
        
        // Keep only those contours that are sufficiently large.
        this->filterOutBadContours(contours);
    }
}