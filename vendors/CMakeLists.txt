option(SPDLOG_BUILD_TESTING "Build spdlog tests" OFF)
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "Build examples")

add_subdirectory(spdlog)

list (APPEND VENDORS_INCLUDES ${VENDORS_SOURCE_DIR}/spdlog/include)

set (GMON_VENDORS_INCLUDES  ${VENDORS_INCLUDES}  PARENT_SCOPE)
set (GMON_VENDORS_LIBRARIES ${VENDORS_LIBRARIES} PARENT_SCOPE)

set (BUILD_SHARED_LIBS OFF CACHE BOOL "We don't need shared libs" FORCE)
set (BUILD_TESTING OFF CACHE BOOL "We don't need test libs" FORCE)
set (BUILD_NC_TESTS OFF CACHE BOOL "We don't need test libs" FORCE)
set (BUILD_CONFIG_TESTS OFF CACHE BOOL "We don't need test libs" FORCE)
add_subdirectory(gflags)