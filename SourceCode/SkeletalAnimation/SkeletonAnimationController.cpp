#include "stdafx.h"
#include "SkeletonAnimationController.h"
#include "MoveData.h"
#include "BoneData.h"
#include "AnimationData.h"
#include <math.h>
#include "Utility/BeatsUtility/StringHelper.h"
#include "SkeletonAnimationManager.h"

using namespace _2DSkeletalAnimation;

CSkeletonAnimationController::CSkeletonAnimationController()
    : m_bLoop(true)
    , m_uLoopCount(0)
    , m_bPlaying(false)
    , m_fPlayingTime(0.f)
    , m_fBlendTime(0.5f)
    , m_fDuration(0.f)
    , m_uFps(24)
    , m_pPreMove(nullptr)
    , m_pCurrMove(nullptr)
{

}

CSkeletonAnimationController::~CSkeletonAnimationController()
{

}

void CSkeletonAnimationController::ReflectData( CSerializer& serializer )
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bPlaying, true, 0xFFFFFFFF, _T("是否播放"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bLoop, true, 0xFFFFFFFF, _T("是否循环"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fBlendTime, true, 0xFFFFFFFF, _T("动画混合时间"), NULL, NULL, NULL);
}

bool CSkeletonAnimationController::OnPropertyChange( void* pVariableAddr, CSerializer* pSerializer )
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pSerializer);
    if(pVariableAddr == &m_bPlaying)
    {
        DeserializeVariable(m_bPlaying, pSerializer);
        if(m_bPlaying)
        {
            Play();
        }
        else
        {
            Stop();
        }
        bRet = true;
    }
    return bRet;
}

void CSkeletonAnimationController::Initialize()
{
    super::Initialize();
    CSkeletonAnimationManager::GetInstance()->RegisterController(this);
}

void CSkeletonAnimationController::Uninitialize()
{
    super::Uninitialize();
    CSkeletonAnimationManager::GetInstance()->UnRegisterController(this);
}

void CSkeletonAnimationController::SetMoveData(CMoveData* pMoveData)
{
    BEATS_ASSERT(pMoveData);
    m_pPreMove = m_pCurrMove;
    m_pCurrMove = pMoveData;
    m_fDuration = (float)(m_pCurrMove->GetDuration()) / (float)m_uFps;
    m_uFrameCount = (size_t)m_pCurrMove->GetDuration();
    m_uLoopCount = 0;
}

const CMoveData* CSkeletonAnimationController::GetCurrMove() const
{
    return m_pCurrMove;
}

void CSkeletonAnimationController::Play()
{
    m_fPlayingTime = 0.f;
    m_bPlaying = true;
    m_uLoopCount = 0;
}

void CSkeletonAnimationController::Update(float fDeltaTime)
{
    if(m_bPlaying)
    {
        m_fPlayingTime += fDeltaTime;
        if (m_fPlayingTime > m_fDuration)
        {
            if(m_bLoop)
            {
                m_fPlayingTime = fmod(m_fPlayingTime, m_fDuration);
                m_uLoopCount++;
            }
            else
            {
                m_fPlayingTime = m_fDuration;
            }
        }
    }
}

void CSkeletonAnimationController::GoToFrame(size_t frame)
{
    if(m_pCurrMove)
    {
        m_fPlayingTime = (float)frame / m_uFps;
        m_bPlaying = false;
    }
}
void CSkeletonAnimationController::Pause()
{
    m_bPlaying= false;
}
void CSkeletonAnimationController::Resume()
{
    if(m_pCurrMove)
    {
        m_bPlaying = true;
    }
}
void CSkeletonAnimationController::Stop()
{
    m_fPlayingTime = 0;
    m_bPlaying = false;
}
bool CSkeletonAnimationController::IsPlaying()
{
    return m_bPlaying;
}

void CSkeletonAnimationController::Restore()
{
    Stop();
    m_pPreMove = nullptr;
    m_pCurrMove = nullptr;
}

size_t CSkeletonAnimationController::GetCurrFrame()
{
    size_t iFrame = (size_t)(m_uFps * m_fPlayingTime);
    if(iFrame >= m_uFrameCount)
        iFrame = m_uFrameCount -1;
    return iFrame;
}

CSkeletonAnimationController::TFrameDataMap& CSkeletonAnimationController::GetCurrFrameBonesData()
{
    m_frameDataMap.clear();
    if(IsBlendState())
    {
        TFrameDataMap preFrameDataMap;
        TFrameDataMap currFrameDataMap;
        GetMoveFrameData(preFrameDataMap, m_pPreMove);
        if(preFrameDataMap.empty())
        {
            GetMoveFrameData(m_frameDataMap, m_pCurrMove);
        }
        else
        {
            GetMoveFrameData(currFrameDataMap, m_pCurrMove);
            Interpolation(preFrameDataMap, currFrameDataMap);
        }
    }
    else
    {
        GetMoveFrameData(m_frameDataMap, m_pCurrMove);
    }
    return m_frameDataMap;
}

void CSkeletonAnimationController::InsertFrameData(SFrameData& result,  const SFrameData& beforeData, const SFrameData& afterData, int iFrame)
{
    float percent = (float)(iFrame - beforeData.frameID) / (float)(afterData.frameID - beforeData.frameID);
    result.frameID = iFrame;
    result.x = beforeData.x + ( afterData.x - beforeData.x) * percent;
    result.y = beforeData.y + ( afterData.y - beforeData.y) * percent;

    result.skewX = beforeData.skewX + ( afterData.skewX - beforeData.skewX) * percent;
    result.skewY = beforeData.skewY + ( afterData.skewY - beforeData.skewY) * percent;

    result.scaleX = beforeData.scaleX + ( afterData.scaleX - beforeData.scaleX) * percent;
    result.scaleY = beforeData.scaleY + ( afterData.scaleY - beforeData.scaleY) * percent;
}

bool CSkeletonAnimationController::IsBlendState()
{
    return m_pPreMove && m_pCurrMove && m_fPlayingTime < m_fBlendTime 
        && m_uLoopCount < 1;
}

void CSkeletonAnimationController::GetMoveFrameData(TFrameDataMap& frameDataMap, CMoveData* pMoveData)
{
    return GetFrameDataByFrame(frameDataMap,pMoveData,GetCurrFrame());
}

void CSkeletonAnimationController::GetFrameDataByFrame(TFrameDataMap& frameDataMap, CMoveData* pMoveData, int iFrame)
{
    BEATS_ASSERT(pMoveData);
    frameDataMap.clear();
    const std::map<std::string, SMoveBoneData*>& moveBoneDataMap = pMoveData->GetMoveBoneDataMap();
    for(auto iter : moveBoneDataMap)
    {
        std::string boneName = iter.first;
        SMoveBoneData* pBoneData = iter.second;
        for(size_t i = 0; i<pBoneData->frameList.size(); ++i)
        {
            SFrameData* framedata = pBoneData->frameList[i];
            SFrameData* next =nullptr;
            if( i +1 < pBoneData->frameList.size())
            {
                next = pBoneData->frameList[i+1];
            }

            if(framedata->frameID == iFrame && framedata->dI != -1)
            {
                SFrameData data;
                data.dI = framedata->dI;
                data.frameID = framedata->frameID;
                data.x = framedata->x;
                data.y = framedata->y;
                data.skewX = framedata->skewX;
                data.skewY = framedata->skewY;
                data.scaleX = framedata->scaleX;
                data.scaleY = framedata->scaleY;
                frameDataMap[boneName] = data;
                break;
            }
            else if(boneName.compare("effect") != 0 && iFrame > framedata->frameID 
                && next && next->frameID > iFrame)
            {
                SFrameData data;
                data.dI = framedata->dI;
                InsertFrameData(data,  *framedata, *next, iFrame);
                frameDataMap[boneName] = data;
                break;
            }
        }
    }
}

void CSkeletonAnimationController::Interpolation(const TFrameDataMap& preFrameDataMap, const TFrameDataMap& currFrameDataMap)
{
    float fWeight = m_fPlayingTime / m_fBlendTime;
    for (auto& item : preFrameDataMap)
    {
        const SFrameData& preFrameData = item.second;
        TFrameDataMap ::const_iterator iter = currFrameDataMap.find(item.first);
        if (iter != currFrameDataMap.end())
        {
            const SFrameData& currFrameData = iter->second;
            SFrameData framedata;
            framedata.dI = currFrameData.dI;
            framedata.frameID = currFrameData.frameID;
            framedata.x = preFrameData.x + ( currFrameData.x - preFrameData.x) * fWeight;
            framedata.y = preFrameData.y + ( currFrameData.y - preFrameData.y) * fWeight;
            framedata.scaleX = preFrameData.scaleX + (currFrameData.scaleX - preFrameData.scaleX) * fWeight;
            framedata.scaleY = preFrameData.scaleY + (currFrameData.scaleY - preFrameData.scaleY) * fWeight;
            framedata.skewX = preFrameData.skewX + (currFrameData.skewX - preFrameData.skewX) * fWeight;
            framedata.skewY = preFrameData.skewY + (currFrameData.skewY - preFrameData.skewY) * fWeight;
            m_frameDataMap[item.first] = framedata;
        }
    }
}
