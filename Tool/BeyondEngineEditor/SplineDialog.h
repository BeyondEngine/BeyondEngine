#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SplineDialog_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SplineDialog_H__INCLUDE

#include "EditDialogBase.h"

class CSplineGLWindow;
class CSplineDialog : public CEditDialogBase
{
    typedef CEditDialogBase super;
public:
    CSplineDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style =  wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~CSplineDialog();
    virtual int ShowModal();
    void InitCtrls();

private:
    CSplineGLWindow* m_pSplineGLWindow;

    DECLARE_EVENT_TABLE()
};

#endif