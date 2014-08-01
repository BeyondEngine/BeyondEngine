LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := BeyondEngine
LOCAL_SRC_FILES := libBeyondEngine.a
LOCAL_CPPFLAGS := 
LOCAL_CFLAGS := 
LOCAL_LDFLAGS := 
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Component
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/Beats/prebuilt/android/libComponents_d.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Utility
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/Beats/prebuilt/android/libUtility_d.a
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
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/png/prebuilt/android/$(TARGET_ARCH_ABI)/libpng.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := TIFFLIB
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/tiff/prebuilt/android/$(TARGET_ARCH_ABI)/libtiff.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := WEBPLIB
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../Include/webp/prebuilt/android/$(TARGET_ARCH_ABI)/libwebp.a
include $(PREBUILT_STATIC_LIBRARY)
 
include $(CLEAR_VARS)

LOCAL_MODULE    := SampleNativeActivity
#VisualGDBAndroid: AutoUpdateSourcesInNextLine
LOCAL_SRC_FILES := ../../../main.cpp ../../../SampleApplication.cpp
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
					$(LOCAL_PATH)/../../../../SourceCode/Platform\
					$(LOCAL_PATH)/../../../../SourceCode/Platform/android\
					$(LOCAL_PATH)/../../../../Include/platform/android\
					$(LOCAL_PATH)/../../../../Include/glfw3/include/android\
					$(LOCAL_PATH)/../../../../Sample\
					$(LOCAL_PATH)/../../../../GameProject


LOCAL_CFLAGS := -D_DEBUG
LOCAL_CPPFLAGS := -D_DEBUG
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -lz

ifneq ($(filter %armeabi-v7a,$(TARGET_ARCH_ABI)),)
#LOCAL_CFLAGS += -mhard-float -D_NDK_MATH_NO_SOFTFP=1
#LOCAL_LDLIBS += -lm_hard
ifeq (,$(filter -fuse-ld=mcld,$(APP_LDFLAGS) $(LOCAL_LDFLAGS)))
LOCAL_LDFLAGS := -Wl,--no-warn-mismatch
endif
endif
LOCAL_STATIC_LIBRARIES := BeyondEngine StarRaiders Component Utility WEBPLIB TIFFLIB PNGLIB JPEGLIB FreeType cpufeatures android_native_app_glue ndk_helper

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/ndk_helper)
$(call import-module,android/native_app_glue)
$(call import-module,android/cpufeatures)
