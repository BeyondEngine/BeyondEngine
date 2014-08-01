#ifndef BEYOND_ENGINE_PLATFORM_WIN32_PLATFORMCONFIG_H__INCLUDE
#define BEYOND_ENGINE_PLATFORM_WIN32_PLATFORMCONFIG_H__INCLUDE

#include "PlatformDefine.h"


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlwapi.h>


#include "GL/glew.h"

#include "glfw3.h"

#ifdef BEYONDENGINE_PLATFORM
#undef BEYONDENGINE_PLATFORM
#endif

#define BEYONDENGINE_PLATFORM PLATFORM_WIN32

#define BEYONDENGINE_UNUSED_PARAM(unusedparam) (void)unusedparam

#define ssize_t int

#define TOUCH_MAX_NUM 1

#define BEYONDENGINE_SLEEP(time) Sleep(time)

#endif