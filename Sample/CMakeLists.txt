set(ProjectName Sample)

#1. Set Include Path.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/SourceCode)
include_directories(${CMAKE_SOURCE_DIR}/GameProject/StarRaiders/SourceCode)

#2. Set App Generation.
if(APPLE)
elseif()
file(GLOB_RECURSE SAMPLE_RESOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/resource/*.*)
endif()

set(SAMPLE_SOURCE_FILES main.cpp 
	StarRaidersApplication.cpp)

add_definitions(-DASIO_SEPARATE_COMPILATION)
    
if(WIN32)
set(SAMPLE_ALL_FILES ${SAMPLE_SOURCE_FILES})
elseif(APPLE)
file(GLOB XIB_FILES *.xib)
file(GLOB ICON_FILES Images.xcassets)
file(GLOB SUPERSDK_BUNDLE_FILE ${CMAKE_SOURCE_DIR}/Include/SuperSdk/ios/SuperSdk.bundle)
file(GLOB RESOURCE_INFO ${CMAKE_SOURCE_DIR}/resourceios/ResourceList.bin)
file(GLOB VERSION_INFO ${CMAKE_SOURCE_DIR}/resourceios/VersionInfo.bin)
set(SAMPLE_ALL_FILES ${SAMPLE_SOURCE_FILES} ${SAMPLE_RESOURCE_FILES} ${XIB_FILES} ${ICON_FILES} ${SUPERSDK_BUNDLE_FILE} ${RESOURCE_INFO} ${VERSION_INFO})
elseif(ANDROID)
set(SAMPLE_SOURCE_FILES ${SAMPLE_SOURCE_FILES} 
						${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c
						${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
						${ANDROID_NDK}/sources/android/ndk_helper/JNIHelper.cpp
						${ANDROID_NDK}/sources/android/ndk_helper/interpolator.cpp
						${ANDROID_NDK}/sources/android/ndk_helper/gestureDetector.cpp
						${ANDROID_NDK}/sources/android/ndk_helper/vecmath.cpp
						${ANDROID_NDK}/sources/android/ndk_helper/GLContext.cpp
						${ANDROID_NDK}/sources/android/ndk_helper/gl3stub.c)
set(SAMPLE_ALL_FILES ${SAMPLE_SOURCE_FILES} ${SAMPLE_RESOURCE_FILES})
elseif(LINUX)
set(SAMPLE_ALL_FILES ${SAMPLE_SOURCE_FILES})
endif()

if(APPLE)
set_source_files_properties(${XIB_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set_source_files_properties(${ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set_source_files_properties(${SUPERSDK_BUNDLE_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set_source_files_properties(${RESOURCE_INFO} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set_source_files_properties(${VERSION_INFO} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

if(WIN32)
	add_executable(${ProjectName} WIN32 ${SAMPLE_ALL_FILES})
elseif(APPLE)
	add_executable(${ProjectName} MACOSX_BUNDLE ${SAMPLE_ALL_FILES})
elseif(ANDROID)
# Must be android
	ADD_LIBRARY(${ProjectName} SHARED ${SAMPLE_ALL_FILES})
elseif(LINUX)
    ADD_EXECUTABLE(${ProjectName} ${SAMPLE_ALL_FILES})
endif()

#3. Set Solution Framework.
source_group_by_dir(SAMPLE_ALL_FILES)

if(APPLE)
add_dependencies(${ProjectName} BeyondEngine StarRaiders)
target_link_libraries(${ProjectName} libBeyondEngine.a libStarRaiders.a)
else()
target_link_libraries(${ProjectName} BeyondEngine)
target_link_libraries(${ProjectName} StarRaiders)
endif()

if(APPLE)
set_xcode_property(${ProjectName} IPHONEOS_DEPLOYMENT_TARGET "6.0")
endif()

setup_app_target(${ProjectName})
