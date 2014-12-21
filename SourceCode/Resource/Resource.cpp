#include "stdafx.h"
#include "Resource.h"
#include "ResourceManager.h"

CResource::CResource()
    : m_pData(NULL)
{

}

CResource::~CResource()
{
    BEATS_SAFE_DELETE(m_pData);
}

void CResource::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_strPath, true, 0xFF0000FF, _T("文件路径"), NULL, NULL, NULL);
}

const TString& CResource::GetFilePath() const
{
    return m_strPath.m_value;
}

void CResource::SetFilePath(const TString& str)
{
#ifdef DEVELOP_VERSION
    FILE* pFile = fopen(str.c_str(), "rb");
    if (pFile)
    {
        fseek(pFile, 0, SEEK_END);
        m_uFileSize = ftell(pFile);
        fclose(pFile);
    }
#endif
    m_strPath.m_value = str;
}

bool CResource::ShouldClean() const
{
    SharePtr<CResource> this_sharePtr;
    CResourceManager::GetInstance()->QueryResource(GetFilePath(), this_sharePtr);
    return this_sharePtr.RefCount() == CResourceManager::MIN_RESOURCE_REF_COUNT;
}
#ifdef EDITOR_MODE
bool CResource::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pSerializer);
    if (!bRet)
    {
        if (pVariableAddr == &m_strPath)
        {
            if (!m_strPath.m_value.empty()) //change path
            {
                SharePtr<CResource> pRet;
                bool bResourceExists = CResourceManager::GetInstance()->QueryResource(m_strPath.m_value, pRet);
                if (bResourceExists)
                {
                    BEATS_ASSERT(pRet.Get() == this);
                    CResourceManager::GetInstance()->UnregisterResource(pRet);
                }
            }
            Unload();
            Uninitialize();
            DeserializeVariable(m_strPath, pSerializer, this);
            if (!m_strPath.m_value.empty())
            {
                CResourceManager::GetInstance()->LoadResource(this, m_strPath.m_value.c_str());
            }
            bRet = true;
        }
    }
    return bRet;
}
#endif
#ifdef DEVELOP_VERSION
TString CResource::GetDescription() const
{
    TString strDescription = _T("Type: ");
    strDescription.append(pszResourceTypeString[GetType()]).append(", ");
    strDescription.append(_T("Path: "));
    strDescription.append(m_strPath.m_value).append(", ");
    strDescription.append(_T("RefCount: "));
    SharePtr<CResource> this_sharePtr;
    CResourceManager::GetInstance()->QueryResource(GetFilePath(), this_sharePtr);
    TCHAR szBuffer[MAX_PATH];
    _stprintf(szBuffer, _T("%d, "), this_sharePtr.RefCount() - 1);
    strDescription.append(szBuffer);
    _stprintf(szBuffer, _T("LoadTime: %dms, InitTime: %dms, DelayInit:%s, "), m_uLoadTimeMS, m_uInitializeTimeMS, m_bDelayInitialize?"true":"false");
    strDescription.append(szBuffer);
    return strDescription;
}
#endif

#ifdef EDITOR_MODE
void CResource::Reload()
{
    WIN32_FILE_ATTRIBUTE_DATA lpinf;
    GetFileAttributesEx(GetFilePath().c_str(), GetFileExInfoStandard, &lpinf);
    m_uLastModifyTimeLow = lpinf.ftLastWriteTime.dwLowDateTime;
    m_uLastModifyTimeHigh = lpinf.ftLastWriteTime.dwHighDateTime;
}

bool CResource::NeedReload() const
{
    WIN32_FILE_ATTRIBUTE_DATA lpinf;
    GetFileAttributesEx(GetFilePath().c_str(), GetFileExInfoStandard, &lpinf);
    return (lpinf.ftLastWriteTime.dwLowDateTime != m_uLastModifyTimeLow ||
        lpinf.ftLastWriteTime.dwHighDateTime != m_uLastModifyTimeHigh);
}
#endif