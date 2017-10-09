cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
project (graphmon-mqtt CXX)

# vendor (third-party) libraries
set (VENDORS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendors)
set (VENDORS_BINARY_DIR ${PROJECT_BINARY_DIR}/vendors)
add_subdirectory(vendors)

# own contributions to vendor libraries
list (APPEND GMON_VENDORS_INCLUDES ${PROJECT_SOURCE_DIR}/contrib/spdlog/include)

list (APPEND GMON_PROJECT_INCLUDES ${PROJECT_SOURCE_DIR}/src)

link_directories(${PROJECT_SOURCE_DIR}/vendors/nvidia/nvml/lib/x64)
link_directories(${PROJECT_SOURCE_DIR}/vendors/pahomqtt/lib)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(buildinfo)

## Google Test Download begin
# Download and unpack googletest at configure time
if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject)
download_project(PROJ                googletest
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             master
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

# The gtest/gmock targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if(CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
                        "${gmock_SOURCE_DIR}/include")
endif()
## Google Test Download end


# our projects
add_subdirectory(src/agent)
