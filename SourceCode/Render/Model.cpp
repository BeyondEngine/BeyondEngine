#include "stdafx.h"
#include "Model.h"
#include "Skin.h"
#include "Skeleton.h"
#include "SkeletonBone.h"
#include "Texture.h"
#include "AnimationController.h"
#include "ShaderProgram.h"
#include "RenderManager.h"
#include "SkeletonAnimation.h"
#include "Resource/ResourceManager.h"
#include "RenderState.h"
#include "Material.h"
#include "RenderGroup.h"
#include "RenderBatch.h"
#include "external/Configuration.h"
#include "Action/ActionBase.h"
#include "CoordinateRenderObject.h"
#include "ShapeRenderer.h"

static const uint32_t MAX_MATRIX_UNIFORM_COUNT = 32;

CModel::CModel()
    : m_pAnimationController(NULL)
    , m_bPlayAnimation(true)
{
    m_defaultGroupID = LAYER_3D;
}

CModel::~CModel()
{
    BEATS_SAFE_DELETE(m_pAnimationController);
}

bool CModel::Load()
{
    bool bRet = super::Load();
    LoadModelData();
    m_pAnimationController = new CAnimationController;
    if (m_pModelData)
    {
        CSkeleton* pSkeleton = m_pModelData->GetSkeleton();
        if (pSkeleton)
        {
            m_pAnimationController->SetSkeleton(pSkeleton);
        }
    }
#ifdef EDITOR_MODE
    std::vector<TString> animationList;
    if (GetProxyComponent() != nullptr)
    {
        CPropertyDescriptionBase* pProperty = GetProxyComponent()->GetProperty("m_strCurrAnimationName");
        if (pProperty != nullptr && m_pModelData)
        {
            for (auto iter = m_pModelData->GetAnimationMap().begin(); iter != m_pModelData->GetAnimationMap().end(); ++iter)
            {
                const TString& strName = iter->second->GetAniName();
                BEATS_ASSERT(!strName.empty());
                animationList.push_back(strName);
            }
            pProperty->SetValueList(animationList);
        }
    }
#endif

    return bRet;
}

void CModel::Initialize()
{
    super::Initialize();
    if (m_pModelData)
    {
        BEATS_ASSERT(m_pModelData->IsInitialized())
        if (m_pModelData->GetSkin() != nullptr)
        {
            RefreshBySkin();
        }
        if (!m_strCurrAnimationName.empty())
        {
            PlayAnimationByName(m_strCurrAnimationName, 0, true);
        }
    }

    BEATS_ASSERT(CConfiguration::GetInstance()->GetMaxVertexUniformVectors() >= MAX_MATRIX_UNIFORM_COUNT * 4,
        _T("Max vertex uniform vectors overflow, max vertex uniform vector: %d but we defined: %d x4"), CConfiguration::GetInstance()->GetMaxVertexUniformVectors(), MAX_MATRIX_UNIFORM_COUNT); // one matrix contain 4 vec4.
}

void CModel::Uninitialize()
{
    for (size_t i = 0; i < m_renderBatchList.size(); ++i)
    {
        BEATS_SAFE_DELETE(m_renderBatchList[i]);
    }
    m_renderBatchList.clear();
    BEATS_SAFE_DELETE(m_pAnimationController);
    m_pModelData = NULL;
    super::Uninitialize();
}

void CModel::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bFrustumTest, true, 0xFFFFFFFF, _T("开启视锥剔除"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fPlaySpeed, true, 0xFFFFFFFF, _T("播放速度"), NULL, NULL, "MinValue:0");
    DECLARE_PROPERTY(serializer, m_modelFile, true, 0xFFFFFFFF, _T("模型资源文件"), NULL, NULL, _T("DefaultValue:Wildcard@*.model@InitialPath@../Resource/Model@DialogTitle@选择模型文件"));
    DECLARE_PROPERTY(serializer, m_strCurrAnimationName, true, 0xFFFFFFFF, _T("初始动画名称"), NULL, NULL, _T("VisibleWhen:m_modelFile != trigger_empty"));
    DECLARE_PROPERTY(serializer, m_bPlayAnimation, true, 0xFFFFFFFF, _T("是否播放动画"), NULL, NULL, NULL);
}
#ifdef EDITOR_MODE
bool CModel::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pSerializer);
    if (!bRet)
    {
        if(pVariableAddr == &m_bPlayAnimation)
        {
            DeserializeVariable(m_bPlayAnimation, pSerializer, this);
            if(m_bPlayAnimation)
            {
                if (!m_strCurrAnimationName.empty())
                {
                    PlayAnimationByName(m_strCurrAnimationName, 0, true);
                }
            }
            else
            {
                m_pAnimationController->Stop();
            }
            bRet = true;
        }
        else if (pVariableAddr == &m_modelFile)
        {
            DeserializeVariable(m_modelFile, pSerializer, this);
#ifdef EDITOR_MODE
            LoadFile(m_modelFile.m_value);
#endif
            std::vector<TString> animationList;
            if (GetProxyComponent() != nullptr)
            {
                CPropertyDescriptionBase* pProperty = GetProxyComponent()->GetProperty("m_strCurrAnimationName");
                if (pProperty != nullptr && m_pModelData)
                {
                    for (auto iter = m_pModelData->GetAnimationMap().begin(); iter != m_pModelData->GetAnimationMap().end(); ++iter)
                    {
                        const TString& strName = iter->second->GetAniName();
                        BEATS_ASSERT(!strName.empty());
                        animationList.push_back(strName);
                    }
                    pProperty->SetValueList(animationList);
                }
            }
            TString strAnimation;
            if (animationList.size() > 0)
            {
                strAnimation = animationList[0];
                BEATS_ASSERT(!strAnimation.empty());
            }
            SetCurrAnimation(strAnimation);
            bRet = true;
        }
        else if (pVariableAddr == &m_strCurrAnimationName)
        {
            TString strFileName;
            DeserializeVariable(strFileName, pSerializer, this);
            SetCurrAnimation(strFileName);
            bRet = true;
        }
    }
    return bRet;
}
#endif
void CModel::Update(float dtt)
{
    if (m_pAnimationController != NULL)
    {
        m_pAnimationController->Update(dtt * m_fPlaySpeed);
    }
    super::Update(dtt);
}

void CModel::PlayAnimationByName(const TString& strName, float fBlendTime, bool bLoop)
{
    CSkeletonAnimation* pAnimation = NULL;
    ASSUME_VARIABLE_IN_EDITOR_BEGIN(m_pModelData != NULL)
    const std::map<TString, CSkeletonAnimation* >& animationMap = m_pModelData->GetAnimationMap();

    auto iter = animationMap.find(strName);
    BEATS_ASSERT(iter != animationMap.end(), _T("no animation of model %s named %s"), m_pModelData->GetFilePath().c_str(), strName.c_str());
    if (iter == animationMap.end())
    {
        pAnimation = animationMap.begin()->second;
    }
    else
    {
        pAnimation = iter->second;
    }
    if (pAnimation)
    {
        m_pAnimationController->PlayAnimation(pAnimation, fBlendTime, bLoop);
    }
    ASSUME_VARIABLE_IN_EDITOR_END
}

CAnimationController* CModel::GetAnimationController() const
{
    return m_pAnimationController;
}

void CModel::SetCurrAnimation(const TString& strFileName)
{
    if (strFileName != m_strCurrAnimationName)
    {
        m_strCurrAnimationName = strFileName;
        if(m_bPlayAnimation)
        {
            if (!m_strCurrAnimationName.empty())
            {
                PlayAnimationByName(m_strCurrAnimationName, 0, true);
            }
            else
            {
                m_pAnimationController->Stop();
            }
        }
    }
}

void CModel::DoRender()
{
#ifdef DEVELOP_VERSION
    if (!CRenderManager::GetInstance()->m_bRenderModel)
    {
        return;
    }
#endif
    bool bFrustumTest = true;
    if (m_pModelData && m_pModelData->GetSkin())
    {
        if (m_renderBatchList.size() > 0)
        {
            if (m_pAnimationController && m_pAnimationController->IsPlaying())
            {
                CSkeletonAnimation* pCurrAnimation = m_pAnimationController->GetCurrentAnimation();
                if (pCurrAnimation != NULL)
                {
                    float fCurrentTime = m_pAnimationController->GetPlayingTime();
                    CAABBBox aabb = pCurrAnimation->GetAABBBox(fCurrentTime);
#ifdef DEVELOP_VERSION
                    if (CRenderManager::GetInstance()->m_bRenderAABB)
                    {
                        CShapeRenderer::GetInstance()->DrawAABB(aabb, 0xFF0000FF);
                    }
#endif
                    bFrustumTest = !m_bFrustumTest || aabb.IntersectWithFrustum(CRenderManager::GetInstance()->GetCamera(CCamera::eCT_3D)->GetFrustumPlanes());
                    if (bFrustumTest)
                    {
                        auto matrices = pCurrAnimation->GetDeltaTM(fCurrentTime);
                        std::map<TString, CShaderUniform>& uniformMap = m_renderBatchList[0]->GetUniformMap();
                        for (auto &item : matrices)
                        {
                            uint32_t uBoneIndex = item.first;
                            TCHAR szUniformName[32];
                            _stprintf(szUniformName, _T("%s[%d]"), COMMON_UNIFORM_NAMES[UNIFORM_BONE_MATRICES], uBoneIndex);
                            BEATS_ASSERT(MAX_MATRIX_UNIFORM_COUNT > uBoneIndex, _T("Bone index  %d is out of range!"), uBoneIndex);
                            CShaderUniform& shaderUniform = uniformMap[szUniformName];
                            if (shaderUniform.GetName().empty())
                            {
                                shaderUniform.SetName(szUniformName);
                                shaderUniform.SetType(eSUT_Matrix4f);
                            }
                            std::vector<float>& data = shaderUniform.GetData();
                            BEATS_ASSERT(data.size() == 16);
                            memcpy(data.data(), item.second.Mat4ValuePtr(), 16 * sizeof(float));
                        }
                        CColor colorScale = GetColorScale(true);
                        CShaderUniform& alphaUniform = uniformMap[COMMON_UNIFORM_NAMES[UNIFORM_COLOR_SCALE]];
                        if (alphaUniform.GetName().empty())
                        {
                            alphaUniform.SetName(COMMON_UNIFORM_NAMES[UNIFORM_COLOR_SCALE]);
                            alphaUniform.SetType(eSUT_4f);
                        }
                        std::vector<float>& alphadata = alphaUniform.GetData();
                        BEATS_ASSERT(alphadata.size() == 4);
                        alphadata[0] = colorScale.r * 0.01f;
                        alphadata[1] = colorScale.g * 0.01f;
                        alphadata[2] = colorScale.b * 0.01f;
                        alphadata[3] = colorScale.a * 0.01f;
                    }
                }
            }
        }
        if (bFrustumTest)
        {
            CRenderGroup* pRenderGroup = GetRenderGroup();
            for (uint32_t i = 0; i < m_renderBatchList.size(); ++i)
            {
                m_renderBatchList[i]->SetWorldTM(&GetWorldTM());
                pRenderGroup->AddRenderBatch(m_renderBatchList[i]);
#ifdef DEVELOP_VERSION
                if (m_renderBatchList[i]->m_usage == ERenderBatchUsage::eRBU_Count)
                {
                    m_renderBatchList[i]->m_usage = ERenderBatchUsage::eRBU_Model;
                }
                BEATS_ASSERT(m_renderBatchList[i]->m_usage == ERenderBatchUsage::eRBU_Model);
#endif
            }
#ifdef EDITOR_MODE
            if (m_pModelData->GetSkeleton() != NULL && m_pAnimationController != NULL)
            {
                RenderSkeleton();
            }
#endif
        }
    }
}

const std::map<TString, CSkeletonAnimation* > CModel::GetAnimations() const
{
    BEATS_ASSERT(m_pModelData);
    return m_pModelData->GetAnimationMap();
}

void CModel::RefreshBySkin()
{
    if (m_pModelData && m_pModelData->GetSkin() != nullptr)
    {
        CSkin* pSkin = m_pModelData->GetSkin();
        for (size_t i = 0; i < m_renderBatchList.size(); ++i)
        {
            BEATS_SAFE_DELETE(m_renderBatchList[i]);
        }
        m_renderBatchList.clear();
        const std::map<std::string, SSkinBatchInfo>& batchRenderInfoMap = pSkin->GetBatchInfoMap();
        SharePtr<CMaterial> skinMaterial = CRenderManager::GetInstance()->GetSkinMaterial();
        for (auto iter = batchRenderInfoMap.begin(); iter != batchRenderInfoMap.end(); ++iter)
        {
            const SSkinBatchInfo& batchInfo = iter->second;
            std::map<unsigned char, SharePtr<CTexture> > textureInfo;
            textureInfo[0] = CResourceManager::GetInstance()->GetResource<CTexture>(iter->first);
            CRenderBatch* pRenderBatch = new CRenderBatch(VERTEX_FORMAT(CVertexPTB), skinMaterial, GL_TRIANGLES, true, true);
            pRenderBatch->SetStatic(true);
            pRenderBatch->SetAutoManage(false);
            pRenderBatch->SetVAO(pSkin->GetVAO());
            pRenderBatch->SetVBO(pSkin->GetVBO());
            pRenderBatch->SetEBO(pSkin->GetEBO());
            pRenderBatch->SetDataSize(batchInfo.m_uDataSize);
            pRenderBatch->SetStartPos(batchInfo.m_uStartPos);
            pRenderBatch->SetTextureMap(textureInfo);
            m_renderBatchList.push_back(pRenderBatch);
        }
    }
}

#ifdef EDITOR_MODE
void CModel::RenderSkeleton()
{
    CSkeletonAnimation* pCurrentAnimation = m_pAnimationController->GetCurrentAnimation();
    if (pCurrentAnimation)
    {
        //Construct render info
        CVertexPC startVertex;
        startVertex.color = 0xFF0000FF;
        CVertexPC endVertex;
        endVertex.color = 0xFFFF00FF;
        const std::map<uint8_t, CSkeletonBone*>& bonesMap = m_pModelData->GetSkeleton()->GetBoneMap();
        for (auto iter = bonesMap.begin(); iter != bonesMap.end(); ++iter)
        {
            CSkeletonBone* pSkeletonBone = iter->second;
            if (pSkeletonBone->GetVisible() || CRenderManager::GetInstance()->m_bRenderSkeletonDbgLine)
            {
                CMat4 currWorldMatrix = m_pAnimationController->GetBoneCurrWorldTM(pSkeletonBone->GetIndex());
                startVertex.position.X() = currWorldMatrix[12];
                startVertex.position.Y() = currWorldMatrix[13];
                startVertex.position.Z() = currWorldMatrix[14];
                CVec3 direction = currWorldMatrix.GetRightVec3();
                endVertex.position = direction * 10 + startVertex.position;
                CRenderManager::GetInstance()->RenderLine(startVertex, endVertex, 10);
            }
            if (pSkeletonBone->GetCoordinateVisible() || CRenderManager::GetInstance()->m_bRenderSkeletonDbgLine)
            {
                CMat4 currWorldMatrix = m_pAnimationController->GetBoneCurrWorldTM(pSkeletonBone->GetIndex());
                CRenderManager::GetInstance()->GetCoordinateRenderObject()->RenderMatrix(currWorldMatrix, true);
            }
        }
    }
}
#endif

void CModel::LoadModelData()
{
    BEATS_ASSERT(m_pModelData == NULL);
    if (!m_modelFile.m_value.empty())
    {
        m_pModelData = CResourceManager::GetInstance()->GetResource<CModelData>(m_modelFile.m_value.c_str());
        BEATS_ASSERT(m_pModelData != NULL);
    }
}

CSkeleton* CModel::GetSkeleton() const
{
    BEATS_ASSERT(m_pModelData);
    BEATS_ASSERT(m_pModelData->GetSkeleton());
    return m_pModelData->GetSkeleton();
}

SharePtr<CModelData> CModel::GetModelData() const
{
    return m_pModelData;
}

const TString& CModel::GetCurrentAnimationName() const
{
    return m_strCurrAnimationName;
}

void CModel::LoadFile(const TString& strFile)
{
    m_modelFile.m_value = strFile;
    if (m_pModelData)
    {
        for (size_t i = 0; i < m_renderBatchList.size(); ++i)
        {
            BEATS_SAFE_DELETE(m_renderBatchList[i]);
        }
        m_renderBatchList.clear();

        m_pModelData = NULL;
    }
    LoadModelData();

    if (m_pModelData)
    {
        CSkeleton* pSkeleton = m_pModelData->GetSkeleton();
        if (pSkeleton)
        {
            m_pAnimationController->SetSkeleton(pSkeleton);
        }
    }
    if (m_pModelData && m_pModelData->GetSkin() != nullptr)
    {
        if (!m_pModelData->GetSkin()->IsInitialized())
        {
            m_pModelData->GetSkin()->Initialize();
        }
        RefreshBySkin();
    }
}

#ifdef EDITOR_MODE
void CModel::SendDataToGraphics()
{
    CSkeletonAnimation* pCurrAnimation = m_pAnimationController->GetCurrentAnimation();
    if (pCurrAnimation != NULL)
    {
        auto matrices = pCurrAnimation->GetDeltaTM(m_pAnimationController->GetPlayingTime());
        for (auto &item : matrices)
        {
            uint32_t uBoneIndex = item.first;
            TCHAR szUniformName[32];
            _stprintf(szUniformName, _T("%s[%d]"), COMMON_UNIFORM_NAMES[UNIFORM_BONE_MATRICES], uBoneIndex);
            BEATS_ASSERT(MAX_MATRIX_UNIFORM_COUNT > uBoneIndex, _T("Bone index  %d is out of range!"), uBoneIndex);
            std::map<TString, CShaderUniform>& uniformMap = m_renderBatchList[0]->GetUniformMap();
            CShaderUniform& shaderUniform = uniformMap[szUniformName];
            if (shaderUniform.GetName().empty())
            {
                shaderUniform.SetName(szUniformName);
                shaderUniform.SetType(eSUT_Matrix4f);
            }
            std::vector<float>& data = shaderUniform.GetData();
            BEATS_ASSERT(data.size() == 16);
            memcpy(data.data(), item.second.Mat4ValuePtr(), 16 * sizeof(float));
        }
    }
}


#endif
