#ifndef BEYOND_ENGINE_GUI_FONT_BITMAPFONTFACE_H__INCLUDE
#define BEYOND_ENGINE_GUI_FONT_BITMAPFONTFACE_H__INCLUDE

#include "FontFace.h"

class CTextureFrag;

struct CBitmapFontGlyph : public CFontGlyph
{
    virtual ~CBitmapFontGlyph(){}
    CTextureFrag *pFrag;
};

class CBitmapFontFace : public CFontFace
{
public:
    CBitmapFontFace(const TString &filename);
    virtual ~CBitmapFontFace();

    virtual const CFontGlyph *PrepareChar(wchar_t character) override;

private:
    virtual void DrawGlyph(const CFontGlyph *glyph, float x, float y, float fFontSize, CColor color, CColor borderColor,
        const kmMat4 *transform, bool bGUI, const CRect *pRect ) const override;

    void LoadFontFile(const TString &filename);

private:
    float m_fDefaultWidth;
    float m_fDefaultHeight;
};

#endif