cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
project (graphmon-mqtt CXX)

# vendor (third-party) libraries
set (VENDORS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendors)
set (VENDORS_BINARY_DIR ${PROJECT_BINARY_DIR}/vendors)
add_subdirectory(vendors)

list (APPEND GMON_VENDORS_INCLUDES ${PROJECT_SOURCE_DIR}/vendors/nvidia/nvml/include)
list (APPEND GMON_VENDORS_INCLUDES ${PROJECT_SOURCE_DIR}/vendors/pahomqtt/include)
# own contributions to vendor libraries
list (APPEND GMON_VENDORS_INCLUDES ${PROJECT_SOURCE_DIR}/contrib/spdlog/include)

list (APPEND GMON_PROJECT_INCLUDES ${PROJECT_SOURCE_DIR}/src)

link_directories(${PROJECT_SOURCE_DIR}/vendors/nvidia/nvml/lib/x64)
link_directories(${PROJECT_SOURCE_DIR}/vendors/pahomqtt/lib)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(buildinfo)

# our projects
add_subdirectory(src/agent)
