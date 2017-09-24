set (TARGET_NAME agent)

project(${TARGET_NAME})

generate_build_info(
    "${PROJECT_SOURCE_DIR}/inc/buildversion.h.in"
    "${PROJECT_BINARY_DIR}/buildversion.h"
)

set (SOURCES 
	src/app.cpp
	src/app.h
	src/stdafx.cpp
	src/stdafx.h
	src/util.h
	src/manager_mqtt.cpp
	src/manager_nvml.cpp
	inc/manager_mqtt.h
	inc/manager_nvml.h
	res/version.rc)

if (WIN32)
    set (SOURCES_PLATFORM
        src/util_win.cpp
    )
elseif (UNIX)
    set (SOURCES_PLATFORM
        src/util_linux.cpp
    )
endif ()

add_executable(${TARGET_NAME} ${SOURCES} ${SOURCES_PLATFORM})

target_include_directories (${TARGET_NAME}
    PRIVATE ${GMON_VENDORS_INCLUDES}
)
target_include_directories (${TARGET_NAME}
    PRIVATE ${GMON_PROJECT_INCLUDES}
)

include_directories(inc)
include_directories(src)
include_directories("${PROJECT_BINARY_DIR}")

if(MSVC)
	set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
	target_compile_options (${TARGET_NAME}
    	PRIVATE /MP
	)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yustdafx.h")
	set_source_files_properties(src/stdafx.cpp
	    PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")

endif(MSVC)

if (UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif(UNIX)

add_definitions(-DUNICODE -D_UNICODE -D_NO_ASYNCRTIMP)

IF (WIN32)
  SET(OPENSSL_SEARCH_PATH "C:/OpenSSL-Win64")
ENDIF ()

FIND_LIBRARY(OPENSSL_LIB NAMES ssl libssl ssleay32
    HINTS ${OPENSSL_SEARCH_PATH}/lib ${OPENSSL_SEARCH_PATH}/lib64)
FIND_LIBRARY(OPENSSLCRYPTO_LIB NAMES crypto libcrypto libeay32
    HINTS ${OPENSSL_SEARCH_PATH}/lib ${OPENSSL_SEARCH_PATH}/lib64)

if (WIN32)
  SET(LIB_PLATFORM
    nvml
    paho-mqtt3cs
    ws2_32
    winmm
    )
elseif (UNIX)
  SET(LIB_PLATFORM
        nvidia-ml
	paho-mqtt3cs
    )
endif()

target_link_libraries (${TARGET_NAME} gflags ${OPENSSL_LIB} ${OPENSSLCRYPTO_LIB} cpprest ${LIB_PLATFORM})
