#include "stdafx.h"
#include "AnimationData.h"
#include "MoveData.h"
#include "Resource/ResourcePublic.h"
#include "Resource/ResourceManager.h"
#include "SkeletonAnimationController.h"
#include "AnimationImporter.h"

using namespace _2DSkeletalAnimation;

CAnimationData::CAnimationData()
    : m_pAnimationController(nullptr)
{
    m_strCfgFilePath.m_value = _T("tauren.ExportJson");
    LoadData();
}

CAnimationData::~CAnimationData()
{
    for (auto &item : m_movedataMap)
    {
        CMoveData* pMoveData = item.second;
        BEATS_SAFE_DELETE(pMoveData);
    }
}

void CAnimationData::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_strCfgFilePath, true, 0xFFFFFFFF, _T("资源文件"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_strMoveName, true, 0xFFFFFFFF, _T("动画列表"), NULL, NULL, GEN_ENUM_PARAM(m_moveNames, NULL));
    DECLARE_DEPENDENCY(serializer, m_pAnimationController, _T("动画控制器"), eDT_Strong);
}

bool CAnimationData::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pSerializer);
    if(pVariableAddr == &m_strMoveName)
    {
        DeserializeVariable(m_strMoveName, pSerializer);
        size_t len = m_strMoveName.size() * 2 + 1;
        char *movename = new char[len];
        CStringHelper::GetInstance()->ConvertToCHAR(m_strMoveName.c_str(), movename, len);
        CMoveData* pMoveData = GetMovData(movename);
        BEATS_SAFE_DELETE_ARRAY(movename);
        if(m_pAnimationController)
            m_pAnimationController->SetMoveData(pMoveData);
        bRet = true;
    }
    else if(pVariableAddr == &m_strCfgFilePath)
    {
        DeserializeVariable(m_strCfgFilePath, pSerializer);
        ReLoadData();
        CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(this->GetId(), this->GetGuid()));
        BEATS_ASSERT(pProxy != NULL, _T("Can't get proxy with id: %d guid:0x%x"), GetId(), GetGuid());
        CPropertyDescriptionBase* pProperty = pProxy->GetPropertyDescription(GET_VAR_NAME(m_strMoveName));
        BEATS_ASSERT(pProperty != NULL, _T("Get property %s failed!"), GET_VAR_NAME(m_strMoveName));
        pProperty->SetValueList(m_moveNames);
        bRet = true;
    }
    return bRet;
}

bool CAnimationData::OnDependencyChange(void* pComponentAddr, CComponentBase* pComponent)
{
    bool bRet = super::OnDependencyChange(pComponentAddr, pComponent);
    if(pComponentAddr == &m_pAnimationController)
    {
        m_pAnimationController = down_cast<CSkeletonAnimationController*>(pComponent);
        BEATS_ASSERT(m_pAnimationController != NULL);
        if(m_pAnimationController)
        {
            size_t len = m_strMoveName.size() * 2 + 1;
            char *movename = new char[len];
            CStringHelper::GetInstance()->ConvertToCHAR(m_strMoveName.c_str(), movename, len);
            CMoveData* pMoveData = GetMovData(movename);
            BEATS_SAFE_DELETE_ARRAY(movename);
            m_pAnimationController->SetMoveData(pMoveData);
        }
        bRet = true;
    }
    return bRet;
}

void CAnimationData::Initialize()
{
    super::Initialize();
}

void CAnimationData::AddMoveData(CMoveData *moveData)
{
    m_movedataMap[moveData->GetName()] = moveData;
}

CMoveData *CAnimationData::GetMovData(const std::string& moveName)
{
    CMoveData* pRet = nullptr;
    MoveDataMap ::const_iterator iter = m_movedataMap.find(moveName);
    BEATS_ASSERT(iter != m_movedataMap.end());
    if (iter != m_movedataMap.end())
    {
        pRet = iter->second;
    }
    return pRet;
}

void CAnimationData::SetName(const std::string name)
{
    m_strName = name;
}

const std::string& CAnimationData::GetName() const
{
    return m_strName;
}

const std::vector<std::string>& CAnimationData::GetMoveNameList()
{
    if(m_moveNameList.empty())
    {
        for (auto &item : m_movedataMap)
        {
            m_moveNameList.push_back(item.first);
        }
    }

    return m_moveNameList;
}

std::map<std::string, SFrameData>& CAnimationData::GetInitBonesData()
{
    size_t len = m_strMoveName.size() * 2 + 1;
    char *movename = new char[len];
    CStringHelper::GetInstance()->ConvertToCHAR(m_strMoveName.c_str(), movename, len);
    CMoveData* pMoveData = GetMovData(movename);
    BEATS_SAFE_DELETE_ARRAY(movename);
    BEATS_ASSERT(pMoveData);
    m_frameDataMap.clear();
    const std::map<std::string, SMoveBoneData*>& moveBoneDataMap = pMoveData->GetMoveBoneDataMap();
    for(auto iter : moveBoneDataMap)
    {
        std::string boneName = iter.first;
        SMoveBoneData* pBoneData = iter.second;
        for(size_t i = 0; i<pBoneData->frameList.size(); i++)
        {
            SFrameData* framedata = pBoneData->frameList[i];
            SFrameData* next =nullptr;
            if( i +1 < pBoneData->frameList.size())
            {
                next = pBoneData->frameList[i+1];
            }

            if(framedata->frameID == 0 && framedata->dI != -1)
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
                m_frameDataMap[boneName] = data;
                break;
            }
        }
    }
    return m_frameDataMap;
}

std::map<std::string, SFrameData>& CAnimationData::GetCurrFrameBonesData() const
{
    return m_pAnimationController->GetCurrFrameBonesData();
}

CSkeletonAnimationController* CAnimationData::GetAnimationController() const
{
    return m_pAnimationController;
}

void CAnimationData::LoadData()
{
    //TODO:HACK:DON'T USE _T("/SkeletalAnimation/") Directly!
    TString strResourcePath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Resource);
    strResourcePath += _T("/SkeletalAnimation/");
    strResourcePath += m_strCfgFilePath.m_value;
    CAnimationImporter::GetInstance()->DecodeAnimation(strResourcePath, this);
    const std::vector<std::string> moveNamelist = GetMoveNameList();
    for (auto &item : moveNamelist)
    {
        size_t len = item.size() + 1;
        TCHAR *movename = new TCHAR[len];
        CStringHelper::GetInstance()->ConvertToTCHAR(item.c_str(), movename, len);
        m_moveNames.push_back(movename);
        if(m_strMoveName.empty())
            m_strMoveName = movename;
        BEATS_SAFE_DELETE_ARRAY(movename);
    }
}

void CAnimationData::ReLoadData()
{
    for (auto &item : m_movedataMap)
    {
        CMoveData* pMoveData = item.second;
        BEATS_SAFE_DELETE(pMoveData);
    }
    m_strMoveName = _T("");
    m_moveNameList.clear();
    m_moveNames.clear();
    if(m_pAnimationController)
        m_pAnimationController->Restore();
    LoadData();
}

void CAnimationData::InsertFrameData(SFrameData& result,  const SFrameData& beforeData, const SFrameData& afterData, int iFrame)
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
