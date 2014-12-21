#include "stdafx.h"
#include "NodeAnimation.h"
#include "NodeAnimationManager.h"
#include "NodeAnimationData.h"
#include "Scene/Node.h"
#include "Event/EventDispatcher.h"
#include "Scene/NodeEvent.h"
#include "Render/Sprite.h"
#include "Scene/SceneManager.h"

CNodeAnimation::CNodeAnimation()
    : m_bPause(false)
    , m_bIsPlaying(false)
    , m_bReversePlay(false)
    , m_bAutoDestroy(false)
    , m_bResetNodeWhenStop(false)
    , m_uCurrFramePos(0)
    , m_uFPS(0)
    , m_fElapsedTime(0)
    , m_fTimeForAFrame(0)
    , m_playType(eNAPT_Count)
    , m_pData(NULL)
    , m_pOwner(NULL)
    , m_pEventDispatcher(NULL)
    , m_pEventScene(NULL)
    , m_bDispatcherBeginEvent(false)
    , m_bDeleteFlag(false)
{

}

CNodeAnimation::~CNodeAnimation()
{
    BEATS_SAFE_DELETE(m_pEventDispatcher)
}

void CNodeAnimation::Play(uint32_t uStartPos /*= 0*/)
{
#ifndef DISABLE_NODE_ANIMATION
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    if (!m_bIsPlaying)
    {
        m_bIsPlaying = true;
        InitStartPos(uStartPos);
        BEATS_ASSERT(m_pOwner);
        m_fElapsedTime = uStartPos * m_fTimeForAFrame;
        m_bDispatcherBeginEvent = true;
        CNodeAnimationManager::GetInstance()->RegisterNodeAnimation(this);
    }
#endif
}

void CNodeAnimation::Stop()
{
#ifndef DISABLE_NODE_ANIMATION
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    BEATS_ASSERT(m_pOwner != NULL);
    m_bIsPlaying = false;
    m_fElapsedTime = 0;
    if (m_bResetNodeWhenStop)
    {
        ResetNode();
    }
#endif
}

void CNodeAnimation::Pause()
{
    m_bPause = true;
}

void CNodeAnimation::Resume()
{
    m_bPause = false;
}

bool CNodeAnimation::IsPaused() const
{
    return m_bPause;
}

bool CNodeAnimation::IsPlaying() const
{
    return m_bIsPlaying;
}

void CNodeAnimation::SetPlayingFlag(bool bPlaying)
{
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    m_bIsPlaying = bPlaying;
}

void CNodeAnimation::Update(float ddt)
{
#ifndef DISABLE_NODE_ANIMATION
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    if (!IsPaused() && m_bIsPlaying && m_playType != eNAPT_NONE)
    {
        if (m_pData && m_pData->GetFrameCount() > 0)
        {
            uint32_t uTotalFrame = m_pData->GetFrameCount() - 1;
            uint32_t uFrame = (uint32_t)(m_fElapsedTime / m_fTimeForAFrame);
            uint32_t uLastFramePos = m_uCurrFramePos;
            bool bAnimationFinished = false;

            bool bTriggerLoopEndEvent = false;
            if (m_playType == eNAPT_ONCE)
            {
                m_uCurrFramePos = uFrame;
                if (m_uCurrFramePos > uTotalFrame)
                {
                    m_uCurrFramePos = uTotalFrame;
                    bAnimationFinished = true;
                }
            }
            else if (m_playType == eNAPT_LOOP)
            {
                if (uFrame > uTotalFrame)
                {
                    m_fElapsedTime -= m_fTimeForAFrame * (uTotalFrame + 1);
                    bTriggerLoopEndEvent = true;
                }
                m_uCurrFramePos = uFrame % (uTotalFrame + 1);
            }
            else if (m_playType == eNAPT_REVERSE)
            {
                int nPos = (int)uTotalFrame - (int)uFrame;
                if (nPos < 0)
                {
                    m_uCurrFramePos = 0;
                    bAnimationFinished = true;
                }
                else
                {
                    m_uCurrFramePos = nPos;
                }
            }
            else if (m_playType == eNAPT_BOUNCE)
            {
                if (uFrame > uTotalFrame)
                {
                    BEATS_ASSERT(m_fElapsedTime >= m_fTimeForAFrame * uTotalFrame);
                    m_fElapsedTime = BEATS_FMOD(m_fElapsedTime, m_fTimeForAFrame * uTotalFrame);
                    m_bReversePlay = !m_bReversePlay;
                }
                if (m_bReversePlay)
                {
                    m_uCurrFramePos = (int)uTotalFrame - (int)(uFrame % (uTotalFrame + 1));
                }
                else
                {
                    m_uCurrFramePos = uFrame % (uTotalFrame + 1);
                }
            }
            else if (m_playType == eNAPT_REVERSE_LOOP)
            {
                if (uFrame > uTotalFrame)
                {
                    m_fElapsedTime -= m_fTimeForAFrame * (uTotalFrame + 1);
                }
                m_uCurrFramePos = (int)uTotalFrame - (int)(uFrame % (uTotalFrame + 1));
            }
            else if (m_playType == eNAPT_NONE)
            {
                // Do nothing.
            }
            else
            {
                BEATS_ASSERT(false, _T("Unknown play type!"));
            }
            if (uLastFramePos != m_uCurrFramePos || m_bDispatcherBeginEvent)
            {
                BEATS_ASSERT(m_pOwner != NULL);
                BEATS_ASSERT(m_pData != NULL);
                m_pData->Apply(m_pOwner, m_uCurrFramePos);
                if (m_bDispatcherBeginEvent)//dispatch this event must after first frame apply
                {
                    TriggerEvent(eET_EVENT_NODE_ANIMATION_START);
                    m_bDispatcherBeginEvent = false;
                }
            }
            m_fElapsedTime += ddt;
            if (bTriggerLoopEndEvent)
            {
                TriggerEvent(eET_EVENT_NODE_ANIMATION_LOOP_END);
            }
            if (bAnimationFinished)
            {
                Stop();
                TriggerEvent(eET_EVENT_NODE_ANIMATION_FINISHED);
            }
        }
    }
#endif
}

uint32_t CNodeAnimation::GetFPS() const
{
    return m_uFPS;
}

void CNodeAnimation::SetFPS(uint32_t uFPS)
{
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    BEATS_ASSERT(uFPS > 0);
    m_uFPS = uFPS;
    m_fTimeForAFrame = 1.0f / m_uFPS;
}

ENodeAnimationPlayType CNodeAnimation::GetPlayType() const
{
    return m_playType;
}

void CNodeAnimation::SetPlayType(ENodeAnimationPlayType type)
{
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    m_playType = type;
}

void CNodeAnimation::SetData(CNodeAnimationData* pData)
{
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    ResetNode();
    m_pData = pData;
}

CNodeAnimationData* CNodeAnimation::GetData() const
{
    return m_pData;
}

void CNodeAnimation::SetOwner(CNode* pOwner, bool bNeedStop)
{
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    if (m_pOwner != pOwner)
    {
        if (m_bIsPlaying && m_pOwner && bNeedStop)
        {
            ResetNode();
            Stop();
        }
        m_pOwner = pOwner;
    }
}

CNode* CNodeAnimation::GetOwner() const
{
    return m_pOwner;
}

uint32_t CNodeAnimation::GetCurrentFrame() const
{
    return m_uCurrFramePos;
}

void CNodeAnimation::SetCurrentFrame(uint32_t uFrame)
{
#ifndef DISABLE_NODE_ANIMATION
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    m_uCurrFramePos = 0;
    if (m_pData)
    {
        m_uCurrFramePos = MIN(uFrame, m_pData->GetFrameCount() - 1);
        m_pData->Apply(GetOwner(), m_uCurrFramePos);
    }
#endif
}

CEventDispatcher* CNodeAnimation::GetEventDispatcher()
{
    if (m_pEventDispatcher == NULL)
    {
        m_pEventDispatcher = new CCompWrapper<CEventDispatcher, CNode>(m_pOwner);
    }
    return m_pEventDispatcher;
}

void CNodeAnimation::ResetNode()
{
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    if (m_pOwner != NULL)
    {
        SAnimationProperty* pProperty = m_pOwner->GetAnimationProperty();
        if (pProperty != NULL)
        {
            pProperty->Reset();
            m_pOwner->InvalidateLocalTM();
        }
        CColor defaultColor;
        defaultColor.r = 100;
        defaultColor.g = 100;
        defaultColor.b = 100;
        defaultColor.a = 100;
        m_pOwner->SetColorScale(defaultColor);
        if (m_pData)
        {
            const std::vector<CNodeAnimationElement*>& elements = m_pData->GetElements();
            for (size_t i = 0; i < elements.size(); ++i)
            {
                m_pOwner->NodeAnimationReset(elements[i]->GetType());
            }
        }
    }
}

void CNodeAnimation::Reset()
{
    m_bPause = false;
    m_bIsPlaying = false;
    m_bReversePlay = false;
    m_bAutoDestroy = false;
    m_bResetNodeWhenStop = false;
    m_uCurrFramePos = 0;
    m_uFPS = 0;
    m_fElapsedTime = 0;
    m_fTimeForAFrame = 0;
    m_playType = eNAPT_Count;
    if (m_pData != nullptr && m_pData->GetTempFlag())
    {
        BEATS_SAFE_DELETE(m_pData);
    }
    m_pData = NULL;
    m_pOwner = NULL;
    BEATS_SAFE_DELETE(m_pEventDispatcher);
    m_pEventScene = NULL;
    m_bDispatcherBeginEvent = false;
    m_bDeleteFlag = false;
}

void CNodeAnimation::BindScene(CScene* pScene)
{
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    m_pEventScene = pScene;
}

void CNodeAnimation::SetAutoDestroy(bool bAutoDestroy)
{
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    m_bAutoDestroy = bAutoDestroy;
}

bool CNodeAnimation::IsAutoDestroy() const
{
    return m_bAutoDestroy;
}

void CNodeAnimation::InitStartPos(uint32_t uStartPos /*= 0*/)
{
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    switch (m_playType)
    {
    case eNAPT_ONCE:
    case eNAPT_LOOP:
    case eNAPT_BOUNCE:
        SetCurrentFrame(uStartPos);
        break;
    case eNAPT_REVERSE:
    case eNAPT_REVERSE_LOOP:
        if (m_pData != NULL)
        {
            BEATS_ASSERT(m_pData->GetFrameCount() >= 1 + uStartPos);
            SetCurrentFrame(m_pData->GetFrameCount() - 1 - uStartPos);
        }
        break;
    default:
        BEATS_ASSERT(false, _T("Unknown play type!"));
        break;
    }
}

void CNodeAnimation::TriggerEvent(EEventType eventType)
{
#ifndef DISABLE_NODE_ANIMATION
    if (m_pEventDispatcher != NULL && (m_pEventScene == NULL || m_pEventScene == CSceneManager::GetInstance()->GetCurrentScene()))
    {
        CNodeEvent event(eventType);
        m_pEventDispatcher->DispatchEvent(&event);
    }
#endif
}

void CNodeAnimation::SetOnEndHandler(CEventSubscription::EventHandler pHandler)
{
#ifndef DISABLE_NODE_ANIMATION
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    GetEventDispatcher()->Clear();
    if (pHandler != nullptr)
    {
        GetEventDispatcher()->SubscribeEvent(eET_EVENT_NODE_ANIMATION_STOP, pHandler);
    }
#endif
}

void CNodeAnimation::SetResetNodeWhenStop(bool bValue)
{
    BEATS_ASSERT(!m_bDeleteFlag, "Can't operate on a animation which has delete flag, this may cause bug.");
    m_bResetNodeWhenStop = bValue;
}
