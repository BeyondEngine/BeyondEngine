#include "stdafx.h"
#include "BeyondEngineEditorGLWindow.h"
#include "Render/RenderManager.h"
#include "Render/Renderer.h"
#include "Render/Camera.h"
#include "Utility/PerformDetector/PerformDetector.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "EditPerformanceDialog.h"
#include "WxGLRenderWindow.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Node3D.h"
#include "Render/GridRenderObject.h"
#include "Render/Viewport.h"
#include "Framework/Application.h"
#include "EditCameraWnd.h"
#include "BeyondEngineEditorComponentWindow.h"
#include "Render/RenderGroupManager.h"
#include "ViewAgentBase.h"

#pragma comment(lib,"Imm32.lib")

#define MaxSpeedUpScale 30.0f
#define SpeedUpRate 0.3f

BEGIN_EVENT_TABLE(CBeyondEngineEditorGLWindow, wxGLCanvas)
    EVT_SIZE(CBeyondEngineEditorGLWindow::OnSize)
    EVT_CHAR(CBeyondEngineEditorGLWindow::OnChar)
    EVT_KEY_DOWN(CBeyondEngineEditorGLWindow::OnKey)
    EVT_MOUSE_EVENTS(CBeyondEngineEditorGLWindow::OnMouse)
    EVT_SET_CURSOR(CBeyondEngineEditorGLWindow::OnSetCursor)
    EVT_MOUSE_CAPTURE_LOST(CBeyondEngineEditorGLWindow::OnCaptureLost)
END_EVENT_TABLE()

const int CBeyondEngineEditorGLWindow::m_attribList[] = {
    WX_GL_RGBA,
    WX_GL_DOUBLEBUFFER,
    WX_GL_STENCIL_SIZE,
    32,
    0,
};

CBeyondEngineEditorGLWindow::CBeyondEngineEditorGLWindow(wxWindow *parent,
                                     wxGLContext* pContext, wxWindowID id,
                                     const wxPoint& pos, const wxSize& size,
                                     long style, const wxString& name)
    : wxGLCanvas(parent, id, m_attribList, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
    , m_fSpeedUpScale(1.0F)
{
    m_pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    m_pRenderWindow = new wxGLRenderWindow(this);
    m_pRenderWindow->SetContext(pContext);
}

CBeyondEngineEditorGLWindow::~CBeyondEngineEditorGLWindow()
{
    if(CRenderManager::GetInstance()->GetCurrentRenderTarget() == m_pRenderWindow)
    {
        CRenderManager::GetInstance()->SetCurrentRenderTarget(nullptr);
    }
    BEATS_SAFE_DELETE(m_pRenderWindow);
}

wxGLContext* CBeyondEngineEditorGLWindow::GetGLContext() const
{
    return m_pRenderWindow->GetContext();
}

void CBeyondEngineEditorGLWindow::OnSize(wxSizeEvent& event)
{
    wxSize size = event.GetSize();
    uint32_t uWidth = static_cast<uint32_t>(size.GetWidth());
    uint32_t uHeight = static_cast<uint32_t>(size.GetHeight());
    m_pRenderWindow->SetDeviceSize(uWidth, uHeight);
    if (IsShownOnScreen())
    {
        CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
    }
}

void CBeyondEngineEditorGLWindow::SetContextToCurrent()
{
    if ( IsShownOnScreen() )
    {
        CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
    }
}

wxGLRenderWindow *CBeyondEngineEditorGLWindow::GetRenderWindow() const
{
    return m_pRenderWindow;
}

void CBeyondEngineEditorGLWindow::OnMouse(wxMouseEvent& event)
{
    m_pMainFrame->OnMouseInCurrentView(event);
}

void CBeyondEngineEditorGLWindow::SetGLContext( wxGLContext* pContext )
{
    m_pRenderWindow->SetContext(pContext);
}

void CBeyondEngineEditorGLWindow::ShowCursor()
{
    int iCount = ::ShowCursor(true);
    while (iCount < 0)
    {
        iCount = ::ShowCursor(true);
    }
}

void CBeyondEngineEditorGLWindow::HideCursor()
{
    int iCount = ::ShowCursor(false);
    while (iCount >= 0)
    {
        iCount = ::ShowCursor(false);
    }
}

bool CBeyondEngineEditorGLWindow::SafeGetKeyStage( int nVirtKey )
{
    bool bRet = false;
    if (HasFocus())
    {
        bRet = (::GetKeyState(nVirtKey) & 0x8000) > 0;
    }
    return bRet;
}

void CBeyondEngineEditorGLWindow::OnSetCursor( wxSetCursorEvent& event )
{
    event.Skip();//call skip to avoid the default SetCursor or the cursor will be set as wxSTANDARD_CURSOR
    ((CEngineEditor*)wxApp::GetInstance())->GetMainFrame()->SetCursor();
}

void CBeyondEngineEditorGLWindow::OnCaptureLost(wxMouseCaptureLostEvent& /*event*/)
{
    if (HasCapture())
    {
        ReleaseCapture();
    }
}

void CBeyondEngineEditorGLWindow::OnChar(wxKeyEvent& event)
{
    wxUint32 wParam = event.GetRawKeyCode();
    if (wParam == VK_F1)
    {
#ifdef DEVELOP_VERSION
        CLogManager::GetInstance()->Show(!CLogManager::GetInstance()->IsShown());
#endif
    }
    if (wParam < 0x20)
    {
        if (VK_BACK == wParam)
        {
        }
        else if (VK_RETURN == wParam)
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
#ifdef UNICODE
        TCHAR szUtf8[8] = { 0 };
        int nLen = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)&wParam, 1, szUtf8, sizeof(szUtf8), NULL, NULL);
        CIMEManager::GetInstance()->OnCharEvent(szUtf8, nLen);
#else
        static bool bFirstChar = true;
        static char iFirsz = 0;
        if (bFirstChar)
        {
            iFirsz = wParam;
            bFirstChar = false;
        }
        else
        {
            char szChar[2];
            szChar[0] = iFirsz;
            szChar[1] = wParam;
            int n = MultiByteToWideChar(CP_ACP, 0, szChar, 2, NULL, 0);
            wchar_t* szWchar = new wchar_t[n];
            MultiByteToWideChar(CP_ACP, 0, szChar, 2, szWchar, n);
            TCHAR szUtf8[8] = { 0 };
            WideCharToMultiByte(CP_UTF8, 0, szWchar, 1, szUtf8, sizeof(szUtf8), NULL, NULL);
            iFirsz = 0;
            bFirstChar = true;
            delete[] szWchar;
        }
#endif
    }
}

void CBeyondEngineEditorGLWindow::OnKey(wxKeyEvent& event)
{
    ((CEngineEditor*)wxApp::GetInstance())->GetMainFrame()->OnKeyInCurrentView(event);
    event.Skip();
}
