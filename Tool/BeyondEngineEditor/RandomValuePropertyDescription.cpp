#include "stdafx.h"
#include "RandomValuePropertyDescription.h"
#include "Component/ComponentPublic.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "EngineEditor.h"
#include "RandomValue.h"

CRandomValuePropertyDescription::CRandomValuePropertyDescription(CSerializer* pSerializer)
: super(eRPT_RandomValue)
{
    SRandomValue randomValue;
    if (pSerializer != nullptr)
    {
        randomValue.Deserialize(pSerializer);
    }
    InitializeValue<SRandomValue>(randomValue);
}

CRandomValuePropertyDescription::CRandomValuePropertyDescription(const CRandomValuePropertyDescription& rRef)
: super(rRef)
{
    SRandomValue randomValue;
    InitializeValue<SRandomValue>(randomValue);
}

CRandomValuePropertyDescription::~CRandomValuePropertyDescription()
{

}

bool CRandomValuePropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    if (pSourceValue != pTargetValue)
    {
        SRandomValue* pSource = (SRandomValue*)(pSourceValue);
        SRandomValue* pTarget = (SRandomValue*)(pTargetValue);
        *pTarget = *pSource;
    }
    return true;
}

bool CRandomValuePropertyDescription::IsDataSame(bool /*bWithDefaultOrXML*/)
{
    return true;
}

CPropertyDescriptionBase* CRandomValuePropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CRandomValuePropertyDescription(*this);
    return pNewProperty;
}

void CRandomValuePropertyDescription::GetValueAsChar(EValueType type, char* pOut) const
{
    SRandomValue* pRandomValue = ((SRandomValue*)(m_valueArray[type]));
    ENewRandomValueType randomValueType = pRandomValue->GetType();
    TCHAR szBuffer[64];
    _stprintf(szBuffer, "%d,", (int)randomValueType);
    TString strData = szBuffer;
    switch (randomValueType)
    {
    case ENewRandomValueType::eRVT_Constant:
        _stprintf(szBuffer, "%g", pRandomValue->GetFactor());
        strData.append(szBuffer);
        break;
    case ENewRandomValueType::eRVT_Curve:
    {
        const SCurveData& maxCurve = pRandomValue->GetMaxCurve();
        _stprintf(szBuffer, "%g,%g", pRandomValue->GetFactor(), (float)maxCurve.m_keyList.size());
        strData.append(szBuffer);
        for (auto iter = maxCurve.m_keyList.begin(); iter != maxCurve.m_keyList.end(); ++iter)
        {
            _stprintf(szBuffer, ",%g,%g,%g,%g", iter->first, iter->second.m_fValue, iter->second.m_fInSlope, iter->second.m_fOutSlope);
            strData.append(szBuffer);
        }
    }
        break;
    case ENewRandomValueType::eRVT_RandomConstant:
        _stprintf(szBuffer, "%g,%g,%g", pRandomValue->GetFactor(), pRandomValue->GetMinCurve().m_keyList.begin()->second.m_fValue, pRandomValue->GetMaxCurve().m_keyList.begin()->second.m_fValue);
        strData.append(szBuffer);
        break;
    case ENewRandomValueType::eRVT_RandomCurve:
    {
        const SCurveData& minCurve = pRandomValue->GetMinCurve();
        _stprintf(szBuffer, "%g,%g", pRandomValue->GetFactor(), (float)minCurve.m_keyList.size());
        strData.append(szBuffer);
        for (auto iter = minCurve.m_keyList.begin(); iter != minCurve.m_keyList.end(); ++iter)
        {
            _stprintf(szBuffer, ",%g,%g,%g,%g", iter->first, iter->second.m_fValue, iter->second.m_fInSlope, iter->second.m_fOutSlope);
            strData.append(szBuffer);
        }
        const SCurveData& maxCurve = pRandomValue->GetMaxCurve();
        _stprintf(szBuffer, ",%g", (float)maxCurve.m_keyList.size());
        strData.append(szBuffer);
        for (auto iter = maxCurve.m_keyList.begin(); iter != maxCurve.m_keyList.end(); ++iter)
        {
            _stprintf(szBuffer, ",%g,%g,%g,%g", iter->first, iter->second.m_fValue, iter->second.m_fInSlope, iter->second.m_fOutSlope);
            strData.append(szBuffer);
        }
    }
        break;
    default:
        break;
    }
    strcpy(pOut, strData.c_str());
}

bool CRandomValuePropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    std::vector<TString> strData;
    CStringHelper::GetInstance()->SplitString(pIn, ",", strData);
    std::vector<float> realData;
    for (size_t i = 0; i < strData.size(); ++i)
    {
        float fFloatDat = _tstof(strData[i].c_str());
        realData.push_back(fFloatDat);
    }
    uint32_t uReader = 0;
    ENewRandomValueType randomType = (ENewRandomValueType)((int)realData[uReader++]);
    SRandomValue* pOutValueImp = (SRandomValue*)(pOutValue);
    pOutValueImp->SetType(randomType);
    switch (randomType)
    {
    case ENewRandomValueType::eRVT_Constant:
        pOutValueImp->SetFactor(realData[uReader++]);
        break;
    case ENewRandomValueType::eRVT_Curve:
    {
        pOutValueImp->SetFactor(realData[uReader++]);
        uint32_t uKeySize = (uint32_t)(realData[uReader++]);
        pOutValueImp->GetMaxCurve().m_keyList.clear();
        for (uint32_t i = 0; i < uKeySize; ++i)
        {
            SCurveKey& curveKey = pOutValueImp->GetMaxCurve().m_keyList[realData[uReader++]];
            curveKey.m_fValue = realData[uReader++];
            curveKey.m_fInSlope = realData[uReader++];
            curveKey.m_fOutSlope = realData[uReader++];
        }
    }
        break;
    case ENewRandomValueType::eRVT_RandomConstant:
        pOutValueImp->SetFactor(realData[uReader++]);
        pOutValueImp->GetMinCurve().m_keyList[0].m_fValue = realData[uReader++];
        pOutValueImp->GetMaxCurve().m_keyList[0].m_fValue = realData[uReader++];
        break;
    case ENewRandomValueType::eRVT_RandomCurve:
    {
        pOutValueImp->SetFactor(realData[uReader++]);
        uint32_t uKeySize = (uint32_t)(realData[uReader++]);
        pOutValueImp->GetMinCurve().m_keyList.clear();
        for (uint32_t i = 0; i < uKeySize; ++i)
        {
            SCurveKey& curveKey = pOutValueImp->GetMinCurve().m_keyList[realData[uReader++]];
            curveKey.m_fValue = realData[uReader++];
            curveKey.m_fInSlope = realData[uReader++];
            curveKey.m_fOutSlope = realData[uReader++];
        }
        uKeySize = (uint32_t)(realData[uReader++]);
        pOutValueImp->GetMaxCurve().m_keyList.clear();
        for (uint32_t i = 0; i < uKeySize; ++i)
        {
            SCurveKey& curveKey = pOutValueImp->GetMaxCurve().m_keyList[realData[uReader++]];
            curveKey.m_fValue = realData[uReader++];
            curveKey.m_fInSlope = realData[uReader++];
            curveKey.m_fOutSlope = realData[uReader++];
        }
    }
        break;
    default:
        break;
    }
    BEATS_ASSERT(realData.size() == uReader);
    return true;
}

void CRandomValuePropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType)
{
    ((SRandomValue*)(m_valueArray[eValueType]))->Serialize(&serializer);
}

void CRandomValuePropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType)
{
    ((SRandomValue*)(m_valueArray[eValueType]))->Deserialize(&serializer);
}

void CRandomValuePropertyDescription::SetValue(wxVariant& value, bool bSaveValue)
{
    SRandomValue* pRandomValue = (SRandomValue*)value.GetVoidPtr();
    SetValueWithType(pRandomValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(pRandomValue, eVT_SavedValue);
    }
}

bool CRandomValuePropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& /*parameterUnit*/)
{
    return true;
}

wxPGProperty* CRandomValuePropertyDescription::CreateWxProperty()
{
    TCHAR szBuffer[MAX_PATH];
    GetValueAsChar(eVT_CurrentValue, szBuffer);
    std::string strDisplayString = szBuffer;
    size_t uPos = strDisplayString.find_first_of(',');
    if (uPos != std::string::npos)
    {
        strDisplayString = strDisplayString.substr(uPos + 1);
    }
    wxPGProperty* pRet = new wxStringProperty(wxPG_LABEL, wxPG_LABEL, strDisplayString);
    pRet->SetClientData(this);
    pRet->SetEditor(static_cast<CEngineEditor*>(wxApp::GetInstance())->GetRandomValueEditor());
    return pRet;
}