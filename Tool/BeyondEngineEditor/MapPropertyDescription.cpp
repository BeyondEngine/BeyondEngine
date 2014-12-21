#include "stdafx.h"
#include "MapPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "EngineEditor.h"
#include "ListPropertyDescription.h"
#include "Component/Component/ComponentProxyManager.h"
#include <wx/propgrid/propgrid.h>
#include "StringPropertyDescription.h"
#include "Component/ComponentPublic.h"
#include "MapElementPropertyDescription.h"

static const TString EMPTY_STRING = _T("Empty");

CMapPropertyDescription::CMapPropertyDescription(CSerializer* pSerializer)
    : super(eRPT_Map)
    , m_pKeyPropertyTemplate(NULL)
    , m_pValuePropertyTemplate(NULL)
{
    if (pSerializer != NULL)
    {
        EReflectPropertyType keyType;
        (*pSerializer) >> keyType;
        BEATS_ASSERT(keyType != eRPT_Ptr, _T("Key can't be ePT_Ptr! It's not implemented yet!"));
        m_pKeyPropertyTemplate = CComponentProxyManager::GetInstance()->CreateProperty(keyType, pSerializer);
        EReflectPropertyType valueType;
        (*pSerializer) >> valueType;
        m_pValuePropertyTemplate = CComponentProxyManager::GetInstance()->CreateProperty(valueType, pSerializer);
    }
    InitializeValue(EMPTY_STRING);
}

CMapPropertyDescription::CMapPropertyDescription(const CMapPropertyDescription& rRef)
    : super(rRef)
    , m_pKeyPropertyTemplate(rRef.m_pKeyPropertyTemplate->Clone(true))
    , m_pValuePropertyTemplate(rRef.m_pValuePropertyTemplate->Clone(true))
{
    InitializeValue(EMPTY_STRING);
}

CMapPropertyDescription::~CMapPropertyDescription()
{
    BEATS_SAFE_DELETE(m_pKeyPropertyTemplate);
    BEATS_SAFE_DELETE(m_pValuePropertyTemplate);
    DestroyValue<TString>();
}

wxPGProperty* CMapPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
    pProperty->SetClientData(this);
    if (GetBasicInfo()->m_bEditable)
    {
        pProperty->SetEditor(static_cast<CEngineEditor*>(wxApp::GetInstance())->GetPtrEditor());
    }
    TCHAR szName[64];
    GetCurrentName(szName);
    wxVariant var(szName);
    pProperty->SetValue(var);
    wxVariant defaultVar(EMPTY_STRING);
    pProperty->SetDefaultValue(defaultVar);
    pProperty->SetModifiedStatus(m_pChildren->size() > 0);
    return pProperty;
}

void CMapPropertyDescription::SetValue( wxVariant& value, bool bSaveValue/* = true*/ )
{
    TString strNewValue = value.GetString();
    SetValueWithType(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&strNewValue, eVT_SavedValue);
    }
}

bool CMapPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(TString*)pTargetValue != *(TString*)pSourceValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *(TString*)pSourceValue;
    }
    return bRet;
}

bool CMapPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    return bWithDefaultOrXML && m_pChildren->size() == 0;
}

void CMapPropertyDescription::Initialize()
{
    super::Initialize();
    for (uint32_t i = 0; i < m_pChildren->size(); ++i)
    {
        CPropertyDescriptionBase* pPropertyBase = m_pChildren->at(i);
        BEATS_ASSERT(pPropertyBase != NULL && pPropertyBase->GetChildren().size() == 2);
        pPropertyBase->GetChildren()[0]->Initialize();
        pPropertyBase->GetChildren()[1]->Initialize();
        pPropertyBase->Initialize();
    }
    ResetName();
}

void CMapPropertyDescription::Uninitialize()
{
    for (uint32_t i = 0; i < m_pChildren->size(); ++i)
    {
        CPropertyDescriptionBase* pPropertyBase = m_pChildren->at(i);
        BEATS_ASSERT(pPropertyBase != NULL && pPropertyBase->GetChildren().size() == 2);
        pPropertyBase->GetChildren()[0]->Uninitialize();
        pPropertyBase->GetChildren()[1]->Uninitialize();
        pPropertyBase->Uninitialize();
    }
    super::Uninitialize();
}

bool CMapPropertyDescription::IsContainerProperty()
{
    return true;
}

CPropertyDescriptionBase* CMapPropertyDescription::InsertChild(CPropertyDescriptionBase* pChild, uint32_t uPreIndex)
{
    if (pChild == NULL)
    {
        pChild = CreateMapElementProp();
    }
    super::InsertChild(pChild, uPreIndex);
    bool bSyncProperty = !CComponentInstanceManager::GetInstance()->IsInLoadingPhase() && GetOwner() && !GetOwner()->GetTemplateFlag();
    EReflectOperationType originalOperateType = EReflectOperationType::ChangeValue;
    CPropertyDescriptionBase* pOriginalProperty = CComponentProxyManager::GetInstance()->GetCurrReflectProperty(&originalOperateType);
    if (bSyncProperty)
    {
        CComponentProxyManager::GetInstance()->SetCurrReflectProperty(pChild, EReflectOperationType::AddChild);
    }
    ResetName();
    if (bSyncProperty)
    {
        CComponentProxyManager::GetInstance()->SetCurrReflectProperty(pOriginalProperty, originalOperateType);
    }
    return pChild;
}

CMapElementPropertyDescription* CMapPropertyDescription::CreateMapElementProp()
{
   SBasicPropertyInfo basicInfo = *(GetBasicInfo().Get());
    basicInfo.m_catalog.clear();
    basicInfo.m_displayName.assign(_T("Map_Child"));
    basicInfo.m_variableName.assign(_T("Map_Child"));
    basicInfo.m_bEditable = true;

    CMapElementPropertyDescription* pRet = new CMapElementPropertyDescription(nullptr);
    pRet->SetBasicInfo(basicInfo);

    basicInfo.m_bEditable = true;// Only label can't be changed.
    CPropertyDescriptionBase* pKey = m_pKeyPropertyTemplate->Clone(false);
    basicInfo.m_displayName.assign(_T("Key"));
    basicInfo.m_variableName.assign(_T("Key"));
    pKey->SetBasicInfo(basicInfo);

    CPropertyDescriptionBase* pValue = m_pValuePropertyTemplate->Clone(false);
    basicInfo.m_displayName.assign(_T("Value"));
    basicInfo.m_variableName.assign(_T("Value"));
    pValue->SetBasicInfo(basicInfo);

    pRet->InsertChild(pKey);
    pRet->InsertChild(pValue);
    pRet->SetOwner(this->GetOwner());
    if (IsInitialized())
    {
        pKey->Initialize();
        pValue->Initialize();
        pRet->Initialize();
    }
    return pRet;
}

CPropertyDescriptionBase* CMapPropertyDescription::GetKeyPropertyTemplate() const
{
    return m_pKeyPropertyTemplate;
}

CPropertyDescriptionBase* CMapPropertyDescription::GetValuePropertyTemplate() const
{
    return m_pValuePropertyTemplate;
}

bool CMapPropertyDescription::RemoveChild(CPropertyDescriptionBase* pProperty, bool bDelete)
{
    BEATS_ASSERT((*m_pChildren).size() > 0 && pProperty != NULL);
    bool bSyncProperty = !CComponentInstanceManager::GetInstance()->IsInLoadingPhase() && GetOwner() && !GetOwner()->GetTemplateFlag();
    EReflectOperationType originalOperateType = EReflectOperationType::ChangeValue;
    CPropertyDescriptionBase* pOriginalProperty = CComponentProxyManager::GetInstance()->GetCurrReflectProperty(&originalOperateType);
    if (bSyncProperty)
    {
        CComponentProxyManager::GetInstance()->SetCurrReflectProperty(this, EReflectOperationType::RemoveChild);
        CSerializer& removeChildInfo = CComponentProxyManager::GetInstance()->GetRemoveChildInfo();
        removeChildInfo.Reset();
        removeChildInfo << false;
        SerializeContainerElementLocation(removeChildInfo, pProperty);
    }
    bool bRet = super::RemoveChild(pProperty, bDelete);
    if (bRet)
    {
        ResetName();
        if (bSyncProperty)
        {
            CComponentProxyManager::GetInstance()->SetCurrReflectProperty(pOriginalProperty, originalOperateType);
        }
    }
    BEATS_ASSERT(bRet, _T("Can't Find the property to delete!"));
    return bRet;
}

void CMapPropertyDescription::RemoveAllChild(bool bDelete)
{
    bool bSyncProperty = !CComponentInstanceManager::GetInstance()->IsInLoadingPhase() && GetOwner() && !GetOwner()->GetTemplateFlag();
    EReflectOperationType originalOperateType = EReflectOperationType::ChangeValue;
    CPropertyDescriptionBase* pOriginalProperty = CComponentProxyManager::GetInstance()->GetCurrReflectProperty(&originalOperateType);
    if (bSyncProperty)
    {
        CComponentProxyManager::GetInstance()->SetCurrReflectProperty(this, EReflectOperationType::RemoveChild);
        CSerializer& removeChildInfo = CComponentProxyManager::GetInstance()->GetRemoveChildInfo();
        removeChildInfo.Reset();
        removeChildInfo << true;
    }
    super::RemoveAllChild(bDelete);
    ResetName();
    if (bSyncProperty)
    {
        CComponentProxyManager::GetInstance()->SetCurrReflectProperty(pOriginalProperty, originalOperateType);
    }
}

void CMapPropertyDescription::ResetName()
{
    TCHAR szName[64];
    GetCurrentName(szName);
    wxVariant var(szName);
    SetValue(var, false);
}

void CMapPropertyDescription::GetCurrentName( TCHAR* pszName )
{
    if (m_pChildren->size() == 0)
    {
        _stprintf(pszName, EMPTY_STRING.c_str());
    }
    else
    {
        _stprintf(pszName, _T("%dx%s"), m_pChildren->size(), szPropertyTypeStr[(*m_pChildren)[0]->GetType()]);
    }
}

void CMapPropertyDescription::LoadFromXML(rapidxml::xml_node<>* pNode)
{
    super::LoadFromXML(pNode);
    rapidxml::xml_node<>* pVarElement = pNode->first_node("VariableNode");
    while (pVarElement != NULL)
    {
        int iVarType = 0;
        iVarType = atoi(pVarElement->first_attribute("Type")->value());
        BEATS_WARNING(iVarType == eRPT_MapElement, _T("UnMatch type of property!"));
        CPropertyDescriptionBase* pNewProperty = InsertChild(NULL);
        BEATS_ASSERT(pNewProperty != 0, _T("Create property failed when load from xml for list property description."));
        if (pNewProperty != NULL)
        {
            rapidxml::xml_node<>* pChildVarElement = pVarElement->first_node("VariableNode");
            pNewProperty->GetChildren()[0]->LoadFromXML(pChildVarElement);
            pChildVarElement = pChildVarElement->next_sibling("VariableNode");
            pNewProperty->GetChildren()[1]->LoadFromXML(pChildVarElement);
        }
        pVarElement = pVarElement->next_sibling("VariableNode");
    }
}

CPropertyDescriptionBase* CMapPropertyDescription::Clone(bool bCloneValue)
{
    CMapPropertyDescription* pNewProperty = static_cast<CMapPropertyDescription*>(super::Clone(bCloneValue));
    if (bCloneValue)
    {
        for (uint32_t i = 0; i < m_pChildren->size(); ++i)
        {
            CPropertyDescriptionBase* pPropertyBase = (*m_pChildren)[i];
            CPropertyDescriptionBase* pNewChildPropertyBase = pPropertyBase->Clone(true);
            pNewProperty->InsertChild(pNewChildPropertyBase);
        }
    }
    return pNewProperty;
}

CPropertyDescriptionBase* CMapPropertyDescription::CreateNewInstance()
{
    CMapPropertyDescription* pNewProperty = new CMapPropertyDescription(*this);
    return pNewProperty;
}

void CMapPropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    _tcscpy(pOut, ((TString*)GetValue(type))->c_str());
}

bool CMapPropertyDescription::GetValueByTChar(const TCHAR* /*pIn*/, void* /*pOutValue*/)
{
    // Do nothing.
    return true;
}

void CMapPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    serializer << m_pChildren->size();
    for (uint32_t i = 0; i < m_pChildren->size(); ++i)
    {
        (*m_pChildren)[i]->Serialize(serializer, eValueType);
    }
}

void CMapPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    RemoveAllChild();
    uint32_t uChildrenCount = 0;
    serializer >> uChildrenCount;
    for (uint32_t i = 0; i < uChildrenCount; ++i)
    {
        InsertChild(NULL);
        (*m_pChildren)[i]->Deserialize(serializer, eValueType);
    }
}

void CMapPropertyDescription::SetOwner(CComponentProxy* pOwner)
{
    super::SetOwner(pOwner);
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        m_pChildren->at(i)->SetOwner(pOwner);
    }
}

bool CMapPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& /*parameterUnit*/)
{
    return true;
}

void CMapPropertyDescription::SerializeContainerElementLocation(CSerializer& serializer, CPropertyDescriptionBase* pChildProperty)
{
    pChildProperty->GetChildren()[0]->Serialize(serializer); // Serialize the key value.
    // TODO: HACK: When the key is changed, we need to erase the old key and insert the new key, so we need both old and new value.
    // But when we get here, the current value of the key property has already changed.
    // So we promise a rule: the saved value indicate the old key and the current value indicate the new key.
    // That's why we need to call Save here, to send the current value(new key) to the saved value.
    pChildProperty->GetChildren()[0]->Save();
}
