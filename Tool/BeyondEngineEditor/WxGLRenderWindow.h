#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_WXGLRENDERWINDOW_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_WXGLRENDERWINDOW_H__INCLUDE

#include "Render/RenderTarget.h"

class wxGLCanvas;
class wxGLContext;

class wxGLRenderWindow : public CRenderTarget
{
    typedef CRenderTarget super;
public:
    wxGLRenderWindow(wxGLCanvas *pCanvas);
    virtual ~wxGLRenderWindow();

    void SetContext(wxGLContext *pRC);
    wxGLContext *GetContext() const;
    void UseFBO(bool bUse);
    bool IsFBOInUse() const;
    void SetFBOViewPort(uint32_t uWidth, uint32_t uHeight);
    uint32_t GetFBOViewPortWidth() const;
    uint32_t GetFBOViewPortHeight() const;
    wxGLCanvas* GetCanvas() const;
    CVec2 ScreenToClient(const CVec2& screenPos) const;

    virtual void SetToCurrent() override;
    virtual void Render() override;

private:
    bool InitGlew();
    bool BindingFBO();

private:
    bool m_bUseFBO;
    bool m_bUsePostProcessOriginValue;
    bool m_bGlewInited;
    uint32_t m_uFBOViewPortWidth;
    uint32_t m_uFBOViewPortHeight;
    wxGLCanvas* m_pCanvas;
    wxGLContext *m_pRC;
};

#endif