﻿#ifndef BEATS_PLATFORM_BEATSPLATFORMCONFIG_INCLUDE
#define BEATS_PLATFORM_BEATSPLATFORMCONFIG_INCLUDE

#include "BeatsPlatform.h"

#ifdef BEATS_PLATFORM
    #undef BEATS_PLATFORM
#endif

#define BEATS_PLATFORM BEATS_PLATFORM_WIN32

#endif