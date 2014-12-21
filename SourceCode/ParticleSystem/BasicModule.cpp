#include "stdafx.h"
#include "BasicModule.h"

CBasicModule::CBasicModule()
{

}

CBasicModule::~CBasicModule()
{

}

void CBasicModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_b3D, true, 0xFFFFFFFF, "3D粒子", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_fDuration, true, 0xFFFFFFFF, "持续时长（秒）", nullptr, "发射器持续发射的时间，如果为循环发射，那么这指一个周期的时间", nullptr);
    DECLARE_PROPERTY(serializer, m_bLoop, true, 0xFFFFFFFF, "循环", nullptr, "如果为真，在一个发射周期之后会继续发射", nullptr);
    DECLARE_PROPERTY(serializer, m_bStopWhenDeactive, true, 0xFFFFFFFF, "立刻停止", nullptr, nullptr, "VisibleWhen:m_bLoop == true");
    DECLARE_PROPERTY(serializer, m_bPrewarm, true, 0xFFFFFFFF, "预加载", nullptr, "如果为真，发射器会直接从一个发射周期以后的状态开始", "VisibleWhen:m_bLoop==true");
    DECLARE_PROPERTY(serializer, m_fStartDelay, true, 0xFFFFFFFF, "延迟发射时间（秒）", nullptr, "开始发射后，粒子系统将等待该时间后才真正发射", "VisibleWhen:m_bPrewarm==false");
    DECLARE_PROPERTY(serializer, m_fGravityModifier, true, 0xFFFFFFFF, "重力因子", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_bEnableDepth, true, 0xFFFFFFFF, "开启深度检测", nullptr, nullptr, nullptr);

    DECLARE_PROPERTY(serializer, m_startLifeTime, true, 0xFFFFFFFF, "粒子生命周期（秒）", nullptr, "粒子的生命周期，超过该时间后，粒子将消亡", nullptr);
    DECLARE_PROPERTY(serializer, m_startSpeed, true, 0xFFFFFFFF, "初始速率", nullptr, "粒子的初始速率", nullptr);
    DECLARE_PROPERTY(serializer, m_startSize, true, 0xFFFFFFFF, "初始尺寸", nullptr, "粒子的初始尺寸", "MinValue:0");
    DECLARE_PROPERTY(serializer, m_startRotation, true, 0xFFFFFFFF, "初始旋转（角度）", nullptr, "粒子的初始旋转", nullptr);
    DECLARE_PROPERTY(serializer, m_startColor, true, 0xFFFFFFFF, "初始颜色", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_bAttachToEmitter, true, 0xFFFFFFFF, "局部坐标", nullptr, "如果为真，表示在局部空间坐标系，否则为世界坐标空间系", nullptr);
    DECLARE_PROPERTY(serializer, m_bPlayOnAwake, true, 0xFFFFFFFF, "自动开始发射", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_uMaxParticle, true, 0xFFFFFFFF, "最大粒子数量", nullptr, "如果当前粒子数量超过这个值, 发射器将停止发射", nullptr);
}

bool CBasicModule::IsParticleAttach() const
{
    return m_bAttachToEmitter;
}

bool CBasicModule::IsLoop() const
{
    return m_bLoop;
}

bool CBasicModule::IsPlayOnAwake() const
{
    return m_bPlayOnAwake;
}

bool CBasicModule::IsStopWhenDeactive() const
{
    return m_bStopWhenDeactive;
}

void CBasicModule::SetDepthTest(bool bEnable)
{
    m_bEnableDepth = bEnable;
}

bool CBasicModule::IsDepthTestEnable() const
{
    return m_bEnableDepth;
}

bool CBasicModule::Is3DEmitter() const
{
    return m_b3D;
}

void CBasicModule::Set3DEmitterFlag(bool b3DOr2D)
{
    m_b3D = b3DOr2D;
}

float CBasicModule::GetDuration() const
{
    return m_fDuration;
}

float CBasicModule::GetStartDelay() const
{
    return m_fStartDelay;
}

float CBasicModule::GetGravityModifier() const
{
    return m_fGravityModifier;
}

uint32_t CBasicModule::GetMaxParticleCount() const
{
    return m_uMaxParticle;
}

const SRandomValue& CBasicModule::GetInitLifeTime() const
{
    return m_startLifeTime;
}

const SRandomValue& CBasicModule::GetInitSpeed() const
{
    return m_startSpeed;
}

const SRandomValue& CBasicModule::GetInitSize() const
{
    return m_startSize;
}

const SRandomValue& CBasicModule::GetInitRotation() const
{
    return m_startRotation;
}

const CRandomColor& CBasicModule::GetInitColor() const
{
    return m_startColor;
}

