cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 14)
project( ObjectTracker )
find_package( OpenCV REQUIRED )

set( NAME_SRC
    src/main.cpp
    src/utils/draw_utils.cpp
    src/tracker/kalman_tracker.cpp
    src/tracker/multi_object_tracker.cpp
    src/lib/hungarian.cpp
    src/tracker/contour_finder.cpp
    src/lib/disjoint_set.cpp
    src/tracker/tracker_log.cpp
    src/utils/perspective_transformer.cpp
    src/utils/utils.cpp
    src/modes/tracking_mode.cpp
    src/modes/plotting_mode.cpp
    src/ground_truth/ground_truth_log.cpp
    src/modes/ground_truth_mode.cpp
)

set( NAME_HEADERS
    include/utils/draw_utils.hpp
    include/tracker/kalman_tracker.hpp
    include/tracker/multi_object_tracker.hpp
    include/lib/hungarian.hpp
    include/tracker/contour_finder.hpp
    include/lib/disjoint_set.hpp
    include/tracker/tracker_log.hpp
    include/lib/json.hpp
    include/lib/cmdparser.hpp
    include/utils/perspective_transformer.hpp
    include/utils/utils.hpp
    include/modes/tracking_mode.hpp
    include/lib/csv.hpp
    include/modes/plotting_mode.hpp
    include/ground_truth/ground_truth_log.hpp
    include/modes/ground_truth_mode.hpp
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )
add_executable( main ${NAME_SRC} ${NAME_HEADERS})
target_link_libraries( main ${OpenCV_LIBS} )
