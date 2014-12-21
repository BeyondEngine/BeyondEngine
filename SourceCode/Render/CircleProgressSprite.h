#ifndef BEYOND_ENGINE_RENDER_CIRCLEPROGRESSSPRITE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_CIRCLEPROGRESSSPRITE_H__INCLUDE

#include "Sprite.h"

class CShaderUniform;
class CMaterial;

class CCircleProgressSprite : public CSprite
{
    DECLARE_REFLECT_GUID(CCircleProgressSprite, 0xb9e3b077, CSprite)
public:
    CCircleProgressSprite();
    virtual ~CCircleProgressSprite();
    virtual void Initialize() override;
    virtual void DoRender() override;
    void SetTotleCount(float count);
    void SetCurrentCount(float count);

private:
    float m_fTotleCount = 0;
    float m_fCurrentCount = 0;
    CColor m_maskColor = 0;
    SharePtr<CMaterial> m_pCircleProgressSpriteMaterial = nullptr;
};

#endif