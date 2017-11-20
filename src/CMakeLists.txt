rock_library(hazard_detector
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(hazard_detector_bin Main.cpp
    DEPS hazard_detector)

