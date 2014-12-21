#include "stdafx.h"
#include "wxPtrButtonEditor.h"
#include "Component/Component/ComponentProxyManager.h"
#include "Component/Component/ComponentProxy.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "PtrPropertyDescription.h"
#include "ListPropertyDescription.h"
#include "MapPropertyDescription.h"
#include "EditorMainFrame.h"
#include "EnginePropertyGrid.h"
#include "MapElementPropertyDescription.h"

IMPLEMENT_DYNAMIC_CLASS(wxPtrButtonEditor, wxPGTextCtrlEditor);

wxPtrButtonEditor::wxPtrButtonEditor()
{

}

wxPtrButtonEditor::~wxPtrButtonEditor()
{

}

wxPGWindowList wxPtrButtonEditor::CreateControls(wxPropertyGrid* propGrid,
    wxPGProperty* property,
    const wxPoint& pos,
    const wxSize& sz) const
{
    void* pClientData = property->GetClientData();
    BEATS_ASSERT(pClientData != NULL);
    CPropertyDescriptionBase* pPropertyDescription = static_cast<CPropertyDescriptionBase*>(pClientData);
    wxPGMultiButton* pMulButton = new wxPGMultiButton(propGrid, sz);
    if (pPropertyDescription->IsContainerProperty() && pPropertyDescription->GetType() != eRPT_MapElement)
    {
        pMulButton->Add(_T("+"));
        if (property->GetChildCount() > 0)
        {
            pMulButton->Add(_T("x"));
        }
    }
    else if (pPropertyDescription->GetType() == eRPT_Ptr)
    {
        BEATS_ASSERT(pPropertyDescription->GetType() == eRPT_Ptr);
        pMulButton->Add(pPropertyDescription->GetInstanceComponent() == NULL ? _T("+") : _T("x"));
    }
    AddDeleteButton(property, pMulButton);
    wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls(propGrid, property, pos, pMulButton->GetPrimarySize());
    pMulButton->Finalize(propGrid, pos);
    wndList.SetSecondary(pMulButton);
    return wndList;
}

bool wxPtrButtonEditor::OnEvent(wxPropertyGrid* propGrid,
                                wxPGProperty* property,
                                wxWindow* pClickButton,
                                wxEvent& event) const
{
    if (event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)
    {
        CWxwidgetsPropertyBase* pPropertyDescription = static_cast<CWxwidgetsPropertyBase*>(property->GetClientData());
        CEnginePropertyGridManager* pManager = static_cast<CEnginePropertyGridManager*>(static_cast<CEnginePropertyGrid*>(propGrid)->GetManager());
        if (pClickButton != NULL)
        {
            if (pClickButton->GetLabel().CmpNoCase(_T("+")) == 0)
            {
                if (pPropertyDescription->GetType() == eRPT_Ptr)
                {
                    CPtrPropertyDescription* pPtrPropertyDescription = static_cast<CPtrPropertyDescription*>(pPropertyDescription);
                    if (SelectDerivedInstanceInEditor(pPtrPropertyDescription))
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
                        pManager->InsertInPropertyGrid(*pCompBase->GetPropertyPool(), property);
                        pClickButton->SetLabel(_T("x"));
                        propGrid->Refresh();
                    }
                }
                else
                {
                    BEATS_ASSERT(pPropertyDescription->IsContainerProperty());

                    CPropertyDescriptionBase* pNewChild = nullptr;
                    if (pPropertyDescription->GetType() == eRPT_Map)
                    {
                        TString strUserInputKey = wxGetTextFromUser("Input a key value for the map property", "Key value input");
                        while (!strUserInputKey.empty())
                        {
                            bool bFound = false;
                            for (size_t i = 0; i < pPropertyDescription->GetChildren().size(); ++i)
                            {
                                CMapElementPropertyDescription* pMapElementProp = down_cast<CMapElementPropertyDescription*>(pPropertyDescription->GetChildren()[i]);
                                BEATS_ASSERT(pMapElementProp->GetChildren().size() == 2);
                                CPropertyDescriptionBase* pKeyValue = pMapElementProp->GetChildren()[0];
                                TCHAR szBuffer[MAX_PATH];
                                pKeyValue->GetValueAsChar(eVT_CurrentValue, szBuffer);
                                if (TString(szBuffer) == strUserInputKey)
                                {
                                    bFound = true;
                                    strUserInputKey = wxGetTextFromUser(wxString::Format("The key %s already exists!\nInput another key value for the map property", strUserInputKey.c_str()), "Key value already exists");
                                    break;
                                }
                            }
                            if (!bFound)
                            {
                                break;
                            }
                        }
                        if (!strUserInputKey.empty())
                        {
                            pNewChild = down_cast<CMapPropertyDescription*>(pPropertyDescription)->CreateMapElementProp();
                            BEATS_ASSERT(pNewChild->GetChildren().size() == 2);
                            bool bSetValueSuccess = pNewChild->GetChildren()[0]->GetValueByTChar(strUserInputKey.c_str(), pNewChild->GetChildren()[0]->GetValue(eVT_CurrentValue));
                            if (!bSetValueSuccess)
                            {
                                wxMessageBox(wxString::Format("该值 %s 无效，操作取消", strUserInputKey.c_str()));
                                pNewChild->Uninitialize();
                                BEATS_SAFE_DELETE(pNewChild);
                            }
                            pPropertyDescription->InsertChild(pNewChild);
                        }
                    }
                    else
                    {
                        pNewChild = pPropertyDescription->InsertChild(NULL);
                    }
                    if (pNewChild != NULL)
                    {
                        BEATS_ASSERT(pNewChild->IsInitialized());
                        std::vector<CPropertyDescriptionBase*> value;
                        value.push_back(pNewChild);
                        pManager->InsertInPropertyGrid(value, property);

                        char valueStr[256];
                        pPropertyDescription->GetValueAsChar(eVT_CurrentValue, valueStr);
                        property->SetValue(valueStr);
                        property->SetModifiedStatus(pPropertyDescription->GetChildren().size() > 0);
                        property->RecreateEditor();
                    }
                }
            }
            else if (pClickButton->GetLabel().CmpNoCase(_T("x")) == 0)
            {
                if (pPropertyDescription->GetType() == eRPT_Ptr)
                {
                    CPtrPropertyDescription* pPtrPropertyDescription = static_cast<CPtrPropertyDescription*>(pPropertyDescription);
                    wxString valueStr = property->GetValueAsString();
                    if (wxMessageBox(_T("确定要删除吗？"), _T("删除"), wxYES_NO) == wxYES)
                    {
                        pManager->RemovePropertyFromGrid(pPtrPropertyDescription, true);
                        bool bDeleteInstance = pPtrPropertyDescription->DestroyInstance();
                        BEYONDENGINE_UNUSED_PARAM(bDeleteInstance);
                        BEATS_ASSERT(bDeleteInstance);
                        // Destroy instance may cause the value changed, so we fetch it again.
                        char szTmp[MAX_PATH];
                        pPtrPropertyDescription->GetValueAsChar(eVT_CurrentValue, szTmp);
                        valueStr = szTmp;
                        pPtrPropertyDescription->RemoveAllChild(false);
                        pClickButton->SetLabel(_T("+"));
                        wxVariant newValue(valueStr);
                        pPtrPropertyDescription->SetValue(newValue, false);
                        property->SetValue(newValue);
                        propGrid->Refresh();
                    }
                }
                else
                {
                    BEATS_ASSERT(pPropertyDescription->IsContainerProperty());
                    if (wxMessageBox(_T("确定要删除吗？"), _T("删除"), wxYES_NO) == wxYES)
                    {
                        pPropertyDescription->RemoveAllChild();
                        pManager->RemovePropertyFromGrid(pPropertyDescription, true);
                    }
                    char valueStr[256];
                    pPropertyDescription->GetValueAsChar(eVT_CurrentValue, valueStr);
                    property->SetValue(valueStr);
                    property->SetModifiedStatus(pPropertyDescription->GetChildren().size() > 0);
                    property->RecreateEditor();
                }
            }
            else if (pClickButton->GetLabel().CmpNoCase(_T("-")) == 0)
            {
                DeleteEvent(propGrid, property);
            }
            else
            {
                BEATS_ASSERT(false, "Never reach here!");
            }
        }
    }
    return wxPGTextCtrlEditor::OnEvent(propGrid, property, pClickButton, event);
}

bool wxPtrButtonEditor::SelectDerivedInstanceInEditor(CPtrPropertyDescription* pPtrPropertyDescription)
{
    uint32_t uPtrGuid = pPtrPropertyDescription->GetPtrGuid();
    std::set<uint32_t> instanceClassGuid;
    CComponentProxyManager::GetInstance()->QueryDerivedClass(uPtrGuid, instanceClassGuid, true);
    bool bRet = instanceClassGuid.size() == 0;
    if (!bRet)
    {
        BEATS_ASSERT(instanceClassGuid.find(uPtrGuid) == instanceClassGuid.end());
        instanceClassGuid.insert(uPtrGuid);
        std::map<TString, uint32_t> mapSort; // Sort by the map.
        for (auto guid: instanceClassGuid)
        {
            CComponentBase* pBase = CComponentProxyManager::GetInstance()->GetComponentTemplate(guid);
            if (pBase != NULL)//If it is NULL, it must be an abstract class.
            {
                mapSort[pBase->GetClassStr()] = pBase->GetGuid();
            }
            BEATS_ASSERT(pBase != NULL ||
                CComponentProxyManager::GetInstance()->GetAbstractComponentNameMap().find(guid) != CComponentProxyManager::GetInstance()->GetAbstractComponentNameMap().end(),
                _T("We can't get a template component with guid %d while it can't be found in abstract class map!"), guid);
        }
        wxPGChoices choice;
        for (auto iter = mapSort.begin(); iter != mapSort.end(); ++iter)
        {
            choice.Add(iter->first, iter->second);
        }
        wxString strSelectItem = ::wxGetSingleChoice(wxT("TypeChoice"), wxT("Caption"), choice.GetLabels(),
            NULL, wxDefaultCoord, wxDefaultCoord, false, wxCHOICE_WIDTH, wxCHOICE_HEIGHT);
        if (!strSelectItem.empty())
        {
            int nSelectIndex = choice.Index(strSelectItem);
            uint32_t uDerivedGuid = choice.GetValue(nSelectIndex);
            if (uDerivedGuid != pPtrPropertyDescription->GetPtrGuid())
            {
                pPtrPropertyDescription->SetDerivedGuid(uDerivedGuid);
            }
            uint32_t uInstanceGuid = pPtrPropertyDescription->GetDerivedGuid() == 0 ? pPtrPropertyDescription->GetPtrGuid() : pPtrPropertyDescription->GetDerivedGuid();
            bRet = CComponentProxyManager::GetInstance()->GetComponentTemplate(uInstanceGuid) != NULL;
            if (!bRet)
            {
                MessageBox(BEYONDENGINE_HWND, _T("Can't Instance a abstract class! Maybe this property is a abstract class and it has no derived class!"), _T("Abstact class instance!"), MB_OK);
            }
        }
    }
    return bRet;
}

