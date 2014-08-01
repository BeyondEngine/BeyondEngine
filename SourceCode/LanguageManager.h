#ifndef BEYOND_ENGINE_LANGUAGEMANAGER_H__INCLUDE
#define BEYOND_ENGINE_LANGUAGEMANAGER_H__INCLUDE
#include "Language.h"

enum ELanguage
{
    eLT_Chinese,
    eLT_English,
};

class CLanguageManager
{
    BEATS_DECLARE_SINGLETON(CLanguageManager);

public:
    const TString &GetUtf8Text(EText textid) const;
    TString GetTText(EText textid) const;
    void LoadFromFile(ELanguage language);

private:
    std::map<EText, TString> m_texts;
    TString m_emptyString;
};

#define L10N(textid) \
    CLanguageManager::GetInstance()->GetUtf8Text(textid)

#define L10N_T(textid) \
    CLanguageManager::GetInstance()->GetTText(textid)

#endif