#include "stdafx.h"
#include "EngineEditor.h"
#include "EngineProperGridManager.h"
#include "EnginePropertyGrid.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxy.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstanceManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentGraphic.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProjectDirectory.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProject.h"
#include "wxWidgetsPropertyBase.h"
#include "ComponentGraphics_GL.h"
#include "resource/ResourcePathManager.h"
#include "wx/html/helpfrm.h"
#include "ComponentFileTreeItemData.h"
#include <wx/treectrl.h>
#include "EditorMainFrame.h"


BEGIN_EVENT_TABLE(CEnginePropertyGirdManager, wxPropertyGridManager)
    EVT_PG_CHANGED(wxID_ANY, CEnginePropertyGirdManager::OnComponentPropertyChanged)
    EVT_PG_SELECTED (wxID_ANY, CEnginePropertyGirdManager::OnPropertyGridSelect)
END_EVENT_TABLE()

CEnginePropertyGirdManager::CEnginePropertyGirdManager()
    : m_bNeedUpdatePropertyGrid(false)
    , m_pProperty(NULL)
{
    
}

CEnginePropertyGirdManager::~CEnginePropertyGirdManager()
{
}

wxPropertyGrid* CEnginePropertyGirdManager::CreatePropertyGrid() const
{
    CEnginePropertyGrid* pGrid = new CEnginePropertyGrid();
    pGrid->ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_ALWAYS);
    pGrid->SetManager(const_cast<CEnginePropertyGirdManager*>(this));
    return pGrid;
}
    

void CEnginePropertyGirdManager::InsertComponentsInPropertyGrid( CComponentProxy* pComponent, wxPGProperty* pParent /*= NULL*/ )
{
    // Set Guid property.
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    bool bReference = pComponent->GetProxyId() != pComponent->GetId();
    if (pMainFrame->GetShowGuidId())
    {
        wxPGProperty* pPGProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
        pPGProperty->SetValue(wxString::Format(_T("0x%x"), pComponent->GetGuid()));
        pPGProperty->ChangeFlag(wxPG_PROP_READONLY, true);
        pPGProperty->SetName(_T("Guid"));
        pPGProperty->SetLabel(_T("Guid"));
        GetGrid()->GetState()->DoInsert(pParent, -1, pPGProperty);
        // Set Property for Id
        if (pComponent->GetId() != -1)
        {
            wxPGProperty* pPGProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
            pPGProperty->SetValue(wxString::Format(_T("%d"), pComponent->GetId()));
            pPGProperty->ChangeFlag(wxPG_PROP_READONLY, true);
            pPGProperty->SetName(_T("Id"));
            pPGProperty->SetLabel(_T("Id"));
            GetGrid()->GetState()->DoInsert(pParent, -1, pPGProperty);
        }
        if (bReference)
        {
            wxPGProperty* pPGProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
            pPGProperty->SetValue(wxString::Format(_T("%d"), pComponent->GetProxyId()));
            pPGProperty->ChangeFlag(wxPG_PROP_READONLY, true);
            pPGProperty->SetName(_T("ReferenceId"));
            pPGProperty->SetLabel(_T("ReferenceId"));
            GetGrid()->GetState()->DoInsert(pParent, -1, pPGProperty);
        }
    }
    InsertInPropertyGrid(*pComponent->GetPropertyPool(), pParent, bReference);
}

void CEnginePropertyGirdManager::InsertInPropertyGrid( const std::vector<CPropertyDescriptionBase*>& pProperties, wxPGProperty* pParent /*= NULL*/, bool bIsReference)
{
    for (size_t i = 0; i < pProperties.size(); ++i)
    {
        CWxwidgetsPropertyBase* pPropertyBase = const_cast<CWxwidgetsPropertyBase*>(static_cast<CWxwidgetsPropertyBase*>(pProperties[i]));
        if (!pPropertyBase->IsHide())
        {
            wxEnumProperty* pComboProperty = pPropertyBase->GetComboProperty();
            wxPGProperty* pPGProperty =  pComboProperty ? pPropertyBase->CreateComboProperty() : pPropertyBase->CreateWxProperty();
            BEATS_ASSERT(pPGProperty != NULL);
            BEATS_ASSERT(pParent != pPGProperty, _T("Can't insert a property in itself"));
            wxPGProperty* pRealParent = pParent;
            const TString& strCatalogName = pPropertyBase->GetBasicInfo()->m_catalog;
            if (strCatalogName.length() > 0)
            {
                wxPGProperty* pCatalogProperty = NULL;
                if (pParent != NULL)
                {
                    pCatalogProperty = pParent->GetPropertyByName(strCatalogName.c_str());
                }
                else
                {
                    pCatalogProperty = GetGrid()->GetProperty(strCatalogName.c_str());
                }
                if (pCatalogProperty == NULL)
                {
                    pCatalogProperty = new wxStringProperty(strCatalogName.c_str(), wxPG_LABEL, wxT("<composed>"));
                    GetGrid()->GetState()->DoInsert(pParent, -1, pCatalogProperty);
                    pCatalogProperty->SetBackgroundColour(0xFF777777);
                }
                pRealParent = pCatalogProperty;
            }
            pPGProperty->SetName(pPropertyBase->GetBasicInfo()->m_variableName);
            pPGProperty->SetLabel(pPropertyBase->GetBasicInfo()->m_displayName);
            pPGProperty->Enable(!bIsReference);
            pPGProperty->ChangeFlag(wxPG_PROP_READONLY, !pPropertyBase->GetBasicInfo()->m_bEditable || pPropertyBase->GetType() == eRPT_Ptr || pPropertyBase->GetType() == eRPT_List || pPropertyBase->GetType() == eRPT_Vec3F);
            pPGProperty->SetHelpString(pPropertyBase->GetBasicInfo()->m_tip);
            GetGrid()->GetState()->DoInsert(pRealParent, -1, pPGProperty);
            // This function can only be called after property be inserted to grid, or it will crash. It's a wxwidgets rule.
            pPGProperty->SetBackgroundColour(pPropertyBase->GetBasicInfo()->m_color);
            if (pPropertyBase->ShowChildrenInGrid())
            {
                InsertInPropertyGrid(pPropertyBase->GetChildren(), pPGProperty, bIsReference);
            }
        }
    }
    // Update the visibility after all the properties have been inserted in the grid.
    for (size_t i = 0; i < pProperties.size(); ++i)
    {
        CWxwidgetsPropertyBase* pPropertyBase = const_cast<CWxwidgetsPropertyBase*>(static_cast<CWxwidgetsPropertyBase*>(pProperties[i]));
        if (!pPropertyBase->IsHide())
        {
            UpdatePropertyVisiblity(pPropertyBase);
        }
    }
    Refresh();
}

void CEnginePropertyGirdManager::UpdatePropertyVisiblity(CWxwidgetsPropertyBase* pPropertyBase)
{
    // Do visible check logic.
    std::set<CWxwidgetsPropertyBase*>& effectProperties = pPropertyBase->GetEffectProperties();
    std::set<CWxwidgetsPropertyBase*>::iterator iter = effectProperties.begin();
    for (; iter != effectProperties.end(); ++iter)
    {
        bool bShouldBeVisible = (*iter)->CheckVisibleTrigger();
        wxPGProperty* pPGProperty = GetPGPropertyByBase(*iter);
        BEATS_ASSERT(pPGProperty != NULL, _T("Can't find property %s in data grid!"), (*iter)->GetBasicInfo()->m_displayName.c_str());
        if (pPGProperty != NULL)
        {
            pPGProperty->Hide(!bShouldBeVisible);
        }
    }
}

wxPGProperty* CEnginePropertyGirdManager::GetPGPropertyByBase(CPropertyDescriptionBase* pBase)
{
    wxPGProperty* pRet = NULL;
    for (wxPropertyGridIterator it = GetCurrentPage()->GetIterator(); !it.AtEnd(); ++it)
    {
        if (it.GetProperty()->GetClientData() == pBase)
        {
            pRet = it.GetProperty();
            break;
        }
    }
    return pRet;
}

void CEnginePropertyGirdManager::OnComponentPropertyChanged( wxPropertyGridEvent& event )
{
    wxPGProperty* pProperty = event.GetProperty();
    OnComponentPropertyChangedImpl(pProperty);
    event.Skip();
}

void CEnginePropertyGirdManager::OnComponentPropertyChangedImpl(wxPGProperty* pProperty)
{
    void* pClientData = pProperty->GetClientData();
    if (pClientData != NULL)
    {
        CWxwidgetsPropertyBase* pPropertyBase = static_cast<CWxwidgetsPropertyBase*>(pClientData);
        BEATS_ASSERT(pPropertyBase != NULL);
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

bool CEnginePropertyGirdManager::IsNeedUpdatePropertyGrid()
{
    return m_bNeedUpdatePropertyGrid;
}

void CEnginePropertyGirdManager::PreparePropertyDelete( wxPGProperty* pProperty )
{
    m_bNeedUpdatePropertyGrid = true;
    m_pProperty = pProperty;
}

void CEnginePropertyGirdManager::ExecutePropertyDelete()
{
    m_bNeedUpdatePropertyGrid = false;
    DeleteProperty(m_pProperty);
    m_pProperty = NULL;
}

void CEnginePropertyGirdManager::OnPropertyGridSelect(wxPropertyGridEvent& event)
{
    if (!HasFocus())
    {
        SetFocus();
    }
    event.Skip();
}
