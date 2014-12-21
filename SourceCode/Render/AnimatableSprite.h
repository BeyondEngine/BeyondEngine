#ifndef BEYOND_ENGINE_RENDER_ANIMATABLESPRITE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_ANIMATABLESPRITE_H__INCLUDE

#include "Sprite.h"
#include "ReflectTextureInfo.h"
class CActionBase;

class CAnimatableSprite : public CSprite
{
    DECLARE_REFLECT_GUID( CAnimatableSprite, 0xe3bb1021, CSprite )
public:
    CAnimatableSprite();
    virtual ~CAnimatableSprite();

    virtual void Initialize() override;
    virtual void ReflectData(CSerializer& serializer) override;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
#endif
    virtual void Update(float dtt) override;
    virtual bool Load() override;
    virtual void UpdateFrameCount();
    virtual uint32_t GetRandomPos() const;
    virtual void SyncCurFrameTexture();
    virtual bool HitTest(const CVec2& pt) override;

    uint32_t GetFrameCount() const override;
    uint32_t GetCurrFrame() const;
    virtual void SetCurrFrame(uint32_t currFrame) override;

    virtual const CQuadT& GetQuadT() const override;
    virtual SharePtr<CTextureFrag> GetTextureFrag() const override;
    virtual void Play(bool bPause, bool bContinuePlay, bool bReversePlay, uint32_t uStartIndex, float fSpeed) override;

    void SetLoopCount(uint32_t uLoopCount);
    uint32_t GetLoopCount() const;
    void Play();
    void Pause();
    void Stop(bool bResetAtStart);
    bool IsPlaying() const;
    void SetStopAtFirstFrame(bool bStopAtFirstFrame);
    bool GetStopAtFirstFrame() const;
    void SetAutoHideWhenEnd(bool bAutoHide);
    bool GetAutoHideWhenEnd() const;
    void InheritState(CAnimatableSprite* pSource);

    void SetReversePlay(bool bValue);
    bool GetReversePlay() const;

    void Reset(bool bResetAtStart);
    void SetPlaySpeed(float fSpeed);
    float GetPlaySpeed() const;

protected:
    void SetElapsedTimeMS(uint32_t uElapsedTime);
    uint32_t GetElapsedTimeMS() const;
    uint32_t GetAnimationIntervalMS() const;

protected:
    uint32_t m_uFrameCount;

private:
    bool m_bPlaying;
    bool m_bNeedFadeOut;
    bool m_bPlayAtRandomPos;
    bool m_bStartFadeOut;
    bool m_bAutoHideWhenPlayEnd;
    bool m_bStopAtFirstFrame;
    bool m_bBounce;
    bool m_bReversePlay;
    float m_fPlaySpeed = 1.0f;
    unsigned char m_uInitAlpha;
    uint32_t m_uCurrFrame;
    uint32_t m_uCurrLoopCount;
    uint32_t m_uLoopCount;
    uint32_t m_uAnimationIntervalMS;
    uint32_t m_uElapsedTimeInFrameMS;
    float m_fFadeOutDeltaTime;
    float m_fFadeOutTotalTime;
    std::vector<SReflectTextureInfo> m_frames;
};

#endif