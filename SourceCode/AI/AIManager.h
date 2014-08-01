#ifndef BEYOND_ENGINE_AI_AIMANAGER_H__INCLUDE
#define BEYOND_ENGINE_AI_AIMANAGER_H__INCLUDE

class CAIScheme;
class CAIManager
{
    BEATS_DECLARE_SINGLETON(CAIManager);
public:
    void RegisterScheme(CAIScheme* pScheme);
    void UnregisterScheme(CAIScheme* pScheme);

    CAIScheme* GetScheme(const TString& strSchemeName);

private:
    std::map<TString, CAIScheme*> m_schemeMap;
};

#endif