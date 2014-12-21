#include "stdafx.h"
#include "ComponentProxyTreeItemData.h"

CComponentProxyTreeItemData::CComponentProxyTreeItemData(CComponentProxy* pComponentProxy)
: m_pComponentProxy(pComponentProxy)
{

}
CComponentProxyTreeItemData::~CComponentProxyTreeItemData()
{

}
bool CComponentProxyTreeItemData::IsDirectory()
{
    return m_pComponentProxy == NULL;
}

CComponentProxy* CComponentProxyTreeItemData::GetComponentProxy() const
{
    return m_pComponentProxy;
}