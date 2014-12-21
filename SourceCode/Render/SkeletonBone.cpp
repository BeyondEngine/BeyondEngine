#include "stdafx.h"
#include "SkeletonBone.h"
#include "Skeleton.h"

CSkeletonBone::CSkeletonBone(uint8_t uIndex, const TString& strName, CSkeleton* /*pSkeleton*/)
    : m_bVisible(false)
    , m_bCoordinateVisible(false)
    , m_uIndex(uIndex)
    , m_strName(strName)
    , m_pParent(nullptr)
{
}

CSkeletonBone::~CSkeletonBone()
{

}

void CSkeletonBone::SetVisible(bool bRender)
{
    m_bVisible = bRender;
}

bool CSkeletonBone::GetVisible()const
{
    return m_bVisible;
}

bool CSkeletonBone::GetCoordinateVisible()const
{
    return m_bCoordinateVisible;
}

void CSkeletonBone::SetCoordinateVisible(bool bRender)
{
    m_bCoordinateVisible = bRender;
}

void CSkeletonBone::SetTPosWorldTM(const CMat4& worldTM)
{
    m_TPosWorldTM = worldTM;
    SetCurrentWorldTM(m_TPosWorldTM);
}

const CMat4& CSkeletonBone::GetTPosWorldTM()
{
    return m_TPosWorldTM;
}

const CMat4& CSkeletonBone::GetCurrentWorldTM()
{
    return m_currWorldTM;
}

void CSkeletonBone::SetCurrentWorldTM(const CMat4& mat)
{
    m_currWorldTM = mat;
}

CSkeletonBone* CSkeletonBone::GetParent() const
{
    return m_pParent;
}

void CSkeletonBone::SetParent(CSkeletonBone* parent)
{
    BEATS_ASSERT(m_pParent == NULL);
    m_pParent = parent;
    m_pParent->AddChildBone(this);
}

const std::vector<CSkeletonBone*>& CSkeletonBone::GetChildrenBone() const
{
    return m_childrenBone;
}

void CSkeletonBone::AddChildBone(CSkeletonBone* pChild)
{
    m_childrenBone.push_back(pChild);
}

uint8_t CSkeletonBone::GetIndex() const
{
    return m_uIndex;
}

const TString& CSkeletonBone::GetName() const
{
    return m_strName;
}