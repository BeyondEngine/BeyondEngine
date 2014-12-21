#include "stdafx.h"
#include "IdManager.h"
#include "time.h"

CIdManager::CIdManager()
: uLockCount(0)
{

}

CIdManager::~CIdManager()
{

}

uint32_t CIdManager::GenerateId()
{
    BEATS_ASSERT(0 == uLockCount, _T("can't Generate id when it is locked!"));
    srand((unsigned int)time(nullptr));
    uint32_t uRet = rand();
    while (!IsIdFree(uRet))
    {
        uRet = rand();
    }
    m_reservedIdPool.insert(uRet);
    return uRet;
}

void CIdManager::RecycleId( uint32_t id )
{
    if (0 == uLockCount)
    {
        BEATS_ASSERT(!IsIdFree(id), _T("Can't recycle an id %d which is not reserved."), id);
        m_reservedIdPool.erase(id);
    }
}

bool CIdManager::ReserveId( uint32_t id , bool bCheckIsAlreadyRequested/* = true*/)
{
    bool bRet = true;
    if (0 == uLockCount)
    {
        bRet = IsIdFree(id);
        m_reservedIdPool.insert(id);
        BEYONDENGINE_UNUSED_PARAM(bCheckIsAlreadyRequested);
        BEATS_ASSERT(!bCheckIsAlreadyRequested || bRet, _T("Id: %d can't be request twice!"), id);
    }

    return bRet;
}

void CIdManager::Reset()
{
    m_reservedIdPool.clear();
}

bool CIdManager::IsIdFree(uint32_t id)
{
    return m_reservedIdPool.find(id) == m_reservedIdPool.end();
}

void CIdManager::Lock()
{
    ++uLockCount;
}

void CIdManager::UnLock()
{
    BEATS_ASSERT(uLockCount > 0);
    --uLockCount;
}

bool CIdManager::IsLocked() const
{
    return uLockCount > 0;
}
