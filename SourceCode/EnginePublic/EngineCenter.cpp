#include "stdafx.h"
#include "EngineCenter.h"
#include "Render/RenderManager.h"
#include "Resource/ResourceManager.h"
#include "Render/Renderer.h"
#include "Component/ComponentPublic.h"
#include "Component/Component/ComponentInstanceManager.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Utility/PerformDetector/PerformDetector.h"

#include "GUI/Font/FontManager.h"
#include "Render/RenderGroupManager.h"
#include "Render/RenderTarget.h"
#include "Render/TextureFormatConverter.h"
#include "external/Configuration.h"
#include "Event/TouchDelegate.h"
#include "Scene/SceneManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "TimeMeter.h"
#include "Task/TaskManager.h"
#include "Render/Texture.h"
#ifdef EDITOR_MODE
#include "Component/Component/ComponentProxyManager.h"
#include "Component/Component/ComponentProject.h"
#include "BeyondEngineEditor/ComponentGraphics_GL.h"
#endif
#include "NodeAnimation/NodeAnimationManager.h"
#include "Scene/Scene.h"
#include "Framework/Application.h"
#include "Audio/include/AudioEngine.h"
#ifdef USE_VLD
#include "../Include/vld/include/vld.h"
#endif

#include "Script/ScriptManager.h"
#include "MathExt/Ray3.h"
#include "MathExt/Plane.h"
#include "PlayerPrefs.h"
#include "ParticleSystem/ParticleManager.h"
#include "BeatsUtility/md5.h"
#include "BeyondEngineVersion.h"
#include "Render/Material.h"
#include "Language/LanguageManager.h"
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
#include <shellapi.h>
#include "GUI/Font/FontFace.h"
#endif
#if (BEYONDENGINE_PLATFORM != PLATFORM_WIN32)
#include "Framework/PlatformHelper.h"
#endif
#if (BEYONDENGINE_PLATFORM == PLATFORM_IOS || BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
#include "XGSdkImplement.h"
#include "SuperSdkImplement.h"
#endif

CEngineCenter* CEngineCenter::m_pInstance = NULL;
const float CEngineCenter::FPS_CALC_INTERVAL = 0.2f;

// Add two static variable to pass the link in android mode.
static CRay3 Force_Link_Ray3;
static CPlane Force_Link_Panel;
static CMD5 Force_Link_MD5;
#ifdef DEVELOP_VERSION
std::vector<TString> g_registeredSingleton;
#endif

CEngineCenter::CEngineCenter()
    : m_bInitialize(false)
    , m_bIsEnableSfx(true)
    , m_bIsEnableMusic(true)
    , m_uFPS(0)
    , m_uFrameTimeUS(0)
    , m_uMakeUpTime(0)
    , m_uLastUpdateTime(0)
    , m_uUpdateCount(0)
    , m_fFPSCalcTimeAccum(0.f)
    , m_fAcctualFPS(0.f)
    , m_uFrameCounter(0)
#ifdef DEVELOP_VERSION
    , m_uDrawCallCounter(0)
    , m_uDrawCallLastFrame(0)
#endif
{
}

CEngineCenter::~CEngineCenter()
{
    CAudioEngine::End();
    DESTROY_SINGLETON(CComponentInstanceManager);
#ifdef EDITOR_MODE
    DESTROY_SINGLETON(CComponentProxyManager);
#endif
    DESTROY_SINGLETON(CScriptManager);
    DESTROY_SINGLETON(CFontManager);
    DESTROY_SINGLETON(CParticleManager);
    BEATS_ASSERT(!m_bInitialize, _T("Call uninitialize before engine center destructor."));
#ifdef EDITOR_MODE
    DESTROY_SINGLETON(CPerformDetector);
#endif
    DESTROY_SINGLETON(CRenderGroupManager);
    DESTROY_SINGLETON(CRenderManager);
    DESTROY_SINGLETON(CTextureFormatConverter);
    DESTROY_SINGLETON(CConfiguration);
    DESTROY_SINGLETON(CTouchDelegate);
    DESTROY_SINGLETON(CSceneManager);
    DESTROY_SINGLETON(CLanguageManager);
    DESTROY_SINGLETON(CTaskManager);
    DESTROY_SINGLETON(CPlayerPrefs);
    DESTROY_SINGLETON(CResourceManager);
    DESTROY_SINGLETON(CNodeAnimationManager);
    DESTROY_SINGLETON(CRenderer);
    DESTROY_SINGLETON(CFilePathTool);
#if defined _DEBUG && (BEYONDENGINE_PLATFORM == PLATFORM_WIN32) && defined(SHARE_PTR_TRACE)
    SymCleanup(GetCurrentProcess());
#endif
#ifdef DEVELOP_VERSION
    DESTROY_SINGLETON(CLogManager);
#endif
    DESTROY_SINGLETON(CStringHelper);
#ifdef EDITOR_MODE
    DESTROY_SINGLETON(CEnumStrGenerator);
    DESTROY_SINGLETON(CUtilityManager);
#endif

#if (BEYONDENGINE_PLATFORM == PLATFORM_IOS || BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    DESTROY_SINGLETON(CPlatformHelper);
#endif

#ifdef DEVELOP_VERSION
    for (auto iter = g_registeredSingleton.begin(); iter != g_registeredSingleton.end(); ++iter)
    {
        BEATS_PRINT("Singleton %s is not destroyed!\n", (*iter).c_str());
    }
#endif
    BEYONDENGINE_CHECK_HEAP;
}
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32 && defined DEVELOP_VERSION
TString ConvertExceptionCode(uint32_t uExceptionCode)
{
    TString strRet = "Unknown";
    switch (uExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:
        strRet = "ACCESS_VIOLATION";
        break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        strRet = "DATATYPE_MISALIGNMENT";
        break;
    case EXCEPTION_BREAKPOINT:
        strRet = "BREAKPOINT";
        break;
    case EXCEPTION_SINGLE_STEP:
        strRet = "SINGLE_STEP";
        break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        strRet = "ARRAY_BOUNDS_EXCEEDED";
        break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        strRet = "FLT_DENORMAL_OPERAND";
        break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        strRet = "FLT_DIVIDE_BY_ZERO";
        break;
    case EXCEPTION_FLT_INEXACT_RESULT:
        strRet = "FLT_INEXACT_RESULT";
        break;
    case EXCEPTION_FLT_INVALID_OPERATION:
        strRet = "FLT_INVALID_OPERATION";
        break;
    case EXCEPTION_FLT_OVERFLOW:
        strRet = "FLT_OVERFLOW";
        break;
    case EXCEPTION_FLT_STACK_CHECK:
        strRet = "FLT_STACK_CHECK";
        break;
    case EXCEPTION_FLT_UNDERFLOW:
        strRet = "FLT_UNDERFLOW";
        break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        strRet = "INT_DIVIDE_BY_ZERO";
        break;
    case EXCEPTION_INT_OVERFLOW:
        strRet = "INT_OVERFLOW";
        break;
    case EXCEPTION_PRIV_INSTRUCTION:
        strRet = "PRIV_INSTRUCTION";
        break;
    case EXCEPTION_IN_PAGE_ERROR:
        strRet = "IN_PAGE_ERROR";
        break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        strRet = "ILLEGAL_INSTRUCTION";
        break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        strRet = "NONCONTINUABLE_EXCEPTION";
        break;
    case EXCEPTION_STACK_OVERFLOW:
        strRet = "STACK_OVERFLOW";
        break;
    case EXCEPTION_INVALID_DISPOSITION:
        strRet = "INVALID_DISPOSITION";
        break;
    case EXCEPTION_GUARD_PAGE:
        strRet = "GUARD_PAGE";
        break;
    case EXCEPTION_INVALID_HANDLE:
        strRet = "INVALID_HANDLE";
        break;
    default:
        break;
    }
    return strRet;
}

void PrintStack(CSerializer& serialzier, PCONTEXT pContext)
{
    STACKFRAME64 sf;
    memset(&sf, 0, sizeof(sf));
    SYMBOL_INFO_PACKAGE sip = { 0 };
    IMAGEHLP_LINE64 lineinfo = { 0 };
    DWORD64 dw64Displacement = 0;
    DWORD dwLineDisplacement;
#if defined(_WIN64)
    DWORD dwMachineType = IMAGE_FILE_MACHINE_AMD64;
    sf.AddrPC.Offset = pContext->Rip;
    sf.AddrFrame.Offset = pContext->Rsp;
    sf.AddrStack.Offset = pContext->Rsp;
#else
    DWORD dwMachineType = IMAGE_FILE_MACHINE_I386;
    sf.AddrPC.Offset = pContext->Eip;
    sf.AddrFrame.Offset = pContext->Ebp;
    sf.AddrStack.Offset = pContext->Esp;
#endif
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrFrame.Mode = AddrModeFlat;
    sf.AddrStack.Mode = AddrModeFlat;

    HANDLE hprocess = GetCurrentProcess();
    SymSetOptions(SYMOPT_DEFERRED_LOADS);
    SymSetOptions(SYMOPT_LOAD_LINES);
    bool bInitializeSuccess = SymInitialize(hprocess, NULL, TRUE) == TRUE;
    BEATS_ASSERT(bInitializeSuccess);
    BEYONDENGINE_UNUSED_PARAM(bInitializeSuccess);
    serialzier << "Callstack:\r\n";
    while (TRUE)
    {
        // Get the next stack frame
        if (!StackWalk64(dwMachineType, hprocess, GetCurrentThread(), &sf, pContext, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0))
            break;
        if (0 == sf.AddrFrame.Offset)
            break;
        sip.si.SizeOfStruct = sizeof(SYMBOL_INFO);
        sip.si.MaxNameLen = sizeof(sip.name);
        if (SymFromAddr(hprocess, sf.AddrPC.Offset, &dw64Displacement, &sip.si))
        {
            serialzier << sip.si.Name;
        }
        lineinfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        if (SymGetLineFromAddr64(hprocess, sf.AddrPC.Offset, &dwLineDisplacement, &lineinfo))
        {
            TCHAR szBuffer[MAX_PATH];
            _stprintf(szBuffer, "    %s line %d\r\n", lineinfo.FileName, lineinfo.LineNumber);
            serialzier << szBuffer;
        }
        else
        {
            if (GetLastError() == 0x1E7)
            {  // If err_code == 0x1e7, no symbol was found.
                serialzier << "No debug symbol loaded for this function.\r\n";
            }
            else
            {
                serialzier << "unknown info.\r\n";
            }
        }
    }
    SymCleanup(hprocess);
}

LONG WINAPI BeyondEngineHandleWin32Exception(_EXCEPTION_POINTERS* einfo)
{
    TString strDumpPath = CResourceManager::GetInstance()->GetResourcePath(eRT_Resource);
    strDumpPath.append("/../Dump");
    CFilePathTool::GetInstance()->MakeDirectory(strDumpPath.c_str());
    // Generate log file.
    TCHAR szBuffer[1024];
    CSerializer dumpData;
    _stprintf(szBuffer, _T("Exception:\r\nAddress: 0x%p\r\n%s 0x%x\r\n"), einfo->ExceptionRecord->ExceptionAddress, ConvertExceptionCode(einfo->ExceptionRecord->ExceptionCode).c_str(), einfo->ExceptionRecord->ExceptionCode);
    dumpData << szBuffer;
    PrintStack(dumpData, einfo->ContextRecord);
    dumpData << "\r\n\r\n";

    MEMORYSTATUS mem_status;
    mem_status.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&mem_status);
    _stprintf(szBuffer, _T("Physics Memory : %d/%d MB\r\n"), mem_status.dwAvailPhys / 1024 / 1024, mem_status.dwTotalPhys / 1024 / 1024);
    dumpData << szBuffer;
    _stprintf(szBuffer, _T("Virtual Memory : %d/%d MB\r\n"), mem_status.dwAvailVirtual / 1024 / 1024, mem_status.dwTotalVirtual / 1024 / 1024);
    dumpData << szBuffer;
    _stprintf(szBuffer, _T("Page File : %d/%d MB\r\n"), mem_status.dwAvailPageFile / 1024 / 1024, mem_status.dwTotalPageFile / 1024 / 1024);
    dumpData << szBuffer;

    dumpData << "\r\n\r\n";
    dumpData << "Log:\r\n";
    const std::vector<SLog*>& logList = CLogManager::GetInstance()->GetLogList();
    for (size_t i = 0; i < logList.size(); ++i)
    {
        for (size_t j = 0; j < logList[i]->m_strLogLineList.size(); ++j)
        {
            TString& strLog = logList[i]->m_strLogLineList[j];
            if (!strLog.empty())
            {
                dumpData << strLog << "\r\n";
            }
        }
    }
    SYSTEMTIME st;
    ::GetLocalTime(&st);
    // Generate dump file.
    _stprintf(szBuffer, _T("BeyondEngineReport-%d_%d_%d-%04d%02d%02d-%02d%02d%02d-%03d.dmp"),
        BEYONDENGINE_VER_MAJOR,
        BEYONDENGINE_VER_MINOR,
        BEYONDENGINE_VER_REVISION,
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond,
        st.wMilliseconds);
    strDumpPath.append("/").append(szBuffer);
    HANDLE hDumpFile = CreateFile(strDumpPath.c_str(), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
    MINIDUMP_EXCEPTION_INFORMATION ExpParam;
    ExpParam.ThreadId = GetCurrentThreadId();
    ExpParam.ExceptionPointers = einfo;
    ExpParam.ClientPointers = TRUE;

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &ExpParam, NULL, NULL);
    CloseHandle(hDumpFile);

    _stprintf(szBuffer, _T("BeyondEngineReport-%d_%d_%d-%04d%02d%02d-%02d%02d%02d-%03d.txt"),
        BEYONDENGINE_VER_MAJOR,
        BEYONDENGINE_VER_MINOR,
        BEYONDENGINE_VER_REVISION,
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond,
        st.wMilliseconds);
    strDumpPath = CFilePathTool::GetInstance()->ParentPath(strDumpPath.c_str());
    strDumpPath.append("/").append(szBuffer);
    dumpData.Deserialize(strDumpPath.c_str(), "wt+");
    if (MessageBox(NULL, _T("Sorry, Application Crashed! Click yes to open the report."), _T("BeyondEngine"), MB_ICONHAND | MB_YESNO) == IDYES)
    {
        ::ShellExecute(NULL, _T("open"), _T("notepad"), strDumpPath.c_str(), _T(""), SW_SHOWNORMAL);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif
extern void EngineLaunch();
bool CEngineCenter::Initialize()
{
    BEATS_ASSERT(sizeof(COMMON_UNIFORM_NAMES) / sizeof(char*) == UNIFORM_COUNT);
    BEATS_ASSERT(sizeof(COMMON_ATTIB_NAMES) / sizeof(char*) == ATTRIB_INDEX_COUNT);
    BEATS_ASSERT(sizeof(pszResourceTypeString) / sizeof(char*) == eRT_Count);
    BEATS_ASSERT(sizeof(pszResourcePathName) / sizeof(char*) == eRT_Count);

    SetFPS(45);
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32 && defined DEVELOP_VERSION
    SetUnhandledExceptionFilter(BeyondEngineHandleWin32Exception);
    // Write return code to kernel32.dll's SetUnhandledExceptionFilter, so nothing can call SetUnhandledExceptionFilter again
    // to make sure BeyondEngineHandleWin32Exception will be the only callback function.
    VOID* procSetUnhandledExceptionFilter = (VOID*)GetProcAddress(LoadLibrary("kernel32.dll"), "SetUnhandledExceptionFilter");
    if (procSetUnhandledExceptionFilter)
    {
        DWORD dwOldFlag, dwTempFlag;
        UCHAR code[16];
        INT size = 0;
        code[size++] = 0x33;
        code[size++] = 0xC0;
        code[size++] = 0xC2;
        code[size++] = 0x04;
        code[size++] = 0x00;
        VirtualProtect(procSetUnhandledExceptionFilter, size, PAGE_READWRITE, &dwOldFlag);
        WriteProcessMemory(GetCurrentProcess(), procSetUnhandledExceptionFilter, code, size, NULL);
        VirtualProtect(procSetUnhandledExceptionFilter, size, dwOldFlag, &dwTempFlag);
    }
#endif
    m_mainThreadId = std::this_thread::get_id();
#ifdef _DEBUG
    std::stringstream strm;
    strm << m_mainThreadId;
    BEATS_PRINT("Main thread launched with id %s\n", strm.str().c_str());
#endif
#if defined _DEBUG && (BEYONDENGINE_PLATFORM == PLATFORM_WIN32) && defined SHARE_PTR_TRACE
    SymSetOptions(SYMOPT_LOAD_LINES);
    bool bInitializeSuccess = SymInitialize(GetCurrentProcess(), NULL, TRUE) == TRUE;
    BEATS_ASSERT(bInitializeSuccess);
#endif
    CRenderer::GetInstance()->Initialize();
    #ifdef DEVELOP_VERSION
        CLogManager::GetInstance()->Initialize();
    #endif
        BEATS_ASSERT(!m_bInitialize, _T("Can't Initialize CEngineCenter twice!"));
    #ifdef EDITOR_MODE
        CPerformDetector::GetInstance()->SetTypeName(pszPerformNodeStr, sizeof(pszPerformNodeStr) / sizeof(char*));
    #endif
    #ifndef EDITOR_MODE
        CLanguageManager::GetInstance()->SetCurrentLanguage(eLT_Chinese, true);
    #endif
        CConfiguration *conf = CConfiguration::GetInstance();
        conf->GatherDeviceInfo();
        BEATS_PRINT(_T("os: %s\n"), conf->GetOsInfo().c_str());
        BEATS_PRINT(_T("model: %s\n"), conf->GetModelInfo().c_str());
        BEATS_PRINT(_T("deviceType: %s\n"), strDeviceType[conf->GetDeviceType()]);
        BEATS_PRINT(_T("networkstate: %s\n"), strNetState[conf->GetNetworkState()]);

        CRenderManager::GetInstance()->Initialize();
        BEATS_PRINT(_T("RenderManager Initialized!\n"));
        CTaskManager::GetInstance()->Initialize();
        BEATS_PRINT(_T("TaskManager Initialized!\n"));
#ifndef DISABLE_MULTI_THREAD
        CAudioEngine::LazyInit();
#endif
    #ifdef EDITOR_MODE
        CEngineCenter::GetInstance()->m_bExportMode = true;
    #endif
    EngineLaunch();
    BEATS_PRINT(_T("EngineLaunch Finished!\n"));
#ifdef EDITOR_MODE
    CEngineCenter::GetInstance()->m_bExportMode = false;
#endif
    CScriptManager::GetInstance()->Initialize();
    m_bInitialize = true;
    return true;
}

bool CEngineCenter::Uninitialize()
{
    BEATS_ASSERT(m_bInitialize, _T("Engine center is not initialized when call Uninitialize."));
    BEATS_ASSERT(GetComponentManager()->GetLoadedFiles().size() == 0);
    //TODO: try to figure out why only proxy manager need uninitialize all templates.
#ifdef EDITOR_MODE
    down_cast<CComponentProxyManager*>(GetComponentManager())->UninitializeAllTemplate();
#else
    BEATS_ASSERT(GetComponentManager()->GetLoadedFiles().size() == 0);
#endif
    CRenderManager::GetInstance()->Uninitialize();
#ifdef DEVELOP_VERSION
    CLogManager::GetInstance()->Uninitialize();
#endif
    m_bInitialize = false;
    return true;
}

void CEngineCenter::Update(float dt)
{
#ifdef DEVELOP_VERSION
    BEATS_ASSERT(m_uDrawCallCounter == 0, _T("It's forbid to render outside render function, current draw call counter:%d."), m_uDrawCallCounter);
#endif
    ++m_uFrameCounter;
    CTaskManager::GetInstance()->Update(dt);
        BEYONDENGINE_PERFORMDETECT_START(ePNT_CSenceManager);
        CSceneManager::GetInstance()->UpdateScene(dt);
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_CSenceManager);
        BEYONDENGINE_PERFORMDETECT_START(ePNT_ParticleUpdate);
        if (CSceneManager::GetInstance()->GetUpdateSwitcher())
        {
            CParticleManager::GetInstance()->Update(dt);
        }
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_ParticleUpdate);
#ifndef DISABLE_MULTI_THREAD
        CAudioEngine::Update(dt);
#endif
        BEYONDENGINE_PERFORMDETECT_START(ePNT_NodeAnimation);
        CNodeAnimationManager::GetInstance()->Update(dt);
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_NodeAnimation);
    if (m_delayCloseFileList.size() > 0 && CTaskManager::GetInstance()->IsAllAyncTaskFinished())
    {
        for (auto iter = m_delayCloseFileList.begin(); iter != m_delayCloseFileList.end(); ++iter)
        {
            uint32_t uFileId = *iter;
            GetComponentManager()->CloseFile(uFileId);
            BEATS_PRINT("Delay close file %d\n", uFileId);
        }
        m_delayCloseFileList.clear();
        CResourceManager::GetInstance()->CleanUp();
    }
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32) && defined(DEVELOP_VERSION)
    if ((::GetKeyState('R') & 0x8000) > 0 && (::GetKeyState(VK_CONTROL) & 0x8000) > 0)
    {
        CResourceManager::GetInstance()->GenerateResourceReport();
        auto& fontFaceMap = CFontManager::GetInstance()->GetFontFaceMap();
        for (auto iter = fontFaceMap.begin(); iter != fontFaceMap.end(); ++iter)
        {
            TCHAR szBuffer[MAX_PATH];
            _stprintf(szBuffer, "C:/font_%s.png", iter->first.c_str());
            iter->second->SaveToTextureFile(szBuffer);
        }
#ifdef EDITOR_MODE
        CComponentGraphic_GL::GetFontFace()->SaveToTextureFile("C:/font_componentGraphics.png");
#endif
        MessageBox(BEYONDENGINE_HWND, _T("请到C盘根目录下查看资源报告"), _T("资源报告已经生成"), MB_OK);
    }
#endif
}

void CEngineCenter::Render()
{
    // If we request to switch the scene, don't render this frame.
#if !defined(EDITOR_MODE)
    CScene* pCurrScene = CSceneManager::GetInstance()->GetCurrentScene();
    if (pCurrScene != NULL)
    {
        CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D)->SetCameraData(pCurrScene->GetCamera(CCamera::eCT_3D)->GetCameraData());
        CRenderManager::GetInstance()->GetCamera(CCamera::eCT_2D)->SetCameraData(pCurrScene->GetCamera(CCamera::eCT_2D)->GetCameraData());
    }
    else
    {
        BEATS_ASSERT(CSceneManager::GetInstance()->GetSwitchSceneState(), "current scene can be null when we are switching scene.");
    }
#endif
#ifdef DEVELOP_VERSION
    BEATS_ASSERT(m_uDrawCallCounter == 0, _T("It's forbid to render outside render function, current draw call counter:%d."), m_uDrawCallCounter);
#endif
    CRenderTarget* pRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
    BEATS_ASSERT(pRenderTarget != nullptr, _T("RenderTarget can't be null!"));
    pRenderTarget->Render();
#ifdef DEVELOP_VERSION
    ResetDrawCall();
    m_uRenderTextCounter = 0;
    CMaterial::m_uUseCountPerFrame = 0;
    m_renderBatchCount.clear();
    m_uBufferDataInvokeTimes = 0;
    m_uBufferDataTransferSize = 0;
#endif
#ifdef DEVELOP_VERSION
    CParticleManager::GetInstance()->m_uRenderCountLastFrame = CParticleManager::GetInstance()->m_uParticleRenderedCount;
    CParticleManager::GetInstance()->m_uUpdateCountLastFrame = CParticleManager::GetInstance()->m_uParticleUpdatedCount;
    CParticleManager::GetInstance()->m_uParticleUpdatedCount = 0;
    CParticleManager::GetInstance()->m_uParticleRenderedCount = 0;
#endif
}

void CEngineCenter::SetFPS(uint32_t uFramePerSecond)
{
    m_uFPS = uFramePerSecond;
    m_uFrameTimeUS = 1000000 / m_uFPS;
}

uint32_t CEngineCenter::GetFPS() const
{
    return m_uFPS;
}

uint32_t CEngineCenter::GetFrameTimeUS() const
{
    return m_uFrameTimeUS;
}

float CEngineCenter::GetActualFPS() const
{
    return m_fAcctualFPS;
}

bool CEngineCenter::ShouldUpdateThisFrame()
{
    uint64_t uCurrTime = CTimeMeter::GetCurrUSec();
    bool bRet = false;

    if (m_uLastUpdateTime == 0)
    {
        bRet = true;
    }
    else
    {
        // TODO: sometimes uCurrTime is less than m_uLastUpdateTime, I don't know why, so I hard fix here
        if (uCurrTime <= m_uLastUpdateTime)
        {
            m_uLastUpdateTime = uCurrTime;
        }
        else
        {
            uint64_t uTimeOffSet = uCurrTime - m_uLastUpdateTime;
            if (uTimeOffSet + m_uMakeUpTime >= m_uFrameTimeUS)
            {
                m_uMakeUpTime = m_uMakeUpTime + uTimeOffSet - m_uFrameTimeUS;
                static const uint64_t maxMakeUpTime = 16000 * 180;
                if (m_uMakeUpTime >= maxMakeUpTime)
                {
                    m_uMakeUpTime = 0;//clear the make up time to avoid keeps high performance.
                }
                bRet = true;
            }
        }
    }

    if (bRet)
    {
        m_uLastUpdateTime = uCurrTime;
    }
    return bRet;
}

void CEngineCenter::ClearMakeUpTime()
{
    m_uMakeUpTime = 0;
}

uint32_t CEngineCenter::GetFrameCounter() const
{
    return m_uFrameCounter;
}

const std::thread::id& CEngineCenter::GetMainThreadId() const
{
    return m_mainThreadId;
}

void CEngineCenter::DelayCloseFile(uint32_t uFileID)
{
    BEATS_PRINT("Request delay close file %d\n", uFileID);
    m_delayCloseFileList.insert(uFileID);
}

void CEngineCenter::UpdateFPS(float dt)
{
    ++m_uUpdateCount;
    m_fFPSCalcTimeAccum += dt;
    if (m_fFPSCalcTimeAccum >= FPS_CALC_INTERVAL)
    {
        m_fAcctualFPS = m_uUpdateCount / m_fFPSCalcTimeAccum;
        m_uUpdateCount = 0;
        m_fFPSCalcTimeAccum = 0.f;
    }
}

bool CEngineCenter::IsEnableSfx()
{
    return m_bIsEnableSfx;
}

void CEngineCenter::EnableSfx(bool bEnable)
{
    m_bIsEnableSfx = bEnable;
}

bool CEngineCenter::IsEnableMusic()
{
    return m_bIsEnableMusic;
}

void CEngineCenter::EnableMusic(bool bEnable)
{
    m_bIsEnableMusic = bEnable;
}

CComponentManagerBase* CEngineCenter::GetComponentManager() const
{
#ifdef EDITOR_MODE
    return CComponentProxyManager::GetInstance();
#else
    return CComponentInstanceManager::GetInstance();
#endif
}

#ifdef DEVELOP_VERSION

void CEngineCenter::IncreaseDrawCall()
{
    ++m_uDrawCallCounter;
}

uint32_t CEngineCenter::GetDrawCallLastFrame()
{
    return m_uDrawCallLastFrame;
}

void CEngineCenter::ResetDrawCall()
{
    m_uDrawCallLastFrame = m_uDrawCallCounter;
    m_uDrawCallCounter = 0;
    m_uDrawTriangleCount = 0;
}

#endif
