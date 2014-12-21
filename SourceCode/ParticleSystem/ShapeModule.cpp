#include "stdafx.h"
#include "ShapeModule.h"
#include "ParticleEmitter.h"
#include "Particle.h"
#include "Render/ShapeRenderer.h"
#include "Render/RenderManager.h"

CShapeModule::CShapeModule()
{

}

CShapeModule::~CShapeModule()
{

}

void CShapeModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_emitShapeType, true, 0xFFFFFFFF, "发射形状", nullptr, "定义了发射点的空间集合，以及初始速率所对应的方向", nullptr);
    DECLARE_PROPERTY(serializer, m_fRadius, true, 0xFFFFFFFF, "半径", nullptr, nullptr, "VisibleWhen:m_emitShapeType!=eEST_Box && m_emitShapeType!=eEST_Mesh");
    DECLARE_PROPERTY(serializer, m_bEmitFromShell, true, 0xFFFFFFFF, "从表面发射", nullptr, nullptr, "VisibleWhen:m_emitShapeType!=eEST_Box && m_emitShapeType!=eEST_Mesh&& m_emitShapeType!=eEST_Edge");
    DECLARE_PROPERTY(serializer, m_fAngle, true, 0xFFFFFFFF, "角度", nullptr, nullptr, "VisibleWhen:m_emitShapeType==eEST_Cone");
    DECLARE_PROPERTY(serializer, m_fArcForCircle, true, 0xFFFFFFFF, "角度", nullptr, nullptr, "VisibleWhen:m_emitShapeType==eEST_Circle");
    DECLARE_PROPERTY(serializer, m_bEmitFromBaseOrVolume, true, 0xFFFFFFFF, "仅从锥底", nullptr, "如果为真，则发射点在锥底，否则在锥空间内", "VisibleWhen:m_emitShapeType==eEST_Cone");
    DECLARE_PROPERTY(serializer, m_fConeLength, true, 0xFFFFFFFF, "锥高度", nullptr, nullptr, "VisibleWhen:m_emitShapeType==eEST_Cone");
    DECLARE_PROPERTY(serializer, m_boxSize, true, 0xFFFFFFFF, "BOX尺寸", nullptr, nullptr, "VisibleWhen:m_emitShapeType==eEST_Box, DefaultValue:1@1@1");
    DECLARE_PROPERTY(serializer, m_bRandomDirection, true, 0xFFFFFFFF, "随机朝向", nullptr, nullptr, nullptr);
}

void CShapeModule::Render(CParticleEmitter* pEmitter)
{
    BEATS_ASSERT(pEmitter != nullptr);
    const CMat4& emitterMat = pEmitter->GetWorldTM();
    switch (m_emitShapeType)
    {
    case EEmitShapeType::eEST_Sphere:
    case EEmitShapeType::eEST_HemiSphere:
    {
        CMat4 emitterSphereMat;
        emitterSphereMat.FromPitchYawRoll(DegreesToRadians(90), 0, 0);
        emitterSphereMat = emitterMat * emitterSphereMat;
        bool bHemiSphere = m_emitShapeType == EEmitShapeType::eEST_HemiSphere;
        float fRate = bHemiSphere ? 0.5f : 1.0f;
        CShapeRenderer::GetInstance()->DrawCircle(emitterSphereMat, m_fRadius, 0x00FF00FF, false, 1.0f);
        CMat4 mat;
        mat.FromPitchYawRoll(0, MATH_PI_HALF, MATH_PI_HALF);
        mat = emitterSphereMat * mat;
        CShapeRenderer::GetInstance()->DrawCircle(mat, m_fRadius, 0xFF0000FF, false, fRate);
        mat.FromPitchYawRoll(MATH_PI, 0, -MATH_PI_HALF);
        mat = emitterSphereMat * mat;
        CShapeRenderer::GetInstance()->DrawCircle(mat, m_fRadius, 0x0000FFFF, false, fRate);
    }
        break;
    case EEmitShapeType::eEST_Cone:
    {
        CMat4 emitterConeMat;
        emitterConeMat.FromPitchYawRoll(DegreesToRadians(90), 0, 0);
        emitterConeMat = emitterMat * emitterConeMat;
        float fTopRadius = m_fRadius + m_fConeLength * tanf(DegreesToRadians(m_fAngle));
        CShapeRenderer::GetInstance()->DrawCone(emitterConeMat, m_fRadius, fTopRadius, m_fConeLength, 0x0000FFFF, 0x00FF00FF, 0xFF0000FF, false);
    }
        break;
    case EEmitShapeType::eEST_Box:
        CShapeRenderer::GetInstance()->DrawBox(emitterMat, m_boxSize.X(), m_boxSize.Y(), m_boxSize.Z(), 0xFF0000FF, false);
        break;
    case EEmitShapeType::eEST_Mesh:
        break;
    case EEmitShapeType::eEST_Circle:
    {
        CVertexPC startPos, endPos;
        startPos.color = 0xFFFF00FF;
        endPos.color = 0xFFFF00FF;
        startPos.position = pEmitter->GetWorldTM().GetTranslate();
        endPos.position = CVec3(0, 0, m_fRadius) * pEmitter->GetWorldTM();
        CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f);

        CQuaternion quat;
        quat.FromPitchYawRoll(0, DegreesToRadians(m_fArcForCircle), 0);
        endPos.position = (CVec3(0, 0, m_fRadius) * quat) * pEmitter->GetWorldTM();
        CRenderManager::GetInstance()->RenderLine(startPos, endPos, 1.0f);
        CMat4 emitterConeMat;
        emitterConeMat.FromPitchYawRoll(DegreesToRadians(90), 0, 0);
        emitterConeMat = emitterMat * emitterConeMat;
        CShapeRenderer::GetInstance()->DrawCircle(emitterConeMat, m_fRadius, 0xFFFF00FF, false, m_fArcForCircle / 360.f);
    }
        break;
    case EEmitShapeType::eEST_Edge:
    {
        CVertexPC startPos, endPos;
        CVec3 rightDir = emitterMat.GetRightVec3();
        startPos.position = rightDir * m_fRadius;
        startPos.color = 0xFFFF00FF;
        endPos.position = rightDir * -m_fRadius;
        endPos.color = 0xFFFF00FF;
        CRenderManager::GetInstance()->RenderLine(startPos, endPos, 3.0f);
    }
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
}

void CShapeModule::CalcPosAndDirection(CVec3& localPos, CVec3& direction, float fParticleScale)
{
    switch (m_emitShapeType)
    {
    case EEmitShapeType::eEST_Sphere:
        InitParticleForSphere(localPos, direction, fParticleScale);
        break;
    case EEmitShapeType::eEST_HemiSphere:
        InitParticleForHemiSphere(localPos, direction, fParticleScale);
        break;
    case EEmitShapeType::eEST_Cone:
        InitParticleForCone(localPos, direction, fParticleScale);
        break;
    case EEmitShapeType::eEST_Box:
        InitParticleForBox(localPos, direction, fParticleScale);
        break;
    case EEmitShapeType::eEST_Mesh:
        InitParticleForMesh(localPos, direction, fParticleScale);
        break;
    case EEmitShapeType::eEST_Circle:
        InitParticleForCircle(localPos, direction, fParticleScale);
        break;
    case EEmitShapeType::eEST_Edge:
        InitParticleForEdge(localPos, direction, fParticleScale);
        break;
    default:
        BEATS_ASSERT(false, "Never reach here!");
        break;
    }
}

void CShapeModule::InitParticleForSphere(CVec3& localPos, CVec3& direction, float fParticleScale) const
{
    CVec3 tmpDirection = GetRandomDirection();
    float fRadius = (m_bEmitFromShell ? m_fRadius : PARTICLE_RAND_RANGE(0, m_fRadius)) * fParticleScale;
    CVec3 pos = tmpDirection * fRadius;
    localPos = pos;
    direction = m_bRandomDirection ? GetRandomDirection() : tmpDirection;
}

void CShapeModule::InitParticleForHemiSphere(CVec3& localPos, CVec3& direction, float fParticleScale) const
{
    CVec3 randomHemiDirection(PARTICLE_RAND_RANGE(-1, 1), PARTICLE_RAND_RANGE(-1, 1), PARTICLE_RAND_RANGE(1, 0));
    randomHemiDirection.Normalize();
    float fRadius = (m_bEmitFromShell ? m_fRadius : PARTICLE_RAND_RANGE(0, m_fRadius)) * fParticleScale;
    localPos = randomHemiDirection * fRadius;
    direction = m_bRandomDirection ? GetRandomDirection() : randomHemiDirection;
}

void CShapeModule::InitParticleForCone(CVec3& localPos, CVec3& direction, float fParticleScale) const
{
    CVec3 finalPos, finalDirection;
    float fClipAngle = m_fAngle;
    BEATS_CLIP_VALUE(fClipAngle, 0, 89.99f);
    float fRadius = m_fRadius * fParticleScale;
    float fConeLength = m_fConeLength * fParticleScale;
    float fTopRadius = fRadius + fConeLength * tanf(DegreesToRadians(fClipAngle));
    CVec3 randomDirection(PARTICLE_RAND_RANGE(-1, 1), PARTICLE_RAND_RANGE(-1, 1), 0);
    randomDirection.Normalize();

    float fRadiusOnBase = m_bEmitFromShell ? fRadius : PARTICLE_RAND_RANGE(0, fRadius);
    finalPos = randomDirection * fRadiusOnBase;
    BEATS_ASSERT(!BEATS_FLOAT_EQUAL(fRadius, 0));
    fTopRadius *= (fRadiusOnBase / fRadius);
    CVec3 topPos = (m_bRandomDirection ? GetRandomDirection() : randomDirection) * fTopRadius;
    topPos.Z() = fConeLength;
    finalDirection = topPos - finalPos;
    if (!m_bEmitFromBaseOrVolume)
    {
        finalPos += (finalDirection * PARTICLE_RAND_RANGE(0, 1));
        if (m_bRandomDirection)
        {
            finalDirection = GetRandomDirection();
        }
    }
    finalDirection.Normalize();
    localPos = finalPos;
    direction = finalDirection;
}

void CShapeModule::InitParticleForBox(CVec3& localPos, CVec3& direction, float fParticleScale) const
{
    CVec3 halfSize = m_boxSize * 0.5f * fParticleScale;
    localPos.Fill(RANGR_RANDOM_FLOAT(-halfSize.X(), halfSize.X()),
        RANGR_RANDOM_FLOAT(-halfSize.Y(), halfSize.Y()),
        RANGR_RANDOM_FLOAT(-halfSize.Z(), halfSize.Z()));

    direction = CVec3(0, 0, 1);
    if (m_bRandomDirection)
    {
        direction = GetRandomDirection();
    }
}

void CShapeModule::InitParticleForMesh(CVec3& /*localPos*/, CVec3& /*direction*/, float /*fParticleScale*/) const
{
    BEATS_ASSERT(false, "TODO: Not implemented!");
}

void CShapeModule::InitParticleForCircle(CVec3& localPos, CVec3& direction, float fParticleScale) const
{
    direction = CVec3(1, 0, 0);
    CQuaternion quat;
    quat.FromPitchYawRoll(0, 0, PARTICLE_RAND_RANGE(0, DegreesToRadians(m_fArcForCircle)));
    direction *= quat;
    float fRadius = m_fRadius * fParticleScale;
    localPos = direction * (m_bEmitFromShell ? fRadius : PARTICLE_RAND_RANGE(0, fRadius));
    if (m_bRandomDirection)
    {
        direction = GetRandomDirection();
    }
}

void CShapeModule::InitParticleForEdge(CVec3& localPos, CVec3& direction, float fParticleScale) const
{
    float fRadius = fParticleScale * m_fRadius;
    localPos = CVec3(PARTICLE_RAND_RANGE(-fRadius, fRadius), 0, 0);
    direction = CVec3(0, 1, 0);
    if (m_bRandomDirection)
    {
        direction = GetRandomDirection();
    }
}

CVec3 CShapeModule::GetRandomDirection() const
{
    CVec3 randomDirection(PARTICLE_RAND_RANGE(-1, 1), PARTICLE_RAND_RANGE(-1, 1), PARTICLE_RAND_RANGE(-1, 1));
    randomDirection.Normalize();
    return randomDirection;
}