#include "stdafx.h"
#include "Configuration.h"
#if(BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
#define sprintf_s sprintf
#include "Framework/android/AndroidHandler.h"
#elif(BEYONDENGINE_PLATFORM == PLATFORM_IOS)
#define sprintf_s sprintf
#endif


CConfiguration* CConfiguration::m_pInstance = NULL;

CConfiguration::CConfiguration()
{
    m_maxTextureSize = 0;
    m_supportsPVRTC = false;
    m_supportsETC1 = false;
    m_supportsS3TC = false;
    m_supportsATITC = false;
    m_supportsNPOT = false;
    m_supportsBGRA8888 = false;
    m_supportsDiscardFramebuffer = false;
    m_supportsShareableVAO = false;
    m_maxSamplesAllowed = 0;
    m_maxTextureUnits = 0;
    m_maxVertexAttributes = 0;
    m_maxVertexUniformVectors = 0;
    m_maxFragmentUniformVectors = 0;
    m_maxVaryingVectors = 0;
    m_strGL_Extensions = NULL;
    m_eDeviceType = eDT_UNKNOWN;
}

CConfiguration::~CConfiguration()
{
}

int CConfiguration::GetMaxTextureSize() const
{
    return m_maxTextureSize;
}

int CConfiguration::GetMaxTextureUnits() const
{
    return m_maxTextureUnits;
}

int CConfiguration::GetMaxVertexUniformVectors() const
{
    return m_maxVertexUniformVectors;
}

int CConfiguration::GetMaxFragmentUniformVectors() const
{
    return m_maxFragmentUniformVectors;
}

int CConfiguration::GetMaxVertexAttributes() const
{
    return m_maxVertexAttributes;
}

bool CConfiguration::SupportsNPOT() const
{
    //TODO: Try to detect!
    return m_supportsNPOT;
}

bool CConfiguration::SupportsPVRTC() const
{
    return m_supportsPVRTC;
}

bool CConfiguration::SupportsETC() const
{
    return m_supportsETC1;
}

bool CConfiguration::SupportsS3TC() const
{
    return m_supportsS3TC;
}

bool CConfiguration::SupportsATITC() const
{
    return m_supportsATITC;
}

bool CConfiguration::SupportsBGRA8888() const
{
    return m_supportsBGRA8888;
}

bool CConfiguration::SupportsDiscardFramebuffer() const
{
    return m_supportsDiscardFramebuffer;
}

bool CConfiguration::SupportsShareableVAO() const
{
// Some android device doesn't support VAO well, on those device, VAO is valid only for one frame
// If we want keep use VAO, we have to reset VAO every frame, that's not good. so we disable it in android device.
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    return false;
#else
    return m_supportsShareableVAO;
#endif
}

void CConfiguration::SetSupportShareableVAO(bool bEnable)
{
    m_supportsShareableVAO = bEnable;
}

bool CConfiguration::CheckForGLExtension( const TString& searchName ) const
{
    return (m_strGL_Extensions && strstr(m_strGL_Extensions, searchName.c_str()));
}

const TString& CConfiguration::GetOsInfo()
{
    return m_osInfo;
}

const TString& CConfiguration::GetModelInfo()
{
    return m_modelInfo;
}

const TString CConfiguration::GetOsType()
{
	TString strRet;
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    strRet = "android";
#elif (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
    strRet = "ios";
#elif (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    strRet = "win32";
#endif
    return strRet;
}

#define BOOL_TO_STRING(boolValue) ((boolValue) ? "true" : "false")
void CConfiguration::GatherGPUInfo()
{
    BEATS_PRINT("gl_vendor: %s\n", (const char*)glGetString(GL_VENDOR));
    BEATS_PRINT("gl_renderer: %s\n", (const char*)glGetString(GL_RENDERER));
    BEATS_PRINT("gl_version: %s\n", (const char*)glGetString(GL_VERSION));
    m_strGL_Extensions = (const char*)glGetString(GL_EXTENSIONS);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_maxTextureUnits);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &m_maxVertexAttributes);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &m_maxVertexUniformVectors);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &m_maxFragmentUniformVectors);
    glGetIntegerv(GL_MAX_VARYING_VECTORS, &m_maxVaryingVectors);

#if (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
    glGetIntegerv(GL_MAX_SAMPLES_APPLE, &m_maxSamplesAllowed);
#endif

    m_supportsETC1 = CheckForGLExtension(_T("GL_OES_compressed_ETC1_RGB8_texture"));
    m_supportsS3TC = CheckForGLExtension(_T("GL_EXT_texture_compression_s3tc"));
    m_supportsATITC = CheckForGLExtension(_T("GL_AMD_compressed_ATC_texture"));
    m_supportsPVRTC = CheckForGLExtension(_T("GL_IMG_texture_compression_pvrtc"));
    m_supportsBGRA8888 = CheckForGLExtension(_T("GL_IMG_texture_format_BGRA888"));
    m_supportsDiscardFramebuffer = CheckForGLExtension(_T("GL_EXT_discard_framebuffer"));
    m_supportsShareableVAO = CheckForGLExtension(_T("vertex_array_object"));
    
    BEATS_PRINT("Max texture size: %d\n", m_maxTextureSize);
    BEATS_PRINT("Max texture units: %d\n", m_maxTextureUnits);
    BEATS_PRINT("Max vertex attributes: %d\n", m_maxVertexAttributes);
    BEATS_PRINT("Max vertex uniform vectors: %d\n", m_maxVertexUniformVectors);
    BEATS_PRINT("Max fragment uniform vectors: %d\n", m_maxFragmentUniformVectors);
    BEATS_PRINT("Max varying vectors: %d\n", m_maxVaryingVectors);

    BEATS_PRINT("Support ETC1: %s\n", BOOL_TO_STRING(m_supportsETC1));
    BEATS_PRINT("Support S3TC: %s\n", BOOL_TO_STRING(m_supportsS3TC));
    BEATS_PRINT("Support ATITC: %s\n", BOOL_TO_STRING(m_supportsATITC));
    BEATS_PRINT("Support PVRTC: %s\n", BOOL_TO_STRING(m_supportsPVRTC));
    BEATS_PRINT("Support BGRA8888: %s\n", BOOL_TO_STRING(m_supportsBGRA8888));
    BEATS_PRINT("Support discard frame buffer: %s\n", BOOL_TO_STRING(m_supportsDiscardFramebuffer));
    BEATS_PRINT("Support VAO: %s\n", BOOL_TO_STRING(m_supportsShareableVAO));
    BEATS_PRINT("Working on %d bit system\n", sizeof(void*) == 8 ? 64 : 32);

#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    BEATS_PRINT("Android Directory Info:\n");
    for (size_t i = 0; i < eAPT_Count; ++i)
    {
        BEATS_PRINT("%s: %s\n", pszAndroidPathString[i], CAndroidHandler::GetInstance()->GetAndroidPath(EAndroidPathType(i)).c_str());
    }
    BEATS_PRINT("PackageName: %s\n", CAndroidHandler::GetInstance()->GetPackageName().c_str());
#endif

    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}

EDeviceType CConfiguration::GetDeviceType() const
{
    BEATS_ASSERT(m_eDeviceType != eDT_UNKNOWN);
    return m_eDeviceType;
}

TString CConfiguration::GetNetworkStateString() const
{
    return strNetState[GetNetworkState()];
}

TString CConfiguration::GetDeviceTypeString() const
{
    return strDeviceType[GetDeviceType()];
}

