#include "stdafx.h"
#include "PlayerPrefs.h"
#include "Resource/ResourceManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"

CPlayerPrefs* CPlayerPrefs::m_pInstance = nullptr;

CPlayerPrefs::CPlayerPrefs()
{
    Load();
}

CPlayerPrefs::~CPlayerPrefs()
{
}

TString CPlayerPrefs::GetString(const TString& strKey)
{
    TString strRet;
    if (m_dataMap.find(strKey) != m_dataMap.end())
    {
        strRet = m_dataMap[strKey];
    }
    return strRet;
}

void CPlayerPrefs::SetString(const TString& strKey, const TString& strValue)
{
    m_dataMap[strKey] = strValue;
}

void CPlayerPrefs::Save()
{
    if (m_strSavePath.empty())
    {
        m_strSavePath = CResourceManager::GetInstance()->GetPersistentDataPath().append(_T("/")).append(_T("PlayerPrefs"));
    }
    CSerializer serializer;
    uint32_t uMapSize = m_dataMap.size();
    serializer << uMapSize;
    for (auto itr : m_dataMap)
    {
        serializer << itr.first;
        serializer << itr.second;
    }
    serializer.Deserialize(m_strSavePath.c_str());
    serializer.Reset();
}

void CPlayerPrefs::Load()
{
    if (m_strSavePath.empty())
    {
        m_strSavePath = CResourceManager::GetInstance()->GetPersistentDataPath().append(_T("/")).append(_T("PlayerPrefs"));
    }
    if (CFilePathTool::GetInstance()->Exists(m_strSavePath.c_str()))
    {
        CSerializer serializer;
        serializer.Serialize(m_strSavePath.c_str());
        uint32_t uMapSize = 0;
        serializer >> uMapSize;
        for (uint32_t i = 0; i < uMapSize; ++i)
        {
            TString strKey;
            TString strValue;
            serializer >> strKey;
            serializer >> strValue;
            m_dataMap[strKey] = strValue;
        }
    }
}

void CPlayerPrefs::ClearDataMap()
{
    m_dataMap.clear();
}
