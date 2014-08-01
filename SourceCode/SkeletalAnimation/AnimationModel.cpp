#include "stdafx.h"
#include "AnimationModel.h"
#include "SkeletonAnimationManager.h"
#include "SkeletonAnimationController.h"
#include "GUI/Window/Window.h"
#include "GUI/WindowManager.h"
#include "Render/TextureFragManager.h"
#include "Render/RenderManager.h"
#include "Render/Texture.h"
#include "Render/TextureFrag.h"
#include "Resource/ResourcePublic.h"
#include "Resource/ResourceManager.h"

#include "AnimationData.h"
#include "ArmatureData.h"
#include "MoveData.h"
#include "TextureData.h"
#include "BoneData.h"
#include "DisplayData.h"
#include "SkeletonAnimationController.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Render/TextureAtlas.h"

using namespace _2DSkeletalAnimation;

CAnimationModel::CAnimationModel()
    : m_bCreateModel(false)
    , m_pArmatureData(NULL)
    , m_pAnimationData(NULL)
{

}

void  CAnimationModel::DestroyModel()
{
    TCHAR szMode[MAX_PATH];
    if (m_pAnimationData != NULL)
    {
        CStringHelper::GetInstance()->ConvertToTCHAR(m_pAnimationData->GetName().c_str(), szMode, MAX_PATH);
    }
}

CAnimationModel::~CAnimationModel()
{

}

void CAnimationModel::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_DEPENDENCY(serializer, m_pArmatureData, _T("骨架"), eDT_Strong);
    DECLARE_DEPENDENCY(serializer, m_pAnimationData, _T("动画"), eDT_Strong);
}

bool CAnimationModel::OnDependencyChange(void* pVariableAddr, CComponentBase* pComponent)
{
    bool bRet = super::OnDependencyChange(pVariableAddr, pComponent);
    if(!bRet)
    {
        if(pVariableAddr == &m_pArmatureData)
        {
            BEATS_ASSERT(dynamic_cast<CArmatureData*>(pComponent) != NULL);
            m_pArmatureData = (CArmatureData*)pComponent;
            if(m_pArmatureData && m_pAnimationData)
            {
                CreateModel();
            }
            bRet = true;
        }
        else if(pVariableAddr == &m_pAnimationData)
        {
            BEATS_ASSERT(dynamic_cast<CArmatureData*>(pComponent) != NULL);
            m_pAnimationData = (CAnimationData*)pComponent;
            if(m_pArmatureData && m_pAnimationData)
            {
                CreateModel();
            }
            bRet = true;
        }
    }
    return bRet;
}

void CAnimationModel::Initialize()
{
    super::Initialize();
    if(m_pAnimationData && m_pArmatureData)
    {
        CreateModel();
    }
    CSkeletonAnimationManager::GetInstance()->RegisterAnimationModel(this);
}

void CAnimationModel::Uninitialize()
{
    super::Uninitialize();
    DestroyModel();
    CSkeletonAnimationManager::GetInstance()->UnregisterAnimationModel(this);
}

void CAnimationModel::CreateModel()
{
    TCHAR szMode[MAX_PATH];
    CStringHelper::GetInstance()->ConvertToTCHAR(m_pAnimationData->GetName().c_str(), szMode, MAX_PATH);

    m_bCreateModel = true;
}

void CAnimationModel::CheckBoneChange()
{
    if(m_bCreateModel)
    {
        if(m_pArmatureData->CheckReplaceData())
        {
            ReplaceDisplayData();
        }
    }
}

void CAnimationModel::Update()
{
    CheckBoneChange();
    if(m_pAnimationData && m_pArmatureData)
    {

    }
}

void CAnimationModel::ReplaceDisplayData()
{
}

