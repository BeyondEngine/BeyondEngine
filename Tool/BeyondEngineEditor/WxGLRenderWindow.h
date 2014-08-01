#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_WXGLRENDERWINDOW_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GLWINDOW_WXGLRENDERWINDOW_H__INCLUDE

#include "Render/RenderWindow.h"

class wxGLCanvas;
class wxGLContext;

class wxGLRenderWindow : public CRenderWindow
{
    typedef CRenderTarget super;
public:
    wxGLRenderWindow(wxGLCanvas *pCanvas);
    virtual ~wxGLRenderWindow();

    void SetContext(wxGLContext *pRC);
    wxGLContext *GetContext() const;
    void UseFBO(bool bUse);
    bool IsFBOInUse() const;
    void SetFBOViewPort(size_t uWidth, size_t uHeight);
    size_t GetFBOViewPortWidth() const;
    size_t GetFBOViewPortHeight() const;
    wxGLCanvas* GetCanvas() const;

    virtual void SetToCurrent() override;
    virtual void Render() override;

private:
    bool InitGlew();
    bool BindingFBO();

private:
    bool m_bUseFBO;
    bool m_bGlewInited;
    size_t m_uFBOViewPortWidth;
    size_t m_uFBOViewPortHeight;
    wxGLCanvas* m_pCanvas;
    wxGLContext *m_pRC;
};

#endif