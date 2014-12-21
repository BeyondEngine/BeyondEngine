#ifndef PLATFORMCONFIG_INCLUDE
#define PLATFORMCONFIG_INCLUDE

#include "PlatformDefine.h"

#ifdef BEYONDENGINE_PLATFORM
#undef BEYONDENGINE_PLATFORM
#endif
#define BEYONDENGINE_PLATFORM PLATFORM_ANDROID

#define BEYONDENGINE_UNUSED_PARAM(unusedparam) (void)unusedparam

#define TOUCH_MAX_NUM 5

#include <NDKHelper.h>
#include <unistd.h>
#include <GLES2/gl2ext.h>

// gl2.h doesn't define GLchar on Android
typedef char GLchar;
// android defines GL_BGRA_EXT but not GL_BRGA
#ifndef GL_BGRA
#define GL_BGRA  0x80E1
#endif
#define glClearDepth glClearDepthf

#define BEYONDENGINE_SLEEP(time) usleep(time * 1000);

#endif