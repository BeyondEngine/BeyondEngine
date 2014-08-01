#include "stdafx.h"
#include "GlfwRenderWindow.h"
#include "Render/RenderManager.h"
#include "Event/TouchDelegate.h"
#include "Event/KeyboardEvent.h"
#include "Scene/SceneManager.h"
#include "TimeMeter.h"
#include "Scene/Scene.h"

//#define ENABLE_DEFAULT_CAMERA_MOUSE_CONTROL

std::map<GLFWwindow *, CGlfwRenderWindow *> CGlfwRenderWindow::m_sInstanceMap;

CGlfwRenderWindow::CGlfwRenderWindow(int nWidth, int nHeight, bool bAutoCreateViewport)
    : CRenderWindow(nWidth, nHeight, bAutoCreateViewport)
    , m_pMainWindow(nullptr)
    , m_bGlewInited(false)
    , m_bLeftMouseDown(false)
    , m_uLastMousePosX(0)
    , m_uLastMousePosY(0)
    , m_uCurMousePosX(0)
    , m_uCurMousePosY(0)
    , m_uPressedMousePosX(0)
    , m_uPressedMousePosY(0)
    , m_fPressStartYaw(0.f)
    , m_fPressStartPitch(0.f)
    , m_uLastTapTime(0)
    , m_uTapCount(0)
{
    glfwSetErrorCallback(CGlfwRenderWindow::onGLFWError);
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

    m_pMainWindow = glfwCreateWindow(GetDeviceWidth(), GetDeviceHeight(),
        "Unknown window", NULL, NULL);
    m_sInstanceMap[m_pMainWindow] = this;

    glfwSetMouseButtonCallback(m_pMainWindow,CGlfwRenderWindow::onGLFWMouseCallBack);
    glfwSetCursorPosCallback(m_pMainWindow,CGlfwRenderWindow::onGLFWMouseMoveCallBack);
    glfwSetScrollCallback(m_pMainWindow, CGlfwRenderWindow::onGLFWMouseScrollCallback);
    glfwSetCharCallback(m_pMainWindow, CGlfwRenderWindow::onGLFWCharCallback);
    glfwSetKeyCallback(m_pMainWindow, CGlfwRenderWindow::onGLFWKeyCallback);
    glfwSetWindowPosCallback(m_pMainWindow, CGlfwRenderWindow::onGLFWWindowPosCallback);
    glfwSetFramebufferSizeCallback(m_pMainWindow, CGlfwRenderWindow::onGLFWframebuffersize);
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
    TCHAR szBuffer[MAX_PATH];
    CStringHelper::GetInstance()->ConvertToTCHAR(errorDesc, szBuffer, MAX_PATH);
    BEATS_ASSERT(false, _T("An error was detected! ID : %d\ndesc : %s"), errorID, szBuffer);
}

void CGlfwRenderWindow::onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify)
{
    CGlfwRenderWindow *pObject = m_sInstanceMap[window];
    BEATS_ASSERT(pObject);

    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            pObject->m_bLeftMouseDown = true;
            intptr_t id = 0;
            size_t uCurrMSec = (size_t)(CTimeMeter::GetCurrUSec() / 1000);
            if( uCurrMSec - pObject->m_uLastTapTime <= TAP_INTERVAL &&
                labs(pObject->m_uCurMousePosX - pObject->m_uPressedMousePosX) <= TAP_DEVIATION &&
                labs(pObject->m_uCurMousePosY - pObject->m_uPressedMousePosY) <= TAP_DEVIATION )
            {
                ++pObject->m_uTapCount;
            }
            else
            {
                pObject->m_uTapCount = 1;
                pObject->m_uPressedMousePosX = pObject->m_uCurMousePosX;
                pObject->m_uPressedMousePosY = pObject->m_uCurMousePosY;
            }
            pObject->m_uLastTapTime = uCurrMSec;
            float x = (float)pObject->m_uCurMousePosX;
            float y = (float)pObject->m_uCurMousePosY;
            CTouchDelegate::GetInstance()->OnTouchBegan( 1, &id, &x, &y, &pObject->m_uTapCount);
        }
        else if(action == GLFW_RELEASE)
        {
            pObject->m_bLeftMouseDown = false;
            intptr_t id = 0;
            float x = (float)pObject->m_uCurMousePosX;
            float y = (float)pObject->m_uCurMousePosY;
            size_t uCurrMSec = (size_t)(CTimeMeter::GetCurrUSec() / 1000);
            if( uCurrMSec - pObject->m_uLastTapTime <= TAP_INTERVAL &&
                labs(pObject->m_uCurMousePosX - pObject->m_uPressedMousePosX) <= TAP_DEVIATION &&
                labs(pObject->m_uCurMousePosY - pObject->m_uPressedMousePosY) <= TAP_DEVIATION )
            {
                BEATS_ASSERT(pObject->m_uTapCount > 0);
                if(pObject->m_uTapCount == 1 || pObject->m_uTapCount == 2)
                    CTouchDelegate::GetInstance()->OnTapped( id, x, y, pObject->m_uTapCount);
            }
            else
            {
                pObject->m_uTapCount = 0;
            }
            pObject->m_uLastTapTime = uCurrMSec;
            CTouchDelegate::GetInstance()->OnTouchEnded( 1, &id, &x,&y, &pObject->m_uTapCount);
        }
    }
}

void CGlfwRenderWindow::onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
{
    CGlfwRenderWindow *pObject = m_sInstanceMap[window];
    BEATS_ASSERT(pObject);
    x /= pObject->GetScaleFactor();
    y /= pObject->GetScaleFactor();
    pObject->m_uCurMousePosX = (size_t)x;
    pObject->m_uCurMousePosY = (size_t)y;

    int width = 0, height = 0;
    glfwGetFramebufferSize(window,&width, &height);

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
                    float fYawValue = (float)iDeltaX / pObject->GetWidth();
                    pCamera->Yaw(fYawValue);

                    float fPitchValue = (float)iDeltaY / pObject->GetHeight();
                    pCamera->Pitch( fPitchValue);
                }
            }
#endif

            intptr_t id = 0;
            float rx = (float)x;
            float ry = (float)y;
            CTouchDelegate::GetInstance()->OnTouchMoved( 1, &id, &rx, &ry );
        }
    }

    pObject->m_uLastMousePosX = (size_t)x;
    pObject->m_uLastMousePosY = (size_t)y;
}

void CGlfwRenderWindow::onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
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
}

void CGlfwRenderWindow::onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    CGlfwRenderWindow *pObject = m_sInstanceMap[window];
    BEATS_ASSERT(pObject);

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        pObject->UpdateCamera();
        if (action == GLFW_PRESS)
        {
            switch (key)
            {
            case GLFW_KEY_F3:
                {
                    CRenderManager::GetInstance()->SwitchPolygonMode();
                }
                break;
            default:
                break;
            }
        }
    }

    EEventType eventType;
    switch(action)
    {
    case GLFW_PRESS:
        eventType = eET_EVENT_KEY_PRESSED;
        break;
    case GLFW_RELEASE:
        eventType = eET_EVENT_KEY_RELEASED;
        break;
    case GLFW_REPEAT:
        eventType = eET_EVENT_KEY_REPEATED;
        break;
    }
}

void CGlfwRenderWindow::onGLFWCharCallback(GLFWwindow* window, unsigned int character)
{
}

void CGlfwRenderWindow::onGLFWWindowPosCallback(GLFWwindow* windows, int x, int y)
{

}

void CGlfwRenderWindow::onGLFWframebuffersize(GLFWwindow* window, int w, int h)
{

}

void CGlfwRenderWindow::UpdateCamera()
{
    CScene *pCurrScene = CSceneManager::GetInstance()->GetCurrentScene();
    if(pCurrScene)
    {
        CCamera* pCamera = pCurrScene->GetCamera(CCamera::eCT_3D);
        if(pCamera)
        {
            kmVec3 vec3Speed;
            kmVec3Fill(&vec3Speed, 1.0f, 1.0f, 1.0f);
            if (glfwGetKey(m_pMainWindow,GLFW_KEY_LEFT_SHIFT) ||
                glfwGetKey(m_pMainWindow,GLFW_KEY_RIGHT_SHIFT) )
            {
                kmVec3Scale(&vec3Speed, &vec3Speed, 100.0F * 0.016F);
            }
            else
            {
                kmVec3Scale(&vec3Speed, &vec3Speed, 1.0F * 0.016F);
            }
            int type = CCamera::eCMT_NOMOVE;

            bool bPressA = glfwGetKey(m_pMainWindow,GLFW_KEY_A) != 0;
            bool bPressD = glfwGetKey(m_pMainWindow,GLFW_KEY_D) != 0;
            if ( bPressA || bPressD )
            {
                type |= CCamera::eCMT_TRANVERSE;
                if (bPressA)
                {
                    vec3Speed.x *= -1;
                }
            }
            bool bPressW = glfwGetKey(m_pMainWindow,GLFW_KEY_W) != 0;
            bool bPressS = glfwGetKey(m_pMainWindow,GLFW_KEY_S) != 0;

            if ( bPressW || bPressS )
            {
                type |= CCamera::eCMT_STRAIGHT;
                if (bPressW)
                {
                    vec3Speed.z *= -1;
                }
            }
            bool bPressUp = glfwGetKey(m_pMainWindow,GLFW_KEY_UP) != 0;
            bool bPressDown = glfwGetKey(m_pMainWindow,GLFW_KEY_DOWN) != 0;

            if ( bPressUp || bPressDown )
            {
                type |= CCamera::eCMT_UPDOWN;
                if (bPressDown)
                {
                    vec3Speed.y *= -1;
                }
            }

            if (type != CCamera::eCMT_NOMOVE)
            {
                pCamera->ExecuteMovement(vec3Speed, type);
            }
        }
    }
}