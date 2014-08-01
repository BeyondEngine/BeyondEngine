#ifndef BEYOND_ENGINE_RENDER_RENDERPUBLIC_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERPUBLIC_H__INCLUDE

#include <set>
#include <map>
#include <vector>

#include "Utility/BeatsUtility/mathpublic.h"
#include "CommonTypes.h"


#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>    //added here to avoid:  warning C4005: 'DEVICE_TYPE' : macro redefinition
#endif //WIN32

#ifndef _DEBUG
#define BEYONDENGINE_CHECK_GL_ERROR_DEBUG()
#else
#define BEYONDENGINE_CHECK_GL_ERROR_DEBUG() \
    do { \
    BEATS_ASSERT(std::this_thread::get_id() == CEngineCenter::GetInstance()->GetMainThreadId(), _T("Must Call Opengl Function In Main Thread!"));\
    GLenum __error = glGetError(); \
    BEATS_ASSERT(!__error, _T("OpenGL error 0x%04X in %s %s %d\n"), __error, _T(__FILE__), _T(__FUNCTION__), __LINE__);\
    } while (false)
#endif

enum ECommonUniformType
{
    UNIFORM_P_MATRIX,
    UNIFORM_MV_MATRIX,
    UNIFORM_MVP_MATRIX,
    UNIFORM_TIME,
    UNIFORM_SIN_TIME,
    UNIFORM_COS_TIME,
    UNIFORM_RANDOM01,
    UNIFORM_SAMPLER,
    UNIFORM_TEX0,
    UNIFORM_TEX1,
    UNIFORM_BONE_MATRICES,
    UNIFORM_AMBIENT_COLOR,
    UNIFORM_DIFFUSE_COLOR,
    UNIFORM_SPECULAR_COLOR,
    UNIFORM_SHININESS,

    UNIFORM_COUNT,
};

static const char *COMMON_UNIFORM_NAMES[] =
{
    "u_PMatrix",
    "u_MVMatrix",
    "u_MVPMatrix",
    "u_Time",
    "u_SinTime",
    "u_CosTime",
    "u_Random01",
    "u_Sampler",
    "u_tex0",
    "u_tex1",
    "u_BoneMatrices",
    "u_AmbientColor",
    "u_DiffuseColor",
    "u_SpecularColor",
    "u_Shininess"
};

#ifdef USE_UBO
enum ECommonUniformBlockType
{
    UNIFORM_BLOCK_MVP_MATRIX,

    UNIFORM_BLOCK_COUNT,
};

static const char *COMMON_UNIFORM_BLOCK_NAMES[] =
{
    "ub_MVPMatrix",
};
#endif

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

#endif

