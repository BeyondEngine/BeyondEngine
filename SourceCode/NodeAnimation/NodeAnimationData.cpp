#include "stdafx.h"
#include "NodeAnimationData.h"
#include "NodeAnimationElement.h"
#include "NodeAnimationManager.h"
#include "Scene\Node.h"

CNodeAnimationData::CNodeAnimationData()
    : m_uFrameCount(0)
{

}

CNodeAnimationData::~CNodeAnimationData()
{

}

void CNodeAnimationData::Initialize()
{
    super::Initialize();
    if (!m_strName.empty())
    {
        CNodeAnimationManager::GetInstance()->RegisterNodeAnimationData(this);
    }
}

void CNodeAnimationData::Uninitialize()
{
    super::Uninitialize();
    CNodeAnimationManager::GetInstance()->UnregisterNodeAnimationData(this);
}

void CNodeAnimationData::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_strName, true, 0xFFFFFFFF, _T("名称"), NULL, _T("名称必须是唯一的"), NULL);
    DECLARE_PROPERTY(serializer, m_uFrameCount, true, 0xFFFFFFFF, _T("总帧数"), NULL, NULL, _T("MinValue:0"));
    DECLARE_PROPERTY(serializer, m_elements, true, 0xFFFFFFFF, _T("动画元素"), NULL, NULL, NULL);
}

bool CNodeAnimationData::OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pNewValueToBeSet);
    if (!bRet)
    {
        if (pVariableAddr == &m_strName)
        {
            TString strName;
            DeserializeVariable(strName, pNewValueToBeSet);
            SetName(strName);
            bRet = true;
        }
    }
    return bRet;
}


void CNodeAnimationData::AddElements(CNodeAnimationElement* pElement)
{
    BEATS_ASSERT(pElement != NULL);
    m_elements.push_back(pElement);
}

size_t CNodeAnimationData::GetFrameCount() const
{
    return m_uFrameCount;
}

void CNodeAnimationData::SetFrameCount(size_t uFrameCount)
{
    m_uFrameCount = uFrameCount;
}

void CNodeAnimationData::Apply(CNode* pNode, size_t uCurrFrame)
{
    BEATS_ASSERT(uCurrFrame < m_uFrameCount);
    bool bNeedUpdateAnimationProperty = false;;
    for (size_t i = 0; i < m_elements.size(); ++i)
    {
        m_elements[i]->Apply(pNode, uCurrFrame);
        ENodeAnimationElementType type = m_elements[i]->GetType();
        bNeedUpdateAnimationProperty = bNeedUpdateAnimationProperty || (type == eNAET_Scale || type == eNAET_Pos || type == eNAET_Rotation);
    }
    if (bNeedUpdateAnimationProperty)
    {
        pNode->InvalidateLocalTM();
#ifdef EDITOR_MODE
        if (pNode->GetProxyComponent())
        {
            const std::vector<CComponentInstance*>& syncNodes = pNode->GetProxyComponent()->GetSyncComponents();
            for (size_t i = 0; i < syncNodes.size(); ++i)
            {
                ((CNode*)syncNodes[i])->InvalidateLocalTM();
            }
        }
#endif
    }
}

const std::vector<CNodeAnimationElement*>& CNodeAnimationData::GetElements() const
{
    return m_elements;
}

void CNodeAnimationData::SetName(const TString& strName)
{
    if (m_strName.compare(strName) != 0)
    {
        if (!m_strName.empty())
        {
            CNodeAnimationManager::GetInstance()->UnregisterNodeAnimationData(this);
        }
        m_strName = strName;
        if (!strName.empty())
        {
            CNodeAnimationManager::GetInstance()->RegisterNodeAnimationData(this);
        }
    }
}

const TString& CNodeAnimationData::GetName() const
{
    return m_strName;
}
