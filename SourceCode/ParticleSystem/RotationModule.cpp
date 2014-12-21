#include "stdafx.h"
#include "RotationModule.h"

CRotationModule::CRotationModule()
{

}

CRotationModule::~CRotationModule()
{

}

void CRotationModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_angularSpeed, true, 0xFFFFFFFF, "½ÇËÙ¶È", nullptr, nullptr, nullptr);
}

const SRandomValue& CRotationModule::GetAngularSpeed() const
{
    return m_angularSpeed;
}