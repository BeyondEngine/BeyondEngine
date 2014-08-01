#include "stdafx.h"
#include "Configuration.h"
#if(BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
#define sprintf_s sprintf
#endif

CConfiguration* CConfiguration::m_pInstance = NULL;

CConfiguration::CConfiguration()
{
    m_maxTextureSize = 0;
    m_maxModeviewStackDepth = 0;
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
    m_strGL_Extensions = NULL;
}

CConfiguration::~CConfiguration()
{
}

int CConfiguration::GetMaxTextureSize() const
{
    return m_maxTextureSize;
}

int CConfiguration::GetMaxModelviewStackDepth() const
{
    return m_maxModeviewStackDepth;
}

int CConfiguration::GetMaxTextureUnits() const
{
    return m_maxTextureUnits;
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
    return m_supportsShareableVAO;
}

void CConfiguration::SetSupportShareableVAO(bool bEnable)
{
    m_supportsShareableVAO = bEnable;
}

bool CConfiguration::CheckForGLExtension( const TString& searchName ) const
{
    bool bReturn = false;
    char name[ MAX_PATH ];
    CStringHelper::GetInstance()->ConvertToCHAR( searchName.c_str(), name, MAX_PATH );
    bReturn = (m_strGL_Extensions && strstr(m_strGL_Extensions, name) ) ? true : false;
    return bReturn;
}

const TString& CConfiguration::GetInfo()
{
    return m_strInfo;
}

#define BOOL_TO_STRING(boolValue) ((boolValue) ? "true" : "false")
void CConfiguration::GatherGPUInfo()
{
    m_strGL_Vendor = (const char*)glGetString(GL_VENDOR);
    m_strGL_Renderer = (const char*)glGetString(GL_RENDERER);
    m_strGL_Version = (const char*)glGetString(GL_VERSION);
    m_strGL_Extensions = (const char*)glGetString(GL_EXTENSIONS);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_maxTextureUnits);

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

    std::string strInfo;
    char szBuffer[1024];
    strInfo.append("gl_vendor:").append(m_strGL_Vendor).append("\n");
    strInfo.append("gl_renderer:").append(m_strGL_Renderer).append("\n");
    strInfo.append("gl_version:").append(m_strGL_Version).append("\n");

    sprintf_s(szBuffer, "Max texture size: %d\n", m_maxTextureSize);
    strInfo.append(szBuffer);
    sprintf_s(szBuffer, "Max texture units: %d\n", m_maxTextureUnits);
    strInfo.append(szBuffer);
    sprintf_s(szBuffer, "Support ETC1: %s\n", BOOL_TO_STRING(m_supportsETC1));
    strInfo.append(szBuffer);
    sprintf_s(szBuffer, "Support S3TC: %s\n", BOOL_TO_STRING(m_supportsS3TC));
    strInfo.append(szBuffer);
    sprintf_s(szBuffer, "Support ATITC: %s\n", BOOL_TO_STRING(m_supportsATITC));
    strInfo.append(szBuffer);
    sprintf_s(szBuffer, "Support PVRTC: %s\n", BOOL_TO_STRING(m_supportsPVRTC));
    strInfo.append(szBuffer);
    sprintf_s(szBuffer, "Support BGRA8888: %s\n", BOOL_TO_STRING(m_supportsBGRA8888));
    strInfo.append(szBuffer);
    sprintf_s(szBuffer, "Support discard frame buffer: %s\n", BOOL_TO_STRING(m_supportsDiscardFramebuffer));
    strInfo.append(szBuffer);
    sprintf_s(szBuffer, "Support VAO: %s\n", BOOL_TO_STRING(m_supportsShareableVAO));
    strInfo.append(szBuffer);

    TCHAR* szTcharBuffer = new TCHAR[strInfo.length() + 1];
    CStringHelper::GetInstance()->ConvertToTCHAR(strInfo.c_str(), szTcharBuffer, strInfo.length() + 1);
    m_strInfo.assign(szTcharBuffer);
    BEATS_SAFE_DELETE_ARRAY(szTcharBuffer);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
}
