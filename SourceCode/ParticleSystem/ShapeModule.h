#ifndef BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_SHAPEMODULE_H__INCLUDE
#define BEYOND_ENGINE_NEWPARTICLESYSTEM_MODULE_SHAPEMODULE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
enum class EEmitShapeType
{
    eEST_Sphere,
    eEST_HemiSphere,
    eEST_Cone,
    eEST_Box,
    eEST_Mesh,
    eEST_Circle,
    eEST_Edge,

    eEST_Count,
};

class CParticle;
class CParticleEmitter;
class CShapeModule : public CComponentInstance
{
    DECLARE_REFLECT_GUID(CShapeModule, 0x7014BBAE, CComponentInstance)

public:
    CShapeModule();
    virtual ~CShapeModule();

    virtual void ReflectData(CSerializer& serializer) override;
    void Render(CParticleEmitter* pEmitter);
    void CalcPosAndDirection(CVec3& localPos, CVec3& direction, float fParticleScale);

private:
    CVec3 GetRandomDirection() const;

private:
    void InitParticleForSphere(CVec3& localPos, CVec3& direction, float fParticleScale) const;
    void InitParticleForHemiSphere(CVec3& localPos, CVec3& direction, float fParticleScale) const;
    void InitParticleForCone(CVec3& localPos, CVec3& direction, float fParticleScale) const;
    void InitParticleForBox(CVec3& localPos, CVec3& direction, float fParticleScale) const;
    void InitParticleForMesh(CVec3& localPos, CVec3& direction, float fParticleScale) const;
    void InitParticleForCircle(CVec3& localPos, CVec3& direction, float fParticleScale) const;
    void InitParticleForEdge(CVec3& localPos, CVec3& direction, float fParticleScale) const;

private:
    EEmitShapeType m_emitShapeType = EEmitShapeType::eEST_Cone;
    float m_fRadius = 1.0f;
    bool m_bEmitFromShell = false;
    bool m_bRandomDirection = false;

    float m_fAngle = 25.0f;
    float m_fArcForCircle = 360;
    bool m_bEmitFromBaseOrVolume = true;
    float m_fConeLength = 5.0f;

    CVec3 m_boxSize;
};

#endif