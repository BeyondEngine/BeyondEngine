#include "stdafx.h"
#include "wxWidgetsPropertyBase.h"
#include "PtrPropertyDescription.h"
#include "Component/Component/ComponentProxy.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Component/ComponentPublic.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include "EditorMainFrame.h"
#include "EngineEditor.h"

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
    BEATS_SAFE_DELETE(m_pValueImage);
}

void CWxwidgetsPropertyBase::Initialize()
{
    super::Initialize();
    if (m_pVisibleWhenTrigger.Get() != NULL)
    {
        m_pVisibleWhenTrigger->Initialize();
        const std::vector<STriggerContent*>& triggerContent = m_pVisibleWhenTrigger->GetContent();
        for (uint32_t i = 0; i < triggerContent.size(); ++i)
        {
            // 0 means or 1 means and.
            if(triggerContent[i] != NULL && (int)triggerContent[i] != 1)
            {
                CWxwidgetsPropertyBase* pProperty = static_cast<CWxwidgetsPropertyBase*>(m_pOwner->GetProperty(triggerContent[i]->GetPropertyName().c_str()));
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
        for (uint32_t i = 0; i < labels.size(); ++i)
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
        for (uint32_t i = 0; i < labels.size(); ++i)
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

const wxImage& CWxwidgetsPropertyBase::GetValueImage()
{
    if (m_pValueImage == nullptr)
    {
        m_pValueImage = new wxImage;
    }
    return *m_pValueImage;
}

void CWxwidgetsPropertyBase::SetValueImage(const wxImage& image)
{
    if (m_pValueImage == nullptr)
    {
        m_pValueImage = new wxImage;
    }
    if (m_pValueImage->IsOk())
    {
        m_pValueImage->Clear();
    }
    if (image.IsOk())
    {
        *m_pValueImage = image.Copy();
    }
}

bool CWxwidgetsPropertyBase::IsExpanded() const
{
    return m_bExpand;
}

void CWxwidgetsPropertyBase::SetExpandFlag(bool bExpand)
{
    m_bExpand = bExpand;
}

void CWxwidgetsPropertyBase::SetValueList(const std::vector<TString>& valueList)
{
    wxArrayString labels;
    for (uint32_t i = 0; i < valueList.size(); ++i)
    {
        labels.push_back(valueList[i].c_str());
    }

    if (m_pComboProperty == NULL)
    {
        if (labels.size() > 0)
        {
            m_pComboProperty = new wxEnumProperty(wxPG_LABEL, wxPG_LABEL, labels);
            m_pComboProperty->SetClientData(this);
            wxVariant var(labels[0]);
            m_pComboProperty->SetDefaultValue(var);
            wxVariant curVar(labels[0]);
            m_pComboProperty->SetValue(curVar);
            m_pComboProperty->SetModifiedStatus(!IsDataSame(true));
        }
    }
    else
    {
        if (valueList.size() > 0)
        {
            wxPGChoices choices;
            for (uint32_t i = 0; i < valueList.size(); ++i)
            {
                choices.Add(valueList[i].c_str());
            }
            m_pComboProperty->SetChoices(choices);
        }
        else
        {
            BEATS_SAFE_DELETE(m_pComboProperty);
        }
    }
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();

    if (m_pOwner != nullptr && m_pOwner == pMainFrame->GetSelectedComponent())
    {
        pMainFrame->GetPropGridManager()->RefreshPropertyInGrid(this);
    }
}

void CWxwidgetsPropertyBase::SaveToXML(rapidxml::xml_node<>* pParentNode)
{
    bool bHide = IsHide();
    bool bShouldBeVisible = CheckVisibleTrigger();
    if (!bHide && bShouldBeVisible)
    {
        rapidxml::xml_document<>* pDoc = pParentNode->document();
        rapidxml::xml_node<>* pVariableElement = pDoc->allocate_node(rapidxml::node_element, "VariableNode");
        pVariableElement->append_attribute(pDoc->allocate_attribute("Type", pDoc->allocate_string(std::to_string(m_type).c_str())));
        pVariableElement->append_attribute(pDoc->allocate_attribute("Variable", pDoc->allocate_string(GetBasicInfo()->m_variableName.c_str())));
        if (m_pComboProperty == NULL)
        {
            TCHAR szBuffer[BEATS_PRINT_BUFFER_SIZE];
            GetValueAsChar(eVT_CurrentValue, szBuffer);
            pVariableElement->append_attribute(pDoc->allocate_attribute("SavedValue", pDoc->allocate_string(szBuffer)));
        }
        else
        {
            pVariableElement->append_attribute(pDoc->allocate_attribute("SavedValue", pDoc->allocate_string(m_pComboProperty->GetValueAsString())));
        }
        pParentNode->append_node(pVariableElement);
        for (uint32_t i = 0; i < m_pChildren->size(); ++i)
        {
            (*m_pChildren)[i]->SaveToXML(pVariableElement);
        }
    }
}

void CWxwidgetsPropertyBase::LoadFromXML(rapidxml::xml_node<>* pNode)
{
    BEATS_ASSERT(pNode != NULL);
    const char* pValue = pNode->first_attribute("SavedValue")->value();
    GetValueByTChar(pValue, m_valueArray[eVT_CurrentValue]);
    GetValueByTChar(pValue, m_valueArray[eVT_SavedValue]);
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
