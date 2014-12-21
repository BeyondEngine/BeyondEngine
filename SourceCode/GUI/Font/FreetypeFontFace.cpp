#include "stdafx.h"
#include "FreetypeFontFace.h"
#include "Font.h"
#include "Render/Material.h"
#include "Resource/ResourceManager.h"
#include "FontManager.h"
#include "Render/RenderManager.h"
#include "Render/RenderGroup.h"
#include "Render/VertexFormat.h"
#include "Render/RenderBatch.h"
#include "Render/RenderTarget.h"
#include "Render/RenderState.h"
#include "Render/Renderer.h"
#include "Render/Texture.h"

#include FT_FREETYPE_H
#include FT_STROKER_H

#define FT_SHIFT_NUM 6

//next multiple of 8
inline int32_t nextMOE(int32_t x)
{
    return (x + 7) & 0xFFFFFFF8;
}

CFreetypeFontFace::CFreetypeFontFace(const TString &name, const TString &file, int32_t size, int32_t dpi/*= -1*/)
    : CFontFace(name)
    , m_pFont(CResourceManager::GetInstance()->GetResource<CFont>(file))
    , m_uCurrX(0)
    , m_uCurrY(0)
    , m_nDpi(dpi)
    , m_nLineHeight(0)
    , m_nAscender(0)
    , m_pRelatedRenderTarget(nullptr)
{
    m_nSize = size;
    NewPage();
    ApplyFTSize();
}

CFreetypeFontFace::~CFreetypeFontFace()
{
    m_pTexture->Unload();
    m_fontUpdateImageCacheMutex.lock();
    BEATS_SAFE_DELETE_VECTOR(m_fontUpdateImageCache);
    m_fontUpdateImageCacheMutex.unlock();
}

void CFreetypeFontFace::NewPage()
{
    BEATS_ASSERT(m_pTexture.Get() == nullptr);
    static const int32_t nDataSize = PAGE_WIDTH * PAGE_HEIGHT * 2;
    char *pZeroBuf = new char[nDataSize];
    memset(pZeroBuf, 0, nDataSize);
    m_pTexture = new CTexture;
    m_pTexture->SetFilePath(_T("FreetypeFontFaceTexture"));
    m_pTexture->InitWithData(pZeroBuf, nDataSize, PixelFormat::AI88, PAGE_WIDTH, PAGE_HEIGHT);
    BEATS_SAFE_DELETE_ARRAY(pZeroBuf);
    m_pTexture->SetInitializeFlag(true);
    m_pTexture->SetLoadFlag(true);
}

void CFreetypeFontFace::ApplyFTSize()
{
    const float fScale = GetScaleFactor();
    FT_Error err = 0;
    if (m_nDpi == -1)
    {
        err = FT_Set_Pixel_Sizes(m_pFont->GetFontFace(), (int32_t)(m_nSize * fScale), (int32_t)(m_nSize * fScale));
        BEATS_ASSERT(!err);
    }
    else
    {
        FT_F26Dot6 sizeTrans = (int32_t)(m_nSize * fScale) << FT_SHIFT_NUM;
        err = FT_Set_Char_Size(m_pFont->GetFontFace(), sizeTrans, sizeTrans, m_nDpi, m_nDpi);
        BEATS_ASSERT(!err);
    }

    const FT_Size& ftSize = m_pFont->GetFontFace()->size;
    m_nAscender = ftSize->metrics.ascender >> FT_SHIFT_NUM;
    int32_t descender = ftSize->metrics.descender >> FT_SHIFT_NUM;
    m_nLineHeight = MAX(m_nLineHeight, m_nAscender - descender);
}

FT_Face CFreetypeFontFace::GetFontFace() const
{
    FT_Face pRet = NULL;
    if (m_pFont)
    {
        pRet = m_pFont->GetFontFace();
    }
    return pRet;
}

void CFreetypeFontFace::SetFontSize(int32_t nFontSize)
{
    m_nSize = nFontSize;
}

const CFontGlyph *CFreetypeFontFace::PrepareChar(wchar_t character, bool& bGlyphRestFlag)
{
    bGlyphRestFlag = false;
    m_glyphMapLocker.lock();
    auto itr = m_glyphMap.find(character);
    CFreetypeFontGlyph *pGlyph = itr != m_glyphMap.end() ? down_cast<CFreetypeFontGlyph *>(itr->second) : nullptr;
    m_glyphMapLocker.unlock();

    if (!pGlyph)
    {
        float outlineWidth = GetBorderWeight() * GetScaleFactor();
        ApplyFTSize();
        FT_Face pFontFace = m_pFont->GetFontFace();
        BEATS_ASSERT(pFontFace != NULL);
        bool bFindCharacterGlyph = FT_Get_Char_Index(pFontFace, character) != 0;
        BEYONDENGINE_UNUSED_PARAM(bFindCharacterGlyph);
        BEATS_ASSERT(bFindCharacterGlyph, _T("Character %d can't be found in all font face!"), character);
        FT_Error err = FT_Load_Char(pFontFace, character, FT_LOAD_NO_BITMAP);
        BEYONDENGINE_UNUSED_PARAM(err);
        BEATS_ASSERT(!err);

        FT_GlyphSlot pGlyphSlot = pFontFace->glyph;
        BEATS_ASSERT(pGlyphSlot != NULL);
        int32_t nBorderAdvanceX = pGlyphSlot->metrics.horiAdvance >> FT_SHIFT_NUM;
        int32_t nBorderAdvanceY = m_nLineHeight + (uint32_t)ceil(outlineWidth * 2.0f);
        int32_t nFontAdvanceX = nBorderAdvanceX;
        int32_t nFontHeight = 0;
        int32_t nBorderOriginWidth = 0;
        int32_t nFontOriginWidth = 0;
        int32_t nBorderHeight = 0;
        FT_BBox borderBox;
        FT_BBox fontBox;
        int32_t x = 0, y = 0;
        if (pGlyphSlot->format == FT_GLYPH_FORMAT_OUTLINE)
        {
            FT_Library ftLib = CFont::GetLibrary();
            // Set up a stroker.
            FT_Stroker stroker;
            FT_Stroker_New(ftLib, &stroker);
            FT_Stroker_Set(stroker, (int32_t)(outlineWidth * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
            FT_Glyph pOutlineGlyph, pInnerGlyph;
            if (FT_Get_Glyph(pGlyphSlot, &pOutlineGlyph) == 0 && FT_Get_Glyph(pGlyphSlot, &pInnerGlyph) == 0)
            {
                FT_Glyph_StrokeBorder(&pOutlineGlyph, stroker, 0, 1);
                BEATS_ASSERT(pOutlineGlyph->format == FT_GLYPH_FORMAT_OUTLINE && pInnerGlyph->format == FT_GLYPH_FORMAT_OUTLINE);
                FT_Outline *pOutLine = &reinterpret_cast<FT_OutlineGlyph>(pOutlineGlyph)->outline;
                FT_Glyph_Get_CBox(pOutlineGlyph, FT_GLYPH_BBOX_GRIDFIT, &borderBox);
                FT_Glyph_Get_CBox(pInnerGlyph, FT_GLYPH_BBOX_GRIDFIT, &fontBox);

                nBorderOriginWidth = (borderBox.xMax - borderBox.xMin) >> FT_SHIFT_NUM;

                nFontOriginWidth = (fontBox.xMax - fontBox.xMin) >> FT_SHIFT_NUM;
                int32_t nBorderWidth = nextMOE(nBorderOriginWidth); // Because our GL_UNPACK_ALIGNMENT should be 8 here.
                nBorderHeight = (borderBox.yMax - borderBox.yMin) >> FT_SHIFT_NUM;
                nFontHeight = (fontBox.yMax - fontBox.yMin) >> FT_SHIFT_NUM;
                x = pGlyphSlot->metrics.horiBearingX >> FT_SHIFT_NUM;
                y = pGlyphSlot->metrics.horiBearingY >> FT_SHIFT_NUM;
                if(nBorderAdvanceX < x + nBorderOriginWidth) // It is true for most of the time, because border size always greater than nAdvanceX
                {
                    nBorderAdvanceX = x + nBorderOriginWidth;
                }
                if (nFontAdvanceX < x + nFontOriginWidth)
                {
                    nFontAdvanceX = nFontOriginWidth; 
                }
                if(m_uCurrX + x + nBorderWidth > PAGE_WIDTH)
                {
                    m_uCurrX = 0;
                    m_uCurrY += (nBorderAdvanceY + m_nBorderSpace);
                    if (m_uCurrY + nBorderAdvanceY > PAGE_HEIGHT)
                    {
                        BEATS_WARNING(false, "Freetype texture buffer overflow for %d glyphs, we will rebuild this texture buffer.", (uint32_t)m_glyphMap.size());
                        ResetGlyphs();
                        bGlyphRestFlag = true;
                        return nullptr;
                    }
                }

                int32_t nDataSize = nBorderWidth * nBorderHeight;
                float fBorderOffsetY = 1.0f; // Makes it look like a shadow.
                unsigned char* pBorderData = RenderFontDataToBmp(nBorderWidth, nBorderHeight, -borderBox.xMin, (int32_t)(-borderBox.yMin * fBorderOffsetY), pOutLine);

                FT_Outline *pInnerOutLine = &reinterpret_cast<FT_OutlineGlyph>(pInnerGlyph)->outline;
                unsigned char* pFontData = RenderFontDataToBmp(nBorderWidth, nBorderHeight, -borderBox.xMin, -borderBox.yMin, pInnerOutLine);

                unsigned char* pAllData = new unsigned char[nDataSize * 2];
                for (int32_t i = 0; i < nDataSize; ++i)
                {
                    pAllData[i * 2] = pBorderData[i];
                    pAllData[i * 2 + 1] = pFontData[i];
                }

                BEATS_ASSERT(m_pTexture.Get() != nullptr);
                GLint nX = MAX((int32_t)m_uCurrX + x, 0);
                GLint nY = MAX((int32_t)m_uCurrY + (m_nAscender - y), 0);
                SFontUpdateImageInfo* pImageInfo = new SFontUpdateImageInfo;
                pImageInfo->m_pTexture = m_pTexture;
                pImageInfo->m_nWidth = nBorderWidth;
                pImageInfo->m_nHeight = nBorderHeight;
                pImageInfo->m_x = nX;
                pImageInfo->m_y = nY;
                pImageInfo->m_pData = pAllData;
                m_fontUpdateImageCacheMutex.lock();
                m_fontUpdateImageCache.push_back(pImageInfo);
                m_fontUpdateImageCacheMutex.unlock();

                // Clean up afterwards.
                FT_Stroker_Done(stroker);
                FT_Done_Glyph(pOutlineGlyph);
                FT_Done_Glyph(pInnerGlyph);
                BEATS_SAFE_DELETE_ARRAY(pBorderData);
                BEATS_SAFE_DELETE_ARRAY(pFontData);
            }
        }
        pGlyph = new CFreetypeFontGlyph;
        const float fScale = GetScaleFactor();
        pGlyph->SetWidth((float)nBorderAdvanceX / fScale, true);
        pGlyph->SetWidth((float)nFontAdvanceX / fScale, false);
        pGlyph->SetHeight((float)nBorderAdvanceY / fScale, true);
        pGlyph->SetHeight((float)m_nLineHeight / fScale, false);
        pGlyph->SetU(true, (float)m_uCurrX / PAGE_WIDTH);
        pGlyph->SetV(true, (float)m_uCurrY / PAGE_HEIGHT);
        BEATS_ASSERT(nFontOriginWidth <= nBorderOriginWidth);
        pGlyph->SetU(false, (float)(m_uCurrX + (nBorderOriginWidth - nFontOriginWidth) * 0.5f) / PAGE_WIDTH);
        pGlyph->SetV(false, (float)(m_uCurrY + (nBorderHeight - nFontHeight) * 0.5f) / PAGE_HEIGHT);
        pGlyph->texture = m_pTexture;
        m_glyphMapLocker.lock();
        m_glyphMap[character] = pGlyph;
        m_glyphMapLocker.unlock();
        BEATS_ASSERT(nBorderAdvanceX > 0);
        m_uCurrX += (nBorderAdvanceX + m_nBorderSpace);
    }
    return pGlyph;
}

void CFreetypeFontFace::ClicPointToRect(CVertexPTCC& vertex, const CRect* pRect, float fQuadWidth, float fQuadHeight, float fTexWidth, float fTextHeight ) const
{
    CVec2 vec2Distance = GetDistanceBetweenPointAndRect(vertex.position, pRect);
    if (!BEATS_FLOAT_EQUAL(vec2Distance.X(), 0) || !BEATS_FLOAT_EQUAL(vec2Distance.Y(), 0))
    {
        UpdateVertice( vertex.position, vertex.tex, fQuadWidth, fQuadHeight, fTexWidth, fTextHeight, vec2Distance );
    }
}

bool CFreetypeFontFace::AdjustQuadCornerByRectClip(CQuadPTCC& quad, const CRect *pRect) const
{
    // We detect if the quad intersects with the rect first, if never across, do nothing.
    // 1. Check left top
    bool bOutOfScope = quad.tl.position.X() > pRect->position.X() + pRect->size.X() || quad.tl.position.Y() > pRect->position.Y() + pRect->size.Y();
    // 2. Check right top
    bOutOfScope = bOutOfScope && (quad.tr.position.X() < pRect->position.X() || quad.tr.position.Y() > pRect->position.Y() + pRect->size.Y());
    // 3. Check right bottom
    bOutOfScope = bOutOfScope && (quad.br.position.X() < pRect->position.X() || quad.br.position.Y() < pRect->position.Y());
    // 4. Check left bottom
    bOutOfScope = bOutOfScope && (quad.bl.position.X() > pRect->position.X() + pRect->size.X() || quad.bl.position.Y() > pRect->position.Y());
    // The quad intersect with the rect, we try to find the point to clip.
    if (!bOutOfScope)
    {
        float fTexWidth = quad.tr.tex.u - quad.tl.tex.u;
        float fTexHeight = quad.br.tex.v - quad.tr.tex.v;
        float fQuadWidth = quad.tr.position.X() - quad.tl.position.X();
        float fQuadHeight = quad.br.position.Y() - quad.tr.position.Y();

        ClicPointToRect(quad.bl, pRect, fQuadWidth, fQuadHeight, fTexWidth, fTexHeight);
        ClicPointToRect(quad.tl, pRect, fQuadWidth, fQuadHeight, fTexWidth, fTexHeight);
        ClicPointToRect(quad.br, pRect, fQuadWidth, fQuadHeight, fTexWidth, fTexHeight);
        ClicPointToRect(quad.tr, pRect, fQuadWidth, fQuadHeight, fTexWidth, fTexHeight);
    }
    return bOutOfScope;
}

void CFreetypeFontFace::DrawGlyph(CRenderBatch* pBatch, const CFontGlyph *glyph, float x, float y, float fFontSize, const CColor& color, const CColor& borderColor,
    const CMat4 *transform, const CRect *pRect, float fAlphaScale) const
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderText4)
    if (m_fontUpdateImageCache.size() > 0)
    {
        CFreetypeFontFace* pThis = const_cast<CFreetypeFontFace*>(this);
        pThis->m_fontUpdateImageCacheMutex.lock();
        for (size_t i = 0; i < m_fontUpdateImageCache.size(); ++i)
        {
            SFontUpdateImageInfo* pImageInfo = m_fontUpdateImageCache[i];
            BEATS_ASSERT(pImageInfo && pImageInfo->m_pTexture);
            pImageInfo->m_pTexture->UpdateSubImage(pImageInfo->m_x, pImageInfo->m_y, pImageInfo->m_nWidth, pImageInfo->m_nHeight, pImageInfo->m_pData);
            BEATS_SAFE_DELETE(pImageInfo);
        }
        pThis->m_fontUpdateImageCache.clear();
        pThis->m_fontUpdateImageCacheMutex.unlock();
    }
    const CFreetypeFontGlyph *pGlyph = down_cast<const CFreetypeFontGlyph *>(glyph);
    float fScale = GetScaleFactor();
    bool bBorder = borderColor.a != 0;
    CQuadPTCC quad;
    quad.tl.position.X() = x;
    quad.tl.position.Y() = y;
    quad.tr.position.X() = x + pGlyph->GetWidth(bBorder) * fFontSize;
    quad.tr.position.Y() = quad.tl.position.Y();
    quad.bl.position.X() = quad.tl.position.X();
    quad.bl.position.Y() = y + pGlyph->GetHeight(bBorder) * fFontSize;
    quad.br.position.X() = quad.tr.position.X();
    quad.br.position.Y() = quad.bl.position.Y();

    quad.tl.tex.u = pGlyph->GetU(bBorder);
    quad.tl.tex.v = pGlyph->GetV(bBorder);
    quad.tr.tex.u = pGlyph->GetU(bBorder) + pGlyph->GetWidth(bBorder) * fScale / pGlyph->texture->Width();
    quad.tr.tex.v = quad.tl.tex.v;
    quad.bl.tex.u = quad.tl.tex.u;
    quad.bl.tex.v = pGlyph->GetV(bBorder) + pGlyph->GetHeight(bBorder) * fScale / pGlyph->texture->Height();
    quad.br.tex.u = quad.tr.tex.u;
    quad.br.tex.v = quad.bl.tex.v;
    CColor relColor = color;
    relColor.a = (unsigned char)((float)color.a * fAlphaScale);
    CColor relborderColor = borderColor;
    relborderColor.a = (unsigned char)((float)borderColor.a * fAlphaScale);
    quad.tl.color = relColor;
    quad.tr.color = relColor;
    quad.bl.color = relColor;
    quad.br.color = relColor;
    quad.tl.color2 = relborderColor;
    quad.tr.color2 = relborderColor;
    quad.bl.color2 = relborderColor;
    quad.br.color2 = relborderColor;
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderText4)
    if (!pRect || !AdjustQuadCornerByRectClip(quad, pRect))
    {
        BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderText5)
            pBatch->AddQuad(&quad, transform);
        BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderText5)
    }
}

CRenderBatch* CFreetypeFontFace::GetRenderBatch(ERenderGroupID renderGroupId) const
{
    CRenderGroup* pRenderGroup = CRenderGroupManager::GetInstance()->GetRenderGroupByID(renderGroupId);
    SharePtr<CMaterial> pMaterial = CRenderManager::GetInstance()->GetFreeTypeFontMaterial();
    std::map<unsigned char, SharePtr<CTexture> > textureMap;
    textureMap[0] = m_pTexture;
    CRenderBatch* pRet = pRenderGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPTCC), pMaterial, GL_TRIANGLES, true, true, &textureMap);
    BEATS_ASSERT(pRet);
    return pRet;
}

void CFreetypeFontFace::Clear()
{
    ResetGlyphs();
    m_pTexture->Unload();
    m_pTexture->Uninitialize();
    m_pTexture = nullptr;
    NewPage();
}

unsigned char* CFreetypeFontFace::RenderFontDataToBmp(int32_t nWidth, int32_t nHeight, int32_t startPosX, int32_t startPosY, FT_Outline* pOutLine)
{
    FT_Library ftLib = CFont::GetLibrary();
    int32_t nDataSize = nWidth * nHeight;
    unsigned char* pData = new unsigned char[nDataSize];
    memset(pData, 0, nDataSize);
    FT_Bitmap bmp;
    bmp.buffer = pData;
    bmp.width = nWidth;
    bmp.rows = nHeight;
    bmp.pitch = nWidth;
    bmp.pixel_mode = FT_PIXEL_MODE_GRAY;
    bmp.num_grays = 256;

    FT_Raster_Params params;
    memset(&params, 0, sizeof (params));
    params.source = pOutLine;
    params.target = &bmp;
    params.flags = FT_RASTER_FLAG_AA;
    FT_Outline_Translate(pOutLine, startPosX, startPosY);
    FT_Outline_Render(ftLib, pOutLine, &params);

    return pData;
}

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32) && defined(DEVELOP_VERSION)
void CFreetypeFontFace::SaveToTextureFile(const TCHAR* pszFilePath)
{
    CRenderManager* pRenderMgr = CRenderManager::GetInstance();
    BEATS_ASSERT(m_pTexture.Get() != nullptr);
    pRenderMgr->SaveTextureToFile(m_pTexture->ID(), pszFilePath, true, GL_LUMINANCE_ALPHA);
}
#endif

float CFreetypeFontFace::GetScaleFactor() const
{
    return m_pRelatedRenderTarget ? 
        m_pRelatedRenderTarget->GetScaleFactor() : 
        CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetScaleFactor();
}

void CFreetypeFontFace::SetRelatedRenderTarget(CRenderTarget *pRenderTarget)
{
    m_pRelatedRenderTarget = pRenderTarget;
}

CVec2 CFreetypeFontFace::GetDistanceBetweenPointAndRect( const CVec3& position, const CRect* pRect) const
{
    BEATS_ASSERT( pRect );
    CVec2 positionOffset;
    positionOffset.X() = GetDistanceFromRange( position.X(), pRect->position.X(), pRect->position.X() + pRect->size.X() );
    positionOffset.Y() = GetDistanceFromRange( position.Y(), pRect->position.Y(), pRect->position.Y() + pRect->size.Y() );
    return positionOffset;
}

float CFreetypeFontFace::GetDistanceFromRange( float fCheckValue, float fRangeStart, float fRangeEnd) const
{
    float fRet = 0.0f;
    BEATS_ASSERT(fRangeStart < fRangeEnd);
    if ( fCheckValue < fRangeStart )
    {
        fRet = fCheckValue - fRangeStart;
    }
    else if( fCheckValue > fRangeEnd )
    {
        fRet = fCheckValue - fRangeEnd;
    }
    return fRet;
}

void CFreetypeFontFace::ResetGlyphs()
{
    m_uCurrX = 0;
    m_uCurrY = 0;
    m_nLineHeight = 0;
    m_nAscender = 0;
    m_glyphMapLocker.lock();
    for (auto glyph : m_glyphMap)
    {
        BEATS_SAFE_DELETE(glyph.second);
    }
    m_glyphMap.clear();
    m_glyphMapLocker.unlock();

    for (size_t i = 0; i < m_fontUpdateImageCache.size(); ++i)
    {
        BEATS_SAFE_DELETE(m_fontUpdateImageCache[i]);
    }
    m_fontUpdateImageCache.clear();
    static const int32_t nDataSize = PAGE_WIDTH * PAGE_HEIGHT * 2;
    char *pZeroBuf = new char[nDataSize];
    memset(pZeroBuf, 0, nDataSize);
    m_pTexture->InitWithData(pZeroBuf, nDataSize, PixelFormat::AI88, PAGE_WIDTH, PAGE_HEIGHT);
    BEATS_SAFE_DELETE_ARRAY(pZeroBuf);
}

void CFreetypeFontFace::UpdateVertice( CVec3& positon, CTex& tex, float quadWidth, float quadHeight, float texWidth, float texHeight, const CVec2& offset ) const
{
    positon.X() -= offset.X();
    tex.u -= offset.X() / quadWidth * texWidth;

    positon.Y() -= offset.Y();
    tex.v -= offset.Y() / quadHeight * texHeight;
}
