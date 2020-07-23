if (DEFINED ENV{ROCK_PREFIX})
	rock_library(hazard_detector
	    SOURCES HazardDetector.cpp
	    HEADERS HazardDetector.hpp Config.hpp)
	set (CMAKE_CXX_STANDARD 11)
endif()