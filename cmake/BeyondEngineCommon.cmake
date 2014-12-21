macro(include_common_dirs )
	set(DEPENDENCIES_DIR ${BeyondEngine_DIR}/Include)
    	set(DEPENDENCIES_DIR_LINUX /usr/include)

	include_directories(
		${DEPENDENCIES_DIR}/freetype2/include/${PLATFORM_FOLDER}
		${DEPENDENCIES_DIR}/jpeg/include/${PLATFORM_FOLDER}
		${DEPENDENCIES_DIR}/png/include
		${DEPENDENCIES_DIR}/tiff/include/${PLATFORM_FOLDER}
		${DEPENDENCIES_DIR}/webp/include/${PLATFORM_FOLDER}
		${BeyondEngine_DIR}/SourceCode/Utility
		${BeyondEngine_DIR}/SourceCode/Utility/BeatsUtility/Platform
		${BeyondEngine_DIR}/SourceCode/Utility/BeatsUtility/Platform/${PLATFORM_FOLDER}
		${BeyondEngine_DIR}/SourceCode/Framework
		${BeyondEngine_DIR}/SourceCode/Framework/${PLATFORM_FOLDER}
		${DEPENDENCIES_DIR}/asio
		${DEPENDENCIES_DIR}/msgpack
        ${DEPENDENCIES_DIR}/openssl/include
		${DEPENDENCIES_DIR}/MP3Decoder/include
		${DEPENDENCIES_DIR}/OggDecoder/include
		${DEPENDENCIES_DIR}/OpenalSoft/include
		${DEPENDENCIES_DIR}/lua/Include
        ${DEPENDENCIES_DIR}/tolua
        ${DEPENDENCIES_DIR}/glm
        ${DEPENDENCIES_DIR}/protobuf/include
		${DEPENDENCIES_DIR}/curl/include/${PLATFORM_FOLDER}
		${DEPENDENCIES_DIR}/XGPushSdk/
		${DEPENDENCIES_DIR}/crow/include
		${DEPENDENCIES_DIR}/glog/include
	)

	if(WIN32)
		include_directories(
			${DEPENDENCIES_DIR}/platform/win32/gles/include/OGLES
			${DEPENDENCIES_DIR}/platform/win32
            ${DEPENDENCIES_DIR}/platform/win32/zlib/include
			${DEPENDENCIES_DIR}/glfw3/include/${PLATFORM_FOLDER}
		)
	elseif(APPLE)
	elseif(ANDROID)
	include_directories(${ANDROID_NDK}/platforms/android-${ANDROID_API_LEVEL}/arch-arm/usr/include)
	include_directories(${ANDROID_NDK}/sources/android)
	include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
	include_directories(${ANDROID_NDK}/sources/android/ndk_helper)
	endif()
endmacro()

macro(setup_app_target Target)
    if(LINUX)
    # `-l` let ld link shared objects first, but we prefer static lib
	 target_link_libraries(${Target}
	 		z
	 		dl
	 		rt
	 		sqlite3
	 		pthread
	 		${DEPENDENCIES_DIR}/glog/prebuilt/libglog.a
	 		${DEPENDENCIES_DIR}/boost/prebuilt/libboost_regex.a
	 		${DEPENDENCIES_DIR}/boost/prebuilt/libboost_system.a
	 		${DEPENDENCIES_DIR}/boost/prebuilt/libboost_program_options.a
	 		${DEPENDENCIES_DIR}/openssl/prebuilt/linux/libssl.a
	 		${DEPENDENCIES_DIR}/openssl/prebuilt/linux/libcrypto.a
         	${DEPENDENCIES_DIR}/protobuf/prebuilt/linux/libprotobuf.a
	)
    else()
    target_link_libraries(${Target}
        debug ${DEPENDENCIES_DIR}/protobuf/prebuilt/${PLATFORM_FOLDER}/${LIB_PREFIX}protobuf_d${LIB_SUFFIX}
		optimized ${DEPENDENCIES_DIR}/protobuf/prebuilt/${PLATFORM_FOLDER}/${LIB_PREFIX}protobuf_r${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/freetype2/prebuilt/${PLATFORM_FOLDER}/${ANDROID_ABI}/${LIB_PREFIX}freetype${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/jpeg/prebuilt/${PLATFORM_FOLDER}/${ANDROID_ABI}/libjpeg${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/png/prebuilt/${PLATFORM_FOLDER}/libpng${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/tiff/prebuilt/${PLATFORM_FOLDER}/${ANDROID_ABI}/libtiff${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/webp/prebuilt/${PLATFORM_FOLDER}/${ANDROID_ABI}/libwebp${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/lua/prebuilt/${PLATFORM_FOLDER}/liblua${LIB_SUFFIX}
        ${DEPENDENCIES_DIR}/curl/prebuilt/${PLATFORM_FOLDER}/libcrypto${LIB_SUFFIX}
        ${DEPENDENCIES_DIR}/curl/prebuilt/${PLATFORM_FOLDER}/libcurl${LIB_SUFFIX}
        ${DEPENDENCIES_DIR}/curl/prebuilt/${PLATFORM_FOLDER}/libssl${LIB_SUFFIX}
        ${DEPENDENCIES_DIR}/openssl/prebuilt/${PLATFORM_FOLDER}/libcrypto${LIB_SUFFIX}
		)
    endif()

	if(WIN32)
		target_link_libraries(${Target}
			${DEPENDENCIES_DIR}/MP3Decoder/prebuilt/libmpg123.lib
			${DEPENDENCIES_DIR}/OggDecoder/prebuilt/win32/libogg.lib
			${DEPENDENCIES_DIR}/OggDecoder/prebuilt/win32/libvorbis.lib
			${DEPENDENCIES_DIR}/OggDecoder/prebuilt/win32/libvorbisfile.lib
			${DEPENDENCIES_DIR}/OpenalSoft/prebuilt/OpenAL32.lib
			${DEPENDENCIES_DIR}/platform/${PLATFORM_FOLDER}/gles/prebuilt/glew32.lib
			${DEPENDENCIES_DIR}/platform/${PLATFORM_FOLDER}/zlib/prebuilt/libzlib.lib
			${DEPENDENCIES_DIR}/glfw3/prebuilt/${PLATFORM_FOLDER}/glfw3.lib
			opengl32.lib dbghelp.lib ws2_32.lib iphlpapi.lib)

	elseif(APPLE)
		target_link_libraries(${Target}
		${DEPENDENCIES_DIR}/SuperSdk/ios/SuperSdkStatImp${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/SuperSdk/ios/SuperSdkImp${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/OggDecoder/prebuilt/ios/libogg${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/OggDecoder/prebuilt/ios/libvorbis${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/OggDecoder/prebuilt/ios/libvorbisenc${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/OggDecoder/prebuilt/ios/libvorbisfile${LIB_SUFFIX}
		${DEPENDENCIES_DIR}/XGPushSdk/libXG-SDK${LIB_SUFFIX}
			)
			set_target_properties(${Target} PROPERTIES LINK_FLAGS "-lsqlite3 -lstdc++.6 -framework Bugly -framework SuperSdk -framework Security -ObjC -framework CoreTelephony -framework CoreGraphics -framework OpenAL -framework SystemConfiguration -framework CFNetwork -framework AudioToolbox -framework AVFoundation -framework Foundation -framework OpenGLES -framework UIKit -framework GLKit -lz -framework CoreMotion")
	elseif(ANDROID)
			target_link_libraries(${Target} OpenSLES GLESv2 log android z EGL)
			set_target_properties(${Target} PROPERTIES LINK_FLAGS "-Wl,--no-warn-mismatch")
	endif()

	#settings
	if(APPLE)
		set_target_properties(${Target} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/info.plist)
		set_xcode_property(${Target} FRAMEWORK_SEARCH_PATHS "${BeyondEngine_DIR}/Include/SuperSdk/ios ${BeyondEngine_DIR}/Include/OggDecoder/prebuilt/ios ${BeyondEngine_DIR}/Include/Bugly")
		set_xcode_property(${Target} LIBRARY_SEARCH_PATHS "$(CONFIGURATION_BUILD_DIR) $(PROJECT_DIR)/SourceCode/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME) $(PROJECT_DIR)/GameProject/StarRaiders/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)")
		set_xcode_property(${Target} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
		set_xcode_property(${Target} ASSETCATALOG_COMPILER_LAUNCHIMAGE_NAME "LaunchImage")
		set_xcode_property(${Target} GCC_GENERATE_DEBUGGING_SYMBOLS YES)
		#set_xcode_property(${Target} GCC_SYMBOLS_PRIVATE_EXTERN YES)
		#set_xcode_property(${Target} GCC_OPTIMIZATION_LEVEL s)
	endif()

	#transfer file
	if(APPLE)
		add_custom_command(TARGET ${Target} POST_BUILD
			COMMAND cp ARGS -R ${BeyondEngine_DIR}/resourceios/resource $(CONFIGURATION_BUILD_DIR)/${Target}.app
		)
	elseif(WIN32)
		set_target_properties(${Target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BeyondEngine_DIR}/bin)
		set_target_properties(${Target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BeyondEngine_DIR}/bin)
		set_target_properties(${Target} PROPERTIES RUNTIME_OUTPUT_NAME_DEBUG ${Target}d)
		set_target_properties(${Target} PROPERTIES RUNTIME_OUTPUT_NAME_RELEASE ${Target})
    elseif(LINUX)
        set(EXECUTABLE_OUTPUT_PATH ${BeyondEngine_DIR}/bin)
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





