#include "stdafx.h"
#include "FontFace.h"
#include "Render/Texture.h"
#include "Render/RenderGroup.h"
#include "Render/Material.h"
#include "Resource/ResourceManager.h"
#include "FontManager.h"
#include "Render/RenderBatch.h"
#include "Render/RenderManager.h"
#include "Utility/Utf8String.h"

CFontFace::CFontFace(const TString &name)
    : m_strName(name)
    , m_fBorderWeight(1.0f)
{
}

CFontFace::~CFontFace()
{
    for(auto glyph : m_glyphMap)
    {
        BEATS_SAFE_DELETE(glyph.second);
    }
}

void CFontFace::PrepareCharacters(const TString &chars)
{
#ifdef UNICODE
    PrepareCharacters(chars.c_str());
#else
    PrepareCharacters(Utf8ToWString(chars.c_str()).c_str());
#endif
}

void CFontFace::PrepareCharacters(const wchar_t *wchars)
{
    for(size_t i = 0; wchars[i]; ++i)
    {
        PrepareChar(wchars[i]);
    }
}

void CFontFace::RenderText(const TString &text, kmScalar x, kmScalar y, float fFontSize,
                          CColor color, CColor borderColor, const kmMat4 *transform,
                          bool bGUI, const CRect *pRect)
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderText2)
    auto glyphs = GetGlyphs(text);
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderText2)
    BEYONDENGINE_PERFORMDETECT_START(ePNT_RenderText3)
    for(auto glyph : glyphs)
    {
        DrawGlyph(glyph, x, y, fFontSize, color, borderColor, transform, bGUI, pRect );
        x += glyph->GetWidth(borderColor.a != 0) * fFontSize;
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_RenderText3)
}

const CFontGlyph *CFontFace::GetGlyph( unsigned long character ) const
{
    auto itr = m_glyphMap.find(character);
    return itr != m_glyphMap.end() ? itr->second : nullptr;
}

std::vector<const CFontGlyph *> CFontFace::GetGlyphs(const TString &text)
{
#ifdef UNICODE
    const wchar_t *wchars = text.c_str();
#else
    std::wstring wstr = Utf8ToWString(text.c_str());
    const wchar_t *wchars = wstr.c_str();
#endif

    PrepareCharacters(wchars);
    std::vector<const CFontGlyph *> glyphs;
    for(size_t i = 0; wchars[i]; ++i)
    {
        const CFontGlyph *glyph = GetGlyph(wchars[i]);
        if(glyph)
            glyphs.push_back(glyph);
    }
    return glyphs;
}

const TString& CFontFace::GetName()const
{
    return m_strName;
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