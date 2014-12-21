#include "stdafx.h"
#include "SizeModule.h"

CSizeModule::CSizeModule()
{

}

CSizeModule::~CSizeModule()
{

}

void CSizeModule::ReflectData(CSerializer& serializer)
{
    super::ReflectData(serializer);
    DECLARE_PROPERTY(serializer, m_size, true, 0xFFFFFFFF, "³ß´ç", nullptr, nullptr, nullptr);
}

const SRandomValue& CSizeModule::GetSize() const
{
    return m_size;
}