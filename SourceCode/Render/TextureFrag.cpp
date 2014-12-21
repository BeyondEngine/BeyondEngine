#include "stdafx.h"
#include "TextureFrag.h"
#include "Texture.h"
#include "TextureAtlas.h"

CTextureFrag::CTextureFrag(const TString &name,
                           SharePtr<CTextureAtlas> pAtlas, CVec2 origin, CVec2 size )
    : m_name(name)
    , m_pAtlas(pAtlas)
    , m_origin(origin)
    , m_size(size)
{
    if (m_pAtlas != NULL)
    {
        SharePtr<CTexture> pTexture = GetTexture();
        BEATS_ASSERT(pTexture != NULL);
        uint32_t uTextureWidth = pTexture->Width();
        uint32_t uTextureHeight = pTexture->Height();
        BEATS_ASSERT(uTextureWidth > 0 && uTextureHeight > 0);
        m_quad.tl.u = m_origin.X() / uTextureWidth;
        // NOTICE:
        // add 0.5 offset for UV. 
        // Because screen coordinate is the center of the pixel.
        // But the uv is the center of 4 pixel around. so to make them the same meaning, we move the uv pos to the center of pixel.
        m_quad.tl.v = 1.0f - (m_origin.Y() + 0.5f) / uTextureHeight;
        m_quad.tr.u = (m_origin.X() + m_size.X()) / uTextureWidth;
        m_quad.tr.v = m_quad.tl.v;
        m_quad.bl.u = m_quad.tl.u;
        m_quad.bl.v = 1 - (m_origin.Y() + m_size.Y() - 0.5f) / uTextureHeight;
        m_quad.br.u = m_quad.tr.u;
        m_quad.br.v = m_quad.bl.v;
    }
}

CTextureFrag::~CTextureFrag()
{

}

const TString& CTextureFrag::GetName() const
{
    return m_name;
}

const CQuadT& CTextureFrag::GetQuadT() const
{
    return m_quad;
}

SharePtr<CTextureAtlas> CTextureFrag::GetAtlas() const
{
    return m_pAtlas;
}

SharePtr<CTexture> CTextureFrag::GetTexture() const
{
    return m_pAtlas->Texture();
}

const CVec2& CTextureFrag::GetOrigin() const
{
    return m_origin;
}

const CVec2& CTextureFrag::GetSize() const
{
    return m_size;
}

CVec2 CTextureFrag::ConvertToRealUV(const CVec2& localUV) const
{
    CVec2 realSize(m_quad.tr.u - m_quad.bl.u, m_quad.tr.v - m_quad.bl.v);
    BEATS_ASSERT(realSize.X() > 0 && realSize.Y() > 0);
    CVec2 ret;
    ret.X() = m_quad.bl.u + realSize.X() * localUV.X();
    ret.Y() = m_quad.bl.v + realSize.Y() * localUV.Y();
    return ret;
}