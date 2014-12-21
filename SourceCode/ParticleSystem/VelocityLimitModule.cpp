#include "stdafx.h"
#include "VelocityLimitModule.h"

CVelocityLimitModule::CVelocityLimitModule()
{

}

CVelocityLimitModule::~CVelocityLimitModule()
{

}

void CVelocityLimitModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bSeparateAxis, true, 0xFFFFFFFF, "独立坐标轴", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_bLocalOrWorld, true, 0xFFFFFFFF, "局部坐标", nullptr, nullptr, "VisibleWhen:m_bSeparateAxis==true");
    DECLARE_PROPERTY(serializer, m_speed, true, 0xFFFFFFFF, "速度", nullptr, nullptr, "VisibleWhen:m_bSeparateAxis==false, MinValue:0");
    DECLARE_PROPERTY(serializer, m_velocityX, true, 0xFFFFFFFF, "X轴速度限制", nullptr, nullptr, "VisibleWhen:m_bSeparateAxis==true");
    DECLARE_PROPERTY(serializer, m_velocityY, true, 0xFFFFFFFF, "Y轴速度限制", nullptr, nullptr, "VisibleWhen:m_bSeparateAxis==true");
    DECLARE_PROPERTY(serializer, m_velocityZ, true, 0xFFFFFFFF, "Z轴速度限制", nullptr, nullptr, "VisibleWhen:m_bSeparateAxis==true");
    DECLARE_PROPERTY(serializer, m_fDampen, true, 0xFFFFFFFF, "阻尼系数", nullptr, "最终速度 = 当前速度 - （当前速度-速度限制） * 阻尼系数", "MinValue:0,MaxValue:1");
}

bool CVelocityLimitModule::IsLocal() const
{
    return m_bLocalOrWorld;
}

bool CVelocityLimitModule::IsSeparateAxis() const
{
    return m_bSeparateAxis;
}

float CVelocityLimitModule::GetDampen() const
{
    return m_fDampen;
}

const SRandomValue& CVelocityLimitModule::GetSpeed() const
{
    return m_speed;
}

const SRandomValue& CVelocityLimitModule::GetX() const
{
    return m_velocityX;
}

const SRandomValue& CVelocityLimitModule::GetY() const
{
    return m_velocityY;
}

const SRandomValue& CVelocityLimitModule::GetZ() const
{
    return m_velocityZ;
}
