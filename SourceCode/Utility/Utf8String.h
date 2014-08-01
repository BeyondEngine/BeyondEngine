#ifndef BEYOND_ENGINE_UTILITY_UTF8STRING_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_UTF8STRING_H__INCLUDE

size_t Utf8GetByteNum(char byte);
size_t UnicodeGetByteNumFrom(int code);

const char *Utf8ExtractWChar(const char *pBuffer, wchar_t &character);

std::wstring Utf8ToWString(const std::string &utf8str);
std::wstring Utf8ToWString(const char *utf8str);
TString Utf8ToTString(const std::string &utf8str);
TString Utf8ToTString(const char *utf8str);

std::string WStringToUtf8(const std::wstring &wstr);
std::string WStringToUtf8(const wchar_t *wstr);
std::string TStringToUtf8(const TString &tstr);
std::string TStringToUtf8(const TCHAR *tstr);


#endif