#include "stdafx.h"
#include "CoordinateSettingWnd.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/advprops.h"
#include "EditorMainFrame.h"
#include "EngineEditor.h"
#include "Render/RenderManager.h"
#include "Render/CoordinateRenderObject.h"
#include "EditorConfig.h"

CCoordinateSettingWnd::CCoordinateSettingWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name /*= wxDialogNameStr*/)
    :super(parent, id, title, pos, size, style, name)
    ,m_pPropertyGrid(NULL)
    , m_axisColorX(NULL)
    , m_axisColorY(NULL)
    , m_axisColorZ(NULL)
    , m_axisSelectColor(NULL)
    , m_panelSelectColor(NULL)
    , m_fAxisLengthFactor(NULL)
    , m_fConeHeightFactor(NULL)
    , m_fConeBottomColorFactor(NULL)
    , m_fConeAngle(NULL)
    , m_fTranslatePanelStartPosRate(NULL)
{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);
    m_pPropertyGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxSize(300, 400),
        wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED);
    m_pVisibleProperty = m_pPropertyGrid->Append(new wxBoolProperty(wxT("开启"), wxT("m_pVisibleProperty"), true));
    m_axisColorX = m_pPropertyGrid->Append(new wxColourProperty(wxT("X轴颜色"), wxT("X_AIXS_COLOR"), *wxBLUE));
    m_axisColorY = m_pPropertyGrid->Append(new wxColourProperty(wxT("Y轴颜色"), wxT("Y_AIXS_COLOR"), *wxBLUE));
    m_axisColorZ = m_pPropertyGrid->Append(new wxColourProperty(wxT("Z轴颜色"), wxT("Z_AIXS_COLOR"), *wxBLUE));
    m_axisSelectColor = m_pPropertyGrid->Append(new wxColourProperty(wxT("选中轴颜色"), wxT("SELECT_AIXS_COLOR"), *wxBLUE));
    m_panelSelectColor = m_pPropertyGrid->Append(new wxColourProperty(wxT("选中平面颜色"), wxT("SELECT_PANEL_COLOR"), *wxBLUE));
    m_fAxisLengthFactor = m_pPropertyGrid->Append(new wxFloatProperty(wxT("坐标轴长度系数"), wxT("AxisLengthFactor")));
    AddSpinCtrl(m_fAxisLengthFactor, 0.1f);
    m_fConeHeightFactor = m_pPropertyGrid->Append(new wxFloatProperty(wxT("锥高度"), wxT("ConeHeightFactor")));
    AddSpinCtrl(m_fConeHeightFactor, 0.1f);
    m_fConeBottomColorFactor = m_pPropertyGrid->Append(new wxFloatProperty(wxT("锥底面颜色系数"), wxT("ConeBottomColorFactor")));
    AddSpinCtrl(m_fConeBottomColorFactor, 0.1f);
    m_fConeAngle = m_pPropertyGrid->Append(new wxFloatProperty(wxT("锥角度"), wxT("ConeRadiusRate")));
    AddSpinCtrl(m_fConeAngle, 0.1f);
    m_fTranslatePanelStartPosRate = m_pPropertyGrid->Append(new wxFloatProperty(wxT("平面边长系数"), wxT("TranslatePanelStartPosRate")));
    AddSpinCtrl(m_fTranslatePanelStartPosRate, 0.1f);

    pMainSizer->Add(m_pPropertyGrid, 1, wxALL, 5);

    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CCoordinateSettingWnd::OnClose), NULL, this);
    m_pPropertyGrid->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CCoordinateSettingWnd::OnPropertyChanged), NULL, this);
}

CCoordinateSettingWnd::~CCoordinateSettingWnd()
{
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CCoordinateSettingWnd::OnClose), NULL, this);
    m_pPropertyGrid->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CCoordinateSettingWnd::OnPropertyChanged), NULL, this);
}

void CCoordinateSettingWnd::OnClose(wxCloseEvent& /*event*/)
{
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    pMainFrame->GetAuiToolBarPerformPtr()->ToggleTool(ID_SetCoordinateBtn, false);
    pMainFrame->GetAuiToolBarPerformPtr()->Refresh(false);
    this->Show(false);
}

void CCoordinateSettingWnd::OnPropertyChanged(wxPropertyGridEvent& event)
{
    wxPGProperty* pChangedProperty = event.GetProperty();
    CCoordinateRenderObject* pCoordinateRenderObject = CRenderManager::GetInstance()->GetCoordinateRenderObject();
    if (pChangedProperty == m_pVisibleProperty)
    {
        pCoordinateRenderObject->SetVisible(m_pVisibleProperty->GetValue());
    }
    else if (pChangedProperty == m_axisColorX)
    {
        pCoordinateRenderObject->SetAxisColorX(GetColorValue(m_axisColorX));
        CEditorConfig::GetInstance()->SetAxisColorX(GetColorValue(m_axisColorX));
    }
    else if (pChangedProperty == m_axisColorY)
    {
        pCoordinateRenderObject->SetAxisColorY(GetColorValue(m_axisColorY));
        CEditorConfig::GetInstance()->SetAxisColorY(GetColorValue(m_axisColorY));
    }
    else if (pChangedProperty == m_axisColorZ)
    {
        pCoordinateRenderObject->SetAxisColorZ(GetColorValue(m_axisColorZ));
        CEditorConfig::GetInstance()->SetAxisColorZ(GetColorValue(m_axisColorZ));
    }
    else if (pChangedProperty == m_axisSelectColor)
    {
        pCoordinateRenderObject->SetAxisSelectColor(GetColorValue(m_axisSelectColor));
        CEditorConfig::GetInstance()->SetAxisSelectColor(GetColorValue(m_axisSelectColor));
    }
    else if (pChangedProperty == m_panelSelectColor)
    {
        pCoordinateRenderObject->SetPanelSelectColor(GetColorValue(m_panelSelectColor));
        CEditorConfig::GetInstance()->SetPanelSelectColor(GetColorValue(m_panelSelectColor));
    }
    else if (pChangedProperty == m_fAxisLengthFactor)
    {
        pCoordinateRenderObject->SetAxisLengthFactor(m_fAxisLengthFactor->GetValue().GetDouble());
        CEditorConfig::GetInstance()->SetAxisLengthFactor(m_fAxisLengthFactor->GetValue().GetDouble());
    }
    else if (pChangedProperty == m_fConeHeightFactor)
    {
        pCoordinateRenderObject->SetConeHeightFactor(m_fConeHeightFactor->GetValue().GetDouble());
        CEditorConfig::GetInstance()->SetConeHeightFactor(m_fConeHeightFactor->GetValue().GetDouble());
    }
    else if (pChangedProperty == m_fConeBottomColorFactor)
    {
        pCoordinateRenderObject->SetConeBottomColorFactor(m_fConeBottomColorFactor->GetValue().GetDouble());
        CEditorConfig::GetInstance()->SetConeBottomColorFactor(m_fConeBottomColorFactor->GetValue().GetDouble());
    }
    else if (pChangedProperty == m_fConeAngle)
    {
        pCoordinateRenderObject->SetConeAngle(m_fConeAngle->GetValue().GetDouble());
        CEditorConfig::GetInstance()->SetConeAngle(m_fConeAngle->GetValue().GetDouble());
    }
    else if (pChangedProperty == m_fTranslatePanelStartPosRate)
    {
        pCoordinateRenderObject->SetTranslatePanelStartPosRate(m_fTranslatePanelStartPosRate->GetValue().GetDouble());
        CEditorConfig::GetInstance()->SetTranslatePanelStartPosRate(m_fTranslatePanelStartPosRate->GetValue().GetDouble());
    }

}

bool CCoordinateSettingWnd::Show(bool bShow /*= true*/)
{
    if (bShow)
    {
        CCoordinateRenderObject* pCoordinateRenderObject = CRenderManager::GetInstance()->GetCoordinateRenderObject();
        CColor color = pCoordinateRenderObject->GetAxisColorX();
        wxString strColorValue = static_cast<wxColourProperty*>(m_axisColorX)->ColourToString(wxColour(color.r, color.g, color.b, color.a), wxNOT_FOUND, wxPG_FULL_VALUE);
        m_axisColorX->SetValueFromString(strColorValue);
        color = pCoordinateRenderObject->GetAxisColorY();
        strColorValue = static_cast<wxColourProperty*>(m_axisColorY)->ColourToString(wxColour(color.r, color.g, color.b, color.a), wxNOT_FOUND, wxPG_FULL_VALUE);
        m_axisColorY->SetValueFromString(strColorValue);
        color = pCoordinateRenderObject->GetAxisColorZ();
        strColorValue = static_cast<wxColourProperty*>(m_axisColorZ)->ColourToString(wxColour(color.r, color.g, color.b, color.a), wxNOT_FOUND, wxPG_FULL_VALUE);
        m_axisColorZ->SetValueFromString(strColorValue);
        color = pCoordinateRenderObject->GetAxisSelectColor();
        strColorValue = static_cast<wxColourProperty*>(m_axisSelectColor)->ColourToString(wxColour(color.r, color.g, color.b, color.a), wxNOT_FOUND, wxPG_FULL_VALUE);
        m_axisSelectColor->SetValueFromString(strColorValue);
        color = pCoordinateRenderObject->GetPanelSelectColor();
        strColorValue = static_cast<wxColourProperty*>(m_panelSelectColor)->ColourToString(wxColour(color.r, color.g, color.b, color.a), wxNOT_FOUND, wxPG_FULL_VALUE);
        m_panelSelectColor->SetValueFromString(strColorValue);
        m_fAxisLengthFactor->SetValue(pCoordinateRenderObject->GetAxisLengthFactor());
        m_fConeHeightFactor->SetValue(pCoordinateRenderObject->GetConeHeightFactor());
        m_fConeBottomColorFactor->SetValue(pCoordinateRenderObject->GetConeBottomColorFactor());
        m_fConeAngle->SetValue(pCoordinateRenderObject->GetConeAngle());
        m_fTranslatePanelStartPosRate->SetValue(pCoordinateRenderObject->GetTranslatePanelStartPosRate());
    }
    return super::Show(bShow);
}

uint32_t CCoordinateSettingWnd::GetColorValue(wxPGProperty* property)
{
    wxColor colourValue;
    colourValue << property->GetValue();
    return (colourValue.Red() << 24) + (colourValue.Green() << 16) + (colourValue.Blue() << 8) + colourValue.Alpha();
}

void CCoordinateSettingWnd::AddSpinCtrl(wxPGProperty* pProperty, float fStep /*= 1.0f*/)
{
    pProperty->SetEditor(wxPGEditor_SpinCtrl);
    pProperty->SetAttribute(wxT("Step"), fStep);
    pProperty->SetAttribute(wxT("MotionSpin"), true);
}
