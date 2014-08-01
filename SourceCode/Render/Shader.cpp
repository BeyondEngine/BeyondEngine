#include "stdafx.h"
#include "Shader.h"
#include "Renderer.h"
#include "Resource/ResourcePathManager.h"
#include "Resource/ResourceManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"

CSerializer CShader::m_commonHeader;
CSerializer CShader::m_vsHeader;
CSerializer CShader::m_psHeader;

CShader::CShader()
    : m_shaderType(0)
    , m_uId(0)
{

}

CShader::~CShader()
{
    CRenderer::GetInstance()->DeleteShader( m_uId );
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

    if(m_commonHeader.GetWritePos() == 0)
    {
#ifdef USE_UBO
        char def[] = "#define USE_UBO\r\n";
        m_commonHeader.Serialize((const void *)def, sizeof(def) - 1);
#endif
        TString strPath = CResourceManager::GetInstance()->GetFullPath(_T("CommonHeader.txt"), eRT_Shader);
        m_commonHeader.Serialize(strPath.c_str());
    }

    if(m_vsHeader.GetWritePos() == 0)
    {
        TString strPath = CResourceManager::GetInstance()->GetFullPath(_T("Header.vs"), eRT_Shader);
        m_vsHeader.Serialize(strPath.c_str());
    }

    if(m_psHeader.GetWritePos() == 0)
    {
        TString strPath = CResourceManager::GetInstance()->GetFullPath(_T("Header.ps"), eRT_Shader);
        m_psHeader.Serialize(strPath.c_str());
    }
    BEATS_ASSERT(m_pData == NULL);
    m_pData = new CSerializer(m_strPath.m_value.c_str());
    super::Load();
    return true;
}

void CShader::Initialize()
{
    super::Initialize();
    bool bRet = Compile(&m_uId, (const GLchar*)m_pData->GetBuffer(), m_pData->GetWritePos());
    BEYONDENGINE_UNUSED_PARAM(bRet);
    BEATS_SAFE_DELETE(m_pData);
    BEATS_ASSERT(bRet);
}

GLuint CShader::ID() const
{
     return m_uId;
}

bool CShader::Compile(GLuint * shader, const GLchar* pszSource, GLint length)
{
    GLint status = GL_FALSE;

    if (pszSource != NULL)
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        BEATS_ASSERT( m_shaderType == GL_VERTEX_SHADER || m_shaderType == GL_FRAGMENT_SHADER);
        *shader = pRenderer->CreateShader(m_shaderType);

        const GLchar *sources[3] = {0};
        GLint lengths[3] = {0};

        sources[0] = reinterpret_cast<const GLchar *>(m_commonHeader.GetBuffer());
        lengths[0] = m_commonHeader.GetWritePos();
        if(m_shaderType == GL_VERTEX_SHADER)
        {
            sources[1] = reinterpret_cast<const GLchar *>(m_vsHeader.GetBuffer());
            lengths[1] = m_vsHeader.GetWritePos();
        }
        else
        {
            sources[1] = reinterpret_cast<const GLchar *>(m_psHeader.GetBuffer());
            lengths[1] = m_psHeader.GetWritePos();
        }
        sources[2] = pszSource;
        lengths[2] = length;
        pRenderer->ShaderSource(*shader, 3, sources, lengths );
        pRenderer->CompileShader(*shader);
#ifdef _DEBUG
        pRenderer->GetShaderiv(*shader, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            GLsizei length = 0;
            pRenderer->GetShaderiv(*shader, GL_INFO_LOG_LENGTH, &length);
            GLchar* src = (GLchar *)new char[sizeof(GLchar) * length];

            pRenderer->GetShaderInfoLog(*shader, length, nullptr, src);
            TCHAR *buf = new TCHAR[length];
            CStringHelper::GetInstance()->ConvertToTCHAR(src, buf, length);
            BEATS_ASSERT(false, _T("ERROR: Failed to compile shader: %s"), buf);

            BEATS_SAFE_DELETE_ARRAY(buf);
            BEATS_SAFE_DELETE(src);
        }
#endif
    }
    return (status == GL_TRUE);
}


