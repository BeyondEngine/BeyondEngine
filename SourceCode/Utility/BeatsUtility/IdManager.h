#ifndef BEATS_UTILITY_IDMANAGER_IDMANAGER_H__INCLUDE
#define BEATS_UTILITY_IDMANAGER_IDMANAGER_H__INCLUDE

#include <set>

class CIdManager
{
public:
    CIdManager();
    ~CIdManager();
public:
    size_t GenerateId();
    bool ReserveId(size_t id, bool bCheckIsAlreadyRequested = true);
    void RecycleId(size_t id);
    void Reset();
    bool IsIdFree(size_t id);
    void Lock();
    void UnLock();
    bool IsLocked() const;

private:
    bool m_bLock;
    size_t m_lastId;
    std::set<size_t> m_freeIdPool;
};


#endif