#include "stdafx.h"
#include "StyledText.h"

#include "FontManager.h"
#include "FontFace.h"

#define SIZEMARK _T("size=")
#define COLORMARK _T("color=")
#define FONTMARK _T("font=")

CStyledTextParser::CStyledTextParser(
    const TCHAR *pText, const CVec2 &startPos, const CVec2 &endPos,
    CFontFace *pDefaultFontFace, CColor defaultColor, CColor borderColor,
    EHoriAlign hAlign, EVertAlign vAlign, float fFontSize, bool bNumberStyle)
    : m_fLineHeight(0.f)
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
    , m_fCurrentTextHeight(0.0f)
    , m_bNumberStyle(bNumberStyle)
{
    m_Stack.emplace_back(pDefaultFontFace, defaultColor, fFontSize);
}

void CStyledTextParser::Parse(TStyledTextSegmentList& segmentList, bool bParseMark)
{
    m_pTextSegList = &segmentList;
    bool bParesNumber = false;
    uint32_t uNumberCount = 0;
    uint32_t uNumberIndex = 0;
    bool bEndParsing = false;
    bool bMark = false;
    TString markFlag;
    while (*m_pText)
    {
        SMarkInfo* pInfo = &m_Stack.back();
#ifndef _UNICODE
        uint32_t byteNum = CStringHelper::GetInstance()->Utf8GetByteNum(*m_pText);
        if (byteNum > 1) //non-ascii, ignore.
        {
            if (!CollectCharacter(&m_pText, pInfo->pFontFace, pInfo->fCalculationWidthFontSize))
            {
                bEndParsing = true;
                break;
            }
            continue;
        }
#endif
        TCHAR c = *m_pText;
        if (bParseMark)
        {
            bool bMarkEnd = false;
            switch (c)
            {
            case _T(':'):
                {
                    if (bMark)
                    {
                        bMark = false;
                        bMarkEnd = true;
                        ParseMark(markFlag);
                        ++m_pText;
                    }
                }
                break;
            case _T('<'):  //mark begin
                {
                    bMark = true;
                    markFlag.clear();
                    BuildSegment(pInfo->pFontFace, pInfo->ccolor, pInfo->fSize);
                    ++m_pText;
                }
                break;
            case _T('>'):  //mark end
                {
                    if (pInfo->bStartMarkFlag)
                    {
                        bMarkEnd = true;
                        BuildSegment(pInfo->pFontFace, pInfo->ccolor, pInfo->fSize);
                        m_Stack.pop_back();
                        bMark = false;
                        ++m_pText;
                    }
                }
                break;
            default:
                if (bMark)
                {
                    markFlag.push_back(c);
                    ++m_pText;
                }
                break;
            }//end of switch
            if (!bMark  && bMarkEnd)
            {
                continue;
            }
        }

        if (_T('\\') == c)
        {
            if (bMark)
            {
                markFlag.push_back(c);
                ++m_pText;
            }
            else
            {
                ++m_pText;
                HandleEscaping();
            }
            continue;
        }

        if (!bMark)
        {
            if (CollectCharacter(&m_pText, pInfo->pFontFace, pInfo->fCalculationWidthFontSize))
            {
                if (m_bNumberStyle)
                {
                    if (IsNumber(c) && !bParesNumber)
                    {
                        const TCHAR *pNextText = m_pText;
                        uNumberCount = 1;
                        while (pNextText && IsNumber(*pNextText))
                        {
                            uNumberCount++;
                            pNextText++;
                        }
                        bParesNumber = uNumberCount > 3;
                        uNumberIndex = 0;
                    }
                    if (bParesNumber)
                    {
                        uNumberIndex++;
                        BEATS_ASSERT(IsNumber(c));
                        if (uNumberIndex >= uNumberCount)
                        {
                            bParesNumber = false;
                        }
                        else
                        {
                            if ((uNumberCount - uNumberIndex) % 3 == 0)
                            {
                                static const TCHAR blank = _T(' ');
                                const TCHAR* pBlankChar = &blank;
                                CollectCharacter(&pBlankChar, pInfo->pFontFace, pInfo->fCalculationWidthFontSize);
                            }
                        }
                    }
                }
            }
            else
            {
                bEndParsing = true;
            }
            if (bEndParsing)
                break;
        } //end of while
    }
    if(!bEndParsing)
        BuildSegment(m_Stack.back().pFontFace, m_Stack.back().ccolor, m_Stack.back().fCalculationWidthFontSize);

    HandleHoriAlign();
    HandleVertAlign();

    m_pTextSegList = nullptr;
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
        CollectCharacter(&m_pText, m_Stack.back().pFontFace, m_Stack.back().fSize);
        break;
    }
}

bool CStyledTextParser::CollectCharacter(const TCHAR** ppText, CFontFace * fontFace, float fSize)
{
    bool bNotFull = true;
    uint32_t uCodePoint = 0;
    const char *pTextNew = CStringHelper::GetInstance()->Utf8ExtractCodePoint(*ppText, uCodePoint);
    wchar_t wc = (wchar_t)uCodePoint;
    BEATS_ASSERT(fontFace);
    bool bGlyphRest = false;
    const CFontGlyph* pGlyph = fontFace->PrepareChar(wc, bGlyphRest);
    if (bGlyphRest)
    {
        pGlyph = fontFace->PrepareChar(wc, bGlyphRest);
        BEATS_ASSERT(bGlyphRest == false);
    }
    BEATS_ASSERT(pGlyph != NULL);
    if (pGlyph)
    {
        float fWidth = pGlyph->GetWidth(m_borderColor.a > 0) * fSize;
        bool bMakeNewLine = m_currPos.X() + fWidth > m_endPos.X();
        // If the first character of new line is comment or dot, and it is not repeated, we will append the single character to the last line.
        if (bMakeNewLine)
        {
            static const uint32_t dotUnicode = 12290;
            static const uint32_t commonUnicode = 65292;
            if (dotUnicode == uCodePoint || uCodePoint == commonUnicode)
            {
                if (*pTextNew == 0)
                {
                    bMakeNewLine = false;
                }
                else
                {
                    uint32_t uNextCodePoint = 0;
                    CStringHelper::GetInstance()->Utf8ExtractCodePoint(pTextNew, uNextCodePoint);
                    bMakeNewLine = uNextCodePoint == dotUnicode || uNextCodePoint == commonUnicode;
                }
            }
        }
        if (bMakeNewLine)
        {
            bNotFull = NewLine();
        }
        else
        {
            m_currPos.X() += fWidth;
            float fGlyphHeight = pGlyph->GetHeight(m_borderColor.a > 0);
            m_fLineHeight = MAX(m_fLineHeight, fGlyphHeight * fSize);
            m_fCurrentTextHeight = MAX(m_fCurrentTextHeight, fGlyphHeight * fSize);
            uint32_t byteNum = uint32_t(pTextNew - *ppText);
            AppendCharacter(*ppText, byteNum);
            *ppText = pTextNew;
        }
    }
    else
    {
        bNotFull = false;
    }
    return bNotFull;
}

void CStyledTextParser::AppendCharacter(TCHAR c)
{
    AppendCharacter(&c, 1);
}

void CStyledTextParser::AppendCharacter(const TCHAR *pChars, uint32_t num)
{
    m_strCache.append(pChars, num);
}

bool CStyledTextParser::NewLine()
{
    bool bRet = false;
    BuildSegment(m_Stack.back().pFontFace, m_Stack.back().ccolor, m_Stack.back().fSize);
    m_bNextSegmentIsNewLine = true;
    m_currPos.X() = m_startPos.X();
    m_currPos.Y() += m_fLineHeight;
    m_currStartPos = m_currPos;
    HandleHoriAlign();
    m_currLineWidth = 0.0f;
    m_currLineStartIndex = (uint32_t)(*m_pTextSegList).size();
    bRet = m_currPos.Y() + m_fLineHeight <= m_endPos.Y();
    return bRet;
}

void CStyledTextParser::BuildSegment(CFontFace * fontFace, CColor color, float fTextSize)
{
    AppendCharacter(_T('\0'));
    CVec2 size = m_currPos - m_currStartPos;
    m_currLineWidth += size.X();
    if (!(*m_pTextSegList).empty() && (*m_pTextSegList).back().fHeight != m_fCurrentTextHeight )
    {
        if( m_fCurrentTextHeight == 0.0f )
        {
            m_fCurrentTextHeight = (*m_pTextSegList).back().fHeight;
        }
        else
        {
            if (fontFace == (*m_pTextSegList).back().pFontFace)
            {
                m_currStartPos.Y() -= (m_fCurrentTextHeight - (*m_pTextSegList).back().fHeight) * 0.5f;
                m_currPos.Y() = m_currStartPos.Y();
                if (m_fLineHeight > m_fCurrentTextHeight)
                {
                    m_fLineHeight += (m_fCurrentTextHeight - (*m_pTextSegList).back().fHeight) * 0.5f;
                }
            }
        }
    }
    SStyledTextSegment seg(m_strCache, m_currStartPos, fTextSize, m_fCurrentTextHeight, fontFace, color);
    if (m_bNextSegmentIsNewLine)
    {
        seg.m_bIsNewLine = true;
    }
    (*m_pTextSegList).push_back(seg);
    m_bNextSegmentIsNewLine = false;
    m_fCurrentTextHeight = 0.0f;
    m_strCache.clear();
    m_currStartPos = m_currPos;
}

void CStyledTextParser::HandleHoriAlign()
{
    float xAdjust = 0.f;
    if(m_hAlign == eHA_CENTER)
    {
        xAdjust = (m_endPos.X() - m_startPos.X() - m_currLineWidth) * 0.5f;
    }
    else if(m_hAlign == eHA_RIGHT)
    {
        xAdjust = (m_endPos.X() - m_startPos.X()) - m_currLineWidth;
    }
    if(xAdjust > 0.f)
    {
        for(uint32_t i = m_currLineStartIndex; i < (*m_pTextSegList).size(); ++i)
        {
            (*m_pTextSegList)[i].pos.X() += xAdjust;
        }
    }
}

void CStyledTextParser::HandleVertAlign()
{
    if(m_vAlign != eVA_TOP)
    {
        float fTotalLineHeight = m_currPos.Y() < m_endPos.Y() ?
            m_currPos.Y() + m_fLineHeight - m_startPos.Y() : m_currPos.Y() - m_startPos.Y();
        float yAdjust = 0.f;
        if(m_vAlign == eVA_MIDDLE)
        {
            yAdjust = (m_endPos.Y() - m_startPos.Y() - fTotalLineHeight) * 0.5f;
        }
        else if(m_vAlign == eVA_BOTTOM)
        {
            yAdjust = m_endPos.Y() - m_startPos.Y() - fTotalLineHeight;
        }
        for(uint32_t i = 0; i < (*m_pTextSegList).size(); ++i)
        {
            (*m_pTextSegList)[i].pos.Y() += yAdjust;
        }
    }
}

void CStyledTextParser::ParseMark(const TString &strMark)
{
    SMarkInfo markInfo(m_Stack.back().pFontFace, m_Stack.back().ccolor, m_Stack.back().fSize);

    TString resultString, nextString;
    size_t multiMark = CutString(strMark, resultString, nextString);
    while (multiMark != std::string::npos)
    {
        ParseMark(markInfo, resultString);
        multiMark = CutString(nextString, resultString, nextString);
    }
    ParseMark(markInfo, resultString);

    markInfo.bStartMarkFlag = true;
    m_Stack.push_back(markInfo);
}

size_t CStyledTextParser::CutString(const TString& orginString, TString& frontString, TString& backString)
{
    frontString = orginString;
    size_t multiMark = orginString.find(_T(','));
    if (multiMark != std::string::npos)
    {
        frontString = orginString.substr(0, multiMark);
        backString = orginString.substr(multiMark + 1, orginString.length() - multiMark - 1);
    }
    return multiMark;
}

void CStyledTextParser::ParseMark(SMarkInfo& markInfo, const TString &strMark)
{
    size_t sizeFlag = strMark.find(SIZEMARK);
    size_t colorFlag = strMark.find(COLORMARK);
    size_t fontFlag = strMark.find(FONTMARK);
    BEATS_ASSERT(sizeFlag != std::string::npos || colorFlag != std::string::npos || fontFlag != std::string::npos);
    if (sizeFlag != std::string::npos)
    {
        size_t beginPos = sizeFlag + strlen(SIZEMARK);
        float baseSize = (float)atof(strMark.substr(beginPos, strMark.length() - beginPos).c_str());
        markInfo.fSize = baseSize;
    }
    else if (colorFlag != std::string::npos)
    {
        size_t beginPos = colorFlag + strlen(COLORMARK);
        uint32_t uColor;
        _stscanf_s(strMark.substr(beginPos, strMark.length() - beginPos).c_str(), _T("0x%x"), &uColor);
        markInfo.ccolor = uColor;
    }
    else
    {
        size_t beginPos = fontFlag + strlen(FONTMARK);
        markInfo.pFontFace = CFontManager::GetInstance()->GetFace(strMark.substr(beginPos, strMark.length() - beginPos));
    }
}

float CStyledTextParser::GetAdaptiveHeight() const
{
    //the last line height add to the adaptive
    float fAdaptiveHeight = m_currPos.Y() + m_fLineHeight;
    return fAdaptiveHeight;
}

float CStyledTextParser::GetAdaptiveWidth() const
{
    float fAdaptiveWidth = m_currPos.X();
    return fAdaptiveWidth;
}

bool CStyledTextParser::IsNumber(TCHAR szChar)
{
    return szChar >= _T('0') && szChar <= _T('9');
}
