#include "stdafx.h"
#include "MoveData.h"

using namespace _2DSkeletalAnimation;

CMoveData::CMoveData()
{

}

CMoveData::~CMoveData()
{
    for (auto &item : m_movBoneDataMap)
    {
        SMoveBoneData* pBonedata = item.second;
        for (auto &pFramedata : pBonedata->frameList)
        {
            BEATS_SAFE_DELETE(pFramedata);
        }
        pBonedata->frameList.clear();
        BEATS_SAFE_DELETE(pBonedata);
    }
    m_movBoneDataMap.clear();
}

void CMoveData::AddMoveBoneData(SMoveBoneData *boneData)
{
    m_movBoneDataMap[boneData->name] = boneData;
}

SMoveBoneData *CMoveData::GetMovBoneData(const std::string boneName)
{
    SMoveBoneData* pRet = nullptr;
    TMoveBoneDataMap ::const_iterator iter = m_movBoneDataMap.find(boneName);
    if (iter != m_movBoneDataMap.end())
    {
        pRet = iter->second;
    }
    return pRet;
}

const CMoveData::TMoveBoneDataMap& CMoveData::GetMoveBoneDataMap() const
{
    return m_movBoneDataMap;
}

const std::string& CMoveData::GetName() const
{
    return m_strName;
}

void CMoveData::SetName(const std::string& name)
{
    m_strName = name;
}

int CMoveData::GetDuration() const
{
    return m_iDuration;
}

void CMoveData::SetDuration(int duration)
{
    m_iDuration = duration;
}

int CMoveData::GetDurationTo() const
{
    return m_iDurationTo;
}

void CMoveData::SetDurationTo(int durationto)
{
    m_iDurationTo = durationto;
}

int CMoveData::GetDurationTween() const
{
    return m_iDurationTween;
}

void CMoveData::SetDuraionTween(int durationtween)
{
    m_iDurationTween = durationtween;
}

bool CMoveData::GetLoop() const
{
    return m_bLoop;
}

void CMoveData::SetLoop(bool bLoop)
{
    m_bLoop = bLoop;
}
