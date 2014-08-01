#include "stdafx.h"
#include "AnimationGLWindow.h"
#include "Render/Model.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "Render/AnimationController.h"
#include "EditAnimationDialog.h"
#include "timebarframe.h"
#include "Render/GridRenderObject.h"
#include "Render/Camera.h"
#include "Render/RenderGroupManager.h"
#include "WxGLRenderWindow.h"
#include "Render/Viewport.h"
#include "Render/RenderManager.h"

CAnimationGLWindow::CAnimationGLWindow(wxWindow *parent, wxGLContext* pContext, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : CBeyondEngineEditorGLWindow(parent, pContext, id, pos, size, style, name)
    , m_pModel(NULL)
    , m_pCamera(NULL)
    , m_pGrid(NULL)
{
    m_pGrid = new CGridRenderObject;
    m_pGrid->Activate();
    m_pGrid->SetName(_T("AnimationGrid"));
    m_pGrid->SetPosition(0.0f, -0.1f, 0.0f );
    m_pGrid->SetLineColor(0x999999FF);
    m_pGrid->SetGridWidth( 1000 );
    m_pGrid->SetGridHeight( 1000 );
    m_pGrid->SetAxisZColor(0xFFFF00FF);
    m_pGrid->SetGridStartPos(-500, -500);
    m_pGrid->SetGridDistance(1.0f);
    m_pGrid->Initialize();

    m_pCamera = new CCamera(CCamera::eCT_3D);
    m_pCamera->SetNear(0.01f);
    m_pCamera->SetFar(100.0f);
    m_pCamera->SetFOV(51.8f);
    m_pCamera->SetViewPos(0,3,10);
}

CAnimationGLWindow::~CAnimationGLWindow()
{
    m_pGrid->Uninitialize();
    BEATS_SAFE_DELETE(m_pGrid);
    BEATS_SAFE_DELETE(m_pCamera);
    CRenderGroupManager::GetInstance()->SetDefault3DCamera(NULL);
}

void CAnimationGLWindow::Update()
{
    if (IsShownOnScreen())
    {
        CRenderGroupManager::GetInstance()->SetDefault3DCamera(m_pCamera);
        UpdateCamera();
        BEATS_ASSERT(m_pRenderWindow->GetViewport() != NULL);
        CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
        CViewport* pCurViewport = m_pRenderWindow->GetViewport();
        pCurViewport->Apply();
        m_pGrid->Render();
        CEditAnimationDialog* pDialog = (CEditAnimationDialog*)GetParent();
        if (m_pModel != NULL)
        {
            m_pModel->Update(0.016f);
            if (m_pModel->GetAnimationController()->IsPlaying())
            {
                int iCur = m_pModel->GetAnimationController()->GetCurrFrame();
                pDialog->GetTimeBar()->SetCurrentCursor(iCur);
            }
            m_pModel->Render();
        }
        CRenderManager::GetInstance()->Render();
        m_pRenderWindow->GetCanvas()->SwapBuffers();
#ifdef _DEBUG
        CEngineCenter::GetInstance()->ResetDrawCall();
#endif
    }
}

void CAnimationGLWindow::SetModel( CModel* pModel )
{
    m_pModel = pModel;
}

CModel* CAnimationGLWindow::GetModel()
{
    return m_pModel;
}

CCamera* CAnimationGLWindow::GetCamera()
{
    return m_pCamera;
}
