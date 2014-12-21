#include "stdafx.h"
#include "SoundFileProperty.h"
#include "wx/object.h"
#include "EngineEditor.h"

IMPLEMENT_DYNAMIC_CLASS(CSoundFileProperty, wxFileProperty);
CSoundFileProperty::CSoundFileProperty(const wxString& label, const wxString& name, wxString value)
    : wxFileProperty(label, name, value)
{
}

CSoundFileProperty::~CSoundFileProperty()
{
}

wxPGEditorDialogAdapter* CSoundFileProperty::GetEditorDialog() const
{
    return NULL;
}

const wxPGEditor* CSoundFileProperty::DoGetEditorClass() const
{
    return static_cast<CEngineEditor*>(wxApp::GetInstance())->GetSoundFileEditor();
}
