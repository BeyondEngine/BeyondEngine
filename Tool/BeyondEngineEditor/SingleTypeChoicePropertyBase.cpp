#include "stdafx.h"
#include "SingleTypeChoicePropertyBase.h"
#include "ConstantCurveProperty.h"
#include "EnginePropertyGrid.h"

#define INITIALSIZEX 1

wxSingleTypeChoiceDialogAdapterBase::wxSingleTypeChoiceDialogAdapterBase( const wxPGChoices& choices )
    : wxPGEditorDialogAdapter(), m_choices(choices)
{

}

bool wxSingleTypeChoiceDialogAdapterBase::DoShowDialog( wxPropertyGrid* WXUNUSED(propGrid),
                          wxPGProperty* property )
{

    CConstantCurveProperty*p = (CConstantCurveProperty*)property;
    wxString s = ::wxGetSingleChoice(wxT("TypeChoice"), wxT("Caption"), m_choices.GetLabels(), 
                                    NULL, wxDefaultCoord, wxDefaultCoord, 
                                    false, wxCHOICE_WIDTH, wxCHOICE_HEIGHT, 
                                    p->GetValueType());
    if ( !s.empty() )
    {
        int iType = 0;
        for (int i = 0; i < (int)m_choices.GetCount(); i++)
        {
            if (s == m_choices.GetLabel(i))
            {
                iType = i;
                break;
            }
        }
        p->SetValueType(iType);
    }
    ((CEnginePropertyGrid*)p->GetGrid())->FreeEditorCtrl();
    p->GetGrid()->ClearInternalFlag(wxPG_FL_IN_HANDLECUSTOMEDITOREVENT);
    p->GetGrid()->DoSelectProperty(NULL);
    p->GetGrid()->DoSelectProperty(p);
    return false;
}

CSingleTypeChoicePropertyBase::CSingleTypeChoicePropertyBase( const wxString& label,
                         const wxString& name, double value )
                         : wxFloatProperty(label, name, value), m_iType(0)
{
    m_iImageSizeX = INITIALSIZEX;
    m_choices.Add(wxT("Constant"));
    m_choices.Add(wxT("Curve"));
    m_choices.Add(wxT("ConstantRandom"));
    m_choices.Add(wxT("CurveRandom"));
}

CSingleTypeChoicePropertyBase::~CSingleTypeChoicePropertyBase()
{

}

const wxPGEditor* CSingleTypeChoicePropertyBase::DoGetEditorClass() const
{
    return wxPGEditor_TextCtrlAndButton;
}

wxPGEditorDialogAdapter* CSingleTypeChoicePropertyBase::GetEditorDialog() const
{
    return new wxSingleTypeChoiceDialogAdapterBase(m_choices);
}
