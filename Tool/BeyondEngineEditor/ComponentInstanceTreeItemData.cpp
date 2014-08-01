#include "stdafx.h"
#include "ComponentInstanceTreeItemData.h"

CComponentInstanceTreeItemData::CComponentInstanceTreeItemData(CComponentBase* pComponentBase)
    : m_pComponentBase(pComponentBase)
{

}
CComponentInstanceTreeItemData::~CComponentInstanceTreeItemData()
{

}
bool CComponentInstanceTreeItemData::IsDirectory()
{
    return m_pComponentBase == NULL;
}

CComponentBase* CComponentInstanceTreeItemData::GetComponentBase() const
{
    return m_pComponentBase;
}