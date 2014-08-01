#ifndef BEYOND_ENGINE_RENDER_SPRITE_H__INCLUDE
#define BEYOND_ENGINE_RENDER_SPRITE_H__INCLUDE

#include "Scene/Node3D.h"
#include "CommonTypes.h"
#include "ReflectTextureInfo.h"

class CTexture;
class CTextureFrag;
class CMaterial;

class CSprite : public CNode3D
{
    DECLARE_REFLECT_GUID( CSprite, 0xe3bb1001, CNode3D )
public:
    CSprite();
    virtual ~CSprite();

    virtual void ReflectData(CSerializer& serializer) override;
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer) override;
    virtual void Initialize() override;

    virtual void PreRender() override;
    virtual void DoRender() override;
    virtual bool HitTest(float x, float y) const override;

    void SetAnchor(float anchorX, float anchorY);
    const kmVec2 &Anchor() const;
    void SetSize(float width, float height);
    const kmVec2 &Size() const;
    void UpdateVertices();

    const CQuadP &QuadP() const;
    virtual const CQuadT &QuadT() const;
    virtual CTextureFrag* GetTextureFrag() const;
    virtual SharePtr<CMaterial> Material();

    void SetUseInheritColorScale(bool bUse);

protected:
    void OnTextureFragChanged();

private:
    void SetRealSize(float fWidth, float fHeight);

protected:
    bool m_bUseInheritColorScale;
    CVec2 m_anchor;
    CVec2 m_size;
    CVec2 m_realSize;
    SReflectTextureInfo m_fragInfo;
    SharePtr<CMaterial> m_pMaterial;
    CQuadP m_quad;
};

#endif