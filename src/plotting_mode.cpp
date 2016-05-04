#include "plotting_mode.hpp"

#include <string>
#include <vector>
#include <unordered_map>

#include <opencv2/opencv.hpp>

#include "cmdparser.hpp"
#include "csv.hpp"
#include "utils.hpp"
#include "perspective_transformer.hpp"

/**
 * Plots points listed in the (timestamp, x, y, frame) CSV overlaid on a video.
 * If you have a (timestamp, x, y) file, you can get the frame number by using the
 * script in scripts/timestamp_to_frame.py.
 */
namespace OT {
    namespace Mode {
        namespace Plotting {
            // Represents an entry of the track.
            struct TrackEntry {
                double timestamp;
                long frameNumber;
                int x;
                int y;
            };
            
            // Read the CSV and output track entries.
            void readCsv(std::string path, std::vector<TrackEntry>& outputTracks) {
                outputTracks.clear();
                
                io::CSVReader<4> in(path);
                in.read_header(io::ignore_no_column, "timestamp", "x", "y", "frame");
                double timestamp;
                long frameNumber;
                int x;
                int y;
                while(in.read_row(timestamp, x, y, frameNumber)){
                    outputTracks.push_back(TrackEntry{timestamp, frameNumber, x, y});
                }
            }
            
            void run(const cli::Parser& parser) {
                // We'll use this variable to store the current frame captured from the video.
                cv::Mat frame;
                
                // This object represents the video or image sequence that we are reading from.
                cv::VideoCapture capture;
                
                // We'll count the frame with this variable.
                long frameNumber = 0;
                
                // Read the track CSV.
                std::vector<TrackEntry> track;
                readCsv(parser.get<std::string>("s"), track);
                
                // Create a mapping from frame number to TrackEntry.
                std::unordered_map<long, TrackEntry> entryForFrame;
                for (auto entry : track) {
                    entryForFrame.insert(std::make_pair(entry.frameNumber, entry));
                }
                
                // Open the video.
                capture.open(parser.get<std::string>("i"));
                
                // Ensure that the video has been opened correctly.
                if(!capture.isOpened()) {
                    std::cerr << "Problem opening video source" << std::endl;
                }
                
                // Current track entry.
                TrackEntry currentTrackEntry{0, 0, 0, 0};
                
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
                
                // Repeat while the user has not pressed "q" and while there's another frame.
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
                    
                    // Update the current track entry.
                    if (entryForFrame.find(frameNumber) != entryForFrame.end()) {
                        currentTrackEntry = entryForFrame.at(frameNumber);
                    }
                    
                    // If we have something to draw this frame, draw it.
                    cv::circle(frame,
                               cv::Point(currentTrackEntry.x, frame.rows - currentTrackEntry.y),
                               4,
                               cv::Scalar::all(255),
                               -1);
                    
                    imshow("Video", frame);
                }

            } // run
        } // Tracking
    } // Mode
} // OT