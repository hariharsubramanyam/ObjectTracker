#ifndef DRAW_UTILS_HPP
#define DRAW_UTILS_HPP

#include <opencv2/opencv.hpp>

namespace OT {
  namespace DrawUtils {
    /**
     * Draw a cross on the image.
     *
     * img - The image
     * center - The center of the cross.
     * color - The RGB color.
     * diameter - The diameter of the cross.
     */
     void drawCross(const cv::Mat& img,
       const cv::Point& center,
       const cv::Scalar color,
       const int diameter);
  }
}

#endif
