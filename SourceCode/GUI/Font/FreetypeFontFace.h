#ifndef BEYOND_ENGINE_GUI_FONT_FREETYPEFONTFACE_H__INCLUDE
#define BEYOND_ENGINE_GUI_FONT_FREETYPEFONTFACE_H__INCLUDE

#include "FontFace.h"

class CRenderTarget;
typedef struct FT_Outline_ FT_Outline;
typedef struct FT_FaceRec_*  FT_Face;

class CFreetypeFontGlyph : public CFontGlyph
{
public:
    CFreetypeFontGlyph()
        : u (0)
        , v (0)
        , u_border(0)
        , v_border(0)
    {

    }
    virtual ~CFreetypeFontGlyph(){}

    float GetU(bool bBorder) const
    {
        return bBorder ? u_border : u;
    }
    float GetV(bool bBorder) const
    {
        return bBorder ? v_border : v;
    }
    void SetU(bool bBorder, float fU)
    {
        if (bBorder)
        {
            u_border = fU;
        }
        else
        {
            u = fU;
        }
    }
    void SetV(bool bBorder, float fV)
    {
        if (bBorder)
        {
            v_border = fV;
        }
        else
        {
            v = fV;
        }
    }
    SharePtr<CTexture> texture;
private:
    float u, v, u_border, v_border;
};

class CFreetypeFontFace : public CFontFace
{
public:
    CFreetypeFontFace(const TString &name, const TString &file, int size, int dpi = -1);
    virtual ~CFreetypeFontFace();

    virtual const CFontGlyph *PrepareChar(wchar_t character) override;

    virtual void Clear() override;
#ifdef EDITOR_MODE
    virtual void SaveToTextureFile(const TCHAR* pszFilePath) override;
#endif
    void SetRelatedRenderTarget(CRenderTarget *pRenderTarget);

private:
    virtual void DrawGlyph(const CFontGlyph *glyph, float x, float y, float fFontSize,
        CColor color, CColor borderColor, const kmMat4 *transform, bool bGUI, const CRect *pRect) const override;

    float GetScaleFactor() const;

    void NewPage();

    void ApplyFTSize();

    FT_Face GetFontFace() const;
    virtual void SetFontSize(int nFontSize);

private:
    unsigned char* RenderFontDataToBmp(int nWidth, int nHeight, int startPosX, int startPosY, FT_Outline* pOutLine);

private:
    SharePtr<CFont> m_pFont;
    std::vector<SharePtr<CTexture>> m_textures;
    int m_nCurrPage;
    size_t m_uCurrX;
    size_t m_uCurrY;
    int m_nSize;
    int m_nDpi;
    int m_nLineHeight;    //maxheight of current font face
    int m_nAscender;  //distance from top to baseline

    CRenderTarget *m_pRelatedRenderTarget;

    static const int PAGE_WIDTH = 1024;
    static const int PAGE_HEIGHT = 1024;
};

#endif