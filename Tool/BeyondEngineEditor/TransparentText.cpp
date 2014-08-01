#include "stdafx.h"
#include "TransparentText.h"

CTransparentText::CTransparentText(wxWindow *parent,
                 wxWindowID id,
                 const wxString& label,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxString& name)
                 :wxStaticText(parent, id, label, pos, size, style, name)
{
}

CTransparentText::~CTransparentText()
{

}

WXHBRUSH CTransparentText::MSWControlColor(WXHDC pDC, WXHWND /*hWnd*/)
{
    wxColour fgColour = GetForegroundColour();
    ::SetTextColor((HDC)pDC, RGB(fgColour.Red(), fgColour.Green(), fgColour.Blue()));
    ::SetBkMode((HDC)pDC, TRANSPARENT);
    return (WXHBRUSH)GetStockObject(HOLLOW_BRUSH);
}

