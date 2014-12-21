#include "stdafx.h"
#include "TerrainViewAgent.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "EditorMainFrame.h"
#include "wxWidgetsPropertyBase.h"
#include "wx/tglbtn.h"
#include "Component/Component/ComponentInstance.h"
#include "Render/RenderManager.h"
#include "Resource/ResourceManager.h"
#include "Event/TouchDelegate.h"
#include <wx/msw/private.h>
#include "WxGLRenderWindow.h"
#include "EditorSceneWindow.h"
#include "Component/Component/ComponentProxyManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Component/Component/ComponentProject.h"
#include "MapPropertyDescription.h"
#include "MapElementPropertyDescription.h"

const static int MaxBrushSize = 100;
CTerrainViewAgent* CTerrainViewAgent::m_pInstance = nullptr;
CTerrainViewAgent::CTerrainViewAgent() 
    : m_bEditUniverseOrIsland(false)
    , m_bInitUniverseEditMode(false)
    , m_nBrushSize(1)
    , m_pTextCtrl(NULL)
    , m_pCurrentSize(NULL)
    , m_pDirectionType(NULL)
    , m_pTextObjectName(NULL)
    , m_pTextObjectLevel(NULL)
    , m_pTextObjectNum(NULL)
    , m_pToolMainSizer(NULL)
    , m_pBrushTypeSizer(NULL)
    , m_pLandingAreaBrushSettingSizer(nullptr)
    , m_pObjectBrushSettingSizer(nullptr)
    , m_eBrushType(eBrushType::eBT_Eraser)
{
}

CTerrainViewAgent::~CTerrainViewAgent()
{

}

void CTerrainViewAgent::ProcessMouseEvent(wxMouseEvent& event)
{
    __super::ProcessMouseEvent(event);
    wxPoint MousePos = event.GetPosition();
    CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    wxGLRenderWindow* pSceneRenderWnd = down_cast<wxGLRenderWindow*>(pRenderTarget);
    BEATS_ASSERT(pSceneRenderWnd != NULL);
    float fScaleFactor = pSceneRenderWnd->IsFBOInUse() ? (float)pSceneRenderWnd->GetFBOViewPortWidth() / pSceneRenderWnd->GetLogicWidth() : pSceneRenderWnd->GetScaleFactor();
    float fX = MousePos.x / fScaleFactor;
    float fY = MousePos.y / fScaleFactor;
    size_t id = 0;
    static bool bStartTouch = false;

    if (event.LeftIsDown())
    {
        if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_LEFT) && bStartTouch)
        {
            CTouchDelegate::GetInstance()->OnTouchMoved(1, &id, &fX, &fY);
        }
        else
        {
            CTouchDelegate::GetInstance()->OnTouchBegan(1, &id, &fX, &fY);
            bStartTouch = true;
        }
    }
    else if (event.LeftUp())
    {
        CTouchDelegate::GetInstance()->OnTouchEnded(1, &id, &fX, &fY);
        bStartTouch = false;
    }
    else if (event.GetEventType() == wxEVT_MOUSEWHEEL)
    {
        CCamera* pCamera = m_pMainFrame->GetSceneWindow()->GetCamera();
        pCamera->SetFOV(pCamera->GetFOV() + 0.5f * event.GetWheelRotation() / -120);
    }
}

void CTerrainViewAgent::CreateTools()
{
    wxPanel* pPanel = new wxPanel(m_pMainFrame->m_pToolBook);
    m_pToolMainSizer = new wxBoxSizer(wxVERTICAL);
    pPanel->SetSizer(m_pToolMainSizer);

    wxSizer* pBrushSizeSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* pBrushSizeButton1 = new wxButton(pPanel, Ctrl_Button_BrushSize1, wxT("1"), wxDefaultPosition, wxSize(60, 30));
    pBrushSizeButton1->Connect(wxEVT_BUTTON, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    wxButton* pBrushSizeButton2 = new wxButton(pPanel, Ctrl_Button_BrushSize2, wxT("2"), wxDefaultPosition, wxSize(60, 30));
    pBrushSizeButton2->Connect(wxEVT_BUTTON, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    wxButton* pBrushSizeButton3 = new wxButton(pPanel, Ctrl_Button_BrushSize3, wxT("3"), wxDefaultPosition, wxSize(60, 30));
    pBrushSizeButton3->Connect(wxEVT_BUTTON, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    wxButton* pBrushSizeButtonN = new wxButton(pPanel, Ctrl_Button_BrushSizeN, wxT("n"), wxDefaultPosition, wxSize(60, 30));
    pBrushSizeButtonN->Connect(wxEVT_BUTTON, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);

    m_pTextCtrl = new wxTextCtrl(pPanel, wxID_ANY, wxString::Format("%d", m_nBrushSize));
    const DWORD styleOld = ::GetWindowLong(m_pTextCtrl->GetHWND(), GWL_STYLE);
    DWORD styleNew = styleOld | ES_NUMBER;
    ::SetWindowLong(m_pTextCtrl->GetHWND(), GWL_STYLE, styleNew);//limit to input only figures
    m_pTextCtrl->Connect(wxEVT_TEXT, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);

    pBrushSizeSizer->Add(pBrushSizeButton1, 1, wxGROW, 0);
    pBrushSizeSizer->Add(pBrushSizeButton2, 1, wxGROW, 0);
    pBrushSizeSizer->Add(pBrushSizeButton3, 1, wxGROW, 0);
    pBrushSizeSizer->Add(pBrushSizeButtonN, 1, wxGROW, 0);
    pBrushSizeSizer->Add(m_pTextCtrl, 1, wxGROW, 0);


    m_pBrushTypeSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* pBattleAreaBrush = new wxButton(pPanel, Ctrl_Button_Brush_BattleArea, wxT("战斗区"), wxDefaultPosition, wxSize(60, 30));
    pBattleAreaBrush->Connect(wxEVT_BUTTON, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    wxButton* pLandingAreaBrush = new wxButton(pPanel, Ctrl_Button_Brush_LandingArea, wxT("登陆区"), wxDefaultPosition, wxSize(60, 30));
    pLandingAreaBrush->Connect(wxEVT_BUTTON, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    wxButton* pObjectBrush = new wxButton(pPanel, Ctrl_Button_Brush_ObjectBrush, wxT("物体刷子"), wxDefaultPosition, wxSize(60, 30));
    pObjectBrush->Connect(wxEVT_BUTTON, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    wxButton* pEraser = new wxButton(pPanel, Ctrl_Button_Brush_Eraser, wxT("橡皮擦"), wxDefaultPosition, wxSize(60, 30));
    pEraser->Connect(wxEVT_BUTTON, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    m_pBrushTypeSizer->Add(pBattleAreaBrush, 1, wxGROW | wxALL, 0);
    m_pBrushTypeSizer->Add(pLandingAreaBrush, 1, wxGROW | wxALL, 0);
    m_pBrushTypeSizer->Add(pObjectBrush, 1, wxGROW | wxALL, 0);
    m_pBrushTypeSizer->Add(pEraser, 1, wxGROW | wxALL, 0);

    m_pUniverseBrushSizer = new wxBoxSizer(wxHORIZONTAL);
    wxCheckBox* pInitEditCheckBox = new wxCheckBox(pPanel, Ctrl_CheckBox_InitUniverseEdit, wxT("编辑初始区(按ctrl)"));
    pInitEditCheckBox->SetValue(false);
    pInitEditCheckBox->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    m_pUniverseBrushSizer->Add(pInitEditCheckBox, 1, wxGROW | wxALL, 0);

    wxCheckBox* pAllUnlockEditCheckBox = new wxCheckBox(pPanel, Ctrl_CheckBox_HideCover, wxT("隐藏遮罩"));
    pAllUnlockEditCheckBox->SetValue(m_bHideCover);
    pAllUnlockEditCheckBox->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    m_pUniverseBrushSizer->Add(pAllUnlockEditCheckBox, 1, wxGROW | wxALL, 0);

    wxCheckBox* pHideMovableCellCheckBox = new wxCheckBox(pPanel, Ctrl_CheckBox_HideMovableCell, wxT("隐藏可行走区"));
    pHideMovableCellCheckBox->SetValue(m_bHideMovableCell);
    pHideMovableCellCheckBox->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    m_pUniverseBrushSizer->Add(pHideMovableCellCheckBox, 1, wxGROW | wxALL, 0);

    wxCheckBox* pShowIndexText = new wxCheckBox(pPanel, Ctrl_CheckBox_ShowIndexText, wxT("显示索引"));
    pShowIndexText->SetValue(m_bShowIndexText);
    pShowIndexText->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    m_pUniverseBrushSizer->Add(pShowIndexText, 1, wxGROW | wxALL, 0);
    m_pLandingAreaBrushSettingSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* pDirectionLabel = new wxStaticText(pPanel, wxID_ANY, _T("方向:"));
    m_pDirectionType = new wxTextCtrl(pPanel, wxID_ANY);
    m_pLandingAreaBrushSettingSizer->Add(pDirectionLabel, 0, wxGROW | wxALL, 0);
    m_pLandingAreaBrushSettingSizer->Add(m_pDirectionType, 1, wxGROW | wxALL, 0);

    m_pObjectBrushSettingSizer = new wxBoxSizer(wxVERTICAL);
    m_pMapInfoSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* pLabelSizer = new wxBoxSizer(wxHORIZONTAL);
    wxSizer* pTextSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* pNameLabel = new wxStaticText(pPanel, wxID_ANY, _T("名字"));
    wxStaticText* pLevelLabel = new wxStaticText(pPanel, wxID_ANY, _T("等级"));
    wxStaticText* pNumLabel = new wxStaticText(pPanel, wxID_ANY, _T("数量"));
    m_pTextObjectName = new wxTextCtrl(pPanel, wxID_ANY);
    m_pTextObjectLevel = new wxTextCtrl(pPanel, wxID_ANY);
    m_pTextObjectNum = new wxTextCtrl(pPanel, wxID_ANY);
    pLabelSizer->Add(pNameLabel, 1, wxGROW | wxALL, 0);
    pLabelSizer->Add(pLevelLabel, 1, wxGROW | wxALL, 0);
    pLabelSizer->Add(pNumLabel, 1, wxGROW | wxALL, 0);
    pTextSizer->Add(m_pTextObjectName, 1, wxGROW | wxALL, 0);
    pTextSizer->Add(m_pTextObjectLevel, 1, wxGROW | wxALL, 0);
    pTextSizer->Add(m_pTextObjectNum, 1, wxGROW | wxALL, 0);

    wxButton* pButtonApplyMapSize = new wxButton(pPanel, Ctrl_Button_ApplyMapSize, wxT("应用"), wxDefaultPosition, wxSize(60, 30));
    pButtonApplyMapSize->Connect(wxEVT_BUTTON, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    m_pMapSizeX = new wxTextCtrl(pPanel, wxID_ANY);
    ::SetWindowLong(m_pMapSizeX->GetHWND(), GWL_STYLE, styleNew);//limit to input only figures
    m_pMapSizeY = new wxTextCtrl(pPanel, wxID_ANY);
    ::SetWindowLong(m_pMapSizeY->GetHWND(), GWL_STYLE, styleNew);//limit to input only figures

    m_pCurrentMapData = new wxStaticText(pPanel, wxID_ANY, "");
    wxStaticText* pMapDataFile = new wxStaticText(pPanel, wxID_ANY, _T("地形文件:"));
    m_pMapInfoSizer->Add(pMapDataFile, 3, wxGROW | wxALL, 0);
    m_pMapInfoSizer->Add(m_pCurrentMapData, 3, wxGROW | wxALL, 0);
    m_pMapInfoSizer->Add(m_pMapSizeX, 0.15f, wxGROW | wxALL, 0);
    m_pMapInfoSizer->Add(m_pMapSizeY, 0.15f, wxGROW | wxALL, 0);
    m_pMapInfoSizer->Add(pButtonApplyMapSize, 0.1f, wxGROW | wxALL, 0);
    m_pObjectBrushSettingSizer->Add(pLabelSizer, 0, wxGROW | wxALL, 0);
    m_pObjectBrushSettingSizer->Add(pTextSizer, 0, wxGROW | wxALL, 0);

    m_pCurrentSize = new wxStaticText(pPanel, wxID_ANY, _T("Brush Size:"));

    m_pToolMainSizer->Add(pBrushSizeSizer, 0, wxALIGN_CENTER | wxALL, 0);
    m_pToolMainSizer->Add(m_pCurrentSize, 0, wxGROW | wxALL, 0);
    m_pToolMainSizer->Add(m_pBrushTypeSizer, 0, wxGROW | wxALL, 0);
    m_pToolMainSizer->Add(m_pMapInfoSizer, 0, wxGROW | wxALL, 0);
    m_pToolMainSizer->Add(m_pUniverseBrushSizer, 0, wxGROW | wxALL, 0);
    m_pToolMainSizer->Add(m_pLandingAreaBrushSettingSizer, 0, wxGROW | wxALL, 0);
    m_pToolMainSizer->Add(m_pObjectBrushSettingSizer, 0, wxGROW | wxALL, 0);
    m_pMainFrame->m_pToolBook->AddPage(pPanel, wxT("Tools"));
    m_pMainFrame->m_Manager.Update();
}

void CTerrainViewAgent::SetBrushSize( int nSize )
{
    if (nSize <= 0)
    {
        nSize = 1;
    }
    else if(nSize > MaxBrushSize)
    {
        nSize = MaxBrushSize;
    }
    wxString strBrushName;
    switch (m_eBrushType)
    {
    case eBrushType::eBT_Eraser:
        strBrushName = _T("橡皮擦");
        break;
    case eBrushType::eBT_BattleArea:
        strBrushName = _T("战斗区");
        break;
    case eBrushType::eBT_LandingArea:
        strBrushName = _T("登陆区");
        break;
    case eBrushType::eBT_ObjectBrush:
        strBrushName = _T("物体刷子");
        break;
    default:
        break;
    }
    m_nBrushSize = nSize;
    m_pCurrentSize->SetLabel(wxString::Format("%s Size:%d", strBrushName.c_str(), m_nBrushSize));
}

void CTerrainViewAgent::InView()
{
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pToolPanel).Show();
    CreateTools();
}

void CTerrainViewAgent::OutView()
{
    m_pMainFrame->m_pToolBook->DeleteAllPages();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pToolPanel).Hide();
    m_pMainFrame->m_Manager.Update();
    m_pMainFrame->m_nCursorIconID = wxCURSOR_ARROW;
}

void CTerrainViewAgent::OnCommandEvent(wxCommandEvent& event)
{
    switch (event.GetId())
    {
    case Ctrl_Button_BrushSize1:
        SetBrushSize(1);
        break;
    case Ctrl_Button_BrushSize2:
        SetBrushSize(2);
        break;
    case Ctrl_Button_BrushSize3:
        SetBrushSize(3);
        break;
    case Ctrl_Button_BrushSizeN:
        {
            long nSize = 0;
            if (m_pTextCtrl->GetValue().ToLong(&nSize))
            {
                SetBrushSize(nSize);
            }
        }
        break;
    case Ctrl_Button_Brush_BattleArea:
        SetBrushType(eBrushType::eBT_BattleArea);
        break;
    case Ctrl_Button_Brush_LandingArea:
        SetBrushType(eBrushType::eBT_LandingArea);
        break;
    case Ctrl_Button_Brush_ObjectBrush:
        SetBrushType(eBrushType::eBT_ObjectBrush);
        break;
    case Ctrl_Button_Brush_Eraser:
        SetBrushType(eBrushType::eBT_Eraser);
        break;
    case Ctrl_Choice_Universe_Type:
        {
            uint32_t uSelectioin = event.GetSelection();
            SetBrushType((eBrushType)((uint32_t)eBrushType::eBT_Near + uSelectioin));
        }
        break;
    case Ctrl_CheckBox_InitUniverseEdit:
        break;
    case Ctrl_CheckBox_HideCover:
        {
            m_bHideCover = event.IsChecked();
        }
        break;
    case Ctrl_CheckBox_HideMovableCell:
        {
            m_bHideMovableCell = event.IsChecked();
        }
        break;
    case Ctrl_CheckBox_ShowIndexText:
        {
            m_bShowIndexText = event.IsChecked();
        }
        break;
    case Ctrl_Button_ApplyMapSize:
        {
        }
        break;
    default:
        break;
    }
}

void CTerrainViewAgent::SetBrushType(eBrushType eType)
{
    m_eBrushType = eType;
    m_pLandingAreaBrushSettingSizer->Show(m_eBrushType == eBrushType::eBT_LandingArea);
    m_pObjectBrushSettingSizer->Show(m_eBrushType == eBrushType::eBT_ObjectBrush);
    m_pToolMainSizer->Layout();
    SetBrushSize(m_nBrushSize);
}

void CTerrainViewAgent::BrushOnPos(float fX, float fY)
{
BEYONDENGINE_UNUSED_PARAM(fX);
BEYONDENGINE_UNUSED_PARAM(fY);
}

void CTerrainViewAgent::OnOpenComponentFile()
{
    RefreshTools();
}

void CTerrainViewAgent::RefreshTools()
{
}
