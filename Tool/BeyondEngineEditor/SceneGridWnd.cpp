#include "stdafx.h"
#include "SceneGridWnd.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "BeyondEngineEditorGLWindow.h"
#include "Render/GridRenderObject.h"
#include "Render/RenderManager.h"
#include "EditorSceneWindow.h"

CSceneGridWnd::CSceneGridWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : super(parent, id, title, pos, size, style, name)
    , m_pPropertyGrid(NULL)
    , m_pGridStartPosX(NULL)
    , m_pGridStartPosY(NULL)
    , m_pGridWidth(NULL)
    , m_pGridHeight(NULL)
    , m_pGridDistance(NULL)
    , m_pGridLineWidth(NULL)
    , m_pSimulateWidth(NULL)
    , m_pSimulateHeight(NULL)

{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);
    m_pPropertyGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxSize(400,400),
        wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED );

    m_pGridStartPosX = m_pPropertyGrid->Append( new wxIntProperty(wxT("网格起点X"), wxT("GridStartPosX")) );
    AddSpinCtrl(m_pGridStartPosX);
    m_pGridStartPosY = m_pPropertyGrid->Append( new wxIntProperty(wxT("网格起点Y"), wxT("GridStartPosY")) );
    AddSpinCtrl(m_pGridStartPosY);
    m_pGridWidth = m_pPropertyGrid->Append( new wxUIntProperty(wxT("网格宽度"), wxT("GridWidth")) );
    AddSpinCtrl(m_pGridWidth);
    m_pGridHeight = m_pPropertyGrid->Append( new wxUIntProperty(wxT("网格高度"), wxT("GridHeight")) );
    AddSpinCtrl(m_pGridHeight);
    m_pGridDistance = m_pPropertyGrid->Append( new wxFloatProperty(wxT("网格密度"), wxT("GridDistance")) );
    m_pGridDistance->SetAttribute( wxPG_ATTR_MIN, 0.1f );
    m_pGridDistance->SetAttribute( wxPG_ATTR_MAX, 100);
    AddSpinCtrl(m_pGridDistance);
    m_pGridLineWidth = m_pPropertyGrid->Append( new wxFloatProperty(wxT("网格粗细"), wxT("GridLineWidth")) );
    m_pGridLineWidth->SetAttribute( wxPG_ATTR_MIN, 0.1f );
    AddSpinCtrl(m_pGridLineWidth);

    m_pRenderPositiveDirectionLine = m_pPropertyGrid->Append( new wxBoolProperty(wxT("绘制正向线"), wxT("RenderPositiveLine")) );
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderPositiveDirectionLine, wxPG_BOOL_USE_CHECKBOX, true);
    m_pRenderMapGrid = m_pPropertyGrid->Append( new wxBoolProperty(wxT("绘制地图网格"), wxT("RenderMapGrid")) );
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderMapGrid, wxPG_BOOL_USE_CHECKBOX, true);
    m_pRenderPathFindingGrid = m_pPropertyGrid->Append( new wxBoolProperty(wxT("绘制寻路网格"), wxT("RenderPathFindingGrid")) );
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderPathFindingGrid, wxPG_BOOL_USE_CHECKBOX, true);
    m_pRenderBuilding = m_pPropertyGrid->Append( new wxBoolProperty(wxT("绘制建筑"), wxT("RenderBuilding")) );
    m_pRenderBuilding->SetValue(wxVariant(true));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderBuilding, wxPG_BOOL_USE_CHECKBOX, true);

    m_pSimulateWidth = m_pPropertyGrid->Append( new wxUIntProperty(wxT("模拟宽度"), wxT("SimulateWidth")) );
    m_pSimulateWidth->SetAttribute( wxPG_ATTR_MIN, 400 );
    m_pSimulateHeight = m_pPropertyGrid->Append( new wxUIntProperty(wxT("模拟高度"), wxT("SimulateHeight")) );
    m_pSimulateHeight->SetAttribute( wxPG_ATTR_MIN, 300 );

    m_pFPS = m_pPropertyGrid->Append( new wxUIntProperty(wxT("FPS"), wxT("FPS")) );
    m_pFPS->SetAttribute( wxPG_ATTR_MIN, 1 );

    pMainSizer->Add(m_pPropertyGrid, 1, wxALL, 5);

    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CSceneGridWnd::OnClose), NULL, this);
    m_pPropertyGrid->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CSceneGridWnd::OnPropertyChanged), NULL, this);
}

CSceneGridWnd::~CSceneGridWnd()
{
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CSceneGridWnd::OnClose), NULL, this);
    m_pPropertyGrid->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CSceneGridWnd::OnPropertyChanged), NULL, this);
}

bool CSceneGridWnd::Show(bool bShow/* = true*/)
{
    bool bRet = false;
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    CEditorSceneWindow* pSceneWnd = pMainFrame->GetSceneWindow();
    CGridRenderObject* pGrid = pSceneWnd->GetGrid();
    if (pGrid != NULL)
    {
        if (bShow)
        {
            int nStartPosX, nStartPosY;
            pGrid->GetGridStartPos(nStartPosX, nStartPosY);
            m_pGridStartPosX->SetValue(nStartPosX);
            m_pGridStartPosY->SetValue(nStartPosY);
            size_t uGridWidth = pGrid->GetGridWidth();
            m_pGridWidth->SetValue((int)uGridWidth);
            size_t uGridHeight = pGrid->GetGridHeight();
            m_pGridHeight->SetValue((int)uGridHeight);
            float fGridDistance = pGrid->GetGridDistance();
            m_pGridDistance->SetValue(fGridDistance);
            float fLineWidth = pGrid->GetLineWidth();
            m_pGridLineWidth->SetValue(fLineWidth);
            size_t uWidth, uHeight;
            CRenderManager::GetInstance()->GetSimulateSize(uWidth, uHeight);
            m_pSimulateWidth->SetValue((int)uWidth);
            m_pSimulateHeight->SetValue((int)uHeight);
            size_t curFPS = CEngineCenter::GetInstance()->GetFPS();
            m_pFPS->SetValue((int)curFPS);
        }
        bRet = super::Show(bShow);
    }
    else
    {
        CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
        pMainFrame->GetAuiToolBarPerformPtr()->ToggleTool(ID_SettingBtn, false);
        pMainFrame->GetAuiToolBarPerformPtr()->Refresh(false);
    }
    return bRet;
}

void CSceneGridWnd::OnClose(wxCloseEvent& /*event*/)
{
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    pMainFrame->GetAuiToolBarPerformPtr()->ToggleTool(ID_SettingBtn, false);
    pMainFrame->GetAuiToolBarPerformPtr()->Refresh(false);
    this->Show(false);
}

void CSceneGridWnd::OnPropertyChanged(wxPropertyGridEvent &event)
{
    wxPGProperty* pChangedProperty = event.GetProperty();
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    CEditorSceneWindow* pSceneWnd = pMainFrame->GetSceneWindow();
    CGridRenderObject* pGrid = pSceneWnd->GetGrid();
    if (pChangedProperty == m_pGridDistance)
    {
        pGrid->SetGridDistance((float)(m_pGridDistance->GetValue().GetDouble()));
    }
    else if (pChangedProperty == m_pGridLineWidth)
    {
        pGrid->SetLineWidth((float)(m_pGridLineWidth->GetValue().GetDouble()));
    }
    else if (pChangedProperty == m_pRenderPositiveDirectionLine)
    {
        pGrid->SetPositiveLineLength(m_pRenderPositiveDirectionLine->GetValue().GetBool() ? 1000.0f : 0);
    }
    else if (pChangedProperty == m_pRenderMapGrid)
    {
    }
    else if (pChangedProperty == m_pRenderPathFindingGrid)
    {
    }
    else if (pChangedProperty == m_pRenderBuilding)
    {
    }
    else if (pChangedProperty == m_pGridStartPosX ||
            pChangedProperty == m_pGridStartPosY)
    {
        pGrid->SetGridStartPos(m_pGridStartPosX->GetValue().GetLong(), m_pGridStartPosY->GetValue().GetLong());
    }
    else if (pChangedProperty == m_pGridWidth)
    {
        pGrid->SetGridWidth((size_t)(m_pGridWidth->GetValue().GetLong()));
    }
    else if (pChangedProperty == m_pGridHeight)
    {
        pGrid->SetGridHeight((size_t)(m_pGridHeight->GetValue().GetLong()));
    }
    else if (pChangedProperty == m_pSimulateWidth || pChangedProperty == m_pSimulateHeight)
    {
        CRenderManager::GetInstance()->SetSimulateSize(m_pSimulateWidth->GetValue().GetInteger(), m_pSimulateHeight->GetValue().GetInteger());
        CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
        pMainFrame->GetSceneWindow()->SendSizeEvent();//Trigger re-calculate window size.
    }
    else if (pChangedProperty == m_pFPS)
    {
        CEngineCenter::GetInstance()->SetFPS(m_pFPS->GetValue().GetInteger());
    }
    else
    {
        BEATS_ASSERT(false, _T("Never reach here!"));
    }
}

void CSceneGridWnd::AddSpinCtrl(wxPGProperty* pProperty)
{
    pProperty->SetEditor(wxPGEditor_SpinCtrl);
    pProperty->SetAttribute( wxT("Step"), 0.2f );
    pProperty->SetAttribute( wxT("MotionSpin"), true );
}