#ifndef BEYOND_ENGINE_RENDER_RENDERTARGET_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERTARGET_H__INCLUDE

#include "RenderEventType.h"
#include "Event/EventDispatcher.h"

class CViewport;
class CRenderTarget
{
public:
    CRenderTarget(size_t nWidth, size_t nHeight, bool bAutoCreateViewport = false, float fScaleFactor = 1.f);
    virtual ~CRenderTarget();

    void SetSize(size_t nWidth, size_t nHeight);
    size_t GetWidth() const;
    size_t GetHeight() const;

    void SetScaleFactor(float fScaleFactor);
    float GetScaleFactor() const;

    void SetViewport(CViewport *pViewport);
    CViewport *GetViewport() const;

    virtual void SetToCurrent();
    virtual void Render();

private:
    size_t m_uWidth;
    size_t m_uHeight;
    bool m_bAutoCreateViewport;
    float m_fScaleFactor;
    CViewport* m_pViewport;
};

#endif