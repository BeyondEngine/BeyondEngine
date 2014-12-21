#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_GRADIENTDIALOG_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_GRADIENTDIALOG_H__INCLUDE

#include "EditDialogBase.h"

class CGradientCtrl;
class CGradientDialog : public CEditDialogBase
{
    typedef CEditDialogBase super;
public:
    CGradientDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style =  wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~CGradientDialog();
    virtual int ShowModal();
    bool CheckPosIsOnly(float fPos);
    int GetPosCount();
    float GetPosByIndex(int nIndex);
    wxColor GetColorByIndex(int nIndex);
    CGradientCtrl* GetGradientCtrl() const;
    void Reset();
    void OnYesBtnClicked(wxCommandEvent& event);
    void OnCancelBtnClicked(wxCommandEvent& event);

private:
    CGradientCtrl* m_pGradientCtrl;
    std::vector<float> m_allCursorPosList;
};

#endif