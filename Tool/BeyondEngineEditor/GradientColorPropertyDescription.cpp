#include "stdafx.h"
#include "EngineEditor.h"
#include "GradientColorPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Component/ComponentPublic.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "GradientDialog.h"
#include "GradientCtrl.h"

CGradientColorPropertyDescription::CGradientColorPropertyDescription(CSerializer* /*pSerializer*/)
    : super(eRPT_GradientColor)
{
    TString strValue;
    InitializeValue(strValue);
    m_colorMap[0] = 0xFFFFFFFF;
    m_colorMap[1] = 0xFFFFFFFF;
    m_alphaMap[0] = 0xFF;
    m_alphaMap[1] = 0xFF;
}

CGradientColorPropertyDescription::CGradientColorPropertyDescription(const CGradientColorPropertyDescription& rRef)
    : super(rRef)
{
    TString strValue;
    InitializeValue<TString>(strValue);
    m_colorMap = rRef.m_colorMap;
    m_alphaMap = rRef.m_alphaMap;
}

CGradientColorPropertyDescription::~CGradientColorPropertyDescription()
{
    DestroyValue<TString>();
}

bool CGradientColorPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& /*parameterUnit*/)
{
    return true;
}

void CGradientColorPropertyDescription::Initialize()
{
    super::Initialize();
    ApplyToDialog();
    CEngineEditor* pEngineEditor = down_cast<CEngineEditor*>(wxApp::GetInstance());
    CGradientDialog* pDialog = pEngineEditor->GetGradientDialog();
    CGradientCtrl* pGradientCtrl = pDialog->GetGradientCtrl();
    pGradientCtrl->DrawColorBoard();
    SetValueImage(pGradientCtrl->GetImage());
}

wxPGProperty* CGradientColorPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL, wxEmptyString);
    wxVariant defaultValue(((TString*)m_valueArray[eVT_DefaultValue])->c_str());
    pProperty->SetDefaultValue(defaultValue);
    pProperty->SetClientData(this);
    pProperty->SetModifiedStatus(GetInstanceComponent() != NULL);
    CEngineEditor* pEngineEditor = down_cast<CEngineEditor*>(wxApp::GetInstance());
    pProperty->SetEditor(pEngineEditor->GetGradientColorEditor());
    CGradientDialog* pDialog = pEngineEditor->GetGradientDialog();
    BEATS_ASSERT(pDialog);
    pDialog->Reset();
    return pProperty;
}

void CGradientColorPropertyDescription::SetValue( wxVariant& value, bool bSaveValue /*= true*/ )
{
    TString strNewValue = value.GetString();
    SetValueWithType(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&strNewValue, eVT_SavedValue);
    } 
}

bool CGradientColorPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(TString*)pTargetValue != *(TString*)pSourceValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *(TString*)pSourceValue;
    }
    //return true means always do value sync( from PropertyDescription.Serialize() to Component.ReflectData() )
    return true;
}

bool CGradientColorPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = _tcscmp(((TString*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)])->c_str(), ((TString*)m_valueArray[eVT_CurrentValue])->c_str()) == 0;
    return bRet;
}

CPropertyDescriptionBase* CGradientColorPropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CGradientColorPropertyDescription(*this);
    return pNewProperty;
}

void CGradientColorPropertyDescription::GetValueAsChar( EValueType /*type*/, char* pOut ) const
{
    TString strValue = WriteToString();
    _tcscpy(pOut, strValue.c_str());
}

bool CGradientColorPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* /*pOutValue*/)
{
    ReadFromString(pIn);
    return true;
}

void CGradientColorPropertyDescription::Serialize(CSerializer& serializer, EValueType /*eValueType*/ /*= eVT_SavedValue*/)
{
    CColorSpline colorSpline(m_colorMap, m_alphaMap);
    serializer << colorSpline.m_mapColors.size();
    for (auto iter = colorSpline.m_mapColors.begin(); iter != colorSpline.m_mapColors.end(); ++iter)
    {
        serializer << iter->first;
        serializer << iter->second;
    }
}

void CGradientColorPropertyDescription::Deserialize(CSerializer& /*serializer*/, EValueType /*eValueType*/ /*= eVT_CurrentValue*/)
{
    BEATS_ASSERT(false, "Why reach here?");
}

std::map<float, CColor>& CGradientColorPropertyDescription::GetColorMap()
{
    return m_colorMap;
}

std::map<float, uint8_t>& CGradientColorPropertyDescription::GetAlphaMap()
{
    return m_alphaMap;
}

TString CGradientColorPropertyDescription::WriteToString() const
{
    TString strNewValue;
    BEATS_ASSERT(m_colorMap.size() > 0 && m_alphaMap.size() > 0);
    TCHAR szBuffer[MAX_PATH];
    _stprintf(szBuffer, _T("%d"), m_colorMap.size());
    strNewValue.append(szBuffer).append(_T(":"));
    for (auto iter = m_colorMap.begin(); iter != m_colorMap.end(); ++iter)
    {
        if (iter != m_colorMap.begin())
        {
            strNewValue.append(_T(","));
        }
        _stprintf(szBuffer, _T("%f"), iter->first);
        strNewValue.append(szBuffer).append(_T(","));
        uint32_t uColor = iter->second;
        _stprintf(szBuffer, _T("%x"), uColor);
        strNewValue.append(szBuffer);
    }

    strNewValue.append(_T("@"));

    _stprintf(szBuffer, _T("%d"), m_alphaMap.size());
    strNewValue.append(szBuffer).append(_T(":"));
    for (auto iter = m_alphaMap.begin(); iter != m_alphaMap.end(); ++iter)
    {
        if (iter != m_alphaMap.begin())
        {
            strNewValue.append(_T(","));
        }
        _stprintf(szBuffer, _T("%f"), iter->first);
        strNewValue.append(szBuffer).append(_T(","));
        _stprintf(szBuffer, _T("%x"), iter->second);
        strNewValue.append(szBuffer);
    }
    return strNewValue;
}

void CGradientColorPropertyDescription::ReadFromString(const TString& strValue)
{
    if (!strValue.empty())
    {
        m_colorMap.clear();
        m_alphaMap.clear();

        std::vector<TString> ret;
        CStringHelper::GetInstance()->SplitString(strValue.c_str(), _T("@"), ret);
        BEATS_ASSERT(ret.size() == 2);

        //1 .Read color map
        const TString& colorMapString = ret[0];
        BEATS_ASSERT(!colorMapString.empty());
        std::vector<TString> colorMapRet;
        CStringHelper::GetInstance()->SplitString(colorMapString.c_str(), _T(":"), colorMapRet);
        BEATS_ASSERT(colorMapRet.size() == 2);
        TCHAR* pEndChar = NULL;
        uint32_t uColorCount = _tcstoul(colorMapRet[0].c_str(), &pEndChar, 10);
        BEYONDENGINE_UNUSED_PARAM(uColorCount);
        BEATS_ASSERT(_tcslen(pEndChar) == 0, _T("Read uint from string %s error, stop at %s"), colorMapRet[0].c_str(), pEndChar);
        std::vector<TString> colorMapData;
        CStringHelper::GetInstance()->SplitString(colorMapRet[1].c_str(), _T(","), colorMapData);
        BEATS_ASSERT(colorMapData.size() == uColorCount * 2);
        for (uint32_t i = 0; i < colorMapData.size();)
        {
            float fProgress = (float)_tstof(colorMapData[i].c_str());
            uint32_t colorValue = _tcstoul(colorMapData[i + 1].c_str(), &pEndChar, 16);
            BEATS_ASSERT(_tcslen(pEndChar) == 0, _T("Read 0x number from string failed!"));
            CColor color(colorValue);
            m_colorMap[fProgress] = color;
            i += 2;
        }

        //2 .Read alpha map
        const TString& alphaMapString = ret[1];
        BEATS_ASSERT(!alphaMapString.empty());
        std::vector<TString> alphaMapRet;
        CStringHelper::GetInstance()->SplitString(alphaMapString.c_str(), _T(":"), alphaMapRet);
        BEATS_ASSERT(alphaMapRet.size() == 2);
        uint32_t uAlphaCount = _tcstoul(alphaMapRet[0].c_str(), &pEndChar, 10);
        BEYONDENGINE_UNUSED_PARAM(uAlphaCount);
        BEATS_ASSERT(_tcslen(pEndChar) == 0, _T("Read uint from string %s error, stop at %s"), alphaMapRet[0].c_str(), pEndChar);
        std::vector<TString> alphaMapData;
        CStringHelper::GetInstance()->SplitString(alphaMapRet[1].c_str(), _T(","), alphaMapData);
        BEATS_ASSERT(alphaMapData.size() == uAlphaCount * 2);
        for (uint32_t i = 0; i < alphaMapData.size();)
        {
            float fProgress = (float)_tstof(alphaMapData[i].c_str());
            uint8_t alpha = (uint8_t)(_tcstoul(alphaMapData[i + 1].c_str(), &pEndChar, 16));
            BEATS_ASSERT(_tcslen(pEndChar) == 0, _T("Read 0x number from string failed!"));
            m_alphaMap[fProgress] = alpha;
            i += 2;
        }
    }
}

void CGradientColorPropertyDescription::ApplyToDialog()
{
    CEngineEditor* pEngineEditor = down_cast<CEngineEditor*>(wxApp::GetInstance());
    CGradientDialog* pDialog = pEngineEditor->GetGradientDialog();
    CGradientCtrl* pGradientCtrl = pDialog->GetGradientCtrl();
    BEATS_ASSERT(pGradientCtrl != nullptr);
    std::map<float, CColor>& colorMap = m_colorMap;
    std::map<float, uint8_t>& alphaMap = m_alphaMap;
    pGradientCtrl->Reset(colorMap.size() < 2, alphaMap.size() < 2);
    if (colorMap.size() < 2)
    {
        CColor color = 0xFFFFFFFF;
        if (colorMap.size() > 0)
        {
            color = colorMap.begin()->second;
        }
        colorMap.clear();
        colorMap[0] = color;
        colorMap[1] = color;
    }
    if (alphaMap.size() < 2)
    {
        uint8_t uAlpha = 0xFF;
        if (alphaMap.size() > 0)
        {
            uAlpha = alphaMap.begin()->second;
        }
        alphaMap.clear();
        alphaMap[0] = uAlpha;
        alphaMap[1] = uAlpha;
    }
    for (auto iter = colorMap.begin(); iter != colorMap.end(); ++iter)
    {
        CGradientCursor* pNewCursor = new CGradientCursor(iter->first, wxColor(iter->second.r, iter->second.g, iter->second.b, iter->second.a), eCT_Color);
        pGradientCtrl->AddCursor(pNewCursor);
    }
    for (auto iter = alphaMap.begin(); iter != alphaMap.end(); ++iter)
    {
        CGradientCursor* pNewCursor = new CGradientCursor(iter->first, wxColor(iter->second, iter->second, iter->second, iter->second), eCT_Alpha);
        pGradientCtrl->AddCursor(pNewCursor);
    }
}
