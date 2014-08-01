#include "stdafx.h"
#include "LanguageManager.h"
#include "Resource/ResourcePathManager.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utf8String.h"

CLanguageManager* CLanguageManager::m_pInstance = NULL;

CLanguageManager::CLanguageManager()
{

}

CLanguageManager::~CLanguageManager()
{

}

const TString &CLanguageManager::GetUtf8Text(EText textid)  const
{
    auto itr = m_texts.find(textid);
    return itr != m_texts.end() ? itr->second : m_emptyString;
}

TString CLanguageManager::GetTText(EText textid) const
{
    return Utf8ToTString(GetUtf8Text(textid));
}

void CLanguageManager::LoadFromFile(ELanguage language)
{
    TString filePath = CResourcePathManager::GetInstance()->GetResourcePath( CResourcePathManager::eRPT_Language );
    filePath.append(_T("/Language.bin"));
    int count = 0;
    CSerializer tmp(filePath.c_str());
    tmp >> count;
    m_texts.clear();
    for (int i = 0; i < count; ++i)
    {
        TString strEnum;
        tmp >> strEnum;
        std::string strChinese;
        tmp >> strChinese;
        std::string strEnglish;
        tmp >> strEnglish;
        std::string strCurrLanguage;
        switch (language)
        {
        case eLT_Chinese:
            strCurrLanguage = strChinese;
            break;
        case eLT_English:
            strCurrLanguage = strEnglish;
            break;
        default:
            break;
        }
        m_texts.emplace(static_cast<EText>(i), strCurrLanguage );
    }
}
