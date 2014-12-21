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

    const TString& GetName() const;
    const CQuadT &GetQuadT() const;
    SharePtr<CTextureAtlas> GetAtlas() const;
    SharePtr<CTexture> GetTexture() const;
    const CVec2& GetOrigin() const;
    const CVec2& GetSize() const;
    CVec2 ConvertToRealUV(const CVec2& localUV) const;
#ifdef EDITOR_MODE
    bool m_bTrimmed = false;
    CVec2 m_originSize;
    CVec2 m_originOffset;
#endif

private:
    TString m_name;
    CQuadT m_quad;
    SharePtr<CTextureAtlas> m_pAtlas;
    CVec2 m_origin;
    CVec2 m_size;
};

#endif