#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TRANSPARENTBGTEXT_TRANSPARENTTEXT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TRANSPARENTBGTEXT_TRANSPARENTTEXT_H__INCLUDE

#include "wx/msw/stattext.h"

class CTransparentText : public wxStaticText
{
public:
    CTransparentText(wxWindow *parent,
        wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxStaticTextNameStr);
    ~CTransparentText();

    virtual WXHBRUSH MSWControlColor(WXHDC pDC, WXHWND hWnd);
};
#endif