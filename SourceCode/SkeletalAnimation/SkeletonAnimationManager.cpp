#include "stdafx.h"
#include "SkeletonAnimationManager.h"
#include "AnimationImporter.h"
#include "SkeletonAnimationController.h"
#include "ArmatureData.h"
#include "TextureData.h"
#include "AnimationModel.h"

using namespace _2DSkeletalAnimation;

CSkeletonAnimationManager *CSkeletonAnimationManager::m_pInstance = nullptr;

CSkeletonAnimationManager::CSkeletonAnimationManager()
{

}

CSkeletonAnimationManager::~CSkeletonAnimationManager()
{
    for (auto  item : m_animationDataMap)
    {
        BEATS_SAFE_DELETE(item.second);
    }

    for (auto  item : m_armatureDataMap)
    {
        BEATS_SAFE_DELETE(item.second);
    }

    for (auto &item : m_textureList)
    {
        BEATS_SAFE_DELETE(item);
    }
}

CAnimationData* CSkeletonAnimationManager::GetAnimationData(const std::string& name) const
{
    CAnimationData* pAnimationData = nullptr;
    std::map<std::string, CAnimationData*>::const_iterator iter = m_animationDataMap.find(name);
    BEATS_ASSERT(iter != m_animationDataMap.end());
    if(iter != m_animationDataMap.end())
    {
        pAnimationData = iter->second;
    }
    return pAnimationData;
}

CArmatureData* CSkeletonAnimationManager::GetArmatureData(const std::string& name) const
{
    CArmatureData* pArmatureData = nullptr;
    std::map<std::string, CArmatureData*>::const_iterator iter = m_armatureDataMap.find(name);
    BEATS_ASSERT(iter != m_armatureDataMap.end());
    if(iter != m_armatureDataMap.end())
    {
        pArmatureData = iter->second;
    }
    return pArmatureData;
}

void CSkeletonAnimationManager::AddAnimationData(CAnimationData* pAnimationData)
{
    BEATS_ASSERT(pAnimationData);
    m_animationDataMap[pAnimationData->GetName()] = pAnimationData;
    m_strLastAnimationName = pAnimationData->GetName();
}

void CSkeletonAnimationManager::AddArmatureData(CArmatureData* pArmatureData)
{
    BEATS_ASSERT(pArmatureData);
    m_armatureDataMap[pArmatureData->GetName()] = pArmatureData;
    m_strLastArmatureName = pArmatureData->GetName();
}

std::vector<CTextureData*>& CSkeletonAnimationManager::GetTextureData()
{
    return m_textureList;
}

const std::string CSkeletonAnimationManager::GetLastAnimationName() const
{
    return m_strLastAnimationName;
}

const std::string CSkeletonAnimationManager::GetLastArmatureName() const
{
    return m_strLastArmatureName;
}

void CSkeletonAnimationManager::AddAnimationDataFromFile(const TString &file)
{
    CAnimationImporter::GetInstance()->ImportData(file);
}

void CSkeletonAnimationManager::Update(float fDeltaTime)
{
    for (auto pController : m_controllerList)
    {
        pController->Update(fDeltaTime);
    }

    for (auto pAniModel : m_animationModelList)
    {
        pAniModel->Update();
    }
}

void CSkeletonAnimationManager::RegisterController(CSkeletonAnimationController* pController)
{
    BEATS_ASSERT(pController);
    m_controllerList.push_back(pController);
}

void CSkeletonAnimationManager::UnRegisterController(CSkeletonAnimationController* pController)
{
    BEATS_ASSERT(pController);
    m_controllerList.remove(pController);
}

void CSkeletonAnimationManager::RegisterAnimationModel( CAnimationModel* pAniModel )
{
    BEATS_ASSERT(pAniModel);
    m_animationModelList.push_back(pAniModel);
}

void CSkeletonAnimationManager::UnregisterAnimationModel( CAnimationModel* pAniModel )
{
    BEATS_ASSERT(pAniModel);
    m_animationModelList.remove(pAniModel);
}
