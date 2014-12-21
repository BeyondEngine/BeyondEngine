#include "stdafx.h"
#include "EngineEditor.h"
#include "EngineProperGridManager.h"
#include "EnginePropertyGrid.h"
#include "Component/Component/ComponentProxy.h"
#include "Component/Component/ComponentInstance.h"
#include "Component/Component/ComponentInstanceManager.h"
#include "Component/Component/ComponentGraphic.h"
#include "Component/Component/ComponentProjectDirectory.h"
#include "Component/Component/ComponentProject.h"
#include "wxWidgetsPropertyBase.h"
#include "ComponentGraphics_GL.h"
#include "resource/ResourceManager.h"
#include "wx/html/helpfrm.h"
#include "ComponentFileTreeItemData.h"
#include <wx/treectrl.h>
#include "EditorMainFrame.h"
#include "ChangeValueRecord.h"
#include "OperationRecordManager.h"

BEGIN_EVENT_TABLE(CEnginePropertyGridManager, wxPropertyGridManager)
    EVT_PG_CHANGED(wxID_ANY, CEnginePropertyGridManager::OnComponentPropertyChanged)
    EVT_PG_SELECTED (wxID_ANY, CEnginePropertyGridManager::OnPropertyGridSelect)
    EVT_PG_ITEM_EXPANDED(wxID_ANY, CEnginePropertyGridManager::OnPropertyExpand)
    EVT_PG_ITEM_COLLAPSED(wxID_ANY, CEnginePropertyGridManager::OnPropertyCollapsed)
END_EVENT_TABLE()

CEnginePropertyGridManager::CEnginePropertyGridManager()
    : m_bNeedUpdatePropertyGrid(false)
    , m_pPropertyToDelete(NULL)
{
    
}

CEnginePropertyGridManager::~CEnginePropertyGridManager()
{
}

wxPropertyGrid* CEnginePropertyGridManager::CreatePropertyGrid() const
{
    CEnginePropertyGrid* pGrid = new CEnginePropertyGrid();
    pGrid->ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_ALWAYS);
    pGrid->SetManager(const_cast<CEnginePropertyGridManager*>(this));
    return pGrid;
}

void CEnginePropertyGridManager::InsertComponentsInPropertyGrid( CComponentProxy* pComponent)
{
    // Set Guid property.
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    if (pMainFrame->GetShowGuidId())
    {
        wxPGProperty* pPGProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
        pPGProperty->SetValue(wxString::Format(_T("0x%x"), pComponent->GetGuid()));
        pPGProperty->ChangeFlag(wxPG_PROP_READONLY, true);
        pPGProperty->SetName(_T("Guid"));
        pPGProperty->SetLabel(_T("Guid"));
        GetGrid()->GetState()->DoInsert(nullptr, -1, pPGProperty);
        // Set Property for Id
        if (pComponent->GetId() != -1)
        {
            wxPGProperty* pPGProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
            pPGProperty->SetValue(wxString::Format(_T("%d"), pComponent->GetId()));
            pPGProperty->ChangeFlag(wxPG_PROP_READONLY, true);
            pPGProperty->SetName(_T("Id"));
            pPGProperty->SetLabel(_T("Id"));
            GetGrid()->GetState()->DoInsert(nullptr, -1, pPGProperty);
        }
        if (pComponent->GetId() != -1)
        {
            wxPGProperty* pPGProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
            pPGProperty->SetValue(wxString::Format(_T("%s"), pComponent->GetClassStr()));
            pPGProperty->ChangeFlag(wxPG_PROP_READONLY, true);
            pPGProperty->SetName(_T("Class"));
            pPGProperty->SetLabel(_T("Class"));
            GetGrid()->GetState()->DoInsert(nullptr, -1, pPGProperty);
        }
    }
    m_propertyMap.clear();
    InsertInPropertyGrid(*pComponent->GetPropertyPool(), nullptr, false);
    Refresh();
}

void CEnginePropertyGridManager::RefreshPropertyInGrid(CPropertyDescriptionBase* pPropertyDesc)
{
    if (m_propertyMap.find(pPropertyDesc) != m_propertyMap.end())
    {
        wxPGProperty* pParent = m_propertyMap[pPropertyDesc]->GetParent();
        int32_t nIndex = m_propertyMap[pPropertyDesc]->GetIndexInParent();
        RemovePropertyFromGrid(pPropertyDesc);
        InsertInPropertyGrid(pPropertyDesc, pParent, nIndex);
    }
}

void CEnginePropertyGridManager::InsertInPropertyGrid( const std::vector<CPropertyDescriptionBase*>& properties, wxPGProperty* pParent /*= NULL*/, bool bRefresh/* = false*/)
{
    if (pParent == nullptr && properties.size() > 0)
    {
        m_pCurrentPropertyOwner = nullptr;
        CPropertyDescriptionBase* pPropertyBase = properties[0];
        while (pPropertyBase->GetParent() != nullptr)
        {
            pPropertyBase = pPropertyBase->GetParent();
        }
        if (pPropertyBase->GetOwner() && pPropertyBase->GetOwner()->GetId() != 0xFFFFFFFF)
        {
            m_pCurrentPropertyOwner = pPropertyBase->GetOwner();
        }
    }
    for (uint32_t i = 0; i < properties.size(); ++i)
    {
        CWxwidgetsPropertyBase* pPropertyBase = const_cast<CWxwidgetsPropertyBase*>(static_cast<CWxwidgetsPropertyBase*>(properties[i]));
        if (!pPropertyBase->IsHide())
        {
            InsertInPropertyGrid(pPropertyBase, pParent, -1, bRefresh);
        }
    }
    // Update the visibility after all the properties have been inserted in the grid.
    for (uint32_t i = 0; i < properties.size(); ++i)
    {
        CWxwidgetsPropertyBase* pPropertyBase = const_cast<CWxwidgetsPropertyBase*>(static_cast<CWxwidgetsPropertyBase*>(properties[i]));
        if (!pPropertyBase->IsHide())
        {
            UpdatePropertyVisiblity(pPropertyBase);
        }
    }
    if (bRefresh)
    {
        Refresh();
    }
}

wxPGProperty* CEnginePropertyGridManager::InsertInPropertyGrid(CPropertyDescriptionBase* pPropertyDesc, wxPGProperty* pParent, int nIndex, bool bRefresh)
{
    wxPGProperty* pPGProperty = nullptr;
    CWxwidgetsPropertyBase* pPropertyBase = const_cast<CWxwidgetsPropertyBase*>(static_cast<CWxwidgetsPropertyBase*>(pPropertyDesc));
    if (!pPropertyBase->IsHide())
    {
        wxEnumProperty* pComboProperty = pPropertyBase->GetComboProperty();
        pPGProperty = pComboProperty ? pPropertyBase->CreateComboProperty() : pPropertyBase->CreateWxProperty();
        BEATS_ASSERT(pPGProperty != NULL);
        BEATS_ASSERT(pParent != pPGProperty, _T("Can't insert a property in itself"));
        const wxPGEditor* pEditor = pPGProperty->GetEditorClass();
        std::set<const wxPGEditor*> editorRegisterSet = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetEditorRegisterSet();
        if (pPropertyBase->GetBasicInfo()->m_bEditable && pPropertyBase->GetParent() != nullptr && pPropertyBase->GetParent()->IsContainerProperty() && editorRegisterSet.find(pEditor) == editorRegisterSet.end() && pPropertyDesc->GetType() != eRPT_Enum)
        {
            pPGProperty->SetEditor(static_cast<CEngineEditor*>(wxApp::GetInstance())->GetPtrEditor());
        }
        wxPGProperty* pRealParent = pParent;
        const wxString strCatalogName = wxString::FromUTF8(pPropertyBase->GetBasicInfo()->m_catalog.c_str());
        if (strCatalogName.length() > 0)
        {
            wxPGProperty* pCatalogProperty = NULL;
            if (pParent != NULL)
            {
                pCatalogProperty = pParent->GetPropertyByName(strCatalogName);
            }
            else
            {
                pCatalogProperty = GetGrid()->GetProperty(strCatalogName);
            }
            if (pCatalogProperty == NULL)
            {
                pCatalogProperty = new wxStringProperty(strCatalogName, wxPG_LABEL, wxT("<composed>"));
                GetGrid()->GetState()->DoInsert(pParent, -1, pCatalogProperty);
                pCatalogProperty->ChangeFlag(wxPG_PROP_READONLY, true);
                pCatalogProperty->SetBackgroundColour(0xFF777777, 0);
            }
            pRealParent = pCatalogProperty;
            GetGrid()->GetState()->DoCollapse(pCatalogProperty);
        }
        pPGProperty->SetName(wxString::FromUTF8(pPropertyBase->GetBasicInfo()->m_variableName.c_str()));
        wxString strDisplayName = pPropertyBase->GetBasicInfo()->m_displayName;
        if (pPropertyBase->GetParent() && strDisplayName == "List_Child" || strDisplayName == "Map_Child")
        {
            uint32_t uIndex = pPropertyBase->GetParent()->GetChildIndex(pPropertyBase);
            strDisplayName.append(wxString::Format("_%d", uIndex));
        }
        pPGProperty->SetLabel(wxString::FromUTF8(strDisplayName));
        pPGProperty->Enable(true);
        pPGProperty->ChangeFlag(wxPG_PROP_READONLY, !pPropertyBase->GetBasicInfo()->m_bEditable || pPropertyBase->GetType() == eRPT_Ptr || pPropertyBase->GetType() == eRPT_List || pPropertyBase->GetType() == eRPT_Vec3F || pPropertyBase->GetType() == eRPT_Vec2F || pPropertyBase->GetType() == eRPT_Vec4F || pPropertyBase->GetType() == eRPT_RandomValue);
        pPGProperty->SetHelpString(wxString::FromUTF8(pPropertyBase->GetBasicInfo()->m_tip.c_str()));
        BEATS_ASSERT(m_propertyMap.find(pPropertyBase) == m_propertyMap.end());
        m_propertyMap[pPropertyBase] = pPGProperty;
        GetGrid()->GetState()->DoInsert(pRealParent, nIndex, pPGProperty);
        if (pPropertyBase->GetValueImage().IsOk())
        {
            wxBitmap bitmap(pPropertyBase->GetValueImage());
            pPGProperty->SetValueImage(bitmap);
        }
        // This function can only be called after property be inserted to grid, or it will crash. It's a wxwidgets rule.
        wxColor selfDefineBackGroundColor = pPropertyBase->GetBasicInfo()->m_color;
        bool bUsePtrColor = selfDefineBackGroundColor == 0xFFFFFFFF && pPropertyBase->GetType() == eRPT_Ptr;
        if (bUsePtrColor)
        {
            bool bIsFirstPtr = true;
            CPropertyDescriptionBase* pParent = pPropertyBase->GetParent();
            while (pParent != nullptr)
            {
                if (pParent->GetType() == eRPT_Ptr)
                {
                    bIsFirstPtr = false;
                    break;
                }
                pParent = pParent->GetParent();
            }
            if (bIsFirstPtr)
            {
                selfDefineBackGroundColor = 0xFF64A05F;
            }
        }
        pPGProperty->SetBackgroundColour(selfDefineBackGroundColor, 0);
        if (pPropertyBase->ShowChildrenInGrid())
        {
            InsertInPropertyGrid(pPropertyBase->GetChildren(), pPGProperty, bRefresh);
        }
        if (pPropertyBase->IsExpanded())
        {
            GetGrid()->GetState()->DoExpand(pPGProperty);
        }
        else
        {
            GetGrid()->GetState()->DoCollapse(pPGProperty);
        }
        if (m_viewStartPosMap.find(m_pCurrentPropertyOwner) != m_viewStartPosMap.end())
        {
            GetGrid()->SetScrollPos(wxVERTICAL, m_viewStartPosMap[m_pCurrentPropertyOwner]);
        }
    }
    return pPGProperty;
}

void CollectPropertyDescriptionFromRoot(wxPGProperty* pPGRoot, std::vector<CPropertyDescriptionBase*>& propList)
{
    propList.push_back((CPropertyDescriptionBase*)pPGRoot->GetClientData());
    for (size_t i = 0; i < pPGRoot->GetChildCount(); ++i)
    {
        CollectPropertyDescriptionFromRoot(pPGRoot->Item(i), propList);
    }
}

void CEnginePropertyGridManager::RemovePropertyFromGrid(CPropertyDescriptionBase* pPropertyDesc, bool OnlyRemoveChild/* = false*/)
{
    BEATS_ASSERT(pPropertyDesc != nullptr);
    BEATS_ASSERT(m_propertyMap.find(pPropertyDesc) != m_propertyMap.end());
    wxPGProperty* pPGProperty = m_propertyMap[pPropertyDesc];
    std::vector<CPropertyDescriptionBase*> descriptionList;
    CollectPropertyDescriptionFromRoot(pPGProperty, descriptionList);
    if (OnlyRemoveChild)
    {
        auto iter = std::find(descriptionList.begin(), descriptionList.end(), pPropertyDesc);
        BEATS_ASSERT(iter != descriptionList.end());
        descriptionList.erase(iter);
    }
    for (size_t i = 0; i < descriptionList.size(); ++i)
    {
        m_propertyMap.erase(descriptionList[i]);
    }
    if (OnlyRemoveChild)
    {
        pPGProperty->DeleteChildren();
    }
    else
    {
        GetGrid()->DeleteProperty(pPGProperty);
    }
}

void CEnginePropertyGridManager::UpdatePropertyVisiblity(CWxwidgetsPropertyBase* pPropertyBase)
{
    // Do visible check logic.
    std::set<CWxwidgetsPropertyBase*>& effectProperties = pPropertyBase->GetEffectProperties();
    std::set<CWxwidgetsPropertyBase*>::iterator iter = effectProperties.begin();
    for (; iter != effectProperties.end(); ++iter)
    {
        if (!(*iter)->IsHide())
        {
            bool bShouldBeVisible = (*iter)->CheckVisibleTrigger();
            wxPGProperty* pPGProperty = GetPGPropertyByBase(*iter);
            BEATS_ASSERT(pPGProperty != NULL, _T("Can't find property %s in data grid!"), CStringHelper::GetInstance()->Utf8ToString((*iter)->GetBasicInfo()->m_displayName.c_str()).c_str());
            if (pPGProperty != NULL)
            {
                pPGProperty->Hide(!bShouldBeVisible, wxPG_DONT_RECURSE);
            }
        }
    }
}

wxPGProperty* CEnginePropertyGridManager::GetPGPropertyByBase(CPropertyDescriptionBase* pBase)
{
    BEATS_ASSERT(pBase != nullptr);
    wxPGProperty* pRet = NULL;
    auto iter = m_propertyMap.find(pBase);
    if (iter != m_propertyMap.end())
    {
        pRet = iter->second;
        BEATS_ASSERT(pRet->GetClientData() == pBase);
    }
    return pRet;
}

void CEnginePropertyGridManager::ClearGrid()
{
    m_propertyMap.clear();
    ClearPage(0);
}

void CEnginePropertyGridManager::OnComponentPropertyChanged( wxPropertyGridEvent& event )
{
    wxPGProperty* pProperty = event.GetProperty();
    OnComponentPropertyChangedImpl(pProperty);
    event.Skip();
}

void CEnginePropertyGridManager::OnScrollChanged(int nNewPos)
{
    if (m_pCurrentPropertyOwner != nullptr)
    {
        m_viewStartPosMap[m_pCurrentPropertyOwner] = nNewPos;
    }
}

void CEnginePropertyGridManager::OnComponentPropertyChangedImpl(wxPGProperty* pProperty)
{
    void* pClientData = pProperty->GetClientData();
    if (pClientData != NULL)
    {
        CWxwidgetsPropertyBase* pPropertyBase = static_cast<CWxwidgetsPropertyBase*>(pClientData);
        BEATS_ASSERT(pPropertyBase != NULL && pPropertyBase->IsInitialized());
        CChangeValueRecord* pChangeValueRecord = COperationRecordManager::GetInstance()->RequestRecord<CChangeValueRecord>(EOperationRecordType::eORT_ChangeValue);
        pChangeValueRecord->SetPropertyDescription(pPropertyBase);
        pPropertyBase->Serialize(pChangeValueRecord->GetOldData(), eVT_CurrentValue);
        if (pPropertyBase->GetComboProperty() != NULL)
        {
            int nSelection = pProperty->GetValue().GetInteger();
            wxString label = pProperty->GetChoices().GetLabel(nSelection);
            pPropertyBase->GetValueByTChar(label, pPropertyBase->GetValue(eVT_CurrentValue));
            // Force update the host component, because we have set to the current value by GetValueByTChar.
            pPropertyBase->SetValueWithType(pPropertyBase->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
        }
        else
        {
            wxVariant var = pProperty->GetValue();
            pPropertyBase->SetValue(var, false);
        }
        pPropertyBase->Serialize(pChangeValueRecord->GetNewData(), eVT_CurrentValue);
        COperationRecordManager::GetInstance()->AppendRecord(pChangeValueRecord);
        CPropertyDescriptionBase* pRealProperty = pPropertyBase;
        while (pRealProperty->GetParent() != NULL && pRealProperty->GetOwner() == pRealProperty->GetParent()->GetOwner())
        {
            pRealProperty = pRealProperty->GetParent();
        }
        CComponentBase* pHostComponent = pRealProperty->GetOwner()->GetHostComponent();
        CEngineEditor* pEditer = static_cast<CEngineEditor*>(wxApp::GetInstance());
        pEditer->GetMainFrame()->OnComponentPropertyChange(pHostComponent);

        wxPGProperty* pCurProperty = pProperty;
        while (pCurProperty != NULL)
        {
            void* pCurClientData = pCurProperty->GetClientData();
            if (pCurClientData == NULL)
            {
                break;
            }
            CWxwidgetsPropertyBase* pCurPropertyBase = static_cast<CWxwidgetsPropertyBase*>(pCurClientData);
            bool bModified = !pCurPropertyBase->IsDataSame(true);
            pCurProperty->SetModifiedStatus(bModified);
            // HACK: I don't know how to update those property which needs to be update when its child is changed.
            if (pCurPropertyBase->GetType() == eRPT_Vec2F || pCurPropertyBase->GetType() == eRPT_Vec3F || pCurPropertyBase->GetType() == eRPT_Vec4F)
            {
                char tmp[128];
                pCurPropertyBase->GetValueAsChar(eVT_CurrentValue, tmp);
                pCurProperty->SetValue(tmp);
            }
            pCurProperty = pCurProperty->GetParent();
        }
        UpdatePropertyVisiblity(pPropertyBase);
        pProperty->RefreshEditor();
        Refresh();
    }
}

bool CEnginePropertyGridManager::IsNeedUpdatePropertyGrid()
{
    return m_bNeedUpdatePropertyGrid;
}

void CEnginePropertyGridManager::PreparePropertyDelete( wxPGProperty* pProperty )
{
    m_bNeedUpdatePropertyGrid = true;
    m_pPropertyToDelete = pProperty;
}

void CEnginePropertyGridManager::ExecutePropertyDelete()
{
    m_bNeedUpdatePropertyGrid = false;
    wxString label = m_pPropertyToDelete->GetLabel();
    wxPGProperty* pParent = m_pPropertyToDelete->GetParent();
    DeleteProperty(m_pPropertyToDelete);
    m_pPropertyToDelete = NULL;
    if (label.Find("List_Child") != -1)
    {
        BEATS_ASSERT(pParent != nullptr);
        for (uint32_t i = 0; i < pParent->GetChildCount(); ++i)
        {
            pParent->Item(i)->SetLabel(wxString::Format("List_Child_%d", i));
        }
    }
    if (label.Find("Map_Child") != -1)
    {
        BEATS_ASSERT(pParent != nullptr);
        for (uint32_t i = 0; i < pParent->GetChildCount(); ++i)
        {
            pParent->Item(i)->SetLabel(wxString::Format("Map_Child_%d", i));
        }
    }
}

void CEnginePropertyGridManager::OnPropertyGridSelect(wxPropertyGridEvent& event)
{
    if (!HasFocus())
    {
        SetFocus();
    }
    event.Skip();
}

void CEnginePropertyGridManager::OnPropertyExpand(wxPropertyGridEvent& event)
{
    wxPGProperty* pProperty = event.GetProperty();
    if (pProperty && pProperty->GetClientData())
    {
        ((CWxwidgetsPropertyBase*)pProperty->GetClientData())->SetExpandFlag(true);
    }
}

void CEnginePropertyGridManager::OnPropertyCollapsed(wxPropertyGridEvent& event)
{
    wxPGProperty* pProperty = event.GetProperty();
    if (pProperty && pProperty->GetClientData())
    {
        ((CWxwidgetsPropertyBase*)pProperty->GetClientData())->SetExpandFlag(false);
    }
}
