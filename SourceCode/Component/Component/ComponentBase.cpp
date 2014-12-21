#include "stdafx.h"
#include "ComponentBase.h"

CComponentBase::CComponentBase()
: m_bLoaded(false)
, m_bInitialize(false)
, m_id(0xFFFFFFFF)
{
}

CComponentBase::~CComponentBase()
{
    BEATS_ASSERT(!m_bInitialize && !m_bLoaded, _T("Call Uninitialize before delete the component!"));
}

uint32_t CComponentBase::GetId() const
{
    return m_id;
}

void CComponentBase::SetId(uint32_t id)
{
    m_id = id;
}

bool CComponentBase::Load()
{
    BEATS_ASSERT(!m_bLoaded, _T("Can't load a component twice!"));
    m_bLoaded = true;
    return true;
}

bool CComponentBase::Unload()
{
    BEATS_ASSERT(m_bLoaded, _T("Component is not loaded in uninitialize!"));
    m_bLoaded = false;
    return true;
}

bool CComponentBase::IsLoaded() const
{
    return m_bLoaded;
}

void CComponentBase::SetLoadFlag(bool bFlag)
{
    m_bLoaded = bFlag;
}

bool CComponentBase::IsInitialized() const
{
    return m_bInitialize;
}

void CComponentBase::Initialize()
{
    if (!m_bLoaded)
    {
        Load();
    }
    BEATS_ASSERT(!m_bInitialize, _T("Can't initialize a component twice!"));
    m_bInitialize = true;
}

void CComponentBase::Uninitialize()
{
    if (m_bLoaded)
    {
        Unload();
    }
    BEATS_ASSERT(m_bInitialize, _T("Can't uninitialize a component twice!"));
    m_bInitialize = false;
}

void CComponentBase::SetInitializeFlag(bool bFlag)
{
    m_bInitialize = bFlag;
}

void CComponentBase::ReflectData(CSerializer& /*serializer*/)
{

}
