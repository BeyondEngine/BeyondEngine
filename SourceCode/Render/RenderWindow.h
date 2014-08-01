#ifndef BEYOND_ENGINE_RENDER_RENDERWINDOW_H__INCLUDE
#define BEYOND_ENGINE_RENDER_RENDERWINDOW_H__INCLUDE

#include "RenderTarget.h"

class CRenderWindow : public CRenderTarget
{
    typedef CRenderTarget super;
public:
    CRenderWindow(size_t nDeviceWidth, size_t nDeviceHeight, bool bAutoCreateViewport = false);
    virtual ~CRenderWindow();

    CEventDispatcher *EventDispatcher() const;

    void SetDeviceResolution(size_t nDeviceWidth, size_t nDeviceHeight);
    size_t GetDeviceWidth() const;
    size_t GetDeviceHeight() const;

private:
    void AdjustResolution();

private:
    size_t m_nDeviceWidth;
    size_t m_nDeviceHeight;
    CEventDispatcher *m_pEventDispatcher;
};

#endif