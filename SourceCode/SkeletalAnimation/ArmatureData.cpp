#include "stdafx.h"
#include "ArmatureData.h"
#include "BoneData.h"
#include "DisplayData.h"
#include "AnimationImporter.h"
#include "Resource/ResourcePublic.h"
#include "Resource/ResourceManager.h"
#include "AnimationImporter.h"

using namespace _2DSkeletalAnimation;

CArmatureData::CArmatureData()
    : m_strBoneName(_T("ax"))
    , m_pDisplayData(nullptr)
{
    m_strCfgFilePath.m_value = _T("tauren.ExportJson");
    LoadData();
}

CArmatureData::~CArmatureData()
{
    for (auto &item : m_boneDataMap)
    {
        CBoneData* pBoneData = item.second;
        BEATS_SAFE_DELETE(pBoneData);
    }
}

void CArmatureData::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_strCfgFilePath, true, 0xFFFFFFFF, _T("资源文件"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_strBoneName, true, 0xFFFFFFFF, _T("待换装骨骼列表"), NULL, NULL, GEN_ENUM_PARAM(m_boneNameList, NULL));
    DECLARE_DEPENDENCY(serializer, m_pDisplayData, _T("待换装蒙皮"), eDT_Weak);
}

bool CArmatureData::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pSerializer);
    if(!bRet)
    {
        if(pVariableAddr == &m_pDisplayData)
        {
            m_pDisplayData = (CDisplayData*)pSerializer;
            if(m_pDisplayData)
            {
                m_pDisplayData->SetChange(true);
            }
            bRet = true;
        }
        else if(pVariableAddr == &m_strCfgFilePath)
        {
            DeserializeVariable(m_strCfgFilePath, pSerializer);
            ReLoadData();
            CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(this->GetId(), this->GetGuid()));
            BEATS_ASSERT(pProxy != NULL, _T("Can't get proxy with id: %d guid:0x%x"), GetId(), GetGuid());
            CPropertyDescriptionBase* pProperty = pProxy->GetPropertyDescription(GET_VAR_NAME(m_strBoneName));
            BEATS_ASSERT(pProperty != NULL, _T("Get property %s failed!"), GET_VAR_NAME(m_strBoneName));
            pProperty->SetValueList(m_boneNameList);

            bRet = true;
        }
    }
    return bRet;
}

void CArmatureData::Initialize()
{
    super::Initialize();
}

void CArmatureData::AddBoneData(CBoneData *boneData)
{
    m_boneDataMap[boneData->GetName()] = boneData;
    size_t insertPos = 0;
    for (insertPos = 0; insertPos < m_boneDataOrderList.size(); ++insertPos)
    {
        if(boneData->GetZOrder() < m_boneDataOrderList[insertPos]->GetZOrder())
        {
            break;
        }
    }
    m_boneDataOrderList.insert(m_boneDataOrderList.begin() + insertPos, boneData);
}

const CBoneData* CArmatureData::GetBoneData(const std::string &boneName) const
{
    CBoneData* pRet = nullptr;
    BonesDataMap ::const_iterator iter = m_boneDataMap.find(boneName);
    BEATS_ASSERT(iter != m_boneDataMap.end());
    if (iter != m_boneDataMap.end())
    {
        pRet = iter->second;
    }
    return pRet;
}

const std::string& CArmatureData::GetName() const
{
    return m_strName;
}

void CArmatureData::SetName(const std::string& name)
{
    m_strName = name;
}

const CArmatureData::BonesDataMap& CArmatureData::GetBonesMap() const
{
    return m_boneDataMap;
}

const std::vector<CBoneData*>& CArmatureData::GetBoneOrderList()
{
    return m_boneDataOrderList;
}

const std::vector<std::string>& CArmatureData::GetBoneOrderNames()
{
    return m_boneNames;
}

CDisplayData CArmatureData::ReplaceDisplayData()
{
    size_t len = m_strBoneName.size() + 1;
    char *strBoneName = new char[len];
    CStringHelper::GetInstance()->ConvertToCHAR(m_strBoneName.c_str(), strBoneName, len);
    BonesDataMap ::const_iterator iter = m_boneDataMap.find(strBoneName);
    BEATS_SAFE_DELETE_ARRAY(strBoneName);
    BEATS_ASSERT(iter != m_boneDataMap.end());
    CBoneData* pBoneData = iter->second;
    CDisplayData displayData = *m_pDisplayData;
    displayData.Uninitialize();
    return pBoneData->ReplaceDisplayData(displayData);;
}


bool CArmatureData::CheckReplaceData()
{
    bool bRet = false;
    if(m_pDisplayData && m_pDisplayData->CheckChange())
    {
        bRet = true;
    }
    return bRet;
}

const std::string _2DSkeletalAnimation::CArmatureData::GetReplaceDisplayName() const
{
    return m_pDisplayData->GetName();
}

void CArmatureData::LoadData()
{
    //TODO:HACK: DON'T USE +_T("SkeletalAnimation/") directly!
    TString strResourcePath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Resource);
    strResourcePath += _T("/SkeletalAnimation/");
    strResourcePath += m_strCfgFilePath.m_value;
    CAnimationImporter::GetInstance()->DecodeArmature(strResourcePath, this);
    const std::vector<CBoneData*>& bones = GetBoneOrderList();
    for (auto pBoneData : bones)
    {
        const std::string boneName = pBoneData->GetName();
        m_boneNames.push_back(boneName);
        size_t len = boneName.size() + 1;
        TCHAR *tstrBoneName = new TCHAR[len];
        CStringHelper::GetInstance()->ConvertToTCHAR(boneName.c_str(), tstrBoneName, len);
        m_boneNameList.push_back(tstrBoneName);
        if(m_strBoneName.empty())
            m_strBoneName = tstrBoneName;
        BEATS_SAFE_DELETE_ARRAY(tstrBoneName);
    }
}

void CArmatureData::ReLoadData()
{
    for (auto &item : m_boneDataMap)
    {
        CBoneData* pBoneData = item.second;
        BEATS_SAFE_DELETE(pBoneData);
    }
    m_strBoneName = _T("");
    m_boneNameList.clear();
    m_boneNames.clear();
    m_boneDataOrderList.clear();
    LoadData();
}
