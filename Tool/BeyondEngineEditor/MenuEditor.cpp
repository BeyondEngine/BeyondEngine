#include "stdafx.h"
#include "MenuEditor.h"
#include "Component/Property/PropertyDescriptionBase.h"
#include "RandomColorPropertyDescription.h"

IMPLEMENT_DYNAMIC_CLASS(CMenuEditor, wxPGTextCtrlAndButtonEditor);

CMenuEditor::CMenuEditor()
{

}

CMenuEditor::~CMenuEditor()
{

}

wxPGWindowList CMenuEditor::CreateControls(wxPropertyGrid* propGrid,
    wxPGProperty* property,
    const wxPoint& pos,
    const wxSize& sz) const
{
    return super::CreateControls(propGrid, property, pos, sz);
}

bool CMenuEditor::OnEvent(wxPropertyGrid* propGrid,
    wxPGProperty* property,
    wxWindow* /*ctrl*/,
    wxEvent& event) const
{
    CPropertyDescriptionBase* pPropertyDescription = (CPropertyDescriptionBase*)(property->GetClientData());
    if (event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)
    {
        if (pPropertyDescription->GetType() == eRPT_RandomColor)
        {
            CRandomColorPropertyDescription* pRandomColorProperty = down_cast<CRandomColorPropertyDescription*>(pPropertyDescription);
            wxArrayString displayString;
            int nInitSelection = (int)pRandomColorProperty->GetColorType();
            if (!pRandomColorProperty->IsOnlyGradient())
            {
                displayString.Add("color");
                displayString.Add("random color");
            }
            if (!pRandomColorProperty->IsOnlyColor())
            {
                displayString.Add("spline");
                displayString.Add("random spline");
            }
            if (nInitSelection > (int)displayString.size())
            {
                nInitSelection = 0;
            }
            int nSelection = wxGetSingleChoiceIndex("选取类型", "选取类型", displayString, nInitSelection);
            if (nSelection != 0xFFFFFFFF)
            {
                wxPGProperty* pMinProperty = nullptr;
                pRandomColorProperty->SetColorType(ERandomColorType(nSelection));
                ERandomColorType realColorType = pRandomColorProperty->GetColorType();// may not be the same to nSelection.
                wxPGProperty* pMaxProperty = pRandomColorProperty->CreateRandomColorProperty(realColorType, pMinProperty);
                property->DeleteChildren();
                if (pMinProperty)
                {
                    property->InsertChild(-1, pMinProperty);
                }
                property->InsertChild(-1, pMaxProperty);
                propGrid->RefreshProperty(property);
                wxVariant tmp((int)realColorType);
                pRandomColorProperty->SetValue(tmp);
            }
        }
    }
    return true;
}

