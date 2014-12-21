#pragma once

#include "PlatformConfig.h"

// asio without boost
#define ASIO_STANDALONE

#include "ft2build.h"
#include FT_FREETYPE_H

#include "EnginePublic/BeyondEnginePublic.h"
#include "Render/RenderPublic.h"
#include "Resource/ResourcePublic.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "Component/ComponentPublic.h"
#include "Utility/PerformDetector/PerformDetector.h"
#if (BEYONDENGINE_PLATFORM == PLATFORM_LINUX)
#include "Utility/Logging.h"
#include "Utility/Conv.h"
#include "Utility/Strings.h"
#endif
