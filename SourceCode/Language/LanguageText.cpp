#include "stdafx.h"
#include "LanguageText.h"
#include "LanguageManager.h"

CLanguageText::CLanguageText()
: m_type((ELanguageTextType)0xFFFFFFFF)
{
    CLanguageManager::GetInstance()->RegisterLanguageText(this);
}

CLanguageText::CLanguageText(const CLanguageText& ref)
: m_type(ref.m_type)
{
    m_strValue = ref.m_strValue;
    CLanguageManager::GetInstance()->RegisterLanguageText(this);
}

CLanguageText::CLanguageText(CLanguageText&& rhs)
: m_type((ELanguageTextType)rhs.m_type)
, m_strValue(rhs.m_strValue)
{
    CLanguageManager::GetInstance()->RegisterLanguageText(this);
}

CLanguageText::~CLanguageText()
{
    CLanguageManager::GetInstance()->UnregisterLanguageText(this);
}

CLanguageText& CLanguageText::operator = (const CLanguageText& rhs)
{
    m_type = rhs.m_type;
    m_strValue = rhs.m_strValue;
    return *this;
}

ELanguageTextType CLanguageText::GetType() const
{
    return m_type;
}

void CLanguageText::SetType(ELanguageTextType type)
{
    m_type = type;
    RefreshValueString();
}

const TString& CLanguageText::GetValueString() const
{
    return m_strValue;
}

void CLanguageText::SetValueString(const TString& str)
{
    m_strValue = str;
}

void CLanguageText::RefreshValueString()
{
    if (m_type == 0xFFFFFFFE)
    {
        m_strValue = "Unknown String";
    }
    else if (m_type != 0xFFFFFFFF)
    {
        m_strValue = CLanguageManager::GetInstance()->GetUtf8Text(m_type);
    }
}
