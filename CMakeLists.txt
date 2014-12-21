 cmake_minimum_required(VERSION 2.8)

project(BeyondEngine)

if (CMAKE_SYSTEM_NAME MATCHES "Linux") 
if (ANDROID)
else()
    set (LINUX True)
    MESSAGE(STATUS "current platform: Linux ") 
endif()
endif()

set(BeyondEngine_DIR ${CMAKE_SOURCE_DIR})
include(${BeyondEngine_DIR}/cmake/SourceGroupByDir.cmake)
include(${BeyondEngine_DIR}/cmake/CMakeCommon.cmake)
if(WIN32)
	include(${BeyondEngine_DIR}/cmake/MSVCPCH.cmake)
elseif(APPLE)
	include(${BeyondEngine_DIR}/cmake/iOS.cmake)
endif()
include(${BeyondEngine_DIR}/cmake/BeyondEngineCommon.cmake)

include_common_dirs()

add_subdirectory(SourceCode)
add_subdirectory(Sample)
add_subdirectory(GameProject/StarRaiders)

if(WIN32)
elseif(APPLE)
elseif(ANDROID)
	MESSAGE("\n\n*********************ANDROID BUILD ENV************************************\n"
			"CMAKE_BUILD_TYPE:" ${CMAKE_BUILD_TYPE} "\n"
			"ANDROID_STL:" ${ANDROID_STL} "\n"
			"ANDROID_ABI:" ${ANDROID_ABI} "\n"
			"ANDROID_NATIVE_API_LEVEL:" ${ANDROID_NATIVE_API_LEVEL} "\n"
			"ANDROID_TOOLCHAIN_NAME:" ${ANDROID_TOOLCHAIN_NAME} "\n"
			"CMAKE_CXX_FLAGS:" ${CMAKE_CXX_FLAGS} "\n"
			"CMAKE_C_FLAGS:" ${CMAKE_C_FLAGS} "\n"
			"*********************************************************\n\n")
endif()
