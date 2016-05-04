cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 14)
project( ObjectTracker )
find_package( OpenCV REQUIRED )

set( NAME_SRC
    src/ground_truth/ground_truth_log.cpp
    src/lib/disjoint_set.cpp
    src/lib/hungarian.cpp
    src/modes/ground_truth_mode.cpp
    src/modes/plotting_mode.cpp
    src/modes/tracking_mode.cpp
    src/tracker/contour_finder.cpp
    src/tracker/kalman_tracker.cpp
    src/tracker/multi_object_tracker.cpp
    src/tracker/tracker_log.cpp
    src/utils/draw_utils.cpp
    src/utils/perspective_transformer.cpp
    src/utils/utils.cpp
    src/main.cpp
)

set( NAME_HEADERS
    include/ground_truth/ground_truth_log.hpp
    include/lib/cmdparser.hpp
    include/lib/csv.hpp
    include/lib/disjoint_set.hpp
    include/lib/hungarian.hpp
    include/lib/json.hpp
    include/modes/ground_truth_mode.hpp
    include/modes/plotting_mode.hpp
    include/modes/tracking_mode.hpp
    include/tracker/contour_finder.hpp
    include/tracker/kalman_tracker.hpp
    include/tracker/multi_object_tracker.hpp
    include/tracker/tracker_log.hpp
    include/utils/draw_utils.hpp
    include/utils/perspective_transformer.hpp
    include/utils/utils.hpp
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )
add_executable( main ${NAME_SRC} ${NAME_HEADERS})
target_link_libraries( main ${OpenCV_LIBS} )
