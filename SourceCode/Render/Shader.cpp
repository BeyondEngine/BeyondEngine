#include "stdafx.h"
#include "Shader.h"
#include "Renderer.h"
#include "Resource/ResourceManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"

CSerializer CShader::m_commonHeader;

CShader::CShader()
: m_shaderType(0)
, m_uId(0)
{

}

CShader::~CShader()
{
    if (IsInitialized())
    {
        Uninitialize();
    }
}

GLenum CShader::GetShaderType()const
{
    return m_shaderType;
}

void CShader::SetShaderType(GLenum shaderType)
{
    m_shaderType = shaderType;
}

bool CShader::Load()
{
    TString strExtension = CFilePathTool::GetInstance()->Extension(GetFilePath().c_str());
    bool bIsVS = strExtension.compare(_T(".vs")) == 0;
    m_shaderType = bIsVS ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;

    if (m_commonHeader.GetWritePos() == 0)
    {
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
        const char* pszPlatformDef = "#define PLATFORM_ANDROID\r\n";
#elif (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
        const char* pszPlatformDef = "#define PLATFORM_IOS\r\n";
#else
        const char* pszPlatformDef = "#define PLATFORM_WIN32\r\n";
#endif
        m_commonHeader.Serialize((const void *)pszPlatformDef, strlen(pszPlatformDef));
        TString strPath = CResourceManager::GetInstance()->GetFullPath(_T("commonheader.txt"), eRT_Shader);
        m_commonHeader.Serialize(strPath.c_str());
    }

    BEATS_ASSERT(m_pData == NULL);
    m_pData = new CSerializer(m_commonHeader);
    BEATS_ASSERT(CFilePathTool::GetInstance()->Exists(m_strPath.m_value.c_str()), "Can't find shader file %s", m_strPath.m_value.c_str())
    m_pData->Serialize(m_strPath.m_value.c_str());
    super::Load();
    return true;
}

bool CShader::Unload()
{
    BEATS_SAFE_DELETE(m_pData);
    return super::Unload();
}

void CShader::Initialize()
{
    super::Initialize();
    bool bRet = Compile(&m_uId, (const GLchar*)m_pData->GetBuffer(), m_pData->GetWritePos());
    BEYONDENGINE_UNUSED_PARAM(bRet);
    BEATS_SAFE_DELETE(m_pData);
    BEATS_ASSERT(bRet, _T("Compile shader %s failed!"), GetFilePath().c_str());
}

void CShader::Uninitialize()
{
    super::Uninitialize();
    CRenderer::GetInstance()->DeleteShader(m_uId);
    m_uId = 0;
}

bool CShader::ShouldClean() const
{
    return CApplication::GetInstance()->IsDestructing();//Shader will never be cleaned except exit the program.
}

GLuint CShader::ID() const
{
    return m_uId;
}

bool CShader::Compile(GLuint * shader, const GLchar* pszSource, GLint length)
{
    GLint status = pszSource != NULL;

    if (pszSource != NULL)
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        BEATS_ASSERT(m_shaderType == GL_VERTEX_SHADER || m_shaderType == GL_FRAGMENT_SHADER);
        *shader = pRenderer->CreateShader(m_shaderType);
        pRenderer->ShaderSource(*shader, 1, &pszSource, &length);
        pRenderer->CompileShader(*shader);
#ifdef _DEBUG
        pRenderer->GetShaderiv(*shader, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            GLsizei length = 0;
            pRenderer->GetShaderiv(*shader, GL_INFO_LOG_LENGTH, &length);
            BEATS_ASSERT(length > 0);
            GLchar* src = (GLchar *)new char[sizeof(GLchar)* length];

            pRenderer->GetShaderInfoLog(*shader, length, nullptr, src);
            BEATS_ASSERT(false, _T("ERROR: Failed to compile shader %s: Reason:%s"), m_strPath.m_value.c_str(), src);
            BEATS_SAFE_DELETE(src);
        }
#endif
    }
    return (status == GL_TRUE);
}


