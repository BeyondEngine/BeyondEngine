#include "stdafx.h"
#if (BEYONDENGINE_PLATFORM == PLATFORM_IOS)
#include "ExtractIosAssets.h"
#include "PlatformHelper.h"
#include "Utility/BeatsUtility/UtilityManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "BeyondEngineUI/StarRaidersGUISystem.h"
#include "Framework/PlatformHelper.h"
#include <sys/stat.h>
#import <Foundation/Foundation.h>
#import "ViewController.h"
CExtractIosAssets::CExtractIosAssets()
: m_uTotalSize(0)
, m_uExtractedSize(0)
, m_pAssetsDirectory(nullptr)
{
}

CExtractIosAssets::~CExtractIosAssets()
{
    BEATS_SAFE_DELETE(m_pAssetsDirectory);
}

uint32_t CExtractIosAssets::GetProgress()
{
    uint32_t uRet = 0;
    if (m_uTotalSize > 0)
    {
        uRet = (uint32_t)((float)m_uExtractedSize / m_uTotalSize * 100);
    }
    return uRet;
}

uint32_t CExtractIosAssets::fileSizeAtPath(TString strPath)
{
    NSString *filePath = [NSString stringWithUTF8String:strPath.c_str()];
    NSFileManager* manager = [NSFileManager defaultManager];
    if ([manager fileExistsAtPath:filePath]){
        return (uint32_t)[[manager attributesOfItemAtPath:filePath error:nil] fileSize];
    }
    return 0;
}

//遍历文件夹获得文件夹大小，返回多少M
uint32_t CExtractIosAssets::folderSizeAtPath(TString strPath)
{
    NSString *folderPath = [NSString stringWithUTF8String:strPath.c_str()];
    NSFileManager* manager = [NSFileManager defaultManager];
    if (![manager fileExistsAtPath:folderPath]) return 0;
    NSEnumerator *childFilesEnumerator = [[manager subpathsAtPath:folderPath] objectEnumerator];
    NSString* fileName;
    uint32_t folderSize = 0;
    while ((fileName = [childFilesEnumerator nextObject]) != nil){
        NSString* fileAbsolutePath = [folderPath stringByAppendingPathComponent:fileName];
        std::string strFileAbsolutePath = [fileAbsolutePath UTF8String];
        folderSize += fileSizeAtPath(strFileAbsolutePath);
    }
    return folderSize;
}

void CExtractIosAssets::Execute(float /*ddt*/)
{
    std::string strSrcFilePath = CPlatformHelper::GetInstance()->GetBinaryPath() + "/" + "resource";
    m_uTotalSize = folderSizeAtPath(strSrcFilePath);
    BEATS_PRINT("Extract size :%dM free space:%dM", m_uTotalSize/(1024 * 1024), CPlatformHelper::GetInstance()->GetFreeSpace());
    if (CPlatformHelper::GetInstance()->GetFreeSpace() >= 0
        &&m_uTotalSize/(1024 * 1024) > CPlatformHelper::GetInstance()->GetFreeSpace())
    {
        CStarRaidersUISystem::GetInstance()->ShowMessageBox(_T(""),_T("storage not enough"), true); // restart app
    }
    else
    {
        std::string strDesFilePath = CPlatformHelper::GetInstance()->GetPersistentDataPath();
        
        std::string fileSrcRes = CPlatformHelper::GetInstance()->GetBinaryPath() + "/" + "ResourceList.bin";
        std::string fileDesRes = strDesFilePath + "/" + "ResourceList.bin";
        CopyOneFile(fileSrcRes, fileDesRes);
        
        std::string filePathVer = CPlatformHelper::GetInstance()->GetBinaryPath() + "/" + "VersionInfo.bin";
        std::string fileDesVer = strDesFilePath + "/" + "VersionInfo.bin";
        CopyOneFile(filePathVer, fileDesVer);
        
        strDesFilePath = strDesFilePath + "/" + "resource";
        NSString *strDes = [NSString stringWithUTF8String:strDesFilePath.c_str()];
        NSFileManager *fileManager = [NSFileManager defaultManager];
        
        BOOL bCreateDir = [fileManager createDirectoryAtPath:strDes withIntermediateDirectories:YES attributes:nil error:nil];
        if(!bCreateDir)
        {
            BEATS_PRINT("create %s failed", strDesFilePath.c_str());
        }
        else
        {
            CopyFiles(strSrcFilePath, strDesFilePath);
        }
        m_uExtractedSize = m_uTotalSize;
        
        CPlatformHelper::GetInstance()->SetAssetFileExtracted(true);
    }
}

void CExtractIosAssets::CopyFiles(const std::string& strSrcPath, const std::string& strDesPath)
{
    NSString *strSrc = [NSString stringWithUTF8String:strSrcPath.c_str()];
    
    NSDirectoryEnumerator *enumerator = [[NSFileManager defaultManager]enumeratorAtPath:strSrc];
    for (NSString *fileName in enumerator)
    {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        std::string strFileName = [fileName UTF8String];
        std::string strSrcFilePath = strSrcPath + "/" + strFileName;
        std::string strDesFilePath = strDesPath + "/" + strFileName;
        
        NSString *strSrcFile = [NSString stringWithUTF8String:strSrcFilePath.c_str()];
        BOOL isSrcDir = FALSE;
        BOOL isDirExist = [fileManager fileExistsAtPath:strSrcFile isDirectory:&isSrcDir];
        BEATS_ASSERT(isDirExist);
        if(isSrcDir)
        {
            BOOL isDesDir = FALSE;
            NSString *strDes = [NSString stringWithUTF8String:strDesFilePath.c_str()];
            BOOL isDesDirExist = [fileManager fileExistsAtPath:strDes isDirectory:&isDesDir];
            BOOL bCreateDir = TRUE;
            if (!(isDesDirExist && isDesDir))
            {
                bCreateDir = [fileManager createDirectoryAtPath:strDes withIntermediateDirectories:YES attributes:nil error:nil];
            }
            if(!bCreateDir)
            {
                BEATS_PRINT("create %s failed", strDesFilePath.c_str());
            }
        }
        else
        {
            m_uExtractedSize += fileSizeAtPath(strSrcFilePath);
            CopyOneFile(strSrcFilePath, strDesFilePath);
        }
    }
}

bool CExtractIosAssets::CopyOneFile(const std::string& strSrcPath, const std::string& strDesPath)
{
    CSerializer fileSerializer;
    fileSerializer.Reset();
    CFilePathTool::GetInstance()->LoadFile(&fileSerializer, strSrcPath.c_str(), "rb");
	bool bSuccess = fileSerializer.Deserialize(strDesPath.c_str());
	BEATS_PRINT("Extract %s %s\n", strSrcPath.c_str(), bSuccess ? "Success" : "Failed");
    return bSuccess;
}

#endif
