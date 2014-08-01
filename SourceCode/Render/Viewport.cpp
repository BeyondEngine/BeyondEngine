#include "stdafx.h"
#include "Viewport.h"
#include "Renderer.h"
#include "Render/RenderTarget.h"

CViewport::CViewport(int nLeft, int nTop, int nWidth, int nHeight,
                     GLbitfield clearFlag, CColor clearColor,
                     float fClearDepth, int nClearStencil)
                     : m_pRenderTarget(nullptr)
                     , m_nLeft(nLeft)
                     , m_nTop(nTop)
                     , m_nWidth(nWidth)
                     , m_nHeight(nHeight)
                     , m_clearFlags(clearFlag)
                     , m_clearColor(clearColor)
                     , m_fClearDepth(fClearDepth)
                     , m_nClearStencil(nClearStencil)
{
#ifdef EDITOR_MODE
    m_clearColor = 0x0000FFFF;
#endif
}

CViewport::CViewport(CRenderTarget *pRenderTarget, GLbitfield clearFlag, 
                     CColor clearColor, float fClearDepth, int nClearStencil)
                     : m_pRenderTarget(pRenderTarget)
                     , m_nLeft(0)
                     , m_nTop(0)
                     , m_nWidth(pRenderTarget->GetWidth())
                     , m_nHeight(pRenderTarget->GetHeight())
                     , m_clearFlags(clearFlag)
                     , m_clearColor(clearColor)
                     , m_fClearDepth(fClearDepth)
                     , m_nClearStencil(nClearStencil)
{
#ifdef EDITOR_MODE
    m_clearColor = 0x0000FFFF;
#endif
}

CViewport::~CViewport()
{
}

void CViewport::SetRenderTarget(CRenderTarget *pRenderTarget)
{
    m_pRenderTarget = pRenderTarget;
}

void CViewport::SetLeft(int nLeft)
{
    m_nLeft = nLeft;
}

void CViewport::SetTop(int nTop)
{
    m_nTop = nTop;
}

void CViewport::SetWidth(int nWidth)
{
    m_nWidth = nWidth;
}

void CViewport::SetHeight(int nHeight)
{
    m_nHeight = nHeight;
}

int CViewport::Left() const
{
    return m_nLeft;
}

int CViewport::Top() const
{
    return m_nTop;
}

int CViewport::Width() const
{
    return m_nWidth;
}

int CViewport::Height() const
{
    return m_nHeight;
}

void CViewport::SetClearFlag(GLbitfield clearFlag)
{
    m_clearFlags = clearFlag;
}

void CViewport::SetClearColor(CColor color)
{
    m_clearColor = color;
}

void CViewport::SetClearDepth(float fClearDepth)
{
    m_fClearDepth = fClearDepth;
}

void CViewport::SetClearStencil(int nClearStencil)
{
    m_nClearStencil = nClearStencil;
}

void CViewport::Apply() const
{
    GLsizei iLeft = (GLsizei)(m_nLeft * m_pRenderTarget->GetScaleFactor());
    GLsizei iTop = (GLsizei)(m_nTop * m_pRenderTarget->GetScaleFactor());
    GLsizei iWidth = (GLsizei)(m_nWidth * m_pRenderTarget->GetScaleFactor());
    GLsizei iHeight = (GLsizei)(m_nHeight * m_pRenderTarget->GetScaleFactor());
    
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->Viewport(iLeft, iTop, iWidth, iHeight);
    if(m_clearFlags & GL_COLOR_BUFFER_BIT)
    {
        pRenderer->ClearColor(
            (float)m_clearColor.r / 0xFF,
            (float)m_clearColor.g / 0xFF,
            (float)m_clearColor.b / 0xFF,
            (float)m_clearColor.a / 0xFF);
    }
    if(m_clearFlags & GL_DEPTH_BUFFER_BIT)
    {
        //if the material set the write mask false,his clear is invalid,
        // so before call clear depth, you must set the write mask true
        pRenderer->DepthMask( true );
        pRenderer->ClearDepth(m_fClearDepth);
    }
    if(m_clearFlags & GL_STENCIL_BUFFER_BIT)
    {
        pRenderer->ClearStencil(m_nClearStencil);
    }

    // Should be enable scissor test due the clear region is
    // relied on scissor box bounds.
    GLboolean scissorTestEnabled = pRenderer->GetCurrentState()->GetBoolState(CBoolRenderStateParam::eBSP_ScissorTest);
    if (!scissorTestEnabled)
    {
        pRenderer->EnableGL(CBoolRenderStateParam::eBSP_ScissorTest);
    }

    // Sets the scissor box as same as viewport
    GLint scissor[4] = { 0, 0, 0, 0 };
    pRenderer->GetIntegerV(GL_SCISSOR_BOX, scissor);
    bool scissorBoxDifference = 
        iLeft != scissor[0] || iTop != scissor[1] ||
        iWidth != scissor[2] || iHeight != scissor[3];
    if(scissorBoxDifference)
    {
        pRenderer->SetScissorRect(iLeft, iTop, iWidth, iHeight);
    }

    // Clear buffers
    pRenderer->ClearBuffer(m_clearFlags);

    // Restore scissor box
    if (scissorBoxDifference)
    {
        pRenderer->SetScissorRect(scissor[0], scissor[1], scissor[2], scissor[3]);
    }
    // Restore scissor test
    if (!scissorTestEnabled)
    {
        pRenderer->DisableGL(CBoolRenderStateParam::eBSP_ScissorTest);
    }
}
