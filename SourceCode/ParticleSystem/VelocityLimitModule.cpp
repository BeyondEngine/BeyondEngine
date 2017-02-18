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
    DECLARE_PROPERTY(serializer, m_bSeparateAxis, true, 0xFFFFFFFF, "����������", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_bLocalOrWorld, true, 0xFFFFFFFF, "�ֲ�����", nullptr, nullptr, "VisibleWhen:m_bSeparateAxis==true");
    DECLARE_PROPERTY(serializer, m_speed, true, 0xFFFFFFFF, "�ٶ�", nullptr, nullptr, "VisibleWhen:m_bSeparateAxis==false, MinValue:0");
    DECLARE_PROPERTY(serializer, m_velocityX, true, 0xFFFFFFFF, "X���ٶ�����", nullptr, nullptr, "VisibleWhen:m_bSeparateAxis==true");
    DECLARE_PROPERTY(serializer, m_velocityY, true, 0xFFFFFFFF, "Y���ٶ�����", nullptr, nullptr, "VisibleWhen:m_bSeparateAxis==true");
    DECLARE_PROPERTY(serializer, m_velocityZ, true, 0xFFFFFFFF, "Z���ٶ�����", nullptr, nullptr, "VisibleWhen:m_bSeparateAxis==true");
    DECLARE_PROPERTY(serializer, m_fDampen, true, 0xFFFFFFFF, "����ϵ��", nullptr, "�����ٶ� = ��ǰ�ٶ� - ����ǰ�ٶ�-�ٶ����ƣ� * ����ϵ��", "MinValue:0,MaxValue:1");
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
