set(ProjectName BeyondEngine)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/GameProject/StarRaiders/SourceCode)

#collect all source files
file(GLOB_RECURSE BEYONDENGINE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE BEYONDENGINE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE BEYONDENGINE_HEADERPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE BEYONDENGINE_SOURCEMM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.mm)
file(GLOB_RECURSE BEYONDENGINE_SOURCEC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
set(BEYONDENGINE_ALL_FILES ${BEYONDENGINE_SOURCE_FILES} ${BEYONDENGINE_HEADER_FILES} ${BEYONDENGINE_HEADERPP_FILES} ${BEYONDENGINE_SOURCEMM_FILES} ${BEYONDENGINE_SOURCEC_FILES})

filter_platform_files(BEYONDENGINE_ALL_FILES ${PLATFORM_FOLDER})

add_library(${ProjectName} STATIC ${BEYONDENGINE_ALL_FILES})
source_group_by_dir(BEYONDENGINE_ALL_FILES)

if(APPLE)
	set_xcode_property(${ProjectName} OTHER_LDFLAGS "-framework Bugly -ObjC -framework SuperSdk -framework Ogg -framework Vorbis")
	set_xcode_property(${ProjectName} FRAMEWORK_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/Include/SuperSdk/ios ${CMAKE_SOURCE_DIR}/Include/OggDecoder/prebuilt/ios ${CMAKE_SOURCE_DIR}/Include/Bugly")
	set_xcode_property(${ProjectName} GCC_GENERATE_DEBUGGING_SYMBOLS YES)
	#set_xcode_property(${ProjectName} GCC_SYMBOLS_PRIVATE_EXTERN YES)
	#set_xcode_property(${ProjectName} GCC_OPTIMIZATION_LEVEL s)
elseif(WIN32)
	USE_MSVC_PCH(${ProjectName} stdafx.h stdafx.cpp)
    SET_SOURCE_FILES_PROPERTIES(${BEYONDENGINE_SOURCEC_FILES} PROPERTIES COMPILE_FLAGS "/Y-")
endif()

install(TARGETS ${ProjectName} DESTINATION ${BeyondEngine_DIR}/lib)
		
if(WIN32)
    install(FILES ${DEPENDENCIES_DIR}/platform/win32/gles/prebuilt/glew32.dll DESTINATION ${BeyondEngine_DIR}/bin)
	install(FILES ${DEPENDENCIES_DIR}/platform/win32/zlib/prebuilt/zlib1.dll DESTINATION ${BeyondEngine_DIR}/bin)
    
endif()

