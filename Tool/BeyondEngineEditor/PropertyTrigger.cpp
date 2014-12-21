#include "stdafx.h"
#include "PropertyTrigger.h"
#include "Utility/BeatsUtility/StringHelper.h"

#include "Component/ComponentPublic.h"
#include "Component/Component/ComponentProxy.h"
#include "wxWidgetsPropertyBase.h"
#include <wx/propgrid/propgrid.h>
#include "EnumPropertyDescription.h"

STriggerContent::STriggerContent( const TString& strProperty, ECompareOperator op, const TString& refValue )
: m_strPropertyName(strProperty)
, m_op(op)
, m_strRefValue(refValue)
{
    BEATS_ASSERT(m_strPropertyName.length() > 0, _T("Property Name Can't be Empty!"));
    BEATS_ASSERT(op >= eCO_GreaterEqual && op < eCO_Count, _T("Invalid operator value %d"), op);
    BEATS_ASSERT(m_strRefValue.length() > 0, _T("RefValue can't be Empty!"));
}

STriggerContent::~STriggerContent()
{

}

bool STriggerContent::IsOk(CComponentProxy* pComponent)
{
    BEATS_ASSERT(pComponent != NULL);
    CWxwidgetsPropertyBase* pProperty = NULL;
    const std::vector<CPropertyDescriptionBase*>* pPropertyPool = pComponent->GetPropertyPool();
    for (uint32_t i = 0; i < pPropertyPool->size(); ++i)
    {
        const TString& name = (*pPropertyPool)[i]->GetBasicInfo()->m_variableName;
        if(name.compare(m_strPropertyName) == 0)
        {
            pProperty = static_cast<CWxwidgetsPropertyBase*>((*pPropertyPool)[i]);
            break;
        }
    }
    bool bRet = pProperty != NULL;
    BEATS_ASSERT(bRet, _T("Can't Find Property %s at component %s in the trigger content!"), m_strPropertyName.c_str(), pComponent->GetDisplayName().c_str());
    if (bRet)
    {
        switch (pProperty->GetType())
        {
        case eRPT_Bool:
            {
                bool bData = _tcsicmp(m_strRefValue.c_str(), _T("true")) == 0;
                BEATS_ASSERT(bData || (_tcsicmp(m_strRefValue.c_str(), _T("false")) == 0), _T("Unrecognize value for trigger: %s"), m_strRefValue.c_str());
                BEATS_ASSERT(m_op == eCO_Equal || eCO_NotEqaul, _T("Only eCO_Equal and eCO_NotEqaul can be applied to bool property. Current operator:%d"), m_op);
                bRet = ExamByOperator(*(bool*)pProperty->GetValue(eVT_CurrentValue), bData);
            }
            break;
        case eRPT_Float:
            {
                TCHAR* pEndPtr;
                float fData = (float)_tcstod(m_strRefValue.c_str(), &pEndPtr);
                BEATS_ASSERT(pEndPtr[0] == 0, _T("Convert string to float failed! %s"), m_strRefValue.c_str());
                bRet = ExamByOperator(*(float*)pProperty->GetValue(eVT_CurrentValue), fData);
            }
            break;
        case eRPT_Double:
            {
                TCHAR* pEndPtr;
                double fData = _tcstod(m_strRefValue.c_str(), &pEndPtr);
                BEATS_ASSERT(pEndPtr[0] == 0, _T("Convert string to double failed! %s"), m_strRefValue.c_str());
                bRet = ExamByOperator(*(double*)pProperty->GetValue(eVT_CurrentValue), fData);
            }
        case eRPT_Int:
            {
                TCHAR* pEndPtr;
                int iData = _tcstol(m_strRefValue.c_str(), &pEndPtr, 10);
                BEATS_ASSERT(pEndPtr[0] == 0, _T("Convert string to int failed! %s"), m_strRefValue.c_str());
                bRet = ExamByOperator(*(int*)pProperty->GetValue(eVT_CurrentValue), iData);
            }
            break;
        case eRPT_Enum:
            {
                TCHAR* pEndPtr;
                int nValue = _tcstol(m_strRefValue.c_str(), &pEndPtr, 10);
                CEnumPropertyDescription* pEnumProperty = down_cast<CEnumPropertyDescription*>(pProperty);
                if (pEndPtr[0] != 0)
                {
                    pEnumProperty->QueryValueByString(m_strRefValue.c_str(), nValue);
                }
                int nCurrValue = 0;
                pEnumProperty->QueryValueByString(*(TString*)pProperty->GetValue(eVT_CurrentValue), nCurrValue);
                bRet = ExamByOperator(nCurrValue, nValue);
            }
            break;
        case eRPT_UInt:
            {
                TCHAR* pEndPtr;
                uint32_t uData = _tcstoul(m_strRefValue.c_str(), &pEndPtr, 10);
                BEATS_ASSERT(pEndPtr[0] == 0, _T("Convert string to Uint failed! %s"), m_strRefValue.c_str());
                bRet = ExamByOperator(*(uint32_t*)pProperty->GetValue(eVT_CurrentValue), uData);
            }
            break;
        case eRPT_File:
            {
                TString strPath;
                if (m_strRefValue != "trigger_empty")
                {
                    strPath = m_strRefValue;
                }
                bRet = ExamByOperator(*(TString*)pProperty->GetValue(eVT_CurrentValue), strPath);
            }
            break;
        default:
            BEATS_ASSERT(false, _T("Type is not supported yet!"));
            break;
        }
    }

    return bRet;
}

CPropertyTrigger::CPropertyTrigger(const TString& strRawString)
: m_rawString(strRawString)
{

}

CPropertyTrigger::~CPropertyTrigger()
{
    for (uint32_t i = 0; i < m_content.size(); ++i)
    {
        if (m_content[i] != NULL && (uint32_t)m_content[i] != 1)
        {
            BEATS_SAFE_DELETE(m_content[i]);
        }
    }
}

void CPropertyTrigger::Initialize()
{
    if(!IsInitialized())
    {
        std::vector<TString> revPolishnatation;
        ConvertExpression(m_rawString, revPolishnatation);
        m_content.clear();
        for (uint32_t i = 0; i < revPolishnatation.size(); ++i)
        {
            const TString& str = revPolishnatation[i];
            if (str.compare(_T("||")) == 0)
            {
                m_content.push_back(NULL);
            }
            else if (str.compare(_T("&&")) == 0)
            {
                m_content.push_back((STriggerContent*)1);
            }
            else
            {
                STriggerContent* pContent = GenerateContent(str);
                BEATS_ASSERT(pContent != NULL, _T("Generate content failed with string %s!"), str.c_str());
                if (pContent != NULL)
                {
                    m_content.push_back(pContent);
                }
            }
        }
        BEATS_ASSERT(m_content.size() > 0, _T("Content size must be greater than 0!"));
    }
}

bool CPropertyTrigger::IsInitialized()
{
    return m_content.size() > 0;
}

const std::vector<STriggerContent*>& CPropertyTrigger::GetContent() const
{
    return m_content;
}

STriggerContent* CPropertyTrigger::GenerateContent(const TString& rawString)
{
    STriggerContent* pRet = NULL;
    CStringHelper* pStringHelper = CStringHelper::GetInstance();
    std::set<TString> strFilter;
    strFilter.insert(_T(" "));
    TString str = pStringHelper->FilterString(rawString.c_str(), strFilter);
    TString refValue;
    TString strPropertyName;
    ECompareOperator op = eCO_Count;
    for (uint32_t i = 0; i < str.length(); ++i)
    {
        // 1. Get the variable name
        if (strPropertyName.length() == 0)
        {
            // check next character to see if it is a valid value for variable name.
            TCHAR character = str.at(i + 1);
            CStringHelper::EStringCharacterType characterType = pStringHelper->GetCharacterType(character);
            if (characterType != CStringHelper::eSCT_Number &&
                characterType != CStringHelper::eSCT_LowerCaseCharacter &&
                characterType != CStringHelper::eSCT_UpperCaseCharacter &&
                character != _T('_'))
            {
                strPropertyName = str;
                strPropertyName.resize(i + 1);
            }
        }
        // 2. Get The operator.
        else if (op == eCO_Count) 
        {
            const TCHAR* pszReader = &str.at(i);
            for (uint32_t j = 0; j < eCO_Count; ++j)
            {
                const TCHAR* pszOperatorString = CompareOperatorString[j];
                uint32_t uOperatorLength = _tcslen(pszOperatorString);
                if (memcmp(pszOperatorString, pszReader, sizeof(TCHAR) * uOperatorLength) == 0)
                {
                    i += uOperatorLength - 1; // Skip the next operator character for those length is more than 1.
                    op = (ECompareOperator)j;
                    break;
                }
            }
        }
        else
        {
            refValue.assign(&str.at(i));
            break;
        }
    }
    bool bSuccess = op != eCO_Count && refValue.length() > 0 && strPropertyName.length() > 0;
    BEATS_ASSERT(bSuccess, _T("Generate CPropertyTrigger failed with string %s !"), str.c_str());
    if (bSuccess)
    {
        pRet = new STriggerContent(strPropertyName, op, refValue);
    }
    return pRet;
}

void CPropertyTrigger::ConvertExpression(const TString& rawString, std::vector<TString>& result)
{
    result.clear();
    CStringHelper* pStringHelper = CStringHelper::GetInstance();
    std::set<TString> strFilter;
    strFilter.insert(_T(" "));
    TString str = pStringHelper->FilterString(rawString.c_str(), strFilter);

    const uint32_t uStringLength = str.length();
    bool bValidString = uStringLength > 0;
    BEATS_ASSERT(bValidString, _T("String can't be null for initializing a CPropertyTrigger"));
    if (bValidString)
    {
        std::vector<TString> operatorStack;
        TString symbolString;
        for (uint32_t i = 0; i < uStringLength; ++i)
        {
            bool bReadOperator = false;
            if (str[i] == _T('('))
            {
                bReadOperator = true;
                operatorStack.push_back(_T("("));
            }
            else if (str[i] == _T(')'))
            {
                bReadOperator = true;
                while (operatorStack.back().compare(_T("(")) != 0)
                {
                    if (symbolString.length() > 0)
                    {
                        result.push_back(symbolString);
                        symbolString.clear();
                    }
                    result.push_back(operatorStack.back());
                    operatorStack.pop_back();
                }
                operatorStack.pop_back(); // Pop the "("
            }
            else if (memcmp(&str[i], _T("||"), sizeof(TCHAR) * _tcslen(_T("||"))) == 0)
            {
                bReadOperator = true;
                while (operatorStack.size() > 0 && operatorStack.back().compare(_T("(")) != 0)
                {
                    if (symbolString.length() > 0)
                    {
                        result.push_back(symbolString);
                        symbolString.clear();
                    }

                    result.push_back(operatorStack.back());
                    operatorStack.pop_back();
                }
                operatorStack.push_back(_T("||"));
                ++i; // Skip next '|'
            }
            else if (memcmp(&str[i], _T("&&"), sizeof(TCHAR) * _tcslen(_T("&&"))) == 0)
            {
                bReadOperator = true;
                while (operatorStack.size() > 0 && operatorStack.back().compare(_T("(")) != 0)
                {
                    if (symbolString.length() > 0)
                    {
                        result.push_back(symbolString);
                        symbolString.clear();
                    }

                    result.push_back(operatorStack.back());
                    operatorStack.pop_back();
                }
                operatorStack.push_back(_T("&&"));
                ++i;//Skip next '&'
            }

            if (bReadOperator)
            {
                if (symbolString.length() > 0)
                {
                    result.push_back(symbolString);
                    symbolString.clear();
                }
            }
            else
            {
                symbolString.append(&str[i], 1);
            }
        }
        if (symbolString.length() > 0)
        {
            result.push_back(symbolString);
        }
        while (operatorStack.size() > 0)
        {
            result.push_back(operatorStack.back());
            operatorStack.pop_back();
        }
    }
}

bool CPropertyTrigger::IsOk(CComponentProxy* pComponent)
{
    BEATS_ASSERT(pComponent != NULL);
    if (IsInitialized())
    {
        Initialize();
    }
    bool bRet = false;
    BEATS_ASSERT(m_content.size() > 0);
    std::vector<EAbstractRelation> abstractRelationShip;
    for (uint32_t i = 0; i < m_content.size(); ++i)
    {
        if (m_content[i] == NULL) // means "||"
        {
            abstractRelationShip.push_back(eAR_Or);
        }
        else if ((uint32_t)m_content[i] == 1) // means "&&"
        {
            abstractRelationShip.push_back(eAR_And);
        }
        else
        {
            abstractRelationShip.push_back(m_content[i]->IsOk(pComponent) ? eAR_True : eAR_False);
        }
    }
    std::vector<bool> bFinalResultStack;
    for (uint32_t i = 0; i < abstractRelationShip.size(); ++i)
    {
        if (abstractRelationShip[i] == eAR_Or)
        {
            bool bCondition1 = bFinalResultStack.back();
            bFinalResultStack.pop_back();
            bool bCondition2 = bFinalResultStack.back();
            bFinalResultStack.pop_back();
            bFinalResultStack.push_back(bCondition1 || bCondition2);
        }
        else if (abstractRelationShip[i] == eAR_And)
        {
            bool bCondition1 = bFinalResultStack.back();
            bFinalResultStack.pop_back();
            bool bCondition2 = bFinalResultStack.back();
            bFinalResultStack.pop_back();
            bFinalResultStack.push_back(bCondition1 && bCondition2);
        }
        else
        {
            bFinalResultStack.push_back(abstractRelationShip[i] == eAR_True);
        }
    }
    BEATS_ASSERT(bFinalResultStack.size() == 1, _T("The last result for reverse polishnatation must only exists 1!"));
    bRet = bFinalResultStack.size() == 1 && bFinalResultStack[0];
    return bRet;
}

