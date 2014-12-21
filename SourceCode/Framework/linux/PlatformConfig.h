#ifndef BEYOND_ENGINE_PLATFORM_LINUX_PLATFORMCONFIG_H__INCLUDE
#define BEYOND_ENGINE_PLATFORM_LINUX_PLATFORMCONFIG_H__INCLUDE

#include "PlatformDefine.h"
#include "unistd.h"
#include <inttypes.h>
#include <stddef.h>
#include <string.h>
#include "glew.h"
#ifdef BEYONDENGINE_PLATFORM
#undef BEYONDENGINE_PLATFORM
#endif

#define BEYONDENGINE_PLATFORM PLATFORM_LINUX

#define TOUCH_MAX_NUM 1

#define BEYONDENGINE_SLEEP(timeMS) usleep(timeMS * 1000)

#endif
