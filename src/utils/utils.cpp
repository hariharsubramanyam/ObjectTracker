#include "utils/utils.hpp"

#include <opencv2/opencv.hpp>

namespace OT {
    namespace Utils {
        bool hasFrame(cv::VideoCapture& capture) {
            bool hasNotQuit = ((char) cv::waitKey(1)) != 'q';
            bool hasAnotherFrame = capture.grab();
            return hasNotQuit && hasAnotherFrame;
        }
        
        void scale(cv::Mat& img, int maxDimension) {
            if (maxDimension == -1) {
                return;
            }
            if (maxDimension >= img.rows && maxDimension >= img.cols) {
                return;
            }
            
            double scale = (1.0 * maxDimension) / img.rows;
            if (img.cols > img.rows) {
                scale = (1.0 * maxDimension) / img.cols;
            }
            
            int newRows = img.rows * scale;
            int newCols = img.cols * scale;
            
            cv::resize(img, img, cv::Size(newCols, newRows));
        }
    }
}
