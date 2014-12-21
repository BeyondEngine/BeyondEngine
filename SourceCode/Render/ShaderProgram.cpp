#include "stdafx.h"
#include "ShaderProgram.h"
#include "Renderer.h"
#include "RenderManager.h"


CShaderProgram::CShaderProgram(GLuint uVertexShader, GLuint uPixelShader)
    : m_uVertexShader(uVertexShader)
    , m_uPixelShader(uPixelShader)
    , m_uProgram(0xFFFFFFFF)
{
}

CShaderProgram::~CShaderProgram()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->DetachShader(ID(), m_uVertexShader);
    pRenderer->DetachShader(ID(), m_uPixelShader);

    pRenderer->DeleteProgram(ID());
}

GLuint CShaderProgram::ID()
{
    if (m_uProgram == 0xFFFFFFFF)
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        m_uProgram = pRenderer->CreateProgram();
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        pRenderer->AttachShader(m_uProgram, m_uVertexShader);
        pRenderer->AttachShader(m_uProgram, m_uPixelShader);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();

        //TODO:Move it outside
        for (uint32_t i = 0; i < ATTRIB_INDEX_COUNT; ++i)
        {
            pRenderer->BindAttribLocation(m_uProgram, static_cast<ECommonAttribIndex>(i));
        }

        bool bRet = Link();
        BEYONDENGINE_UNUSED_PARAM(bRet);
        BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
        BEATS_ASSERT(bRet, _T("Link shader program failed!"));
    }
    BEATS_ASSERT(m_uProgram != 0xFFFFFFFF);
    return m_uProgram;
}

GLuint CShaderProgram::GetVertexShader() const
{
    return m_uVertexShader;
}

GLuint CShaderProgram::GetPixelShader() const
{
    return m_uPixelShader;
}

bool CShaderProgram::Link()
{
    BEATS_ASSERT(ID() > 0 && m_uVertexShader > 0 && m_uPixelShader > 0, _T("Cannot link invalid program"));

    GLint status = GL_TRUE;

    CRenderer::GetInstance()->LinkProgram(ID());

#ifdef _DEBUG
    CRenderer::GetInstance()->GetProgramiv(ID(), GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
        int infoLength = 0;
        CRenderer::GetInstance()->GetProgramiv(ID(), GL_INFO_LOG_LENGTH, &infoLength);
        uint32_t uBufferSize = sizeof(GLchar)* infoLength + 1;
        char* pBuffer = new char[uBufferSize];
        memset(pBuffer, 0, uBufferSize);
        CRenderer::GetInstance()->GetProgramInfoLog(ID(), uBufferSize, &infoLength, pBuffer);
        BEATS_ASSERT(false, _T("Failed to link shader program: %d Reason: %s"), m_uProgram, pBuffer);
        BEATS_SAFE_DELETE_ARRAY(pBuffer);
        CRenderer::GetInstance()->DeleteProgram( m_uProgram );
        m_uProgram = 0;
    }
#endif

    return (status == GL_TRUE);
}
