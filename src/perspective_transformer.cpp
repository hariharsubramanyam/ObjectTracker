#include "perspective_transformer.hpp"

#include <opencv2/opencv.hpp>
#include <algorithm>
#include <vector>

namespace OT {
    namespace Perspective {
        void sortFourPoints(std::vector<cv::Point2f>& fourPoints) {
            // Ensure there are only four points.
            assert(fourPoints.size() == 4);
            
            // Find the sums and differences.
            std::vector<float> sums(4);
            std::vector<float> differences(4);
            std::transform(fourPoints.cbegin(),
                           fourPoints.cend(),
                           sums.begin(),
                           [](cv::Point2f pt) {
                               return pt.x + pt.y;
                           });
            std::transform(fourPoints.cbegin(),
                           fourPoints.cend(),
                           differences.begin(),
                           [](cv::Point2f pt) {
                               return pt.y - pt.x;
                           });
            
            // Find the points that minimize the respective sums and differences;
            size_t minDifferenceIndex = std::min_element(differences.cbegin(),
                                                         differences.cend()) -differences.cbegin();
            size_t maxDifferenceIndex = std::max_element(differences.cbegin(),
                                                         differences.cend()) - differences.cbegin();
            size_t minSumIndex = std::min_element(sums.cbegin(),
                                                  sums.cend()) - sums.cbegin();
            size_t maxSumIndex = std::max_element(sums.cbegin(),
                                                  sums.cend()) - sums.cbegin();
            
            // Create a vector with the points ordered as follows:
            // (top left, top right, bottom right, bottom left).
            std::vector<cv::Point2f> copied = {
                fourPoints[minSumIndex],
                fourPoints[minDifferenceIndex],
                fourPoints[maxSumIndex],
                fourPoints[maxDifferenceIndex]
            };
            
            // Copy this into the original vector.
            std::copy(copied.cbegin(), copied.cend(), fourPoints.begin());
        }
        
        cv::Mat getPerspectiveMatrix(std::vector<cv::Point2f> &fourPoints,
                                     cv::Size &size) {
            // Ensure there are four points.
            assert(fourPoints.size() == 4);
            
            // Sort the points.
            sortFourPoints(fourPoints);
            
            // Get the four corners.
            auto tl = fourPoints[0];
            auto tr = fourPoints[1];
            auto br = fourPoints[2];
            auto bl = fourPoints[3];
            
            // Compute the dimensions of a frame after perspective transform.
            auto widthA = cv::norm(br - bl);
            auto widthB = cv::norm(tr - tl);
            float maxWidth = std::max(widthA, widthB);
            
            auto heightA = cv::norm(tr - br);
            auto heightB = cv::norm(tl - bl);
            float maxHeight = std::max(heightA, heightB);
            
            // Set the size as these dimensions.
            size.width = maxWidth;
            size.height = maxHeight;
            
            // Create the output array.
            std::vector<cv::Point2f> output = {
                {0, 0},
                {maxWidth - 1, 0},
                {maxWidth - 1, maxHeight - 1},
                {0, maxHeight - 1}
            };
            
            // Create the matrix.
            return cv::getPerspectiveTransform(fourPoints, output);
        }
    }
}