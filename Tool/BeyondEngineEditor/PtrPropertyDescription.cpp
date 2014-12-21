#include "stdafx.h"
#include "PtrPropertyDescription.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "EngineEditor.h"
#include "EngineProperGridManager.h"
#include "Component/Component/ComponentInstance.h"
#include "Component/Component/ComponentProxy.h"
#include "Component/Component/ComponentProxyManager.h"
#include "Component/ComponentPublic.h"

#define POINTER_SPLIT_SYMBOL _T("@")

CPtrPropertyDescription::CPtrPropertyDescription(CSerializer* pSerializer)
    : super(eRPT_Ptr)
    , m_bCanBeCloned(true)
    , m_uComponentGuid(0)
    , m_uDerivedGuid(0)
    , m_pInstance(NULL)
    , m_bHasInstance(false)
{
    if (pSerializer != NULL)
    { 
        (*pSerializer) >> m_uComponentGuid;
        TString emptyStr = _T("");
        InitializeValue(emptyStr);
    }
}

CPtrPropertyDescription::CPtrPropertyDescription(const CPtrPropertyDescription& rRef)
    : super(rRef)
    , m_bCanBeCloned(rRef.m_bCanBeCloned)
    , m_uComponentGuid(rRef.m_uComponentGuid)
    , m_uDerivedGuid(rRef.m_uDerivedGuid)
    , m_pInstance(NULL)
    , m_bHasInstance(false)
{
    TString emptyStr = _T("");
    InitializeValue(emptyStr);
}

CPtrPropertyDescription::~CPtrPropertyDescription()
{
    DestroyInstance();
    DestroyValue<TString>();
}

bool CPtrPropertyDescription::AnalyseUIParameterImpl( const std::vector<TString>& paramUnit)
{
    for (uint32_t i = 0; i < paramUnit.size(); ++i)
    {
        TString parameter = paramUnit[i];
        std::vector<TString> result;
        CStringHelper::GetInstance()->SplitString(parameter.c_str(), PROPERTY_KEYWORD_SPLIT_STR, result);
        BEATS_ASSERT(result.size() == 2);
        if (_tcsicmp(result[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            parameter = result[1].c_str();
            m_bHasInstance = parameter[0] == _T('+');
        }
        else if (_tcsicmp(result[0].c_str(), UIParameterAttrStr[eUIPAT_CloneAble]) == 0)
        {
            m_bCanBeCloned = _tcsicmp(result[1].c_str(), _T("true")) == 0;
        }
    }
    return true;
}

wxPGProperty* CPtrPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL, ((TString*)m_valueArray[eVT_CurrentValue])->c_str());
    wxVariant defaultValue(((TString*)m_valueArray[eVT_DefaultValue])->c_str());
    pProperty->SetDefaultValue(defaultValue);
    pProperty->SetClientData(this);
    pProperty->SetModifiedStatus(GetInstanceComponent() != NULL);
    pProperty->SetEditor(static_cast<CEngineEditor*>(wxApp::GetInstance())->GetPtrEditor());
    return pProperty;
}

void CPtrPropertyDescription::SetValue( wxVariant& value , bool bSaveValue)
{
    TString bNewValue = value.GetString();
    SetValueWithType(&bNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&bNewValue, eVT_SavedValue);
    }
}

bool CPtrPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    TString* pStrValue = (TString*)pSourceValue;
    bool bRet = *(TString*)pTargetValue != *pStrValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *pStrValue;
    }
    return bRet;
}

uint32_t CPtrPropertyDescription::GetPtrGuid() const
{
    return m_uComponentGuid;
}

void CPtrPropertyDescription::SetPtrGuid(uint32_t uGuid)
{
    m_uComponentGuid = uGuid;
}

void CPtrPropertyDescription::SetDerivedGuid(uint32_t uDerivedGuid)
{
    m_uDerivedGuid = uDerivedGuid;
}

uint32_t CPtrPropertyDescription::GetDerivedGuid() const
{
    return m_uDerivedGuid;
}

CComponentProxy* CPtrPropertyDescription::GetInstanceComponent() const
{
    return m_pInstance;
}

bool CPtrPropertyDescription::CreateInstance(bool bCallInitFunc/* = true*/)
{
    bool bRet = false;
    if ( m_pInstance == NULL)
    {
        uint32_t uInstanceGuid = m_uDerivedGuid == 0 ? m_uComponentGuid : m_uDerivedGuid;
        m_pInstance = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->CreateComponent(uInstanceGuid, false, true, 0xFFFFFFFF, true, NULL, bCallInitFunc));
        m_pInstance->SetPtrPropertyOwner(this);
        const std::vector<CPropertyDescriptionBase*>* propertyPool = m_pInstance->GetPropertyPool();
        m_bHasInstance = true;
        for (uint32_t i = 0; i < propertyPool->size(); ++i)
        {
            InsertChild((*propertyPool)[i]);
        }
        UpdateDisplayString(uInstanceGuid);
        bRet = true;
    }

    return bRet;
}

bool CPtrPropertyDescription::DestroyInstance()
{
    bool bRet = false;
    m_bHasInstance = false;
    if (m_pInstance != NULL)
    {
        CComponentInstance* pHost = m_pInstance->GetHostComponent();
        if (pHost != nullptr)
        {
            BEATS_ASSERT(pHost->GetProxyComponent() == m_pInstance);
            if (pHost->IsInitialized())
            {
                pHost->Uninitialize();
            }
#ifdef _DEBUG
            pHost->m_bReflectPropertyCheck = true;
#endif
            BEATS_ASSERT(m_pInstance->GetPtrPropertyOwner() == this);
            BEATS_SAFE_DELETE(pHost);//m_pInstance is deleted here!
            m_pInstance = NULL;
        }
        else
        {
            if (m_pInstance->IsInitialized())
            {
                m_pInstance->Uninitialize();
            }
            BEATS_SAFE_DELETE(m_pInstance);
        }
        m_pChildren->clear();
        SetDerivedGuid(0);
        if (IsInitialized())
        {
            UpdateDisplayString(m_uComponentGuid);
        }
        bRet = true;
    }
    return bRet;
}

bool CPtrPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = false;
    if (bWithDefaultOrXML)
    {
        bRet = m_pInstance == NULL;
    }
    else
    {
        bRet = m_bHasInstance ? m_pInstance != NULL : m_pInstance == NULL;
    }
    return bRet;
}

void CPtrPropertyDescription::LoadFromXML(rapidxml::xml_node<>* pNode)
{
    super::LoadFromXML(pNode);
    TString* pStrValue = (TString*)m_valueArray[eVT_CurrentValue];
    m_bHasInstance = pStrValue->length() > 1 && pStrValue->at(0) ==_T('+');
    if (m_bHasInstance)
    {
        int nPos = CStringHelper::GetInstance()->FindFirstString(pStrValue->c_str(), POINTER_SPLIT_SYMBOL, false);
        const TCHAR* pszValueString = &(pStrValue->c_str()[nPos + _tcslen(POINTER_SPLIT_SYMBOL)]);
        TCHAR* pEndChar = NULL;
        int nRadix = 16;
        _set_errno(0);
        uint32_t uDerivedValue = _tcstoul(pszValueString, &pEndChar, nRadix);
        BEATS_ASSERT(_tcslen(pEndChar) == 0, _T("Read uint from string %s error, stop at %s"), pszValueString, pEndChar);
        BEATS_ASSERT(errno == 0, _T("Call _tcstoul failed! string %s radix: %d"), pszValueString, nRadix);
        if (uDerivedValue != m_uComponentGuid)
        {
            m_uDerivedGuid = uDerivedValue;
        }
        if (CreateInstance(false))
        {
            GetInstanceComponent()->LoadFromXML(pNode);
        }
    }
}

void CPtrPropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    const TCHAR* pValue = ((TString*)m_valueArray[type])->c_str();
    _tcscpy(pOut, pValue);
}

bool CPtrPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    ((TString*)pOutValue)->assign(pIn);
    return true;
}

CPropertyDescriptionBase* CPtrPropertyDescription::Clone(bool bCloneValue)
{
    // Notice: Should m_bCanBeCloned work in CComponentProxy? That's a question, For now, we only make it work in CComponentInstance, since designers will copy + paste components in editor.
    // They want all ptr property can be cloned.
    //if (CComponentInstanceManager::GetInstance()->IsInClonePhase())
    //{
    //    bCloneValue = m_bCanBeCloned;
    //}
    CPtrPropertyDescription* pNewProperty = static_cast<CPtrPropertyDescription*>(super::Clone(bCloneValue));
    if (m_pInstance != NULL && bCloneValue)
    {
        pNewProperty->m_pInstance = static_cast<CComponentProxy*>(m_pInstance->Clone(true, NULL, 0xFFFFFFFF));
        pNewProperty->m_pInstance->SetPtrPropertyOwner(pNewProperty);
        const std::vector<CPropertyDescriptionBase*>* pNewPropertyPool = pNewProperty->m_pInstance->GetPropertyPool();
        for (uint32_t i = 0; i < pNewPropertyPool->size(); ++i)
        {
            pNewProperty->InsertChild(pNewPropertyPool->at(i));
        }
    }
    return pNewProperty;
}

CPropertyDescriptionBase* CPtrPropertyDescription::CreateNewInstance()
{
    CPtrPropertyDescription* pNewProperty = new CPtrPropertyDescription(*this);
    return pNewProperty;
}

void CPtrPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    serializer << (bool)(m_pInstance != NULL);
    serializer << m_bCanBeCloned;
    if (m_pInstance != NULL)
    {
        if (CComponentProxyManager::GetInstance()->GetCurrUpdateProxy() != nullptr || CComponentProxyManager::GetInstance()->GetCurrReflectProperty() != nullptr)
        {
            BEATS_ASSERT(!CComponentProxyManager::GetInstance()->IsExporting());
            serializer << (uint32_t)(m_pInstance->GetHostComponent());
        }
        else
        {
            m_pInstance->ExportDataToHost(serializer, eValueType);
        }
    }
}

void CPtrPropertyDescription::Deserialize(CSerializer& serializer, EValueType /*eValueType*/ /*= eVT_CurrentValue*/)
{
    BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetCurrUpdateProxy() == nullptr, "This will never happen!");
    bool bHasInstance = false;
    serializer >> bHasInstance;
    serializer >> m_bCanBeCloned;
    if (bHasInstance)
    {
        if (m_pInstance == NULL)
        {
            uint32_t uHostComponent;
            serializer >> uHostComponent;
            CComponentProxy* pProxy = ((CComponentInstance*)(uHostComponent))->GetProxyComponent();
            m_pInstance = down_cast<CComponentProxy*>(pProxy->Clone(true, nullptr, 0xFFFFFFFF));
            m_pInstance->SetPtrPropertyOwner(this);
            const std::vector<CPropertyDescriptionBase*>* propertyPool = m_pInstance->GetPropertyPool();
            for (uint32_t i = 0; i < propertyPool->size(); ++i)
            {
                InsertChild((*propertyPool)[i]);
            }
            m_bHasInstance = true;
            UpdateDisplayString(m_pInstance->GetGuid(), false);
        }
        else
        {
            BEATS_ASSERT(false, "Never reach here! I can't image why we can reach here, what for?");
        }
    }
    else
    {
        if (m_pInstance != NULL)
        {
            DestroyInstance();
        }
    }
}

void CPtrPropertyDescription::Initialize()
{
    super::Initialize();
    // If we are creating a brand new ptrproperty, we need to initialize the string value.
    // If we are cloning from another ptrproperty, the value is already assigned.
    TString* pDefaultValue = (TString*)m_valueArray[eVT_DefaultValue];
    if (pDefaultValue->length() == 0)
    {
        UpdateDisplayString(m_uComponentGuid, false); //Don't trigger the reflect logic since it doesn't make sense.
    }
    if (m_pInstance != NULL)
    {
        // Different ptr property may share same instance, so we should check if the instance is already initialized.
        if (!m_pInstance->IsInitialized())
        {
            m_pInstance->Initialize();
        }
    }
}

void CPtrPropertyDescription::Uninitialize()
{
    if (m_pInstance != nullptr && m_pInstance->IsInitialized())
    {
        m_pInstance->Uninitialize();
    }
    super::Uninitialize();
}

uint32_t CPtrPropertyDescription::HACK_GetPtrReflectGuid() const
{
    uint32_t uRet = GetDerivedGuid();
    if (uRet == 0)
    {
        uRet = GetPtrGuid();
    }
    return uRet;
}

void CPtrPropertyDescription::HACK_InformPtrPropertyToDeleteInstance()
{
    if (m_pInstance)
    {
        m_pInstance->SetHostComponent(nullptr);
        DestroyInstance();
    }
}

void CPtrPropertyDescription::UpdateDisplayString(uint32_t uComponentGuid, bool bUpdateHostComponent/* = true*/)
{
    EReflectOperationType reflectOperateType = EReflectOperationType::ChangeValue;
    CPropertyDescriptionBase* pCurrReflectProperty = CComponentProxyManager::GetInstance()->GetCurrReflectProperty(&reflectOperateType);
    CComponentProxyManager::GetInstance()->SetCurrReflectProperty(nullptr, EReflectOperationType::ChangeValue);
    TString strComponentName = CComponentProxyManager::GetInstance()->QueryComponentName(uComponentGuid);
    BEATS_ASSERT(strComponentName.length() > 0, _T("Can't Find the component name of GUID: 0x%x"), uComponentGuid);
    wxString value = wxString::Format(_T("%s%s%s0x%x"), m_bHasInstance ? _T("+") : _T(""), strComponentName.c_str(), POINTER_SPLIT_SYMBOL, uComponentGuid);
    TString valueStr(value);
    for (uint32_t i = eVT_DefaultValue; i < eVT_Count; ++i)
    {
        if (bUpdateHostComponent)
        {
            SetValueWithType(&valueStr, (EValueType)i);
        }
        else
        {
            CopyValue(&valueStr, m_valueArray[(EValueType)i]);
        }
    }
    CComponentProxyManager::GetInstance()->SetCurrReflectProperty(pCurrReflectProperty, reflectOperateType); // Restore.
}