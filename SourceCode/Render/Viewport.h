#ifndef BEYOND_ENGINE_RENDER_VIEWPORT_H__INCLUDE
#define BEYOND_ENGINE_RENDER_VIEWPORT_H__INCLUDE

#include "CommonTypes.h"
#include "Event/EventSlot.h"

class CRenderTarget;
class CViewport
{
public:
    CViewport(int nLeft, int nTop, int nWidth, int nHeight,
        GLbitfield clearFlag = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        CColor clearColor = 0x000000FF, float fClearDepth = 1.0f, int nClearStencil = 0);
    CViewport(CRenderTarget *pRenderTarget,
        GLbitfield clearFlag = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        CColor clearColor = 0x000000FF, float fClearDepth = 1.0f, int nClearStencil = 0);
    ~CViewport();

    void SetRenderTarget(CRenderTarget *pRenderTarget);

    void SetLeft(int nLeft);
    void SetTop(int nTop);
    void SetWidth(int nWidth);
    void SetHeight(int nHeight);
    int Left() const;
    int Top() const;
    int Width() const;
    int Height() const;

    void SetClearFlag(GLbitfield clearFlag);
    void SetClearColor(CColor color);
    void SetClearDepth(float fClearDepth);
    void SetClearStencil(int nClearStencil);

    void Apply() const;

private:
    CRenderTarget *m_pRenderTarget;
    int m_nLeft;
    int m_nTop;
    int m_nWidth;
    int m_nHeight;
    CColor m_clearColor;
    float m_fClearDepth;
    int m_nClearStencil;
    GLbitfield m_clearFlags;
    CEventSlot m_slot;
};

#endif