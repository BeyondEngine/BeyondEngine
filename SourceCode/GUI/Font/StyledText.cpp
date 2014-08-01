#include "stdafx.h"
#include "StyledText.h"
#include "Utf8String.h"
#include "FontManager.h"
#include "FontFace.h"

CStyledTextParser::CStyledTextParser(
    const TCHAR *pText, const CVec2 &startPos, const CVec2 &endPos,
    CFontFace *pDefaultFontFace, CColor defaultColor, CColor borderColor,
    EHoriAlign hAlign, EVertAlign vAlign, float fFontSize)
    : m_bInMark(false)
    , m_bufIndex(0)
    , m_lineHeight(0.f)
    , m_pTextSegList(nullptr)
    , m_startPos(startPos)
    , m_endPos(endPos)
    , m_currPos(startPos)
    , m_currStartPos(startPos)
    , m_hAlign(hAlign)
    , m_vAlign(vAlign)
    , m_currLineStartIndex(0)
    , m_currLineWidth(0.f)
    , m_pText(pText)
    , m_borderColor(borderColor)
    , m_fFontSize(fFontSize)
{
    m_fontStack.push_back(pDefaultFontFace);
    m_colorStack.push_back(defaultColor);
}

TStyledTextSegmentList CStyledTextParser::Parse()
{
    TStyledTextSegmentList textSegList;
    m_pTextSegList = &textSegList;

    bool bEndParsing = false;
    while(*m_pText)
    {
#ifndef _UNICODE
        size_t byteNum = Utf8GetByteNum(*m_pText);
        if(byteNum > 1) //non-ascii, ignore.
        {
            BEATS_ASSERT(!m_bInMark, _T("non-ascii character in mark."));
            if(!CollectCharacter())
            {
                bEndParsing = true;
                break;
            }
            continue;
        }
#endif
        TCHAR c = *m_pText;
        switch (c)
        {
        case _T('<'):  //mark begin
            m_mark.clear();
            m_bInMark = true;
            ++m_pText;
            break;
        case _T('>'):  //mark end
            {
                BuildSegment();
                m_bInMark = false;
                const SMarkInfo *pMarkInfo = ParseMark(m_mark);
                HandleMark(pMarkInfo);
                ++m_pText;
            }
            break;
        case _T('\\'): //escaping
            if(m_bInMark) //ignore escaping in mark
                m_mark.push_back(c);
            else
            {
                ++m_pText;
                HandleEscaping();
            }
            break;
        default:
            if(m_bInMark)
            {
                m_mark.push_back(c);
                ++m_pText;
            }
            else
            {
                if(!CollectCharacter())
                    bEndParsing = true;
            }
            break;
        } //end of switch
        if(bEndParsing)
            break;
    } //end of while

    if(!bEndParsing)
        BuildSegment();

    HandleHoriAlign();
    HandleVertAlign();

    m_pTextSegList = nullptr;
    return textSegList;
}

void CStyledTextParser::HandleMark(const SMarkInfo *pMarkInfo)
{
#ifdef _DEBUG
    if(pMarkInfo->isOpenMark)
        m_markTypeStack.push_back(pMarkInfo->markType);
    else
    {
        BEATS_ASSERT(m_markTypeStack.back() == pMarkInfo->markType,
            _T("open mark and close mark not match."));
        m_markTypeStack.pop_back();
    }
#endif
    switch(pMarkInfo->markType)
    {
    case SMarkInfo::eMT_UNKNOWN:
        BEATS_ASSERT(false, _T("Illegal mark format in text."));
        break;
    case SMarkInfo::eMT_COLOR:
        if(pMarkInfo->isOpenMark)
            m_colorStack.push_back(down_cast<const SColorMarkInfo *>(pMarkInfo)->color);
        else
            m_colorStack.pop_back();
        break;
    case SMarkInfo::eMT_FONT:
        if(pMarkInfo->isOpenMark)
        {
            CFontFace *pFontFace = down_cast<const SFontMarkInfo *>(pMarkInfo)->pFontFace;
            if(pFontFace)
                m_fontStack.push_back(pFontFace);
            else
                m_fontStack.push_back(m_fontStack.back());
        }
        else
            m_fontStack.pop_back();
        break;
    }
}

void CStyledTextParser::HandleEscaping()
{
    switch(*m_pText)
    {
    case _T('n'):   //new line
        NewLine();
        ++m_pText;
        break;
    default:
        CollectCharacter();
        break;
    }
}

bool CStyledTextParser::CollectCharacter()
{
    bool bNotFull = true;
#ifdef _UNICODE
    wchar_t wc = *m_pText;
#else
    wchar_t wc;
    const char *pTextNew = Utf8ExtractWChar(m_pText, wc);
#endif
    const CFontGlyph *pGlyph = m_fontStack.back()->PrepareChar(wc);
    float fWidth = pGlyph->GetWidth(m_borderColor.a > 0) * m_fFontSize;
    if(m_currPos.x + fWidth > m_endPos.x)
    {
        bNotFull = NewLine();
    }
    else
    {
        m_currPos.x += fWidth;
        m_lineHeight = MAX(m_lineHeight, pGlyph->GetHeight(m_borderColor.a > 0) * m_fFontSize);
    }
#ifdef _UNICODE
    ++m_pText;
    AppendCharacter(wc);
#else
    size_t byteNum = pTextNew - m_pText;
    AppendCharacter(m_pText, byteNum);
    m_pText = pTextNew;
#endif
    return bNotFull;
}

void CStyledTextParser::AppendCharacter(TCHAR c)
{
    AppendCharacter(&c, 1);
}

void CStyledTextParser::AppendCharacter(const TCHAR *pChars, size_t num)
{
    BEATS_ASSERT(m_bufIndex + num < BUF_LEN, _T("buffer is too small"));
    memcpy(m_bufLiteral + m_bufIndex, pChars, num);
    m_bufIndex += num;
}

bool CStyledTextParser::NewLine()
{
    BuildSegment();
    m_currPos.x = m_startPos.x;
    m_currPos.y += m_lineHeight;
    m_currStartPos = m_currPos;
    HandleHoriAlign();
    m_currLineWidth = 0.f;
    m_currLineStartIndex = (*m_pTextSegList).size();
    return m_currPos.y <= m_endPos.y;
}

void CStyledTextParser::BuildSegment()
{
    AppendCharacter(_T('\0'));
    CVec2 size = m_currPos - m_currStartPos;
    m_currLineWidth += size.x;
    (*m_pTextSegList).emplace_back(m_bufLiteral, m_currStartPos, size,
        m_fontStack.back(), m_colorStack.back());
    m_bufIndex = 0;
    m_currStartPos = m_currPos;
}

void CStyledTextParser::HandleHoriAlign()
{
    float xAdjust = 0.f;
    if(m_hAlign == eHA_CENTER)
    {
        xAdjust = (m_endPos.x - m_startPos.x - m_currLineWidth) / 2;
    }
    else if(m_hAlign == eHA_RIGHT)
    {
        xAdjust = (m_endPos.x - m_startPos.x) - m_currLineWidth;
    }
    if(xAdjust > 0.f)
    {
        for(size_t i = m_currLineStartIndex; i < (*m_pTextSegList).size(); ++i)
        {
            (*m_pTextSegList)[i].pos.x += xAdjust;
        }
    }
}

void CStyledTextParser::HandleVertAlign()
{
    if(m_vAlign != eVA_TOP)
    {
        float fTotalLineHeight = m_currPos.y < m_endPos.y ?
            m_currPos.y + m_lineHeight - m_startPos.y : m_currPos.y - m_startPos.y;
        float yAdjust = 0.f;
        if(m_vAlign == eVA_MIDDLE)
        {
            yAdjust = (m_endPos.y - m_startPos.y - fTotalLineHeight) / 2;
        }
        else if(m_vAlign == eVA_BOTTOM)
        {
            yAdjust = m_endPos.y - m_startPos.y - fTotalLineHeight;
        }
        for(size_t i = 0; i < (*m_pTextSegList).size(); ++i)
        {
            (*m_pTextSegList)[i].pos.y += yAdjust;
        }
    }
}

const SMarkInfo *CStyledTextParser::ParseMark(const TString &strMark)
{
    static SColorMarkInfo colorMarkInfo;
    static SFontMarkInfo fontMarkInfo;
    static SMarkInfo unknownMarkInfo;

    const TCHAR *pStr = strMark.c_str();
    bool isOpenMark = true;
    if(*pStr == _T('/'))
    {
        isOpenMark = false;
        ++pStr;
    }

    //Match mark type and value
    SMarkInfo::EMarkType matchedType = SMarkInfo::eMT_UNKNOWN;
    int step = 0;   //0:match type, 1:match =, 2:match value, 3:match ended
    TString strValue;
    while(*pStr)
    {
        if(step == 3)
        {
            matchedType = SMarkInfo::eMT_UNKNOWN;
            break;
        }
        TCHAR c = *pStr++;
        if(step == 1)
        {
            if(c != _T('='))
            {
                matchedType = SMarkInfo::eMT_UNKNOWN;
                break;
            }
            else
            {
                step = 2;
                continue;
            }
        }
        switch(c)
        {
        case _T('C'):
            if(step == 0)
            {
                matchedType = SMarkInfo::eMT_COLOR;
                step = isOpenMark ? 1 : 3;
                break;
            } //else run to default
        case _T('F'):
            if(step == 0)
            {
                matchedType = SMarkInfo::eMT_FONT;
                step = isOpenMark ? 1 : 3;
                break;
            } //else run to default
        default:
            if(step != 2)
            {
                matchedType = SMarkInfo::eMT_UNKNOWN;
                step = 3;
            }
            else
            {
                strValue.push_back(c);
            }
            break;
        } //end of switch
    } //end of while

    SMarkInfo *pMarkInfo = nullptr;
    switch(matchedType)
    {
    case SMarkInfo::eMT_UNKNOWN:
        pMarkInfo = &unknownMarkInfo;
        break;
    case SMarkInfo::eMT_COLOR:
        _stscanf_s(strValue.c_str(), _T("0x%8X"), &colorMarkInfo.color);
        pMarkInfo = &colorMarkInfo;
        break;
    case SMarkInfo::eMT_FONT:
        fontMarkInfo.pFontFace = CFontManager::GetInstance()->GetFace(strValue);
        pMarkInfo = &fontMarkInfo;
        break;
    }
    pMarkInfo->isOpenMark = isOpenMark;
    return pMarkInfo;
}
