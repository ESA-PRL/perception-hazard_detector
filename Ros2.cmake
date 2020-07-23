project(hazard_detector)

add_definitions(-DROS2)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

find_package(OpenCV REQUIRED)

# add library
add_library(${PROJECT_NAME} STATIC src/HazardDetector.cpp)

# specify include paths and dependencies
target_include_directories(${PROJECT_NAME} PRIVATE src PRIVATE include)
ament_target_dependencies(${PROJECT_NAME} rclcpp OpenCV)

# copy public headers to destination
install(
  DIRECTORY include/
  DESTINATION include
)

# install target at destination
install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

# export information for upstream packages
ament_export_libraries(${PROJECT_NAME})
ament_export_include_directories(include)

ament_package()