#include "stdafx.h"
#include "AnimatableSprite.h"
#include "Material.h"
#include "TextureFrag.h"
#include "Render/RenderGroup.h"
#include "RenderGroupManager.h"
#include "RenderBatch.h"
#include "CompWrapper.h"

CAnimatableSprite::CAnimatableSprite()
    : m_bPlaying(false)
    , m_bLoop(false)
    , m_uCurrFrame(0)
    , m_fAnimationIntervalMS(1000.0f)
    , m_fElapsedTime(0)
{
}

CAnimatableSprite::~CAnimatableSprite()
{
}

void CAnimatableSprite::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bPlaying, true, 0xFFFFFFFF, _T("是否播放"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bLoop, true, 0xFFFFFFFF, _T("是否循环"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fAnimationIntervalMS, true, 0xFFFFFFFF, _T("每帧播放时间（毫秒）"), NULL, NULL, _T("MinValue:1, DefaultValue:1000"));
    DECLARE_PROPERTY(serializer, m_uCurrFrame, true, 0xFFFFFFFF, _T("当前帧"), NULL, NULL, _T("MinValue:0"));
    DECLARE_PROPERTY(serializer, m_frames, true, 0xFFFFFFFF, _T("帧序列"), NULL, NULL, NULL);
    HIDE_PROPERTY(m_anchor);
    HIDE_PROPERTY(m_fragInfo);
    HIDE_PROPERTY(m_size);
}

bool CAnimatableSprite::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bHandled = super::OnPropertyChange(pVariableAddr, pSerializer);
    if (!bHandled)
    {
        if (pVariableAddr == &m_uCurrFrame)
        {
            DeserializeVariable(m_uCurrFrame, pSerializer);
            bHandled = true;
            OnTextureFragChanged();
        }
    }
    return bHandled;
}

void CAnimatableSprite::Update(float ddt)
{
    if (m_bPlaying && m_frames.size() > 1)
    {
        m_fElapsedTime += ddt * 1000;
        if (m_fElapsedTime >= m_fAnimationIntervalMS)
        {
            if (m_uCurrFrame < m_frames.size() - 1)
            {
                SetCurrFrame(m_uCurrFrame + 1);
            }
            else if (m_bLoop)
            {
                SetCurrFrame(0);
            }
            m_fElapsedTime = 0;
        }
    }
}

void CAnimatableSprite::SetFrames(const std::vector<SReflectTextureInfo>& frames)
{
    m_frames = frames;
}

void CAnimatableSprite::AddFrame(SReflectTextureInfo frame)
{
    m_frames.push_back(frame);
}

const std::vector<SReflectTextureInfo>& CAnimatableSprite::Frames() const
{
    return m_frames;
}

void CAnimatableSprite::SetCurrFrame(size_t currFrame)
{
    if(currFrame >= m_frames.size())
    {
        m_uCurrFrame = m_frames.size() - 1;
    }
    else
    {
        m_uCurrFrame = currFrame;
    }
    OnTextureFragChanged();
}

size_t CAnimatableSprite::CurrFrame() const
{
    return m_uCurrFrame;
}

const CQuadT &CAnimatableSprite::QuadT() const
{
    CTextureFrag *pFrameFrag = nullptr;
    if(!m_frames.empty() && m_uCurrFrame < m_frames.size())
    {
        const SReflectTextureInfo &frame = m_frames[m_uCurrFrame];
        if(frame.GetTextureFrag())
        {
            pFrameFrag = frame.GetTextureFrag();
        }
    }
    return pFrameFrag ? pFrameFrag->Quad() : super::QuadT();
}

CTextureFrag* CAnimatableSprite::GetTextureFrag() const
{
    CTextureFrag* pRet = NULL;
    if(!m_frames.empty() && m_uCurrFrame < m_frames.size())
    {
        const SReflectTextureInfo &frame = m_frames[m_uCurrFrame];
        if(frame.GetTextureFrag())
        {
            pRet = frame.GetTextureFrag();
        }
        else
        {
            pRet = super::GetTextureFrag();
        }
    }
    return pRet;
}
