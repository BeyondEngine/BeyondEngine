#ifndef BEYOND_ENGINE_RENDER_TEXTUREFRAG_H__INCLUDE
#define BEYOND_ENGINE_RENDER_TEXTUREFRAG_H__INCLUDE

#include "CommonTypes.h"

class CTextureAtlas;
class CTexture;

class CTextureFrag
{
public:
    CTextureFrag(const TString &name, SharePtr<CTextureAtlas> pAtlas, CVec2 origin, CVec2 size);
    ~CTextureFrag();

    TString Name() const;

    const CQuadT &Quad() const;

    SharePtr<CTextureAtlas> Atlas() const;

    SharePtr<CTexture> Texture() const;

    const CVec2& Origin() const;

    const CVec2& Size() const;

    void Initialize();

private:
    TString m_name;
    CQuadT m_quad;
    SharePtr<CTextureAtlas> m_pAtlas;
    CVec2 m_origin;
    CVec2 m_size;
};

#endif