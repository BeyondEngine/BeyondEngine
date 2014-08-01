#ifndef BEATS_UTILITY_FILEFILTER_FILEFILTER_H__INCLUDE
#define BEATS_UTILITY_FILEFILTER_FILEFILTER_H__INCLUDE

class CSerializer;

class CFileFilter
{
public:
    CFileFilter();
    ~CFileFilter();

    void Serialize(CSerializer& serializer);
    void Deserialize(CSerializer& serializer);

    bool FilterFile(const WIN32_FIND_DATA* pFileData) const;

    void SetCheckHiddenFlag(char flag);
    void SetCheckReadOnlyFlag(char flag);
    void SetCheckSystemFlag(char flag);

    void SetFileSizeMutex(bool bMutex);
    void SetFileNameMutex(bool bMutex);

    void SetFileSizeRange(const long long& uMinSize, const long long& uMaxSize);
    void SetFileNameFilter(const std::vector<TString>& strFileNameFilter, bool bMatchAll);

private:
    char m_cCheckHidden; // -1 = Don't care about if it is hidden; 0 = file must be not hidden; 1 = file must be hidden;
    char m_cCheckReadOnly;// -1 = Don't care about if it is read only; 0 = file must be not read only; 1 = file must be read only;
    char m_cCheckSystemFile;// -1 = Don't care about if it is system file; 0 = file must be not system file; 1 = file must be system file;

    bool m_bFileNameWildCardMatchAll; // If it is true, the file name must match all wild card to pass the filter, otherwise, it can pass by only match any one of it.
    bool m_bFileNameWildCardMutex;// reverse the result: if this is false, file MUST has a fileName which is match m_strFileName, otherwise, it MUSTN'T match.
    bool m_bFileSizeMutex;// reverse the result: if this is false, file size MUST between m_iMinFileSize and m_iMaxFileSize, otherwise, it is out of the range.

    long long m_iMinFileSize;// if it is less than 0, ignore the size check.
    long long m_iMaxFileSize;
    std::vector<TString> m_strFileNameWildCard;
};

#endif