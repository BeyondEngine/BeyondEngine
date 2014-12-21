#include "stdafx.h"
#include "Application.h"
#include "BeatsTchar.h"
#include "EnginePublic/PublicDef.h"
#include "Render/CommonTypes.h"
#include "Resource/ResourcePublic.h"
#include "Render/Shader.h"
#include "Resource/ResourceManager.h"
#include "Render/RenderManager.h"
#include "Render/RenderPublic.h"
#include "Render/ShaderProgram.h"
#include "Render/Camera.h"
#include "Render/RenderGroupManager.h"
#include "external/Configuration.h"
#include "Render/Renderer.h"
#include "Audio/include/AudioEngine.h"
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID || BEYONDENGINE_PLATFORM == PLATFORM_IOS)
#include "Framework/PlatformHelper.h"
#endif

CApplication *CApplication::m_sMainApplication = nullptr;

CApplication::CApplication()
    : m_bRunning(true)
    , m_bDestructing(false)
#ifdef EDITOR_MODE
    , m_uPauseDuration(0)
#endif
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    , m_bInitialize(false)
#endif
{
    BEATS_ASSERT(!m_sMainApplication, _T("You should initialize only one application"));
    m_sMainApplication = this;
}

CApplication::~CApplication()
{
    m_sMainApplication = nullptr;
}

void CApplication::Initialize()
{
    CEngineCenter::GetInstance()->Initialize();
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID || BEYONDENGINE_PLATFORM == PLATFORM_IOS)
    CPlatformHelper::GetInstance()->InitSenorsManager();
#endif
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    m_bInitialize = true;
#endif
}

void CApplication::Uninitialize()
{
    m_bDestructing = true;
}

float CApplication::Update()
{
    float dt = 0.0f;
    if (IsRunning())
    {
        m_updateTimeMeter.Tick();
        dt = m_updateTimeMeter.GetDeltaSec();
    #ifdef EDITOR_MODE
        dt *= m_fTimeScale;
    #endif
        CEngineCenter::GetInstance()->Update(dt);
        CEngineCenter::GetInstance()->UpdateFPS(dt);
    }
#ifdef EDITOR_MODE
    else
    {
        m_uPauseDuration += (uint64_t)(m_updateTimeMeter.GetDeltaSec() * 1000000);
    }
#endif
    return dt;
}

void CApplication::Render()
{
#if defined(DISABLE_RENDER)
    return;
#else
#ifdef DEVELOP_VERSION
    uint32_t uStartTimeMS = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000);
#endif
    CEngineCenter::GetInstance()->Render();
#ifdef DEVELOP_VERSION
    CEngineCenter::GetInstance()->m_uTotalRunningTimeMS += ((uint32_t)(CTimeMeter::GetCurrUSec() / 1000) - uStartTimeMS);
#endif
#endif

}

void CApplication::Pause()
{
    m_bRunning = false;
    CAudioEngine::PauseAll();
    m_uPauseTime = m_updateTimeMeter.GetCurrUSec();
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID || BEYONDENGINE_PLATFORM == PLATFORM_IOS)
    CPlatformHelper::GetInstance()->DisableScreenSleep(false);
#endif
#ifdef USE_SDK
    if (eSRGM_Loading != CStarRaidersCenter::GetInstance()->GetGameMode()
        &&eSRGM_Count != CStarRaidersCenter::GetInstance()->GetGameMode())
    {
        CSDKManager::GetInstance()->SetLocalPushes();
    }
#endif
}

void CApplication::Resume()
{
    m_bRunning = true;
    CAudioEngine::ResumeAll();
    m_uResumeTimeUS = m_updateTimeMeter.GetCurrUSec();
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID || BEYONDENGINE_PLATFORM == PLATFORM_IOS)
    const uint64_t uMaxPauseTime = 60 * 1000000;
    if (m_uResumeTimeUS - m_uPauseTime > uMaxPauseTime && m_uPauseTime > 0)
    {
        m_uPauseTime = 0;
        CStarRaidersCenter::GetInstance()->SetNeedRestart(true);
    }
#endif
#ifdef USE_SDK
    CSDKManager::GetInstance()->ClearLocalPushes();
#endif
}

bool CApplication::IsRunning() const
{
    return m_bRunning;
}

bool CApplication::IsDestructing() const
{
    return m_bDestructing;
}

CTimeMeter& CApplication::GetUpdateTimeMeter()
{
    return m_updateTimeMeter;
}

CApplication *CApplication::GetInstance()
{
    return m_sMainApplication;
}

void CApplication::Destroy()
{
    BEATS_ASSERT(m_sMainApplication, _T("You should implement and initialize an application"));
    BEATS_SAFE_DELETE(m_sMainApplication);
}

uint64_t CApplication::GetResumeTimeUS() const
{ 
    return m_uResumeTimeUS;
}

uint32_t CApplication::GetFrameTimeMS() const
{
    uint64_t uFrameTime = m_updateTimeMeter.GetCurrentFrameTimeUS();
#ifdef EDITOR_MODE
    uFrameTime -= m_uPauseDuration;
#endif
    return Conver64To32(uFrameTime / 1000);
}

uint32_t CApplication::Conver64To32(uint64_t uValue)
{
    uint32_t uRet = uValue & 0xFFFFFFFF;
    return uRet;
}