set(ProjectName Sample)

#1. Set Include Path.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#2. Set App Generation.
file(GLOB_RECURSE SAMPLE_RESOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Resource/*.*)
set(SAMPLE_SOURCE_FILES main.cpp 
	SampleApplication.cpp)

if(WIN32)
elseif(APPLE)
else()
set(SAMPLE_SOURCE_FILES ${SAMPLE_SOURCE_FILES} 
						${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c
						${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
						${ANDROID_NDK}/sources/android/ndk_helper/JNIHelper.cpp
						${ANDROID_NDK}/sources/android/ndk_helper/interpolator.cpp
						${ANDROID_NDK}/sources/android/ndk_helper/gestureDetector.cpp
						${ANDROID_NDK}/sources/android/ndk_helper/vecmath.cpp
						${ANDROID_NDK}/sources/android/ndk_helper/GLContext.cpp
						${ANDROID_NDK}/sources/android/ndk_helper/gl3stub.c)
endif()

set(SAMPLE_ALL_FILES ${SAMPLE_SOURCE_FILES} ${SAMPLE_RESOURCE_FILES})

if(WIN32)
	add_executable(${ProjectName} WIN32 ${SAMPLE_ALL_FILES})
elseif(APPLE)
	add_executable(${ProjectName} MACOSX_BUNDLE ${SAMPLE_ALL_FILES})
else()
# Must be android
	ADD_LIBRARY(${ProjectName} SHARED ${SAMPLE_ALL_FILES})
endif()

#3. Set Solution Framework.
source_group_by_dir(SAMPLE_ALL_FILES)

target_link_libraries(${ProjectName} BeyondEngine)
setup_app_target(${ProjectName})
