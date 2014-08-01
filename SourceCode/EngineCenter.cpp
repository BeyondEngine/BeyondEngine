#include "stdafx.h"
#include "EngineCenter.h"
#include "Render/AnimationManager3D.h"
#include "Render/RenderManager.h"
#include "Resource/ResourceManager.h"
#include "Resource/ResourcePathManager.h"
#include "Render/renderer.h"
#include "Utility/BeatsUtility/ComponentSystem/ComponentPublic.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstanceManager.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Utility/PerformDetector/PerformDetector.h"

#include "GUI/Font/FontManager.h"
#include "Render/TextureFragManager.h"
#include "Render/RenderGroupManager.h"
#include "Render/RenderTarget.h"
#include "GUI/WindowManager.h"
#include "LanguageManager.h"
#include "Render/TextureFormatConverter.h"
#include "SkeletalAnimation/SkeletonAnimationManager.h"
#include "SkeletalAnimation/AnimationImporter.h"
#include "external/Configuration.h"
#include "Event/TouchDelegate.h"
#include "Scene/SceneManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "TimeMeter.h"
#include "Task/TaskManager.h"
#include "Render/Texture.h"
#include "Render/RenderWindow.h"

#ifdef EDITOR_MODE
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProject.h"
#endif
#include "Audio/SimpleAudioEngine.h"
#include "NodeAnimation/NodeAnimationManager.h"

CEngineCenter* CEngineCenter::m_pInstance = NULL;
const float CEngineCenter::FPS_CALC_INTERVAL = 0.2f;

CEngineCenter::CEngineCenter()
    : m_bInitialize(false)
    , m_uFPS(0)
    , m_uFrameTimeUS(0)
    , m_uMakeUpTime(0)
    , m_uLastUpdateTime(0)
    , m_uUpdateCount(0)
    , m_fFPSCalcTimeAccum(0.f)
    , m_fAcctualFPS(0.f)
    , m_uNextScene(0xFFFFFFFF)
    , m_uFrameCounter(0)
#ifdef _DEBUG
    , m_uDrawCallCounter(0)
    , m_uDrawCallLastFrame(0)
#endif
{
    SetFPS(60);
}

CEngineCenter::~CEngineCenter()
{
    CComponentInstanceManager::Destroy();
    CComponentProxyManager::Destroy();
    BEATS_ASSERT(!m_bInitialize, _T("Call uninitialize before engine center destructor."));
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    CPerformDetector::Destroy();
#endif
    _2DSkeletalAnimation::CAnimationImporter::Destroy();
    _2DSkeletalAnimation::CSkeletonAnimationManager::Destroy();
    CRenderGroupManager::Destroy();
    CRenderManager::Destroy();
    CResourcePathManager::Destroy();
    CEnumStrGenerator::Destroy();
    CWindowManager::Destroy();
    CStringHelper::Destroy();
    CResourceManager::Destroy();
    CTextureFragManager::Destroy();
    CAnimationManager3D::Destroy();
    CRenderer::Destroy();
    CLanguageManager::Destroy();
    CTextureFormatConverter::Destroy();
    CFilePathTool::Destroy();
    CConfiguration::Destroy();
    CTouchDelegate::Destroy();
    CSceneManager::Destroy();
    SimpleAudioEngine::Destroy();
#if defined _DEBUG && (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    SymCleanup(GetCurrentProcess());
#endif
}

extern void EngineLaunch();
bool CEngineCenter::Initialize()
{
    m_mainThreadId = std::this_thread::get_id();
#if defined _DEBUG && (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    SymSetOptions(SYMOPT_LOAD_LINES);
    bool bInitializeSuccess = SymInitialize(GetCurrentProcess(), NULL, TRUE) == TRUE;
    BEATS_ASSERT(bInitializeSuccess);
#endif

    BEATS_ASSERT(!m_bInitialize, _T("Can't Initialize CEngineCenter twice!"));
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
    CPerformDetector::GetInstance()->SetTypeName(pszPerformNodeStr, sizeof(pszPerformNodeStr));
#endif
    CRenderManager::GetInstance()->Initialize();
    CTaskManager::GetInstance()->Initialize();
    CWindowManager::GetInstance()->Initialize();

    EngineLaunch();
    m_bInitialize = true;
    return true;
}

bool CEngineCenter::Uninitialize()
{
    BEATS_ASSERT(m_bInitialize, _T("Engine center is not initialized when call Uninitialize."));
    // Uninitialize all instance will lead all proxys are also uninitialized
    // Template instances are not initialized, so don't uninitialize them.
    // But all template proxy are needed.
    CComponentInstanceManager::GetInstance()->UninitializeAllInstance();
    CComponentProxyManager::GetInstance()->UninitializeAllTemplate();
    m_bInitialize = false;
    return true;
}

void CEngineCenter::Update()
{
    if (m_uNextScene != 0xFFFFFFFF)
    {
        CSceneManager::GetInstance()->SwitchScene(m_uNextScene);
        m_uNextScene = 0xFFFFFFFF;
    }
    static CTimeMeter meter;
    float dt = meter.GetDeltaSec();
    CTaskManager::GetInstance()->ExecuteSyncTask(dt);
    CTaskManager::GetInstance()->Update();

    BEYONDENGINE_PERFORMDETECT_START(ePNT_GUI_Update);
    CWindowManager::GetInstance()->Update( dt );
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_GUI_Update);

    BEYONDENGINE_PERFORMDETECT_START(ePNT_CSenceManager);
    CSceneManager::GetInstance()->UpdateScene( dt );
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_CSenceManager);

    CNodeAnimationManager::GetInstance()->Update(dt);
    ++m_uFrameCounter;
}

void CEngineCenter::Render()
{
    BEATS_ASSERT(m_uDrawCallCounter == 0, _T("It's forbid to render outside render function."));
    static CTimeMeter meter;
    float dt = meter.GetDeltaSec();

    BEYONDENGINE_PERFORMDETECT_START(ePNT_Render);
    CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    BEATS_ASSERT(pRenderTarget != nullptr, _T("RenderTarget can't be null!"));
    pRenderTarget->Render();
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_Render);

    ++m_uUpdateCount;
    m_fFPSCalcTimeAccum += dt;
    if(m_fFPSCalcTimeAccum >= FPS_CALC_INTERVAL)
    {
        m_fAcctualFPS = m_uUpdateCount / m_fFPSCalcTimeAccum;
        m_uUpdateCount = 0;
        m_fFPSCalcTimeAccum = 0.f;
    }
#ifdef _DEBUG
    ResetDrawCall();
#endif
}

void CEngineCenter::SetFPS(size_t uFramePerSecond)
{
    m_uFPS = uFramePerSecond;
    m_uFrameTimeUS = 1000000 / m_uFPS;
}

size_t CEngineCenter::GetFPS() const
{
    return m_uFPS;
}

size_t CEngineCenter::GetFrameTimeUS() const
{
    return m_uFrameTimeUS;
}

float CEngineCenter::GetActualFPS() const
{
    return m_fAcctualFPS;
}

bool CEngineCenter::ShouldUpdateThisFrame()
{
    long long uCurrTime = CTimeMeter::GetCurrUSec();
    bool bRet = false;

    if (m_uLastUpdateTime== 0)
    {
        bRet = true;
    }
    else
    {
        long long uTimeOffSet = uCurrTime - m_uLastUpdateTime;
        if (uTimeOffSet + m_uMakeUpTime >= m_uFrameTimeUS)
        {
            m_uMakeUpTime = m_uMakeUpTime + uTimeOffSet - m_uFrameTimeUS;
            bRet = true;
        }
    }

    if(bRet)
    {
        m_uLastUpdateTime = uCurrTime;
    }
    return bRet;
}

void CEngineCenter::SwitchScene(size_t uFileId)
{
    m_uNextScene = uFileId;
}

size_t CEngineCenter::GetFrameCounter() const
{
    return m_uFrameCounter;
}

const std::thread::id& CEngineCenter::GetMainThreadId() const
{
    return m_mainThreadId;
}

const TString& CEngineCenter::GetCurPlayingMusic() const
{
    return m_strCurPlayingMusic;
}

void CEngineCenter::SetCurPlayingMusic(const TString& strMusic)
{
    m_strCurPlayingMusic = strMusic;
}

#ifdef _DEBUG

void CEngineCenter::IncreaseDrawCall()
{
    ++m_uDrawCallCounter;
}

size_t CEngineCenter::GetDrawCallLastFrame()
{
    return m_uDrawCallLastFrame;
}

void CEngineCenter::ResetDrawCall()
{
    m_uDrawCallLastFrame = m_uDrawCallCounter;
    m_uDrawCallCounter = 0;
}

#endif