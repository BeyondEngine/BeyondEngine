#include "stdafx.h"
#include "AnimatableSprite.h"
#include "Material.h"
#include "Render/RenderGroup.h"
#include "RenderBatch.h"
#include "Action/ActionBase.h"
#include "RenderManager.h"
#include "Texture.h"

CAnimatableSprite::CAnimatableSprite()
    : m_bPlaying(false)
    , m_bNeedFadeOut(false)
    , m_bStartFadeOut(false)
    , m_bAutoHideWhenPlayEnd(false)
    , m_bStopAtFirstFrame(true)
    , m_bBounce(false)
    , m_bReversePlay(false)
    , m_bPlayAtRandomPos(true)
    , m_uInitAlpha(0)
    , m_uFrameCount(0)
    , m_fFadeOutTotalTime(0.0f)
    , m_fFadeOutDeltaTime(0.0f)
    , m_uCurrFrame(0)
    , m_uCurrLoopCount(0)
    , m_uLoopCount(1)
    , m_uAnimationIntervalMS(1000)
    , m_uElapsedTimeInFrameMS(0)
{
}

CAnimatableSprite::~CAnimatableSprite()
{
}

void CAnimatableSprite::Initialize()
{
    super::Initialize();
    UpdateFrameCount();
    if (m_uLoopCount == 0 && m_bPlayAtRandomPos)
    {
        m_uCurrFrame = GetRandomPos();
        SyncCurFrameTexture();
    }
}

bool CAnimatableSprite::Load()
{
    bool bRet = super::Load();
    m_uInitAlpha = GetColorScale(false).a;
    return bRet;
}

void CAnimatableSprite::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bPlaying, true, 0xFFFFFFFF, _T("是否播放"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bReversePlay, true, 0xFFFFFFFF, _T("是否倒叙播放"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bBounce, true, 0xFFFFFFFF, _T("Bounce"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_uLoopCount, true, 0xFFFFFFFF, _T("循环次数"), NULL, _T("如果为0则表示始终循环"), NULL);
    DECLARE_PROPERTY(serializer, m_bPlayAtRandomPos, true, 0xFFFFFFFF, _T("随机位置播放"), NULL, _T("循环时起始位置随机"), _T("VisibleWhen:m_uLoopCount==0"));
    DECLARE_PROPERTY(serializer, m_bAutoHideWhenPlayEnd, true, 0xFFFFFFFF, _T("自动隐藏"), NULL, _T("播放完成后自动隐藏"), _T("VisibleWhen:m_uLoopCount>0"));
    DECLARE_PROPERTY(serializer, m_bNeedFadeOut, true, 0xFFFFFFFF, _T("渐隐消失"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fFadeOutTotalTime, true, 0xFFFFFFFF, _T("渐隐消失时间(秒)"), NULL, NULL, _T("VisibleWhen:m_bNeedFadeOut==true"));
    DECLARE_PROPERTY(serializer, m_bStopAtFirstFrame, true, 0xFFFFFFFF, _T("停止在第一帧"), NULL, _T("播放完成后停止在第一帧或者最后一帧"), _T("VisibleWhen:m_uLoopCount>0"));
    DECLARE_PROPERTY(serializer, m_uAnimationIntervalMS, true, 0xFFFFFFFF, _T("每帧播放时间（毫秒）"), NULL, NULL, _T("MinValue:1, DefaultValue:1000"));
    DECLARE_PROPERTY(serializer, m_uCurrFrame, true, 0xFFFFFFFF, _T("当前帧"), NULL, NULL, _T("MinValue:0"));
    DECLARE_PROPERTY(serializer, m_frames, true, 0xFFFFFFFF, _T("帧序列"), NULL, NULL, NULL);
    HIDE_PROPERTY(m_fragInfo);
}
#ifdef EDITOR_MODE
bool CAnimatableSprite::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bHandled = super::OnPropertyChange(pVariableAddr, pSerializer);
    if (!bHandled)
    {
        if (pVariableAddr == &m_uCurrFrame)
        {
            DeserializeVariable(m_uCurrFrame, pSerializer, this);
            UpdateRealSize();
            bHandled = true;
        }
        if (pVariableAddr == &m_frames)
        {
            DeserializeVariable(m_frames, pSerializer, this);
            UpdateFrameCount();
            UpdateRealSize();
            bHandled = true;
        }
        else if (pVariableAddr == &m_uLoopCount)
        {
            DeserializeVariable(m_uLoopCount, pSerializer, this);
            if (IsPlaying())
            {
                Stop(true);
                Play();
            }
            bHandled = true;
        }
        else if (pVariableAddr == &m_bPlaying)
        {
            bool bPlaying = false;
            DeserializeVariable(bPlaying, pSerializer, this);
            bPlaying ? Play() : Reset(true);
            bHandled = true;
            BEATS_ASSERT(bPlaying == m_bPlaying);
        }
    }
    return bHandled;
}
#endif
void CAnimatableSprite::Update(float dtt)
{
    dtt *= m_fPlaySpeed;
    if (m_bPlaying && m_uFrameCount > 1)
    {
        m_uElapsedTimeInFrameMS += (uint32_t)(dtt * 1000);
        bool bNeedSwitchFrame = m_uElapsedTimeInFrameMS >= m_uAnimationIntervalMS;
        if (bNeedSwitchFrame)
        {
            uint32_t uNextFrame = 0;
            bool bNeedIncreaseLoopCount = m_bReversePlay ? m_uCurrFrame == 0 : m_uCurrFrame == m_uFrameCount - 1;
            if (!bNeedIncreaseLoopCount)
            {
                uNextFrame = m_uCurrFrame + (m_bReversePlay ? -1 : 1);
                SetCurrFrame(uNextFrame);
            }
            else
            {
                ++m_uCurrLoopCount;
                if (m_uLoopCount == 0 || m_uCurrLoopCount < m_uLoopCount)
                {
                    if (m_bBounce)
                    {
                        uNextFrame = m_uCurrFrame == 0 ? 0 : m_uFrameCount - 1;
                        m_bReversePlay = !m_bReversePlay;
                    }
                    else
                    {
                        uNextFrame = m_uCurrFrame == 0 ? m_uFrameCount - 1 : 0;
                    }
                    SetCurrFrame(uNextFrame);
                }
                else
                {
                    Stop(m_bStopAtFirstFrame);
                    if (m_bAutoHideWhenPlayEnd && !m_bNeedFadeOut)
                    {
                        SetVisible(false);
                    }
                }
            }
            m_uElapsedTimeInFrameMS = 0;
        }
        if (m_bStartFadeOut)
        {
            CColor colorScale = GetColorScale(false);
            m_fFadeOutDeltaTime += dtt;
            if (m_fFadeOutDeltaTime >= m_fFadeOutTotalTime / m_uInitAlpha)
            {
                unsigned char uDeltaAlpha = (unsigned char)(m_fFadeOutDeltaTime / (m_fFadeOutTotalTime / m_uInitAlpha));
                m_fFadeOutDeltaTime = 0.0f;
                colorScale.a = colorScale.a > uDeltaAlpha ? colorScale.a - uDeltaAlpha : 0;
                SetColorScale(colorScale);
                if (colorScale.a == 0)
                {
                    m_bPlaying = false;
                    m_uCurrLoopCount = 0;
                    m_uElapsedTimeInFrameMS = 0;
                    if (m_bStopAtFirstFrame)
                    {
                        SetCurrFrame(0);
                    }
                    if (m_bAutoHideWhenPlayEnd)
                    {
                        SetVisible(false);
                    }
                    m_bStartFadeOut = false;
                    m_fFadeOutDeltaTime = 0.0f;
                }
            }
        }
    }
}

void CAnimatableSprite::SetCurrFrame(uint32_t currFrame)
{
    if (currFrame >= m_uFrameCount)
    {
        currFrame = m_uFrameCount > 0 ? m_uFrameCount - 1 : 0;
    }
    m_uCurrFrame = currFrame;
}

uint32_t CAnimatableSprite::GetFrameCount() const
{
    return m_uFrameCount;
}

uint32_t CAnimatableSprite::GetCurrFrame() const
{
    return m_uCurrFrame;
}

const CQuadT &CAnimatableSprite::GetQuadT() const
{
    SharePtr<CTextureFrag> pFrameFrag = GetTextureFrag();
    return pFrameFrag ? pFrameFrag->GetQuadT() : super::GetQuadT();
}

SharePtr<CTextureFrag> CAnimatableSprite::GetTextureFrag() const
{
    SharePtr<CTextureFrag> pRet;
    if (!m_frames.empty() && m_uCurrFrame < m_frames.size())
    {
        const SReflectTextureInfo &frame = m_frames[m_uCurrFrame];
        pRet = frame.GetTextureFrag();
    }
    return pRet;
}

void CAnimatableSprite::SetLoopCount(uint32_t uLoopCount)
{
    m_uLoopCount = uLoopCount;
}

uint32_t CAnimatableSprite::GetLoopCount() const
{
    return m_uLoopCount;
}

void CAnimatableSprite::Play()
{
    if (m_uFrameCount >= 1)
    {
        m_bPlaying = true;
        if (m_uLoopCount == 0 && m_bPlayAtRandomPos)
        {
            m_uCurrFrame = GetRandomPos();
        }
    }
}

void CAnimatableSprite::Play(bool bPause, bool bContinuePlay, bool bReversePlay, uint32_t uStartIndex, float fSpeed)
{
    m_bPlaying = bPause;
    m_fPlaySpeed = fSpeed;
    if (m_bPlaying)
    {
        m_bReversePlay = bReversePlay;
        if (!bContinuePlay)
        {
            if (uStartIndex >= m_uFrameCount)
            {
                uStartIndex = uStartIndex % m_uFrameCount;
            }
            m_uCurrFrame = uStartIndex;
        }
    }
}

void CAnimatableSprite::Pause()
{
    m_bPlaying = false;
}

void CAnimatableSprite::Stop(bool bResetAtStart)
{
    BEATS_ASSERT(m_bPlaying);
    if (m_bNeedFadeOut)
    {
        m_bStartFadeOut = true;
    }
    else
    {
        Reset(bResetAtStart);
    }
}

bool CAnimatableSprite::IsPlaying() const
{
    return m_bPlaying;
}

void CAnimatableSprite::SetStopAtFirstFrame(bool bStopAtFirstFrame)
{
    m_bStopAtFirstFrame = bStopAtFirstFrame;
}

bool CAnimatableSprite::GetStopAtFirstFrame() const
{
    return m_bStopAtFirstFrame;
}

void CAnimatableSprite::SetAutoHideWhenEnd(bool bAutoHide)
{
    m_bAutoHideWhenPlayEnd = bAutoHide;
}

bool CAnimatableSprite::GetAutoHideWhenEnd() const
{
    return m_bAutoHideWhenPlayEnd;
}

void CAnimatableSprite::InheritState(CAnimatableSprite* pSource)
{
    m_uCurrLoopCount = pSource->m_uCurrLoopCount;
    SetCurrFrame(pSource->m_uCurrFrame);
    m_uElapsedTimeInFrameMS = pSource->m_uElapsedTimeInFrameMS;
    m_bPlaying = pSource->m_bPlaying;
}

void CAnimatableSprite::SetElapsedTimeMS(uint32_t uElapsedTime)
{
    m_uElapsedTimeInFrameMS = uElapsedTime;
}

uint32_t CAnimatableSprite::GetElapsedTimeMS() const
{
    return m_uElapsedTimeInFrameMS;
}

uint32_t CAnimatableSprite::GetAnimationIntervalMS() const
{
    return m_uAnimationIntervalMS;
}

void CAnimatableSprite::SetReversePlay(bool bValue)
{
    m_bReversePlay = bValue;
}

bool CAnimatableSprite::GetReversePlay() const
{
    return m_bReversePlay;
}

void CAnimatableSprite::Reset(bool bResetAtStart)
{
    m_bPlaying = false;
    m_uCurrLoopCount = 0;
    m_uElapsedTimeInFrameMS = 0;
    m_bStartFadeOut = false;
    CColor colorScale = GetColorScale(false);
    colorScale.a = m_uInitAlpha;
    SetColorScale(colorScale);
    if (bResetAtStart)
    {
        SetCurrFrame(0);
    }
}

void CAnimatableSprite::SetPlaySpeed(float fSpeed)
{
    m_fPlaySpeed = fSpeed;
}

float CAnimatableSprite::GetPlaySpeed() const
{
    return m_fPlaySpeed;
}

uint32_t CAnimatableSprite::GetRandomPos() const
{
    return m_uFrameCount > 0 ? rand() % m_uFrameCount : 0;
}

void CAnimatableSprite::SyncCurFrameTexture()
{
    SetCurrFrame(m_uCurrFrame);
}

bool CAnimatableSprite::HitTest(const CVec2& pt)
{
    CRay3 selectRay = CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D)->GetRayFromScreenPos(pt.X(), pt.Y());
    CQuadP quad = GetQuadP();
    const CMat4& worldTM = GetWorldTM();
    quad.bl *= worldTM;
    quad.br *= worldTM;
    quad.tl *= worldTM;
    quad.tr *= worldTM;
    CVec3 hitRet;
    return IsVisible() && (selectRay.IntersectTriangleBothSide(quad.tl, quad.tr, quad.bl, hitRet) || selectRay.IntersectTriangleBothSide(quad.bl, quad.br, quad.tr, hitRet));
}

void CAnimatableSprite::UpdateFrameCount()
{
    m_uFrameCount = m_frames.size();
}