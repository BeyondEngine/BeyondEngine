#include "stdafx.h"
#include "Skeleton.h"
#include "SkeletonBone.h"

CSkeleton::CSkeleton()
{

}

CSkeleton::~CSkeleton()
{
    BEATS_SAFE_DELETE_MAP(m_bonesIdMap);
}

CSkeletonBone* CSkeleton::GetSkeletonBoneById(uint8_t uIndex) const
{
    CSkeletonBone* pRet = nullptr;
    std::map<uint8_t, CSkeletonBone* >::const_iterator iter = m_bonesIdMap.find(uIndex);
    BEATS_ASSERT(iter != m_bonesIdMap.end(), _T("Can't find bones with type %d %s"), uIndex, GetBoneName(uIndex).c_str());

    if (iter != m_bonesIdMap.end())
    {
        BEATS_ASSERT(iter->second->GetIndex() == uIndex);
        pRet = iter->second;
    }
    return pRet;
}

CSkeletonBone* CSkeleton::GetSkeletonBoneByName(const TString& strName) const
{
    CSkeletonBone* pRet = nullptr;
    uint8_t uIndex = GetBoneIndex(strName);
    ASSUME_VARIABLE_IN_EDITOR_BEGIN(uIndex != 0xFF)
        pRet = GetSkeletonBoneById(uIndex);
    ASSUME_VARIABLE_IN_EDITOR_END
    return pRet;
}

bool CSkeleton::Load(CSerializer& serializer)
{
    bool bRet = true;

    BEATS_ASSERT(serializer.GetWritePos() > 0);
    uint32_t uBoneCount = 0;
    // key is the child id, value is the parent id.
    std::map<uint8_t, uint8_t> childToParentMap;
    std::map<CSkeletonBone*, CLocation> localLocation;
    serializer >> uBoneCount;
    // NOTICE: you have to get this self by this way, so we can get a share pointer.
    for (uint32_t i = 0; i < uBoneCount; ++i)
    {
        TString strBoneName;
        serializer >> strBoneName;
        uint8_t boneId = 0xFF;
        serializer >> boneId;
        BEATS_ASSERT(boneId != 0xFF, _T("Invalid bone type!"));
        uint8_t boneParentId = 0xFF;
        serializer >> boneParentId;

        BEATS_ASSERT(childToParentMap.find(boneId) == childToParentMap.end(), _T("Bone %d should have only one parent %d but we are trying to add %d as its parent."), boneId, childToParentMap[boneId], boneParentId);
        childToParentMap[boneId] = boneParentId;
        CSkeletonBone* _pSkeletonBone = new CSkeletonBone(boneId, strBoneName, this);
        CLocation location;
        serializer >> location.m_scale >> location.m_rotation >> location.m_pos;
        localLocation[_pSkeletonBone] = location;
        m_bonesIdMap[boneId] = _pSkeletonBone;
    }
    //Fill the parent
    for (auto iter = childToParentMap.begin(); iter != childToParentMap.end(); ++iter)
    {
        BEATS_ASSERT(m_bonesIdMap.find(iter->first) != m_bonesIdMap.end());
        if (iter->second != 0xFF)
        {
            BEATS_ASSERT(m_bonesIdMap.find(iter->second) != m_bonesIdMap.end());
            m_bonesIdMap[iter->first]->SetParent(m_bonesIdMap[iter->second]);
        }
    }
    for (auto iter = localLocation.begin(); iter != localLocation.end(); ++iter)
    {
        CMat4 worldTPosMat(iter->second);
        CSkeletonBone* pParent = iter->first->GetParent();
        while (pParent != nullptr)
        {
            worldTPosMat = CMat4(localLocation[pParent]) * worldTPosMat;
            pParent = pParent->GetParent();
        }
        iter->first->SetTPosWorldTM(worldTPosMat);
    }
    return bRet;
}

uint8_t CSkeleton::GetFPS()const
{
    return m_uFPS;
}

void CSkeleton::SetFPS(uint8_t uFps)
{
    m_uFPS = uFps;
}

#ifdef DEVELOP_VERSION
void CSkeleton::PrintBoneTree(CSkeletonBone* pBone, uint32_t level)
{
    if (pBone == nullptr)
    {
        for (auto iter = m_bonesIdMap.begin(); iter != m_bonesIdMap.end(); ++iter)
        {
            CSkeletonBone* pRootBone = iter->second;
            if (pRootBone->GetParent() == nullptr)
            {
                BEATS_ASSERT(iter->first == pRootBone->GetIndex());
                BEATS_PRINT("Root Bone: %s id:%d\n", pRootBone->GetName().c_str(), pRootBone->GetIndex());
                const std::vector<CSkeletonBone*>& childrenBone = pRootBone->GetChildrenBone();
                for (size_t i = 0; i < childrenBone.size(); ++i)
                {
                    BEATS_ASSERT(childrenBone[i] != nullptr);
                    PrintBoneTree(childrenBone[i], 1);
                }
            }
        }
    }
    else
    {
        uint32_t uSpaceCount = level;
        while (uSpaceCount > 0)
        {
            BEATS_PRINT("    ");
            uSpaceCount -= 1;
        }
        BEATS_PRINT("Bone: %s id:%d\n", pBone->GetName().c_str(), pBone->GetIndex());
        const std::vector<CSkeletonBone*>& childrenBone = pBone->GetChildrenBone();
        for (size_t i = 0; i < childrenBone.size(); ++i)
        {
            BEATS_ASSERT(childrenBone[i] != nullptr);
            PrintBoneTree(childrenBone[i], level + 1);
        }
    }
}
#endif

const std::map<uint8_t, CSkeletonBone*>& CSkeleton::GetBoneMap() const
{
    return m_bonesIdMap;
}

const TString& CSkeleton::GetBoneName(uint8_t uIndex) const
{
    auto iter = m_bonesIdMap.find(uIndex);
    BEATS_ASSERT(iter != m_bonesIdMap.end());
    return iter->second->GetName();
}

uint8_t CSkeleton::GetBoneIndex(const TString& strName) const
{
    uint8_t uRet = 0xFF;
    for (auto iter = m_bonesIdMap.begin(); iter != m_bonesIdMap.end(); ++iter)
    {
        if (iter->second->GetName() == strName)
        {
            uRet = iter->first;
            break;
        }
    }
    return uRet;
}