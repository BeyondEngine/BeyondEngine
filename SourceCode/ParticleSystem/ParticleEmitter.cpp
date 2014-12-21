#include "stdafx.h"
#include "ParticleEmitter.h"
#include "EmissionModule.h"
#include "ShapeModule.h"
#include "VelocityModule.h"
#include "VelocityLimitModule.h"
#include "ColorModule.h"
#include "ForceModule.h"
#include "SizeModule.h"
#include "RotationModule.h"
#include "TextureSheetAnimationModule.h"
#include "RenderModule.h"
#include "ParticleManager.h"
#include "Particle.h"
#ifdef EDITOR_MODE
#include "BeyondEngineEditor/RandomValuePropertyDescription.h"
#include "BeyondEngineEditor/MapElementPropertyDescription.h"
#include "BeyondEngineEditor/MapPropertyDescription.h"
#endif
#include "BasicModule.h"
#include "Render/RenderManager.h"
#include "Render/RenderTarget.h"
#include "Scene/SceneManager.h"
CParticleEmitter::CParticleEmitter()
{
    m_defaultGroupID = LAYER_Particle;
    m_uRandomSeed = rand();
    m_uCurrSeedForParticle = m_uRandomSeed;
#ifdef DEVELOP_VERSION
    CParticleManager::GetInstance()->m_emitterRegisterSet.insert(this);
#endif
}

CParticleEmitter::~CParticleEmitter()
{
#ifdef DEVELOP_VERSION
    CParticleManager::GetInstance()->m_emitterRegisterSet.erase(this);
#endif
}

void CParticleEmitter::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bTemplate, true, 0xFFFFFFFF, "是否为模板", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_bRenderWithOwner, true, 0xFFFFFFFF, "绑定到owner", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_fParticleScale, true, 0xFFFFFFFF, "粒子缩放值", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_pBasicModule, true, 0xFFFFFFFF, "Basic", nullptr, nullptr, "CloneAble:false");
    DECLARE_PROPERTY(serializer, m_pEmissionModule, true, 0xFFFFFFFF, "Emission", nullptr, nullptr, "CloneAble:false");
    DECLARE_PROPERTY(serializer, m_pShapeModule, true, 0xFFFFFFFF, "Shape", nullptr, nullptr, "CloneAble:false");
    DECLARE_PROPERTY(serializer, m_pVelocityModule, true, 0xFFFFFFFF, "Velocity", nullptr, nullptr, "CloneAble:false");
    DECLARE_PROPERTY(serializer, m_pVelocityLimitModule, true, 0xFFFFFFFF, "VelocityLimit", nullptr, nullptr, "CloneAble:false");
    DECLARE_PROPERTY(serializer, m_pForceModule, true, 0xFFFFFFFF, "Force", nullptr, nullptr, "CloneAble:false");
    DECLARE_PROPERTY(serializer, m_pColorModule, true, 0xFFFFFFFF, "Color", nullptr, nullptr, "CloneAble:false");
    DECLARE_PROPERTY(serializer, m_pSizeModule, true, 0xFFFFFFFF, "Size", nullptr, nullptr, "CloneAble:false");
    DECLARE_PROPERTY(serializer, m_pRotationModule, true, 0xFFFFFFFF, "Rotation", nullptr, nullptr, "CloneAble:false");
    DECLARE_PROPERTY(serializer, m_pTextureSheetAnimationModule, true, 0xFFFFFFFF, "TextureSheetAnimation", nullptr, nullptr, "CloneAble:false");
    DECLARE_PROPERTY(serializer, m_pRenderModule, true, 0xFFFFFFFF, "Render", nullptr, nullptr, "CloneAble:false");

    DECLARE_PROPERTY(serializer, m_childrenEmitter, true, 0xFFFFFFFF, "子发射器", nullptr, nullptr, nullptr);
}

CNode* CParticleEmitter::GetOwner() const
{
    return m_pOwner;
}

void CParticleEmitter::SetOwner(CNode* pOwner)
{
    m_pOwner = pOwner;
}

CScene* CParticleEmitter::GetAttachScene() const
{
    return m_pAttachScene;
}

void CParticleEmitter::SetAttachScene(CScene* pScene)
{
    m_pAttachScene = pScene;
}

void CParticleEmitter::Play(bool bForceReplay)
{
    if (IsInitialized())
    {
        if (!IsPlaying())
        {
            m_bPlaying = true;
            m_bPaused = false;
            for (auto iter = m_childrenEmitter.begin(); iter != m_childrenEmitter.end(); ++iter)
            {
                (*iter)->Play();
            }
            if (m_pParentEmitter == nullptr)
            {
                CParticleManager::GetInstance()->AddPlayingEmitter(this);
            }
        }
        else if (bForceReplay)
        {
            m_fPlayingTime = 0;
            m_fLastEmitTime = 0;
            m_fLastBurstTime = -1.0f;
            m_uRandomSeed = CParticleManager::GetInstance()->GetParticleRandValue();
            m_uCurrSeedForParticle = 0;
        }
    }
    else
    {
        m_bDelayPlay = true;
    }
}

void CParticleEmitter::Pause()
{
    m_bPlaying = false;
    m_bPaused = true;
    for (auto iter = m_childrenEmitter.begin(); iter != m_childrenEmitter.end(); ++iter)
    {
        (*iter)->Pause();
    }
}

void CParticleEmitter::Stop()
{
    if (IsPlaying() || m_bPaused)
    {
        if (m_bRemoveParentWhenStop && m_pParentNode)
        {
            m_pParentNode->RemoveChild(this);
        }
        m_bPlaying = false;
        m_bPaused = false;
        m_fPlayingTime = 0;
        m_uCurrSeedForParticle = m_uRandomSeed;
        m_fLastEmitTime = 0;
        m_fLastBurstTime = -1.f;
        std::set<CParticle*> bak = m_particleList;
        for (auto iter = bak.begin(); iter != bak.end(); ++iter)
        {
            RemoveParticle(*iter, true);
        }
        BEATS_ASSERT(m_particleList.size() == 0);
        for (auto iter = m_childrenEmitter.begin(); iter != m_childrenEmitter.end(); ++iter)
        {
            (*iter)->Stop();
        }
        if (m_pParentEmitter == nullptr)
        {
            CParticleManager::GetInstance()->RemovePlayingEmitter(this);
            if (IsAutoRecycle())
            {
                CParticleManager::GetInstance()->RecycleEmitter(this);
            }
        }
    }
}

bool CParticleEmitter::IsPlaying() const
{
    return m_bPlaying && !m_bPaused;
}

bool CParticleEmitter::IsPaused() const
{
    return m_bPaused;
}

void CParticleEmitter::SetPlayingFlag(bool bFlag)
{
    m_bPlaying = bFlag;
    for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
    {
        m_childrenEmitter[i]->SetPlayingFlag(bFlag);
    }
}

void CParticleEmitter::Initialize()
{
    InitChildEmitter();
    if (m_bTemplate)
    {
        CParticleManager::GetInstance()->RegisterEmitterTemplate(this);
    }
    else
    {
        if (m_pBasicModule)
        {
            SetDepthTestEnable(m_pBasicModule->IsDepthTestEnable());
        }
    }
    super::Initialize();
    if (m_bDelayPlay)
    {
        m_bDelayPlay = false;
        Play();
    }
}

void CParticleEmitter::Uninitialize()
{
    if (m_bTemplate)
    {
        CParticleManager::GetInstance()->StopAllEmitter(GetName());
        CParticleManager::GetInstance()->UnregisterEmitterTemplate(GetName());
    }
    else
    {
        Stop();
    }
    super::Uninitialize();
}

void CParticleEmitter::Update(float dtt)
{
#ifdef DEVELOP_VERSION
    uint32_t uCurrFrameCounter = CEngineCenter::GetInstance()->GetFrameCounter();
    BEATS_ASSERT(uCurrFrameCounter != m_uLastUpdateFrameCount, "Detect on emitter is updated more than once in a loop! Name:%s", GetName().c_str());
    m_uLastUpdateFrameCount = uCurrFrameCounter;
#endif
    BEATS_ASSERT(IsInitialized());
    if (m_pBasicModule)
    {
        BEATS_ASSERT(m_pBasicModule->IsInitialized());
#ifdef DEVELOP_VERSION
        if (!GetName().empty())
        {
            std::map<TString, SParticleDetail*>& particleDetailMap = CParticleManager::GetInstance()->m_particleDetailMap;
            if (particleDetailMap.find(GetName()) == particleDetailMap.end())
            {
                SParticleDetail* pParticleDetail = NULL;
                if (CParticleManager::GetInstance()->m_particleDataPool.size() > 0)
                {
                    pParticleDetail = CParticleManager::GetInstance()->m_particleDataPool.back();
                    pParticleDetail->emitterUpdateCount = 0;
                    pParticleDetail->particleUpdateCount = 0;
                    pParticleDetail->RenderCount = 0;
                    CParticleManager::GetInstance()->m_particleDataPool.pop_back();
                }
                else
                {
                    pParticleDetail = new SParticleDetail();
                }
                pParticleDetail->emitterUpdateCount++;
                pParticleDetail->particleUpdateCount += m_particleList.size();
                particleDetailMap[GetName()] = pParticleDetail;
            }
            else
            {
                particleDetailMap[GetName()]->emitterUpdateCount++;
                particleDetailMap[GetName()]->particleUpdateCount += m_particleList.size();
            }
        }
        if (m_pBasicModule)
        {
            static bool bLastValue = m_pBasicModule->IsDepthTestEnable();
            if (bLastValue != m_pBasicModule->IsDepthTestEnable())
            {
                SetDepthTestEnable(m_pBasicModule->IsDepthTestEnable());
                bLastValue = m_pBasicModule->IsDepthTestEnable();
            }
        }
#endif
        if (m_bPlaying)
        {
            bool bIsLoop = m_pBasicModule->IsLoop();
            float fDuration = m_pBasicModule->GetDuration();
            if (bIsLoop)
            {
                float fRestTimeForDuration = BEATS_FMOD(GetPlayingTime(false), fDuration);
                if (BEATS_FLOAT_GREATER_EQUAL(fRestTimeForDuration + dtt, fDuration))
                {
                    m_fLastBurstTime = -1.f;
                }
            }
            m_fPlayingTime += dtt;
            bool bTimeUp = (!bIsLoop && GetPlayingTime(false) > fDuration) || !IsActive();
            if (GetPlayingTime(true) > m_pBasicModule->GetStartDelay())
            {
                if (!bTimeUp)
                {
                    CEmissionModule* pEmissionModule = GetEmissionModule();
                    if (pEmissionModule && m_particleList.size() < m_pBasicModule->GetMaxParticleCount())
                    {
                        BEATS_ASSERT(pEmissionModule->IsInitialized());
                        std::vector<CParticle*> newParticle;
                        if (m_pBasicModule->GetMaxParticleCount() > m_particleList.size())
                        {
                            pEmissionModule->Update(this, newParticle, m_pBasicModule->GetMaxParticleCount() - m_particleList.size());
                            for (size_t i = 0; i < newParticle.size(); ++i)
                            {
                                AddParticle(newParticle[i]);
                            }
                        }
                    }
                }
            }
            for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
            {
                ASSUME_VARIABLE_IN_EDITOR_BEGIN(m_childrenEmitter[i])
#ifdef EDITOR_MODE
                    if (m_childrenEmitter[i]->GetParentNode() == nullptr)
                    {
                        BEATS_ASSERT(m_childrenEmitter[i]->m_pParentEmitter == nullptr);
                        m_childrenEmitter[i]->m_pParentEmitter = this;
                        AddChild(m_childrenEmitter[i]);
                    }
#else
                    BEATS_ASSERT(m_childrenEmitter[i]->GetParentNode() == this && m_childrenEmitter[i]->m_pParentEmitter == this);
#endif
                    m_childrenEmitter[i]->Update(dtt);
                ASSUME_VARIABLE_IN_EDITOR_END
            }
            BEATS_ASSERT(m_particleList.size() <= m_pBasicModule->GetMaxParticleCount(), "current particle count is %d, but the max count is %d", (uint32_t)m_particleList.size(), m_pBasicModule->GetMaxParticleCount());
            std::set<CParticle*> bak = m_particleList;
            for (auto iter = bak.begin(); iter != bak.end(); ++iter)
            {
                (*iter)->Update(dtt);
            }
        }
        if (IsFinished())
        {
            Stop();
        }
    }
}

void CParticleEmitter::Render()
{
#ifdef EDITOR_MODE
    if (m_pBasicModule)
    {
        static bool b3DFlag = m_pBasicModule->Is3DEmitter();
        if (b3DFlag != m_pBasicModule->Is3DEmitter())
        {
            b3DFlag = m_pBasicModule->Is3DEmitter();
            Set3DRender(b3DFlag);
        }
    }
#endif
#ifdef DEVELOP_VERSION
    uint32_t uCurrFrameCounter = CEngineCenter::GetInstance()->GetFrameCounter();
    BEATS_ASSERT(!CApplication::GetInstance()->IsRunning() || uCurrFrameCounter != m_uLastRenderFrameCount, "Detect on emitter is rendered more than once in a loop! Name:%s", GetName().c_str());
    m_uLastRenderFrameCount = uCurrFrameCounter;
#endif
    CScene* pCurrScene = CSceneManager::GetInstance()->GetCurrentScene();
    if ((!m_bRenderWithOwner || m_bOwnerRenderFlag) && (m_pAttachScene == nullptr || (m_pAttachScene == pCurrScene && CSceneManager::GetInstance()->GetRenderSwitcher())))
    {
        bool bNeedPop = false;
        if (m_renderGroupID != LAYER_UNSET)
        {
            bNeedPop = true;
            CRenderGroupManager::GetInstance()->PushRenderGroupID(m_renderGroupID);
        }
        else
        {
            if (CRenderGroupManager::GetInstance()->GetRenderGroupIDStack().size() == 0)
            {
                bNeedPop = true;
                CRenderGroupManager::GetInstance()->PushRenderGroupID(GetBasicModule()->Is3DEmitter() ? m_defaultGroupID : LAYER_GUI);
            }
        }
        m_bOwnerRenderFlag = false;
        for (auto iter = m_particleList.begin(); iter != m_particleList.end(); ++iter)
        {
            (*iter)->Render();
        }
        for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
        {
            ASSUME_VARIABLE_IN_EDITOR_BEGIN(m_childrenEmitter[i])
                m_childrenEmitter[i]->Render();
            ASSUME_VARIABLE_IN_EDITOR_END
        }
        if (bNeedPop)
        {
            CRenderGroupManager::GetInstance()->PopRenderGroupID();
        }
    }
}

CComponentInstance* CParticleEmitter::CloneInstance() const
{
    CParticleEmitter* pRet = down_cast<CParticleEmitter*>(super::CloneInstance());
    CopyModulesToEmitter(pRet);
#ifdef EDITOR_MODE
    pRet->SetSyncProxyComponent(GetProxyComponent());
#endif
    return pRet;
}
#ifdef EDITOR_MODE
bool CParticleEmitter::OnPropertyChange(void* pVariableAddr, CSerializer* pSerializer)
{
    bool bRet = super::OnPropertyChange(pVariableAddr, pSerializer);
    if (!bRet)
    {
        if (pVariableAddr == &m_childrenEmitter)
        {
            DeserializeVariable(m_childrenEmitter, pSerializer, this);
            InitChildEmitter();
            bRet = true;
        }
        else if (pVariableAddr == &m_fParticleScale)
        {
            float fScale = 0.0f;
            DeserializeVariable(fScale, pSerializer, this);
            SetParticleScale(fScale);
            RefreshAllParticle();
            bRet = true;
        }
        if (pVariableAddr == &m_bTemplate)
        {
            DeserializeVariable(m_bTemplate, pSerializer, this);
            bool bIsTemplateRegistered = CParticleManager::GetInstance()->GetEmitterTemplate(GetName()) != nullptr;
            if (m_bTemplate)
            {
                if (!bIsTemplateRegistered)
                {
                    CParticleManager::GetInstance()->RegisterEmitterTemplate(this);
                }
            }
            else
            {
                if (bIsTemplateRegistered)
                {
                    CParticleManager::GetInstance()->UnregisterEmitterTemplate(GetName());
                }
            }
            bRet = true;
        }
    }
    return bRet;
}
#endif
void CParticleEmitter::Deactivate(bool bApplyToChild/* = true*/)
{
    super::Deactivate(bApplyToChild);
    if (m_pBasicModule && m_pBasicModule->IsLoop() && m_pBasicModule->IsStopWhenDeactive())
    {
        Stop();
    }
}

#ifdef EDITOR_MODE
bool CParticleEmitter::OnPropertyImport(const TString& /*strPropertyName*/, const TString& strFile)
{
    bool bRet = false;
    const TCHAR* pszFilePath = strFile.c_str();
    rapidxml::file<> fdoc(pszFilePath);
    rapidxml::xml_document<> doc;
    try
    {
        doc.parse<rapidxml::parse_default>(fdoc.data());
        doc.m_pszFilePath = pszFilePath;
        bRet = true;
    }
    catch (rapidxml::parse_error &e)
    {
        TCHAR info[MAX_PATH];
        _stprintf(info, _T("Load file :%s Failed! error :%s"), pszFilePath, e.what());
        MessageBox(BEYONDENGINE_HWND, info, _T("Load File Failed"), MB_OK | MB_ICONERROR);
    }
    rapidxml::xml_node<>* pXMLNode = doc.first_node();
    BEATS_ASSERT(pXMLNode != nullptr);
    pXMLNode = pXMLNode->next_sibling();
    Stop();
    ImportFromUnityXML(pXMLNode);
    GetProxyComponent()->UpdateHostComponent();
    return bRet;
}

void CParticleEmitter::ImportFromUnityXML(rapidxml::xml_node<>* pXMLNode)
{
    CComponentProxy* pProxy = GetProxyComponent();
    BEATS_ASSERT(pProxy != nullptr && pProxy->GetGuid() == CParticleEmitter::REFLECT_GUID);

    // Reset all property.
    const std::vector<CPropertyDescriptionBase*>* pPropertyPool = pProxy->GetPropertyPool();
    for (size_t i = 0; i < pPropertyPool->size(); ++i)
    {
        CPropertyDescriptionBase* pCurrProperty = pPropertyPool->at(i);
        if (pCurrProperty->IsContainerProperty())
        {
            pCurrProperty->RemoveAllChild(true);
        }
        else if (pCurrProperty->GetType() == eRPT_Ptr)
        {
            down_cast<CPtrPropertyDescription*>(pCurrProperty)->DestroyInstance();
        }
    }

    pProxy->GetProperty("m_strName")->SetValueByString(pXMLNode->name());
    rapidxml::xml_node<>* pPosNode = pXMLNode->first_node("Position");
    BEATS_ASSERT(pPosNode);
    CPropertyDescriptionBase* pPosProperty = pProxy->GetProperty("m_pos");
    BEATS_ASSERT(pPosProperty);
    pPosProperty->GetChildren()[0]->SetValueByString(pPosNode->first_node("x")->value());
    pPosProperty->GetChildren()[1]->SetValueByString(pPosNode->first_node("y")->value());
    pPosProperty->GetChildren()[2]->SetValueByString(pPosNode->first_node("z")->value());

    rapidxml::xml_node<>* pRotationNode = pXMLNode->first_node("Rotation");
    BEATS_ASSERT(pRotationNode);
    CPropertyDescriptionBase* pRotateProperty = pProxy->GetProperty("m_rotation");
    BEATS_ASSERT(pRotateProperty);
    CQuaternion rotationQuat;
    rotationQuat.X() = (float)_tstof(pRotationNode->first_node("x")->value());
    rotationQuat.Y() = (float)_tstof(pRotationNode->first_node("y")->value());
    rotationQuat.Z() = (float)_tstof(pRotationNode->first_node("z")->value());
    rotationQuat.W() = (float)_tstof(pRotationNode->first_node("w")->value());
    float fPitch = 0;
    float fYaw = 0;
    float fRoll = 0;
    rotationQuat.ToPitchYawRoll(fPitch, fYaw, fRoll);
    fPitch = RadiansToDegrees(fPitch);
    fYaw = RadiansToDegrees(fYaw);
    fRoll = RadiansToDegrees(fRoll);

    if (BEATS_FLOAT_LESS_EPSILON(fPitch, 0, 0.0001f))
    {
        fPitch += 360.f;
    }
    if (BEATS_FLOAT_LESS_EPSILON(fYaw, 0, 0.0001f))
    {
        fYaw += 360.f;
    }
    if (BEATS_FLOAT_LESS_EPSILON(fRoll, 0, 0.0001f))
    {
        fRoll += 360.f;
    }
    pRotateProperty->GetChildren()[0]->SetValueWithType(&fPitch, eVT_CurrentValue, true);
    pRotateProperty->GetChildren()[1]->SetValueWithType(&fYaw, eVT_CurrentValue, true);
    pRotateProperty->GetChildren()[2]->SetValueWithType(&fRoll, eVT_CurrentValue, true);

    rapidxml::xml_node<>* pScaleNode = pXMLNode->first_node("Scale");
    BEATS_ASSERT(pScaleNode);
    CPropertyDescriptionBase* pScalesProperty = pProxy->GetProperty("m_scale");
    BEATS_ASSERT(pScalesProperty);
    pScalesProperty->GetChildren()[0]->SetValueByString(pScaleNode->first_node("x")->value());
    pScalesProperty->GetChildren()[1]->SetValueByString(pScaleNode->first_node("y")->value());
    pScalesProperty->GetChildren()[2]->SetValueByString(pScaleNode->first_node("z")->value());

    rapidxml::xml_node<>* pParticleSystemNode = pXMLNode->first_node("ParticleSystem");
    if (pParticleSystemNode)
    {
        CPtrPropertyDescription* pBasicProperty = down_cast<CPtrPropertyDescription*>(pProxy->GetProperty("m_pBasicModule"));
        CComponentProxy* pBasicModuleProxy = pBasicProperty->GetInstanceComponent();
        BEATS_ASSERT(pBasicModuleProxy == nullptr);
        pBasicProperty->CreateInstance(false);
        pBasicModuleProxy = pBasicProperty->GetInstanceComponent();
        pBasicModuleProxy->GetProperty("m_fDuration")->SetValueByString(pParticleSystemNode->first_node("lengthInSec")->value());
        pBasicModuleProxy->GetProperty("m_fStartDelay")->SetValueByString(pParticleSystemNode->first_node("startDelay")->value());
        pBasicModuleProxy->GetProperty("m_bLoop")->SetValueByString(pParticleSystemNode->first_node("looping")->value());
        pBasicModuleProxy->GetProperty("m_bPrewarm")->SetValueByString(pParticleSystemNode->first_node("prewarm")->value());
        pBasicModuleProxy->GetProperty("m_bPlayOnAwake")->SetValueByString(pParticleSystemNode->first_node("playOnAwake")->value());
        pBasicModuleProxy->GetProperty("m_bAttachToEmitter")->SetValueByString(pParticleSystemNode->first_node("moveWithTransform")->value());
        rapidxml::xml_node<>* pInitialModuleNode = pParticleSystemNode->first_node("InitialModule");
        BEATS_ASSERT(pInitialModuleNode);
        ParseColorSplineValue(pBasicModuleProxy->GetProperty("m_startColor"), pInitialModuleNode->first_node("startColor"));
        pBasicModuleProxy->GetProperty("m_fGravityModifier")->SetValueByString(pInitialModuleNode->first_node("gravityModifier")->value());
        pBasicModuleProxy->GetProperty("m_uMaxParticle")->SetValueByString(pInitialModuleNode->first_node("maxNumParticles")->value());
        ParseRandomValue(pBasicModuleProxy->GetProperty("m_startLifeTime"), pInitialModuleNode->first_node("startLifetime"));
        ParseRandomValue(pBasicModuleProxy->GetProperty("m_startSpeed"), pInitialModuleNode->first_node("startSpeed"));
        ParseRandomValue(pBasicModuleProxy->GetProperty("m_startSize"), pInitialModuleNode->first_node("startSize"));
        ParseRandomValue(pBasicModuleProxy->GetProperty("m_startRotation"), pInitialModuleNode->first_node("startRotation"));
        pBasicModuleProxy->GetHostComponent()->Initialize();

        rapidxml::xml_node<>* pShapeModuleNode = pParticleSystemNode->first_node("ShapeModule");
        if (pShapeModuleNode != nullptr)
        {
            CPtrPropertyDescription* pShapeProperty = down_cast<CPtrPropertyDescription*>(pProxy->GetProperty("m_pShapeModule"));
            CComponentProxy* pShapeModuleProxy = pShapeProperty->GetInstanceComponent();
            BEATS_ASSERT(pShapeModuleProxy == nullptr);
            pShapeProperty->CreateInstance(false);
            pShapeModuleProxy = pShapeProperty->GetInstanceComponent();
            BEATS_ASSERT(pShapeModuleProxy != nullptr);
            const TCHAR* pszType = pShapeModuleNode->first_node("type")->value();
            int32_t nType = _tstoi(pszType);
            switch (nType)
            {
            case 0:
            case 1:
                pShapeModuleProxy->GetProperty("m_emitShapeType")->SetValueByString("eEST_Sphere");
                pShapeModuleProxy->GetProperty("m_bEmitFromShell")->SetValueByString(nType == 1 ? "true" : "false");
                break;
            case 2:
            case 3:
                pShapeModuleProxy->GetProperty("m_emitShapeType")->SetValueByString("eEST_HemiSphere");
                pShapeModuleProxy->GetProperty("m_bEmitFromShell")->SetValueByString(nType == 3 ? "true" : "false");
                break;
            case 4:
                pShapeModuleProxy->GetProperty("m_emitShapeType")->SetValueByString("eEST_Cone");
                pShapeModuleProxy->GetProperty("m_bEmitFromShell")->SetValueByString("false");
                pShapeModuleProxy->GetProperty("m_bEmitFromBaseOrVolume")->SetValueByString("true");
                break;
            case 5:
                pShapeModuleProxy->GetProperty("m_emitShapeType")->SetValueByString("eEST_Box");
                pShapeModuleProxy->GetProperty("m_boxSize")->GetChildren()[0]->SetValueByString(pShapeModuleNode->first_node("boxX")->value());
                pShapeModuleProxy->GetProperty("m_boxSize")->GetChildren()[1]->SetValueByString(pShapeModuleNode->first_node("boxY")->value());
                pShapeModuleProxy->GetProperty("m_boxSize")->GetChildren()[2]->SetValueByString(pShapeModuleNode->first_node("boxZ")->value());
                break;
            case 7://cone from base shell
                pShapeModuleProxy->GetProperty("m_emitShapeType")->SetValueByString("eEST_Cone");
                pShapeModuleProxy->GetProperty("m_bEmitFromShell")->SetValueByString("true");
                pShapeModuleProxy->GetProperty("m_bEmitFromBaseOrVolume")->SetValueByString("true");
                break;
            case 8://cone from volume
                pShapeModuleProxy->GetProperty("m_emitShapeType")->SetValueByString("eEST_Cone");
                pShapeModuleProxy->GetProperty("m_bEmitFromBaseOrVolume")->SetValueByString("false");
                pShapeModuleProxy->GetProperty("m_bEmitFromShell")->SetValueByString("false");
                break;
            case 9://cone from volume shell
                pShapeModuleProxy->GetProperty("m_emitShapeType")->SetValueByString("eEST_Cone");
                pShapeModuleProxy->GetProperty("m_bEmitFromBaseOrVolume")->SetValueByString("false");
                pShapeModuleProxy->GetProperty("m_bEmitFromShell")->SetValueByString("true");
                break;
            case 10:
            case 11:
                pShapeModuleProxy->GetProperty("m_emitShapeType")->SetValueByString("eEST_Circle");
                pShapeModuleProxy->GetProperty("m_fArcForCircle")->SetValueByString(pShapeModuleNode->first_node("arc")->value());
                pShapeModuleProxy->GetProperty("m_bEmitFromShell")->SetValueByString(nType == 11 ? "true" : "false");
                break;
            case 12:
                pShapeModuleProxy->GetProperty("m_emitShapeType")->SetValueByString("eEST_Edge");
                break;
            default:
                //6 mesh 
                BEATS_ASSERT(false, _T("type is not support"));
                break;
            }
            pShapeModuleProxy->GetProperty("m_fRadius")->SetValueByString(pShapeModuleNode->first_node("radius")->value());
            pShapeModuleProxy->GetProperty("m_fAngle")->SetValueByString(pShapeModuleNode->first_node("angle")->value());
            pShapeModuleProxy->GetProperty("m_fConeLength")->SetValueByString(pShapeModuleNode->first_node("length")->value());
            pShapeModuleProxy->GetProperty("m_bRandomDirection")->SetValueByString(pShapeModuleNode->first_node("randomDirection")->value());
            pShapeModuleProxy->GetHostComponent()->Initialize();
        }
        rapidxml::xml_node<>* pEmissionModuleNode = pParticleSystemNode->first_node("EmissionModule");
        if (pEmissionModuleNode)
        {
            CPtrPropertyDescription* pEmissionProperty = down_cast<CPtrPropertyDescription*>(pProxy->GetProperty("m_pEmissionModule"));
            CComponentProxy* pEmissionModuleProxy = pEmissionProperty->GetInstanceComponent();
            BEATS_ASSERT(pEmissionModuleProxy == nullptr);
            pEmissionProperty->CreateInstance(false);
            pEmissionModuleProxy = pEmissionProperty->GetInstanceComponent();
            BEATS_ASSERT(pEmissionModuleProxy != nullptr);
            pEmissionModuleProxy->GetProperty("m_fRate")->SetValueByString(pEmissionModuleNode->first_node("rate")->first_node("scalar")->value());
            int32_t nBurstCount = _tstoi(pEmissionModuleNode->first_node("m_BurstCount")->value());
            CMapPropertyDescription* pExtraBurstMapProperty = down_cast<CMapPropertyDescription*>(pEmissionModuleProxy->GetProperty("m_extraBurst"));
            TCHAR szBuffer[64];
            for (int32_t i = 0; i < nBurstCount; ++i)
            {
                _stprintf(szBuffer, "cnt%d", i);
                TString strCount = pEmissionModuleNode->first_node(szBuffer)->value();
                _stprintf(szBuffer, "time%d", i);
                TString strTime = pEmissionModuleNode->first_node(szBuffer)->value();
                CMapElementPropertyDescription* pBurstMapElementProperty = pExtraBurstMapProperty->CreateMapElementProp();
                pBurstMapElementProperty->GetChildren()[0]->SetValueByString(strTime.c_str());
                pBurstMapElementProperty->GetChildren()[1]->SetValueByString(strCount.c_str());
                pExtraBurstMapProperty->InsertChild(pBurstMapElementProperty);
            }
            pEmissionModuleProxy->GetHostComponent()->Initialize();
        }
        rapidxml::xml_node<>* pRenderModuleNode = pParticleSystemNode->first_node("RendererModule");
        if (pRenderModuleNode != nullptr)
        {
            CPtrPropertyDescription* pRenderProperty = down_cast<CPtrPropertyDescription*>(pProxy->GetProperty("m_pRenderModule"));
            CComponentProxy* pRenderModuleProxy = pRenderProperty->GetInstanceComponent();
            BEATS_ASSERT(pRenderModuleProxy == nullptr);
            pRenderProperty->CreateInstance(false);
            pRenderModuleProxy = pRenderProperty->GetInstanceComponent();
            BEATS_ASSERT(pRenderModuleProxy != nullptr);
            TString strRenderMode = pRenderModuleNode->first_node("RenderMode")->value();
            if (strRenderMode == _T("HorizontalBillboard"))
            {
                pRenderModuleProxy->GetProperty("m_mode")->SetValueByString("ePRM_HorizontalBillboard");
            }
            else if (strRenderMode == _T("Billboard"))
            {
                pRenderModuleProxy->GetProperty("m_mode")->SetValueByString("ePRM_Billboard");
            }
            else if (strRenderMode == _T("Stretch"))
            {
                pRenderModuleProxy->GetProperty("m_mode")->SetValueByString("ePRM_StretchedBillboard");
            }
            else if (strRenderMode == _T("VerticalBillboard"))
            {
                pRenderModuleProxy->GetProperty("m_mode")->SetValueByString("ePRM_VerticalBillboard");
            }
            else
            {
                BEATS_ASSERT(false, _T("Never reach here!"));
            }
            pRenderModuleProxy->GetProperty("m_fSpeedScale")->SetValueByString(pRenderModuleNode->first_node("m_VelocityScale")->value());
            pRenderModuleProxy->GetProperty("m_fLengthScale")->SetValueByString(pRenderModuleNode->first_node("m_LengthScale")->value());
            TString strTextureFileName = pRenderModuleNode->first_node("Texture")->value();
            if (strTextureFileName == "default-particle")
            {
                strTextureFileName = "default_particle";
            }
            TString strTextureAtlasName = "3d_effect.xml@";
            strTextureAtlasName.append(strTextureFileName);
            pRenderModuleProxy->GetProperty("m_texture")->SetValueByString(strTextureAtlasName.c_str());
            TString strShader = pRenderModuleNode->first_node("Shader")->value();
            bool bAddMode = (strShader == _T("Particles/Additive") ||
                strShader == _T("Mobile/Particles/Additive"));
            pRenderModuleProxy->GetProperty("m_bBlendOrAdd")->SetValueByString(bAddMode ? "false" : "true");
            rapidxml::xml_node<>* pTintColorNode = pRenderModuleNode->first_node("TintColor");
            if (pTintColorNode != nullptr)
            {
                // Convert ABGR to RGBA.
                const TCHAR* pszABGR = pTintColorNode->first_node("rgba")->value();
                BEATS_ASSERT(_tcslen(pszABGR) == 8);
                TCHAR szTintColor[11] = { '0', 'x', pszABGR[6], pszABGR[7], pszABGR[4], pszABGR[5], pszABGR[2], pszABGR[3], pszABGR[0], pszABGR[1], 0 };
                pRenderModuleProxy->GetProperty("m_tintColor")->SetValueByString(szTintColor);
            }
            CPropertyDescriptionBase* pTextureOffsetProperty = pRenderModuleProxy->GetProperty("m_textureOffset");
            pTextureOffsetProperty->GetChildren()[0]->SetValueByString(pRenderModuleNode->first_node("Tiling")->first_node("X")->value());
            pTextureOffsetProperty->GetChildren()[1]->SetValueByString(pRenderModuleNode->first_node("Tiling")->first_node("Y")->value());
            pTextureOffsetProperty->GetChildren()[2]->SetValueByString(pRenderModuleNode->first_node("Offset")->first_node("X")->value());
            pTextureOffsetProperty->GetChildren()[3]->SetValueByString(pRenderModuleNode->first_node("Offset")->first_node("Y")->value());
            pRenderModuleProxy->GetHostComponent()->Initialize();
        }

        rapidxml::xml_node<>* pVelocityModuleNode = pParticleSystemNode->first_node("VelocityModule");
        if (pVelocityModuleNode != nullptr)
        {
            CPtrPropertyDescription* pVelocityProperty = down_cast<CPtrPropertyDescription*>(pProxy->GetProperty("m_pVelocityModule"));
            CComponentProxy* pVelocityModuleProxy = pVelocityProperty->GetInstanceComponent();
            BEATS_ASSERT(pVelocityModuleProxy == nullptr);
            pVelocityProperty->CreateInstance(false);
            pVelocityModuleProxy = pVelocityProperty->GetInstanceComponent();
            BEATS_ASSERT(pVelocityModuleProxy != nullptr);
            bool bInLocal = _tcscmp(pVelocityModuleNode->first_node("inWorldSpace")->value(), "False") == 0;
            pVelocityModuleProxy->GetProperty("m_bLocalOrWorld")->SetValueByString(bInLocal ? "true" : "false");
            ParseRandomValue(pVelocityModuleProxy->GetProperty("m_velocityX"), pVelocityModuleNode->first_node("x"));
            ParseRandomValue(pVelocityModuleProxy->GetProperty("m_velocityY"), pVelocityModuleNode->first_node("y"));
            ParseRandomValue(pVelocityModuleProxy->GetProperty("m_velocityZ"), pVelocityModuleNode->first_node("z"));
            pVelocityModuleProxy->GetHostComponent()->Initialize();
        }

        rapidxml::xml_node<>* pVelocityLimitModuleNode = pParticleSystemNode->first_node("ClampVelocityModule");
        if (pVelocityLimitModuleNode != nullptr)
        {
            CPtrPropertyDescription* pVelocityLimitProperty = down_cast<CPtrPropertyDescription*>(pProxy->GetProperty("m_pVelocityLimitModule"));
            CComponentProxy* pVelocityLimitModuleProxy = pVelocityLimitProperty->GetInstanceComponent();
            BEATS_ASSERT(pVelocityLimitModuleProxy == nullptr);
            pVelocityLimitProperty->CreateInstance(false);
            pVelocityLimitModuleProxy = pVelocityLimitProperty->GetInstanceComponent();
            BEATS_ASSERT(pVelocityLimitModuleProxy != nullptr);
            bool bInLocal = _tcscmp(pVelocityLimitModuleNode->first_node("inWorldSpace")->value(), "False") == 0;
            pVelocityLimitModuleProxy->GetProperty("m_bLocalOrWorld")->SetValueByString(bInLocal ? "true" : "false");
            bool bSeparateAxis = _tcscmp(pVelocityLimitModuleNode->first_node("separateAxis")->value(), "True") == 0;
            pVelocityLimitModuleProxy->GetProperty("m_bSeparateAxis")->SetValueByString(bSeparateAxis ? "true" : "false");
            ParseRandomValue(pVelocityLimitModuleProxy->GetProperty("m_velocityX"), pVelocityLimitModuleNode->first_node("x"));
            ParseRandomValue(pVelocityLimitModuleProxy->GetProperty("m_velocityY"), pVelocityLimitModuleNode->first_node("y"));
            ParseRandomValue(pVelocityLimitModuleProxy->GetProperty("m_velocityZ"), pVelocityLimitModuleNode->first_node("z"));
            ParseRandomValue(pVelocityLimitModuleProxy->GetProperty("m_speed"), pVelocityLimitModuleNode->first_node("magnitude"));
            pVelocityLimitModuleProxy->GetProperty("m_fDampen")->SetValueByString(pVelocityLimitModuleNode->first_node("dampen")->value());
            pVelocityLimitModuleProxy->GetHostComponent()->Initialize();
        }
        rapidxml::xml_node<>* pForceModuleNode = pParticleSystemNode->first_node("ForceModule");
        if (pForceModuleNode != nullptr)
        {
            CPtrPropertyDescription* pForceMoudleProperty = down_cast<CPtrPropertyDescription*>(pProxy->GetProperty("m_pForceModule"));
            CComponentProxy* pForceModuleProxy = pForceMoudleProperty->GetInstanceComponent();
            BEATS_ASSERT(pForceModuleProxy == nullptr);
            pForceMoudleProperty->CreateInstance(false);
            pForceModuleProxy = pForceMoudleProperty->GetInstanceComponent();
            BEATS_ASSERT(pForceModuleProxy != nullptr);
            bool bInLocal = _tcscmp(pForceModuleNode->first_node("inWorldSpace")->value(), "False") == 0;
            pForceModuleProxy->GetProperty("m_bLocalOrWorld")->SetValueByString(bInLocal ? "true" : "false");
            pForceModuleProxy->GetProperty("m_bRandomize")->SetValueByString(pForceModuleNode->first_node("randomizePerFrame")->value());
            ParseRandomValue(pForceModuleProxy->GetProperty("m_forceX"), pForceModuleNode->first_node("x"));
            ParseRandomValue(pForceModuleProxy->GetProperty("m_forceY"), pForceModuleNode->first_node("y"));
            ParseRandomValue(pForceModuleProxy->GetProperty("m_forceZ"), pForceModuleNode->first_node("z"));
            pForceModuleProxy->GetHostComponent()->Initialize();
        }
        rapidxml::xml_node<>* pSizeModuleNode = pParticleSystemNode->first_node("SizeModule");
        if (pSizeModuleNode != nullptr)
        {
            CPtrPropertyDescription* pSizeMoudleProperty = down_cast<CPtrPropertyDescription*>(pProxy->GetProperty("m_pSizeModule"));
            CComponentProxy* pSizeModuleProxy = pSizeMoudleProperty->GetInstanceComponent();
            BEATS_ASSERT(pSizeModuleProxy == nullptr);
            pSizeMoudleProperty->CreateInstance(false);
            pSizeModuleProxy = pSizeMoudleProperty->GetInstanceComponent();
            BEATS_ASSERT(pSizeModuleProxy != nullptr);
            ParseRandomValue(pSizeModuleProxy->GetProperty("m_size"), pSizeModuleNode->first_node("curve"));
            pSizeModuleProxy->GetHostComponent()->Initialize();
        }
        rapidxml::xml_node<>* pRotationModuleNode = pParticleSystemNode->first_node("RotationModule");
        if (pRotationModuleNode != nullptr)
        {
            CPtrPropertyDescription* pRotationMoudleProperty = down_cast<CPtrPropertyDescription*>(pProxy->GetProperty("m_pRotationModule"));
            CComponentProxy* pRotateModuleProxy = pRotationMoudleProperty->GetInstanceComponent();
            BEATS_ASSERT(pRotateModuleProxy == nullptr);
            pRotationMoudleProperty->CreateInstance(false);
            pRotateModuleProxy = pRotationMoudleProperty->GetInstanceComponent();
            BEATS_ASSERT(pRotateModuleProxy != nullptr);
            ParseRandomValue(pRotateModuleProxy->GetProperty("m_angularSpeed"), pRotationModuleNode->first_node("curve"), true);
            pRotateModuleProxy->GetHostComponent()->Initialize();
        }

        rapidxml::xml_node<>* pUVModuleNode = pParticleSystemNode->first_node("UVModule");
        if (pUVModuleNode != nullptr)
        {
            CPtrPropertyDescription* pUVMoudleProperty = down_cast<CPtrPropertyDescription*>(pProxy->GetProperty("m_pTextureSheetAnimationModule"));
            CComponentProxy* pUVModuleProxy = pUVMoudleProperty->GetInstanceComponent();
            BEATS_ASSERT(pUVModuleProxy == nullptr);
            pUVMoudleProperty->CreateInstance(false);
            pUVModuleProxy = pUVMoudleProperty->GetInstanceComponent();
            BEATS_ASSERT(pUVModuleProxy != nullptr);
            pUVModuleProxy->GetProperty("m_uRow")->SetValueByString(pUVModuleNode->first_node("tilesY")->value()); // tilesY means how many rows in a col
            pUVModuleProxy->GetProperty("m_uCol")->SetValueByString(pUVModuleNode->first_node("tilesX")->value()); // tilesX means how many cols in a row
            pUVModuleProxy->GetProperty("m_uRowIndex")->SetValueByString(pUVModuleNode->first_node("rowIndex")->value());
            pUVModuleProxy->GetProperty("m_uCycles")->SetValueByString(pUVModuleNode->first_node("cycles")->value());
            pUVModuleProxy->GetProperty("m_bRandomRow")->SetValueByString(pUVModuleNode->first_node("randomRow")->value());
            bool bIsWholeSheet = _tcscmp(pUVModuleNode->first_node("animationType")->value(), "0") == 0;
            pUVModuleProxy->GetProperty("m_bWholeSheetOrSingleRow")->SetValueByString(bIsWholeSheet ? "true" : "false");
            ParseRandomValue(pUVModuleProxy->GetProperty("m_frameOverTime"), pUVModuleNode->first_node("frameOverTime"));
            pUVModuleProxy->GetHostComponent()->Initialize();
        }

        rapidxml::xml_node<>* pColorModuleNode = pParticleSystemNode->first_node("ColorModule");
        if (pColorModuleNode != nullptr)
        {
            CPtrPropertyDescription* pColorMoudleProperty = down_cast<CPtrPropertyDescription*>(pProxy->GetProperty("m_pColorModule"));
            CComponentProxy* pColorModuleProxy = pColorMoudleProperty->GetInstanceComponent();
            BEATS_ASSERT(pColorModuleProxy == nullptr);
            pColorMoudleProperty->CreateInstance(false);
            pColorModuleProxy = pColorMoudleProperty->GetInstanceComponent();
            BEATS_ASSERT(pColorModuleProxy != nullptr);
            ParseColorSplineValue(pColorModuleProxy->GetProperty("m_color"), pColorModuleNode->first_node("gradient"));
            pColorModuleProxy->GetHostComponent()->Initialize();
        }
    }

    CPropertyDescriptionBase* pChildProperty = pProxy->GetProperty("m_childrenEmitter");
    pChildProperty->RemoveAllChild(true);
    rapidxml::xml_node<>* pChildEmitterNode = pParticleSystemNode != nullptr ? pParticleSystemNode->next_sibling() : pScaleNode->next_sibling();
    while (pChildEmitterNode != nullptr)
    {
        CPtrPropertyDescription* pEmitterChildProperty = down_cast<CPtrPropertyDescription*>(pChildProperty->InsertChild(nullptr));
        pEmitterChildProperty->CreateInstance(false);
        CComponentProxy* pChildEmitterProxy = pEmitterChildProperty->GetInstanceComponent();
        down_cast<CParticleEmitter*>(pChildEmitterProxy->GetHostComponent())->ImportFromUnityXML(pChildEmitterNode);
        pChildEmitterProxy->GetHostComponent()->Initialize();
        pChildEmitterNode = pChildEmitterNode->next_sibling();
    }
    pProxy->UpdateHostComponent();
}

void CParticleEmitter::RenderShape()
{
    if (GetShapeModule() != nullptr)
    {
        GetShapeModule()->Render(this);
    }
    for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
    {
        if (m_childrenEmitter[i] != nullptr)
        {
            m_childrenEmitter[i]->RenderShape();
        }
    }
}

void CParticleEmitter::ParseRandomValue(CPropertyDescriptionBase* pProperty, rapidxml::xml_node<>* pNode,bool bConvertRadianToDegree)
{
    CRandomValuePropertyDescription* pRandomValueProperty = down_cast<CRandomValuePropertyDescription*>(pProperty);
    TString strBuildData;
    TString strState = pNode->first_node("minMaxState")->value();
    const char* pszScalar = pNode->first_node("scalar")->value();
    TCHAR szBuffer[32];
    if (bConvertRadianToDegree)
    {
        float fRadianValue = (float)_tstof(pszScalar);
        float fDegree = RadiansToDegrees(fRadianValue);
        _stprintf(szBuffer, "%g", fDegree);
        pszScalar = szBuffer;
    }
    if (strState == "0")// Constant value
    {
        strBuildData.append("0,").append(pszScalar);
    }
    else if (strState == "1")// Curve
    {
        strBuildData.append("1,").append(pszScalar).append(",");
        rapidxml::xml_node<>* pArrayNode = pNode->first_node("maxCurve")->first_node("m_Curve")->first_node("Array");
        const TCHAR* pszSize = pArrayNode->first_node("size")->value();
        strBuildData.append(pszSize);
        uint32_t uSize = (uint32_t)_tstof(pszSize);
        rapidxml::xml_node<>* pDataNode = pArrayNode->first_node("data");
        for (uint32_t i = 0; i < uSize; ++i)
        {
            strBuildData.append(",");
            strBuildData.append(pDataNode->first_node("time")->value()).append(",");
            strBuildData.append(pDataNode->first_node("value")->value()).append(",");
            strBuildData.append(pDataNode->first_node("inSlope")->value()).append(",");
            strBuildData.append(pDataNode->first_node("outSlope")->value());
            pDataNode = pDataNode->next_sibling("data");
        }
    }
    else if (strState == "2")// Random between two curve
    {
        strBuildData.append("3,").append(pszScalar).append(",");
        rapidxml::xml_node<>* pArrayNode = pNode->first_node("minCurve")->first_node("m_Curve")->first_node("Array");
        const TCHAR* pszSize = pArrayNode->first_node("size")->value();
        strBuildData.append(pszSize);
        uint32_t uSize = (uint32_t)_tstof(pszSize);
        rapidxml::xml_node<>* pDataNode = pArrayNode->first_node("data");
        for (uint32_t i = 0; i < uSize; ++i)
        {
            strBuildData.append(",");
            strBuildData.append(pDataNode->first_node("time")->value()).append(",");
            strBuildData.append(pDataNode->first_node("value")->value()).append(",");
            strBuildData.append(pDataNode->first_node("inSlope")->value()).append(",");
            strBuildData.append(pDataNode->first_node("outSlope")->value());
            pDataNode = pDataNode->next_sibling("data");
        }
        strBuildData.append(",");
        pArrayNode = pNode->first_node("maxCurve")->first_node("m_Curve")->first_node("Array");
        pszSize = pArrayNode->first_node("size")->value();
        strBuildData.append(pszSize);
        uSize = (uint32_t)_tstof(pszSize);
        pDataNode = pArrayNode->first_node("data");
        for (uint32_t i = 0; i < uSize; ++i)
        {
            strBuildData.append(",");
            strBuildData.append(pDataNode->first_node("time")->value()).append(",");
            strBuildData.append(pDataNode->first_node("value")->value()).append(",");
            strBuildData.append(pDataNode->first_node("inSlope")->value()).append(",");
            strBuildData.append(pDataNode->first_node("outSlope")->value());
            pDataNode = pDataNode->next_sibling("data");
        }
    }
    else if (strState == "3")// Random between two constant value
    {
        strBuildData.append("2,").append(pszScalar).append(",");

        rapidxml::xml_node<>* pArrayNode = pNode->first_node("minCurve")->first_node("m_Curve")->first_node("Array");
        const TCHAR* pszSize = pArrayNode->first_node("size")->value();
        uint32_t uSize = (uint32_t)_tstof(pszSize);
        BEATS_ASSERT(uSize == 1);
        rapidxml::xml_node<>* pDataNode = pArrayNode->first_node("data");
        strBuildData.append(pDataNode->first_node("value")->value()).append(",");

        pArrayNode = pNode->first_node("maxCurve")->first_node("m_Curve")->first_node("Array");
        pszSize = pArrayNode->first_node("size")->value();
        uSize = (uint32_t)_tstof(pszSize);
        BEATS_ASSERT(uSize == 1);
        pDataNode = pArrayNode->first_node("data");
        strBuildData.append(pDataNode->first_node("value")->value());
    }
    else
    {
        BEATS_ASSERT(false, "Never reach here!");
    }
    pRandomValueProperty->SetValueByString(strBuildData.c_str());
}

TString ConvertToGradientString(rapidxml::xml_node<>* pGradientNode)
{
    TCHAR szBuffer[MAX_PATH];
    TString strData;
    int32_t nColorCount = _tstoi(pGradientNode->first_node("m_NumColorKeys")->value());
    int32_t nAlphaCount = _tstoi(pGradientNode->first_node("m_NumAlphaKeys")->value());
    _stprintf(szBuffer, _T("%d"), nColorCount);
    strData.append(szBuffer);
    strData.append(":");
    for (int32_t i = 0; i < nColorCount; i++)
    {
        if (i != 0)
        {
            strData.append(",");
        }
        _stprintf(szBuffer, "ctime%d", i);
        float fPos = ((float)_tstoi(pGradientNode->first_node(szBuffer)->value())) / 65535.0f;
        _stprintf(szBuffer, "%f", fPos);
        strData.append(szBuffer);
        strData.append(",");
        _stprintf(szBuffer, "key%d", i);
        TString strABGR = pGradientNode->first_node(szBuffer)->first_node("rgba")->value();

        strData.append(strABGR.substr(6, 2));
        strData.append(strABGR.substr(4, 2));
        strData.append(strABGR.substr(2, 2));
        strData.append("00");
    }
    strData.append(_T("@"));
    _stprintf(szBuffer, _T("%d"), nAlphaCount);
    strData.append(szBuffer);
    strData.append(":");
    for (int32_t i = 0; i < nAlphaCount; i++)
    {
        if (i != 0)
        {
            strData.append(",");
        }
        TCHAR szBuffer[64];
        _stprintf(szBuffer, "atime%d", i);
        float fPos = ((float)_tstoi(pGradientNode->first_node(szBuffer)->value())) / 65535.0f;
        _stprintf(szBuffer, "%f", fPos);
        strData.append(szBuffer);
        strData.append(",");

        _stprintf(szBuffer, "key%d", i);
        TString strABGR = pGradientNode->first_node(szBuffer)->first_node("rgba")->value();
        strData.append(strABGR.substr(0, 2));
    }
    return strData;
}

void CParticleEmitter::ParseColorSplineValue(CPropertyDescriptionBase* pProperty, rapidxml::xml_node<>* pNode)
{
    const TCHAR* pszStateString = pNode->first_node("minMaxState")->value();
    TString strData;
    if (_tcsicmp(pszStateString, "0") == 0) // constant color
    {
        const TCHAR* pszABGR = pNode->first_node("maxColor")->first_node("rgba")->value();
        BEATS_ASSERT(_tcslen(pszABGR) == 8);
        TCHAR szColor[11] = { '0', 'x', pszABGR[6], pszABGR[7], pszABGR[4], pszABGR[5], pszABGR[2], pszABGR[3], pszABGR[0], pszABGR[1], 0 };
        strData.append("0,").append(szColor);
    }
    else if (_tcsicmp(pszStateString, "1") == 0) //Gradient
    {
        rapidxml::xml_node<>* pGradientNode = pNode->first_node("maxGradient");
        strData.append("2,").append(ConvertToGradientString(pGradientNode));
    }
    else if (_tcsicmp(pszStateString, "2") == 0)//Random between two Color
    {
        const TCHAR* pszABGR = pNode->first_node("maxColor")->first_node("rgba")->value();
        BEATS_ASSERT(_tcslen(pszABGR) == 8);
        TCHAR szColor[11] = { '0', 'x', pszABGR[6], pszABGR[7], pszABGR[4], pszABGR[5], pszABGR[2], pszABGR[3], pszABGR[0], pszABGR[1], 0 };
        strData.append("1,").append(szColor);

        const TCHAR* pszMinABGR = pNode->first_node("minColor")->first_node("rgba")->value();
        BEATS_ASSERT(_tcslen(pszMinABGR) == 8);
        TCHAR szMinColor[11] = { '0', 'x', pszMinABGR[6], pszMinABGR[7], pszMinABGR[4], pszMinABGR[5], pszMinABGR[2], pszMinABGR[3], pszMinABGR[0], pszMinABGR[1], 0 };
        strData.append("@").append(szMinColor);
    }
    else if (_tcsicmp(pszStateString, "3") == 0)//Random between two Gradient
    {
        rapidxml::xml_node<>* pGradientNode = pNode->first_node("maxGradient");
        strData.append("3,").append(ConvertToGradientString(pGradientNode));
        rapidxml::xml_node<>* pMinGradientNode = pNode->first_node("minGradient");
        strData.append("#").append(ConvertToGradientString(pMinGradientNode));
    }
    else
    {
        BEATS_ASSERT(false, "Never reach here!");
    }
    pProperty->SetValueByString(strData.c_str());
}

#endif

float CParticleEmitter::GetPlayingTime(bool bContainDelay) const
{
    float fRet = m_fPlayingTime;
    if (!bContainDelay)
    {
        fRet -= m_pBasicModule->GetStartDelay();
        if (BEATS_FLOAT_LESS(fRet, 0))
        {
            fRet = 0;
        }
    }
    return fRet;
}

void CParticleEmitter::SetTemplateFlag(bool bFlag)
{
    m_bTemplate = bFlag;
}

bool CParticleEmitter::GetTemplateFlag() const
{
    return m_bTemplate;
}

void CParticleEmitter::SetDepthTestEnable(bool bEnable)
{
    if (GetBasicModule())
    {
        GetBasicModule()->SetDepthTest(bEnable);
    }
    for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
    {
        m_childrenEmitter[i]->SetDepthTestEnable(bEnable);
    }
}

void CParticleEmitter::SetRemoveFromParentWhenStop(bool bFlag)
{
    m_bRemoveParentWhenStop = bFlag;
}

void CParticleEmitter::Reset()
{
   m_bPlaying = false;
   m_bTemplate = false;
   m_bRemoveParentWhenStop = false;
   m_fPlayingTime = 0;
   m_fLastEmitTime = 0;
   m_fLastBurstTime = -1.0f;
   m_uRandomSeed = CParticleManager::GetInstance()->GetParticleRandValue();
   m_uCurrSeedForParticle = 0;
   m_pAttachScene = nullptr;
#ifdef DEVELOP_VERSION
   m_uLastRenderFrameCount = 0;
   m_uLastUpdateFrameCount = 0;
#endif
   SetOwner(nullptr);
   for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
   {
       m_childrenEmitter[i]->Reset();
   }
}

CBasicModule* CParticleEmitter::GetBasicModule()
{
    return m_pBasicModule;
}

CEmissionModule* CParticleEmitter::GetEmissionModule()
{
    return m_pEmissionModule;
}

CShapeModule* CParticleEmitter::GetShapeModule()
{
    return m_pShapeModule;
}

CVelocityModule* CParticleEmitter::GetVelocityModule()
{
    return m_pVelocityModule;
}

CVelocityLimitModule* CParticleEmitter::GetVelocityLimitModule()
{
    return m_pVelocityLimitModule;
}

CForceModule* CParticleEmitter::GetForceModule()
{
    return m_pForceModule;
}

CRenderModule* CParticleEmitter::GetRenderModule()
{
    return m_pRenderModule;
}

CSizeModule* CParticleEmitter::GetSizeModule()
{
    return m_pSizeModule;
}

CRotationModule* CParticleEmitter::GetRotationModule()
{
    return m_pRotationModule;
}

CColorModule* CParticleEmitter::GetColorModule()
{
    return m_pColorModule;
}

CTextureSheetAnimationModule* CParticleEmitter::GetTextureSheetAnimationModule()
{
    return m_pTextureSheetAnimationModule;
}

CParticleEmitter* CParticleEmitter::GetParentEmitter() const
{
    return m_pParentEmitter;
}

void CParticleEmitter::RemoveParticle(CParticle* pParticle, bool bDelete)
{
    BEATS_ASSERT(m_particleList.find(pParticle) != m_particleList.end());
#ifdef _DEBUG
    BEATS_ASSERT(!pParticle->m_bDeleteFlag);
#endif
    m_particleList.erase(pParticle);
    if (bDelete)
    {
        CParticleManager::GetInstance()->RecycleParticle(pParticle);
    }
}

void CParticleEmitter::AddParticle(CParticle* pParticle)
{
    BEATS_ASSERT(m_particleList.find(pParticle) == m_particleList.end());
#ifdef _DEBUG
    BEATS_ASSERT(!pParticle->m_bDeleteFlag);
#endif
    m_particleList.insert(pParticle);
}

uint32_t CParticleEmitter::RequestRandomSeed()
{
    uint32_t uRet = m_uCurrSeedForParticle;
    CParticleManager::GetInstance()->SetCurrentRandSeed(m_uCurrSeedForParticle);
    m_uCurrSeedForParticle = CParticleManager::GetInstance()->GetParticleRandValue();
    return uRet;
}

void CParticleEmitter::ResetRandomSeed()
{
    m_uRandomSeed = CParticleManager::GetInstance()->GetParticleRandValue();
    m_uCurrSeedForParticle = m_uRandomSeed;
    for (auto iter = m_childrenEmitter.begin(); iter != m_childrenEmitter.end(); ++iter)
    {
        (*iter)->ResetRandomSeed();
    }
}

float CParticleEmitter::GetLastEmitTime() const
{
    return m_fLastEmitTime;
}

void CParticleEmitter::SetLastEmitTime(float fTime)
{
    m_fLastEmitTime = fTime;
}

float CParticleEmitter::GetLastBurstTime() const
{
    return m_fLastBurstTime;
}

void CParticleEmitter::SetLastBurstTime(float fTime)
{
    m_fLastBurstTime = fTime;
}

const std::vector<CParticleEmitter*>& CParticleEmitter::GetChildrenEmitter() const
{
    return m_childrenEmitter;
}

void CParticleEmitter::CopyModulesToEmitter(CParticleEmitter* pEmitter) const
{
#ifdef DEVELOP_VERSION
    pEmitter->m_bModuleIsCopy = true;
#endif
    pEmitter->m_pBasicModule = m_pBasicModule;
    pEmitter->m_pShapeModule = m_pShapeModule;
    pEmitter->m_pEmissionModule = m_pEmissionModule;
    pEmitter->m_pColorModule = m_pColorModule;
    pEmitter->m_pForceModule = m_pForceModule;
    pEmitter->m_pRenderModule = m_pRenderModule;
    pEmitter->m_pRotationModule = m_pRotationModule;
    pEmitter->m_pSizeModule = m_pSizeModule;
    pEmitter->m_pTextureSheetAnimationModule = m_pTextureSheetAnimationModule;
    pEmitter->m_pVelocityModule = m_pVelocityModule;
    pEmitter->m_pVelocityLimitModule = m_pVelocityLimitModule;
    BEATS_ASSERT(pEmitter->GetChildrenEmitter().size() == m_childrenEmitter.size());
    for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
    {
        m_childrenEmitter[i]->CopyModulesToEmitter(pEmitter->GetChildrenEmitter()[i]);
    }
}

void CParticleEmitter::ClearModules()
{
#ifdef DEVELOP_VERSION
    BEATS_ASSERT(m_bModuleIsCopy, "Clear non-copy modules for emitter will leak memory.");
#endif
    m_pBasicModule = nullptr;
    m_pShapeModule = nullptr;
    m_pEmissionModule = nullptr;
    m_pColorModule = nullptr;
    m_pForceModule = nullptr;
    m_pRenderModule = nullptr;
    m_pRotationModule = nullptr;
    m_pSizeModule = nullptr;
    m_pTextureSheetAnimationModule = nullptr;
    m_pVelocityModule = nullptr;
    m_pVelocityLimitModule = nullptr;
    for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
    {
        m_childrenEmitter[i]->ClearModules();
    }
}

bool CParticleEmitter::IsAutoRecycle() const
{
    return m_bAutoRecycle;
}

void CParticleEmitter::SetAutoRecycle(bool bAutoRecycle)
{
    m_bAutoRecycle = bAutoRecycle;
}

bool CParticleEmitter::IsRenderWithOwner() const
{
    return m_bRenderWithOwner;
}

void CParticleEmitter::SetRenderWithOwner(bool bRenderWithOwner)
{
    m_bRenderWithOwner = bRenderWithOwner;
}

void CParticleEmitter::SetRenderWithOwnerFlag(bool bOwnerRenderFlag)
{
    m_bOwnerRenderFlag = bOwnerRenderFlag;
}

void CParticleEmitter::SetParticleScale(float fScale)
{
    BEATS_ASSERT(BEATS_FLOAT_GREATER(fScale, 0));
    m_fParticleScale = fScale;
}

void CParticleEmitter::SetParticleScaleFactor(float fScaleFactor)
{
    BEATS_ASSERT(BEATS_FLOAT_GREATER(fScaleFactor, 0));
    m_fParticleScaleFactor = fScaleFactor;
}

float CParticleEmitter::GetParticleScale(bool bInherit)
{
    float fRet = m_fParticleScale;
    if (m_pParentEmitter == nullptr)
    {
        fRet *= m_fParticleScaleFactor;
        if (m_pBasicModule && !m_pBasicModule->Is3DEmitter())
        {
            fRet *= CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetScaleFactor();
        }
    }
    else if (bInherit)
    {
        fRet *= m_pParentEmitter->GetParticleScale(true);
    }
    return fRet;
}

void CParticleEmitter::RefreshAllParticle()
{
    for (auto iter = m_particleList.begin(); iter != m_particleList.end(); ++iter)
    {
        (*iter)->InitByEmitter();
    }
    for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
    {
        m_childrenEmitter[i]->RefreshAllParticle();
    }
}

void CParticleEmitter::SetPathFindingGridIndex(int32_t nIndex)
{
    m_nPathFindingGridIndex = nIndex;
}

bool CParticleEmitter::IsFinished()
{
    bool bIsFinished = false;
    if (!m_bPaused)
    {
        bool bTimeUp = (!GetBasicModule()->IsLoop() && GetPlayingTime(false) > GetBasicModule()->GetDuration()) || !IsActive();
        bIsFinished = !m_bPlaying || m_pEmissionModule == nullptr || (bTimeUp && m_particleList.size() == 0);
        for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
        {
            bIsFinished = bIsFinished && m_childrenEmitter[i]->IsFinished();
        }
    }
    return bIsFinished;
}

void CParticleEmitter::InitChildEmitter()
{
    bool bIs3DEmitter = true;
    if (m_pBasicModule)
    {
        bIs3DEmitter = m_pBasicModule->Is3DEmitter();
    }
    for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
    {
        CParticleEmitter* pCurrEmitter = m_childrenEmitter[i];
        if (pCurrEmitter != nullptr)
        {
            if (pCurrEmitter->GetParentNode() != this)
            {
                BEATS_ASSERT(pCurrEmitter->GetParentNode() == nullptr);
                AddChild(pCurrEmitter);
            }
            if (pCurrEmitter->m_pParentEmitter != this)
            {
                BEATS_ASSERT(pCurrEmitter->m_pParentEmitter == nullptr);
                pCurrEmitter->m_pParentEmitter = this;
            }
            pCurrEmitter->Set3DRender(bIs3DEmitter);
        }
    }
}

void CParticleEmitter::Set3DRender(bool b3DRender)
{
    if (GetBasicModule())
    {
        GetBasicModule()->Set3DEmitterFlag(b3DRender);
    }
    for (size_t i = 0; i < m_childrenEmitter.size(); ++i)
    {
        if (m_childrenEmitter[i])
        {
            m_childrenEmitter[i]->Set3DRender(b3DRender);
        }
    }
}
