#ifndef BEYOND_ENGINE_EDITOR_OPERATIONRECORDMANAGER_OPERATIONRECORDMANAGER_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_OPERATIONRECORDMANAGER_OPERATIONRECORDMANAGER_H__INCLUDE
#include "OperationRecord.h"

class COperationRecordManager
{
    BEATS_DECLARE_SINGLETON(COperationRecordManager)
public:
    template<class T>
    T* RequestRecord(EOperationRecordType operateType);
    void AppendRecord(COperationRecord* pRecord);
    bool DoRecord();
    bool UndoRecord();
    void ClearRecord();

private:
    int m_nCurrIndex = -1;
    std::vector<COperationRecord*> m_recordList;
    std::map<EOperationRecordType, std::vector<COperationRecord*>> m_recordCache;
};

template<class T>
T* COperationRecordManager::RequestRecord(EOperationRecordType operateType)
{
    T* pRet = nullptr;
    std::vector<COperationRecord*>& recordPool = m_recordCache[operateType];
    if (recordPool.size() > 0)
    {
        pRet = down_cast<T*>(recordPool.back());
        pRet->Reset();
        recordPool.pop_back();
    }
    else
    {
        pRet = new T;
        pRet->Reset();
    }
    return pRet;
}
#endif