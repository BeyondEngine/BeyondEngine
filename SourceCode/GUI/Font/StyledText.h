#ifndef BEYOND_ENGINE_GUI_FONT_STYLEDTEXT_H__INCLUDE
#define BEYOND_ENGINE_GUI_FONT_STYLEDTEXT_H__INCLUDE

class CFontFace;

struct SStyledTextSegment
{
    SStyledTextSegment(const TString &text, const CVec2 &pos, const CVec2 &size,
        CFontFace *pFontFace, CColor color)
        : text(text), pos(pos), size(size), pFontFace(pFontFace), color(color){}

    SStyledTextSegment( const SStyledTextSegment& other )
    {
        text = other.text;
        pos = other.pos;
        size = other.size;
        pFontFace = other.pFontFace;
        color = other.color;
    }

    TString text;
    CVec2 pos;
    CVec2 size;
    CFontFace *pFontFace;
    CColor color;
};

typedef std::vector<SStyledTextSegment> TStyledTextSegmentList;

struct SMarkInfo
{
    enum EMarkType
    {
        eMT_UNKNOWN,
        eMT_COLOR,
        eMT_FONT,
    };

    SMarkInfo()
    {
        markType = eMT_UNKNOWN;
        isOpenMark = true;
    }
    virtual ~SMarkInfo(){}

    EMarkType markType;
    bool isOpenMark;
};

struct SColorMarkInfo : public SMarkInfo
{
    SColorMarkInfo()
    {
        markType = eMT_COLOR;
    }
    CColor color;
};

struct SFontMarkInfo : public SMarkInfo
{
    SFontMarkInfo()
    {
        markType = eMT_FONT;
        pFontFace = nullptr;
    }
    CFontFace *pFontFace;
};
enum EHoriAlign
{
    eHA_LEFT,
    eHA_CENTER,
    eHA_RIGHT,
};
enum EVertAlign
{
    eVA_TOP,
    eVA_MIDDLE,
    eVA_BOTTOM,
};

class CStyledTextParser
{
public:
    CStyledTextParser(
        const TCHAR *pText, const CVec2 &startPos, const CVec2 &endPos,
        CFontFace *pDefaultFontFace, CColor defaultColor, CColor borderColor,
        EHoriAlign hAlign = eHA_LEFT, EVertAlign vAlign = eVA_MIDDLE, float fFontSize = 1.0f);

    TStyledTextSegmentList Parse();

    static const SMarkInfo *ParseMark(const TString &strMark);

private:
    void Clear();
    bool CollectCharacter();
    void AppendCharacter(TCHAR c);
    void AppendCharacter(const TCHAR *pChars, size_t num);
    void BuildSegment();
    bool NewLine();

    void HandleMark(const SMarkInfo *pMarkInfo);
    void HandleEscaping();
    void HandleHoriAlign();
    void HandleVertAlign();

    std::vector<CFontFace *> m_fontStack;
    std::vector<CColor> m_colorStack;
#ifdef _DEBUG
    std::vector<SMarkInfo::EMarkType> m_markTypeStack;
#endif

    TStyledTextSegmentList *m_pTextSegList;

    bool m_bInMark;
    TString m_mark;

    static const size_t BUF_LEN = 1024;
    TCHAR m_bufLiteral[BUF_LEN];
    size_t m_bufIndex;

    CVec2 m_currPos;
    CVec2 m_currStartPos;
    CVec2 m_startPos;
    CVec2 m_endPos;
    CColor m_borderColor;
    float m_fFontSize;
    float m_lineHeight;
    EHoriAlign m_hAlign;
    EVertAlign m_vAlign;
    size_t m_currLineStartIndex;
    float m_currLineWidth;

    const TCHAR *m_pText;
};

#endif