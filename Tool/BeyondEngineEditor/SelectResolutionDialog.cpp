#include "stdafx.h"
#include "SelectResolutionDialog.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "BeyondEngineEditorGLWindow.h"
#include "EditorSceneWindow.h"
#include "WxGLRenderWindow.h"
#include "Render/RenderManager.h"
#include "EditorConfig.h"
#include "CustomResolutionDialog.h"

CSelectResolutionDialog::CSelectResolutionDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : super(parent, id, title, pos, size, style, name)
    , m_bSetResolution(false)
    , m_pSizer(NULL)
    , m_pMainFrame(NULL)
{
    m_pMainFrame = ((CEngineEditor*)wxApp::GetInstance())->GetMainFrame();
    m_pSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(m_pSizer);
    InitSelectResolutionDialog();
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CSelectResolutionDialog::OnClose), NULL, this);
}

CSelectResolutionDialog::~CSelectResolutionDialog()
{
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CSelectResolutionDialog::OnClose), NULL, this);
}

void CSelectResolutionDialog::InitSelectResolutionDialog()
{
    m_pSizer->Clear(true);
    std::vector<CVec2> resolutionVector;
    resolutionVector.push_back(CVec2(1024, 768));
    resolutionVector.push_back(CVec2(1920, 1080));
    for (uint32_t i = 0; i < resolutionVector.size(); ++i)
    {
        wxButton* pResolutionBtn = new wxButton(this, wxID_ANY, wxString::Format(_T("%dx%d"), uint32_t(resolutionVector[i].X()), uint32_t(resolutionVector[i].Y())));
        pResolutionBtn->Connect(wxEVT_BUTTON, wxCommandEventHandler(CSelectResolutionDialog::OnResolutionButton), NULL, this);
        m_pSizer->Add(pResolutionBtn, 0, wxALIGN_CENTER | wxALL, 0);
    }
    for (auto iter : CEditorConfig::GetInstance()->GetResolution())
    {
        wxButton* pButtonResolution = new wxButton(this, wxID_ANY, wxString::Format(_T("%dx%d"), uint32_t(iter.X()), uint32_t(iter.Y())));
        pButtonResolution->Connect(wxEVT_BUTTON, wxCommandEventHandler(CSelectResolutionDialog::OnResolutionButton), NULL, this);
        m_pSizer->Add(pButtonResolution, 0, wxALIGN_CENTER | wxALL, 0);
    }
    wxButton* pCustomResolution = new wxButton(this, wxID_ANY, _T("Custom"));
    m_pSizer->Add(pCustomResolution, 0, wxALIGN_CENTER | wxALL, 0);

    pCustomResolution->Connect(wxEVT_BUTTON, wxCommandEventHandler(CSelectResolutionDialog::OnButtonCustom), NULL, this);
    this->Layout();
}

void CSelectResolutionDialog::OnClose(wxCloseEvent& /*event*/)
{
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    if (!m_bSetResolution)
    {
        pMainFrame->GetAuiToolBarPerformPtr()->ToggleTool(ID_ViewAllBtn, false);
    }
    pMainFrame->GetAuiToolBarPerformPtr()->Refresh(false);
    EndModal(0);
}

void CSelectResolutionDialog::OnResolutionButton(wxCommandEvent& event)
{
    wxButton* pButton = dynamic_cast<wxButton*>(event.GetEventObject());
    wxString label = pButton->GetLabelText();
    std::vector<std::string> strArray;
    CStringHelper::GetInstance()->SplitString(label, _T("x"), strArray);
    SetResolution(atoi(strArray[0].c_str()), atoi(strArray[1].c_str()));
}
void CSelectResolutionDialog::SetResolution(uint32_t uWidth, uint32_t uHeight)
{
    m_bSetResolution = true;
    CRenderManager::GetInstance()->SetSimulateSize(uWidth, uHeight);
    CEditorSceneWindow* pViewScreen = m_pMainFrame->GetSceneWindow();
    pViewScreen->GetRenderWindow()->UseFBO(true);
    pViewScreen->SendSizeEvent(); // In order to call SetFBOViewPort, so force call a on size callback.
}

void CSelectResolutionDialog::OnButtonCustom(wxCommandEvent& /*event*/)
{
    CCustomResolutionDialog resolutionDlg(this, wxID_ANY, wxT("Custom Resolution"), wxPoint(0, 0), wxSize(230, 250), wxDEFAULT_DIALOG_STYLE);
    resolutionDlg.SetPosition(GetPosition());
    resolutionDlg.ShowModal();
    InitSelectResolutionDialog();
}