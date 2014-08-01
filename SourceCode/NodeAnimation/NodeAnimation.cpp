#include "stdafx.h"
#include "NodeAnimation.h"
#include "NodeAnimationManager.h"
#include "NodeAnimationData.h"
#include "Scene\Node.h"
#include "Event\EventDispatcher.h"
#include "Event\BaseEvent.h"
#include "Render\Sprite.h"

CNodeAnimation::CNodeAnimation()
    : m_bPause(false)
    , m_bIsPlaying(false)
    , m_bReversePlay(false)
    , m_uCurrFramePos(0)
    , m_uFPS(0)
    , m_fElapsedTime(0)
    , m_fTimeForAFrame(0)
    , m_playType(eNAPT_Count)
    , m_pData(NULL)
    , m_pOwner(NULL)
    , m_pEventDispatcher(NULL)
{

}

CNodeAnimation::~CNodeAnimation()
{
    if (m_bIsPlaying)
    {
        Stop();
        CNodeAnimationManager::GetInstance()->RemoveNodeAnimation(this->GetOwner(), this);
    }
    BEATS_SAFE_DELETE(m_pEventDispatcher)
}

void CNodeAnimation::Play()
{
    if (!m_bIsPlaying)
    {
        InitStartPos();
        CNodeAnimationManager::GetInstance()->RegisterNodeAnimation(this);
        m_bIsPlaying = true;
        TriggerEvent(eET_EVENT_NODE_ANIMATION_START);
    }
}

void CNodeAnimation::Stop()
{
    BEATS_ASSERT(m_bIsPlaying, _T("Don't stop an animation which is already stopped!"));
    BEATS_ASSERT(m_pOwner != NULL);
    m_bIsPlaying = false;
    TriggerEvent(eET_EVENT_NODE_ANIMATION_STOP);
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

void CNodeAnimation::Update(float ddt)
{
    if (!IsPaused())
    {
        m_fElapsedTime += ddt;
        if (m_fElapsedTime > m_fTimeForAFrame && m_pData)
        {
            size_t uFrame = (size_t)(m_fElapsedTime / m_fTimeForAFrame);
            size_t uTotalCount = m_pData->GetFrameCount();
            if (m_playType == eNAPT_ONCE)
            {
                m_uCurrFramePos += uFrame;
                if (m_uCurrFramePos >= uTotalCount)
                {
                    m_uCurrFramePos = uTotalCount - 1;
                    TriggerEvent(eET_EVENT_NODE_ANIMATION_FINISHED);
                    Stop();
                }
            }
            else if (m_playType == eNAPT_LOOP)
            {
                m_uCurrFramePos += uFrame;
                if (m_uCurrFramePos >= uTotalCount)
                {
                    m_uCurrFramePos = m_uCurrFramePos % uTotalCount;
                }
            }
            else if (m_playType == eNAPT_REVERSE)
            {
                int nCurPos = (int)m_uCurrFramePos - (int)uFrame;
                if ( nCurPos < 0 )
                {
                    m_uCurrFramePos = 0;
                    TriggerEvent(eET_EVENT_NODE_ANIMATION_FINISHED);
                    Stop();
                }
                else
                {
                    m_uCurrFramePos = nCurPos;
                }
            }
            else if (m_playType == eNAPT_BOUNCE)
            {
                int nFrameCount = m_bReversePlay ? (int)m_uCurrFramePos - (int)uFrame : m_uCurrFramePos + uFrame;
                if (nFrameCount < 0)
                {
                    nFrameCount = -nFrameCount;
                    m_bReversePlay = false;
                }
                if (nFrameCount >= (int)uTotalCount)
                {
                    while (nFrameCount >= (int)uTotalCount)
                    {
                        nFrameCount -= uTotalCount;
                        m_bReversePlay = !m_bReversePlay;
                    }
                    if (m_bReversePlay)
                    {
                        m_uCurrFramePos = uTotalCount - nFrameCount - 1;
                    }
                    else
                    {
                        BEATS_ASSERT(nFrameCount >= 0);
                        m_uCurrFramePos = nFrameCount;
                    }
                }
                else
                {
                    m_uCurrFramePos = nFrameCount;
                    if (m_uCurrFramePos == uTotalCount - 1)
                    {
                        m_bReversePlay = true;
                    }
                }
                
                BEATS_ASSERT(m_uCurrFramePos < uTotalCount);
            }
            else if (m_playType == eNAPT_REVERSE_LOOP)
            {
                int curPos = (int)m_uCurrFramePos - (int)uFrame;
                while (curPos < 0)
                {
                    curPos += uTotalCount;
                }
                m_uCurrFramePos = curPos;
            }
            else
            {
                BEATS_ASSERT(false, _T("Unknown play type!"));
            }
            BEATS_ASSERT(m_pOwner != NULL);
            m_pData->Apply(m_pOwner, m_uCurrFramePos);
            m_fElapsedTime -= m_fTimeForAFrame * uFrame;
            BEATS_ASSERT(m_pData != NULL);
        }
    }
}

size_t CNodeAnimation::GetFPS() const
{
    return m_uFPS;
}

void CNodeAnimation::SetFPS(size_t uFPS)
{
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
    m_playType = type;
}

void CNodeAnimation::SetData(CNodeAnimationData* pData)
{
    ResetNode();
    m_pData = pData;
}

CNodeAnimationData* CNodeAnimation::GetAnimationData() const
{
    return m_pData;
}

void CNodeAnimation::SetOwner(CNode* pOwner)
{
    m_pOwner = pOwner;
    if (m_pData != NULL)
    {
        m_pData->Apply(m_pOwner, m_uCurrFramePos);
    }
}

CNode* CNodeAnimation::GetOwner() const
{
    return m_pOwner;
}

size_t CNodeAnimation::GetCurrentFrame() const
{
    return m_uCurrFramePos;
}

void CNodeAnimation::SetCurrentFrame(size_t uFrame)
{
    m_uCurrFramePos = 0;
    if (m_pData)
    {
        if (uFrame < m_pData->GetFrameCount())
        {
            m_uCurrFramePos = uFrame;
        }
        else
        {
            m_uCurrFramePos = m_pData->GetFrameCount() - 1;
        }
    }
}

CEventDispatcher* CNodeAnimation::GetEventDispatcher()
{
    if (m_pEventDispatcher == NULL)
    {
        m_pEventDispatcher = new CEventDispatcher;
    }
    return m_pEventDispatcher;
}

void CNodeAnimation::ResetNode()
{
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
        m_pOwner->SetColorScale(defaultColor);
    }
}

void CNodeAnimation::InitStartPos()
{
    switch (m_playType)
    {
    case eNAPT_ONCE:
    case eNAPT_LOOP:
    case eNAPT_BOUNCE:
        SetCurrentFrame(0);
        break;
    case eNAPT_REVERSE:
    case eNAPT_REVERSE_LOOP:
        if (m_pData != NULL)
        {
            SetCurrentFrame(m_pData->GetFrameCount() - 1);
        }
        break;
    default:
        BEATS_ASSERT(false, _T("Unknown play type!"));
        break;
    }
}

void CNodeAnimation::TriggerEvent(EEventType eventType)
{
    if (m_pEventDispatcher != NULL)
    {
        CBaseEvent event(eventType);
        m_pEventDispatcher->DispatchEvent(&event);
    }
}
