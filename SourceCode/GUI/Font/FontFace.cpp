#include "stdafx.h"
#include "FontFace.h"
#include "Render/Texture.h"
#include "Render/RenderGroup.h"
#include "Render/Material.h"
#include "Resource/ResourceManager.h"
#include "FontManager.h"
#include "Render/RenderBatch.h"
#include "Render/RenderManager.h"

CFontFace::CFontFace(const TString &name)
: m_strName(name)
, m_fBorderWeight(3.0f)
{
}

CFontFace::~CFontFace()
{
    m_glyphMapLocker.lock();
    BEATS_SAFE_DELETE_MAP(m_glyphMap);
    m_glyphMapLocker.unlock();
}

void CFontFace::PrepareCharacters(const TString &chars, std::vector<const CFontGlyph *>& glyphs)
{
    PrepareCharacters(CStringHelper::GetInstance()->Utf8ToWString(chars.c_str()).c_str(), glyphs);
}

void CFontFace::PrepareCharacters(const wchar_t *wchars, std::vector<const CFontGlyph *>& glyphs)
{
    bool bGlyphHasRest = false;
    for (uint32_t i = 0; wchars[i];)
    {
        bool bGlyphResetFlag = false;
        const CFontGlyph * glyph = PrepareChar(wchars[i], bGlyphResetFlag);
        if (bGlyphResetFlag)
        {
            glyphs.clear();
            if (bGlyphHasRest)
            {
                BEATS_ASSERT(false, "the text %s is too long for free type texture buffer!", wchars);
                break;
            }
            else
            {
                bGlyphHasRest = true;
                i = 0;
            }
        }
        else
        {
            BEATS_ASSERT(glyph);
            if (glyph)
            {
                glyphs.push_back(glyph);
            }
            ++i;
        }
    }
}

CVec2 CFontFace::GetTextSize(const TString& text, float fontSize, bool hasBorder)
{
    float startX = 0;
    float maxHeight = 0.0f;

    std::vector<const CFontGlyph *> glyphs;
    GetGlyphs(text, glyphs);

    for (auto glyph : glyphs)
    {
        if (glyph)
        {
            startX += glyph->GetWidth(hasBorder) * fontSize;
            maxHeight = glyph->GetHeight(hasBorder) > maxHeight ? glyph->GetHeight(hasBorder) : maxHeight;
        }
    }
    return CVec2(startX, maxHeight * fontSize);
}

float CFontFace::RenderText(const TString &text, float x, float y, ERenderGroupID renderGroupId, float fFontSize,
    const CColor& color, const CColor& borderColor, const CMat4 *transform,
    const CRect *pRect, float fAlphaScale)
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderText2)
        std::vector<const CFontGlyph *> glyphs;
        GetGlyphs(text, glyphs);
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderText2)

    BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderText3)
    CRenderBatch* pRenderBatch = GetRenderBatch(renderGroupId);
#ifdef DEVELOP_VERSION
    CEngineCenter::GetInstance()->m_uRenderTextCounter += glyphs.size();
    if (pRenderBatch->m_usage == ERenderBatchUsage::eRBU_Count && m_eFontType == e_free_type_font)
    {
        pRenderBatch->m_usage = ERenderBatchUsage::eRBU_Text;
    }
    BEATS_ASSERT(m_eFontType != e_free_type_font || pRenderBatch->m_usage == ERenderBatchUsage::eRBU_Text);
#endif
    for (auto glyph : glyphs)
    {
        DrawGlyph(pRenderBatch, glyph, x, y, fFontSize, color, borderColor, transform, pRect, fAlphaScale);
        x += glyph->GetWidth(borderColor.a != 0) * fFontSize;
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderText3)
        return x;
}

float CFontFace::GetScaleFactor() const
{
    return 1.0f;
}

void CFontFace::GetGlyphs(const TString &text, std::vector<const CFontGlyph *>& glyphs)
{
    std::wstring wstr = CStringHelper::GetInstance()->Utf8ToWString(text.c_str());
    const wchar_t *wchars = wstr.c_str();
    PrepareCharacters(wchars, glyphs);
}

const TString& CFontFace::GetName()const
{
    return m_strName;
}

void CFontFace::SetFontType(EFontType type)
{
    m_eFontType = type;
}

EFontType CFontFace::GetFontType()
{
    return m_eFontType;
}

void CFontFace::Clear()
{
}

void CFontFace::SetBorderWeight(float fBorderWeight)
{
    m_fBorderWeight = fBorderWeight;
}

float CFontFace::GetBorderWeight() const
{
    return m_fBorderWeight;
}

int CFontFace::GetFontSize()
{
    return m_nSize;
}
