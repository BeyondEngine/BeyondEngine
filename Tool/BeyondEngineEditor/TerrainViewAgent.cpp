#include "stdafx.h"
#include "TerrainViewAgent.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "EditorMainFrame.h"
#include "wxWidgetsPropertyBase.h"
#include "wx/tglbtn.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"
#include "Render/RenderManager.h"
#include "Resource/ResourcePathManager.h"
#include "Event/TouchDelegate.h"
#include <wx/msw/private.h>
#include "WxGLRenderWindow.h"

const static int MaxBrushSize = 100;
CTerrainViewAgent* CTerrainViewAgent::m_pInstance = nullptr;
CTerrainViewAgent::CTerrainViewAgent() 
    : m_nTerrainType(1)
    , m_nBrushSize(1)
{
}

CTerrainViewAgent::~CTerrainViewAgent()
{

}

void CTerrainViewAgent::ProcessMouseEvent( wxMouseEvent& event )
{
    __super::ProcessMouseEvent(event);
    wxPoint MousePos = event.GetPosition();
    CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    wxGLRenderWindow* pSceneRenderWnd = down_cast<wxGLRenderWindow*>(pRenderTarget);
    BEATS_ASSERT(pSceneRenderWnd != NULL);
    float fScaleFactor = pSceneRenderWnd->IsFBOInUse() ? (float)pSceneRenderWnd->GetFBOViewPortWidth() / pSceneRenderWnd->GetWidth() : pSceneRenderWnd->GetScaleFactor();
    float fX = MousePos.x / fScaleFactor;
    float fY = MousePos.y / fScaleFactor;
    int id = 0;
    unsigned int uTapCount = 1;
    static bool bStartTouch = false;

    if(event.LeftIsDown())
    {
        if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_LEFT) && bStartTouch)
        {
            CTouchDelegate::GetInstance()->OnTouchMoved( 1, &id, &fX, &fY );
        }
        else
        {
            CTouchDelegate::GetInstance()->OnTouchBegan( 1, &id, &fX, &fY, &uTapCount );
            bStartTouch = true;
        }
    }
    else if(event.LeftUp())
    {
        CTouchDelegate::GetInstance()->OnTouchEnded( 1, &id, &fX, &fY, &uTapCount );
        bStartTouch = false;
    }
    if((event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_RIGHT)) || event.ButtonUp(wxMOUSE_BTN_RIGHT))
    {
        CScene* pCurScene = CSceneManager::GetInstance()->GetCurrentScene();
        if (pCurScene != NULL)
        {
        }
    }
}

void CTerrainViewAgent::CreateTools()
{
    wxPanel* pPanel = new wxPanel(m_pMainFrame->m_pToolBook);
    wxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    pPanel->SetSizer(pSizer);

    wxSizer* pChildSizer = new wxBoxSizer(wxHORIZONTAL);
    wxToggleButton* pToggleButton = new wxToggleButton(pPanel, Ctrl_Button_BrushModel, wxT("Brush"));
    pToggleButton->Connect(wxEVT_TOGGLEBUTTON, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);

    m_pTextCtrl = new wxTextCtrl(pPanel, Ctrl_Text_BrushSize, wxString::Format("%d", m_nBrushSize));
    const DWORD styleOld = ::GetWindowLong(m_pTextCtrl->GetHWND(), GWL_STYLE);
    DWORD styleNew = styleOld | ES_NUMBER;
    ::SetWindowLong(m_pTextCtrl->GetHWND(), GWL_STYLE, styleNew);//limit to input only figures
    m_pTextCtrl->Connect(wxEVT_TEXT, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);

    wxArrayString strChoiceList;
    strChoiceList.Add(_T("NOTPASS"));
    strChoiceList.Add(_T("PASS"));
    strChoiceList.Add(_T("SPEEDUP"));
    strChoiceList.Add(_T("SPEEDDOWN"));
    wxChoice* pChoice = new wxChoice(pPanel, Ctrl_Choice_BrushType, wxDefaultPosition, wxDefaultSize, strChoiceList);
    pChoice->Connect(wxEVT_CHOICE, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    pChoice->SetSelection(m_nTerrainType);

    pChildSizer->Add(pToggleButton, 0, wxGROW|wxALL, 0);
    pChildSizer->AddSpacer(10);
    pChildSizer->Add(m_pTextCtrl, 1, wxGROW|wxALL, 0);
    pSizer->Add(pChildSizer, 0, wxGROW|wxALL, 0);
    pSizer->Add(pChoice, 0, wxGROW|wxALL, 0);

    m_pMainFrame->m_pToolBook->AddPage(pPanel, wxT("Tools"));
    m_pMainFrame->m_Manager.Update();
}

void CTerrainViewAgent::SetBrushSize( int nSize )
{
    if (nSize <= 0)
    {
        nSize = 1;
        m_pTextCtrl->SetValue(wxString::Format("%d",nSize));
    }
    else if(nSize > MaxBrushSize)
    {
        nSize = MaxBrushSize;
        m_pTextCtrl->SetValue(wxString::Format("%d",nSize));
    }
    m_nBrushSize = nSize;
}

void CTerrainViewAgent::SetBrushTypeByIndex( int nIndex )
{
    m_nTerrainType = nIndex;
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
}

void CTerrainViewAgent::OnCommandEvent(wxCommandEvent& event)
{
    switch (event.GetId())
    {
    case Ctrl_Choice_BrushType:
        SetBrushTypeByIndex(event.GetInt());
        break;
    case Ctrl_Text_BrushSize:
        {
            long nSize = 0;
            event.GetString().ToLong(&nSize);
            SetBrushSize(nSize);
        }
        break;
    case Ctrl_Button_BrushModel:
        m_pMainFrame->m_nCursorIconID = event.IsChecked() ? wxCURSOR_PAINT_BRUSH : wxCURSOR_ARROW;
        break;
    default:
        break;
    }
}
