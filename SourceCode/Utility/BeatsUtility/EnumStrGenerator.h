#ifndef BEATS_UTILITY_ENUMSTRGENERATOR_ENUMSTRGENERATOR_H__INCLUDE
#define BEATS_UTILITY_ENUMSTRGENERATOR_ENUMSTRGENERATOR_H__INCLUDE

class CSerializer;
struct SEnumData
{
    int m_value;
    TString m_str;
    SEnumData()
        :m_value(0)
    {

    }
    SEnumData(const TString& str, int value)
        : m_value(value)
        , m_str(str)
    {

    }
};

struct SEnumScanData
{
    SEnumScanData()
    {

    }
    ~SEnumScanData()
    {
        BEATS_SAFE_DELETE_VECTOR(m_enumValue);
    }
    std::vector<SEnumData*> m_enumValue;
    TString m_enumFilePath;
};

class CEnumStrGenerator
{
    BEATS_DECLARE_SINGLETON(CEnumStrGenerator);

public:
    void Init(const std::vector<TString>& scanPathList, const TCHAR* pszCacheFileName);
    bool GetEnumValueData(const TCHAR* pEnumType, const std::vector<SEnumData*>*& pResult);
    void SaveCacheFile(const TCHAR* pszCacheFileName);
    bool LoadCacheFile(const TCHAR* pszCacheFileName);

private:
    bool ScanEnumInFile(const TCHAR* pFileName);
    bool ScanEnumInDirectory(const TCHAR* pDirectory);
    // This method allows you scan a C++ file, which means it will handle "//" "/*" to filter comments content.
    bool ScanKeyWordInCPlusPlusFile(const char* pKeyWord, CSerializer* fileSerializer);
    bool FilterCPlusPlusFileComments(const char* text, char* outBuffer, size_t& bufferLenth);
    bool ScanEnum(const TFileData& data, const TString& fullDirectoryPath);
    // To avoid situation: m_enumValue, you can match "enum" while its not a keyword. The serializer must read to the "enum" string.
    bool IsEnumKeyword(CSerializer* pSerailizer);

    SEnumData* AnalyseRawEnumString(const TString& rawEnumStr, int& curValue);

private:
    bool m_bInitFlag;
    std::map<TString, SEnumScanData*> m_enumStrPool;

};

#endif