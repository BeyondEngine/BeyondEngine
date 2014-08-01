macro(include_common_dirs )
	set(DEPENDENCIES_DIR ${BeyondEngine_DIR}/Include)

	include_directories(
		${DEPENDENCIES_DIR}/freetype2/include/${PLATFORM_FOLDER}
		${DEPENDENCIES_DIR}/jpeg/include/${PLATFORM_FOLDER}
		${DEPENDENCIES_DIR}/png/include/${PLATFORM_FOLDER}
		${DEPENDENCIES_DIR}/tiff/include/${PLATFORM_FOLDER}
		${DEPENDENCIES_DIR}/webp/include/${PLATFORM_FOLDER}
		${BeyondEngine_DIR}/GameProject
		${BeyondEngine_DIR}/SourceCode
		${BeyondEngine_DIR}/SourceCode/Utility
		${BeyondEngine_DIR}/SourceCode/Utility/BeatsUtility/Platform
		${BeyondEngine_DIR}/SourceCode/Utility/BeatsUtility/Platform/${PLATFORM_FOLDER}
		${BeyondEngine_DIR}/SourceCode/Platform
		${BeyondEngine_DIR}/SourceCode/Platform/${PLATFORM_FOLDER}
	)
	if(WIN32)
		include_directories(
			${DEPENDENCIES_DIR}/platform/win32/gles/include/OGLES
			${DEPENDENCIES_DIR}/platform/win32
			${DEPENDENCIES_DIR}/glfw3/include/${PLATFORM_FOLDER}
		)
	elseif(APPLE)
	else()
	include_directories(${ANDROID_NDK}/platforms/android-${ANDROID_API_LEVEL}/arch-arm/usr/include)
	include_directories(${ANDROID_NDK}/sources/android)
	include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
	include_directories(${ANDROID_NDK}/sources/android/ndk_helper)
	endif()
endmacro()

macro(setup_app_target Target)
	#library links
	target_link_libraries(${Target}
		debug ${DEPENDENCIES_DIR}/Beats/prebuilt/${PLATFORM_FOLDER}/${LIB_PREFIX}Components_d${LIB_SUFFIX}
		debug ${DEPENDENCIES_DIR}/Beats/prebuilt/${PLATFORM_FOLDER}/${LIB_PREFIX}Utility_d${LIB_SUFFIX}
		optimized ${DEPENDENCIES_DIR}/Beats/prebuilt/${PLATFORM_FOLDER}/${LIB_PREFIX}Components_r${LIB_SUFFIX}
		optimized ${DEPENDENCIES_DIR}/Beats/prebuilt/${PLATFORM_FOLDER}/${LIB_PREFIX}Utility_r${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/freetype2/prebuilt/${PLATFORM_FOLDER}/${ANDROID_ABI}/${LIB_PREFIX}freetype${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/jpeg/prebuilt/${PLATFORM_FOLDER}/${ANDROID_ABI}/libjpeg${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/png/prebuilt/${PLATFORM_FOLDER}/${ANDROID_ABI}/libpng${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/tiff/prebuilt/${PLATFORM_FOLDER}/${ANDROID_ABI}/libtiff${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/webp/prebuilt/${PLATFORM_FOLDER}/${ANDROID_ABI}/libwebp${LIB_SUFFIX}
		)

	if(WIN32)
		target_link_libraries(${Target} 
			${DEPENDENCIES_DIR}/platform/${PLATFORM_FOLDER}/gles/prebuilt/glew32.lib
			${DEPENDENCIES_DIR}/platform/${PLATFORM_FOLDER}/zlib/prebuilt/libzlib.lib
			${DEPENDENCIES_DIR}/glfw3/prebuilt/${PLATFORM_FOLDER}/glfw3.lib
			opengl32.lib dbghelp.lib
		)
	elseif(APPLE)
			set_target_properties(${Target} PROPERTIES LINK_FLAGS "-framework Foundation -framework OpenGLES -framework UIKit -framework GLKit -lz")
	else()
			target_link_libraries(${Target} GLESv2 log android z EGL)
			set_target_properties(${Target} PROPERTIES LINK_FLAGS "-Wl,--no-warn-mismatch")
	endif()

	#settings
	if(APPLE)
		set_target_properties(${Target} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/info.plist)
		set_xcode_property(${Target} GCC_GENERATE_DEBUGGING_SYMBOLS YES)
	endif()

	#transfer file
	if(APPLE)
		add_custom_command(TARGET ${Target} POST_BUILD
			COMMAND cp ARGS -R ${BeyondEngine_DIR}/Resource $(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)/${Target}.app
		)
	elseif(WIN32)
		set_target_properties(${Target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BeyondEngine_DIR}/bin)
		set_target_properties(${Target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BeyondEngine_DIR}/bin)
		set_target_properties(${Target} PROPERTIES RUNTIME_OUTPUT_NAME_DEBUG ${Target}d)
		set_target_properties(${Target} PROPERTIES RUNTIME_OUTPUT_NAME_RELEASE ${Target})
	endif()
endmacro()

macro(filter_platform_files FILE_LIST CURR_PLATFORM)
	set(PLATFORM_NAMES win32 ios android linux)
	foreach(FILE ${${FILE_LIST}})
			foreach(PLATFORM_NAME ${PLATFORM_NAMES})
				if(NOT ${CURR_PLATFORM} STREQUAL ${PLATFORM_NAME})
					string(REGEX MATCH /${PLATFORM_NAME}/ MATCH_RESULT1 ${FILE})
					if(MATCH_RESULT1)
						list(REMOVE_ITEM ${FILE_LIST} ${FILE})
					endif()
				endif()
			endforeach()
	endforeach()
endmacro()
