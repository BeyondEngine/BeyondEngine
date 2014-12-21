#ifndef BEYOND_ENGINE_EDITOR_OPERATIONRECORDMANAGER_OPERATIONRECORD_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_OPERATIONRECORDMANAGER_OPERATIONRECORD_H__INCLUDE
enum class EOperationRecordType
{
    eORT_ChangeValue,

    eORT_Count
};

class COperationRecord
{
public:
    COperationRecord(){};
    virtual ~COperationRecord(){};

    virtual EOperationRecordType GetType() const = 0;
    virtual void Do() = 0;
    virtual void Undo() = 0;
    virtual void Reset() = 0;

};

#endif