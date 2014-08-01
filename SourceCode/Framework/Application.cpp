#include "stdafx.h"
#include "Application.h"
#include "Utility/BeatsUtility/Platform/ios/BeatsPlatformConfig.h"
#include "BeatsTchar.h"
#include "PublicDef.h"
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

CApplication *CApplication::m_sMainApplication = nullptr;

CApplication::CApplication()
    : m_bInitialized(false)
    , m_bRunning(false)
{
    BEATS_ASSERT(!m_sMainApplication, _T("You should initialize only one application"));
    m_sMainApplication = this;
}

CApplication::~CApplication()
{
    m_sMainApplication = nullptr;
}

bool CApplication::IsInitialized() const
{
    return m_bInitialized;
}

void CApplication::Initialize()
{
    CEngineCenter::GetInstance()->Initialize();
    m_bInitialized = true;
}

void CApplication::Uninitialize()
{
    CEngineCenter::GetInstance()->Uninitialize();
    CEngineCenter::Destroy();
    m_bInitialized = false;
}

void CApplication::Update()
{
    CEngineCenter::GetInstance()->Update();
}

void CApplication::Render()
{
    CEngineCenter::GetInstance()->Render();
}

bool CApplication::ShouldUpdateThisFrame()
{
    return CEngineCenter::GetInstance()->ShouldUpdateThisFrame();
}

bool CApplication::Start()
{
    m_bRunning = true;
    return true;
}

void CApplication::Pause()
{
    m_bRunning = false;
}

void CApplication::Resume()
{
    m_bRunning = true;
}

void CApplication::Stop()
{
    m_bRunning = false;
}

bool CApplication::IsRunning() const
{
    return m_bRunning;
}

CApplication *CApplication::GetInstance()
{
    if (m_sMainApplication == NULL)
    {
        m_sMainApplication = new CApplication;
    }
    return m_sMainApplication;
}

void CApplication::Destroy()
{
    BEATS_ASSERT(m_sMainApplication, _T("You should implement and initialize an application"));
    BEATS_SAFE_DELETE(m_sMainApplication);
}
