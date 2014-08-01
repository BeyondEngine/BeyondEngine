#include "stdafx.h"
#include "Skeleton.h"
#include "SkeletonBone.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "RenderManager.h"
#include "ShaderProgram.h"
#include "Resource/ResourceManager.h"

CSkeleton::CSkeleton()
{

}

CSkeleton::~CSkeleton()
{

}

SharePtr<CSkeletonBone> CSkeleton::GetSkeletonBone(ESkeletonBoneType type) const
{
    SharePtr<CSkeletonBone> pRet;
    std::map<ESkeletonBoneType, SharePtr<CSkeletonBone> >::const_iterator iter = m_bonesMap.find(type);
    BEATS_ASSERT(iter != m_bonesMap.end(), _T("Can't find bones with type %d %s"), type, pszSkeletonBoneName[type]);

    if (iter != m_bonesMap.end())
    {
        BEATS_ASSERT(iter->second->GetBoneType() == type);
        pRet = iter->second;
    }
    return pRet;
}

bool CSkeleton::Load()
{
    bool bRet = true;
    // Load From File
    const TString& strFilePath = GetFilePath();
    if (!strFilePath.empty())
    {
        CSerializer serializer(strFilePath.c_str());
        size_t uBoneCount = 0;
        serializer >> uBoneCount;
        for (size_t i = 0; i < uBoneCount; ++i)
        {
            ESkeletonBoneType boneType = eSBT_Count;
            serializer >> boneType;
            BEATS_ASSERT(boneType < eSBT_Count, _T("Invalid bone type!"));
            ESkeletonBoneType boneParentType = eSBT_Count;
            serializer >> boneParentType;
            BEATS_ASSERT(boneParentType < eSBT_Count, _T("Invalid bone type!"));

            SharePtr<CSkeletonBone> pNullParentBone;
            SharePtr<CSkeletonBone> _pSkeletonBone = new CSkeletonBone(pNullParentBone, boneType);
            _pSkeletonBone->SetParentType(boneParentType);
            kmMat4  _TPosMatrix;
            serializer >> _TPosMatrix.mat[0] >> _TPosMatrix.mat[1] >> _TPosMatrix.mat[2] >> _TPosMatrix.mat[3] 
            >> _TPosMatrix.mat[4] >> _TPosMatrix.mat[5] >> _TPosMatrix.mat[6] >> _TPosMatrix.mat[7]
            >> _TPosMatrix.mat[8] >> _TPosMatrix.mat[9] >> _TPosMatrix.mat[10] >> _TPosMatrix.mat[11]
            >> _TPosMatrix.mat[12] >> _TPosMatrix.mat[13] >> _TPosMatrix.mat[14] >> _TPosMatrix.mat[15];
            _pSkeletonBone->SetTPosMatrix( _TPosMatrix );
            m_bonesMap[boneType] = _pSkeletonBone;
        }

        //Fill the parent
        std::map<ESkeletonBoneType, SharePtr<CSkeletonBone>>::iterator iter = m_bonesMap.begin();
        for (; iter != m_bonesMap.end(); ++iter)
        {
            ESkeletonBoneType parentType = iter->second->GetParentType();
            if (parentType != eSBT_Null)
            {
                std::map<ESkeletonBoneType, SharePtr<CSkeletonBone>>::iterator parentIter = m_bonesMap.find(parentType);
                BEATS_ASSERT(parentIter != m_bonesMap.end());
                iter->second->SetParent(parentIter->second);
            }
        }
    }
    super::Load();
    return bRet;
}


std::map<ESkeletonBoneType, SharePtr<CSkeletonBone>>& CSkeleton::GetBoneMap()
{
    return m_bonesMap;
}

