#ifndef BEYOND_ENGINE_RENDER_ANIMATABLESPRITE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_ANIMATABLESPRITE_H__INCLUDE

#include "Sprite.h"
#include "ReflectTextureInfo.h"

class CAnimatableSprite : public CSprite
{
    DECLARE_REFLECT_GUID( CAnimatableSprite, 0xe3bb1021, CSprite )
public:
    CAnimatableSprite();
    virtual ~CAnimatableSprite();

    virtual void ReflectData(CSerializer& serializer) override;
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
    virtual void Update(float ddt) override;

    void SetFrames(const std::vector<SReflectTextureInfo>& frames);
    void AddFrame(SReflectTextureInfo frame);
    const std::vector<SReflectTextureInfo>& Frames() const;
    size_t CurrFrame() const;
    void SetCurrFrame(size_t currFrame);

    virtual const CQuadT &QuadT() const override;
    virtual CTextureFrag* GetTextureFrag() const override;

private:
    bool m_bPlaying;
    bool m_bLoop;
    size_t m_uCurrFrame;
    float m_fAnimationIntervalMS;
    float m_fElapsedTime;
    std::vector<SReflectTextureInfo> m_frames;
};

#endif