#ifndef BEYOND_ENGINE_EDITOR_OPERATIONRECORDMANAGER_CHANGEVALUERECORD_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_OPERATIONRECORDMANAGER_CHANGEVALUERECORD_H__INCLUDE

#include "OperationRecord.h"
#include "Utility/BeatsUtility/Serializer.h"
class CPropertyDescriptionBase;
class CChangeValueRecord : public COperationRecord
{
public:
    CChangeValueRecord();
    virtual ~CChangeValueRecord();

    virtual EOperationRecordType GetType() const override { return EOperationRecordType::eORT_ChangeValue; };
    virtual void Do() override;
    virtual void Undo() override;
    virtual void Reset() override;
    void SetPropertyDescription(CPropertyDescriptionBase* pProperty);
    CSerializer& GetOldData();
    CSerializer& GetNewData();
private:
    CSerializer m_oldData;
    CSerializer m_newData;
    CPropertyDescriptionBase* m_pProperty = nullptr;
};

#endif