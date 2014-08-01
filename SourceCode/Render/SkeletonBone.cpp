#include "stdafx.h"
#include "SkeletonBone.h"

CSkeletonBone::CSkeletonBone( SharePtr<CSkeletonBone> pParent, ESkeletonBoneType type )
    : m_bVisible(false)
    , m_bCoordinateVisible(false)
    , m_pParent(pParent)
    , m_type(type)
{
}

CSkeletonBone::~CSkeletonBone()
{

}

ESkeletonBoneType CSkeletonBone::GetBoneType() const
{
    return m_type; 
}

const kmMat4& CSkeletonBone::GetTPosMatrix() const
{
    return m_TPosMatrix;
}

void CSkeletonBone::SetTPosMatrix( const kmMat4& matrix )
{
    m_TPosMatrix = matrix;
}

void CSkeletonBone::SetVisible(bool bRender)
{
    m_bVisible = bRender;
}

bool CSkeletonBone::GetVisible()const
{
    return m_bVisible;
}

void CSkeletonBone::SetCoordinateVisible(bool bRender)
{
    m_bCoordinateVisible = bRender;
}

bool CSkeletonBone::GetCoordinateVisible()const
{
    return m_bCoordinateVisible;
}

SharePtr<CSkeletonBone> CSkeletonBone::GetParent()
{
    return m_pParent;
}

void CSkeletonBone::SetParent(SharePtr<CSkeletonBone> parent)
{
    m_pParent = parent;
}

void CSkeletonBone::SetParentType(ESkeletonBoneType type)
{
    m_parentType = type;
}

ESkeletonBoneType CSkeletonBone::GetParentType() const
{
    return m_parentType;
}

const std::vector<SharePtr<CSkeletonBone>>& CSkeletonBone::GetChild() const
{
    return m_child;
}
