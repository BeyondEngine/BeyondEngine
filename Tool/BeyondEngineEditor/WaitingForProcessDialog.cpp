#include "stdafx.h"
#include "WaitingForProcessDialog.h"
#include "wx/timer.h"

#define TIMER_ID 1000

wxBEGIN_EVENT_TABLE(CWaitingForProcessDialog, CEditDialogBase)
EVT_IDLE(CWaitingForProcessDialog::OnIdle)
EVT_TIMER(TIMER_ID, CWaitingForProcessDialog::OnTimer)
wxEND_EVENT_TABLE()

CWaitingForProcessDialog::CWaitingForProcessDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : CEditDialogBase(parent, id, title, pos, size, style, name)
    , m_pGaugeCtrl(NULL)
    , m_pProgressText(NULL)
    , m_pTitle(NULL)
    , m_pFileName(NULL)
    , m_bCouldClose(false)
    , m_bWaitingClose(false)
{
    m_pTimer = new wxTimer(this, TIMER_ID);
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    m_pGaugeCtrl = new wxGauge(this, wxID_ANY, 100);
    m_pTitle = new wxStaticText(this, wxID_ANY, wxEmptyString);
    m_pFileName = new wxStaticText(this, wxID_ANY, wxEmptyString);
    m_pProgressText = new wxStaticText(this, wxID_ANY, wxEmptyString);
    m_pUseTime = new wxStaticText(this, wxID_ANY, wxEmptyString);
    pSizer->Add(m_pTitle, 0, wxALIGN_CENTER | wxALL, 0);
    pSizer->Add(m_pUseTime, 0, wxALIGN_CENTER | wxALL, 0);
    pSizer->Add(m_pFileName, 0, wxGROW | wxALL, 0);
    pSizer->Add(m_pGaugeCtrl, 0, wxGROW | wxALL, 0);
    pSizer->Add(m_pProgressText, 1, wxALIGN_CENTER | wxALL, 0);
    SetSizer(pSizer);
    m_pGaugeCtrl->SetMinSize(wxSize(500, m_pGaugeCtrl->GetSize().y));
    Fit();
    CenterOnScreen();
}

CWaitingForProcessDialog::~CWaitingForProcessDialog()
{
}

void CWaitingForProcessDialog::SetTotalCount(uint32_t uCount)
{
    m_pGaugeCtrl->SetRange(uCount);
    m_pProgressText->SetLabel(wxString::Format(_T("%d/%d"), m_pGaugeCtrl->GetValue(), uCount));
}

void CWaitingForProcessDialog::SetCurrentCount(uint32_t uCount)
{
    if ((int)uCount != m_pGaugeCtrl->GetValue())
    {
        m_pGaugeCtrl->SetValue(uCount);
        m_pProgressText->SetLabel(wxString::Format(_T("%d/%d"), uCount, m_pGaugeCtrl->GetRange()));
    }
}

uint32_t CWaitingForProcessDialog::GetTotalCount()
{
    return m_pGaugeCtrl->GetRange();
}

void CWaitingForProcessDialog::SetTitle(wxString strTitle)
{
    m_pTitle->SetLabel(strTitle);
}

void CWaitingForProcessDialog::SetFileName(wxString strFile)
{
    m_pFileName->SetLabel(strFile);
}

void CWaitingForProcessDialog::SetTask(std::function<int()> task)
{
    m_task = task;
}

int CWaitingForProcessDialog::ShowModal()
{
    m_bWaitingClose = false;
    m_bCouldClose = false;
    if (m_task != nullptr)
    {
        m_task();
    }
    return __super::ShowModal();
}

void CWaitingForProcessDialog::EndModal(int retCode)
{
    __super::EndModal(retCode);
    m_task = nullptr;
}

void CWaitingForProcessDialog::OnIdle(wxIdleEvent& /*event*/)
{
    if (m_task != nullptr)
    {
        SetCurrentCount(m_task());
    }
    if (m_pGaugeCtrl->GetRange() == m_pGaugeCtrl->GetValue())
    {
        if (m_bShowUseTime)
        {
            m_pTimer->Stop();
        }
        if (!m_bWaitingClose)
        {
            m_bWaitingClose = true;
            std::thread t1([this](){
                std::this_thread::sleep_for(std::chrono::seconds(1));
                m_bCouldClose = true;
            });
            t1.detach();
        }
        if (IsModal() && m_bCouldClose)
        {
            EndModal(0);
        }
    }
}

void CWaitingForProcessDialog::OnTimer(wxTimerEvent& /*event*/)
{
    m_uUseTime++;
    m_pUseTime->SetLabel(wxString::Format(_T("%dÃë"), m_uUseTime));
}

uint32_t CWaitingForProcessDialog::GetToatalTime()
{
    return m_uUseTime;
}

void CWaitingForProcessDialog::ShowUseTime(bool bShowUseTime)
{
    m_bShowUseTime = bShowUseTime;
    if (m_bShowUseTime)
    {
        m_uUseTime = 0;
        m_pUseTime->SetLabel(wxString::Format(_T("%dÃë"), m_uUseTime));
        m_pTimer->Start(1000);
    }
    else
    {
        m_pUseTime->SetLabel("");
    }
}
