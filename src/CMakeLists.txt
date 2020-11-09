rock_library(hazard_detector
    SOURCES HazardDetector.cpp
    HEADERS HazardDetector.hpp Config.hpp
    DEPS_PKGCONFIG ${OPENCV_PACKAGE}
)
set (CMAKE_CXX_STANDARD 11)
