#include "stdafx.h"
#include "StringHelper.h"

static const unsigned char masksUtf8[6] = {
    (unsigned char)0x80, //10000000
    (unsigned char)0xE0, //11100000
    (unsigned char)0xF0, //11110000
    (unsigned char)0xF8, //11111000
    (unsigned char)0xFC, //11111100
    (unsigned char)0xFE, //11111110
};

static const int bitNums[6] = {
    7, 11, 16, 21, 26, 31,
};

static const int masksUnicode[6] = {
    (int)(0xFFFFFFFFu >> (32 - bitNums[0])),
    (int)(0xFFFFFFFFu >> (32 - bitNums[1])),
    (int)(0xFFFFFFFFu >> (32 - bitNums[2])),
    (int)(0xFFFFFFFFu >> (32 - bitNums[3])),
    (int)(0xFFFFFFFFu >> (32 - bitNums[4])),
    (int)(0xFFFFFFFFu >> (32 - bitNums[5])),
};

CStringHelper* CStringHelper::m_pInstance = NULL;

CStringHelper::CStringHelper()
{
}

CStringHelper::~CStringHelper()
{

}

bool CStringHelper::SplitString(const char* pParameter, const char* pSpliter, std::vector<std::string>& result, bool bIgnoreSpace /*= true*/)
{
    bool bRet = _tcslen(pParameter) != 0 && _tcslen(pSpliter) != 0;
    if (bRet)
    {
        std::string strIgnoreSpaceParam;
        if (bIgnoreSpace)
        {
            std::set<std::string> filter;
            filter.insert(" ");
            strIgnoreSpaceParam = FilterString(pParameter, filter);
            pParameter = strIgnoreSpaceParam.c_str();
        }
        const char* pFindStr = strstr(pParameter, pSpliter);
        const char* pReader = pParameter;
        TString strTmp;
        while (pFindStr != NULL)
        {
            uint32_t uCount = (ptrdiff_t)pFindStr - (ptrdiff_t)pReader;
            strTmp.assign(pReader, uCount);
            result.push_back(strTmp);
            pReader = pFindStr + strlen(pSpliter);
            pFindStr = strstr(pReader, pSpliter);
        }
        result.push_back(pReader);
    }
    return bRet;
}

std::string CStringHelper::FilterString(const char* pData, const std::set<std::string>& filters)
{
    std::string strRet;
    const char* pReader = pData;
    while (*pReader != 0)
    {
        // Reverse iterate, so the longer string filter will be test first, this can handle filters like : filters = {a,ab,abc} data = abcdefg 
        // of course will get "defg" not "bcdefg"
        for (std::set<std::string>::const_reverse_iterator rIter = filters.rbegin(); rIter != filters.rend() && *pReader != 0;)
        {
            const std::string& strFilter = *rIter;
            if (memcmp(pReader, strFilter.c_str(), strFilter.length() * sizeof(char)) == 0)
            {
                pReader += strFilter.length();
                rIter = filters.rbegin(); // Re-detect the filter at new pos.
            }
            else
            {
                ++rIter;
            }
        }
        if (*pReader != 0)
        {
            strRet.append(pReader++, 1);
        }
    }
    return strRet;
}

int CStringHelper::FindFirstString( const char* pSource, const char* pTarget, bool bCaseSensive )
{
    std::string strSource = pSource;
    std::string strTarget = pTarget;
    if (!bCaseSensive)
    {
        std::transform(strSource.begin(), strSource.end(), strSource.begin(), ::toupper);
        std::transform(strTarget.begin(), strTarget.end(), strTarget.begin(), ::toupper);
    }
    return strSource.find(strTarget);
}

int CStringHelper::FindLastString( const char* pSource, const char* pTarget, bool bCaseSensive )
{
    std::string strSource = pSource;
    std::string strTarget = pTarget;
    if (!bCaseSensive)
    {
        std::transform(strSource.begin(), strSource.end(), strSource.begin(), ::toupper);
        std::transform(strTarget.begin(), strTarget.end(), strTarget.begin(), ::toupper);
    }
    return strSource.rfind(strTarget);
}

uint32_t CStringHelper::Utf8GetLength(const char* pszData)
{
    uint32_t uRet = 0;
    size_t uDataBytes = _tcslen(pszData);
    for (size_t i = 0; i < uDataBytes;)
    {
        if (IsUtf8StartByte(pszData[i]))
        {
            i += Utf8GetByteNum(pszData[i]);
            ++uRet;
        }
        else
        {
            ++i;
            ++uRet;
        }
    }
    return uRet;
}

uint32_t CStringHelper::Utf8GetByteNum(char byte)
{
    uint32_t byteNum = 0;
    for (uint32_t i = 0; i < 6; ++i)
    {
        char mask = masksUtf8[i];
        if ((byte & mask) == (char)(mask << 1))
        {
            byteNum = i + 1;
            break;
        }
    }
    //not utf8 encoding
    BEATS_ASSERT(byteNum > 0, _T("Invalid utf8 head byte."));
    return byteNum;
}

uint32_t CStringHelper::UnicodeGetByteNumFrom(uint32_t code)
{
    uint32_t byteNum = 0;
    for (uint32_t i = 0; i < 6; ++i)
    {
        if ((code & ~masksUnicode[i]) == 0)
        {
            byteNum = i + 1;
            break;
        }
    }
    //not unicode
    BEATS_ASSERT(byteNum > 0, _T("Invalid unicode character."));
    return byteNum;
}

const char* CStringHelper::Utf8ExtractCodePoint(const char* pBuffer, uint32_t& uCode)
{
    uCode = 0;
    uint32_t byteNum = Utf8GetByteNum(*pBuffer);
    for (uint32_t i = 0; i < byteNum; ++i)
    {
        unsigned char currByte = pBuffer[i];
        //not utf8 encoding
        BEATS_ASSERT(i == 0 || (currByte & 0xC0) == 0x80, _T("Invalid utf8 encoding."));
        uint32_t shiftNum = i == 0 ? 0 : 6;
        uCode = uCode << shiftNum;
        uCode |= currByte & (i == 0 ? ~masksUtf8[byteNum - 1] : 0x3F);
    }
    return pBuffer + byteNum;
}

std::wstring CStringHelper::Utf8ToWString(const char* utf8str)
{
    std::wstring result;
    while (*utf8str)
    {
        uint32_t uCodePoint = 0;
        utf8str = Utf8ExtractCodePoint(utf8str, uCodePoint);
        wchar_t wchar = (wchar_t)uCodePoint;
        result.push_back(wchar);
    }
    return result;
}

std::string CStringHelper::WStringToUtf8(const wchar_t* wstr)
{
    std::string result;
    uint32_t i = 0;
    int c = wstr[i];
    while (c)
    {
        uint32_t byteNum = UnicodeGetByteNumFrom(c);
        if (byteNum == 1)
            result.push_back((unsigned char)c);
        else
        {
            for (uint32_t j = 0; j < byteNum; ++j)
            {
                unsigned char byteHead = j == 0 ? masksUtf8[byteNum - 1] << 1 : (unsigned char)0x80;
                int shiftNum = 6 * (byteNum - j - 1);
                unsigned char byteTail = (c >> shiftNum) & 0x3F;
                unsigned char byte = byteHead | byteTail;
                result.push_back(byte);
            }
        }
        c = wstr[++i];
    }
    return result;
}

bool CStringHelper::IsUtf8StartByte(char byte)
{
    return (byte & 0xC0) != 0x80;
}

bool CStringHelper::IsAscii(char byte)
{
    return unsigned(byte) < 0x80;
}

#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
CStringHelper::EStringCharacterType CStringHelper::GetCharacterType(wchar_t character) const
{
    EStringCharacterType ret = eSCT_Unknown;

    if (character >= L'0' && character <= L'9')
    {
        ret = eSCT_Number;
    }
    else if (character >= L'a' && character <= L'z')
    {
        ret = eSCT_LowerCaseCharacter;
    }
    else if (character >= L'A' && character <= L'Z')
    {
        ret = eSCT_UpperCaseCharacter;
    }
    else if (character == L'+' ||
             character == L'-' ||
             character == L'*' ||
             character == L'/' ||
             character == L'=' ||
             character == L'>' ||
             character == L'<')
    {
        ret = eSCT_MathOperator;
    }
    else if (character >= 0 && character <= 31)
    {
        ret = eSCT_ControlCharacter;
    }
    else if (character >= 0x4E00 && character <= 0x9FA5)
    {
        ret = eSCT_Chinese;
    }
    else if ((character >= 0x3040 && character <= 0x30FF) || 
            (character >= 0x31F0 && character <= 0x31FF))
    {
        ret = eSCT_Japanese;
    }
    else if ((character >= 0x1100 && character <= 0x11FF) ||
             (character >= 0x3130 && character <= 0x318F) ||
             (character >= 0xAC00 && character <= 0xD7AF) )
    {
        ret = eSCT_Korean;
    }
    else if ((character >= 0x0600 && character <= 0x06FF) ||
            (character >= 0x0750 && character <= 0x077F))
    {
        ret = eSCT_Arabia;
    }
    else if (character >= 0x0E00 && character <= 0x0E7F)
    {
        ret = eSCT_Thai;
    }
    else if (character >= 0x0400 && character <= 0x052F)
    {
        ret = eSCT_Russian;
    }
    else if (character >= 0x00C0 && character <= 0x00FF) 
    {
        ret = eSCT_GermanFrench;
    }
    else if ((character >= 0x0370 && character <= 0x03FF) ||
            (character >= 0x1F00 && character <= 0x1FFF) ||
            (character >= 0x2C89 && character <= 0x2CFF) )
    {
        ret = eSCT_Greek;
    }

    return ret;
}

CStringHelper::EStringCharacterType CStringHelper::GetCharacterType(const char* pszChar) const
{
    wchar_t buffer;
    MultiByteToWideChar(CP_ACP, 0, pszChar, -1, &buffer, sizeof(wchar_t));
    return GetCharacterType(buffer);
}

std::string CStringHelper::Utf8ToString(const char* utf8str)
{
    char szBuffer[1024];
    std::wstring wstr = Utf8ToWString(utf8str);
    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wstr.c_str(), -1, szBuffer, 1024, NULL, NULL);
    return szBuffer;
}

std::string CStringHelper::StringToUtf8(const char* str)
{
    std::wstring strWide;
    int count = MultiByteToWideChar(CP_ACP, 0, str, (int)strlen(str), NULL, 0);
    if (count > 0)
    {
        strWide.resize(count);
        MultiByteToWideChar(CP_ACP, 0, str, (int)strlen(str),
            const_cast<wchar_t*>(strWide.data()), (int)strWide.length());
    }
    return WStringToUtf8(strWide.c_str());
}

#endif

bool CStringHelper::WildMatch( const char* pat, const char* str )
{
    while (*str) 
    {
        switch (*pat) 
        {
        case '?':
            if (*str == '.') 
                return false;
            break;
        case '*':
            do 
            {
                ++pat;
            }
            while (*pat == '*'); /* enddo */
            if (!*pat)
                return true;
            while (*str) 
                if (WildMatch(pat, str++)) 
                    return true;
            return false;
        default:
            if (_totupper(*str) != _totupper(*pat)) 
                return false;
            break;
        } /* endswitch */
        ++pat, ++str;
    } /* endwhile */
    while (*pat == '*')
        ++pat;
    return !*pat;
}

uint32_t CStringHelper::BKDRHash(const char* str) const
{
    unsigned int seed = 131; // it also can be 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
    {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}

TString CStringHelper::ToLower(const TString& strIn) const
{
    TString strRet = strIn;
    std::transform(strRet.begin(), strRet.end(), strRet.begin(), tolower);
    return strRet;
}

TString CStringHelper::ToUpper(const TString& strIn) const
{
    TString strRet = strIn;
    std::transform(strRet.begin(), strRet.end(), strRet.begin(), toupper);
    return strRet;
}

TString CStringHelper::InsertString(const TString& strSourceStr, const TString& strInsertStr, bool bReverse, int32_t nStartPos, uint32_t uInterval)
{
    TString strRet = strSourceStr;
    int32_t currInsertPos = nStartPos;
    if (!strInsertStr.empty() && currInsertPos <= (int32_t)strSourceStr.length())
    {
        if (bReverse)
        {
            currInsertPos = (int32_t)strSourceStr.length() - currInsertPos;
        }
        strRet.insert(currInsertPos, strInsertStr);
        if (uInterval != 0xFFFFFFFF)
        {
            if (bReverse)
            {
                currInsertPos -= uInterval;
            }
            else
            {
                currInsertPos += uInterval;
                currInsertPos += (int32_t)strInsertStr.length();
            }
            while (currInsertPos < (int32_t)strRet.length() && currInsertPos >= 0 && (!bReverse || currInsertPos > 0))
            {
                strRet.insert(currInsertPos, strInsertStr);
                if (bReverse)
                {
                    currInsertPos -= uInterval;
                }
                else
                {
                    currInsertPos += uInterval;
                    currInsertPos += (int32_t)strInsertStr.length();
                }
            }
        }
    }
    return strRet;
}