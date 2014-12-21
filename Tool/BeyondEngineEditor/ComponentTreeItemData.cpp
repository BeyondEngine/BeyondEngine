#include "stdafx.h"
#include "ComponentTreeItemData.h"

CComponentTreeItemData::CComponentTreeItemData(bool bIsDirectory, uint32_t guid)
    : m_bIsDirectory(bIsDirectory)
    , m_uComponentGUID(guid)
{
}

CComponentTreeItemData::~CComponentTreeItemData()
{
}

bool CComponentTreeItemData::IsDirectory()
{
    return m_bIsDirectory;
}

uint32_t CComponentTreeItemData::GetGUID()
{
    return m_uComponentGUID;
}