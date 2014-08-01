#include "stdafx.h"
#include "Utf8String.h"

#ifdef BYTE
#undef BYTE
#endif
#define BYTE unsigned char

static const BYTE masksUtf8[6] = {
    (BYTE)0x80, //10000000
    (BYTE)0xE0, //11100000
    (BYTE)0xF0, //11110000
    (BYTE)0xF8, //11111000
    (BYTE)0xFC, //11111100
    (BYTE)0xFE, //11111110
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

size_t Utf8GetByteNum(char byte)
{
    size_t byteNum = 0;
    for(size_t i = 0; i < 6; ++i)
    {
        char mask = masksUtf8[i];
        if((byte & mask) == (char)(mask<<1))
        {
            byteNum = i + 1;
            break;
        }
    }
    //not utf8 encoding
    BEATS_ASSERT(byteNum > 0, _T("Invalid utf8 head byte."));
    return byteNum;
}

size_t UnicodeGetByteNumFrom(int code)
{
    size_t byteNum = 0;
    for(size_t i = 0; i < 6; ++i)
    {
        if((code & ~masksUnicode[i]) == 0)
        {
            byteNum = i + 1;
            break;
        }
    }
    //not unicode
    BEATS_ASSERT(byteNum > 0, _T("Invalid unicode character."));
    return byteNum;
}

const char *Utf8ExtractWChar(const char *pBuffer, wchar_t &wchar)
{
    wchar = L'\0';
    size_t byteNum = Utf8GetByteNum(*pBuffer);
    for(size_t i = 0; i < byteNum; ++i)
    {
        BYTE currByte = pBuffer[i];
        //not utf8 encoding
        BEATS_ASSERT(i == 0 || (currByte & 0xC0) == 0x80, _T("Invalid utf8 encoding."));
        size_t shiftNum = i == 0 ? 0 : 6;
        wchar = wchar << shiftNum;
        wchar |= currByte & (i == 0 ? ~masksUtf8[byteNum - 1] : 0x3F);
    }
    return pBuffer + byteNum;
}

std::wstring Utf8ToWString(const std::string &utf8str)
{
    return Utf8ToWString(utf8str.c_str());
}

std::wstring Utf8ToWString(const char *utf8str)
{
    std::wstring result;
    while(*utf8str)
    {
        wchar_t wchar = L'\0';
        utf8str = Utf8ExtractWChar(utf8str, wchar);
        result.push_back(wchar);
    }
    return result;
}

TString Utf8ToTString(const std::string &utf8str)
{
    return Utf8ToTString(utf8str.c_str());
}

TString Utf8ToTString(const char *utf8str)
{
#ifdef UNICODE
    return Utf8ToWString(utf8str);
#else
    static const size_t BUF_LEN = 1024;
    std::wstring wstr = Utf8ToWString(utf8str);
    TCHAR buf[BUF_LEN];
    CStringHelper::GetInstance()->ConvertToTCHAR(wstr.c_str(), buf, BUF_LEN);
    return buf;
#endif
}

std::string WStringToUtf8(const std::wstring &wstr)
{
    return WStringToUtf8(wstr.c_str());
}

std::string WStringToUtf8(const wchar_t *wstr)
{
    std::string result;
    size_t i = 0;
    int c = wstr[i];
    while(c)
    {
        size_t byteNum = UnicodeGetByteNumFrom(c);
        if(byteNum == 1)
            result.push_back((BYTE)c);
        else
        {
            for(size_t j = 0; j < byteNum; ++j)
            {
                BYTE byteHead = j == 0 ? masksUtf8[byteNum-1]<<1 : (BYTE)0x80;
                int shiftNum = 6*(byteNum-j-1);
                BYTE byteTail = (c >> shiftNum) & 0x3F;
                BYTE byte = byteHead | byteTail;
                result.push_back(byte);
            }
        }
        c = wstr[++i];
    }
    return result;
}

std::string TStringToUtf8(const TString &tstr)
{
    return TStringToUtf8(tstr.c_str());
}

std::string TStringToUtf8(const TCHAR *tstr)
{
#ifdef UNICODE
    return WStringToUtf8(tstr);
#else
    static const size_t BUF_LEN = 1024;
    wchar_t buf[BUF_LEN];
    CStringHelper::GetInstance()->ConvertToWCHAR(tstr, buf, BUF_LEN);
    return WStringToUtf8(buf);
#endif
}
