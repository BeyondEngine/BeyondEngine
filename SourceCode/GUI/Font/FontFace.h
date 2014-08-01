#ifndef BEYOND_ENGINE_GUI_FONT_FONTFACE_H__INCLUDE
#define BEYOND_ENGINE_GUI_FONT_FONTFACE_H__INCLUDE

class CTexture;
class CMaterial;

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

class CFontFace
{
public:
    CFontFace(const TString &name);
    virtual ~CFontFace();

    void PrepareCharacters(const TString &chars);
    void PrepareCharacters(const wchar_t *wchars);
    virtual const CFontGlyph *PrepareChar(wchar_t character) = 0;
    void RenderText(const TString &text, kmScalar x, kmScalar y, float fFontSize = 1.0f,
        CColor color = 0x000000FF,  CColor borderColor = 0, const kmMat4 *transform = nullptr,
        bool bGUI = true, const CRect *pScissorRect = nullptr);
    std::vector<const CFontGlyph *> GetGlyphs(const TString &text);
    const CFontGlyph *GetGlyph(unsigned long character) const;
    const TString& GetName()const;
    virtual void Clear();
    void SetBorderWeight(float fBorderWeight);
    float GetBorderWeight() const;
    virtual void SetFontSize(int /*nFontSize*/){}; // TODO: HARD HACK. Free type will implement this.
#ifdef EDITOR_MODE
    virtual void SaveToTextureFile(const TCHAR* /*pszFilePath*/) {};
#endif
private:
    virtual void DrawGlyph(const CFontGlyph *glyph, float x, float y, float fFontSize, CColor color, CColor borderColor = 0x000000FF,
        const kmMat4 *transform = nullptr, bool bGUI = true, const CRect *pRect = nullptr) const = 0;

protected:
    TString m_strName;
    std::map<unsigned long, CFontGlyph *> m_glyphMap;
    std::map<GLuint, SharePtr<CMaterial>> m_materialMap;
    float m_fBorderWeight;
};

#endif