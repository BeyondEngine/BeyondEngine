#include "stdafx.h"
#include "Resource.h"
#include "ResourceManager.h"

CResource::CResource()
    : m_pData(NULL)
{

}

CResource::~CResource()
{
    if (IsLoaded())
    {
        Unload();
    }
    if (IsInitialized())
    {
        Uninitialize();
    }
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
    m_strPath.m_value = str;
}

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
            DeserializeVariable(m_strPath, pSerializer);
            if (!m_strPath.m_value.empty())
            {
                CResourceManager::GetInstance()->LoadResource(this, m_strPath.m_value.c_str());
            }
            bRet = true;
        }
    }
    return bRet;
}


