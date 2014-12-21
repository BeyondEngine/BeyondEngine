#ifndef BEYOND_ENGINE_ENGINECENTER_H__INCLUDE
#define BEYOND_ENGINE_ENGINECENTER_H__INCLUDE

#include "TimeMeter.h"

#ifdef DEVELOP_VERSION
enum class ERenderBatchUsage
{
    eRBU_UI,
    eRBU_Text,
    eRBU_Model,
    eRBU_StaticMesh,
    eRBU_Particle,
    eRBU_Sprite,
    eRBU_Water,
    eRBU_GridAndArea,
    eRBU_BackGround,

    eRBU_Count,
};

static const TCHAR* pszRenderBatchUsage[] =
{
    "UI",
    "Text",
    "Model",
    "Mesh",
    "Particle",
    "Sprite",
    "Water",
    "GridAndArea",
    "Background",

    "Unknown"
};
#endif
class CComponentManagerBase;
class CEngineCenter
{
    BEATS_DECLARE_SINGLETON(CEngineCenter);
public:
    bool Initialize();
    bool Uninitialize();
    void Update(float dt);
    void Render();
    void SetFPS(uint32_t uFramePerSecond);
    uint32_t GetFPS() const;
    uint32_t GetFrameTimeUS() const;
    float GetActualFPS() const;
    bool ShouldUpdateThisFrame();
    void ClearMakeUpTime();
    uint32_t GetFrameCounter() const;
    const std::thread::id& GetMainThreadId() const;
    void DelayCloseFile(uint32_t uFileID);
    void UpdateFPS(float dt);
    bool IsEnableSfx();
    void EnableSfx(bool bEnable);
    bool IsEnableMusic();
    void EnableMusic(bool bEnable);
    CComponentManagerBase* GetComponentManager() const;
#ifdef DEVELOP_VERSION
    void IncreaseDrawCall();
    uint32_t GetDrawCallLastFrame();
    void ResetDrawCall();
#endif
private:
    bool m_bInitialize;
    bool m_bIsEnableSfx;
    bool m_bIsEnableMusic;
    uint32_t m_uFPS;
    uint32_t m_uFrameTimeUS;
    uint64_t m_uMakeUpTime;
    uint64_t m_uLastUpdateTime;
    uint32_t m_uUpdateCount;
    uint32_t m_uFrameCounter;
    float m_fFPSCalcTimeAccum;
    float m_fAcctualFPS;
    std::thread::id m_mainThreadId;
    static const float FPS_CALC_INTERVAL;
    std::set<uint32_t> m_delayCloseFileList;
#ifdef DEVELOP_VERSION
public:
    uint32_t m_uDrawCallCounter;
    uint32_t m_uDrawCallLastFrame;
    uint32_t m_uDrawTriangleCount = 0;
    uint32_t m_uRenderTextCounter = 0;
    bool m_bClearPerformRequest = false;
    uint32_t m_uNestPerformCounter = 0;
    uint32_t m_uTotalRunningTimeMS = 0;
    uint32_t m_uBufferDataInvokeTimes = 0;
    uint32_t m_uBufferDataTransferSize = 0;
    std::map<uint32_t, std::pair<uint32_t, uint32_t>> m_performMap;
    std::map<ERenderBatchUsage, uint32_t> m_renderBatchCount;
    bool m_bSingleScene = false;
#endif
#ifdef EDITOR_MODE
public:
    bool m_bExportMode = false;
    bool m_bDelaySync = false; // Some times we are in property grid's callback, at that time, don't do sync operation.
    std::set<class CPropertyDescriptionBase*> m_editorPropertyGridSyncList;
#endif
};

#endif