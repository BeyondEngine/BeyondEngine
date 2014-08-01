#include "stdafx.h"
#include "EditDialogBase.h"

CEditDialogBase::CEditDialogBase(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : wxDialog(parent, id, title, pos, size, style, name)
{

}

CEditDialogBase::~CEditDialogBase()
{

}

void CEditDialogBase::LanguageSwitch()
{

}
