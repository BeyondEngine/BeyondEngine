#include "stdafx.h"
#include "wxSoundPreviewEditor.h"
#include "Audio/include/AudioEngine.h"

IMPLEMENT_DYNAMIC_CLASS(wxSoundPreviewEditor, wxPGTextCtrlEditor);
wxSoundPreviewEditor::wxSoundPreviewEditor()
{
}

wxSoundPreviewEditor::~wxSoundPreviewEditor()
{
}

wxPGWindowList wxSoundPreviewEditor::CreateControls( wxPropertyGrid* propGrid,
                                                 wxPGProperty* property,
                                                 const wxPoint& pos,
                                                 const wxSize& sz ) const
{
    wxPGMultiButton* buttons = new wxPGMultiButton( propGrid, sz );
    buttons->Add(_T("..."));
    buttons->Add(_T(">>"));
    AddDeleteButton(property, buttons);

    buttons->Finalize(propGrid, pos);

    wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls( propGrid, property, pos, buttons->GetPrimarySize() );
    ((wxTextCtrl*)wndList.m_primary)->SetEditable(false);
    wndList.SetSecondary( buttons );
    return wndList;
}

bool wxSoundPreviewEditor::OnEvent( wxPropertyGrid* propGrid,
                                wxPGProperty* property,
                                wxWindow* ctrl,
                                wxEvent& event ) const
{
    if (event.GetEventType() == wxEVT_BUTTON)
    {
        if (ctrl->GetLabel().CmpNoCase(_T("...")) == 0)
        {
            wxPGFileDialogAdapter* pFileDialog = new wxPGFileDialogAdapter();
            pFileDialog->ShowDialog(propGrid, property);
            delete pFileDialog;
        }
        else if (ctrl->GetLabel().CmpNoCase(_T("-")) == 0)
        {
            DeleteEvent(propGrid, property);
        }
        else if (ctrl->GetLabel().CmpNoCase(_T(">>")) == 0)
        {
            wxString valueStr = property->GetValueAsString();
            if (!valueStr.IsNull())
            {
                std::string strName = valueStr;
                CAudioEngine::Play2d(strName);
            }
        }
    }
    return true;
}
