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

}

CTextureFrag::~CTextureFrag()
{

}

TString CTextureFrag::Name() const
{
    return m_name;
}

const CQuadT & CTextureFrag::Quad() const
{
    return m_quad;
}

SharePtr<CTextureAtlas> CTextureFrag::Atlas() const
{
    return m_pAtlas;
}

SharePtr<CTexture> CTextureFrag::Texture() const
{
    return m_pAtlas->Texture();
}

const CVec2& CTextureFrag::Origin() const
{
    return m_origin;
}

const CVec2& CTextureFrag::Size() const
{
    return m_size;
}

void CTextureFrag::Initialize()
{
    SharePtr<CTexture> pTexture = Texture();
    BEATS_ASSERT(pTexture != NULL && pTexture->IsInitialized() && pTexture->Width() > 0 && pTexture->Width() > 0);
    m_quad.tl.u = m_origin.x / pTexture->Width();
    m_quad.tl.v = 1 - m_origin.y / pTexture->Height();
    m_quad.tr.u = (m_origin.x + m_size.x) / pTexture->Width();
    m_quad.tr.v = m_quad.tl.v;
    m_quad.bl.u = m_quad.tl.u;
    m_quad.bl.v = 1 - (m_origin.y + m_size.y) / pTexture->Height();
    m_quad.br.u = m_quad.tr.u;
    m_quad.br.v = m_quad.bl.v;
}