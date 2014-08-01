#ifndef BEYOND_ENGINE_ENGINECENTER_H__INCLUDE
#define BEYOND_ENGINE_ENGINECENTER_H__INCLUDE

#include <thread>

class CEngineCenter
{
    BEATS_DECLARE_SINGLETON(CEngineCenter);
public:
    bool Initialize();
    bool Uninitialize();
    void Update();
    void Render();
    void SetFPS(size_t uFramePerSecond);
    size_t GetFPS() const;
    size_t GetFrameTimeUS() const;
    float GetActualFPS() const;
    bool ShouldUpdateThisFrame();
    void SwitchScene(size_t uFileId);
    size_t GetFrameCounter() const;

    const std::thread::id& GetMainThreadId() const;

    const TString& GetCurPlayingMusic() const;
    void SetCurPlayingMusic(const TString& strMusic);

#ifdef _DEBUG
    void IncreaseDrawCall();
    size_t GetDrawCallLastFrame();
    void ResetDrawCall();
#endif

private:
    bool m_bInitialize;
    size_t m_uFPS;
    size_t m_uFrameTimeUS;
    long long m_uMakeUpTime;
    long long m_uLastUpdateTime;
    size_t m_uUpdateCount;
    float m_fFPSCalcTimeAccum;
    float m_fAcctualFPS;
    size_t m_uNextScene;
    size_t m_uFrameCounter;
    std::thread::id m_mainThreadId;
    static const float FPS_CALC_INTERVAL;
    TString m_strCurPlayingMusic;

#ifdef _DEBUG
    size_t m_uDrawCallCounter;
    size_t m_uDrawCallLastFrame;
#endif
};

#endif