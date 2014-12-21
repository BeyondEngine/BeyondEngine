#include "stdafx.h"
#include "RandomColorPropertyDescription.h"
#include "GradientColorPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "ColorPropertyDescription.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "wx/propgrid/property.h"
#include "RandomColor.h"
#include "wx/propgrid/props.h"
#include "EngineEditor.h"

CRandomColorPropertyDescription::CRandomColorPropertyDescription(CSerializer* pSerializer)
    : super(eRPT_RandomColor)
{
    CRandomColor randomColor;
    if (pSerializer != nullptr)
    {
        DeserializeVariable(randomColor, pSerializer, nullptr);
    }
    InitializeValue<CRandomColor>(randomColor);
}

CRandomColorPropertyDescription::CRandomColorPropertyDescription(const CRandomColorPropertyDescription& rRef)
    :super(rRef)
{
    m_colorType = rRef.m_colorType;
    switch (m_colorType)
    {
    case ERandomColorType::eRCT_ConstantColor:
        BEATS_ASSERT(rRef.m_pMaxColorProperty != nullptr);
        m_pMaxColorProperty = new CColorPropertyDescription(*rRef.m_pMaxColorProperty);
        InsertChild(m_pMaxColorProperty);
        break;
    case ERandomColorType::eRCT_RandomColor:
        m_pMaxColorProperty = new CColorPropertyDescription(*rRef.m_pMaxColorProperty);
        m_pMinColorProperty = new CColorPropertyDescription(*rRef.m_pMinColorProperty);
        InsertChild(m_pMaxColorProperty);
        InsertChild(m_pMinColorProperty);
        break;
    case ERandomColorType::eRCT_ConstantSpline:
        m_pMaxGradientColorProperty = new CGradientColorPropertyDescription(*rRef.m_pMaxGradientColorProperty);
        InsertChild(m_pMaxGradientColorProperty);
        break;
    case ERandomColorType::eRCT_RandomSpline:
        m_pMaxGradientColorProperty = new CGradientColorPropertyDescription(*rRef.m_pMaxGradientColorProperty);
        m_pMinGradientColorProperty = new CGradientColorPropertyDescription(*rRef.m_pMinGradientColorProperty);
        InsertChild(m_pMaxGradientColorProperty);
        InsertChild(m_pMinGradientColorProperty);
        break;
    default:
        break;
    }
    m_bOnlyGradient = rRef.m_bOnlyGradient;
    m_bOnlyColor = rRef.m_bOnlyColor;
    CRandomColor randomColor;
    InitializeValue<CRandomColor>(randomColor);
}

CRandomColorPropertyDescription::~CRandomColorPropertyDescription()
{
    BEATS_SAFE_DELETE_COMPONENT(m_pMinGradientColorProperty);
    BEATS_SAFE_DELETE_COMPONENT(m_pMaxGradientColorProperty);
    BEATS_SAFE_DELETE_COMPONENT(m_pMinColorProperty);
    BEATS_SAFE_DELETE_COMPONENT(m_pMaxColorProperty);
}

void CRandomColorPropertyDescription::Initialize()
{
    if (m_bOnlyColor)
    {
        if (m_colorType == ERandomColorType::eRCT_ConstantSpline)
        {
            m_colorType = ERandomColorType::eRCT_ConstantColor;
        }
        if (m_colorType == ERandomColorType::eRCT_RandomSpline)
        {
            m_colorType = ERandomColorType::eRCT_RandomSpline;
        }
    }
    if (m_bOnlyGradient)
    {
        if (m_colorType == ERandomColorType::eRCT_ConstantColor)
        {
            m_colorType = ERandomColorType::eRCT_ConstantSpline;
        }
        if (m_colorType == ERandomColorType::eRCT_RandomColor)
        {
            m_colorType = ERandomColorType::eRCT_RandomSpline;
        }
    }
    UpdatePropertyByType(m_colorType);
    if (m_pMinGradientColorProperty != nullptr)
    {
        m_pMinGradientColorProperty->Initialize();
    }
    if (m_pMaxGradientColorProperty != nullptr)
    {
        m_pMaxGradientColorProperty->Initialize();
    }
    if (m_pMinColorProperty != nullptr)
    {
        m_pMinColorProperty->Initialize();
    }
    if (m_pMaxColorProperty != nullptr)
    {
        m_pMaxColorProperty->Initialize();
    }

    super::Initialize();
}

CPropertyDescriptionBase* CRandomColorPropertyDescription::CreateNewInstance()
{
    return new CRandomColorPropertyDescription(*this);
}

bool CRandomColorPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    if (pSourceValue != pTargetValue)
    {
        CRandomColor* pSource = (CRandomColor*)(pSourceValue);
        CRandomColor* pTarget = (CRandomColor*)(pTargetValue);
        *pTarget = *pSource;
    }
    return true;
}

bool CRandomColorPropertyDescription::IsDataSame(bool /*bWithDefaultOrXML*/)
{
    return true;
}

void CRandomColorPropertyDescription::GetValueAsChar(EValueType type, char* pOut) const
{
    TString strRet;
    TCHAR szBuffer[MAX_PATH];
    _stprintf(szBuffer, "%d,", m_colorType);
    strRet.append(szBuffer);
    switch (m_colorType)
    {
    case ERandomColorType::eRCT_ConstantColor:
        BEATS_ASSERT(m_pMaxColorProperty != nullptr);
        m_pMaxColorProperty->GetValueAsChar(type, szBuffer);
        strRet.append(szBuffer);
        break;
    case ERandomColorType::eRCT_RandomColor:
        BEATS_ASSERT(m_pMaxColorProperty != nullptr);
        m_pMaxColorProperty->GetValueAsChar(type, szBuffer);
        strRet.append(szBuffer).append("@");
        BEATS_ASSERT(m_pMinColorProperty != nullptr);
        m_pMinColorProperty->GetValueAsChar(type, szBuffer);
        strRet.append(szBuffer);
        break;
    case ERandomColorType::eRCT_ConstantSpline:
        BEATS_ASSERT(m_pMaxGradientColorProperty != nullptr);
        strRet.append(m_pMaxGradientColorProperty->WriteToString());
        break;
    case ERandomColorType::eRCT_RandomSpline:
        BEATS_ASSERT(m_pMaxGradientColorProperty != nullptr);
        strRet.append(m_pMaxGradientColorProperty->WriteToString());
        strRet.append("#");
        BEATS_ASSERT(m_pMinGradientColorProperty != nullptr);
        strRet.append(m_pMinGradientColorProperty->WriteToString());
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
    _tcscpy(pOut, strRet.c_str());
}

bool CRandomColorPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* /*pOutValue*/)
{
    int nTypePos = CStringHelper::GetInstance()->FindFirstString(pIn, ",", false);
    BEATS_ASSERT(nTypePos > 0 && nTypePos != 0xFFFFFFFF);
    TCHAR szBuffer[64];
    _tcsncpy(szBuffer, pIn, nTypePos);
    szBuffer[nTypePos] = 0;
    const TCHAR* pszRealData = pIn;
    pszRealData += (nTypePos + 1);
    m_colorType = (ERandomColorType)_tstoi(szBuffer);
    UpdatePropertyByType(m_colorType);
    switch (m_colorType)
    {
    case ERandomColorType::eRCT_ConstantColor:
    {
        m_pMaxColorProperty->SetValueByString(pszRealData);
    }
    break;
    case ERandomColorType::eRCT_RandomColor:
    {
        std::vector<TString> strVec;
        CStringHelper::GetInstance()->SplitString(pszRealData, "@", strVec, true);
        m_pMaxColorProperty->SetValueByString(strVec[0].c_str());
        m_pMinColorProperty->SetValueByString(strVec[1].c_str());
    }
        break;
    case ERandomColorType::eRCT_ConstantSpline:
    {
        m_pMaxGradientColorProperty->SetValueByString(pszRealData);
    }
        break;
    case ERandomColorType::eRCT_RandomSpline:
    {
        std::vector<TString> strVec;
        CStringHelper::GetInstance()->SplitString(pszRealData, "#", strVec, true);
        m_pMaxGradientColorProperty->SetValueByString(strVec[0].c_str());
        m_pMinGradientColorProperty->SetValueByString(strVec[1].c_str());
    }
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
    return true;
}

void CRandomColorPropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType)
{
    serializer << ((int)m_colorType);
    UpdatePropertyByType(m_colorType);
    switch (m_colorType)
    {
    case ERandomColorType::eRCT_ConstantColor:
        m_pMaxColorProperty->Serialize(serializer, eValueType);
        break;
    case ERandomColorType::eRCT_RandomColor:
        m_pMaxColorProperty->Serialize(serializer, eValueType);
        m_pMinColorProperty->Serialize(serializer, eValueType);
        break;
    case ERandomColorType::eRCT_ConstantSpline:
        m_pMaxGradientColorProperty->Serialize(serializer, eValueType);
        break;
    case ERandomColorType::eRCT_RandomSpline:
        m_pMaxGradientColorProperty->Serialize(serializer, eValueType);
        m_pMinGradientColorProperty->Serialize(serializer, eValueType);
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
}

void CRandomColorPropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType)
{
    serializer >> ((int)m_colorType);
    UpdatePropertyByType(m_colorType);
    switch (m_colorType)
    {
    case ERandomColorType::eRCT_ConstantColor:
        m_pMaxColorProperty->Deserialize(serializer, eValueType);
        break;
    case ERandomColorType::eRCT_RandomColor:
        m_pMaxColorProperty->Deserialize(serializer, eValueType);
        m_pMinColorProperty->Deserialize(serializer, eValueType);
        break;
    case ERandomColorType::eRCT_ConstantSpline:
        m_pMaxGradientColorProperty->Deserialize(serializer, eValueType);
        break;
    case ERandomColorType::eRCT_RandomSpline:
        m_pMaxGradientColorProperty->Deserialize(serializer, eValueType);
        m_pMinGradientColorProperty->Deserialize(serializer, eValueType);
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
}

void CRandomColorPropertyDescription::SetValue(wxVariant& value, bool /*bSaveValue*/)
{
    m_colorType = (ERandomColorType)value.GetInteger();
    switch (m_colorType)
    {
    case ERandomColorType::eRCT_ConstantColor:
        m_pMaxColorProperty->SetValueWithType(m_pMaxColorProperty->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
            break;
    case ERandomColorType::eRCT_RandomColor:
        m_pMaxColorProperty->SetValueWithType(m_pMaxColorProperty->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
        m_pMinColorProperty->SetValueWithType(m_pMinColorProperty->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
        break;
    case ERandomColorType::eRCT_ConstantSpline:
        m_pMaxGradientColorProperty->SetValueWithType(m_pMaxGradientColorProperty->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
        break;
    case ERandomColorType::eRCT_RandomSpline:
        m_pMaxGradientColorProperty->SetValueWithType(m_pMaxGradientColorProperty->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
        m_pMinGradientColorProperty->SetValueWithType(m_pMinGradientColorProperty->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
}

bool CRandomColorPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit)
{
    std::vector<TString> cache;
    for (uint32_t i = 0; i < parameterUnit.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(parameterUnit[i].c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache);
        BEATS_ASSERT(cache.size() == 2);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_ColorOrGradientOnly]) == 0)
        {
            if (_tcsicmp(cache[1].c_str(), _T("true")) == 0)
            {
                m_bOnlyColor = true;
            }
            else
            {
                m_bOnlyGradient = true;
            }
            SetColorType(m_colorType);
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for float property %s"), cache[0].c_str());
        }
    }
    return true;
}

wxPGProperty* CRandomColorPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL, pszRandomColorTypeStr[(int)m_colorType]);
    pProperty->SetClientData(this);
    pProperty->SetEditor(static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMenuEditor());
    return pProperty;
}

void CRandomColorPropertyDescription::UpdatePropertyByType(ERandomColorType type)
{
    switch (type)
    {
    case ERandomColorType::eRCT_ConstantColor:
        if (m_pMaxColorProperty == nullptr)
        {
            m_pMaxColorProperty = new CColorPropertyDescription(nullptr);
            m_pMaxColorProperty->SetValueByString("0xFFFFFFFF");
            m_pMaxColorProperty->SetValueByString("0xFFFFFFFF", eVT_SavedValue);
            m_pMaxColorProperty->GetBasicInfo()->m_variableName = GetBasicInfo()->m_variableName;
            if (IsInitialized())
            {
                m_pMaxColorProperty->Initialize();
            }
            InsertChild(m_pMaxColorProperty);
        }
        m_pMaxColorProperty->GetBasicInfo()->m_displayName = "color";
        m_pMaxColorProperty->SetOwner(GetOwner());
        BEATS_SAFE_DELETE_COMPONENT(m_pMinColorProperty);
        BEATS_SAFE_DELETE_COMPONENT(m_pMinGradientColorProperty);
        BEATS_SAFE_DELETE_COMPONENT(m_pMaxGradientColorProperty);
        break;
    case ERandomColorType::eRCT_RandomColor:
        if (m_pMaxColorProperty == nullptr)
        {
            m_pMaxColorProperty = new CColorPropertyDescription(nullptr);
            m_pMaxColorProperty->GetBasicInfo()->m_variableName = GetBasicInfo()->m_variableName;
            if (IsInitialized())
            {
                m_pMaxColorProperty->Initialize();
            }
            InsertChild(m_pMaxColorProperty);
        }
        m_pMaxColorProperty->GetBasicInfo()->m_displayName = "max color";
        m_pMaxColorProperty->SetOwner(GetOwner());
        if (m_pMinColorProperty == nullptr)
        {
            m_pMinColorProperty = new CColorPropertyDescription(nullptr);
            m_pMinColorProperty->GetBasicInfo()->m_variableName = GetBasicInfo()->m_variableName;
            if (IsInitialized())
            {
                m_pMinColorProperty->Initialize();
            }
            InsertChild(m_pMinColorProperty);
        }
        m_pMaxColorProperty->GetBasicInfo()->m_displayName = "min color";
        m_pMinColorProperty->SetOwner(GetOwner());
        BEATS_SAFE_DELETE_COMPONENT(m_pMinGradientColorProperty);
        BEATS_SAFE_DELETE_COMPONENT(m_pMaxGradientColorProperty);
        break;
    case ERandomColorType::eRCT_ConstantSpline:
        if (m_pMaxGradientColorProperty == nullptr)
        {
            m_pMaxGradientColorProperty = new CGradientColorPropertyDescription(nullptr);
            m_pMaxGradientColorProperty->GetBasicInfo()->m_variableName = GetBasicInfo()->m_variableName;
            if (IsInitialized())
            {
                m_pMaxGradientColorProperty->Initialize();
            }
            InsertChild(m_pMaxGradientColorProperty);
        }
        m_pMaxGradientColorProperty->GetBasicInfo()->m_displayName = "gradient";
        m_pMaxGradientColorProperty->SetOwner(GetOwner());
        BEATS_SAFE_DELETE_COMPONENT(m_pMinColorProperty);
        BEATS_SAFE_DELETE_COMPONENT(m_pMaxColorProperty);
        BEATS_SAFE_DELETE_COMPONENT(m_pMinGradientColorProperty);
        break;
    case ERandomColorType::eRCT_RandomSpline:
        if (m_pMaxGradientColorProperty == nullptr)
        {
            m_pMaxGradientColorProperty = new CGradientColorPropertyDescription(nullptr);
            m_pMaxGradientColorProperty->GetBasicInfo()->m_variableName = GetBasicInfo()->m_variableName;
            if (IsInitialized())
            {
                m_pMaxGradientColorProperty->Initialize();
            }
            InsertChild(m_pMaxGradientColorProperty);
        }
        m_pMaxGradientColorProperty->GetBasicInfo()->m_displayName = "max gradient";
        m_pMaxGradientColorProperty->SetOwner(GetOwner());
        if (m_pMinGradientColorProperty == nullptr)
        {
            m_pMinGradientColorProperty = new CGradientColorPropertyDescription(nullptr);
            m_pMinGradientColorProperty->GetBasicInfo()->m_variableName = GetBasicInfo()->m_variableName;
            if (IsInitialized())
            {
                m_pMinGradientColorProperty->Initialize();
            }
            InsertChild(m_pMinGradientColorProperty);
        }
        m_pMinGradientColorProperty->GetBasicInfo()->m_displayName = "min gradient";
        m_pMinGradientColorProperty->SetOwner(GetOwner());
        BEATS_SAFE_DELETE_COMPONENT(m_pMinColorProperty);
        BEATS_SAFE_DELETE_COMPONENT(m_pMaxColorProperty);
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
}

void CRandomColorPropertyDescription::SetColorType(ERandomColorType type)
{
    if (m_bOnlyColor)
    {
        if (type == ERandomColorType::eRCT_ConstantSpline)
        {
            type = ERandomColorType::eRCT_ConstantColor;
        }
        if (type == ERandomColorType::eRCT_RandomSpline)
        {
            type = ERandomColorType::eRCT_RandomSpline;
        }
    }
    if (m_bOnlyGradient)
    {
        if (type == ERandomColorType::eRCT_ConstantColor)
        {
            type = ERandomColorType::eRCT_ConstantSpline;
        }
        if (type == ERandomColorType::eRCT_RandomColor)
        {
            type = ERandomColorType::eRCT_RandomSpline;
        }
    }
    if (m_colorType != type)
    {
        m_colorType = type;
        UpdatePropertyByType(type);
        CRandomColor* randomColor = (CRandomColor*)m_valueArray[eVT_CurrentValue];
        randomColor->m_type = m_colorType;
    }
}

ERandomColorType CRandomColorPropertyDescription::GetColorType() const
{
    return m_colorType;
}

wxPGProperty* CRandomColorPropertyDescription::CreateRandomColorProperty(ERandomColorType type, wxPGProperty*& minProperty)
{
    wxPGProperty* pRet = nullptr;
    switch (type)
    {
    case ERandomColorType::eRCT_ConstantColor:
        pRet = m_pMaxColorProperty->CreateWxProperty();
        pRet->SetName("maxValue");
        pRet->SetLabel("color");
        break;
    case ERandomColorType::eRCT_RandomColor:
    {
        minProperty = m_pMinColorProperty->CreateWxProperty();
        minProperty->SetLabel("min color");
        minProperty->SetName("minValue");
        pRet = m_pMaxColorProperty->CreateWxProperty();
        pRet->SetName("maxValue");
        pRet->SetLabel("max color");
    }
        break;
    case ERandomColorType::eRCT_ConstantSpline:
        pRet = m_pMaxGradientColorProperty->CreateWxProperty();
        pRet->SetName("maxValue");
        pRet->SetLabel("gradient");
        break;
    case ERandomColorType::eRCT_RandomSpline:
    {
        minProperty = m_pMinGradientColorProperty->CreateWxProperty();
        minProperty->SetLabel("min gradient");
        minProperty->SetName("minValue");
        pRet = m_pMaxGradientColorProperty->CreateWxProperty();
        pRet->SetName("maxValue");
        pRet->SetLabel("max gradient");
    }
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
    BEATS_ASSERT(pRet != nullptr);
    return pRet;
}

bool CRandomColorPropertyDescription::IsOnlyGradient() const
{
    return m_bOnlyGradient;
}

bool CRandomColorPropertyDescription::IsOnlyColor() const
{
    return m_bOnlyColor;
}