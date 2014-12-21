#ifndef BEYOND_ENGINE_LOG_LOGMANAGER_H__INCLUDE
#define BEYOND_ENGINE_LOG_LOGMANAGER_H__INCLUDE

#include "Render/CommonTypes.h"

#ifdef DEVELOP_VERSION
enum class ELogType
{
    eLT_Info,
    eLT_Warning,
    eLT_Error,

    eLT_Count,
};

enum class ELogChannel
{
    eLC_Log,
    eLC_Info,
    eLC_Perform,
    eLC_Resource,

    eLC_Count,
};

static unsigned int LogTypeColor[] = { 0x00FF00FF, 0xFFFF00FF, 0xFF0000FF };

struct SLog
{
    void Reset()
    {
        m_uTimeMS = 0;
        m_type = ELogType::eLT_Count;
        m_strCatalog.clear();
        m_color = 0;
        m_strLogLineList.clear();
    }
    uint32_t m_uTimeMS = 0;
    ELogType m_type = ELogType::eLT_Count;
    int m_color = 0;
    TString m_strCatalog;
    std::vector<TString> m_strLogLineList;
};
class CFreetypeFontFace;
class CLogManager
{
    BEATS_DECLARE_SINGLETON(CLogManager);

public:
    void Initialize();
    void Uninitialize();
    void WriteLog(ELogType type, const TString& strLog, int nColor = 0, const TCHAR* pszCatalog = nullptr);
    const std::vector<SLog*>& GetLogList() const;
    void RenderLog();
    void SetRenderLogType(ELogType type);
    void SetRenderCatalog(const TString& strCatalog);
    void Show(bool bShow);
    bool IsShown() const;
    void IncreaseRenderPos();
    void DecreaseRenderPos();
    void ClearRenderPos();
    void SetRenderPosToTop();
    void Clear();
    TString GetLastLogList(uint32_t uCount);
    uint32_t GetLogFontHeight() const;
    bool SwitchChannel(const CVec2& pos);
    ELogChannel GetLogChannel() const;
    static CFreetypeFontFace* m_pLogFont;

private:
    SLog* RequestLog();

private:
    CRect m_logSelector;
    CRect m_infoSelector;
    CRect m_pauseGameSelector;
    CRect m_perfromSelector;
    CRect m_resourceSelector;
    ELogChannel m_channel = ELogChannel::eLC_Log;
    uint32_t m_uStartRenderLogIndex = 0;
    uint32_t m_uResourceStartLine = 0;
    bool m_bShown = false;
    TString m_strRenderCatalog;
    ELogType m_renderType = ELogType::eLT_Count;
    std::vector<SLog*> m_logList;
    std::vector<SLog*> m_logPool;
    std::recursive_mutex m_mutex;
};
#ifdef BEATS_LOG
#undef BEATS_LOG
#endif
#define BEATS_LOG(type, logString, ...) \
{if (CLogManager::m_pLogFont != nullptr)\
{CLogManager::GetInstance()->WriteLog(type, logString, __VA_ARGS__); }}
#else
#define BEATS_LOG(type, logString, ...)
#endif
#endif
