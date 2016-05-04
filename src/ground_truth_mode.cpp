#include "ground_truth_mode.hpp"

#include <opencv2/opencv.hpp>

#include "cmdparser.hpp"
#include "ground_truth_log.hpp"
#include "utils.hpp"
#include "perspective_transformer.hpp"

namespace OT {
    namespace Mode {
        namespace GroundTruth {
            
            // Whether the user is currently dragging the object.
            bool isDragging = false;
            
            // We'll count the frame with this variable.
            long frameNumber = 0;
            
            // Whether the callback should be triggered to log the current annotation.
            bool triggerCallback = false;
            
            // The most recently recorded annotation.
            OT::GroundTruth::Annotation currentAnnotation;
            
            void mouseHandler(int event, int x, int y, int flags, void* param) {
                if (event == CV_EVENT_LBUTTONUP && !isDragging) {
                    isDragging = true;
                    triggerCallback = true;
                } else if (event == CV_EVENT_MOUSEMOVE && isDragging) {
                    currentAnnotation = OT::GroundTruth::Annotation{frameNumber, x, y};
                    triggerCallback = true;
                } else if (event == CV_EVENT_LBUTTONUP && isDragging) {
                    isDragging = false;
                    triggerCallback = false;
                }
            }
            
            void run(const cli::Parser& parser) {
                // We'll use this variable to store the current frame captured from the video.
                cv::Mat frame;
                
                // This object represents the video or image sequence that we are reading from.
                cv::VideoCapture capture;
                
                // This object will log the annotations.
                OT::GroundTruth::Log logger;
                
                // Open the video.
                capture.open(parser.get<std::string>("i"));
                
                // Read the second positional command line argument and use that as the log
                // for the output file.
                bool hasOutputFile = false;
                std::ofstream outputFile;
                if (!parser.get<std::string>("s").empty()) {
                    hasOutputFile = true;
                    outputFile.open(parser.get<std::string>("s"));
                }
                
                // Ensure that the video has been opened correctly.
                if(!capture.isOpened()) {
                    std::cerr << "Problem opening video source" << std::endl;
                }
                
                // Set the mouse callback.
                cv::namedWindow("Video");
                cv::setMouseCallback("Video", mouseHandler);
                
                // Determine how to scale the video.
                int maxDimension = parser.get<int>("d");
                
                // Get the perspective transform, if there is one.
                auto perspectivePoints = parser.get<std::vector<int>>("p");
                cv::Mat perspectiveMatrix;
                cv::Size perspectiveSize;
                std::vector<cv::Point2f> points;
                OT::Perspective::extractFourPoints(perspectivePoints, points);
                if (!points.empty()) {
                    perspectiveMatrix = OT::Perspective::getPerspectiveMatrix(points, perspectiveSize);
                }
                
                while(OT::Utils::hasFrame(capture)) {
                    // Fetch the next frame.
                    capture.retrieve(frame);
                    frameNumber++;
                    
                    // Do the perspective transform.
                    if (!points.empty()) {
                        cv::warpPerspective(frame, frame, perspectiveMatrix, perspectiveSize);
                    }
                    
                    // Scale the image.
                    OT::Utils::scale(frame, maxDimension);
                    
                    // Show the frame
                    imshow("Video", frame);
                    
                    if (triggerCallback) {
                        logger.addAnnotation(currentAnnotation);
                        std::cout << currentAnnotation.x << "," << currentAnnotation.y << std::endl;
                        triggerCallback = false;
                    }
                }
                
                // Log the output file if we need to.
                if (hasOutputFile) {
                    logger.writeToStream(outputFile);
                    outputFile.close();
                }
            } // run
        } // Tracking
    } // Mode
} // OT