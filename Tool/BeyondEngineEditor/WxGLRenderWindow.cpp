#include "stdafx.h"
#include "WxGLRenderWindow.h"
#include "wx/glcanvas.h"
#include "Render/RenderManager.h"
#include "Render/Viewport.h"
#include "Render/Renderer.h"

wxGLRenderWindow::wxGLRenderWindow(wxGLCanvas *pCanvas)
    : CRenderWindow(pCanvas->GetSize().GetWidth(), pCanvas->GetSize().GetHeight(), true)
    , m_bUseFBO(false)
    , m_bGlewInited(false)
    , m_uFBOViewPortWidth(0)
    , m_uFBOViewPortHeight(0)
    , m_pCanvas(pCanvas)
    , m_pRC(nullptr)
{

}

wxGLRenderWindow::~wxGLRenderWindow()
{

}

void wxGLRenderWindow::SetToCurrent()
{
    m_pCanvas->SetCurrent(*m_pRC);
    if(!m_bGlewInited)
    {
        InitGlew();
        m_bGlewInited = true;
    }
}

wxGLContext *wxGLRenderWindow::GetContext() const
{
    return m_pRC;
}

void wxGLRenderWindow::UseFBO(bool bUse)
{
    m_bUseFBO = bUse;
}

bool wxGLRenderWindow::IsFBOInUse() const
{
    return m_bUseFBO;
}

void wxGLRenderWindow::SetFBOViewPort(size_t uWidth, size_t uHeight)
{
    m_uFBOViewPortWidth = uWidth;
    m_uFBOViewPortHeight = uHeight;
}

size_t wxGLRenderWindow::GetFBOViewPortWidth() const
{
    return m_uFBOViewPortWidth;
}

size_t wxGLRenderWindow::GetFBOViewPortHeight() const
{
    return m_uFBOViewPortHeight;
}

wxGLCanvas* wxGLRenderWindow::GetCanvas() const
{
    return m_pCanvas;
}

void wxGLRenderWindow::Render()
{
    if (IsFBOInUse())
    {
        CRenderer::GetInstance()->BindFramebuffer(GL_FRAMEBUFFER, CRenderManager::GetInstance()->GetMainFBOTexture());
    }
    super::Render();
    if (IsFBOInUse())
    {
        CRenderer::GetInstance()->BindFramebuffer(GL_FRAMEBUFFER, 0);
        CRenderer::GetInstance()->Viewport(0, 0, m_uFBOViewPortWidth, m_uFBOViewPortHeight);
        CRenderManager::GetInstance()->RenderTextureToFullScreen(CRenderManager::GetInstance()->GetMainFBOTexture());
    }
    m_pCanvas->SwapBuffers();
}

void wxGLRenderWindow::SetContext(wxGLContext *pRC)
{
    m_pRC = pRC;
    if(CRenderManager::GetInstance()->GetCurrentRenderTarget() == this)
    {
        SetToCurrent();
    }
}

bool wxGLRenderWindow::InitGlew()
{
    GLenum GlewInitResult = glewInit();
    bool bRet = GlewInitResult == GLEW_OK;
    BEATS_ASSERT(bRet, _T("OpenGL error: %s"), (TCHAR *)glewGetErrorString(GlewInitResult));
    BEATS_WARNING(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader, _T("GLSL is not ready!"));
    BEATS_WARNING(glewIsSupported("GL_VERSION_2_0"), _T("OpenGL 2.0 is not supported!"));
    bool bFrameBufferSupport = BindingFBO();
    BEATS_ASSERT(bFrameBufferSupport,  _T("No OpenGL framebuffer support. Please upgrade the driver of your video card."));
    return bRet && bFrameBufferSupport;
}

bool wxGLRenderWindow::BindingFBO()
{
    const char *gl_extensions = (const char*)glGetString(GL_EXTENSIONS);

    // If the current opengl driver doesn't have framebuffers methods, check if an extension exists
    if (glGenFramebuffers == NULL)
    {
        BEATS_PRINT(_T("OpenGL: glGenFramebuffers is NULL, try to detect an extension"));
        if (strstr(gl_extensions, "ARB_framebuffer_object"))
        {
            BEATS_PRINT(_T("OpenGL: ARB_framebuffer_object is supported"));

            glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) wglGetProcAddress("glIsRenderbuffer");
            glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbuffer");
            glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) wglGetProcAddress("glDeleteRenderbuffers");
            glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffers");
            glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorage");
            glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) wglGetProcAddress("glGetRenderbufferParameteriv");
            glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) wglGetProcAddress("glIsFramebuffer");
            glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebuffer");
            glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffers");
            glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffers");
            glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatus");
            glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) wglGetProcAddress("glFramebufferTexture1D");
            glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2D");
            glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) wglGetProcAddress("glFramebufferTexture3D");
            glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbuffer");
            glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
            glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmap");
        }
        else if (strstr(gl_extensions, "EXT_framebuffer_object"))
        {
            BEATS_PRINT(_T("OpenGL: EXT_framebuffer_object is supported"));
            glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) wglGetProcAddress("glIsRenderbufferEXT");
            glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbufferEXT");
            glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) wglGetProcAddress("glDeleteRenderbuffersEXT");
            glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffersEXT");
            glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorageEXT");
            glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) wglGetProcAddress("glGetRenderbufferParameterivEXT");
            glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) wglGetProcAddress("glIsFramebufferEXT");
            glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebufferEXT");
            glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffersEXT");
            glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffersEXT");
            glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatusEXT");
            glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) wglGetProcAddress("glFramebufferTexture1DEXT");
            glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2DEXT");
            glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) wglGetProcAddress("glFramebufferTexture3DEXT");
            glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbufferEXT");
            glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
            glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmapEXT");
        }
        else
        {
            BEATS_PRINT(_T("OpenGL: No framebuffers extension is supported"));
            BEATS_PRINT(_T("OpenGL: Any call to Fbo will crash!"));
            return false;
        }
    }
    return true;
}