#include "stdafx.h"
#ifdef DEVELOP_VERSION
#include "LogManager.h"
#include "Render/RenderManager.h"
#include "Render/RenderTarget.h"
#include "GUI/Font/FreetypeFontFace.h"
#include "Resource/ResourceManager.h"
#include "Render/Texture.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "ParticleSystem/ParticleManager.h"
#include "external/Configuration.h"
#include "Component/Component/ComponentProject.h"
#include "EnginePublic/BeyondEngineVersion.h"
#include "Render/RenderBatch.h"
#include "Render/Material.h"

#if BEYONDENGINE_PLATFORM == PLATFORM_ANDROID
#include "Framework/android/AndroidHandler.h"
#endif
#if (BEYONDENGINE_PLATFORM != PLATFORM_WIN32)
#include "Framework/PlatformHelper.h"
#else
#include "Render/Renderer.h"
#endif
#include "EnginePublic/PlayerPrefs.h"
#include "Task/TaskManager.h"
#include "Task/TaskBase.h"

static const float fSelectorRectSize = 60;
static const uint32_t uLinesCountOnScreen = 30;
CLogManager* CLogManager::m_pInstance = nullptr;
CFreetypeFontFace* CLogManager::m_pLogFont = nullptr;
#define LOG_MANAGER_MAX_LOG_COUNT 3000
CLogManager::CLogManager()
{
#if defined(_DEBUG) && (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    // Don't call BEATS_ASSERT, or it wil enter dead loop.
    if (CApplication::GetInstance()->IsDestructing())
    {
        OutputDebugString("Should not create singleton when exit the program!");
        __debugbreak();
    }
#endif
}

CLogManager::~CLogManager()
{
    Clear();
    BEATS_SAFE_DELETE_VECTOR(m_logPool);
}

void CLogManager::Initialize()
{
    m_logSelector.position.Fill(3, 3);
    m_logSelector.size.Fill(fSelectorRectSize, fSelectorRectSize);

    m_infoSelector.position = m_logSelector.position + CVec2(fSelectorRectSize * 1.3f, 0);
    m_infoSelector.size.Fill(fSelectorRectSize, fSelectorRectSize);

    m_pauseGameSelector.position = m_infoSelector.position + CVec2(fSelectorRectSize * 1.3f, 0);
    m_pauseGameSelector.size.Fill(fSelectorRectSize, fSelectorRectSize);

    m_perfromSelector.position = m_pauseGameSelector.position + CVec2(fSelectorRectSize * 1.3f, 0);
    m_perfromSelector.size.Fill(fSelectorRectSize, fSelectorRectSize);

    m_resourceSelector.position = m_perfromSelector.position + CVec2(fSelectorRectSize * 1.3f, 0);
    m_resourceSelector.size.Fill(fSelectorRectSize, fSelectorRectSize);
    uint32_t uDeviceHeight = CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetDeviceHeight();
    int nFontSize = uDeviceHeight / uLinesCountOnScreen;
    // CFreetypeFontFace will multiply scale factor, so we divide it first to keep the font size to be absolute value.
    nFontSize = (int)(nFontSize / CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetScaleFactor());
    m_pLogFont = new CFreetypeFontFace("debugFont", "simhei.ttf", nFontSize);
    BEATS_PRINT("Log font size: %d\n", m_pLogFont->GetFontSize());
}

void CLogManager::Uninitialize()
{
    BEATS_SAFE_DELETE(m_pLogFont);
}

void CLogManager::WriteLog(ELogType type, const TString& strLog, int nColor, const TCHAR* pszCatalog)
{
    if (!strLog.empty() && m_pLogFont)
    {
        std::lock_guard<std::recursive_mutex> locker(m_mutex);
        if (m_logList.size() > LOG_MANAGER_MAX_LOG_COUNT)
        {
            // Recycle 2/3 logs from the front.
            std::vector<SLog*> tmp;
            auto iter = m_logList.begin();
            advance(iter, LOG_MANAGER_MAX_LOG_COUNT * 2 / 3);
            tmp.insert(tmp.end(), iter, m_logList.end());
            Clear();
            m_logList.swap(tmp);
        }
        SLog* pLog = RequestLog();
        pLog->m_uTimeMS = (uint32_t)(CTimeMeter::GetCurrUSec() / 1000);
        pLog->m_type = type;
        CStringHelper::GetInstance()->SplitString(strLog.c_str(), "\n", pLog->m_strLogLineList, false);
        pLog->m_color = nColor;
        if (pszCatalog != nullptr)
        {
            pLog->m_strCatalog = pszCatalog;
        }
        m_logList.push_back(pLog);
    }
}

void CLogManager::RenderLog()
{
    if (m_bShown && m_pLogFont)
    {
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
        CPolygonModeRenderStateParam::EPolygonModeType restoreFrontPolygonMode;
        CPolygonModeRenderStateParam::EPolygonModeType restoreBackPolygonMode;
        CRenderer::GetInstance()->GetCurrentState()->GetPolygonMode(restoreFrontPolygonMode, restoreBackPolygonMode);
        CRenderer::GetInstance()->PolygonMode(GL_FILL, GL_FILL);
#endif
        CEngineCenter* pEngineCenter = CEngineCenter::GetInstance();
        TCHAR szBuffer[1024];
        // Render channel selector
        CVertex2DPC pt1, pt2, pt3, pt4;
        uint32_t uLogSelectorColor = 0xFF0000FF;
        // 1. Render log channel selector
        pt1.position = m_logSelector.position;
        pt1.color = uLogSelectorColor;
        pt2.position = pt1.position + CVec2(0, m_logSelector.size.Y());
        pt2.color = uLogSelectorColor;
        pt3.position = pt2.position + CVec2(m_logSelector.size.X(), 0);
        pt3.color = uLogSelectorColor;
        pt4.position = pt3.position + CVec2(0, -m_logSelector.size.Y());
        pt4.color = uLogSelectorColor;
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt2, pt3);
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt3, pt4);
        m_pLogFont->RenderText("Log", pt1.position.X(), m_logSelector.position.Y(), LAYER_GUI_EDITOR, 1.0f, 0xFF);
        // 2. Render Info channel selector
        uint32_t uInfoSelectorColor = 0xFFFF00FF;
        pt1.position = m_infoSelector.position;
        pt1.color = uInfoSelectorColor;
        pt2.position = pt1.position + CVec2(0, m_infoSelector.size.Y());
        pt2.color = uInfoSelectorColor;
        pt3.position = pt2.position + CVec2(m_infoSelector.size.X(), 0);
        pt3.color = uInfoSelectorColor;
        pt4.position = pt3.position + CVec2(0, -m_infoSelector.size.Y());
        pt4.color = uInfoSelectorColor;
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt2, pt3);
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt3, pt4);
        m_pLogFont->RenderText("Info", pt1.position.X(), m_logSelector.position.Y(), LAYER_GUI_EDITOR, 1.0f, 0xFF);

        // 3. Render Pause channel selector
        uint32_t uPauseSelectorColor = 0xFF00FFFF;
        pt1.position = m_pauseGameSelector.position;
        pt1.color = uPauseSelectorColor;
        pt2.position = pt1.position + CVec2(0, m_pauseGameSelector.size.Y());
        pt2.color = uPauseSelectorColor;
        pt3.position = pt2.position + CVec2(m_pauseGameSelector.size.X(), 0);
        pt3.color = uPauseSelectorColor;
        pt4.position = pt3.position + CVec2(0, -m_pauseGameSelector.size.Y());
        pt4.color = uPauseSelectorColor;
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt2, pt3);
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt3, pt4);
        m_pLogFont->RenderText("Pause", pt1.position.X(), m_pauseGameSelector.position.Y(), LAYER_GUI_EDITOR, 1.0f, 0xFF);

        // 3. Render Perform channel selector
        uint32_t uPerformSelectorColor = 0x5F9F9FFF;
        pt1.position = m_perfromSelector.position;
        pt1.color = uPerformSelectorColor;
        pt2.position = pt1.position + CVec2(0, m_perfromSelector.size.Y());
        pt2.color = uPerformSelectorColor;
        pt3.position = pt2.position + CVec2(m_perfromSelector.size.X(), 0);
        pt3.color = uPerformSelectorColor;
        pt4.position = pt3.position + CVec2(0, -m_perfromSelector.size.Y());
        pt4.color = uPerformSelectorColor;
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt2, pt3);
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt3, pt4);
        m_pLogFont->RenderText("Perform", pt1.position.X(), m_perfromSelector.position.Y(), LAYER_GUI_EDITOR, 1.0f, 0xFF);

        // 4. Render resource channel selector
        uint32_t uResourceSelectorColor = 0x00FF00FF;
        pt1.position = m_resourceSelector.position;
        pt1.color = uResourceSelectorColor;
        pt2.position = pt1.position + CVec2(0, m_resourceSelector.size.Y());
        pt2.color = uResourceSelectorColor;
        pt3.position = pt2.position + CVec2(m_resourceSelector.size.X(), 0);
        pt3.color = uResourceSelectorColor;
        pt4.position = pt3.position + CVec2(0, -m_resourceSelector.size.Y());
        pt4.color = uResourceSelectorColor;
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt2, pt3);
        CRenderManager::GetInstance()->Render2DTriangle(pt1, pt3, pt4);
        m_pLogFont->RenderText("Resource", pt1.position.X(), m_resourceSelector.position.Y(), LAYER_GUI_EDITOR, 1.0f, 0xFF);

        float fHorizontalOffset = pt4.position.X() + 10;
        _stprintf(szBuffer, "FPS:%d", (int)pEngineCenter->GetActualFPS());
        fHorizontalOffset = m_pLogFont->RenderText(szBuffer, fHorizontalOffset, 0, LAYER_GUI_EDITOR, 1.0f, 0xFF0000FF);
        fHorizontalOffset += 10;
        _stprintf(szBuffer, "DC:%d", (int)pEngineCenter->m_uDrawCallCounter);
        fHorizontalOffset = m_pLogFont->RenderText(szBuffer, fHorizontalOffset, 0, LAYER_GUI_EDITOR, 1.0f, 0xFF0000FF);
        fHorizontalOffset += 10;
#ifndef EDITOR_MODE
        fHorizontalOffset = m_pLogFont->RenderText(CStarRaidersCenter::GetInstance()->GetAccountString(), fHorizontalOffset, 0, LAYER_GUI_EDITOR, 1.0f, 0xFF0000FF);
        fHorizontalOffset += 10;
#if defined(DEVELOP_VERSION)
        SPlayerBasicInfo* pTargetInfo = CStarRaidersCenter::GetInstance()->GetTargetPlayer()->GetBasicInfo();
        if (pTargetInfo != nullptr)
        {
            fHorizontalOffset = m_pLogFont->RenderText(pTargetInfo->m_strAccount, fHorizontalOffset, 0, LAYER_GUI_EDITOR, 1.0f, 0xFF0000FF);
            fHorizontalOffset += 10;
        }
#endif
#endif
#ifdef _DEBUG
        _stprintf(szBuffer, "D_%d", BEYONDENGINE_VER_REVISION);
#else
        _stprintf(szBuffer, "R_%d", BEYONDENGINE_VER_REVISION);
#endif
        fHorizontalOffset = m_pLogFont->RenderText(szBuffer, fHorizontalOffset, 0, LAYER_GUI_EDITOR, 1.0f, 0xFF0000FF);
        fHorizontalOffset += 10;

        _stprintf(szBuffer, "Res:%d", (int)CResourceManager::GetInstance()->GetResourceMap().size());
        fHorizontalOffset = m_pLogFont->RenderText(szBuffer, fHorizontalOffset, 0, LAYER_GUI_EDITOR, 1.0f, 0xFF0000FF);
        fHorizontalOffset += 10;
        _stprintf(szBuffer, "ResMem:%dkb", (int)CResourceManager::GetInstance()->uTotalFileSize/1000);
        fHorizontalOffset = m_pLogFont->RenderText(szBuffer, fHorizontalOffset, 0, LAYER_GUI_EDITOR, 1.0f, 0xFF0000FF);
        fHorizontalOffset += 10;

        CRenderTarget* pCurrRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
        if (m_channel == ELogChannel::eLC_Log)
        {
            int32_t nCurrPos = pCurrRenderTarget->GetLogicHeight() - GetLogFontHeight();
            bool bStop = false;
            std::lock_guard<std::recursive_mutex> locker(m_mutex);
            auto iter = m_logList.rbegin();
            if (m_uStartRenderLogIndex != 0 && m_logList.size() >= m_uStartRenderLogIndex)
            {
                uint32_t uReverseIndex = m_logList.size() - m_uStartRenderLogIndex;
                BEATS_ASSERT(uReverseIndex < m_logList.size());
                advance(iter, uReverseIndex);
            }
            for (; iter != m_logList.rend() && !bStop; ++iter)
            {
                SLog* pLog = *iter;
                if (m_renderType == ELogType::eLT_Count ||
                    m_renderType == pLog->m_type)
                {
                    if (m_strRenderCatalog.empty() ||
                        m_strRenderCatalog == pLog->m_strCatalog)
                    {
                        for (size_t i = 0; i < pLog->m_strLogLineList.size(); ++i)
                        {
                            if (!pLog->m_strLogLineList[i].empty())
                            {
                                int color = pLog->m_color;
                                if (color == 0)
                                {
                                    color = LogTypeColor[(int)pLog->m_type];
                                }
                                m_pLogFont->RenderText(pLog->m_strLogLineList[i], 0, (float)nCurrPos, LAYER_GUI_EDITOR, 1.0f, color);
                                nCurrPos -= GetLogFontHeight();
                                if (nCurrPos < fSelectorRectSize)
                                {
                                    bStop = true;
                                    break;
                                }
                            }
                        }
                        nCurrPos -= GetLogFontHeight() / 5;
                    }
                }
            }
        }
        else if (m_channel == ELogChannel::eLC_Info)
        {
            int32_t nCurrPos = (int32_t)fSelectorRectSize;
            m_pLogFont->RenderText(szBuffer, 0, (float)nCurrPos, LAYER_GUI_EDITOR, 1.0f, 0x0000FFFF);
            nCurrPos += GetLogFontHeight();

            // Camera info
            CScene* pCurrScene = CSceneManager::GetInstance()->GetCurrentScene();
            if (pCurrScene != nullptr)
            {
                CCamera* pCamera = pCurrScene->GetCamera(CCamera::eCT_3D);
                const CVec3& viewPos = pCamera->GetViewPos();
                _stprintf(szBuffer, "Camera: pos: %.2f %.2f %.2f Fov:%.2f Clip:%.1f to %.1f",
                    viewPos.X(), viewPos.Y(), viewPos.Z(), pCamera->GetFOV(), pCamera->GetNear(), pCamera->GetFar());
                m_pLogFont->RenderText(szBuffer, 0, (float)nCurrPos, LAYER_GUI_EDITOR, 1.0f, 0x0000FFFF);
                nCurrPos += GetLogFontHeight();
            }
            // Particle info
            uint32_t uPlayingEmitterCount = CParticleManager::GetInstance()->GetPlayingEmitter("", nullptr).size();
            _stprintf(szBuffer, "Particle: Emitter: %d/%d update:%d render:%d", uPlayingEmitterCount,
                (uint32_t)CParticleManager::GetInstance()->m_emitterRegisterSet.size(),
                CParticleManager::GetInstance()->m_uUpdateCountLastFrame,
                CParticleManager::GetInstance()->m_uRenderCountLastFrame);
            m_pLogFont->RenderText(szBuffer, 0, (float)nCurrPos, LAYER_GUI_EDITOR, 1.0f, 0x0000FFFF);
            nCurrPos += GetLogFontHeight();
            // Render target
            float fGlobalFactor = CRenderManager::GetInstance()->GetGlobalColorFactor();
            CRenderTarget* pCurrRenderTarget = CRenderManager::GetInstance()->GetCurrentRenderTarget();
            float fScaleFactor = pCurrRenderTarget->GetScaleFactor();
            uint32_t uDeveiceWidth = pCurrRenderTarget->GetDeviceWidth();
            uint32_t uDeviceHeight = pCurrRenderTarget->GetDeviceHeight();
            uint32_t uLogicWidth = pCurrRenderTarget->GetLogicWidth();
            uint32_t uLogicHeight = pCurrRenderTarget->GetLogicHeight();
            _stprintf(szBuffer, "RenderTarget: DeviceSize:%d x %d LogicSize:%d x %d ScaleFactor:%.2f GlobalFactor: %.2f", uDeveiceWidth, uDeviceHeight, uLogicWidth, uLogicHeight, fScaleFactor, fGlobalFactor);
            m_pLogFont->RenderText(szBuffer, 0, (float)nCurrPos, LAYER_GUI_EDITOR, 1.0f, 0x0000FFFF);
            nCurrPos += GetLogFontHeight();

            //Configuration.
            TString strNetStateString = CConfiguration::GetInstance()->GetNetworkStateString();
            TString strDeviceString = CConfiguration::GetInstance()->GetDeviceTypeString();
            TString strModel = CConfiguration::GetInstance()->GetModelInfo();
            TString strOs = CConfiguration::GetInstance()->GetOsInfo();
            uint32_t uMaxTextureSize = CConfiguration::GetInstance()->GetMaxTextureSize();
            _stprintf(szBuffer, "System: %s %s Device:%s Net:%s MaxTex: %d", strModel.c_str(), strOs.c_str(), strDeviceString.c_str(), strNetStateString.c_str(), uMaxTextureSize);
            m_pLogFont->RenderText(szBuffer, 0, (float)nCurrPos, LAYER_GUI_EDITOR, 1.0f, 0x0000FFFF);
            nCurrPos += GetLogFontHeight();

            // Component info
            CComponentManagerBase* pComponentMgr = pEngineCenter->GetComponentManager();
            uint32_t uLoadedFiles = pComponentMgr->GetLoadedFiles().size();
            const std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>* pComponentMap = pComponentMgr->GetComponentInstanceMap();
            uint32_t uLoadedComponents = 0;
            for (auto iter = pComponentMap->begin(); iter != pComponentMap->end(); ++iter)
            {
                uLoadedComponents += iter->second->size();
            }
            _stprintf(szBuffer, "Component: Loaded: %d files %d components", uLoadedFiles, uLoadedComponents);
            m_pLogFont->RenderText(szBuffer, 0, (float)nCurrPos, LAYER_GUI_EDITOR, 1.0f, 0x0000FFFF);
            nCurrPos += GetLogFontHeight();

            //Scene info
            CScene* pScene = CSceneManager::GetInstance()->GetCurrentScene();
            if (pScene != nullptr)
            {
                CComponentProject* pComponentProject = pComponentMgr->GetProject();
                auto iter = pComponentProject->GetComponentToFileMap()->find(pScene->GetId());
                BEATS_ASSERT(iter != pComponentProject->GetComponentToFileMap()->end());
                const TString strBcfName = pComponentProject->GetComponentFileName(iter->second);
                TString strDisplayName = pScene->GetDisplayName();
                _stprintf(szBuffer, "Scene: %s id: %d DisplayName:%s ", strBcfName.c_str(), iter->second, strDisplayName.c_str());
            }
            else
            {
                _stprintf(szBuffer, "No scene is loaded!");
            }
            m_pLogFont->RenderText(szBuffer, 0, (float)nCurrPos, LAYER_GUI_EDITOR, 1.0f, 0x0000FFFF);
            nCurrPos += GetLogFontHeight();
        }
        else if (m_channel == ELogChannel::eLC_Perform)
        {
            uint32_t uTotalRunningTimeMS = pEngineCenter->m_uTotalRunningTimeMS;
            const std::map<uint32_t, std::pair<uint32_t, uint32_t>>& performMap = pEngineCenter->m_performMap;
            std::map<uint32_t, uint32_t> elapsedTimeMap;
            uint32_t uTotalRecordTimeMS = 0;
            for (auto iter = performMap.begin(); iter != performMap.end(); ++iter)
            {
                uint32_t uElapsedTime = iter->second.second;
                uTotalRecordTimeMS += uElapsedTime;
                while (elapsedTimeMap.find(uElapsedTime) != elapsedTimeMap.end())
                {
                    uElapsedTime++;
                }
                BEATS_ASSERT(uElapsedTime >= 0);
                elapsedTimeMap[uElapsedTime] = iter->first;
            }
            BEATS_ASSERT(uTotalRecordTimeMS <= uTotalRunningTimeMS);
            static const int32_t nDegreeForPieceOfCircle = 15;
            CVertex2DPC pt1, pt2, pt3;
            static const CColor circleColor[] =
            {
                0xFF0000FF,
                0xFFFF00FF,
                0xFF00FFFF,
                0x00FF00FF,
                0x00FFFFFF,
                0x0000FFFF,
                0xFFFFFFFF,
                0x000000FF,
                0x70DB93FF,
                0x5C3317FF,
                0x9F5F9FFF,
                0xB5A642FF,
                0xD9D919FF,
                0xA67D3DFF,
                0x5F9F9FFF,
                0xD98719FF,
                0xFF7F00FF,
                0x42426FFF,
                0x2F4F2FFF,
                0xCD7F32FF,
                0xDBDB70FF,
                0xC0C0C0FF,
                0xFF7F00FF,
                0x00009CFF,
                0xFF6EC7FF,
                0x4D4DFFFF,
                0x23238EFF,
                0x2F2F4FFF,
                0xA68064FF,
            };
            const uint32_t uDetailCount = sizeof(circleColor) / sizeof(CColor);
            uint32_t uCounter = 0;
            static const float fRadius = 100;
            CVec2 center = CVec2(fRadius + 20, fRadius + 20 + m_perfromSelector.size.Y());
            uint32_t uStartDegree = 0;
            float fTextVerticalPos = fRadius;
            float fTextHorizontalPos = center.X() + fRadius + 50;
            static const uint32_t uMaxTextRow = 6;
            for (auto elapsedIter = elapsedTimeMap.rbegin(); elapsedIter != elapsedTimeMap.rend() && uCounter < uDetailCount; ++elapsedIter)
            {
                uCounter++;
                uint32_t uElapsedTime = elapsedIter->first;
                BEATS_ASSERT(uElapsedTime < uTotalRunningTimeMS);
                float fRate = (float)uElapsedTime / uTotalRunningTimeMS;
                int nDegree = (int32_t)(fRate * 360);
                for (int32_t i = 0; i < nDegree;)
                {
                    int32_t nDrawDegree = nDegreeForPieceOfCircle;
                    if (i + nDegreeForPieceOfCircle > nDegree)
                    {
                        nDrawDegree = nDegree - i;
                    }
                    pt1.position = center;
                    pt1.color = circleColor[uCounter - 1];
                    pt2.position = center + CVec2(fRadius * sinf(DegreesToRadians((float)uStartDegree)), fRadius * cosf(DegreesToRadians((float)uStartDegree)));
                    pt2.color = circleColor[uCounter - 1];
                    uStartDegree += nDrawDegree;
                    pt3.position = center + CVec2(fRadius * sinf(DegreesToRadians((float)uStartDegree)), fRadius * cosf(DegreesToRadians((float)uStartDegree)));
                    pt3.color = circleColor[uCounter - 1];
                    CRenderManager::GetInstance()->Render2DTriangle(pt1, pt2, pt3);
                    i += nDrawDegree;
                }
                _stprintf(szBuffer, "%s %.2f%%", pszPerformNodeStr[elapsedIter->second], fRate * 100);
                m_pLogFont->RenderText(szBuffer, fTextHorizontalPos, fTextVerticalPos, LAYER_GUI_EDITOR, 1.0f, circleColor[uCounter - 1]);
                if (fTextVerticalPos > fRadius + uMaxTextRow * GetLogFontHeight())
                {
                    fTextHorizontalPos += 400.f;
                    fTextVerticalPos = fRadius;
                }
                else
                {
                    fTextVerticalPos += GetLogFontHeight();
                }
            }
            if (uStartDegree < 360)
            {
                int32_t nResetDegree = 360 - uStartDegree;
                _stprintf(szBuffer, "%s %.2f%%", "Unknown", (nResetDegree / 360.f) * 100);
                m_pLogFont->RenderText(szBuffer, fTextHorizontalPos, fTextVerticalPos, LAYER_GUI_EDITOR, 1.0f, circleColor[uCounter]);
                for (int32_t i = 0; i < nResetDegree;)
                {
                    int32_t nDrawDegree = nDegreeForPieceOfCircle;
                    if (i + nDegreeForPieceOfCircle > nResetDegree)
                    {
                        nDrawDegree = nResetDegree - i;
                    }
                    pt1.position = center;
                    pt1.color = circleColor[uCounter];
                    pt2.position = center + CVec2(fRadius * sinf(DegreesToRadians((float)uStartDegree)), fRadius * cosf(DegreesToRadians((float)uStartDegree)));
                    pt2.color = circleColor[uCounter];
                    uStartDegree += nDrawDegree;
                    pt3.position = center + CVec2(fRadius * sinf(DegreesToRadians((float)uStartDegree)), fRadius * cosf(DegreesToRadians((float)uStartDegree)));
                    pt3.color = circleColor[uCounter];
                    CRenderManager::GetInstance()->Render2DTriangle(pt1, pt2, pt3);
                    i += nDrawDegree;
                }
            }
            fTextVerticalPos = fRadius + uMaxTextRow * GetLogFontHeight() + 50.f;
            _stprintf(szBuffer, "RenderTextCount: %d Material Count: %d UseCount:%d", pEngineCenter->m_uRenderTextCounter, CMaterial::m_uMaterialCount, CMaterial::m_uUseCountPerFrame);
            m_pLogFont->RenderText(szBuffer, 0, fTextVerticalPos, LAYER_GUI_EDITOR, 1.0f, 0xFF0000FF);
            fTextVerticalPos += GetLogFontHeight();
            float fRenderBatchHorizontalOffset = m_pLogFont->RenderText("RenderBatch: ", 0, fTextVerticalPos, LAYER_GUI_EDITOR, 1.0f, 0xFF0000FF);
            for (auto iter = pEngineCenter->m_renderBatchCount.begin(); iter != pEngineCenter->m_renderBatchCount.end(); ++iter)
            {
                _stprintf(szBuffer, "%s %d  ", pszRenderBatchUsage[(int)iter->first], iter->second);
                fRenderBatchHorizontalOffset = m_pLogFont->RenderText(szBuffer, fRenderBatchHorizontalOffset, fTextVerticalPos, LAYER_GUI_EDITOR, 1.0f, 0xFF0000FF);
            }
            fTextVerticalPos += GetLogFontHeight();
            _stprintf(szBuffer, "BufferData: %d %.2fkb Triangles:%d", pEngineCenter->m_uBufferDataInvokeTimes, pEngineCenter->m_uBufferDataTransferSize * 0.001f, pEngineCenter->m_uDrawTriangleCount);
            m_pLogFont->RenderText(szBuffer, 0, fTextVerticalPos, LAYER_GUI_EDITOR, 1.0f, 0xFF0000FF);
            fTextVerticalPos += GetLogFontHeight();
        }
        else if (m_channel == ELogChannel::eLC_Resource)
        {
            const std::map<TString, SharePtr<CResource>>& resourceMap = CResourceManager::GetInstance()->GetResourceMap();
            std::map<EResourceType, std::vector< SharePtr<CResource> > > resourceTypeMap;
            for (auto iter = resourceMap.begin(); iter != resourceMap.end(); ++iter)
            {
                resourceTypeMap[iter->second->GetType()].push_back(iter->second);
            }
            float fTextPos = fSelectorRectSize;
            uint32_t uSkipLine = m_uResourceStartLine;
            bool bStop = false;
            for (auto iter = resourceTypeMap.begin(); iter != resourceTypeMap.end() && !bStop; ++iter)
            {
                if (uSkipLine == 0)
                {
                    _stprintf(szBuffer, _T("Type: %s x %d"), pszResourceTypeString[iter->first], (uint32_t)iter->second.size());
                    m_pLogFont->RenderText(szBuffer, 0, fTextPos, LAYER_GUI_EDITOR, 1.0f, 0x00FF00FF);
                    fTextPos += GetLogFontHeight();
                    if (pCurrRenderTarget->GetLogicHeight() < fTextPos)
                    {
                        bStop = true;
                        break;
                    }
                }
                else
                {
                    --uSkipLine;
                }
                for (uint32_t i = 0; i < iter->second.size() && !bStop; ++i)
                {
                    if (uSkipLine == 0)
                    {
                        m_pLogFont->RenderText(iter->second[i]->GetDescription().c_str(), 0, fTextPos, LAYER_GUI_EDITOR, 1.0f, 0x00FF00FF);
                        fTextPos += GetLogFontHeight();
                        if (pCurrRenderTarget->GetLogicHeight() < fTextPos)
                        {
                            bStop = true;
                            break;
                        }
                    }
                    else
                    {
                        --uSkipLine;
                    }
                }
            }
        }
        CRenderManager::GetInstance()->Render();
#if BEYONDENGINE_PLATFORM == PLATFORM_WIN32
        CRenderer::GetInstance()->PolygonMode(restoreFrontPolygonMode, restoreBackPolygonMode);
#endif
    }
}

void CLogManager::SetRenderLogType(ELogType type)
{
    m_renderType = type;
}

void CLogManager::SetRenderCatalog(const TString& strCatalog)
{
    m_strRenderCatalog = strCatalog;
}

void CLogManager::Show(bool bShow)
{
    m_bShown = bShow;
}

bool CLogManager::IsShown() const
{
    return m_bShown;
}

void CLogManager::IncreaseRenderPos()
{
    if (m_channel == ELogChannel::eLC_Log)
    {
        if (m_uStartRenderLogIndex != 0 && m_uStartRenderLogIndex < m_logList.size() - 1)
        {
            ++m_uStartRenderLogIndex;
        }
        else
        {
            m_uStartRenderLogIndex = 0;
        }
    }
    else if (m_channel == ELogChannel::eLC_Resource)
    {
        ++m_uResourceStartLine;
    }
}

void CLogManager::DecreaseRenderPos()
{
    if (m_channel == ELogChannel::eLC_Log)
    {
        if (m_uStartRenderLogIndex == 0)
        {
            if (m_logList.size() > 0)
            {
                m_uStartRenderLogIndex = m_logList.size() - 1;
            }
        }
        else if (m_uStartRenderLogIndex > 1)
        {
            --m_uStartRenderLogIndex;
        }
    }
    else if (m_channel == ELogChannel::eLC_Resource)
    {
        if (m_uResourceStartLine > 0)
        {
            --m_uResourceStartLine;
        }
    }
}

void CLogManager::ClearRenderPos()
{
    m_uStartRenderLogIndex = 0;
}

void CLogManager::SetRenderPosToTop()
{
    m_uStartRenderLogIndex = 1;
}

const std::vector<SLog*>& CLogManager::GetLogList() const
{
    return m_logList;
}

void CLogManager::Clear()
{
    std::lock_guard<std::recursive_mutex> locker(m_mutex);
    m_logPool.insert(m_logPool.end(), m_logList.begin(), m_logList.end());
    m_logList.clear();
    m_uStartRenderLogIndex = 0;
}

uint32_t CLogManager::GetLogFontHeight() const
{
    return m_pLogFont ? (uint32_t)(m_pLogFont->GetFontSize()) : 0;
}

bool CLogManager::SwitchChannel(const CVec2& pos)
{
    bool bRet = false;
    if (IsShown())
    {
        if (m_logSelector.containsPoint(pos))
        {
            if (m_channel != ELogChannel::eLC_Log)
            {
                m_channel = ELogChannel::eLC_Log;
            }
            else
            {
                CLogManager::GetInstance()->Clear();
            }
            bRet = true;
        }
        else if (m_infoSelector.containsPoint(pos))
        {
            m_channel = ELogChannel::eLC_Info;
            bRet = true;
        }
        else if (m_perfromSelector.containsPoint(pos))
        {
            if (m_channel != ELogChannel::eLC_Perform)
            {
                m_channel = ELogChannel::eLC_Perform;
            }
            else
            {
                CEngineCenter::GetInstance()->m_bClearPerformRequest = true;
                CRenderManager::GetInstance()->SetGlobalColorFactor(1.0f);
            }
            bRet = true;
        }
        else if (m_pauseGameSelector.containsPoint(pos))
        {
            if (CApplication::GetInstance()->IsRunning())
            {
                CApplication::GetInstance()->Pause();
            }
            else
            {
                CApplication::GetInstance()->Resume();
            }
            bRet = true;
        }
        else if (m_resourceSelector.containsPoint(pos))
        {
            if (m_channel != ELogChannel::eLC_Resource)
            {
                m_channel = ELogChannel::eLC_Resource;
            }
            bRet = true;
        }
    }
    return bRet;
}

ELogChannel CLogManager::GetLogChannel() const
{
    return m_channel;
}

SLog* CLogManager::RequestLog()
{
    SLog* pRet = nullptr;
    if (m_logPool.size() > 0)
    {
        pRet = m_logPool.back();
        pRet->Reset();
        m_logPool.pop_back();
    }
    else
    {
        pRet = new SLog;
    }
    return pRet;
}

TString CLogManager::GetLastLogList(uint32_t uCount)
{
    TString strRet;
    const std::vector<SLog*>& logList = GetLogList();
    int startIndex = 0;
    if (logList.size() > uCount)
    {
        startIndex = logList.size() - uCount;
    }

    for (size_t i = startIndex; i < logList.size(); ++i)
    {
        for (size_t j = 0; j < logList[i]->m_strLogLineList.size(); ++j)
        {
            TString& strLog = logList[i]->m_strLogLineList[j];
            if (!strLog.empty())
            {
                strRet += strLog + "\n";
            }
        }
    }
    return strRet;
}

#endif