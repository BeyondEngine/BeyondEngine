#include "stdafx.h"
#include "Particle.h"
#include "ShapeModule.h"
#include "ParticleEmitter.h"
#include "RandomValue.h"
#include "VelocityModule.h"
#include "VelocityLimitModule.h"
#include "ForceModule.h"
#include "Render/RenderGroupManager.h"
#include "Render/RenderBatch.h"
#include "Render/RenderGroup.h"
#include "Render/RenderManager.h"
#include "RenderModule.h"
#include "SizeModule.h"
#include "Render/Material.h"
#include "Render/CommonTypes.h"
#include "RotationModule.h"
#include "TextureSheetAnimationModule.h"
#include "ParticleManager.h"
#include "ColorModule.h"
#include "RandomColor.h"
#include "BasicModule.h"
#include "Render/RenderTarget.h"

CParticle::CParticle()
{
#ifdef DEVELOP_VERSION
    CParticleManager::GetInstance()->m_particleRegisterSet.insert(this);
#endif
}

CParticle::~CParticle()
{
#ifdef DEVELOP_VERSION
    CParticleManager::GetInstance()->m_particleRegisterSet.erase(this);
#endif
}

void CParticle::SetEmitter(CParticleEmitter* pEmitter)
{
    if (m_pEmitter != pEmitter)
    {
        m_pEmitter = pEmitter;
        InitByEmitter();
    }
}

void CParticle::Reset()
{
    SetEmitter(nullptr);
    m_fLifeTime = 0;
    m_fElapsedLifeTime = 0;
    m_uRandomSeed = 0;
    m_initVelocity.Fill(0, 0, 0);
    m_currVelocity.Fill(0, 0, 0);
    m_initColor = 0xFFFFFFFF;
    m_fInitSize = 1.0f;
    m_fParticleRotationInRadians = 0;
    SetPosition(CVec3(0, 0, 0));
    SetRotation(CVec3(0, 0, 0));
    SetScale(CVec3(1,1,1));
#ifdef _DEBUG
    m_bDeleteFlag = false;
#endif
}

void CParticle::InitByEmitter()
{
    BEATS_ASSERT(GetParentNode() == nullptr);
    if (m_pEmitter != nullptr)
    {
        if (m_pEmitter->GetBasicModule()->IsParticleAttach())
        {
            SetPosition(CVec3(0, 0, 0));
        }
        else
        {
            SetPosition(m_pEmitter->GetWorldPosition());
        }
        m_uRandomSeed = m_pEmitter->RequestRandomSeed();
        float fDuration = m_pEmitter->GetBasicModule()->GetDuration();
        float fEmitterTimeRate = 0;
        if (BEATS_FLOAT_GREATER(fDuration, 0))
        {
            float fPlayingTimeInDuration = BEATS_FMOD(m_pEmitter->GetPlayingTime(false), fDuration);
            fEmitterTimeRate = fPlayingTimeInDuration / fDuration;
        }
        m_initColor = m_pEmitter->GetBasicModule()->GetInitColor().GetColor(fEmitterTimeRate, PARTICLE_RAND_RANGE(0, 1));
        m_vertices[0].color = m_initColor;
        m_vertices[1].color = m_initColor;
        m_vertices[2].color = m_initColor;
        m_vertices[3].color = m_initColor;
        CRenderModule* pRenderModule = m_pEmitter->GetRenderModule();
        if (pRenderModule && pRenderModule->GetTextureFrag())
        {
            SharePtr<CTextureFrag> textureFrag = pRenderModule->GetTextureFrag();
            if (pRenderModule->GetRenderMode() == EParticleRenderMode::ePRM_StretchedBillboard)
            {
                m_vertices[0].tex = textureFrag->GetQuadT().tl;
                m_vertices[1].tex = textureFrag->GetQuadT().tr;
                m_vertices[2].tex = textureFrag->GetQuadT().br;
                m_vertices[3].tex = textureFrag->GetQuadT().bl;
            }
            else
            {
                m_vertices[0].tex = textureFrag->GetQuadT().tl;
                m_vertices[1].tex = textureFrag->GetQuadT().bl;
                m_vertices[2].tex = textureFrag->GetQuadT().br;
                m_vertices[3].tex = textureFrag->GetQuadT().tr;
            }
        }
        CParticleManager::GetInstance()->SetCurrentRandSeed(m_uRandomSeed);
        m_fEmitterScale = m_pEmitter->GetParticleScale();
        m_fLifeTime = m_pEmitter->GetBasicModule()->GetInitLifeTime().GetValue(fEmitterTimeRate, PARTICLE_RAND_RANGE(0, 1));
        float fSpeed = m_pEmitter->GetBasicModule()->GetInitSpeed().GetValue(fEmitterTimeRate, PARTICLE_RAND_RANGE(0, 1)) * m_fEmitterScale;
        m_fInitSize = m_pEmitter->GetBasicModule()->GetInitSize().GetValue(fEmitterTimeRate, PARTICLE_RAND_RANGE(0, 1)) * m_fEmitterScale;
        m_fParticleRotationInRadians = m_pEmitter->GetBasicModule()->GetInitRotation().GetValue(fEmitterTimeRate, PARTICLE_RAND_RANGE(0, 1));
        BEATS_ASSERT(BEATS_FLOAT_GREATER_EQUAL(fEmitterTimeRate, 0) && BEATS_FLOAT_LESS_EQUAL(fEmitterTimeRate, 1), "fEmitterTimeRate should in range [0, 1], current value is %f, name is %s", fEmitterTimeRate, m_pEmitter->GetParentEmitter()->GetName().c_str());
        m_initVelocity = CVec3(0, 0, fSpeed);
        CShapeModule* pShapeModule = m_pEmitter->GetShapeModule();
        if (pShapeModule != nullptr)
        {
            CVec3 pos, direction;
            pShapeModule->CalcPosAndDirection(pos, direction, m_fEmitterScale);
            pos *= m_pEmitter->GetWorldTM().GetScale();
            if (!m_pEmitter->GetBasicModule()->IsParticleAttach())
            {
                pos += m_pEmitter->GetWorldPosition();
                direction *= m_pEmitter->GetWorldQuaternion();
                CVec3 rotation;
                m_pEmitter->GetWorldTM().ToQuaternion().ToPitchYawRoll(rotation.X(), rotation.Y(), rotation.Z());
                rotation.X() = RadiansToDegrees(rotation.X());
                rotation.Y() = RadiansToDegrees(rotation.Y());
                rotation.Z() = RadiansToDegrees(rotation.Z());
                SetRotation(rotation);
            }
            SetPosition(pos);
            m_initVelocity = direction * fSpeed;
        }
    }
}

void CParticle::Update(float dtt)
{
#ifdef DEVELOP_VERSION
    if (!CParticleManager::GetInstance()->m_bUpdateSwitcher)
    {
        return;
    }
    ++CParticleManager::GetInstance()->m_uParticleUpdatedCount;
#endif
    m_fElapsedLifeTime += dtt;
    if (m_fElapsedLifeTime >= m_fLifeTime)
    {
        m_pEmitter->RemoveParticle(this, true);
    }
    else
    {
        CQuaternion localQuaternion = m_pEmitter->GetWorldQuaternion().Inverse();
        CQuaternion currQuaternion = GetWorldQuaternion();
        BEATS_ASSERT(!BEATS_FLOAT_EQUAL(m_fLifeTime, 0));
        float fLifeTimeRate = m_fElapsedLifeTime / m_fLifeTime;
        CParticleManager::GetInstance()->SetCurrentRandSeed(m_uRandomSeed + 54321);//Add a magic number 54321 to make it different from srand(m_uRandomSeed) in InitByEmitter;
        m_currVelocity = m_initVelocity;
        CVelocityModule* pVelocityModule = m_pEmitter->GetVelocityModule();
        if (pVelocityModule != nullptr)
        {
            BEATS_ASSERT(pVelocityModule->IsInitialized());
            CVec3 velocity;
            velocity.X() = pVelocityModule->GetX().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1));
            velocity.Y() = pVelocityModule->GetY().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1));
            velocity.Z() = pVelocityModule->GetZ().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1));
            velocity *= m_fEmitterScale;
            if (m_pEmitter->GetBasicModule()->IsParticleAttach())
            {
                if (!pVelocityModule->IsLocal())
                {
                    velocity *= localQuaternion;
                }
            }
            else
            {
                if (pVelocityModule->IsLocal())
                {
                    velocity *= currQuaternion;
                }
            }
            m_currVelocity += velocity;
        }
        CForceModule* pForceModule = m_pEmitter->GetForceModule();
        if (pForceModule != nullptr)
        {
            BEATS_ASSERT(pForceModule->IsInitialized());
            uint32_t uSeed = CParticleManager::GetInstance()->GetParticleRandValue();
            if (pForceModule->IsRandomizeEveryFrame())
            {
                CParticleManager::GetInstance()->SetCurrentRandSeed(CEngineCenter::GetInstance()->GetFrameCounter());
            }
            CVec3 force(pForceModule->GetX().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1)),
                pForceModule->GetY().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1)),
                pForceModule->GetZ().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1)));
            force *= m_fEmitterScale;
            if (m_pEmitter->GetBasicModule()->IsParticleAttach())
            {
                if (!pForceModule->IsLocal())
                {
                    force *= localQuaternion;
                }
            }
            else
            {
                if (pForceModule->IsLocal())
                {
                    force *= currQuaternion;
                }
            }
            m_currVelocity += (force * m_fElapsedLifeTime);
            CParticleManager::GetInstance()->SetCurrentRandSeed(uSeed);
        }
        if (!BEATS_FLOAT_EQUAL(m_pEmitter->GetBasicModule()->GetGravityModifier(), 0))
        {
            CVec3 gravityDirection(0, -1.0f, 0);
            if (m_pEmitter->GetBasicModule()->IsParticleAttach())
            {
                gravityDirection *= localQuaternion;
            }
            CVec3 gravityPlusVelocity = gravityDirection * (m_pEmitter->GetBasicModule()->GetGravityModifier() * 9.8f * m_fElapsedLifeTime * m_fEmitterScale);
            m_currVelocity += gravityPlusVelocity;
        }
        CVelocityLimitModule* pVelocityLimitModule = m_pEmitter->GetVelocityLimitModule();
        if (pVelocityLimitModule != nullptr)
        {
            BEATS_ASSERT(pVelocityLimitModule->IsInitialized());
            m_currVelocity = LimitVelocity(m_currVelocity, localQuaternion);
        }

        SetPosition(GetPosition() + m_currVelocity * dtt);

        CRotationModule* pRotationModule = m_pEmitter->GetRotationModule();
        if (pRotationModule != nullptr)
        {
            BEATS_ASSERT(pRotationModule->IsInitialized());
            float fAngularSpeedInRadians = DegreesToRadians(pRotationModule->GetAngularSpeed().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1)));
            m_fParticleRotationInRadians += fAngularSpeedInRadians * dtt;
        }
    }
}

void CParticle::Render()
{
#ifdef DEVELOP_VERSION
    if (!CParticleManager::GetInstance()->m_bRenderSwitcher)
    {
        return;
    }
    ++CParticleManager::GetInstance()->m_uParticleRenderedCount;
#endif
    float fLifeTimeRate = m_fElapsedLifeTime / m_fLifeTime;
    CRenderModule* pRenderModule = m_pEmitter->GetRenderModule();
    if (pRenderModule)
    {
        BEATS_ASSERT(pRenderModule->IsInitialized());
        CParticleManager::GetInstance()->SetCurrentRandSeed(m_uRandomSeed + 12345);//Add a magic number 12345 to make it different from srand(m_uRandomSeed) in update;
        CTextureSheetAnimationModule* pSheetAnimationModule = m_pEmitter->GetTextureSheetAnimationModule();
        if (pSheetAnimationModule != nullptr)
        {
            BEATS_ASSERT(pSheetAnimationModule->IsInitialized());
            uint32_t uTilesCount = pSheetAnimationModule->GetRow() * pSheetAnimationModule->GetCol();
            BEATS_ASSERT(uTilesCount > 1, "tiles must greater than 1 in emitter %s!", m_pEmitter->GetName().c_str());
            if (uTilesCount > 1)
            {
                fLifeTimeRate *= pSheetAnimationModule->GetLoopTimes();
                while (BEATS_FLOAT_GREATER(fLifeTimeRate, 1.0f))
                {
                    fLifeTimeRate -= 1.0f;
                }
                float fFrame = pSheetAnimationModule->GetFrame().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1.f));
                BEATS_CLIP_VALUE(fFrame, 0, 1);
                uint32_t uX = 0;
                uint32_t uY = 0;
                if (pSheetAnimationModule->IsWholeSheetOrSingleRow())
                {
                    uint32_t uIndex = (uint32_t)(fFrame * uTilesCount);
                    uX = uIndex % pSheetAnimationModule->GetCol();
                    uY = uIndex / pSheetAnimationModule->GetCol();
                }
                else
                {
                    uX = pSheetAnimationModule->GetSpecificRow();
                    if (pSheetAnimationModule->IsRandomRow())
                    {
                        uX = (uint32_t)PARTICLE_RAND_RANGE(0, (float)pSheetAnimationModule->GetRow());
                    }
                    BEATS_ASSERT(uX < pSheetAnimationModule->GetRow());
                    uY = (uint32_t)(fFrame * pSheetAnimationModule->GetCol());
                }
                BEATS_ASSERT(pSheetAnimationModule->GetCol() > 0);
                BEATS_ASSERT(pSheetAnimationModule->GetRow() > 0);
                SharePtr<CTextureFrag> textureFrag = pRenderModule->GetTextureFrag();
                if (textureFrag != nullptr)
                {
                    const CQuadT& quadT = textureFrag->GetQuadT();
                    float fUWidth = quadT.br.u - quadT.bl.u;
                    float fUHeight = quadT.tl.v - quadT.bl.v;
                    BEATS_ASSERT(fUHeight > 0 && fUWidth > 0);
                    float fWidthForSingle = fUWidth / pSheetAnimationModule->GetCol();
                    float fHeightForSingle = fUHeight / pSheetAnimationModule->GetRow();
                    CTex uvForLeftUpPos(quadT.tl);
                    uvForLeftUpPos.u += fWidthForSingle * uX;
                    uvForLeftUpPos.v -= fHeightForSingle * uY;
                    m_vertices[0].tex = uvForLeftUpPos;
                    m_vertices[1].tex = uvForLeftUpPos;
                    m_vertices[1].tex.v -= fHeightForSingle;
                    m_vertices[2].tex = uvForLeftUpPos;
                    m_vertices[2].tex.u += fWidthForSingle;
                    m_vertices[2].tex.v -= fHeightForSingle;
                    m_vertices[3].tex = uvForLeftUpPos;
                    m_vertices[3].tex.u += fWidthForSingle;
                }
            }
        }
        BEATS_ASSERT(m_renderGroupID == LAYER_UNSET && CRenderGroupManager::GetInstance()->GetRenderGroupIDStack().size() > 0);
        ERenderGroupID groupId = CRenderGroupManager::GetInstance()->GetRenderGroupIDStack().back();
        CRenderBatch* pRenderBatch = pRenderModule->RequestRenderBatch(m_pEmitter->GetBasicModule()->IsDepthTestEnable(), groupId);
        static unsigned short pIndex[6] = { 0, 1, 2, 0, 2, 3 };
        CCamera* pCamera = CRenderManager::GetInstance()->GetCamera(m_pEmitter->GetBasicModule()->Is3DEmitter() ? CCamera::eCT_3D : CCamera::eCT_2D);
        BEATS_ASSERT(pCamera);
        const CMat4& viewMatInverse = pCamera->GetViewMatrixInverse();
        EParticleRenderMode renderMode = pRenderModule->GetRenderMode();
        CSizeModule* pSizeModule = m_pEmitter->GetSizeModule();
        float fSize = m_fInitSize;
        if (pSizeModule != nullptr)
        {
            BEATS_ASSERT(pSizeModule->IsInitialized());
            fSize *= pSizeModule->GetSize().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0,1));
        }
        float fHalfWidth = fSize * 0.5f;
        float fHalfHeight = fHalfWidth;
        CVec3 billboardRight;
        CVec3 billboardUp;
        CVec3 billboardForward;
        CVec3 particlePos = GetWorldPosition();
        if (m_pEmitter->GetBasicModule()->IsParticleAttach())
        {
            particlePos *= m_pEmitter->GetWorldQuaternion();
            particlePos += m_pEmitter->GetWorldPosition();
        }
        switch (renderMode)
        {
        case EParticleRenderMode::ePRM_Billboard:
        {
            billboardUp = viewMatInverse.GetUpVec3();
            billboardRight = viewMatInverse.GetRightVec3();
            billboardForward = viewMatInverse.GetForwardVec3();
        }
            break;
        case EParticleRenderMode::ePRM_StretchedBillboard:
        {
            if (m_currVelocity.LengthSq() == 0)
            {
                billboardUp.Fill(0, 0, 1);
            }
            else
            {
                billboardUp = m_currVelocity;
            }
            if (m_pEmitter->GetBasicModule()->IsParticleAttach())
            {
                billboardUp *= m_pEmitter->GetWorldQuaternion();
            }
            billboardUp.Normalize();
            CVec3 cameraToParticle = pCamera->GetViewPos() - particlePos;
            cameraToParticle.Normalize();
            if (billboardUp == cameraToParticle)
            {
                billboardRight.Fill(1, 0, 0);
            }
            else
            {
                billboardRight = billboardUp.Cross(cameraToParticle);
            }
            billboardRight.Normalize();
            billboardForward = billboardRight.Cross(billboardUp);
            billboardForward.Normalize();
            float fCurrSpeed = m_currVelocity.Length();
            fHalfHeight = (fHalfHeight * 2 * pRenderModule->GetLengthScale() + pRenderModule->GetSpeedScale() * fCurrSpeed) * 0.5f;
        }
            break;
        case EParticleRenderMode::ePRM_HorizontalBillboard:
        {
            billboardForward.Fill(0, 1.0f, 0);
            billboardRight.Fill(-1.0f, 0, 0);
            billboardUp.Fill(0, 0, 1.0f);
        }
            break;
        case EParticleRenderMode::ePRM_VerticalBillboard:
        {
            billboardUp = CVec3(0, 1.0f, 0);
            billboardRight = viewMatInverse.GetRightVec3();
            billboardForward = billboardUp.Cross(billboardRight);
            billboardForward.Normalize();
        }
            break;
        default:
            BEATS_ASSERT(false, "never reach here!");
            break;
        }
        if (renderMode != EParticleRenderMode::ePRM_StretchedBillboard)
        {
            if (!BEATS_FLOAT_EQUAL(m_fParticleRotationInRadians, 0))
            {
                CQuaternion quat;
                quat.FromAxisAngle(billboardForward, m_fParticleRotationInRadians);
                billboardRight *= quat;
                billboardUp *= quat;
            }
        }
#ifdef _DEBUG
        if (m_pEmitter->GetBasicModule()->Is3DEmitter())
        {
            CMat4 orthogonalMat;
            orthogonalMat[0] = billboardRight.X();
            orthogonalMat[1] = billboardRight.Y();
            orthogonalMat[2] = billboardRight.Z();

            orthogonalMat[4] = billboardUp.X();
            orthogonalMat[5] = billboardUp.Y();
            orthogonalMat[6] = billboardUp.Z();

            orthogonalMat[8] = billboardForward.X();
            orthogonalMat[9] = billboardForward.Y();
            orthogonalMat[10] = billboardForward.Z();

            orthogonalMat[15] = 1.0f;
            BEATS_ASSERT(orthogonalMat.IsOrthogonal());
        }
#endif
        if (renderMode == EParticleRenderMode::ePRM_StretchedBillboard)
        {
            /*
            1__________0
            |          | pos __\   particlePos is at the right middle.
            |__________|       /   up direction, also the velocity direction.
            2           3
            */
            m_vertices[0].position = particlePos - billboardRight * fHalfWidth;
            m_vertices[1].position = m_vertices[0].position - billboardUp * fHalfHeight * 2;
            m_vertices[2].position = m_vertices[1].position + billboardRight * fHalfWidth * 2;
            m_vertices[3].position = m_vertices[2].position + billboardUp * fHalfHeight * 2;
        }
        else
        {
            /*
                 up
            0__________3
            |    pos   |    particle pos is the center.
            |__________|
            1           2
            */
            m_vertices[0].position = particlePos + billboardUp * fHalfHeight - billboardRight * fHalfWidth;//Left Up
            m_vertices[1].position = m_vertices[0].position - billboardUp * fHalfHeight * 2;//Left Down
            m_vertices[2].position = m_vertices[1].position + billboardRight * fHalfWidth * 2;//Right Down
            m_vertices[3].position = m_vertices[2].position + billboardUp * fHalfHeight * 2;//Right Up
        }

        CColorModule* pColorModule = m_pEmitter->GetColorModule();
        if (pColorModule != nullptr)
        {
            BEATS_ASSERT(pColorModule->IsInitialized());
            CColor color = pColorModule->GetColor(fLifeTimeRate);

            CColor mergeColor;
            mergeColor.r = (unsigned char)(((float)color.r / 255.0f) * (float)m_initColor.r);
            mergeColor.g = (unsigned char)(((float)color.g / 255.0f) * (float)m_initColor.g);
            mergeColor.b = (unsigned char)(((float)color.b / 255.0f) * (float)m_initColor.b);
            mergeColor.a = (unsigned char)(((float)color.a / 255.0f) * (float)m_initColor.a);

            m_vertices[0].color = mergeColor;
            m_vertices[1].color = mergeColor;
            m_vertices[2].color = mergeColor;
            m_vertices[3].color = mergeColor;
        }
        pRenderBatch->AddIndexedVertices(m_vertices, 4, pIndex, 6);
#ifdef DEVELOP_VERSION
        if (pRenderBatch->m_usage == ERenderBatchUsage::eRBU_Count)
        {
            pRenderBatch->m_usage = ERenderBatchUsage::eRBU_Particle;
        }
        BEATS_ASSERT(pRenderBatch->m_usage == ERenderBatchUsage::eRBU_Particle);
#endif

#ifdef EDITOR_MODE
        if (CParticleManager::GetInstance()->m_bRenderParticleEdge)
        {
            if (m_pEmitter->GetBasicModule()->Is3DEmitter())
            {
                CVertexPC startPos, endPos;
                startPos.color = endPos.color = 0xFFFFFFFF;
                startPos.position = m_vertices[0].position;
                endPos.position = m_vertices[1].position;
                CRenderManager::GetInstance()->RenderLine(startPos, endPos);
                endPos.position = m_vertices[3].position;
                CRenderManager::GetInstance()->RenderLine(startPos, endPos);
                startPos.position = m_vertices[2].position;
                CRenderManager::GetInstance()->RenderLine(startPos, endPos);
                endPos.position = m_vertices[1].position;
                CRenderManager::GetInstance()->RenderLine(startPos, endPos);
                startPos.position = particlePos;
                startPos.color = 0xFF0000FF;
                endPos.position = particlePos - billboardUp * fHalfHeight * 2;
                endPos.color = 0xFFFF00FF;
                CRenderManager::GetInstance()->RenderLine(startPos, endPos, 2.0f);
            }
            else
            {
                float fScaleFactor = CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetScaleFactor();
                float fTmp = 1.0f / fScaleFactor;
                CVertexPC startPos, endPos;
                startPos.color = endPos.color = 0xFFFFFFFF;
                m_vertices[0].position *= fTmp;
                m_vertices[1].position *= fTmp;
                m_vertices[2].position *= fTmp;
                m_vertices[3].position *= fTmp;
                startPos.position = m_vertices[0].position;
                endPos.position = m_vertices[1].position;
                CRenderManager::GetInstance()->Render2DLine(startPos, endPos);
                endPos.position = m_vertices[3].position;
                CRenderManager::GetInstance()->Render2DLine(startPos, endPos);
                startPos.position = m_vertices[2].position;
                CRenderManager::GetInstance()->Render2DLine(startPos, endPos);
                endPos.position = m_vertices[1].position;
                CRenderManager::GetInstance()->Render2DLine(startPos, endPos);
                startPos.position = particlePos * fTmp;
                startPos.color = 0xFF0000FF;
                endPos.position = (particlePos - billboardUp * fHalfHeight * 2) * fTmp;
                endPos.color = 0xFFFF00FF;
                CRenderManager::GetInstance()->Render2DLine(startPos, endPos);
            }
        }
#endif
#ifdef DEVELOP_VERSION
        std::map<TString, SParticleDetail*>& particleDetailMap = CParticleManager::GetInstance()->m_particleDetailMap;
        if (particleDetailMap.find(m_pEmitter->GetName()) == particleDetailMap.end())
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
            particleDetailMap[m_pEmitter->GetName()] = pParticleDetail;
        }
        particleDetailMap[m_pEmitter->GetName()]->RenderCount++;
#endif
    }
}

CVec3 CParticle::LimitVelocity(const CVec3& curVelocity, const CQuaternion& quaterionToLocal)
{
    CVec3 ret = curVelocity;
    if (BEATS_FLOAT_GREATER(curVelocity.LengthSq(), 0))
    {
        CVelocityLimitModule* pVelocityLimitModule = m_pEmitter->GetVelocityLimitModule();
        BEATS_ASSERT(pVelocityLimitModule != nullptr);
        float fDampen = pVelocityLimitModule->GetDampen();
        float fLifeTimeRate = m_fElapsedLifeTime / m_fLifeTime;
        if (!pVelocityLimitModule->IsSeparateAxis())
        {
            float fSpeed = curVelocity.Length();
            float fLimitSpeed = pVelocityLimitModule->GetSpeed().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1)) * m_fEmitterScale;
            if (BEATS_FLOAT_GREATER(fSpeed, fLimitSpeed))
            {
                fSpeed = CalculateValueByDampen(fSpeed, fLimitSpeed, (uint32_t)(m_fElapsedLifeTime * 1000), fDampen);
                ret.Normalize();
                ret = ret * fSpeed;
            }
        }
        else
        {
            CVec3 velocityLimit(pVelocityLimitModule->GetX().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1)),
                pVelocityLimitModule->GetY().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1)),
                pVelocityLimitModule->GetZ().GetValue(fLifeTimeRate, PARTICLE_RAND_RANGE(0, 1)));
            velocityLimit *= m_fEmitterScale;
            BEATS_ASSERT(BEATS_FLOAT_GREATER_EQUAL(velocityLimit.X(), 0));
            BEATS_ASSERT(BEATS_FLOAT_GREATER_EQUAL(velocityLimit.Y(), 0));
            BEATS_ASSERT(BEATS_FLOAT_GREATER_EQUAL(velocityLimit.Z(), 0));
            if (m_pEmitter->GetBasicModule()->IsParticleAttach())
            {
                if (!pVelocityLimitModule->IsLocal())
                {
                    velocityLimit *= quaterionToLocal;
                }
            }
            else
            {
                if (pVelocityLimitModule->IsLocal())
                {
                    velocityLimit *= GetWorldQuaternion();
                }
            }
            if (BEATS_FLOAT_GREATER(fabs(curVelocity.X()), velocityLimit.X()))
            {
                ret.X() = CalculateValueByDampen(fabs(curVelocity.X()), velocityLimit.X(), (uint32_t)(m_fElapsedLifeTime * 1000), fDampen);
                ret.X() *= BEATS_FLOAT_GREATER_EQUAL(curVelocity.X(), 0) ? 1 : -1;
            }
            if (BEATS_FLOAT_GREATER(fabs(curVelocity.Y()), velocityLimit.Y()))
            {
                ret.Y() = CalculateValueByDampen(fabs(curVelocity.Y()), velocityLimit.Y(), (uint32_t)(m_fElapsedLifeTime * 1000), fDampen);
                ret.Y() *= BEATS_FLOAT_GREATER_EQUAL(curVelocity.Y(), 0) ? 1 : -1;
            }
            if (BEATS_FLOAT_GREATER(fabs(curVelocity.Z()), velocityLimit.Z()))
            {
                ret.Z() = CalculateValueByDampen(fabs(curVelocity.Z()), velocityLimit.Z(), (uint32_t)(m_fElapsedLifeTime * 1000), fDampen);
                ret.Z() *= BEATS_FLOAT_GREATER_EQUAL(curVelocity.Z(), 0) ? 1 : -1;
            }
        }
    }
    return ret;
}

float CParticle::CalculateValueByDampen(float fOriginalValue, float fLimitValue, uint32_t uElapsedTimeMS, float fDampen)
{
    if (BEATS_FLOAT_GREATER(fOriginalValue, fLimitValue))
    {
        uint32_t uFrameIntervalMS = CEngineCenter::GetInstance()->GetFrameTimeUS() / 1000;
        while (uElapsedTimeMS != 0)
        {
            float fRate = 1.0f;
            if (uElapsedTimeMS >= uFrameIntervalMS)
            {
                uElapsedTimeMS -= uFrameIntervalMS;
            }
            else
            {
                fRate = (float)uElapsedTimeMS / uFrameIntervalMS;
                uElapsedTimeMS = 0;
            }
            fOriginalValue = fOriginalValue * (1.0f - fDampen * fRate);
            if (BEATS_FLOAT_LESS(fOriginalValue, fLimitValue))
            {
                fOriginalValue = fLimitValue;
                break;
            }
        }
    }
    return fOriginalValue;
}