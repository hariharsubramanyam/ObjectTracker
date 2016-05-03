#include "tracking_mode.hpp"

#include <string>

#include "cmdparser.hpp"

int main(int argc, char **argv) {
    // Parse the command line arguments.
    cli::Parser parser(argc, argv);
    parser.set_required<std::string>("m", "The mode that the tracker should be run in: either tracker, plotter, annotater");
    
    // Arguments common to all modes.
    parser.set_required<std::string>("i", "input video");
    parser.set_optional<std::vector<int>>("p", "perspective_points", std::vector<int>(), "The perspective points");
    parser.set_optional<int>("d", "max_dimension", -1, "Scale the video so that the # rows and # cols do not exceed this value. Preserve the aspect ratio.");
    
    // Arguments for tracker mode.
    parser.set_optional<int>("w", "webcam", 0, "number to use (this will override -i)");
    parser.set_optional<std::string>("o", "output", "", "path to the output JSON file");
    
    parser.run_and_exit_if_error();
    
    if (parser.get<std::string>("m") == "tracker") {
        OT::Mode::Tracking::run(parser);
    }
    return 0;
}
