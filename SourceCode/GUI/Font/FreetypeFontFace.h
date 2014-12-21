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

struct SFontUpdateImageInfo
{
    SFontUpdateImageInfo()
    {
    }
    ~SFontUpdateImageInfo()
    {
        BEATS_SAFE_DELETE_ARRAY(m_pData);
    }
    SharePtr<CTexture> m_pTexture;
    GLint m_x = 0;
    GLint m_y = 0;
    int32_t m_nWidth = 0;
    int32_t m_nHeight = 0;
    void* m_pData = 0;
};

class CFreetypeFontFace : public CFontFace
{
public:
    CFreetypeFontFace(const TString &name, const TString &file, int32_t size, int32_t dpi = -1);
    virtual ~CFreetypeFontFace();

    virtual const CFontGlyph *PrepareChar(wchar_t character, bool& bGlyphRestFlag) override;

    virtual void Clear() override;
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32) && defined(DEVELOP_VERSION)
    virtual void SaveToTextureFile(const TCHAR* pszFilePath) override;
#endif
    void SetRelatedRenderTarget(CRenderTarget *pRenderTarget);
    virtual float GetScaleFactor() const override;

private:
    virtual void DrawGlyph(CRenderBatch* pBatch, const CFontGlyph *glyph, float x, float y, float fFontSize, const CColor& color, const CColor& borderColor = 0x000000FF,
        const CMat4 *transform = nullptr, const CRect *pRect = nullptr, float fAlphaScale = 1.0f) const override;
    virtual CRenderBatch* GetRenderBatch(ERenderGroupID renderGroupId) const override;
    void NewPage();

    void ApplyFTSize();

    FT_Face GetFontFace() const;
    virtual void SetFontSize(int32_t nFontSize) override;

private:
    unsigned char* RenderFontDataToBmp(int32_t nWidth, int32_t nHeight, int32_t startPosX, int32_t startPosY, FT_Outline* pOutLine);
    bool AdjustQuadCornerByRectClip(CQuadPTCC& quad, const CRect *pRect) const;
    void ClicPointToRect(CVertexPTCC& vertex, const CRect* pRect, float fQuadWidth, float fQuadHeight, float fTexWidth, float fTextHeight ) const;
    // If the position is not in the rect, we return the distance between it, otherwise we return 0,0.
    CVec2 GetDistanceBetweenPointAndRect( const CVec3& position, const CRect* pRect) const;
    void UpdateVertice( CVec3& positon, CTex& tex, float quadWidth, float quadHeight, float texWidth, float texHeight, const CVec2& offset ) const;
    float GetDistanceFromRange( float checkValue, float start, float end ) const;
    void ResetGlyphs();
private:
    SharePtr<CFont> m_pFont;
    uint32_t m_uCurrX;
    uint32_t m_uCurrY;
    int32_t m_nDpi;
    int32_t m_nLineHeight;    //maxheight of current font face
    int32_t m_nAscender;  //distance from top to baseline
    static const int32_t m_nBorderSpace = 2;
    CRenderTarget *m_pRelatedRenderTarget;
    std::mutex m_fontUpdateImageCacheMutex;
    std::vector<SFontUpdateImageInfo*> m_fontUpdateImageCache;
    static const int32_t PAGE_WIDTH = 2048;
    static const int32_t PAGE_HEIGHT = 2048;
};
#endif