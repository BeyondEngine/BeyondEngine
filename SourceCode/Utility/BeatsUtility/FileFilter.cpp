#include "stdafx.h"
#include "FileFilter.h"
#include "Serializer.h"
#include "StringHelper.h"

CFileFilter::CFileFilter()
: m_cCheckHidden(-1)
, m_cCheckReadOnly(-1)
, m_cCheckSystemFile(-1)
, m_bFileNameWildCardMutex(false)
, m_bFileNameWildCardMatchAll(false)
, m_bFileSizeMutex(false)
, m_iMinFileSize(-1)
, m_iMaxFileSize(-1)
{

}

CFileFilter::~CFileFilter()
{

}

void CFileFilter::Serialize(CSerializer& serializer)
{
    serializer >> m_cCheckHidden;
    serializer >> m_cCheckReadOnly;
    serializer >> m_cCheckSystemFile;

    serializer >> m_bFileNameWildCardMutex;
    serializer >> m_bFileSizeMutex;

    serializer >> m_iMinFileSize;
    serializer >> m_iMaxFileSize;
    serializer >> m_bFileNameWildCardMatchAll;
    uint32_t uCounter;
    TString strData;
    serializer >> uCounter;
    for (uint32_t i = 0; i < uCounter; ++i)
    {
        serializer >> strData;
        m_strFileNameWildCard.push_back(strData);
    }
}

void CFileFilter::Deserialize(CSerializer& serializer)
{
    serializer << m_cCheckHidden;
    serializer << m_cCheckReadOnly;
    serializer << m_cCheckSystemFile;

    serializer << m_bFileNameWildCardMutex;
    serializer << m_bFileSizeMutex;

    serializer << m_iMinFileSize;
    serializer << m_iMaxFileSize;
    serializer << m_bFileNameWildCardMatchAll;

    serializer << m_strFileNameWildCard.size();
    for (uint32_t i = 0; i < m_strFileNameWildCard.size(); ++i)
    {
        serializer << m_strFileNameWildCard[i];
    }
}

bool CFileFilter::FilterFile( const TFileData* pFileData ) const
{
    bool bRet = pFileData != NULL;
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)

    if (bRet)
    {
        bool bIsFile = (pFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;

        // 1. Common check for both directory and file.
        if (m_cCheckHidden != -1)
        {
            bool bIsHiden = (pFileData->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) > 0;
            bRet = m_cCheckHidden > 0 ? bIsHiden : !bIsHiden;
        }
        if (bRet && m_cCheckReadOnly != -1)
        {
            bool bIsReadOnly = (pFileData->dwFileAttributes & FILE_ATTRIBUTE_READONLY) > 0;
            bRet = m_cCheckReadOnly > 0 ? bIsReadOnly : !bIsReadOnly;
        }
        if (bRet && m_cCheckSystemFile != -1)
        {
            bool bIsSystemFile = (pFileData->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) > 0;
            bRet = m_cCheckSystemFile > 0 ? bIsSystemFile : !bIsSystemFile;
        }
        // filter name in m_strFileName
        if (bRet)
        {
            for (uint32_t i = 0; i < m_strFileNameWildCard.size(); ++i)
            {
                bool bWildCardResult = CStringHelper::GetInstance()->WildMatch(m_strFileNameWildCard[i].c_str(), pFileData->cFileName);
                bRet = m_bFileNameWildCardMutex ? !bWildCardResult : bWildCardResult;
                if (m_bFileNameWildCardMatchAll ? !bRet : bRet)
                {
                    break;
                }
            }
        }

        // 2. Do check operation for file(non-directory).
        if(bRet && bIsFile)
        {
            if (m_iMinFileSize >= 0 && m_iMaxFileSize >= m_iMinFileSize)
            {
                long long iFileSize = pFileData->nFileSizeHigh;
                iFileSize = iFileSize << 32;
                iFileSize += pFileData->nFileSizeLow;

                bool bSizeInRange = iFileSize >= m_iMinFileSize && iFileSize <= m_iMaxFileSize;
                bRet = m_bFileSizeMutex ? !bSizeInRange : bSizeInRange;
            }
        }
    }
#endif
    return bRet;
}

void CFileFilter::SetCheckHiddenFlag(char flag)
{
    m_cCheckHidden = flag;
}

void CFileFilter::SetCheckReadOnlyFlag(char flag)
{
    m_cCheckReadOnly = flag;
}

void CFileFilter::SetCheckSystemFlag(char flag)
{
    m_cCheckSystemFile = flag;
}

void CFileFilter::SetFileSizeMutex(bool bMutex)
{
    m_bFileSizeMutex = bMutex;
}

void CFileFilter::SetFileNameMutex(bool bMutex)
{
    m_bFileNameWildCardMutex = bMutex;
}

void CFileFilter::SetFileSizeRange(const long long& iMinSize, const long long& iMaxSize)
{
    m_iMinFileSize = iMinSize;
    m_iMaxFileSize = iMaxSize;
}

void CFileFilter::SetFileNameFilter(const std::vector<TString>& strFileNameFilter, bool bMatchAll)
{
    m_strFileNameWildCard = strFileNameFilter;
    m_bFileNameWildCardMatchAll = bMatchAll;
}