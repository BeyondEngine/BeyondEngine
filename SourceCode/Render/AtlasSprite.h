#ifndef BEYOND_ENGINE_RENDER_ATLASSPRITE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_ATLASSPRITE_H__INCLUDE

#include "AnimatableSprite.h"

class CAtlasSprite : public CAnimatableSprite
{
    DECLARE_REFLECT_GUID(CAtlasSprite, 0x131bc2da, CAnimatableSprite)
public:
    CAtlasSprite();
    virtual ~CAtlasSprite();
    virtual bool Load() override;
    virtual void ReflectData(CSerializer& serializer) override;
    virtual void DoRender() override;
#ifdef EDITOR_MODE
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
#endif
    virtual bool HitTest(const CVec2& pt) override;
#ifdef EDITOR_MODE
    virtual void SetSyncProxyComponent(CComponentProxy* pProxy) override;
#endif
    virtual void UpdateFrameCount() override;
    virtual uint32_t GetRandomPos() const override;
    virtual void SyncCurFrameTexture() override;
    virtual SharePtr<CTextureFrag> GetTextureFrag() const override;
    virtual void Update(float dtt) override;
    virtual void SetCurrFrame(uint32_t currFrame) override;

    const std::map<int, CSprite*>& GetSpriteFrameMap() const;
    std::vector<CAtlasSprite*>& GetChildAtlasSprite();
    CSprite* GetSpriteFrame(int nFrameIndex);
    CSprite* GetCurrentSprite() const;

    virtual CRect GetSelfRect() const override;

    virtual void GetCurrentRenderSpriteList(std::vector<CSprite*>& spriteList) const override;

private:
    int m_nCurrRenderFrame;
    std::map<int, CSprite*> m_spriteFramesMap;
    std::vector<CAtlasSprite*> m_childList;
};

#endif