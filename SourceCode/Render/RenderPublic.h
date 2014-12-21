#ifndef BEYOND_ENGINE_RENDER_RENDERPUBLIC_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERPUBLIC_H__INCLUDE

#include "Utility/BeatsUtility/MathPublic.h"
#include "CommonTypes.h"
#include "Framework/Application.h"
#include "external/etc1/etc1.h"

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>    //added here to avoid:  warning C4005: 'DEVICE_TYPE' : macro redefinition
#endif //WIN32

#if !defined(_DEBUG)
#define BEYONDENGINE_CHECK_GL_ERROR_DEBUG()
#else
static const char* pszGLErrorDesc[] =
{
    "GL_INVALID_ENUM",
    "GL_INVALID_VALUE",
    "GL_INVALID_OPERATION",
    "GL_STACK_OVERFLOW",
    "GL_STACK_UNDERFLOW",
    "GL_OUT_OF_MEMORY",
    "GL_INVALID_FRAMEBUFFER_OPERATION",
    "GL_CONTEXT_LOST",
};
#define BEYONDENGINE_CHECK_GL_ERROR_DEBUG() \
do {\
    BEATS_ASSERT(!CApplication::GetInstance() || CApplication::GetInstance()->IsDestructing() || std::this_thread::get_id() == CEngineCenter::GetInstance()->GetMainThreadId(), _T("Must Call Opengl Function In Main Thread!")); \
    GLenum __error = glGetError(); \
    const char* pszReason = (__error >= 0x0500 && __error <= 0x0507) ? pszGLErrorDesc[__error - 0x0500] : "Unknown error";\
    BEATS_ASSERT(!__error, _T("OpenGL error 0x%04X %s in %s %s %d\n"), __error, pszReason, _T(__FILE__), _T(__FUNCTION__), __LINE__); \
    } while (false)
#endif

enum ECommonUniformType
{
    UNIFORM_P_MATRIX,
    UNIFORM_M_MATRIX,
    UNIFORM_V_MATRIX,
    UNIFORM_MVP_MATRIX,
    UNIFORM_TEX0,
    UNIFORM_TEX1,
    UNIFORM_TEX2,
    UNIFORM_TEX3,
    UNIFORM_TEX4,
    UNIFORM_TEX5,
    UNIFORM_TEX6,
    UNIFORM_TEX7,
    UNIFORM_ISETC,
    UNIFORM_ETC_HAS_ALPHA,
    UNIFORM_BONE_MATRICES,
    UNIFORM_COLOR_SCALE,
    UNIFORM_COUNT,
};

static const char *COMMON_UNIFORM_NAMES[] =
{
    "u_PMatrix",
    "u_MMatrix",
    "u_VMatrix",
    "u_MVPMatrix",
    "u_tex0",
    "u_tex1",
    "u_tex2",
    "u_tex3",
    "u_tex4",
    "u_tex5",
    "u_tex6",
    "u_tex7",
    "u_isEtc",
    "u_etcHasAlpha",
    "u_BoneMatrices",
    "u_ColorScale",
};

enum ECommonAttribIndex
{
    ATTRIB_INDEX_POSITION,
    ATTRIB_INDEX_COLOR,
    ATTRIB_INDEX_COLOR2,
    ATTRIB_INDEX_NORMAL,
    ATTRIB_INDEX_TEXCOORD0,
    ATTRIB_INDEX_TEXCOORD1,
    ATTRIB_INDEX_BONE_INDICES,
    ATTRIB_INDEX_WEIGHTS,

    ATTRIB_INDEX_COUNT,
};

static const char *COMMON_ATTIB_NAMES[] =
{
    "a_position",
    "a_color",
    "a_color2",
    "a_normal",
    "a_texCoord0",
    "a_texCoord1",
    "a_boneIndices",
    "a_weights",
};

enum class PixelFormat
{
    //! auto detect the type
    AUTO,
    //! 32-bit texture: BGRA8888
    BGRA8888,
    //! 32-bit texture: RGBA8888
    RGBA8888,
    //! 24-bit texture: RGBA888
    RGB888,
    //! 16-bit texture without Alpha channel
    RGB565,
    //! 8-bit textures used as masks
    A8,
    //! 8-bit intensity texture
    I8,
    //! 16-bit textures used as masks
    AI88,
    //! 16-bit textures: RGBA4444
    RGBA4444,
    //! 16-bit textures: RGB5A1
    RGB5A1,
    //! 4-bit PVRTC-compressed texture: PVRTC4
    PVRTC4,
    //! 4-bit PVRTC-compressed texture: PVRTC4 (has alpha channel)
    PVRTC4A,
    //! 2-bit PVRTC-compressed texture: PVRTC2
    PVRTC2,
    //! 2-bit PVRTC-compressed texture: PVRTC2 (has alpha channel)
    PVRTC2A,
    //! ETC-compressed texture: ETC
    ETC,
    //! S3TC-compressed texture: S3TC_Dxt1
    S3TC_DXT1,
    //! S3TC-compressed texture: S3TC_Dxt3
    S3TC_DXT3,
    //! S3TC-compressed texture: S3TC_Dxt5
    S3TC_DXT5,
    //! ATITC-compressed texture: ATC_RGB
    ATC_RGB,
    //! ATITC-compressed texture: ATC_EXPLICIT_ALPHA
    ATC_EXPLICIT_ALPHA,
    //! ATITC-compresed texture: ATC_INTERPOLATED_ALPHA
    ATC_INTERPOLATED_ALPHA,
    //! Default texture format: AUTO
    DEFAULT = AUTO,

    NONE = -1
};

struct PixelFormatInfo {

    PixelFormatInfo(GLenum anInternalFormat, GLenum aFormat, GLenum aType, int aBpp, bool aCompressed, bool anAlpha)
        : internalFormat(anInternalFormat)
        , format(aFormat)
        , type(aType)
        , bpp(aBpp)
        , compressed(aCompressed)
        , alpha(anAlpha)
    {}

    GLenum internalFormat;
    GLenum format;
    GLenum type;
    int bpp;
    bool compressed;
    bool alpha;
};

typedef std::map<PixelFormat, const PixelFormatInfo> PixelFormatInfoMap;

enum ERenderGroupID
{
    LAYER_UNSET = 0,

    LAYER_BACKGROUND,

    LAYER_3D_MIN,
    LAYER_UNIVERSE_BUTTOM,
    LAYER_UNIVERSE_LUNKUO,
    LAYER_UNIVERSE_HALO,
    LAYER_UNIVERSE_STAR,
    LAYER_3D,
    LAYER_Sprite_Ground,
    LAYER_Sprite_Shadow,
    LAYER_Particle_Ground,
    LAYER_Sprite,
    LAYER_Sprite_Above,
    LAYER_Particle,
    LAYER_3D_AFTER_SCENE,
    LAYER_UniverseCover,
    LAYER_3D_BridgeAnimation,
    LAYER_3D_DEBUG_PRIMITIVE,
    LAYER_3D_MAX,

    LAYER_2D_MIN,
    LAYER_2D,
    LAYER_GUI_MIN = 10000,  // UI RENDER GROUP START
    LAYER_GUI_POP_MIN = 11000,
    LAYER_GUI_POP_MAX = 12000,
    LAYER_GUI = 13000,
    LAYER_GUI_AUTO_INCREASE_MAX = 47000,
    LAYER_GUI_TIP_USED_MAX = 48000,
    LAYER_GUI_SYSTEM_USED_MAX = 49000,
    LAYER_GUI_MAX = 50000,  // UI RENDER GROUP END
    LAYER_GUI_EDITOR,
    LAYER_2D_MAX,

    LAYER_USER,
};
#endif

