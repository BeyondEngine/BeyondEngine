#include "stdafx.h"
#include "Animation3D.h"
#include "Utility/BeatsUtility/Serializer.h"

CAnimation3D::CAnimation3D()
    : m_uFrameCount(0)
    , m_uFPS(0)
    , m_fDuration(0.0F)
    , m_uCurFrame(0)
{

}

CAnimation3D::~CAnimation3D()
{

}

float CAnimation3D::Duration()
{
    return m_fDuration;
}

const std::vector<ESkeletonBoneType>& CAnimation3D::GetBones() const
{
    return m_bones;
}

size_t CAnimation3D::GetBoneIndexFromType(const ESkeletonBoneType type) const
{
    std::map<ESkeletonBoneType,size_t>::const_iterator iter = m_boneIndexMap.find(type);
    BEATS_ASSERT(iter != m_boneIndexMap.end());
    return iter->second;
}

const std::vector<kmMat4*>& CAnimation3D::GetBoneMatrixByFrame( size_t uFrame ) const
{
    BEATS_ASSERT(uFrame < m_uFrameCount);
    return m_posOfFrame[uFrame];
}

const std::vector<kmMat4*>& CAnimation3D::GetBoneMatrixByTime( float playtime ) const
{
    size_t currFrame = static_cast<size_t>(playtime * m_uFPS);
    currFrame %= m_uFrameCount;
    return GetBoneMatrixByFrame(currFrame);
}

size_t CAnimation3D::GetFPS() const
{
    return m_uFPS;
}

const kmMat4* CAnimation3D::GetOneBoneMatrixByTime(float fTime, ESkeletonBoneType boneType) const
{
    kmMat4* pRet = NULL;
    const std::vector<kmMat4*>& bones = GetBoneMatrixByTime(fTime);
    BEATS_ASSERT(m_bones.size() == bones.size());
    size_t boneIndex = GetBoneIndexFromType(boneType);
    pRet = bones[boneIndex];
    return pRet;
}

bool CAnimation3D::Load()
{
    bool bRet = true;
    const TString& strFileName = GetFilePath();
    if (!strFileName.empty())
    {
        CSerializer serializer(strFileName.c_str());
        size_t uBoneCount = 0;
        serializer >> uBoneCount;
        serializer >> m_uFPS;
        int nFrameCount = 0;
        serializer >> nFrameCount;
        m_uFrameCount = (unsigned short)nFrameCount;//Type cast!
        m_fDuration = (float)m_uFrameCount / m_uFPS;
        for ( size_t j = 0 ; j < uBoneCount ; ++ j )
        {
            ESkeletonBoneType boneType = eSBT_Count;
            serializer >> boneType;
            BEATS_ASSERT(boneType < eSBT_Count, _T("Invalid bone type!"));
            m_bones.push_back(boneType);
            m_boneIndexMap[boneType] = j;
        }

        for (size_t i = 0; i < m_uFrameCount; ++i)
        {
            std::vector<kmMat4*> _vMatix;
            for ( size_t j = 0 ; j < uBoneCount; ++j )
            {
                kmMat4* _TPosMatrix = new kmMat4();
                serializer >> _TPosMatrix->mat[0] >> _TPosMatrix->mat[1] >> _TPosMatrix->mat[2] >>_TPosMatrix->mat[3]
                >>_TPosMatrix->mat[4] >> _TPosMatrix->mat[5] >> _TPosMatrix->mat[6] >> _TPosMatrix->mat[7] >> _TPosMatrix->mat[8] 
                >> _TPosMatrix->mat[9] >> _TPosMatrix->mat[10] >> _TPosMatrix->mat[11] >> _TPosMatrix->mat[12] >> _TPosMatrix->mat[13]
                >> _TPosMatrix->mat[14] >> _TPosMatrix->mat[15] ;
                _vMatix.push_back( _TPosMatrix );
            }
            m_posOfFrame.push_back( _vMatix );
        }
    }
    super::Load();
    return bRet;
}

bool CAnimation3D::Unload()
{
    bool bRet = true;

    size_t uCount = m_posOfFrame.size();
    for ( size_t i = 0 ; i < uCount ; ++i )
    {
        size_t uMatixCount = m_posOfFrame[i].size();
        for ( size_t j = 0 ; j < uMatixCount; ++j )
        {
            BEATS_SAFE_DELETE( m_posOfFrame[i][j] );
        }
    }
    super::Unload();
    return bRet;
}

unsigned short CAnimation3D::GetFrameCount() const
{
    return m_uFrameCount;
}

void CAnimation3D::SetCurFrame(size_t curFrame)
{
   m_uCurFrame = curFrame;
}

size_t CAnimation3D::GetCurFrame() const
{
    return m_uCurFrame;
}