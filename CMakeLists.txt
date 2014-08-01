cmake_minimum_required(VERSION 2.8)

project(BeyondEngine)

if(WIN32)
elseif(APPLE)
else()
	MESSAGE("\n\n*********************ANDROID BUILD ENV************************************\n"
			"CMAKE_BUILD_TYPE:" ${CMAKE_BUILD_TYPE} "\n"
			"ANDROID_STL:" ${ANDROID_STL} "\n"
			"ARM_TARGET:" ${ARM_TARGET} "\n"
			"ANDROID_API_LEVEL:" ${ANDROID_API_LEVEL} "\n"
			"ANDROID_TOOLCHAIN_NAME:" ${ANDROID_TOOLCHAIN_NAME} "\n"
			"CMAKE_CXX_FLAGS:" ${CMAKE_CXX_FLAGS} "\n"
			"CMAKE_C_FLAGS:" ${CMAKE_C_FLAGS} "\n"
			"ANDROID_ABI:" ${ANDROID_ABI} "\n"
			"*********************************************************\n\n")
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