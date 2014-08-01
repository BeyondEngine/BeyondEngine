#include "stdafx.h"
#include "wxWidgetsPropertyBase.h"
#include "PtrPropertyDescription.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxy.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/TinyXML/tinyxml.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>

CWxwidgetsPropertyBase::CWxwidgetsPropertyBase(EReflectPropertyType type)
: super(type)
, m_bShowChildrenInGrid(true)
, m_pComboProperty(NULL)
{

}

CWxwidgetsPropertyBase::CWxwidgetsPropertyBase(const CWxwidgetsPropertyBase& rRef)
: super(rRef)
, m_bShowChildrenInGrid(rRef.m_bShowChildrenInGrid)
, m_pVisibleWhenTrigger(rRef.m_pVisibleWhenTrigger)
{
    m_pComboProperty = rRef.CreateComboProperty();
    if (m_pComboProperty != NULL)
    {
        m_pComboProperty->SetClientData(this);
    }
}

CWxwidgetsPropertyBase::~CWxwidgetsPropertyBase()
{
    BEATS_SAFE_DELETE(m_pComboProperty);
}

void CWxwidgetsPropertyBase::Initialize()
{
    super::Initialize();
    if (m_pVisibleWhenTrigger.Get() != NULL)
    {
        m_pVisibleWhenTrigger->Initialize();
        const std::vector<STriggerContent*>& triggerContent = m_pVisibleWhenTrigger->GetContent();
        for (size_t i = 0; i < triggerContent.size(); ++i)
        {
            // 0 means or 1 means and.
            if(triggerContent[i] != NULL && (int)triggerContent[i] != 1)
            {
                CWxwidgetsPropertyBase* pProperty = static_cast<CWxwidgetsPropertyBase*>(m_pOwner->GetPropertyDescription(triggerContent[i]->GetPropertyName().c_str()));
                BEATS_ASSERT(pProperty != NULL);
                pProperty->AddEffectProperties(this);
            }
        }
    }
}

void CWxwidgetsPropertyBase::AddEffectProperties(CWxwidgetsPropertyBase* pProperty)
{
    std::set<CWxwidgetsPropertyBase*>::iterator iter = m_effctProperties.find(pProperty);
    if (iter == m_effctProperties.end())
    {
        m_effctProperties.insert(pProperty);
    }
}

std::set<CWxwidgetsPropertyBase*>& CWxwidgetsPropertyBase::GetEffectProperties()
{
    return m_effctProperties;
}

bool CWxwidgetsPropertyBase::CheckVisibleTrigger()
{
    return m_pVisibleWhenTrigger.Get() == NULL || m_pVisibleWhenTrigger->IsOk(GetOwner());
}

wxEnumProperty* CWxwidgetsPropertyBase::GetComboProperty() const
{
    return m_pComboProperty;
}

wxEnumProperty* CWxwidgetsPropertyBase::CreateComboProperty() const
{
    wxEnumProperty* pRet = NULL;
    if (m_pComboProperty != NULL)
    {
        const wxPGChoices& choices = m_pComboProperty->GetChoices();
        wxArrayString labels = choices.GetLabels();
        BEATS_ASSERT(labels.size() > 0, _T("There must at least one choice for combo property!"));
        pRet = new wxEnumProperty(wxPG_LABEL, wxPG_LABEL, labels);
        bool bMatched = false;
        char szBuffer[1024];
        this->GetValueAsChar(eVT_DefaultValue, szBuffer);
        for (size_t i = 0; i < labels.size(); ++i)
        {
            if (labels[i].CompareTo(wxString(szBuffer)) == 0)
            {
                bMatched = true;
                break;
            }
        }
        if (bMatched)
        {
            wxVariant var(szBuffer);
            pRet->SetDefaultValue(var);
        }
        else
        {
            wxVariant var(labels[0]);
            pRet->SetDefaultValue(var);
        }

        this->GetValueAsChar(eVT_CurrentValue, szBuffer);
        for (size_t i = 0; i < labels.size(); ++i)
        {
            if (labels[i].CompareTo(wxString(szBuffer)) == 0)
            {
                bMatched = true;
                break;
            }
        }
        if (bMatched)
        {
            wxVariant curVar(szBuffer);
            pRet->SetValue(curVar);
        }
        else
        {
            wxVariant var(labels[0]);
            pRet->SetValue(var);
        }
        pRet->SetClientData((void*)this);
    }

    return pRet;
}

bool CWxwidgetsPropertyBase::ShowChildrenInGrid() const
{
    return m_bShowChildrenInGrid;
}

void CWxwidgetsPropertyBase::SetShowChildrenInGrid(bool bShow)
{
    m_bShowChildrenInGrid = bShow;
}

void CWxwidgetsPropertyBase::SetValueList(const std::vector<TString>& valueList)
{
    wxArrayString labels;
    for (size_t i = 0; i < valueList.size(); ++i)
    {
        labels.push_back(valueList[i].c_str());
    }

    if (m_pComboProperty == NULL)
    {
        m_pComboProperty = new wxEnumProperty(wxPG_LABEL, wxPG_LABEL, labels);
        m_pComboProperty->SetClientData(this);
        wxVariant var(labels[0]);
        m_pComboProperty->SetDefaultValue(var);
        wxVariant curVar(labels[0]);
        m_pComboProperty->SetValue(curVar);
        m_pComboProperty->SetModifiedStatus(!IsDataSame(true));
    }
    else
    {
        wxPGChoices choices;
        for (size_t i = 0; i < valueList.size(); ++i)
        {
            choices.Add(valueList[i].c_str());
        }
        m_pComboProperty->SetChoices(choices);
    }
}

void CWxwidgetsPropertyBase::SaveToXML( TiXmlElement* pParentNode )
{
    TiXmlElement* pVariableElement = new TiXmlElement("VariableNode");
    pVariableElement->SetAttribute("Type", m_type);
    char variableName[MAX_PATH] = {0};
    CStringHelper::GetInstance()->ConvertToCHAR(GetBasicInfo()->m_variableName.c_str(), variableName, MAX_PATH);
    pVariableElement->SetAttribute("Variable", variableName);
    char szValue[102400];
    GetValueAsChar(eVT_CurrentValue, szValue);
    pVariableElement->SetAttribute("SavedValue", szValue);
    pParentNode->LinkEndChild(pVariableElement);
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        (*m_pChildren)[i]->SaveToXML(pVariableElement);
    }
}

void CWxwidgetsPropertyBase::LoadFromXML( TiXmlElement* pNode )
{
    BEATS_ASSERT(pNode != NULL);
    const char* pValue = pNode->Attribute("SavedValue");
    TCHAR pTCHARValue[10240];
    CStringHelper::GetInstance()->ConvertToTCHAR(pValue, pTCHARValue, 10240);
    GetValueByTChar(pTCHARValue, m_valueArray[eVT_CurrentValue]);
    GetValueByTChar(pTCHARValue, m_valueArray[eVT_SavedValue]);
}

void CWxwidgetsPropertyBase::AnalyseUIParameter( const TCHAR* parameter )
{
    if (parameter != NULL && parameter[0] != 0)
    {
        CStringHelper* pStringHelper = CStringHelper::GetInstance();
        std::vector<TString> paramUnit;
        // Don't Ignore the space, because the string value may contains space.
        pStringHelper->SplitString(parameter, PROPERTY_PARAM_SPLIT_STR, paramUnit, false);
        std::vector<TString> cache;
        for (std::vector<TString>::iterator iter = paramUnit.begin(); iter != paramUnit.end();)
        {
            cache.resize(0);
            pStringHelper->SplitString(iter->c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache, true);
            bool bHandled = false;
            if(cache.size() == 2)
            {
                if (cache[0].compare(UIParameterAttrStr[eUIPAT_VisibleWhen]) == 0)
                {
                    m_pVisibleWhenTrigger = new CPropertyTrigger(cache[1]);
                    bHandled = true;
                }
                else if (cache[0].compare(UIParameterAttrStr[eUIPAT_EnumStringArray]) == 0)
                {
                    if (m_type != eRPT_Enum)
                    {
                        std::vector<TString> comboLabels;
                        pStringHelper->SplitString(cache[1].c_str(), _T("@"), comboLabels);
                        SetValueList(comboLabels);
                        bHandled = true;
                    }
                }
            }
            if (bHandled)
            {
                iter = paramUnit.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        AnalyseUIParameterImpl(paramUnit);
    }
}
