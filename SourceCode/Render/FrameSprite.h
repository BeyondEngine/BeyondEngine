#ifndef BEYOND_ENGINE_RENDER_FRAMESPRITE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_FRAMESPRITE_H__INCLUDE

#include "Sprite.h"

enum EFrameSpritePlayType
{
    eFSPT_Once,
    eFSPT_Loop,
    eFSPT_Bounce,

    eFSPT_Count,
    eFSPT_Force32Bit = 0xFFFFFFFF
};

class CFrameSprite : public CSprite
{
    DECLARE_REFLECT_GUID( CFrameSprite, 0x910B44A8, CSprite )
public:
    CFrameSprite();
    virtual ~CFrameSprite();

    virtual void ReflectData(CSerializer& serializer) override;
    virtual void Update(float ddt) override;
    virtual void DoRender() override;
    virtual void PostRender() override;

    size_t GetCurrFrame() const;
    void SetCurrFrame(size_t currFrame);
    const std::vector<CSprite*>& Frames() const;

private:
    bool m_bPlaying;
    bool m_bReversePlay;
    EFrameSpritePlayType m_playType;
    size_t m_uCurrFrame;
    float m_fAnimationIntervalMS;
    float m_fElapsedTime;
    std::vector<CSprite*> m_frames;
};

#endif