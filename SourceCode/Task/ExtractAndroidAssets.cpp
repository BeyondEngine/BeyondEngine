#include "stdafx.h"
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
#include "ExtractAndroidAssets.h"
#include "PlatformHelper.h"
#include "Framework/android/JniHelper.h"
#include "Framework/android/AndroidHandler.h"
#include "Utility/BeatsUtility/UtilityManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Framework/PlatformHelper.h"
#include "BeyondEngineUI/StarRaidersGUISystem.h"
#include <sys/stat.h>
CExtractAndroidAssets::CExtractAndroidAssets()
: m_uTotalSize(0)
, m_uExtractedSize(0)
, m_pAssetsDirectory(nullptr)
{
}

CExtractAndroidAssets::~CExtractAndroidAssets()
{
    BEATS_SAFE_DELETE(m_pAssetsDirectory);
}

uint32_t CExtractAndroidAssets::GetProgress()
{
    uint32_t uRet = 0;
    if (m_uTotalSize > 0)
    {
        uRet = (uint32_t)((float)m_uExtractedSize / m_uTotalSize * 100);
    }
    return uRet;
}

void CExtractAndroidAssets::Execute(float /*ddt*/)
{
    if (m_pAssetsDirectory == nullptr)
    {
        BEATS_ASSERT(m_uTotalSize == 0 && m_uExtractedSize == 0);
        m_pAssetsDirectory = new SDirectory(nullptr, "");
        CAndroidHandler::GetInstance()->FillDirectory(*m_pAssetsDirectory);
        m_uTotalSize = m_pAssetsDirectory->m_data.nFileSizeLow;
    }
    BEATS_PRINT("extract res size:%d  last storage size:%d", m_uTotalSize/(1024 * 1024), CPlatformHelper::GetInstance()->GetFreeSpace());
    if (CPlatformHelper::GetInstance()->GetFreeSpace() >= 0
        &&m_uTotalSize/(1024 * 1024) > CPlatformHelper::GetInstance()->GetFreeSpace())
     {
         CStarRaidersUISystem::GetInstance()->ShowMessageBox(_T(""), _T("storage not enough"), true);
     }
     else
    {
        ExtractAssetsFromDirectory(m_pAssetsDirectory, m_uExtractedSize);
        BEATS_SAFE_DELETE(m_pAssetsDirectory);
        CPlatformHelper::GetInstance()->SetAssetFileExtracted(true);
    }
}

void CExtractAndroidAssets::ExtractAssetsFromDirectory(SDirectory* pDirectory, uint32_t& extractSize)
{
    const TString& strExternalRootPath = CAndroidHandler::GetInstance()->GetAndroidPath(eAPT_FilesDirectory);
    TString strDirectoryAbsolutePath = strExternalRootPath;
    if (!pDirectory->m_strPath.empty())
    {
        strDirectoryAbsolutePath.append("/").append(pDirectory->m_strPath);
        CFilePathTool::GetInstance()->MakeDirectory(strDirectoryAbsolutePath.c_str());
        BEATS_PRINT(_T("mkdir:%s\n"), strDirectoryAbsolutePath.c_str());
    }
    CSerializer fileSerializer;
    for (size_t i = 0; i < pDirectory->m_pFileList->size(); ++i)
    {
        TFileData* pFile = pDirectory->m_pFileList->at(i);
        TString strFilePath = ASSET_ROOT_PATH;
        if (!pDirectory->m_strPath.empty())
        {
            strFilePath.append(pDirectory->m_strPath).append("/");
        }
        strFilePath.append(pFile->cFileName);
        fileSerializer.Reset();
        CFilePathTool::GetInstance()->LoadFile(&fileSerializer, strFilePath.c_str(), "rb");
        strFilePath = strDirectoryAbsolutePath;
        strFilePath.append("/").append(pFile->cFileName);
        fileSerializer.Deserialize(strFilePath.c_str());
        BEATS_PRINT(_T("Extract %s\n"), strFilePath.c_str());
        extractSize += pFile->nFileSizeLow;
    }
    for (size_t i = 0; i < pDirectory->m_pDirectories->size(); ++i)
    {
        ExtractAssetsFromDirectory(pDirectory->m_pDirectories->at(i), extractSize);
    }
}
#endif