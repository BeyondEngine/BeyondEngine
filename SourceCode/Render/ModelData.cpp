#include "stdafx.h"
#include "ModelData.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Skin.h"
#include "Skeleton.h"
#include "SkeletonAnimation.h"
#ifdef EDITOR_MODE
#include "Model.h"
#endif

CModelData::CModelData()
: m_pSkin(new CSkin)
, m_pSkeleton(new CSkeleton)
{
}

CModelData::~CModelData()
{
    if (IsInitialized())
    {
        Uninitialize();
    }
    BEATS_SAFE_DELETE(m_pSkin)
    BEATS_SAFE_DELETE(m_pSkeleton)
}

bool CModelData::Load()
{
    BEATS_ASSERT(!IsLoaded());

    BEATS_ASSERT(m_pSkin != NULL);
    BEATS_ASSERT(m_pSkeleton != NULL);
    BEATS_ASSERT(m_animationMap.empty());
    bool bRet = CFilePathTool::GetInstance()->Exists(GetFilePath().c_str());
    // Load From File
    if (bRet)
    {
        CSerializer modeldata;
        CFilePathTool::GetInstance()->LoadFile(&modeldata, GetFilePath().c_str(), _T("rb"));

        m_pSkin->Load(modeldata);
        m_pSkeleton->Load(modeldata);

        uint32_t uAnimationCount = 0xFFFFFFFF;
        uint8_t uFPS = 0;
        modeldata >> uAnimationCount >> uFPS;
        m_pSkeleton->SetFPS(uFPS);
        BEATS_ASSERT(uFPS > 0);
        for (uint32_t i = 0; i < uAnimationCount; i++)
        {
            CSkeletonAnimation* pSkeAnimation = new CSkeletonAnimation(m_pSkeleton);
            pSkeAnimation->Load(modeldata);
            const TString& strAniName = pSkeAnimation->GetAniName();
            if (!strAniName.empty())
            {
                m_animationMap[strAniName] = pSkeAnimation;
            }
            else
            {
                BEATS_ASSERT(false, _T("Animation of model \'%s\' has a null name"), GetFilePath().c_str());
            }
        }
    }
    return bRet && super::Load();
}

bool CModelData::Unload()
{
    BEATS_ASSERT(IsLoaded());
    BEATS_SAFE_DELETE_MAP(m_animationMap);
    return super::Unload();
}

const std::map<TString, CSkeletonAnimation* >& CModelData::GetAnimationMap() const
{
    return m_animationMap;
}

#ifdef DEVELOP_VERSION
TString CModelData::GetDescription() const
{
    uint32_t uFrameCount = 0;
    for (auto iter = m_animationMap.begin(); iter != m_animationMap.end(); ++iter)
    {
        uFrameCount += iter->second->GetFrameCount();
    }
    TString strRet = super::GetDescription();
    TCHAR szBuffer[MAX_PATH];
    _stprintf(szBuffer, "AnimationCount: %d, FrameCount: %d, ", (uint32_t)m_animationMap.size(), uFrameCount);
    strRet.append(szBuffer);
    _stprintf(szBuffer, "Skin vertexCount: %d, ", m_pSkin->m_uVertexCount);
    strRet.append(szBuffer);
    _stprintf(szBuffer, "skeleton bone Count: %d, ", (uint32_t)m_pSkeleton->GetBoneMap().size());
    strRet.append(szBuffer);
    return strRet;
}
#endif

CSkin* CModelData::GetSkin() const
{
    return m_pSkin;
}

CSkeleton* CModelData::GetSkeleton() const
{
    return m_pSkeleton;
}

void CModelData::Initialize()
{
    super::Initialize();
    m_pSkin->Initialize();
}

void CModelData::Uninitialize()
{
    m_pSkin->Uninitialize();
    super::Uninitialize();
}

#ifdef EDITOR_MODE
void CModelData::Reload()
{
    super::Reload();
    Uninitialize();
    Initialize();
    const std::map<uint32_t, std::map<uint32_t, CComponentBase*>*>* pComponentInstanceMap = CComponentProxyManager::GetInstance()->GetComponentInstanceMap();
    auto iter = pComponentInstanceMap->find(CModel::REFLECT_GUID);
    if (iter != pComponentInstanceMap->end())
    {
        std::vector<CModel*> bak;
        for (auto subIter = iter->second->begin(); subIter != iter->second->end(); ++subIter)
        {
            CModel* pModel = down_cast<CModel*>(down_cast<CComponentProxy*>(subIter->second)->GetHostComponent());
            bak.push_back(pModel);
        }
        for (size_t i = 0; i < bak.size(); ++i)
        {
            CModel* pModel = bak[i];
            pModel->Uninitialize();
            pModel->Initialize();
        }
    }
}
#endif
