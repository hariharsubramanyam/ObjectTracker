#include "utils/draw_utils.hpp"

#include <vector>

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
      
      void drawTrajectory(const cv::Mat& img,
                          const std::vector<cv::Point>& trajectory,
                          const cv::Scalar color) {
          if (trajectory.size() < 2) {
              return;
          }
          for (size_t i = 0; i < trajectory.size() - 1; i++) {
              line(img, trajectory[i], trajectory[i+1], color, 1, CV_AA, 0);
          }
      }
      
      void contourShow(std::string drawingName,
                       const std::vector<std::vector<cv::Point>>& contours,
                       const std::vector<cv::Rect>& boundingRect,
                       cv::Size imgSize) {
          cv::Mat drawing = cv::Mat::zeros(imgSize, CV_32FC3);
          for (size_t i = 0; i < contours.size(); i++) {
              cv::drawContours(drawing,
                               contours,
                               i,
                               cv::Scalar::all(127),
                               CV_FILLED,
                               8,
                               std::vector<cv::Vec4i>(),
                               0,
                               cv::Point());
              OT::DrawUtils::drawBoundingRect(drawing, boundingRect[i]);
          }
          cv::imshow(drawingName, drawing);
      }
  }
}
