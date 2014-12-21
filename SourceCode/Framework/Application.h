#ifndef BEYOND_ENGINE_FRAMEWORK_APPLICATION_H__INCLUDE
#define BEYOND_ENGINE_FRAMEWORK_APPLICATION_H__INCLUDE

#include "EnginePublic/TimeMeter.h"

class CApplication
{
public:
    CApplication();
    virtual ~CApplication();

    int Run(int argc, char * argv[], void* pData = nullptr);
    virtual void Initialize();
    virtual float Update();
    virtual void Render();
    virtual void Uninitialize();
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    bool IsInitialized() const
    {
        return m_bInitialize;
    }
#endif
    virtual void Pause();
    virtual void Resume();
    bool IsRunning() const;
    bool IsDestructing() const;
    uint32_t GetFrameTimeMS() const;
    CTimeMeter& GetUpdateTimeMeter();
    uint64_t GetResumeTimeUS() const;

    static CApplication *GetInstance();
    static void Destroy();
    static uint32_t Conver64To32(uint64_t uValue);
#ifdef EDITOR_MODE
    uint64_t m_uPauseDuration;
    float m_fTimeScale = 1.0f;
#endif
    void OnSwitchToBackground();
    void OnSwitchToForeground();

private:
    uint64_t m_uResumeTimeUS = 0;
    uint64_t m_uPauseTime = 0;
    uint64_t m_uServerSyncTime = 0;
    uint64_t m_uLocalStartTime = 0;
    bool m_bRunning;
    bool m_bDestructing;
    CTimeMeter m_updateTimeMeter;
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    bool m_bInitialize;
#endif
    static CApplication *m_sMainApplication;
};

#endif /* defined(__BEYONDENGINE__Application__) */
