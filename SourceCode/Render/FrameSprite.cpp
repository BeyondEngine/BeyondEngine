#include "stdafx.h"
#include "FrameSprite.h"

CFrameSprite::CFrameSprite()
    : m_bPlaying(false)
    , m_bReversePlay(false)
    , m_playType(eFSPT_Once)
    , m_uCurrFrame(0)
    , m_fAnimationIntervalMS(0)
    , m_fElapsedTime(0)
{

}

CFrameSprite::~CFrameSprite()
{

}

void CFrameSprite::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bPlaying, true, 0xFFFFFFFF, _T("是否播放"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_playType, true, 0xFFFFFFFF, _T("播放方式"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fAnimationIntervalMS, true, 0xFFFFFFFF, _T("每帧播放时间（毫秒）"), NULL, NULL, _T("MinValue:1, DefaultValue:1000"));
    DECLARE_PROPERTY(serializer, m_uCurrFrame, true, 0xFFFFFFFF, _T("当前帧"), NULL, NULL, _T("MinValue:0"));
    DECLARE_PROPERTY(serializer, m_frames, true, 0xFFFFFFFF, _T("序列帧"), NULL, NULL, NULL);
    HIDE_PROPERTY(m_anchor);
    HIDE_PROPERTY(m_fragInfo);
    HIDE_PROPERTY(m_size);
}

void CFrameSprite::Update(float ddt)
{
    if (m_bPlaying && m_frames.size() > 1)
    {
        m_fElapsedTime += ddt * 1000;
        if (m_fElapsedTime >= m_fAnimationIntervalMS )
        {
            size_t uTotalCount = m_frames.size();
            size_t uFrame = (size_t)(m_fElapsedTime / m_fAnimationIntervalMS);
            m_fElapsedTime -= m_fAnimationIntervalMS * uFrame;
            switch (m_playType)
            {
            case eFSPT_Once:
                if (m_uCurrFrame < uTotalCount - 1)
                {
                    size_t uCurrentPos = m_uCurrFrame + uFrame;
                    if (uCurrentPos >= uTotalCount)
                    {
                        uCurrentPos = uTotalCount - 1;
                    }
                    SetCurrFrame(uCurrentPos);
                }
                break;
            case eFSPT_Loop:
                {
                    size_t uCurrentPos = m_uCurrFrame + uFrame;
                    if (uCurrentPos > uTotalCount)
                    {
                        uCurrentPos = uCurrentPos % uTotalCount;
                    }
                    SetCurrFrame(uCurrentPos);
                }
                break;
            case eFSPT_Bounce:
                {
                    int nFrameCount = m_bReversePlay ? (int)m_uCurrFrame - (int)uFrame : m_uCurrFrame + uFrame;
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
                            m_uCurrFrame = uTotalCount - nFrameCount - 1;
                        }
                        else
                        {
                            BEATS_ASSERT(nFrameCount >= 0);
                            m_uCurrFrame = nFrameCount;
                        }
                    }
                    else
                    {
                        m_uCurrFrame = nFrameCount;
                        if (m_uCurrFrame == uTotalCount - 1)
                        {
                            m_bReversePlay = true;
                        }
                    }
                    BEATS_ASSERT(m_uCurrFrame < uTotalCount);
                }
                break;
            default:
                BEATS_ASSERT(false, _T("Unknown play type!"));
                break;
            }
        }
    }
}

void CFrameSprite::DoRender()
{
    if (m_uCurrFrame < m_frames.size())
    {
        if (m_frames[m_uCurrFrame] != NULL)
        {
            if (m_frames[m_uCurrFrame]->GetParentNode() == NULL)
            {
                AddChild(m_frames[m_uCurrFrame]);
            }
            m_frames[m_uCurrFrame]->DoRender();
        }
    }
}

void CFrameSprite::PostRender()
{
    // Do nothing, Don't render its child.
}

size_t CFrameSprite::GetCurrFrame() const
{
    return m_uCurrFrame;
}

void CFrameSprite::SetCurrFrame(size_t currFrame)
{
    BEATS_ASSERT(currFrame < m_frames.size());
    if (currFrame < m_frames.size())
    {
        m_uCurrFrame = currFrame;
    }
}

const std::vector<CSprite*>& CFrameSprite::Frames() const
{
    return m_frames;
}
