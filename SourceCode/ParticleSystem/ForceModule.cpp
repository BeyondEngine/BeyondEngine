#include "stdafx.h"
#include "ForceModule.h"

CForceModule::CForceModule()
{

}

CForceModule::~CForceModule()
{

}

void CForceModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_forceX, true, 0xFFFFFFFF, "X������", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_forceY, true, 0xFFFFFFFF, "Y������", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_forceZ, true, 0xFFFFFFFF, "Z������", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_bLocalOrWorld, true, 0xFFFFFFFF, "�ֲ��ռ�", nullptr, nullptr, nullptr);
    DECLARE_PROPERTY(serializer, m_bRandomize, true, 0xFFFFFFFF, "ÿ֡���", nullptr, nullptr, nullptr);
}

bool CForceModule::IsLocal() const
{
    return m_bLocalOrWorld;
}

bool CForceModule::IsRandomizeEveryFrame() const
{
    return m_bRandomize;
}

const SRandomValue& CForceModule::GetX() const
{
    return m_forceX;
}

const SRandomValue& CForceModule::GetY() const
{
    return m_forceY;
}

const SRandomValue& CForceModule::GetZ() const
{
    return m_forceZ;
}
