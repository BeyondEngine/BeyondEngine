#include "stdafx.h"
#include "ComponentFileTreeItemData.h"

CComponentFileTreeItemData::CComponentFileTreeItemData(CComponentProjectDirectory* pData, const TString& fileName)
    : m_pData(pData)
    , m_strFileName(fileName)
{
}

CComponentFileTreeItemData::~CComponentFileTreeItemData()
{
}

bool CComponentFileTreeItemData::IsDirectory()
{
    return m_pData != NULL;
}

const TString& CComponentFileTreeItemData::GetFileName() const
{
    return m_strFileName;
}

CComponentProjectDirectory* CComponentFileTreeItemData::GetProjectDirectory() const
{
    return m_pData;
}

void CComponentFileTreeItemData::SetData(CComponentProjectDirectory* pData)
{
    m_pData = pData;
}