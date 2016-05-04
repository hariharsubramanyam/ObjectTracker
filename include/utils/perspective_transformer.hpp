#ifndef perspective_transformer_h
#define perspective_transformer_h

#include <opencv2/opencv.hpp>
#include <vector>

namespace OT {
    namespace Perspective {
        /**
         * Sort the given vector of four points such that they have
         * the order:
         *
         *   (top left, top right, bottom right, bottom left).
         */
        void sortFourPoints(std::vector<cv::Point2f>& fourPoints);
        
        /**
         * Given four points, compute the perspective transform matrix.
         * The four points will be sorted (see above) after completion.
         * The size will contain the height and width of the perspective
         * transform. The perspective transform matrix will be returned.
         */
        cv::Mat getPerspectiveMatrix(std::vector<cv::Point2f>& fourPoints,
                                     cv::Size& size);
        
        /**
         * Given a vector of 8 ints, convert them into four points.
         */
        void extractFourPoints(const std::vector<int> &ints,
                               std::vector<cv::Point2f> &points);
    }
}

#endif /* perspective_transformer_h */
