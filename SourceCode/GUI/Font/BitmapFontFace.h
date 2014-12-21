#ifndef BEYOND_ENGINE_GUI_FONT_BITMAPFONTFACE_H__INCLUDE
#define BEYOND_ENGINE_GUI_FONT_BITMAPFONTFACE_H__INCLUDE

#include "FontFace.h"

class CTextureFrag;

struct CBitmapFontGlyph : public CFontGlyph
{
    virtual ~CBitmapFontGlyph(){}
    SharePtr<CTextureFrag> pFrag;
};

class CBitmapFontFace : public CFontFace
{
public:
    CBitmapFontFace(const TString &filename);
    virtual ~CBitmapFontFace();

    virtual const CFontGlyph *PrepareChar(wchar_t character, bool& bGlyphReset) override;
    void Reload();
private:
    virtual void DrawGlyph(CRenderBatch* pBatch, const CFontGlyph *glyph, float x, float y, float fFontSize, const CColor& color, const CColor& borderColor = 0x000000FF,
        const CMat4 *transform = nullptr, const CRect *pRect = nullptr, float fAlphaScale = 1.0f) const override;
    virtual CRenderBatch* GetRenderBatch(ERenderGroupID renderGroupId) const override;
    void LoadFontFile(const TString &filename);
    void CleanFontData();
private:
    float m_fDefaultWidth;
    float m_fDefaultHeight;
    TString m_strFontName;
};

#endif