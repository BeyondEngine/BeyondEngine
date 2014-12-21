#include "stdafx.h"
#include "EnumStrGenerator.h"
#include "Serializer.h"
#include "StringHelper.h"
#include "UtilityManager.h"
#include "FilePathTool.h"
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
#include <stdlib.h>
#endif
#include "FilePathTool.h"

static const char* ENUM_KEYWORD_STR = "enum";

CEnumStrGenerator* CEnumStrGenerator::m_pInstance = NULL;

CEnumStrGenerator::CEnumStrGenerator()
: m_bInitFlag(false)
{
}

CEnumStrGenerator::~CEnumStrGenerator()
{
    BEATS_SAFE_DELETE_MAP(m_enumStrPool);
}

bool CEnumStrGenerator::ScanEnumInFile( const TCHAR* pFileName, const TCHAR* pSpecifyEnumName )
{
    bool bRet = false;
    CSerializer serializer(pFileName);
    bool bIsLanguageHeaderFile = CFilePathTool::GetInstance()->FileName(pFileName) == "Language.h";
    while (serializer.GetReadPos() != serializer.GetWritePos())
    {
        if (ScanKeyWordInCPlusPlusFile(ENUM_KEYWORD_STR, &serializer))
        {
            bool bIsKeyWord = IsEnumKeyword(&serializer);
            uint32_t startPos = serializer.GetReadPos();
            startPos += (uint32_t)strlen(ENUM_KEYWORD_STR);
            serializer.SetReadPos(startPos);
            if (bIsKeyWord && ScanKeyWordInCPlusPlusFile("{", &serializer))
            {
                uint32_t endPos = serializer.GetReadPos();
                uint32_t enumNameLength = endPos - startPos;
                char* enumName = new char[enumNameLength + 1];
                enumName[enumNameLength] = 0;
                serializer.SetReadPos(startPos);
                serializer.Deserialize(enumName, enumNameLength);
                char* enumNewName = new char[enumNameLength + 1];
                enumNewName[enumNameLength] = 0;
                FilterCPlusPlusFileComments(enumName, enumNewName, enumNameLength);
                BEATS_SAFE_DELETE_ARRAY(enumName);
                BEATS_ASSERT(serializer.GetReadPos() == endPos);
                std::set<TString> filters;
                filters.insert(_T("\r\n"));
                filters.insert(_T(" "));
                filters.insert(_T("\n"));
                filters.insert(_T("class"));// such as enum class ETest{}
                TString strEnumName = CStringHelper::GetInstance()->FilterString(enumNewName, filters);
                BEATS_SAFE_DELETE_ARRAY(enumNewName);
                int nFindColonPos = CStringHelper::GetInstance()->FindFirstString(strEnumName.c_str(), ":", false);
                if (nFindColonPos != TString::npos)
                {
                    strEnumName = strEnumName.substr(0, nFindColonPos);
                }
                std::map<TString, SEnumScanData*>::iterator iter = m_enumStrPool.find(strEnumName.c_str());
                if (pSpecifyEnumName == NULL || _tcscmp(pSpecifyEnumName, strEnumName.c_str()) == 0)
                {
                    bool bExisting = iter != m_enumStrPool.end();
                    BEATS_WARNING(!bExisting,
                        _T("The enum type %s in %s is already scanned in %s, it may be you have scaned same file twice or a enum type with same name under different name space."),
                        strEnumName.c_str(),
                        pFileName,
                        iter->second->m_enumFilePath.c_str());
                    // If the length is 0, it means we meet no name enum declare or a typedef.
                    if (strEnumName.length() != 0 && !bExisting)
                    {
                        startPos = endPos + 1; // Escape "{"
                        ScanKeyWordInCPlusPlusFile("}", &serializer);
                        endPos = serializer.GetReadPos();
                        serializer.SetReadPos(startPos);
                        uint32_t dataLength = endPos - startPos;
                        char* textBuff = new char[dataLength + 1];
                        textBuff[dataLength] = 0;
                        serializer.Deserialize(textBuff, dataLength);
                        BEATS_ASSERT(serializer.GetReadPos() == endPos);
                        char* newTextBuff = new char[dataLength + 1];
                        newTextBuff[dataLength] = 0;
                        uint32_t length = 0;
                        FilterCPlusPlusFileComments(textBuff, newTextBuff, length);
                        BEATS_SAFE_DELETE_ARRAY(textBuff);
                        TString strNewText = CStringHelper::GetInstance()->FilterString(newTextBuff, filters);
                        BEATS_SAFE_DELETE_ARRAY(newTextBuff);
                        SEnumScanData* pEnumData = new SEnumScanData;
                        std::vector<TString> rawEnumString;
                        CStringHelper::GetInstance()->SplitString(strNewText.c_str(), _T(","), rawEnumString);
                        int iDefaultEnumValue = 0;
                        for (uint32_t i = 0; i < rawEnumString.size(); ++i)
                        {
                            // The last enum string could be empty, such as
                            // enum {E_1, E_2,}
                            if (!rawEnumString[i].empty())
                            {
                                SEnumData* pData = AnalyseRawEnumString(rawEnumString[i], iDefaultEnumValue);
                                pEnumData->m_enumValue.push_back(pData);
                            }
                            else
                            {
                                BEATS_ASSERT(i == rawEnumString.size() - 1);
                            }
                        }
                        pEnumData->m_enumFilePath.assign(pFileName);
                        m_enumStrPool[strEnumName] = pEnumData;
                        bRet = true;
                    }
                }
            }
        }
    }
    if (!bIsLanguageHeaderFile)
    {
        serializer.SetReadPos(0);
        while (serializer.GetReadPos() != serializer.GetWritePos())
        {
            if (ScanKeyWordInCPlusPlusFile("eLTT_", &serializer))
            {
                TString strEnumName = "eLTT_";
                serializer.SetReadPos(serializer.GetReadPos() + strlen("eLTT_"));
                char character;
                serializer >> character;
                while (IsValidateCharacterForEnum(character))
                {
                    strEnumName.push_back(character);
                    serializer >> character;
                }
                m_codeInvokeEnumList.insert(strEnumName);
            }
        }
    }

    return bRet;
}

bool CEnumStrGenerator::ScanKeyWordInCPlusPlusFile( const char* pKeyWord, CSerializer* fileSerializer)
{
    BEATS_ASSERT(strstr(pKeyWord, "//") == NULL);
    BEATS_ASSERT(strstr(pKeyWord, "/*") == NULL);

    std::vector<SBufferData> keyWords;
    SBufferData data;
    data.pData = "//";
    data.dataLength = (uint32_t)(strlen((char*)data.pData) * sizeof(char));
    keyWords.push_back(data);
    data.pData = "/*";
    data.dataLength = (uint32_t)(strlen((char*)data.pData) * sizeof(char));
    keyWords.push_back(data);

    SBufferData quoteSign;
    quoteSign.pData = "\"";
    quoteSign.dataLength = (uint32_t)(strlen((char*)quoteSign.pData) * sizeof(char));
    keyWords.push_back(quoteSign);

    data.pData = (void*)pKeyWord;
    data.dataLength = (uint32_t)(strlen((char*)data.pData) * sizeof(char));
    keyWords.push_back(data);

    SBufferData startComment;
    startComment.pData = "*/";
    startComment.dataLength = (uint32_t)(strlen((char*)startComment.pData) * sizeof(char));

    SBufferData changeLine;
    changeLine.pData = "\r\n";
    changeLine.dataLength = (uint32_t)(strlen((char*)changeLine.pData) * sizeof(char));

    int wordId = 0;
    while (fileSerializer->ReadToDataInList(keyWords, false, &wordId) != fileSerializer->GetWritePos())
    {
        if (wordId == 0) // Read "//"
        {
            fileSerializer->SetReadPos(fileSerializer->GetReadPos() + keyWords[wordId].dataLength);
            bool bReadToEnd = fileSerializer->ReadToData(changeLine) == fileSerializer->GetWritePos();//Read to end of line "/r/b"
            if (!bReadToEnd)
            {
                fileSerializer->SetReadPos(fileSerializer->GetReadPos() + changeLine.dataLength);
            }
        }
        else if (wordId == 1) // Read "/*"
        {
            fileSerializer->SetReadPos(fileSerializer->GetReadPos() + keyWords[wordId].dataLength);
            bool bReadToEnd = fileSerializer->ReadToData(startComment) == fileSerializer->GetWritePos();//Read to end of comment "*/"
            if (!bReadToEnd)
            {
                fileSerializer->SetReadPos(fileSerializer->GetReadPos() + startComment.dataLength);
            }
        }
        else if (wordId == 2)// Read " " "
        {
            fileSerializer->SetReadPos(fileSerializer->GetReadPos() + keyWords[wordId].dataLength);
            bool bReadToEnd = fileSerializer->ReadToData(quoteSign) == fileSerializer->GetWritePos();//Read to end of comment "*/"
            if (!bReadToEnd)
            {
                fileSerializer->SetReadPos(fileSerializer->GetReadPos() + quoteSign.dataLength);
            }
        }
        else if (wordId == 3) // Read the keyword
        {
            break;
        }
    }
    return wordId == 3;
}

bool CEnumStrGenerator::FilterCPlusPlusFileComments( const char* text, char* outBuffer, uint32_t& bufferLenth )
{
    bufferLenth = 0;
    const char* pReader = text;
    char* pWriter = outBuffer;
    while (*pReader != 0)
    {
        if (memcmp(pReader, "//", 2) == 0)
        {
            while (memcmp(pReader, "\r\n", 2) != 0)
            {
                ++pReader;
            }
            pReader += 2;
        }
        else if (memcmp(pReader, "/*", 2) == 0)
        {
            while (memcmp(pReader, "*/", 2) != 0)
            {
                ++pReader;
            }
            pReader += 2;
        }
        else
        {
            *(pWriter++) = *(pReader++);
            ++bufferLenth;
        }
    }
    *pWriter = 0;
    return true;
}

bool CEnumStrGenerator::GetEnumValueData( const TCHAR* pEnumType, const std::vector<SEnumData*>*& pResult)
{
    BEATS_ASSERT(m_bInitFlag, _T("Call CEnumStrGenerator::Init before get enum value!"));
    std::map<TString, SEnumScanData*>::iterator iter = m_enumStrPool.find(pEnumType);
    bool bRet = false;
    if ( iter != m_enumStrPool.end())
    {
        pResult = &iter->second->m_enumValue;
        bRet = true;
    }
    return bRet;
}

void CEnumStrGenerator::RescanEnum(const TString& strEnumType)
{
    auto iter = m_enumStrPool.find(strEnumType);
    if (iter != m_enumStrPool.end())
    {
        TString strFilePath = iter->second->m_enumFilePath;
        BEATS_SAFE_DELETE(iter->second);
        m_enumStrPool.erase(iter);
        ScanEnumInFile(strFilePath.c_str(), strEnumType.c_str());
    }
}

bool CEnumStrGenerator::ScanEnumInDirectory( const TCHAR* pDirectory )
{
    bool bRet = false;
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    const TCHAR* pFileStr = _T("*.*");
    TString wildCmpStr(pDirectory);
    uint32_t pathSize = (uint32_t)(_tcslen(pDirectory));
    if (pDirectory[pathSize - 1] != '/')
    {
        wildCmpStr.append(_T("/"));
    }
    wildCmpStr.append(pFileStr);
    TFileData fileData;
    HANDLE searchHandle = ::FindFirstFile(wildCmpStr.c_str(), &fileData);
    if (searchHandle != INVALID_HANDLE_VALUE)
    {
        ScanEnum(fileData, pDirectory);
        while (::FindNextFile(searchHandle, &fileData))
        {
            ScanEnum(fileData, pDirectory);
        }
        FindClose(searchHandle);
        bRet = true;
    }

    BEATS_ASSERT(GetLastError() == ERROR_NO_MORE_FILES, _T("Scan Enum failed in path %s"), pDirectory);
#endif
    return bRet;
}

bool CEnumStrGenerator::ScanEnum( const TFileData& fileData, const TString& fullDirectoryPath )
{
#if (BEYONDENGINE_PLATFORM == PLATFORM_WIN32)
    if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0)
    {
        if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            const TCHAR* pReader = &fileData.cFileName[_tcslen(fileData.cFileName) - 4];
            if (memcmp(pReader, _T(".cpp"), 4 * sizeof(TCHAR)) == 0 ||
                memcmp(pReader + 2, _T(".h"), 2 * sizeof(TCHAR)) == 0)
            {
                TString fileFullPath = fullDirectoryPath;
                fileFullPath.append(_T("/")).append(fileData.cFileName);
                ScanEnumInFile(fileFullPath.c_str());
            }
        }
        else
        {
            if (_tcsicmp(fileData.cFileName, _T(".")) != 0 &&
                _tcsicmp(fileData.cFileName, _T("..")) != 0 )
            {
                TString fileFullPath = fullDirectoryPath;
                fileFullPath.append(_T("/")).append(fileData.cFileName);
                ScanEnumInDirectory(fileFullPath.c_str());
            }
        }
    }
#endif
    return true;
}

bool CEnumStrGenerator::IsEnumKeyword( CSerializer* pSerailizer )
{
    bool bRet = true;
    //Check front.
    if (pSerailizer->GetReadPos() > 1)
    {
        unsigned char byteData = pSerailizer->GetBuffer()[pSerailizer->GetReadPos() - 1];
        if (byteData != '\n' && byteData != ' ' && byteData != '\t')
        {
            bRet = false;
        }
    }
    if (bRet)
    {
        //Check after, there must be some characters after enum.
        unsigned char byteData = pSerailizer->GetBuffer()[pSerailizer->GetReadPos() + strlen(ENUM_KEYWORD_STR)];
        if (byteData != '\n' && byteData != ' ' && byteData != '\t' && byteData != '{')
        {
            bRet = false;
        }
    }
    return bRet;
}

void CEnumStrGenerator::Init(const std::vector<TString>& scanPathList, const TCHAR* pszCacheFileName)
{
    if (!LoadCacheFile(pszCacheFileName))
    {
        for(uint32_t i = 0; i < scanPathList.size(); ++i)
        {
            ScanEnumInDirectory(scanPathList[i].c_str());
        }
        SaveCacheFile(pszCacheFileName);
    }
    m_bInitFlag = true;
}

void CEnumStrGenerator::SaveCacheFile(const TCHAR* pszCacheFileName)
{
    CSerializer serializer;
    serializer << m_enumStrPool.size();
    std::map<TString, SEnumScanData*>::iterator iter = m_enumStrPool.begin();
    for (; iter != m_enumStrPool.end(); ++iter)
    {
        serializer << iter->first;
        serializer << iter->second->m_enumFilePath;
        serializer << iter->second->m_enumValue.size();
        for (uint32_t i = 0; i < iter->second->m_enumValue.size(); ++i)
        {
            serializer << iter->second->m_enumValue[i]->m_str;
            serializer << iter->second->m_enumValue[i]->m_value;
        }
    }
    serializer << m_codeInvokeEnumList.size();
    for (auto iter = m_codeInvokeEnumList.begin(); iter != m_codeInvokeEnumList.end(); ++iter)
    {
        serializer << *iter;
    }
    serializer.Deserialize(pszCacheFileName);
}

bool CEnumStrGenerator::LoadCacheFile(const TCHAR* pszCacheFileName)
{
    bool bRet = false;
    if (CFilePathTool::GetInstance()->Exists(pszCacheFileName))
    {
        CSerializer serializer(pszCacheFileName);
        uint32_t enumTypeCount = 0;
        serializer >> enumTypeCount;
        for(uint32_t i = 0; i < enumTypeCount; ++i)
        {
            TString enumTypeStr;
            serializer >> enumTypeStr;
            std::map<TString, SEnumScanData*>::iterator iter = m_enumStrPool.find(enumTypeStr);
            if (iter == m_enumStrPool.end())
            {
                SEnumScanData* pEnumScanData = new SEnumScanData;
                serializer >> pEnumScanData->m_enumFilePath;
                uint32_t dataStrCount = 0;
                serializer >> dataStrCount;
                for (uint32_t j = 0; j < dataStrCount; ++j)
                {
                    SEnumData* pEnumData = new SEnumData();
                    serializer >> pEnumData->m_str;
                    serializer >> pEnumData->m_value;
                    pEnumScanData->m_enumValue.push_back(pEnumData);
                }
                m_enumStrPool[enumTypeStr] = pEnumScanData;
            }
            else
            {
                // To skip those ignore data.
                TString strFilePath;
                serializer >> strFilePath;
                BEATS_WARNING(false, 
                    _T("enum type: %s is already scaned in\n%s, we met another in file\n%s\nIt will be ignored!"),
                    enumTypeStr.c_str(),
                    iter->second->m_enumFilePath.c_str(),
                    strFilePath.c_str());
                uint32_t uCount = 0;
                serializer >> uCount;
                for (uint32_t i = 0; i < uCount; ++i)
                {
                    uint32_t uDataHolder2 = 0;
                    serializer >> strFilePath;
                    serializer >> uDataHolder2;
                }
            }
        }
        uint32_t uCodeInvokeEnumCount = 0;
        serializer >> uCodeInvokeEnumCount;
        for (uint32_t i = 0; i < uCodeInvokeEnumCount; ++i)
        {
            TString strEnum;
            serializer >> strEnum;
            m_codeInvokeEnumList.insert(strEnum);
        }
        bRet = true;
    }
    return bRet;
}

bool CEnumStrGenerator::IsLanguageEnumInvokeInCode(const TString& strEnum) const
{
    return m_codeInvokeEnumList.find(strEnum) != m_codeInvokeEnumList.end();
}

SEnumData* CEnumStrGenerator::AnalyseRawEnumString( const TString& rawEnumStr, int& curValue )
{
    SEnumData* pEnumData = new SEnumData;
    std::vector<TString> result;
    CStringHelper::GetInstance()->SplitString(rawEnumStr.c_str(), _T("="), result, true);
    BEATS_ASSERT(!rawEnumStr.empty(), _T("raw enum string can't be empty"));
    BEATS_ASSERT(result.size() == 2 || result.size() == 1, _T("Split raw enum string %s failed"), rawEnumStr.c_str());
    if (result.size() == 2)
    {
        pEnumData->m_str = result[0].c_str();
        const TCHAR* pStrData = result[1].c_str(); 
        TCHAR* pStopPos = NULL;
        bool bIsHex = pStrData[0] == '0' && (pStrData[1] == 'x' || pStrData[1] == 'X');
        pEnumData->m_value = _tcstoul(pStrData, &pStopPos, bIsHex ? 16 : 10);
        curValue = pEnumData->m_value + 1;
    }
    else
    {
        pEnumData->m_str = rawEnumStr;
        pEnumData->m_value = curValue++;
    }
    return pEnumData;
}

bool CEnumStrGenerator::IsValidateCharacterForEnum(char character) const
{
    bool bNumber = character >= '0' && character <= '9';
    bool bChar = (character >= 'a' && character <= 'z') ||
        (character >= 'A' && character <= 'Z');
    bool bSign = character == '_';
    return bNumber || bChar || bSign;
}
