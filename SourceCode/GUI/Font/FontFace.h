#ifndef BEYOND_ENGINE_GUI_FONT_FONTFACE_H__INCLUDE
#define BEYOND_ENGINE_GUI_FONT_FONTFACE_H__INCLUDE

#include "Render/RenderGroupManager.h"

class CTexture;
class CRenderBatch;
class CFont;

class CFontGlyph
{
public:
    CFontGlyph()
        : m_fWidth(0)
        , m_fHeight(0)
        , m_fWidth_border(0)
        , m_fHeight_border(0)
    {

    }
    virtual ~CFontGlyph(){}
    void SetWidth(float fWidth, bool bIncludeBorder)
    {
        if (bIncludeBorder)
        {
            m_fWidth_border = fWidth;
        }
        else
        {
            m_fWidth = fWidth;
        }
    }
    float GetWidth(bool bIncludeBorder = true) const
    {
        return bIncludeBorder ? m_fWidth_border : m_fWidth;
    }
    void SetHeight(float fHeight, bool bIncludeBorder)
    {
        if (bIncludeBorder)
        {
            m_fHeight_border = fHeight;
        }
        else
        {
            m_fHeight = fHeight;
        }
    }
    float GetHeight(bool bIncludeBorder = true) const
    {
        return bIncludeBorder ? m_fHeight_border : m_fHeight;
    }
private:
    float m_fWidth;
    float m_fHeight;
    float m_fWidth_border;
    float m_fHeight_border;
};

enum EFontType
{
    e_free_type_font = 0,
    e_bitmap_font
};

class CFontFace
{
public:
    CFontFace(const TString &name);
    virtual ~CFontFace();

    void PrepareCharacters(const TString &chars, std::vector<const CFontGlyph *>& glyphs);
    void PrepareCharacters(const wchar_t *wchars, std::vector<const CFontGlyph *>& glyphs);
    virtual const CFontGlyph *PrepareChar(wchar_t character, bool& glyphRestFlag) = 0;
    float RenderText(const TString &text, float x, float y, ERenderGroupID renderGroupId, float fFontSize = 1.0f,
        const CColor& color = 0x000000FF, const CColor&  borderColor = 0, const CMat4 *transform = nullptr,
        const CRect *pScissorRect = nullptr, float fAlphaScale = 1.0f);

    void GetGlyphs(const TString &text, std::vector<const CFontGlyph *>& glyphs);
    const TString& GetName()const;
    virtual void Clear();
    void SetBorderWeight(float fBorderWeight);
    float GetBorderWeight() const;
    virtual void SetFontSize(int /*nFontSize*/){}; // TODO: HARD HACK. Free type will implement this.
    virtual int GetFontSize();
    void SetFontType(EFontType type);
    EFontType GetFontType();
    CVec2 GetTextSize(const TString& text, float fontSize, bool hasBorder);
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32) && defined(DEVELOP_VERSION)
    virtual void SaveToTextureFile(const TCHAR* /*pszFilePath*/) {};
#endif
    virtual float GetScaleFactor() const;
private:
    virtual void DrawGlyph(CRenderBatch* pBatch, const CFontGlyph *glyph, float x, float y, float fFontSize, const CColor& color, const CColor& borderColor = 0x000000FF,
        const CMat4 *transform = nullptr, const CRect *pRect = nullptr, float fAlphaScale = 1.0f) const = 0;
    virtual CRenderBatch* GetRenderBatch(ERenderGroupID renderGroupId) const = 0;

protected:
    TString m_strName;
    std::unordered_map<uint32_t, CFontGlyph *> m_glyphMap;//this map will find frequently, unordered map is faster than map
    float m_fBorderWeight = 3.0f;
    EFontType m_eFontType = e_free_type_font;
    int m_nSize = 52;
    SharePtr<CTexture> m_pTexture;
    std::mutex m_glyphMapLocker;
};

#endif