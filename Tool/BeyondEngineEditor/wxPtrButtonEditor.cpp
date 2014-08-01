#include "stdafx.h"
#include "wxPtrButtonEditor.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxy.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "PtrPropertyDescription.h"
#include "ListPropertyDescription.h"
#include "MapPropertyDescription.h"
#include "EditorMainFrame.h"
#include "EnginePropertyGrid.h"

IMPLEMENT_DYNAMIC_CLASS(wxPtrButtonEditor, wxPGTextCtrlEditor);

wxPtrButtonEditor::wxPtrButtonEditor()
{

}

wxPtrButtonEditor::~wxPtrButtonEditor()
{

}

wxPGWindowList wxPtrButtonEditor::CreateControls( wxPropertyGrid* propGrid,
                                                 wxPGProperty* property,
                                                 const wxPoint& pos,
                                                 const wxSize& sz ) const
{
    wxPGMultiButton* buttons = new wxPGMultiButton( propGrid, sz );
    void* pClientData = property->GetClientData();
    BEATS_ASSERT(pClientData != NULL);
    CPropertyDescriptionBase* pPropertyDescription = static_cast<CPropertyDescriptionBase*>(pClientData);
    if (pPropertyDescription->GetType() == eRPT_Ptr)
    {
        CPtrPropertyDescription* pPtrProperty = static_cast<CPtrPropertyDescription*>(pPropertyDescription);
        buttons->Add( pPtrProperty->GetInstanceComponent() == NULL ? _T("+") : _T("-"));
    }
    else if (pPropertyDescription->IsContainerProperty())
    {
        buttons->Add(_T("+"));
        if (property->GetChildCount() > 0)
        {
            buttons->Add(_T("-"));
        }
    }
    CPropertyDescriptionBase* pParent = pPropertyDescription->GetParent();
    // it is a list element.
    if (pParent && pParent->IsContainerProperty())
    {
        buttons->Add(_T("x"));
    }
    wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls( propGrid, property, pos, buttons->GetPrimarySize() );

    buttons->Finalize(propGrid, pos);

    wndList.SetSecondary( buttons );
    return wndList;
}

bool wxPtrButtonEditor::OnEvent( wxPropertyGrid* propGrid,
                                wxPGProperty* property,
                                wxWindow* ctrl,
                                wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        wxPGMultiButton* buttons = (wxPGMultiButton*) propGrid->GetEditorControlSecondary();
        CWxwidgetsPropertyBase* pPropertyDescription = static_cast<CWxwidgetsPropertyBase*>(property->GetClientData());
        CEnginePropertyGirdManager* pManager = static_cast<CEnginePropertyGirdManager*>(static_cast<CEnginePropertyGrid*>(propGrid)->GetManager());
        const wxWindow* pButton = NULL;
        for (size_t i = 0; i < buttons->GetCount(); ++i)
        {
            if (event.GetId() == buttons->GetButtonId(i))
            {
                pButton = buttons->GetButton(i);
                break;
            }
        }
        if (pButton != NULL)
        {
            if (pPropertyDescription->GetType() == eRPT_Ptr)
            {
                CPtrPropertyDescription* pPtrPropertyDescription = static_cast<CPtrPropertyDescription*>(pPropertyDescription);
                wxString valueStr = property->GetValueAsString();
                bool bValueChanged = false;
                if (pButton->GetLabel().CmpNoCase(_T("+")) == 0)
                {
                    bValueChanged = SelectDerivedInstanceInEditor(pPtrPropertyDescription);
                    if (bValueChanged)
                    {
                        bool bCreateInstance = pPtrPropertyDescription->CreateInstance();
                        BEYONDENGINE_UNUSED_PARAM(bCreateInstance);
                        BEATS_ASSERT(bCreateInstance);
                        CComponentProxy* pCompBase = static_cast<CComponentProxy*>(pPtrPropertyDescription->GetInstanceComponent());
                        BEATS_ASSERT(pCompBase != NULL);
                        BEATS_ASSERT(pManager != NULL);
                        TString* pStrValue = (TString*)pPtrPropertyDescription->GetValue(eVT_CurrentValue);
                        property->SetValueFromString(pStrValue->c_str());
                        pManager->RefreshProperty(property);
                        pManager->InsertComponentsInPropertyGrid(pCompBase, property);
                        buttons->GetButton(0)->SetLabel(_T("-"));
                        valueStr.insert(0, _T("+"));
                    }
                }
                else if (pButton->GetLabel().CmpNoCase(_T("-")) == 0)
                {
                    property->DeleteChildren();
                    bool bDeleteInstance = pPtrPropertyDescription->DestroyInstance(true);
                    bDeleteInstance;
                    BEATS_ASSERT(bDeleteInstance);
                    // Destroy instance may cause the value changed, so we fetch it again.
                    char szTmp[MAX_PATH];
                    pPtrPropertyDescription->GetValueAsChar(eVT_CurrentValue, szTmp);
                    valueStr = szTmp;
                    pPtrPropertyDescription->GetChildren().clear();
                    buttons->GetButton(0)->SetLabel(_T("+"));
                    bValueChanged = true;
                }
                if (bValueChanged)
                {
                    wxVariant newValue(valueStr);
                    pPtrPropertyDescription->SetValue(newValue, false);
                    property->SetValue(newValue);
                    propGrid->Refresh();
                }
            }
            if (pPropertyDescription->IsContainerProperty())
            {
                if (pButton->GetLabel().CmpNoCase(_T("+")) == 0)
                {
                    CPropertyDescriptionBase* pNewChild = pPropertyDescription->AddChild(NULL);
                    if (pNewChild != NULL)
                    {
                        std::vector<CPropertyDescriptionBase*> value;
                        value.push_back(pNewChild);
                        pManager->InsertInPropertyGrid(value, property);
                    }
                }
                else if (pButton->GetLabel().CmpNoCase(_T("-")) == 0)
                {
                    pPropertyDescription->DeleteAllChild();
                    property->DeleteChildren();
                }
                char valueStr[256];
                pPropertyDescription->GetValueAsChar(eVT_CurrentValue, valueStr);
                property->SetValue(valueStr);
                property->SetModifiedStatus(pPropertyDescription->GetChildrenCount() > 0);
                property->RecreateEditor();
            }

            if (pButton->GetLabel().CmpNoCase(_T("x")) == 0)
            {
                BEATS_ASSERT(pPropertyDescription->GetParent() != NULL);
                CPropertyDescriptionBase* pParent = pPropertyDescription->GetParent();
                pParent->DeleteChild(pPropertyDescription);
                // NOTICE: We have deleted this already!
                pPropertyDescription = NULL;

                property->SetClientData(NULL);
                char valueStr[256];
                pParent->GetValueAsChar(eVT_CurrentValue, valueStr);
                property->GetParent()->SetValue(valueStr);
                //TODO: I can't refresh property here, because we are trying to delete property of which callback we are in.
                pManager->PreparePropertyDelete(property);
            }
            return true;
        }
    }
    return wxPGTextCtrlEditor::OnEvent(propGrid, property, ctrl, event);
}

bool wxPtrButtonEditor::SelectDerivedInstanceInEditor(CPtrPropertyDescription* pPtrPropertyDescription)
{
    size_t ptrGuid = pPtrPropertyDescription->GetPtrGuid();
    std::vector<size_t> instanceClassGuid;
    CComponentProxyManager::GetInstance()->QueryDerivedClass(ptrGuid, instanceClassGuid, true);
    bool bRet = instanceClassGuid.size() == 0;
    if (!bRet)
    {
        instanceClassGuid.insert(instanceClassGuid.begin(), ptrGuid);
        wxPGChoices choice;
        for (auto i : instanceClassGuid)
        {
            CComponentBase* pBase = CComponentProxyManager::GetInstance()->GetComponentTemplate(i);
            if (pBase != NULL)//If it is NULL, it must be an abstract class.
            {
                choice.Add(pBase->GetClassStr(), pBase->GetGuid());
            }
            BEATS_ASSERT(pBase != NULL || 
                CComponentProxyManager::GetInstance()->GetAbstractComponentNameMap().find(i) != CComponentProxyManager::GetInstance()->GetAbstractComponentNameMap().end(),
                _T("We can't get a template component with guid %d while it can't be found in abstract class map!"), i);
        }
        wxString strSelectItem = ::wxGetSingleChoice(wxT("TypeChoice"), wxT("Caption"), choice.GetLabels(),
            NULL, wxDefaultCoord, wxDefaultCoord, false, wxCHOICE_WIDTH, wxCHOICE_HEIGHT);
        if ( !strSelectItem.empty() )
        {
            int nSelectIndex = choice.Index(strSelectItem);
            size_t uDerivedGuid = choice.GetValue(nSelectIndex);
            if (uDerivedGuid != pPtrPropertyDescription->GetPtrGuid())
            {
                pPtrPropertyDescription->SetDerivedGuid(uDerivedGuid);
            }
            if (bRet)
            {
                size_t uInstanceGuid = pPtrPropertyDescription->GetDerivedGuid() == 0 ? pPtrPropertyDescription->GetPtrGuid() : pPtrPropertyDescription->GetDerivedGuid();
                bRet = CComponentProxyManager::GetInstance()->GetComponentTemplate(uInstanceGuid) != NULL;
                if (!bRet)
                {
                    MessageBox(NULL, _T("Can't Instance a abstract class! Maybe this property is a abstract class and it has no derived class!"), _T("Abstact class instance!"), MB_OK);
                }
            }
        }
    }
    return bRet;
}

