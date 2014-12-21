#include "stdafx.h"
#include "ChangeValueRecord.h"
#include "Component/Property/PropertyDescriptionBase.h"

CChangeValueRecord::CChangeValueRecord()
{

}

CChangeValueRecord::~CChangeValueRecord()
{

}

void CChangeValueRecord::Do()
{
    BEATS_ASSERT(m_pProperty != nullptr);
    m_pProperty->Deserialize(m_newData);
    m_pProperty->SetValueWithType(m_pProperty->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
    m_newData.SetReadPos(0);
    CEngineCenter::GetInstance()->m_editorPropertyGridSyncList.insert(m_pProperty);
}

void CChangeValueRecord::Undo()
{
    BEATS_ASSERT(m_pProperty != nullptr);
    m_pProperty->Deserialize(m_oldData);
    m_pProperty->SetValueWithType(m_pProperty->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
    m_oldData.SetReadPos(0);
    CEngineCenter::GetInstance()->m_editorPropertyGridSyncList.insert(m_pProperty);
}

void CChangeValueRecord::Reset()
{
    m_newData.Reset();
    m_oldData.Reset();
    m_pProperty = nullptr;
}

void CChangeValueRecord::SetPropertyDescription(CPropertyDescriptionBase* pProperty)
{
    m_pProperty = pProperty;
}

CSerializer& CChangeValueRecord::GetOldData()
{
    return m_oldData;
}

CSerializer& CChangeValueRecord::GetNewData()
{
    return m_newData;
}
