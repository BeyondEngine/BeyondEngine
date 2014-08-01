#include "stdafx.h"
#include "EditorSceneWindow.h"
#include "EditorMainFrame.h"
#include "Render\GridRenderObject.h"
#include "Render\Camera.h"
#include "Render\RenderGroupManager.h"
#include "Render\RenderManager.h"
#include "BeyondEngineEditorComponentWindow.h"
#include "WxGLRenderWindow.h"
#include "PerformDetector\PerformDetector.h"
#include "Scene\SceneManager.h"
#include "Framework\Application.h"
#include "EditPerformanceDialog.h"
#include "Scene\Scene.h"

BEGIN_EVENT_TABLE(CEditorSceneWindow, CBeyondEngineEditorGLWindow)
    EVT_SIZE(CEditorSceneWindow::OnSize)
END_EVENT_TABLE()


CEditorSceneWindow::CEditorSceneWindow(wxWindow *parent, wxGLContext* pContext, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : CBeyondEngineEditorGLWindow(parent, pContext, id, pos, size, style, name)
    , m_pNodeGrid(NULL)
    , m_pDefault3DCamera(NULL)
    , m_pDefault2DCamera(NULL)
{
    BEATS_ASSERT(CRenderGroupManager::GetInstance()->GetDefault3DCamera() == NULL);
    m_pNodeGrid = new CGridRenderObject;
    m_pNodeGrid->Activate();
    m_pNodeGrid->SetName(_T("GridRenderObject"));
    m_pNodeGrid->SetPosition(0.0f, -0.1f, 0.0f );
    m_pNodeGrid->SetLineColor(0x999999FF);
    m_pNodeGrid->Initialize();

    m_pDefault3DCamera = new CCamera(CCamera::eCT_3D);
    m_pDefault3DCamera->SetNear(0.01f);
    m_pDefault3DCamera->SetFar(1000.0f);
    m_pDefault3DCamera->SetFOV(25.0f);
    m_pDefault3DCamera->SetViewPos(0,100,0);
    m_pDefault3DCamera->SetRotation(-90, 0, 0);
    m_pDefault2DCamera = new CCamera(CCamera::eCT_2D);
}

CEditorSceneWindow::~CEditorSceneWindow()
{
    m_pNodeGrid->Uninitialize();
    BEATS_SAFE_DELETE(m_pNodeGrid);
    BEATS_SAFE_DELETE(m_pDefault3DCamera);
    BEATS_SAFE_DELETE(m_pDefault2DCamera);
    CRenderGroupManager::GetInstance()->SetDefault3DCamera(NULL);
    CRenderGroupManager::GetInstance()->SetDefault2DCamera(NULL);
}

CGridRenderObject* CEditorSceneWindow::GetGrid() const
{
    return m_pNodeGrid;
}

void CEditorSceneWindow::Update()
{
    if (IsShownOnScreen())
    {
        BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderSceneWindow)

        BEYONDENGINE_PERFORMDETECT_START(ePNT_EngineCenterUpdate)
        CApplication::GetInstance()->Update();

        CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
        CScene* pCurScene = GetScene();
        CSceneManager::GetInstance()->SetCurrentScene(pCurScene);
        CRenderGroupManager::GetInstance()->SetDefault3DCamera(pCurScene == NULL ? m_pDefault3DCamera : pCurScene->GetCamera(CCamera::eCT_3D));
        CRenderGroupManager::GetInstance()->SetDefault2DCamera(pCurScene == NULL ? m_pDefault2DCamera : pCurScene->GetCamera(CCamera::eCT_2D));
        BEYONDENGINE_PERFORMDETECT_START(ePNT_UpdateCamera)
        UpdateCamera();
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_UpdateCamera)

        m_pNodeGrid->Render();
        CApplication::GetInstance()->Render();

        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_EngineCenterUpdate)

        BEYONDENGINE_PERFORMDETECT_START(ePNT_PerformDetector)
        if (m_pMainFrame->GetPerformanceDialogPtr())
        {
            m_pMainFrame->GetPerformanceDialogPtr()->UpdatePerformData();
        }
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_PerformDetector)

        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderSceneWindow)
    }
}

CCamera* CEditorSceneWindow::GetCamera()
{
    CScene *pScene = GetScene();
    return pScene ? pScene->GetCamera(CCamera::eCT_3D) : m_pDefault3DCamera;
}

void CEditorSceneWindow::SetContextToCurrent()
{
    if (IsShownOnScreen())
    {
        CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
    }
}

void CEditorSceneWindow::OnSize(wxSizeEvent& event)
{
    wxSize size = event.GetSize();
    size_t uWidth = static_cast<size_t>(size.GetWidth());
    size_t uHeight = static_cast<size_t>(size.GetHeight());
    if ( IsShownOnScreen() )
    {
        CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
    }
    if (m_pMainFrame->GetAuiToolBarPerformPtr()->GetToolToggled(ID_ViewAllBtn))
    {
        size_t uSimulateWidth, uSimulateHeight;
        CRenderManager::GetInstance()->GetSimulateSize(uSimulateWidth, uSimulateHeight);
        if (uWidth > uSimulateWidth)
        {
            uWidth = uSimulateWidth;
        }
        if (uHeight > uSimulateHeight)
        {
            uHeight = uSimulateHeight;
        }
        if (uWidth * uSimulateHeight > uHeight * uSimulateWidth)
        {
            uWidth = uHeight * uSimulateWidth / uSimulateHeight;
        }
        else
        {
            uHeight = uWidth * uSimulateHeight / uSimulateWidth;
        }
        m_pRenderWindow->SetFBOViewPort(uWidth, uHeight);
        m_pRenderWindow->SetDeviceResolution(uSimulateWidth, uSimulateHeight);
        m_pMainFrame->GetSplitter()->SetSashPosition(uHeight, true);
        m_pMainFrame->GetComponentWindow()->SendSizeEvent();
    }
    else
    {
        m_pRenderWindow->SetDeviceResolution(uWidth, uHeight);
    }
}
