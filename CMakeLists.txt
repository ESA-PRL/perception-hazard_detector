# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
macro(use_cxx11)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set (CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")
        endif ()
    else ()
        set (CMAKE_CXX_STANDARD 11)
    endif ()
endmacro(use_cxx11)

use_cxx11()

cmake_minimum_required(VERSION 2.6)
find_package(Rock)
rock_init(hazard_detector 0.1)
rock_opencv_autodetect(OPENCV_PACKAGE)
rock_standard_layout()
set (CMAKE_CXX_STANDARD 11)
