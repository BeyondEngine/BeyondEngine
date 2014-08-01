#include "stdafx.h"
#include "Model.h"
#include "Skin.h"
#include "Skeleton.h"
#include "SkeletonBone.h"
#include "Texture.h"
#include "AnimationController.h"
#include "ShaderProgram.h"
#include "RenderManager.h"
#include "AnimationManager3D.h"
#include "Animation3D.h"
#include "Resource/ResourceManager.h"
#include "Renderer.h"
#include "Material.h"
#include "RenderState.h"
#include "Shader.h"
#include "RenderGroupManager.h"
#include "RenderGroup.h"
#include "Camera.h"
#include "RenderBatch.h"

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
    ((unsigned long)(unsigned char)(ch0) | ((unsigned long)(unsigned char)(ch1) << 8) |   \
    ((unsigned long)(unsigned char)(ch2) << 16) | ((unsigned long)(unsigned char)(ch3) << 24 ))

CModel::CModel()
    : m_bRenderSkeleton(true)
    , m_pSkinProgram(NULL)
    , m_pAnimationController(NULL)
    , m_bPlayAnimation(true)
{
}

CModel::~CModel()
{

}

void CModel::Initialize()
{
    super::Initialize();
    m_pAnimationController = CAnimationManager3D::GetInstance()->CreateSkelAnimationController();
#ifdef SW_SKEL_ANIM
    SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(_T("PointTexShader.vs"));
    SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(_T("PointTexShader.ps"));
#else
    SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(_T("SkinShader.vs"));
    SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(_T("SkinShader.ps"));
#endif

    m_pSkinProgram = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());
    if (!m_strSkin.m_value.empty())
    {
        m_pSkin =  CResourceManager::GetInstance()->GetResource<CSkin>(m_strSkin.m_value);
    }
    if (!m_strSkeleton.m_value.empty())
    {
        m_pSkeleton =  CResourceManager::GetInstance()->GetResource<CSkeleton>(m_strSkeleton.m_value);
        m_pAnimationController->SetSkeleton(m_pSkeleton);
    }
    if (!m_strSelAnimtaionName.empty())
    {
        SharePtr<CAnimation3D> pAnimation = CResourceManager::GetInstance()->GetResource<CAnimation3D>(m_strSelAnimtaionName);
        m_pAnimationController->PlayAnimation(pAnimation, 0, true);
    }
}

void CModel::Uninitialize()
{
    CAnimationManager3D::GetInstance()->DeleteController(m_pAnimationController);
}

void CModel::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_pMaterial, true, 0xFFFFFFFF, _T("材质"), NULL, NULL, NULL);

    DECLARE_PROPERTY(serializer, m_animationNamePathList, true, 0xFFFFFFFF, _T("动画文件列表"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_strSkeleton, true, 0xFFFFFFFF, _T("骨骼资源名称"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_strSkin, true, 0xFFFFFFFF, _T("蒙皮资源名称"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_strSelAnimtaionName, true, 0xFFFFFFFF, _T("动画文件"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bPlayAnimation, true, 0xFFFFFFFF, _T("是否播放动画"), NULL, NULL, NULL);
}

bool CModel::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pSerializer);
    if (!bRet)
    {
        if(pVariableAddr == &m_bPlayAnimation)
        {
            DeserializeVariable(m_bPlayAnimation, pSerializer);
            if(m_bPlayAnimation)
            {
                SharePtr<CAnimation3D> pAnimation = CResourceManager::GetInstance()->GetResource<CAnimation3D>(m_strSelAnimtaionName);
                m_pAnimationController->PlayAnimation(pAnimation, 0, true);
            }
            else
            {
                m_pAnimationController->Stop();
            }
            bRet = true;
        }
        else if(pVariableAddr == &m_strSkin)
        {
            DeserializeVariable(m_strSkin, pSerializer);
            m_pSkin =  CResourceManager::GetInstance()->GetResource<CSkin>(m_strSkin.m_value);
            bRet = true;
        }
        else if(pVariableAddr == &m_strSkeleton)
        {
            DeserializeVariable(m_strSkeleton, pSerializer);
            m_pSkeleton =  CResourceManager::GetInstance()->GetResource<CSkeleton>(m_strSkeleton.m_value);
            m_pAnimationController->SetSkeleton(m_pSkeleton);
            bRet = true;
        }
        else if(pVariableAddr == &m_strSelAnimtaionName)
        {
            DeserializeVariable(m_strSelAnimtaionName,pSerializer);
            if(m_bPlayAnimation)
            {
                SharePtr<CAnimation3D> pAnimation = CResourceManager::GetInstance()->GetResource<CAnimation3D>(m_strSelAnimtaionName);
                m_pAnimationController->PlayAnimation(pAnimation, 0, true);
            }
            bRet = true;
        }
        else if(pVariableAddr == &m_animationNamePathList)
        {
            DeserializeVariable(m_animationNamePathList, pSerializer);
            if(!m_animationNamePathList.empty())
            {
                CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(this->GetId(), this->GetGuid()));
                BEATS_ASSERT(pProxy != NULL, _T("Can't get proxy with id: %d guid:0x%x"), GetId(), GetGuid());
                CPropertyDescriptionBase* pProperty = pProxy->GetPropertyDescription(GET_VAR_NAME(m_strSelAnimtaionName));
                BEATS_ASSERT(pProperty != NULL, _T("Get property %s failed!"), GET_VAR_NAME(m_strSelAnimtaionName));

                m_animationNameList.clear();
                for (auto &item : m_animationNamePathList)
                {
                    if(!item.m_value.empty())
                        m_animationNameList.push_back(item.m_value);
                }
                if(!m_animationNameList.empty())
                    pProperty->SetValueList(m_animationNameList);
            }
            bRet = true;
        }
    }
    return bRet;
}

void CModel::Update(float dtt)
{
    if (m_pAnimationController != NULL)
    {
        m_pAnimationController->Update(dtt);
    }
    super::Update(dtt);
}

void CModel::PlayAnimationById( long id, float fBlendTime, bool bLoop )
{
    auto itr = m_animations.find(id);
    if(itr != m_animations.end())
    {
        m_pAnimationController->PlayAnimation(itr->second, fBlendTime, bLoop);
    }
}

void CModel::PlayAnimationByName( const char *name, float fBlendTime, bool bLoop )
{
    long id = MAKEFOURCC(name[0], name[1], name[2], name[3]);
    PlayAnimationById(id, fBlendTime, bLoop);
}

CAnimationController* CModel::GetAnimationController()
{
    return m_pAnimationController;
}

void CModel::PreRender()
{
    if (m_pSkin && m_pSkin->IsLoaded())
    {
        GLuint currShaderProgram = CRenderer::GetInstance()->GetCurrentState()->GetShaderProgram();
        if(m_pAnimationController)
        {
            if (currShaderProgram != 0)
            {
                auto &matrices = m_pAnimationController->GetDeltaMatrices();
#ifdef SW_SKEL_ANIM
                m_pSkin->CalcSkelAnim(matrices);
#else
                for(auto &item : matrices)
                {
                    ESkeletonBoneType boneType = item.first;
                    TCHAR szUniformName[32];
                    _stprintf(szUniformName, _T("%s[%d]"), COMMON_UNIFORM_NAMES[UNIFORM_BONE_MATRICES], boneType);
                    const std::vector<CRenderBatch*>& renderBatches = m_pSkin->GetRenderBatches();
                    for (size_t i = 0; i < renderBatches.size(); ++i)
                    {
                        CShaderUniform* pUniform = renderBatches[i]->GetMaterial()->GetUniform(szUniformName);
                        if (pUniform == NULL)
                        {
                            pUniform = new CShaderUniform(szUniformName, eSUT_Matrix4f);
                            renderBatches[i]->GetMaterial()->AddUniform(pUniform);
                        }
                        std::vector<float> data;
                        for (size_t i = 0; i < 16; ++i)
                        {
                            data.push_back(item.second.mat[i]);
                        }
                        pUniform->SetData(data);
                    }
                }
#endif
            }
        }

        CRenderManager::GetInstance()->SendMVPToShader(GetLocalTM());
    }
}

void CModel::DoRender()
{
    if (m_pSkin && m_pSkin->IsLoaded())
    {
        const std::vector<CRenderBatch*>& renderBatches = m_pSkin->GetRenderBatches();
        for (size_t i = 0; i < renderBatches.size(); ++i)
        {
            renderBatches[i]->SetTransform(GetWorldTM());
            renderBatches[i]->GetGroup()->AddRenderBatch(renderBatches[i]);
        }
        if (m_pSkeleton != NULL && m_pAnimationController != NULL)
        {
            RenderSkeleton();
        }
    }
}

void CModel::SetAnimaton( SharePtr<CAnimation3D> pAnimation )
{
    m_animations[0] = pAnimation;
}

void CModel::SetSkeleton( SharePtr<CSkeleton> pSkeleton )
{
    m_pSkeleton = pSkeleton;
    m_pAnimationController->SetSkeleton(pSkeleton);
}

SharePtr<CSkeleton> CModel::GetSkeleton() const
{
    return m_pSkeleton;
}

void CModel::SetSkin( SharePtr<CSkin> pSkin )
{
    m_pSkin = pSkin;
}

void CModel::RenderSkeleton()
{
    SharePtr<CAnimation3D> pCurrentAnimation = m_pAnimationController->GetCurrentAnimation();
    BEATS_ASSERT(pCurrentAnimation);

    //Construct render info
    CVertexPC startVertex;
    CVertexPC endVertex;
    const std::vector<ESkeletonBoneType>& bones = pCurrentAnimation->GetBones();
    for (size_t i = 0; i < bones.size(); ++i)
    {
        SharePtr<CSkeletonBone> pSkeletonBone = m_pSkeleton->GetSkeletonBone(bones[i]);

        kmMat4 matrix;
        m_pAnimationController->GetBoneCurWorldTM(matrix, pSkeletonBone->GetBoneType());
        if (pSkeletonBone->GetVisible())
        {
            SharePtr<CSkeletonBone> pParentBone = pSkeletonBone->GetParent();
            if (pParentBone)
            {
                kmMat4 parentMatrix;
                m_pAnimationController->GetBoneCurWorldTM(parentMatrix,  pParentBone->GetBoneType());

                startVertex.position.x = matrix.mat[12];
                startVertex.position.y = matrix.mat[13];
                startVertex.position.z = matrix.mat[14];
                startVertex.color = 0xFF0000FF;

                endVertex.position.x = parentMatrix.mat[12];
                endVertex.position.y = parentMatrix.mat[13];
                endVertex.position.z = parentMatrix.mat[14];
                endVertex.color = 0xFFFF00FF;
                CRenderManager::GetInstance()->RenderLine(startVertex, endVertex);
            }
        }
        if (pSkeletonBone->GetCoordinateVisible())
        {
            CRenderManager::GetInstance()->RenderCoordinate(&matrix);
        }
    }
}