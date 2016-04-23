#include "draw_utils.hpp"
#include <opencv2/opencv.hpp>

namespace OT {
  namespace DrawUtils {
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
      
      cv::Point drawBoundingRect(cv::Mat& img,
                            const cv::Rect& boundingRect) {
          cv::rectangle(img,
                        boundingRect.tl(),
                        boundingRect.br(),
                        cv::Scalar(0, 255, 0),
                        2,
                        8,
                        0);
          cv::Point center = cv::Point(boundingRect.x + (boundingRect.width /2),
                                       boundingRect.y + (boundingRect.height/2));
          cv::circle(img,center, 3, cv::Scalar(0, 0, 255), -1, 1,0);
          return center;
      }
  }
}
