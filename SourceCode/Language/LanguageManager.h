#ifndef BEYOND_ENGINE_LANGUAGEMANAGER_H__INCLUDE
#define BEYOND_ENGINE_LANGUAGEMANAGER_H__INCLUDE
#include "Language.h"

enum ELanguageType
{
    eLT_Chinese,
    eLT_English,
    eLT_Traditional,
    eLT_French,
    eLT_Korean,
    eLT_Japanese,
    eLT_Russian,
    eLT_Italian,
    eLT_German,
    eLT_Thai,
    eLT_Spanish,
    eLT_Portuguese,

    eLT_Count,
    eLT_Force32Bit = 0xFFFFFFFF
};

static const TCHAR* pszLanguageTypeString[] =
{
    _T("chinese"),
    _T("english"),
    _T("traditional"),
    _T("french"),
    _T("korean"),
    _T("japanese"),
    _T("russian"),
    _T("italian"),
    _T("german"),
    _T("thai"),
    _T("spanish"),
    _T("portuguese"),
};

class CLanguageText;
class CLanguageManager
{
    BEATS_DECLARE_SINGLETON(CLanguageManager);
public:
    const TString& GetUtf8Text(ELanguageTextType textid) const;
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    TString GetTText(ELanguageTextType textid) const;
#endif
    void RegisterLanguageText(CLanguageText* pText);
    void UnregisterLanguageText(CLanguageText* pText);
    void SetCurrentLanguage(ELanguageType type, bool bForceLoad = false);
    ELanguageType GetCurrentLanguage() const;
#ifdef EDITOR_MODE
    std::map<TString, std::map<ELanguageType, TString> >& GetLanguageMap();
    std::map<TString, TString>& GetLanguageTagMap();
    std::set<TString>& GetExportLanguageList();
    void SaveLanguageListToFile();
#endif
    void LoadFromFile(ELanguageType language);

private:
    ELanguageType m_currLanguageType;
    std::set<CLanguageText*> m_refreshLanguageTextList;
    std::mutex m_mutex;
#ifdef EDITOR_MODE
    std::map<TString, std::map<ELanguageType, TString> > m_languageMap;
    std::map<TString, TString> m_languageTagMap;
    std::set<TString> m_exportLanguageList;
#else
    std::map<ELanguageTextType, TString> m_texts;
#endif
};

#define L10N(textid) \
    CLanguageManager::GetInstance()->GetUtf8Text(textid)

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
#define L10N_T(textid) \
    CLanguageManager::GetInstance()->GetTText(textid)
#endif

#endif