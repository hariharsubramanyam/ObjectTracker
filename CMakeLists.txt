cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 14)
project( ObjectTracker )
find_package( OpenCV REQUIRED )

set( NAME_SRC
    src/main.cpp
    src/draw_utils.cpp
    src/kalman_tracker.cpp
    src/multi_object_tracker.cpp
    src/hungarian.cpp
    src/contour_finder.cpp
    src/disjoint_set.cpp
    src/tracker_log.cpp
    src/perspective_transformer.cpp
    src/tracking_mode.cpp
)

set( NAME_HEADERS
    include/draw_utils.hpp
    include/kalman_tracker.hpp
    include/multi_object_tracker.hpp
    include/hungarian.hpp
    include/contour_finder.hpp
    include/disjoint_set.hpp
    include/tracker_log.hpp
    include/json.hpp
    include/cmdparser.hpp
    include/perspective_transformer.hpp
    include/tracking_mode.hpp
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )
add_executable( main ${NAME_SRC} ${NAME_HEADERS})
target_link_libraries( main ${OpenCV_LIBS} )
