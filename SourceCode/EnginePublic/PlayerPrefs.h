#ifndef BEYOND_ENGINE_PLAYERPREFS_H__INCLUDE
#define BEYOND_ENGINE_PLAYERPREFS_H__INCLUDE

enum EPlayerPrefGlobalKey
{
    ePPGK_LastLoginUserName,
    ePPGK_LastLoginPassword,
    ePPGK_EnableMusic,
    ePPGK_EnableSfx,
    ePPGK_DefaultServer,
    
    ePPGK_Count,
    ePPGK_Force32Bit = 0xFFFFFFFF
};

static const TCHAR* PlayerPrefGlobalKey[] = 
{
    "LastLoginUserName",
    "LastLoginPassword",
    "EnableMusic",
    "EnableSfx",
    "DefaultServer",
};

class CPlayerPrefs
{
    BEATS_DECLARE_SINGLETON(CPlayerPrefs)
public:
    TString GetString(const TString& strKey);
    void SetString(const TString& strKey, const TString& strValue);
    void Save();
    void ClearDataMap();

private:
    void Load();

    TString m_strSavePath;
    std::map<TString, TString> m_dataMap;
};

#endif
