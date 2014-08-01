#include "stdafx.h"
#include "ShaderProgram.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "RenderManager.h"
#include "Renderer.h"
#include "RenderPublic.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Resource/ResourcePathManager.h"


CShaderProgram::CShaderProgram(GLuint uVertexShader, GLuint uPixelShader)
    : m_uVertexShader(uVertexShader)
    , m_uPixelShader(uPixelShader)
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    m_uProgram = pRenderer->CreateProgram();
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    pRenderer->AttachShader(m_uProgram, m_uVertexShader);
    pRenderer->AttachShader(m_uProgram, m_uPixelShader);
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();

    //TODO:Move it outside
    for(size_t i = 0; i < ATTRIB_INDEX_COUNT; ++i)
    {
        pRenderer->BindAttribLocation(m_uProgram, static_cast<ECommonAttribIndex>(i));
    }

    bool bRet = Link();
    bRet;
    BEYONDENGINE_CHECK_GL_ERROR_DEBUG();
    BEATS_ASSERT(bRet, _T("Link shader program failed!"));

    for(size_t i = 0; i < UNIFORM_COUNT; ++i)
    {
        m_uniformLocations[static_cast<ECommonUniformType>(i)] = pRenderer->GetUniformLocation(m_uProgram, COMMON_UNIFORM_NAMES[i]);
    }

#ifdef USE_UBO
        for(size_t i = 0; i < UNIFORM_BLOCK_COUNT; ++i)
        {
            GLint index = glGetUniformBlockIndex(m_uProgram, COMMON_UNIFORM_BLOCK_NAMES[i]);
            if(index != GL_INVALID_INDEX)
            {
                GLuint ubo = CRenderManager::GetInstance()->GetUBO(static_cast<ECommonUniformBlockType>(i));
                GLint blockSize;
                pRenderer->GetActiveUniformBlockiv(
                    m_uProgram, index, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
                pRenderer->BindBuffer(GL_UNIFORM_BUFFER, ubo);
                pRenderer->BindBufferRange(GL_UNIFORM_BUFFER, i, ubo, 0, blockSize);
                pRenderer->UniformBlockBinding(m_uProgram, index, i);
            }
        }
#endif
}

CShaderProgram::~CShaderProgram()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->DetachShader(m_uProgram, m_uVertexShader);
    pRenderer->DetachShader(m_uProgram, m_uPixelShader);

    pRenderer->DeleteProgram( m_uProgram );
}

GLuint CShaderProgram::ID() const
{
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
    BEATS_ASSERT(m_uProgram > 0 && m_uVertexShader > 0 && m_uPixelShader > 0, _T("Cannot link invalid program"));

    GLint status = GL_TRUE;

    CRenderer::GetInstance()->LinkProgram(m_uProgram);

#ifdef _DEBUG
    CRenderer::GetInstance()->GetProgramiv(m_uProgram, GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
        BEATS_ASSERT(false, _T("cocos2d: ERROR: Failed to link program: %i"), m_uProgram);
        CRenderer::GetInstance()->DeleteProgram( m_uProgram );
        m_uProgram = 0;
    }
#endif

    return (status == GL_TRUE);
}
