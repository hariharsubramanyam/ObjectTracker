#include "modes/tracking_mode.hpp"
#include "modes/plotting_mode.hpp"
#include "modes/ground_truth_mode.hpp"

#include <string>

#include "lib/cmdparser.hpp"

int main(int argc, char **argv) {
    // Parse the command line arguments.
    cli::Parser parser(argc, argv);
    parser.set_required<std::string>("m", "The mode that the tracker should be run in: either tracker, plotter, annotater");
    
    // Arguments common to all modes.
    parser.set_required<std::string>("i", "input video");
    parser.set_optional<std::vector<int>>("p", "perspective_points", std::vector<int>(), "The perspective points");
    parser.set_optional<int>("d", "max_dimension", -1, "Scale the video so that the # rows and # cols do not exceed this value. Preserve the aspect ratio.");
    parser.set_optional<std::string>("s", "support_file", "", "Path to the support file. If you're in tracker mode, this is the output JSON file for the tracker. If you're in plotter mode, this is the path to the tracks file, which is a (timestamp, x, y, frame) CSV");
    
    // Arguments for tracker mode.
    parser.set_optional<int>("w", "webcam", -1, "number to use (this will override -i)");
    
    parser.run_and_exit_if_error();
    
    auto mode = parser.get<std::string>("m");
    
    if (mode == "tracker") {
        OT::Mode::Tracking::run(parser);
    } else if (mode == "plotter") {
        OT::Mode::Plotting::run(parser);
    } else if (mode == "ground_truth") {
        OT::Mode::GroundTruth::run(parser);
    }
    return 0;
}
