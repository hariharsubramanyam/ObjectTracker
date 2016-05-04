#ifndef DRAW_UTILS_HPP
#define DRAW_UTILS_HPP

#include <vector>

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
      
      cv::Point drawBoundingRect(cv::Mat& img,
                            const cv::Rect& boundingRect);
      
      void drawTrajectory(const cv::Mat& img,
                          const std::vector<cv::Point>& trajectory,
                          const cv::Scalar color);
      
      /**
       * Draw the contours in a new image and show them.
       */
      void contourShow(std::string drawingName,
                       const std::vector<std::vector<cv::Point>>& contours,
                       const std::vector<cv::Rect>& boundingRect,
                       cv::Size imgSize);
  }
}

#endif
