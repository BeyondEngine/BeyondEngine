#include "stdafx.h"
#include "VelocityModule.h"

CVelocityModule::CVelocityModule()
{

}

CVelocityModule::~CVelocityModule()
{

}

void CVelocityModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_bLocalOrWorld, true, 0xFFFFFFFF, "局部坐标", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_velocityX, true, 0xFFFFFFFF, "X轴速度", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_velocityY, true, 0xFFFFFFFF, "Y轴速度", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_velocityZ, true, 0xFFFFFFFF, "Z轴速度", nullptr, nullptr, nullptr);
}

bool CVelocityModule::IsLocal() const
{
    return m_bLocalOrWorld;
}

const SRandomValue& CVelocityModule::GetX() const
{
    return m_velocityX;
}

const SRandomValue& CVelocityModule::GetY() const
{
    return m_velocityY;
}

const SRandomValue& CVelocityModule::GetZ() const
{
    return m_velocityZ;
}
