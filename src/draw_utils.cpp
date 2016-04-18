#include "draw_utils.hpp"
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
       const int diameter) {
         cv::Point p1 = cv::Point(center.x - diameter, center.y - diameter);
         cv::Point p2 = cv::Point(center.x + diameter, center.y + diameter);
         line(img, p1, p2, color, 2, CV_AA, 0);

         cv::Point p3 = cv::Point(center.x + diameter, center.y - diameter);
         cv::Point p4 = cv::Point(center.x - diameter, center.y + diameter);
         line(img, p3, p4, color, 2, CV_AA, 0);
     }
  }
}
