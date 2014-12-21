#include "stdafx.h"
#include "EditorSceneWindow.h"
#include "EditorMainFrame.h"
#include "Render/GridRenderObject.h"
#include "Render/Camera.h"
#include "Render/RenderGroupManager.h"
#include "Render/RenderManager.h"
#include "BeyondEngineEditorComponentWindow.h"
#include "WxGLRenderWindow.h"
#include "PerformDetector/PerformDetector.h"
#include "Scene/SceneManager.h"
#include "Framework/Application.h"
#include "EditPerformanceDialog.h"
#include "Scene/Scene.h"
#include "ViewAgentBase.h"
#include "ParticleSystem/ParticleManager.h"

BEGIN_EVENT_TABLE(CEditorSceneWindow, CBeyondEngineEditorGLWindow)
    EVT_SIZE(CEditorSceneWindow::OnSize)
END_EVENT_TABLE()

CEditorSceneWindow::CEditorSceneWindow(wxWindow *parent, wxGLContext* pContext, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : CBeyondEngineEditorGLWindow(parent, pContext, id, pos, size, style, name)
    , m_pNodeGrid(NULL)
    , m_pDefault3DCamera(NULL)
    , m_pDefault2DCamera(NULL)
    , m_pWxView(nullptr)
{
    m_pNodeGrid = new CGridRenderObject;
    m_pNodeGrid->Activate();
    m_pNodeGrid->SetName(_T("GridRenderObject"));
    m_pNodeGrid->SetPosition(CVec3(0.0f, -0.1f, 0.0f));
    m_pNodeGrid->SetLineColor(0x999999FF);
    m_pNodeGrid->Initialize();

    m_pDefault3DCamera = new CCamera(CCamera::eCT_3D);
    m_pDefault3DCamera->SetNear(0.01f);
    m_pDefault3DCamera->SetFar(1000.0f);
    m_pDefault3DCamera->SetFOV(25.0f);
    m_pDefault3DCamera->SetViewPos(CVec3(0,100,0));
    m_pDefault3DCamera->SetRotation(CVec3(-90, 0, 0));
    m_pDefault2DCamera = new CCamera(CCamera::eCT_2D);
}

CEditorSceneWindow::~CEditorSceneWindow()
{
    m_pNodeGrid->Uninitialize();
    BEATS_SAFE_DELETE(m_pNodeGrid);
    BEATS_SAFE_DELETE(m_pDefault3DCamera);
    BEATS_SAFE_DELETE(m_pDefault2DCamera);
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
        CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
        float dt = CApplication::GetInstance()->Update();
        BEATS_ASSERT(m_pWxView);
        m_pWxView->Update(dt);
        CScene* pCurScene = CSceneManager::GetInstance()->GetCurrentScene();
        if (pCurScene == nullptr)
        {
            // If pCurScene is nullptr, it means:
            // 1. We load no scene, we use editor's camera.
            // 2. We're switching scene async, don't do anything else.
            if (!CSceneManager::GetInstance()->GetSwitchSceneState())
            {
                CRenderManager::GetInstance()->GetCamera(CCamera::eCT_2D)->SetCameraData(m_pDefault2DCamera->GetCameraData());
                CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D)->SetCameraData(m_pDefault3DCamera->GetCameraData());
            }
        }
        else
        {
            CRenderManager::GetInstance()->GetCamera(CCamera::eCT_2D)->SetCameraData(pCurScene->GetCamera(CCamera::eCT_2D)->GetCameraData());
            CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D)->SetCameraData(pCurScene->GetCamera(CCamera::eCT_3D)->GetCameraData());
        }

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

        if (!CEngineCenter::GetInstance()->m_bDelaySync)
        {
            for (auto property : CEngineCenter::GetInstance()->m_editorPropertyGridSyncList)
            {
                m_pMainFrame->GetPropGridManager()->RefreshPropertyInGrid(property);
            }
            CEngineCenter::GetInstance()->m_editorPropertyGridSyncList.clear();
        }
#ifdef DEVELOP_VERSION
        for (auto iter = CParticleManager::GetInstance()->m_particleDetailMap.begin(); iter != CParticleManager::GetInstance()->m_particleDetailMap.end(); ++iter)
        {
            CParticleManager::GetInstance()->m_particleDataPool.push_back(iter->second);
        }
        CParticleManager::GetInstance()->m_particleDetailMap.clear();
#endif
    }
}

CCamera* CEditorSceneWindow::GetCamera()
{
    CScene *pScene = CSceneManager::GetInstance()->GetCurrentScene();
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
    uint32_t uWidth = static_cast<uint32_t>(size.GetWidth());
    uint32_t uHeight = static_cast<uint32_t>(size.GetHeight());
    if ( IsShownOnScreen() )
    {
        CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
    }
    if (m_pMainFrame->GetAuiToolBarPerformPtr()->GetToolToggled(ID_ViewAllBtn))
    {
        uint32_t uSimulateWidth, uSimulateHeight;
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
        m_pRenderWindow->SetDeviceSize(uSimulateWidth, uSimulateHeight);
        m_pMainFrame->GetSplitter()->SetSashPosition(uHeight, true);
        m_pMainFrame->GetComponentWindow()->SendSizeEvent();
    }
    else
    {
        m_pRenderWindow->SetDeviceSize(uWidth, uHeight);
    }
}

void CEditorSceneWindow::SetViewAgent(CViewAgentBase* pWxView)
{
    m_pWxView = pWxView;
}
