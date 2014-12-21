LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := BeyondEngine
LOCAL_SRC_FILES := libBeyondEngine.a
LOCAL_CPPFLAGS := 
LOCAL_CFLAGS := 
LOCAL_LDFLAGS := 
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := StarRaiders
LOCAL_SRC_FILES := libStarRaiders.a
LOCAL_CPPFLAGS := 
LOCAL_CFLAGS := 
LOCAL_LDFLAGS := 
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Lua
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/lua/prebuilt/android/liblua.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := FreeType
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/freetype2/prebuilt/android/$(TARGET_ARCH_ABI)/libfreetype.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := JPEGLIB
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/jpeg/prebuilt/android/$(TARGET_ARCH_ABI)/libjpeg.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := PNGLIB
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/png/prebuilt/android/libpng.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := TIFFLIB
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/tiff/prebuilt/android/$(TARGET_ARCH_ABI)/libtiff.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := CRYPTO
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/openssl/prebuilt/android/libcrypto.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := WEBPLIB
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/webp/prebuilt/android/$(TARGET_ARCH_ABI)/libwebp.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := PROTOBUFLIB
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/protobuf/prebuilt/android/libprotobuf_r.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := CURLLIB
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/curl/prebuilt/android/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE    := StarRaidersActivity
#VisualGDBAndroid: AutoUpdateSourcesInNextLine
LOCAL_SRC_FILES := ../../../main.cpp ../../../StarRaidersApplication.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../SourceCode\
					$(LOCAL_PATH)/../../../../Include/freetype2/include/android\
					$(LOCAL_PATH)/../../../../Include/jpeg/include/android\
					$(LOCAL_PATH)/../../../../Include/png/include/android\
					$(LOCAL_PATH)/../../../../Include/tiff/include/android\
					$(LOCAL_PATH)/../../../../Include/webp/include/android\
					$(LOCAL_PATH)/../../../../SourceCode\
					$(LOCAL_PATH)/../../../../SourceCode/Utility\
					$(LOCAL_PATH)/../../../../SourceCode/Utility/BeatsUtility/Platform\
					$(LOCAL_PATH)/../../../../SourceCode/Utility/BeatsUtility/Platform/android\
					$(LOCAL_PATH)/../../../../SourceCode/Framework\
					$(LOCAL_PATH)/../../../../SourceCode/Framework/android\
					$(LOCAL_PATH)/../../../../Include/platform/android\
					$(LOCAL_PATH)/../../../../Include/glfw3/include/android\
					$(LOCAL_PATH)/../../../../Sample\
					$(LOCAL_PATH)/../../../../GameProject/StarRaiders/SourceCode\
					$(LOCAL_PATH)/../../../../Include/msgpack\
					$(LOCAL_PATH)/../../../../Include/rapidjson\
                    $(LOCAL_PATH)/../../../../Include/glm\
					$(LOCAL_PATH)/../../../../Include/protobuf/include\
LOCAL_CFLAGS := -D_DEBUG
LOCAL_CPPFLAGS := -D_DEBUG
LOCAL_LDLIBS    := -lOpenSLES -llog -landroid -lEGL -lGLESv2 -lz

ifneq ($(filter %armeabi-v7a,$(TARGET_ARCH_ABI)),)
#LOCAL_CFLAGS += -mhard-float -D_NDK_MATH_NO_SOFTFP=1
#LOCAL_LDLIBS += -lm_hard
ifeq (,$(filter -fuse-ld=mcld,$(APP_LDFLAGS) $(LOCAL_LDFLAGS)))
LOCAL_LDFLAGS := -Wl,--no-warn-mismatch
endif
endif
LOCAL_STATIC_LIBRARIES := BeyondEngine StarRaiders CRYPTO WEBPLIB TIFFLIB PNGLIB JPEGLIB FreeType cpufeatures android_native_app_glue ndk_helper Lua PROTOBUFLIB CURLLIB

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/ndk_helper)
$(call import-module,android/native_app_glue)
$(call import-module,android/cpufeatures)
