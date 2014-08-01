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
#include "GUI/WindowManager.h"

#define MaxSpeedUpScale 30.0f
#define SpeedUpRate 0.3f

BEGIN_EVENT_TABLE(CBeyondEngineEditorGLWindow, wxGLCanvas)
    EVT_SIZE(CBeyondEngineEditorGLWindow::OnSize)
    EVT_CHAR(CBeyondEngineEditorGLWindow::OnChar)
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
    , m_fCameraSpeedScale(10.0F)
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
    size_t uWidth = static_cast<size_t>(size.GetWidth());
    size_t uHeight = static_cast<size_t>(size.GetHeight());
    m_pRenderWindow->SetDeviceResolution(uWidth, uHeight);
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
    ((CEngineEditor*)wxApp::GetInstance())->GetMainFrame()->OnMouseInCurrentView(event);
    if (event.GetSkipped())
    {
        if(event.ButtonDown(wxMOUSE_BTN_RIGHT))
        {
            if (!HasCapture())
            {
                CaptureMouse();
            }
            HideCursor();
            SetFocus();//Call this for catch the EVT_MOUSEWHEEL event, in left mouse button down event is not necessary to call this
            m_startPosition = event.GetPosition();
        }
        else if(event.ButtonUp(wxMOUSE_BTN_RIGHT))
        {
            if (!event.LeftIsDown() && HasCapture())
            {
                ReleaseMouse();
                ShowCursor();
            }
        }
        else if(event.ButtonDown(wxMOUSE_BTN_LEFT))
        {
            if (!HasCapture())
            {
                CaptureMouse();
            }
            HideCursor();
            m_startPosition = event.GetPosition();
        }
        else if(event.ButtonUp(wxMOUSE_BTN_LEFT))
        {
            if (!event.RightIsDown() && HasCapture())
            {
                ReleaseMouse();
                ShowCursor();
            }
        }
        else if(event.Dragging())
        {
            wxPoint curPos = event.GetPosition();
            wxPoint pnt = ClientToScreen(m_startPosition);
            SetCursorPos(pnt.x, pnt.y);
            if (event.RightIsDown())
            {
                float fDeltaX = m_startPosition.x - curPos.x;
                float fDeltaY = m_startPosition.y - curPos.y;
                wxSize clientSize = GetClientSize();
                fDeltaX /= clientSize.x;
                fDeltaY /= clientSize.y;
                float fScale = m_pRenderWindow->GetScaleFactor();
                fDeltaX /= fScale;
                fDeltaY /= fScale;
                CCamera *pCamera = GetCamera();
                if(pCamera)
                {
                    pCamera->Yaw(fDeltaX);
                    pCamera->Pitch(fDeltaY);
                }
            }
        }
        if (event.GetWheelRotation() != 0)
        {
            CCamera *pCamera = GetCamera();
            if(pCamera)
            {
                kmVec3 vec3Speed;
                kmVec3Fill(&vec3Speed, SHIFTWHEELSPEED, SHIFTWHEELSPEED, SHIFTWHEELSPEED);
                kmVec3Scale(&vec3Speed, &vec3Speed, event.GetWheelRotation() > 0 ? 1.0f : -1.0f);
                pCamera->ExecuteMovement(vec3Speed, (1 << CCamera::eCMT_STRAIGHT));
            }
        }
    }
    event.Skip();
}

void CBeyondEngineEditorGLWindow::UpdateCamera()
{
    kmVec3 vec3Speed;
    kmVec3Fill(&vec3Speed, 1.0f, 1.0f, 1.0f);
    if ( SafeGetKeyStage(VK_SHIFT) )
    {
        kmVec3Scale(&vec3Speed, &vec3Speed, 5.0F * 0.016F * m_fCameraSpeedScale * m_fSpeedUpScale);
    }
    else
    {
        kmVec3Scale(&vec3Speed, &vec3Speed, 1.0F * 0.016F * m_fCameraSpeedScale * m_fSpeedUpScale);
    }
    int type = CCamera::eCMT_NOMOVE;
    bool bShouldSpeedUp = false;
    bool bPressA = SafeGetKeyStage('A');
    bool bPressD = SafeGetKeyStage('D');
    if ( bPressA || bPressD )
    {
        type |= CCamera::eCMT_TRANVERSE;
        if (bPressA)
        {
            vec3Speed.x *= -1;
        }
        bShouldSpeedUp = true;
    }
    bool bPressW = SafeGetKeyStage('W');
    bool bPressS = SafeGetKeyStage('S');

    if ( bPressW || bPressS )
    {
        type |= CCamera::eCMT_STRAIGHT;
        if (bPressW)
        {
            vec3Speed.z *= -1;
        }
        bShouldSpeedUp = true;
    }
    bool bPressUp = SafeGetKeyStage('E');
    bool bPressDown = SafeGetKeyStage('Q');

    if ( bPressUp || bPressDown )
    {
        type |= CCamera::eCMT_UPDOWN;
        if (bPressDown)
        {
            vec3Speed.y *= -1;
        }
        bShouldSpeedUp = true;
    }

    if (bShouldSpeedUp )
    {
        if (m_fSpeedUpScale < MaxSpeedUpScale)
        {
            m_fSpeedUpScale += SpeedUpRate;
        }
    }
    else
    {
        m_fSpeedUpScale = 1.0f;
    }
    if (type != CCamera::eCMT_NOMOVE)
    {
        CCamera *pCamera = GetCamera();
        if(pCamera)
        {
            BEATS_ASSERT(pCamera->GetType() == CCamera::eCT_3D);
            pCamera->ExecuteMovement(vec3Speed, type);
        }
    }
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

float CBeyondEngineEditorGLWindow::GetCameraSpeedScale() const
{
    return m_fCameraSpeedScale;
}

void CBeyondEngineEditorGLWindow::SetCameraSpeedScale( float fScale )
{
    m_fCameraSpeedScale = fScale;
}

CScene *CBeyondEngineEditorGLWindow::GetScene()
{
    CComponentProxy* pSceneProxy = NULL;
    const std::map<size_t, CComponentProxy*>& components = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
    for (auto iter = components.begin(); iter != components.end(); ++iter)
    {
        if (iter->second->GetGuid() == CScene::REFLECT_GUID || iter->second->GetParentGuid() == CScene::REFLECT_GUID)
        {
            pSceneProxy = iter->second;
            break;
        }
    }
    CScene* pScene = NULL;
    if (pSceneProxy != NULL)
    {
        pScene = down_cast<CScene*>(pSceneProxy->GetHostComponent());
        BEATS_ASSERT(pScene != NULL);
    }
    else
    {
        const std::map<size_t, std::map<size_t, CComponentBase*>*>* pInstanceMap = CComponentProxyManager::GetInstance()->GetComponentInstanceMap();
        bool bFindScene = false;
        auto iterScene = pInstanceMap->find(CScene::REFLECT_GUID);
        if (iterScene != pInstanceMap->end())
        {
            BEATS_ASSERT(iterScene->second->size() < 2);
            CComponentProxy* pProxy = down_cast<CComponentProxy*>(iterScene->second->begin()->second);
            pScene = down_cast<CScene*>(pProxy->GetHostComponent());
            BEATS_ASSERT(pScene != NULL);
            bFindScene = true;
        }
        if (!bFindScene)
        {
            std::vector<size_t> vecDerivedClass;
            CComponentProxyManager::GetInstance()->QueryDerivedClass(CScene::REFLECT_GUID, vecDerivedClass, true);
            for (size_t i = 0; i < vecDerivedClass.size(); ++i)
            {
                auto iterSceneDerived = pInstanceMap->find(vecDerivedClass[i]);
                if (iterSceneDerived != pInstanceMap->end())
                {
                    BEATS_ASSERT(iterSceneDerived->second->size() < 2);
                    CComponentProxy* pProxy = down_cast<CComponentProxy*>(iterSceneDerived->second->begin()->second);
                    pScene = down_cast<CScene*>(pProxy->GetHostComponent());
                    BEATS_ASSERT(pScene != NULL);
                    break;
                }
            }
        }
    }
    // TODO: If this assert never triggered, try to use GetCurrentScene instead.
    //BEATS_ASSERT(pScene == CSceneManager::GetInstance()->GetCurrentScene());
    return pScene;
}

void CBeyondEngineEditorGLWindow::OnCaptureLost(wxMouseCaptureLostEvent& /*event*/)
{
    if (HasCapture())
    {
        ReleaseCapture();
    }
}

void CBeyondEngineEditorGLWindow::OnChar( wxKeyEvent& event )
{
    wxChar c = (wxChar)event.GetKeyCode();
    CWindowManager::GetInstance()->OnCharEvent( (const TCHAR*)&c, 1 );
}
