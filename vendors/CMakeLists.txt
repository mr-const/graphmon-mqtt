option(SPDLOG_BUILD_TESTING "Build spdlog tests" OFF)
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "Build examples")

add_subdirectory(spdlog)

list (APPEND VENDORS_INCLUDES ${VENDORS_SOURCE_DIR}/spdlog/include)

set (BUILD_SHARED_LIBS OFF CACHE BOOL "We don't need shared libs" FORCE)
set (BUILD_TESTING OFF CACHE BOOL "We don't need test libs" FORCE)
set (BUILD_NC_TESTS OFF CACHE BOOL "We don't need test libs" FORCE)
set (BUILD_CONFIG_TESTS OFF CACHE BOOL "We don't need test libs" FORCE)
add_subdirectory(gflags)

set (CPPRESTSDK_SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendors/cpprestsdk/Release)
set (CPPRESTSDK_BINARY_DIR ${PROJECT_BINARY_DIR}/vendors/cpprestsdk/Release)
set (BUILD_SHARED_LIBS OFF CACHE BOOL "Don't Build CPPREST Shared Libs")
set(BUILD_TESTS OFF CACHE BOOL "Don't Build CPPREST tests.")
set(BUILD_SAMPLES OFF CACHE BOOL "Don't Build CPPREST sample applications.")

set (CPPREST_EXCLUDE_WEBSOCKETS ON CACHE BOOL "Don't use Websockets/Boost")
set (CPPREST_EXCLUDE_COMPRESSION ON CACHE BOOL "Don't use zlib")
set (CPPREST_WEBSOCKETS_IMPL "none")
add_subdirectory(cpprestsdk/Release)

list (APPEND VENDORS_INCLUDES ${VENDORS_SOURCE_DIR}/cpprestsdk/Release/include)
list (APPEND VENDORS_INCLUDES ${VENDORS_SOURCE_DIR}/cpprestsdk/Release/include)

list (APPEND VENDORS_INCLUDES ${VENDORS_SOURCE_DIR}/nvidia/nvml/include)
list (APPEND VENDORS_INCLUDES ${VENDORS_SOURCE_DIR}/pahomqtt/include)

set (GMON_VENDORS_INCLUDES  ${VENDORS_INCLUDES}  PARENT_SCOPE)
set (GMON_VENDORS_LIBRARIES ${VENDORS_LIBRARIES} PARENT_SCOPE)
