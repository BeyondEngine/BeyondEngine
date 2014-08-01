#ifndef BEATS_UTILITY_STRINGHELPER_STRINGHELPER_H__INCLUDE
#define BEATS_UTILITY_STRINGHELPER_STRINGHELPER_H__INCLUDE

class CStringHelper
{
    BEATS_DECLARE_SINGLETON(CStringHelper);
public:
    enum EStringCharacterType
    {
        eSCT_Unknown,
        eSCT_Number,
        eSCT_LowerCaseCharacter,
        eSCT_UpperCaseCharacter,
        eSCT_MathOperator,
        eSCT_ControlCharacter,
        eSCT_Chinese,
        eSCT_Japanese,
        eSCT_Korean,
        eSCT_Arabia,
        eSCT_Thai,
        eSCT_Russian,
        eSCT_GermanFrench,
        eSCT_Greek,

        eSCT_Count,
        eSCT_Force32Bit = 0xFFFFFFFF
    };
public:
    // Convert
    bool SplitString(const TCHAR* pParameter, const TCHAR* pSpliter, std::vector<TString>& result, bool bIgnoreSpace = true);
    void ConvertToTCHAR(const wchar_t* pData, TCHAR* pBuffer, size_t bufferLength);
    void ConvertToTCHAR( const char* pData, TCHAR* pBuffer, size_t bufferLength );
    void ConvertToCHAR( const TCHAR* pData, char* pBuffer, size_t bufferLength );
    void ConvertToWCHAR( const TCHAR* pData, wchar_t* pBuffer, size_t bufferLength ) const;

    // Filter and Find
    TString FilterString(const TCHAR* pData, const std::vector<TString>& filters);
    int FindFirstString(const TCHAR* pSource, const TCHAR* pTarget,  bool bCaseSensive);
    int FindLastString(const TCHAR* pSource, const TCHAR* pTarget,  bool bCaseSensive);

    // Check character.
    CStringHelper::EStringCharacterType GetCharacterType(wchar_t character) const;
    CStringHelper::EStringCharacterType GetCharacterType(const char* pszChar) const;

    bool WildMatch(const TCHAR* pat, const TCHAR* str); 
};

#endif