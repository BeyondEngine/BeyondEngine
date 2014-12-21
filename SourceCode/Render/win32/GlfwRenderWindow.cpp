#include "stdafx.h"
#include "GlfwRenderWindow.h"
#include "Render/RenderManager.h"
#include "Event/TouchDelegate.h"
#include "Log/LogManager.h"
#include "Utility/Memory/Memory.h"
#ifdef MEMORY_CAPTURE
#include "Memory/MemoryAnalyzer.h"
#endif
//#define ENABLE_DEFAULT_CAMERA_MOUSE_CONTROL

std::map<GLFWwindow *, CGlfwRenderWindow *> CGlfwRenderWindow::m_sInstanceMap;

CGlfwRenderWindow::CGlfwRenderWindow(int nWidth, int nHeight)
    : super(nWidth, nHeight)
    , m_pMainWindow(nullptr)
    , m_bGlewInited(false)
    , m_bLeftMouseDown(false)
    , m_uLastMousePosX(0)
    , m_uLastMousePosY(0)
    , m_uCurMousePosX(0)
    , m_uCurMousePosY(0)
{
    glfwSetErrorCallback(CGlfwRenderWindow::onGLFWError);
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

    m_pMainWindow = glfwCreateWindow(nWidth, nHeight, "StarRaiders", NULL, NULL);
    m_sInstanceMap[m_pMainWindow] = this;

    glfwSetMouseButtonCallback(m_pMainWindow,CGlfwRenderWindow::onGLFWMouseCallBack);
    glfwSetCursorPosCallback(m_pMainWindow,CGlfwRenderWindow::onGLFWMouseMoveCallBack);
    glfwSetScrollCallback(m_pMainWindow, CGlfwRenderWindow::onGLFWMouseScrollCallback);
    glfwSetCharCallback(m_pMainWindow, CGlfwRenderWindow::onGLFWCharCallback);
    glfwSetKeyCallback(m_pMainWindow, CGlfwRenderWindow::onGLFWKeyCallback);
    glfwSetWindowSizeCallback(m_pMainWindow, CGlfwRenderWindow::onGLFWSizeChangeCallback);
}
CGlfwRenderWindow::~CGlfwRenderWindow()
{
    glfwDestroyWindow(m_pMainWindow);
    glfwTerminate();
}

void CGlfwRenderWindow::SetToCurrent()
{
    glfwMakeContextCurrent(m_pMainWindow);
    if(!m_bGlewInited)
    {
        InitGlew();
        m_bGlewInited = true;
    }
}

void CGlfwRenderWindow::Render()
{
    super::Render();
    BEATS_ASSERT(m_pMainWindow);
    glfwSwapBuffers(m_pMainWindow);
}

GLFWwindow *CGlfwRenderWindow::GetMainWindow() const
{
    return m_pMainWindow;
}

bool CGlfwRenderWindow::InitGlew()
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

bool CGlfwRenderWindow::BindingFBO()
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

void CGlfwRenderWindow::onGLFWError( int errorID, const char* errorDesc )
{
    BEYONDENGINE_UNUSED_PARAM(errorID);
    BEYONDENGINE_UNUSED_PARAM(errorDesc);
    BEATS_ASSERT(false, _T("An error was detected! ID : %d\ndesc : %s"), errorID, errorDesc);
}

void CGlfwRenderWindow::onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int /*modify*/)
{
    CGlfwRenderWindow *pObject = m_sInstanceMap[window];
    BEATS_ASSERT(pObject);

    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            pObject->m_bLeftMouseDown = true;
            float x = (float)pObject->m_uCurMousePosX;
            float y = (float)pObject->m_uCurMousePosY;
            size_t id[1] = { 0 };
            CTouchDelegate::GetInstance()->OnTouchBegan( 1, id, &x, &y);
        }
        else if(action == GLFW_RELEASE)
        {
            pObject->m_bLeftMouseDown = false;
            float x = (float)pObject->m_uCurMousePosX;
            float y = (float)pObject->m_uCurMousePosY;
            size_t id[1] = {0};
            CTouchDelegate::GetInstance()->OnTouchEnded( 1, id, &x,&y);
        }
    }
}

void CGlfwRenderWindow::onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
{
    CGlfwRenderWindow *pObject = m_sInstanceMap[window];
    BEATS_ASSERT(pObject);
    x /= pObject->GetScaleFactor();
    y /= pObject->GetScaleFactor();
    pObject->m_uCurMousePosX = (uint32_t)x;
    pObject->m_uCurMousePosY = (uint32_t)y;

    int width = 0, height = 0;
    width = pObject->GetLogicWidth();
    height = pObject->GetLogicHeight();
    if(x < width  && x > 0 && y < height && y > 0)
    {
        if (pObject->m_bLeftMouseDown)
        {
#ifdef ENABLE_DEFAULT_CAMERA_MOUSE_CONTROL
            int iDeltaX = pObject->m_uLastMousePosX - pObject->m_uCurMousePosX;
            int iDeltaY = pObject->m_uLastMousePosY - pObject->m_uCurMousePosY;


            CScene *pCurrScene = CSceneManager::GetInstance()->GetCurrentScene();
            if(pCurrScene)
            {
                CCamera* pCamera = pCurrScene->GetCamera(CCamera::eCT_3D);
                if(pCamera)
                {
                    float fYawValue = (float)iDeltaX / pObject->GetLogicWidth();
                    pCamera->Yaw(fYawValue);

                    float fPitchValue = (float)iDeltaY / pObject->GetLogicHeight();
                    pCamera->Pitch( fPitchValue);
                }
            }
#endif
            size_t id[1] = {0};
            float rx = (float)x;
            float ry = (float)y;
            CTouchDelegate::GetInstance()->OnTouchMoved( 1, id, &rx, &ry );
        }
    }

    pObject->m_uLastMousePosX = (uint32_t)x;
    pObject->m_uLastMousePosY = (uint32_t)y;
}

void CGlfwRenderWindow::onGLFWMouseScrollCallback(GLFWwindow* /*window*/, double /*x*/, double y)
{
#ifdef ENABLE_DEFAULT_CAMERA_MOUSE_CONTROL
    CScene *pCurrScene = CSceneManager::GetInstance()->GetCurrentScene();
    if(pCurrScene)
    {
        CCamera* pCamera = pCurrScene->GetCamera(CCamera::eCT_3D);
        if(pCamera)
        {
            float fSpeed = (float)y * 0.2F;
            pCamera->Translate(0, 0, -fSpeed);
        }
    }
#endif
    CTouchDelegate::GetInstance()->OnPinched(eGS_BEGAN, (float)y);
    CTouchDelegate::GetInstance()->OnPinched(eGS_ENDED, (float)y * 2);
}

void CGlfwRenderWindow::onGLFWKeyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
    switch (key)
    {
    case GLFW_KEY_F1:
        if (action == GLFW_PRESS)
        {
#ifdef DEVELOP_VERSION
            CLogManager::GetInstance()->Show(!CLogManager::GetInstance()->IsShown());
#endif
        }
        break;
    case GLFW_KEY_F3:
        if (action == GLFW_PRESS)
        {
            CRenderManager::GetInstance()->SwitchPolygonMode();
        }
        break;
    case GLFW_KEY_F4:
        if (action == GLFW_PRESS)
        {
#ifdef MEMORY_CAPTURE
            CMemoryAnalyzer::GetInstance()->DumpToFile("mem.log");
#endif
        }
        break;
    case GLFW_KEY_F5:
        if (action == GLFW_PRESS)
        {
        }
        break;
    case GLFW_KEY_F6:
        if (action == GLFW_PRESS)
        {
            _CrtDumpMemoryLeaks();
        }
        break;
    case GLFW_KEY_BACKSPACE:
        if (action == GLFW_PRESS)
        {
        }
        break;
    case GLFW_KEY_UP:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
#ifdef DEVELOP_VERSION
            if (CLogManager::GetInstance()->IsShown())
            {
                CLogManager::GetInstance()->DecreaseRenderPos();
            }
#endif
        }
        break;
    case GLFW_KEY_DOWN:
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
#ifdef DEVELOP_VERSION
            if (CLogManager::GetInstance()->IsShown())
            {
                CLogManager::GetInstance()->IncreaseRenderPos();
            }
#endif
        }
        break;
    case GLFW_KEY_HOME:
        if (action == GLFW_PRESS)
        {
#ifdef DEVELOP_VERSION
            if (CLogManager::GetInstance()->IsShown())
            {
                CLogManager::GetInstance()->SetRenderPosToTop();
            }
#endif
        }
        break;
    case GLFW_KEY_END:
        if (action == GLFW_PRESS)
        {
#ifdef DEVELOP_VERSION
            if (CLogManager::GetInstance()->IsShown())
            {
                CLogManager::GetInstance()->ClearRenderPos();
            }
#endif
        }
        break;
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS)
        {
        }
        break;
    default:
        break;
    }
}

void CGlfwRenderWindow::onGLFWCharCallback(GLFWwindow* /*window*/, unsigned int character)
{
    WPARAM wParam = character;
    if (wParam < 0x20 )
    {
        if (VK_BACK == wParam)
        {
        }
        else if (VK_TAB == wParam)
        {
            // tab input
        }
        else if (VK_ESCAPE == wParam)
        {
            // ESC input
        }
    }
    else if (wParam < 128)
    {
    }
    else
    {
    }
}

void CGlfwRenderWindow::onGLFWSizeChangeCallback(GLFWwindow* /*window*/, int nWidth, int nHeight)
{
    if (nWidth == 0 && nHeight == 0)
    {
        CApplication::GetInstance()->Pause();
    }
    else if (!CApplication::GetInstance()->IsRunning())
    {
        CApplication::GetInstance()->Resume();
    }
}