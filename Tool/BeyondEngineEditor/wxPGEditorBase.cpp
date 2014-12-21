#include "stdafx.h"
#include "wxPGEditorBase.h"
#include "Component/Property/PropertyDescriptionBase.h"
#include "wxwidgetsPropertyBase.h"
#include "EngineProperGridManager.h"
#include "EnginePropertyGrid.h"
#include "EngineEditor.h"

wxPGEditorBase::wxPGEditorBase()
{
    static_cast<CEngineEditor*>(wxApp::GetInstance())->RegisterPGEditor(this);
}

wxPGEditorBase::~wxPGEditorBase()
{
    static_cast<CEngineEditor*>(wxApp::GetInstance())->UnRegisterPGEditor(this);
}

void wxPGEditorBase::AddDeleteButton(wxPGProperty* property, wxPGMultiButton* pMulButton) const
{
    void* pClientData = property->GetClientData();
    BEATS_ASSERT(pClientData != NULL);
    CPropertyDescriptionBase* pPropertyDescription = static_cast<CPropertyDescriptionBase*>(pClientData);
    CPropertyDescriptionBase* pParent = pPropertyDescription->GetParent();
    bool bContainerElement = pParent && pParent->IsContainerProperty() && pParent->GetType() != eRPT_MapElement;
    if (bContainerElement)
    {
        pMulButton->Add(_T("-"));
    }
}

void wxPGEditorBase::DeleteEvent(wxPropertyGrid* propGrid, wxPGProperty* property) const
{
    CWxwidgetsPropertyBase* pPropertyDescription = static_cast<CWxwidgetsPropertyBase*>(property->GetClientData());
    CEnginePropertyGridManager* pManager = static_cast<CEnginePropertyGridManager*>(static_cast<CEnginePropertyGrid*>(propGrid)->GetManager());
    BEATS_ASSERT(pPropertyDescription->GetParent()->IsContainerProperty());
    if (wxMessageBox(_T("È·¶¨ÒªÉ¾³ýÂð£¿"), _T("É¾³ý"), wxYES_NO) == wxYES)
    {
        BEATS_ASSERT(pPropertyDescription->GetParent() != NULL);
        CPropertyDescriptionBase* pParent = pPropertyDescription->GetParent();
        pParent->RemoveChild(pPropertyDescription);
        // NOTICE: We have deleted this already!
        pPropertyDescription = NULL;

        property->SetClientData(NULL);
        char valueStr[256];
        pParent->GetValueAsChar(eVT_CurrentValue, valueStr);
        property->GetParent()->SetValue(valueStr);
        //TODO: I can't refresh property here, because we are trying to delete property of which callback we are in.
        pManager->PreparePropertyDelete(property);
    }
}
