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
#include "Render/RenderWindow.h"
#include "Render/RenderState.h"
#include "Render/Renderer.h"

#include FT_FREETYPE_H
#include FT_STROKER_H

#define FT_SHIFT_NUM 6

//next multiple of 8
inline int nextMOE(int x)
{
    return (x + 7) & 0xFFFFFFF8;
}

CFreetypeFontFace::CFreetypeFontFace(const TString &name, const TString &file, int size, int dpi/*= -1*/)
    : CFontFace(name)
    , m_pFont(CResourceManager::GetInstance()->GetResource<CFont>(file))
    , m_nCurrPage(-1)
    , m_uCurrX(0)
    , m_uCurrY(0)
    , m_nSize(size)
    , m_nDpi(dpi)
    , m_nLineHeight(0)
    , m_nAscender(0)
    , m_pRelatedRenderTarget(nullptr)
{
    NewPage();
    ApplyFTSize();
}

CFreetypeFontFace::~CFreetypeFontFace()
{
    for(auto texture : m_textures)
    {
        texture->Unload();
    }
}

void CFreetypeFontFace::NewPage()
{
    int nDataSize = PAGE_WIDTH * PAGE_HEIGHT * 2;
    char *pZeroBuf = new char[nDataSize];
    memset(pZeroBuf, 0, nDataSize);
    SharePtr<CTexture> texture = new CTexture;
    texture->InitWithData(pZeroBuf, nDataSize, PixelFormat::AI88, PAGE_WIDTH, PAGE_HEIGHT);
    BEATS_SAFE_DELETE_ARRAY(pZeroBuf);
    texture->SetInitializeFlag(true);
    texture->SetLoadFlag(true);

    ++m_nCurrPage;
    m_textures.push_back(texture);

    SharePtr<CMaterial> material = CFontManager::GetInstance()->CreateDefaultMaterial(false);
    material->SetTexture(0, texture);
    m_materialMap[texture->ID()] = material;
}

void CFreetypeFontFace::ApplyFTSize()
{
    const float fScale = GetScaleFactor();
    FT_Error err = 0;
    if (m_nDpi == -1)
    {
        err = FT_Set_Pixel_Sizes(m_pFont->GetFontFace(), (int)(m_nSize * fScale), (int)(m_nSize * fScale));
        BEATS_ASSERT(!err);
        CFontFace* pDefaultFontFace = CFontManager::GetInstance()->GetDefaultFontFace();
        BEATS_ASSERT(pDefaultFontFace == NULL || dynamic_cast<CFreetypeFontFace*>(pDefaultFontFace) != NULL);
        if (pDefaultFontFace != NULL)
        {
            FT_Face pFTFace = static_cast<CFreetypeFontFace*>(pDefaultFontFace)->GetFontFace();
            err = FT_Set_Pixel_Sizes(pFTFace, (int)(m_nSize * fScale), (int)(m_nSize * fScale));
            BEATS_ASSERT(!err);
        }
    }
    else
    {
        FT_F26Dot6 sizeTrans = (int)(m_nSize * fScale) << FT_SHIFT_NUM;
        err = FT_Set_Char_Size(m_pFont->GetFontFace(), sizeTrans, sizeTrans, m_nDpi, m_nDpi);
        BEATS_ASSERT(!err);
        CFontFace* pDefaultFontFace = CFontManager::GetInstance()->GetDefaultFontFace();
        BEATS_ASSERT(pDefaultFontFace == NULL || dynamic_cast<CFreetypeFontFace*>(pDefaultFontFace) != NULL);
        if (pDefaultFontFace != NULL)
        {
            FT_Face pFTFace = static_cast<CFreetypeFontFace*>(pDefaultFontFace)->GetFontFace();
            err = FT_Set_Char_Size(pFTFace, sizeTrans, sizeTrans, m_nDpi, m_nDpi);
            BEATS_ASSERT(!err);
        }
    }

    const FT_Size& ftSize = m_pFont->GetFontFace()->size;
    m_nAscender = ftSize->metrics.ascender >> FT_SHIFT_NUM;
    long descender = ftSize->metrics.descender >> FT_SHIFT_NUM;
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

void CFreetypeFontFace::SetFontSize(int nFontSize)
{
    m_nSize = nFontSize;
}

const CFontGlyph *CFreetypeFontFace::PrepareChar(wchar_t character)
{
    float outlineWidth = GetBorderWeight() * GetScaleFactor();
    auto itr = m_glyphMap.find(character);
    CFreetypeFontGlyph *glyph = itr != m_glyphMap.end() ? down_cast<CFreetypeFontGlyph *>(itr->second) : nullptr;

    if(!glyph)
    {
        ApplyFTSize();
        FT_Face pFontFace = m_pFont->GetFontFace();
        BEATS_ASSERT(pFontFace != NULL);
        bool bFindCharacterGlyph = FT_Get_Char_Index(pFontFace, character) != 0;
        if (!bFindCharacterGlyph) // Undefined character, we try to use default font.
        {
            CFontFace* pDefaultFontFace = CFontManager::GetInstance()->GetDefaultFontFace();
            BEATS_ASSERT(dynamic_cast<CFreetypeFontFace*>(pDefaultFontFace) != NULL);
            if (pDefaultFontFace != NULL)
            {
                FT_Face pFTFace = static_cast<CFreetypeFontFace*>(pDefaultFontFace)->GetFontFace();
                bFindCharacterGlyph = FT_Get_Char_Index(pFTFace, character) != 0;
                if (bFindCharacterGlyph)
                {
                    pFontFace = pFTFace;
                }
            }
        }
        BEATS_ASSERT(bFindCharacterGlyph, _T("Character %d can't be found in all font face!"), character);
        FT_Error err = FT_Load_Char(pFontFace, character, FT_LOAD_NO_BITMAP);
        BEYONDENGINE_UNUSED_PARAM(err);
        BEATS_ASSERT(!err);

        FT_GlyphSlot pGlyph = pFontFace->glyph;
        BEATS_ASSERT(pGlyph != NULL);
        long nBorderAdvanceX = pGlyph->metrics.horiAdvance >> FT_SHIFT_NUM;
        long nFontAdvanceX = nBorderAdvanceX;
        int nFontHeight = 0;
        int nBorderOriginWidth = 0;
        int nFontOriginWidth = 0;
        int nBorderHeight = 0;
        FT_BBox borderBox;
        FT_BBox fontBox;
        long x = 0, y = 0;
        if (pGlyph->format == FT_GLYPH_FORMAT_OUTLINE)
        {
            FT_Library ftLib = CFont::GetLibrary();
            // Set up a stroker.
            FT_Stroker stroker;
            FT_Stroker_New(ftLib, &stroker);
            FT_Stroker_Set(stroker, (int)(outlineWidth * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
            FT_Glyph pOutlineGlyph, pInnerGlyph;
            if (FT_Get_Glyph(pGlyph, &pOutlineGlyph) == 0 && FT_Get_Glyph(pGlyph, &pInnerGlyph) == 0)
            {
                FT_Glyph_StrokeBorder(&pOutlineGlyph, stroker, 0, 1);
                BEATS_ASSERT(pOutlineGlyph->format == FT_GLYPH_FORMAT_OUTLINE && pInnerGlyph->format == FT_GLYPH_FORMAT_OUTLINE);
                FT_Outline *pOutLine = &reinterpret_cast<FT_OutlineGlyph>(pOutlineGlyph)->outline;
                FT_Glyph_Get_CBox(pOutlineGlyph, FT_GLYPH_BBOX_GRIDFIT, &borderBox);
                FT_Glyph_Get_CBox(pInnerGlyph, FT_GLYPH_BBOX_GRIDFIT, &fontBox);

                nBorderOriginWidth = (borderBox.xMax - borderBox.xMin) >> FT_SHIFT_NUM;

                nFontOriginWidth = (fontBox.xMax - fontBox.xMin) >> FT_SHIFT_NUM;
                int nBorderWidth = nextMOE(nBorderOriginWidth); // Because our GL_UNPACK_ALIGNMENT should be 8 here.
#ifdef _DEBUG
                GLint nUnpackAlignment = 0;
                CRenderer::GetInstance()->GetIntegerV(GL_UNPACK_ALIGNMENT, &nUnpackAlignment);
                BEATS_ASSERT(nUnpackAlignment == 8);
#endif
                nBorderHeight = (borderBox.yMax - borderBox.yMin) >> FT_SHIFT_NUM;
                nFontHeight = (fontBox.yMax - fontBox.yMin) >> FT_SHIFT_NUM;
                x = pGlyph->metrics.horiBearingX >> FT_SHIFT_NUM;
                y = pGlyph->metrics.horiBearingY >> FT_SHIFT_NUM;
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
                    m_uCurrY += m_nLineHeight;
                    if(m_uCurrY + m_nLineHeight > PAGE_HEIGHT)
                    {
                        m_uCurrY = 0;
                        NewPage();
                    }
                }

                int nDataSize = nBorderWidth * nBorderHeight;
                float fBorderOffsetY = 1.0f; // Makes it look like a shadow.
                unsigned char* pBorderData = RenderFontDataToBmp(nBorderWidth, nBorderHeight, -borderBox.xMin, (int)(-borderBox.yMin * fBorderOffsetY), pOutLine);

                FT_Outline *pInnerOutLine = &reinterpret_cast<FT_OutlineGlyph>(pInnerGlyph)->outline;
                unsigned char* pFontData = RenderFontDataToBmp(nBorderWidth, nBorderHeight, -borderBox.xMin, -borderBox.yMin, pInnerOutLine);

                unsigned char* pAllData = new unsigned char[nDataSize * 2];
                for (int i = 0; i < nDataSize; ++i)
                {
                    pAllData[i * 2] = pBorderData[i];
                    pAllData[i * 2 + 1] = pFontData[i];
                }

                SharePtr<CTexture> texture = m_textures[m_nCurrPage];
                texture->UpdateSubImage(MAX((int)m_uCurrX + x, 0), MAX((int)m_uCurrY + (m_nAscender - y), 0), nBorderWidth, nBorderHeight, pAllData);

                // Clean up afterwards.
                FT_Stroker_Done(stroker);
                FT_Done_Glyph(pOutlineGlyph);
                BEATS_SAFE_DELETE_ARRAY(pBorderData);
                BEATS_SAFE_DELETE_ARRAY(pFontData);
                BEATS_SAFE_DELETE_ARRAY(pAllData);
            }
        }
        glyph = new CFreetypeFontGlyph;
        const float fScale = GetScaleFactor();
        glyph->SetWidth((float)nBorderAdvanceX / fScale, true);
        glyph->SetWidth((float)nFontAdvanceX / fScale, false);
        glyph->SetHeight((float)m_nLineHeight / fScale, true);
        glyph->SetHeight((float)m_nLineHeight / fScale, false);
        glyph->SetU(true, (float)m_uCurrX / PAGE_WIDTH);
        glyph->SetV(true, (float)m_uCurrY / PAGE_HEIGHT);
        BEATS_ASSERT(nFontOriginWidth <= nBorderOriginWidth);
        glyph->SetU(false, (float)(m_uCurrX + (nBorderOriginWidth - nFontOriginWidth) * 0.5f) / PAGE_WIDTH);
        glyph->SetV(false, (float)(m_uCurrY + (nBorderHeight - nFontHeight) * 0.5f) / PAGE_HEIGHT);
        glyph->texture = m_textures[m_nCurrPage];
        m_glyphMap[character] = glyph;
        BEATS_ASSERT(nBorderAdvanceX > 0);
        m_uCurrX += nBorderAdvanceX;
    }
    return glyph;
}

void CFreetypeFontFace::DrawGlyph(const CFontGlyph *glyph, kmScalar x, kmScalar y, float fFontSize, CColor color, CColor borderColor,
                                  const kmMat4 *transform, bool bGUI, const CRect *pRect) const
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderText4)
    const CFreetypeFontGlyph *pGlyph = down_cast<const CFreetypeFontGlyph *>(glyph);
    float fScale = GetScaleFactor();
    bool bBorder = borderColor.a != 0;
    CQuadPTCC quad;
    quad.tl.position.x = x;
    quad.tl.position.y = y;
    quad.tr.position.x = x + pGlyph->GetWidth(bBorder) * fFontSize;
    quad.tr.position.y = quad.tl.position.y;
    quad.bl.position.x = quad.tl.position.x;
    quad.bl.position.y = y + pGlyph->GetHeight(bBorder) * fFontSize;
    quad.br.position.x = quad.tr.position.x;
    quad.br.position.y = quad.bl.position.y;
    quad.tl.tex.u = pGlyph->GetU(bBorder);
    quad.tl.tex.v = pGlyph->GetV(bBorder);
    quad.tr.tex.u = pGlyph->GetU(bBorder) + pGlyph->GetWidth(bBorder) * fScale / pGlyph->texture->Width();
    quad.tr.tex.v = quad.tl.tex.v;
    quad.bl.tex.u = quad.tl.tex.u;
    quad.bl.tex.v = pGlyph->GetV(bBorder) + pGlyph->GetHeight(bBorder) * fScale / pGlyph->texture->Height();
    quad.br.tex.u = quad.tr.tex.u;
    quad.br.tex.v = quad.bl.tex.v;
    quad.tl.color = color;
    quad.tr.color = color;
    quad.bl.color = color;
    quad.br.color = color;
    quad.tl.color2 = borderColor;
    quad.tr.color2 = borderColor;
    quad.bl.color2 = borderColor;
    quad.br.color2 = borderColor;
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderText4)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderText5)
    SharePtr<CMaterial> pMaterial;
    if(!pRect)
    {
        auto itr = m_materialMap.find(pGlyph->texture->ID());
        BEATS_ASSERT(itr != m_materialMap.end());
        pMaterial = itr->second;
    }
    else
    {
        pMaterial = CFontManager::GetInstance()->CreateDefaultMaterial(false);
        pMaterial->SetTexture(0, pGlyph->texture);
        pMaterial->GetRenderState()->SetBoolState(CBoolRenderStateParam::eBSP_ScissorTest, true);
        pMaterial->GetRenderState()->SetScissorRect(pRect->position.x, pRect->position.y, pRect->size.x, pRect->size.y);
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderText5)

        BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderText6)
        CRenderGroup* pOperateGroup = bGUI ? CFontManager::GetInstance()->GetRenderGroupGUI() : CFontManager::GetInstance()->GetRenderGroup2D();
    CRenderBatch *batch = pOperateGroup->GetRenderBatch(VERTEX_FORMAT(CVertexPTCC), pMaterial, GL_TRIANGLES, true );
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderText6)
        BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderText7)
        batch->AddQuad(&quad, transform);
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderText7)
}

void CFreetypeFontFace::Clear()
{
    m_nCurrPage = -1;
    m_uCurrX = 0;
    m_uCurrY = 0;
    m_nLineHeight = 0;
    m_nAscender = 0;

    for(auto glyph : m_glyphMap)
    {
        BEATS_SAFE_DELETE(glyph.second);
    }
    m_glyphMap.clear();
    for(auto texture : m_textures)
    {
        texture->Unload();
    }
    m_textures.clear();
    m_materialMap.clear();

    NewPage();
}

unsigned char* CFreetypeFontFace::RenderFontDataToBmp(int nWidth, int nHeight, int startPosX, int startPosY, FT_Outline* pOutLine)
{
    FT_Library ftLib = CFont::GetLibrary();
    int nDataSize = nWidth * nHeight;
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

#ifdef EDITOR_MODE
void CFreetypeFontFace::SaveToTextureFile(const TCHAR* pszFilePath)
{
    CRenderManager* pRenderMgr = CRenderManager::GetInstance();
    for (size_t i = 0; i < m_textures.size(); ++i)
    {
        TCHAR szBuffer[MAX_PATH];
        _stprintf(szBuffer, _T("%s_%s_%d"), pszFilePath, GetName().c_str(), i);
        pRenderMgr->SaveTextureToFile(m_textures[i]->ID(), szBuffer, true, GL_LUMINANCE_ALPHA);
    }
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
