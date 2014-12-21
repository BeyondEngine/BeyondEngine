#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_WAITINGFORPROCESSDIALOG_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_WAITINGFORPROCESSDIALOG_H__INCLUDE

#include "EditDialogBase.h"

class CWaitingForProcessDialog : public CEditDialogBase
{
    typedef CEditDialogBase super;
public:
    CWaitingForProcessDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style =  wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~CWaitingForProcessDialog();
    virtual int ShowModal() override;
    virtual void EndModal(int retCode) override;
    void OnIdle(wxIdleEvent& event);
    void OnTimer(wxTimerEvent& event);
    void SetTotalCount(uint32_t uCount);
    void SetTitle(wxString strTitle);
    void ShowUseTime(bool bShowUseTime);
    void SetFileName(wxString strFile);
    uint32_t GetTotalCount();
    void SetCurrentCount(uint32_t uCount);
    void SetTask(std::function<int()> task);
    uint32_t GetToatalTime();

private:
    bool m_bWaitingClose;
    bool m_bCouldClose;
    bool m_bShowUseTime = false;
    uint32_t m_uUseTime = 0;
    std::function<int()> m_task;
    wxGauge* m_pGaugeCtrl;
    wxStaticText* m_pTitle;
    wxStaticText* m_pFileName;
    wxStaticText* m_pProgressText;
    wxStaticText* m_pUseTime = NULL;
    wxTimer* m_pTimer = NULL;
    wxDECLARE_EVENT_TABLE();
};

#endif