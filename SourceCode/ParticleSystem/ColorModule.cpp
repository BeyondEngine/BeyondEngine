#include "stdafx.h"
#include "ColorModule.h"
#include "Particle.h"

CColorModule::CColorModule()
{

}

CColorModule::~CColorModule()
{

}

void CColorModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_color, true, 0xFFFFFFFF, "ÑÕÉ«", nullptr, nullptr, "ColorOrGradientOnly:false");
}

CColor CColorModule::GetColor(float fProgress) const
{
    return m_color.GetColor(fProgress, PARTICLE_RAND_RANGE(0, 1));
}