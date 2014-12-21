#ifndef BEYOND_ENGINE_GUI_FONT_STYLEDTEXT_H__INCLUDE
#define BEYOND_ENGINE_GUI_FONT_STYLEDTEXT_H__INCLUDE

class CFontFace;

struct SStyledTextSegment
{
    SStyledTextSegment(const TString &text, const CVec2 &pos, float fSize, float fHeight,
        CFontFace *pFontFace, CColor color)
        : text(text), pos(pos), fSize(fSize), fHeight(fHeight), pFontFace(pFontFace), color(color)
    {
    }

    SStyledTextSegment( const SStyledTextSegment& other )
    {
        text = other.text;
        pos = other.pos;
        fSize = other.fSize;
        pFontFace = other.pFontFace;
        color = other.color;
        fHeight = other.fHeight;
        m_bIsNewLine = other.m_bIsNewLine;
        contentSize = other.contentSize;
    }

    bool m_bIsNewLine = false;
    TString text;
    CVec2 pos;
    float fSize;
    float fHeight;
    CFontFace *pFontFace;
    CColor color;
    CVec2 contentSize;
};

typedef std::vector<SStyledTextSegment> TStyledTextSegmentList;

struct SMarkInfo
{
    SMarkInfo(CFontFace * pFace, const CColor& color, float size)
        : fSize(size)
        , fCalculationWidthFontSize(size)
        , ccolor(color)
        , pFontFace(pFace)
        , bStartMarkFlag(false)
    {
    }
    SMarkInfo(const SMarkInfo& info)
    {
        ccolor = info.ccolor;
        fSize = info.fSize;
        fCalculationWidthFontSize = info.fCalculationWidthFontSize;
        pFontFace = info.pFontFace;
        bStartMarkFlag = info.bStartMarkFlag;
    }
    virtual ~SMarkInfo(){}
    CColor ccolor;
    float fSize;
    float fCalculationWidthFontSize;
    CFontFace * pFontFace;
    bool bStartMarkFlag;
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
        EHoriAlign hAlign = eHA_LEFT, EVertAlign vAlign = eVA_MIDDLE, float fFontSize = 1.0f, bool bNumberStyle = true);

    void Parse(TStyledTextSegmentList& segmentList, bool bParseMark);

    void ParseMark(const TString &strMark);

    float GetAdaptiveHeight() const;

    float GetAdaptiveWidth() const;

private:
    void Clear();
    bool CollectCharacter(const TCHAR** ppText, CFontFace * fontFace, float fSize);
    void AppendCharacter(TCHAR c);
    void AppendCharacter(const TCHAR *pChars, uint32_t num);
    void BuildSegment(CFontFace * fontFace, CColor color, float fTextSize);
    bool NewLine();

    void HandleEscaping();
    void HandleHoriAlign();
    void HandleVertAlign();

    void ParseMark(SMarkInfo& asrkInfo, const TString &strMark);

    size_t CutString(const TString& orginString, TString& frontString, TString& backString);

    bool IsNumber( TCHAR szChar );

private:

    TStyledTextSegmentList *m_pTextSegList;
    TString m_strCache;

    CVec2 m_currPos;
    CVec2 m_currStartPos;
    CVec2 m_startPos;
    CVec2 m_endPos;
    CColor m_borderColor;
    
    std::vector<SMarkInfo> m_Stack;

    float m_fLineHeight;
    EHoriAlign m_hAlign;
    EVertAlign m_vAlign;
    uint32_t m_currLineStartIndex;
    float m_currLineWidth;
    const TCHAR *m_pText;
    float m_fCurrentTextHeight;
    bool m_bNumberStyle = false;
    bool m_bNextSegmentIsNewLine = false;
};

#endif