#include "stdafx.h"
#include "EditCameraWnd.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "BeyondEngineEditorGLWindow.h"
#include "Render/Camera.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "EditorSceneWindow.h"

CEditCameraWnd::CEditCameraWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : super(parent, id, title, pos, size, style, name)
    , m_pPropertyGrid(NULL)
    , m_pPosX(NULL)
    , m_pPosY(NULL)
    , m_pPosZ(NULL)
    , m_pRotationX(NULL)
    , m_pRotationY(NULL)
    , m_pRotationZ(NULL)
    , m_pSpeed(NULL)
    , m_pFov(NULL)
{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);
    m_pPropertyGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxSize(400,400),
                                            wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED );

    wxPGProperty* topId = m_pPropertyGrid->Append( new wxStringProperty(wxT("Pos"), wxPG_LABEL, wxT("<composed>")) );
    m_pPosX = m_pPropertyGrid->AppendIn( topId, new wxFloatProperty(wxT("x"), wxT("PosX")) );
    AddSpinCtrl(m_pPosX);
    m_pPosY = m_pPropertyGrid->AppendIn( topId, new wxFloatProperty(wxT("y"), wxT("PosY")) );
    AddSpinCtrl(m_pPosY);
    m_pPosZ = m_pPropertyGrid->AppendIn( topId, new wxFloatProperty(wxT("z"), wxT("PosZ")) );
    AddSpinCtrl(m_pPosZ);
    topId = m_pPropertyGrid->Append( new wxStringProperty(wxT("Rotate"), wxPG_LABEL, wxT("<composed>")) );
    m_pRotationX = m_pPropertyGrid->AppendIn( topId, new wxFloatProperty(wxT("x"), wxT("LookAtX")) );
    AddSpinCtrl(m_pRotationX);
    m_pRotationY = m_pPropertyGrid->AppendIn( topId, new wxFloatProperty(wxT("y"), wxT("LookAtY")) );
    AddSpinCtrl(m_pRotationY);
    m_pRotationZ = m_pPropertyGrid->AppendIn( topId, new wxFloatProperty(wxT("z"), wxT("LookAtZ")) );
    AddSpinCtrl(m_pRotationZ);

    topId = m_pPropertyGrid->Append( new wxStringProperty(wxT("Clip"), wxPG_LABEL, wxT("<composed>")) );
    m_pClipNear = m_pPropertyGrid->AppendIn( topId, new wxFloatProperty(wxT("Near"), wxT("Near")) );
    AddSpinCtrl(m_pClipNear);
    m_pClipFar = m_pPropertyGrid->AppendIn( topId, new wxFloatProperty(wxT("Far"), wxT("Far")) );
    AddSpinCtrl(m_pClipFar);

    m_pSpeed = m_pPropertyGrid->Append( new wxFloatProperty(wxT("Speed"), wxT("Speed"), 0.0f) );
    AddSpinCtrl(m_pSpeed);
    m_pFov = m_pPropertyGrid->Append( new wxFloatProperty(wxT("Fov"), wxT("Fov"), 0.0f) );
    AddSpinCtrl(m_pFov);

    pMainSizer->Add(m_pPropertyGrid, 1, wxALL, 5);

    wxBoxSizer* pBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pSaveAsSceneInitBtn = new wxButton(this, wxID_ANY, _T("设为起始参数"));
    pBtnSizer->Add(m_pSaveAsSceneInitBtn, 0, wxALL, 5);
    m_pLoadSceneInitBtn = new wxButton(this, wxID_ANY, _T("恢复初始状态"));
    pBtnSizer->Add(m_pLoadSceneInitBtn, 0, wxALL, 5);
    pMainSizer->Add(pBtnSizer, 0, wxALL, 0);

    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CEditCameraWnd::OnClose), NULL, this);
    m_pPropertyGrid->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CEditCameraWnd::OnPropertyChanged), NULL, this);
    m_pSaveAsSceneInitBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CEditCameraWnd::OnSaveAsSceneInitClicked), NULL, this);
    m_pLoadSceneInitBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CEditCameraWnd::OnLoadSceneInitClicked), NULL, this);
    
}

CEditCameraWnd::~CEditCameraWnd()
{
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CEditCameraWnd::OnClose), NULL, this);
    m_pPropertyGrid->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CEditCameraWnd::OnPropertyChanged), NULL, this);
    m_pSaveAsSceneInitBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CEditCameraWnd::OnSaveAsSceneInitClicked), NULL, this);
    m_pLoadSceneInitBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CEditCameraWnd::OnLoadSceneInitClicked), NULL, this);
}

#define CHANGE_UN_EDITING_PROPERTY(property, value)\
    if(!BEATS_FLOAT_EQUAL(property->GetValue().GetDouble(), value))\
    {\
        property->SetValue(value);\
    }

void CEditCameraWnd::UpdateInfo(bool bForceUpdate)
{
    if (this->IsShown() || bForceUpdate)
    {
        CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
        CEditorSceneWindow* pSceneWnd = pMainFrame->GetSceneWindow();
        CCamera* pCurCamera = pSceneWnd->GetCamera();
        if (pCurCamera != NULL)
        {
            // When we run to here, the engine's frame counter has already increased, so we need to add 1.
            bool bCameraChangedThisFrame = pCurCamera->GetLastUpdateFrameCounter() + 1 == CEngineCenter::GetInstance()->GetFrameCounter();
            if (bCameraChangedThisFrame || bForceUpdate)
            {
                const kmVec3& viewPos = pCurCamera->GetViewPos();
                const kmVec3& rotation = pCurCamera->GetRotation();
                float fFOV = pCurCamera->GetFOV();
                float fSpeed = pSceneWnd->GetCameraSpeedScale();
                float fNear = pCurCamera->GetNear();
                float fFar = pCurCamera->GetFar();
                CHANGE_UN_EDITING_PROPERTY(m_pPosX, viewPos.x);
                CHANGE_UN_EDITING_PROPERTY(m_pPosY, viewPos.y);
                CHANGE_UN_EDITING_PROPERTY(m_pPosZ, viewPos.z);

                CHANGE_UN_EDITING_PROPERTY(m_pRotationX, rotation.x);
                CHANGE_UN_EDITING_PROPERTY(m_pRotationY, rotation.y);
                CHANGE_UN_EDITING_PROPERTY(m_pRotationZ, rotation.z);

                CHANGE_UN_EDITING_PROPERTY(m_pClipNear, fNear);
                CHANGE_UN_EDITING_PROPERTY(m_pClipFar, fFar);
                CHANGE_UN_EDITING_PROPERTY(m_pSpeed, fSpeed);
                CHANGE_UN_EDITING_PROPERTY(m_pFov, fFOV);
            }
        }
    }
}

bool CEditCameraWnd::Show(bool bShow)
{
    if (bShow)
    {
        UpdateInfo(true);
    }
    return super::Show(bShow);
}

void CEditCameraWnd::OnClose( wxCloseEvent& /*event*/ )
{
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    pMainFrame->GetAuiToolBarPerformPtr()->ToggleTool(ID_CameraBtn, false);
    pMainFrame->GetAuiToolBarPerformPtr()->Refresh(false);
    this->Show(false);
}

void CEditCameraWnd::OnPropertyChanged(wxPropertyGridEvent &event)
{
    wxPGProperty* pChangedProperty = event.GetProperty();
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    CBeyondEngineEditorGLWindow* pSceneWnd = pMainFrame->GetSceneWindow();
    CCamera* pCurCamera = pSceneWnd->GetCamera();

    if (pCurCamera != NULL)
    {
        if (pChangedProperty == m_pSpeed)
        {
            pSceneWnd->SetCameraSpeedScale((float)m_pSpeed->GetValue().GetDouble());
        }
        else if (pChangedProperty == m_pPosX || pChangedProperty == m_pPosY || pChangedProperty == m_pPosZ)
        {
            pCurCamera->SetViewPos((float)m_pPosX->GetValue().GetDouble(),
                                    (float)m_pPosY->GetValue().GetDouble(),
                                    (float)m_pPosZ->GetValue().GetDouble());
        }
        else if (pChangedProperty == m_pRotationX || pChangedProperty == m_pRotationY || pChangedProperty == m_pRotationZ)
        {
            pCurCamera->SetRotation((float)m_pRotationX->GetValue().GetDouble(),
                                   (float)m_pRotationY->GetValue().GetDouble(),
                                    (float)m_pRotationZ->GetValue().GetDouble());
        }
        else if (pChangedProperty == m_pFov)
        {
            pCurCamera->SetFOV((float)m_pFov->GetValue().GetDouble());
        }
        else if (pChangedProperty == m_pClipNear)
        {
            pCurCamera->SetNear((float)m_pClipNear->GetValue().GetDouble());
        }
        else if (pChangedProperty == m_pClipFar)
        {
            pCurCamera->SetFar((float)m_pClipFar->GetValue().GetDouble());
        }
    }
}

void CEditCameraWnd::OnSaveAsSceneInitClicked(wxCommandEvent& /*event*/)
{
    CScene* pCurScene = CSceneManager::GetInstance()->GetCurrentScene();
    if (pCurScene != NULL)
    {
        pCurScene->SetInitCameraPos((float)m_pPosX->GetValue().GetDouble(),
                                    (float)m_pPosY->GetValue().GetDouble(),
                                    (float)m_pPosZ->GetValue().GetDouble());
        pCurScene->SetInitCameraRotation((float)m_pRotationX->GetValue().GetDouble(),
                                        (float)m_pRotationY->GetValue().GetDouble(),
                                        (float)m_pRotationZ->GetValue().GetDouble());
        pCurScene->SetInitCameraNear((float)m_pClipNear->GetValue().GetDouble());
        pCurScene->SetInitCameraFar((float)m_pClipFar->GetValue().GetDouble());
        pCurScene->SetInitCameraFov((float)m_pFov->GetValue().GetDouble());
        CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
        CComponentProxy* pProxy = pCurScene->GetProxyComponent();
        BEATS_ASSERT(pProxy != NULL, _T("Can't find the proxy of Scene!"));
        if (pMainFrame->GetSelectedComponent() == pProxy)
        {
            //Refresh the current showing property grid.
            pMainFrame->SelectComponent(pProxy);
        }
    }
}

void CEditCameraWnd::OnLoadSceneInitClicked(wxCommandEvent& /*event*/)
{
    CScene* pCurScene = CSceneManager::GetInstance()->GetCurrentScene();
    if (pCurScene != NULL)
    {
        CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
        CBeyondEngineEditorGLWindow* pSceneWnd = pMainFrame->GetSceneWindow();
        CCamera* pCurCamera = pSceneWnd->GetCamera();

        if (pCurCamera != NULL)
        {
            const CVec3& rotation = pCurScene->GetInitCameraRotation();
            pCurCamera->SetRotation(rotation.x, rotation.y, rotation.z);
            const CVec3& viewPos = pCurScene->GetInitCameraPos();
            pCurCamera->SetViewPos(viewPos.x, viewPos.y, viewPos.z);
            float fNear = pCurScene->GetInitCameraNear();
            pCurCamera->SetNear(fNear);
            float fFar = pCurScene->GetInitCameraFar();
            pCurCamera->SetFar(fFar);
            float fFov = pCurScene->GetInitCameraFov();
            pCurCamera->SetFOV(fFov);
        }
    }
}

void CEditCameraWnd::AddSpinCtrl(wxPGProperty* pProperty)
{
    pProperty->SetEditor(wxPGEditor_SpinCtrl);
    pProperty->SetAttribute( wxT("Step"), 0.2f );
    pProperty->SetAttribute( wxT("MotionSpin"), true );
}