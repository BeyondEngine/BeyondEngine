#ifndef BEYOND_ENGINE_ACTION_CONDITIONACTIONBASE_H__INCLUDE
#define BEYOND_ENGINE_ACTION_CONDITIONACTIONBASE_H__INCLUDE

#include "ActionBase.h"

class CConditionActionBase : public CActionBase
{
    DECLARE_REFLECT_GUID_ABSTRACT( CConditionActionBase, 0x11D07A1C, CActionBase )
public:
    CConditionActionBase();
    virtual ~CConditionActionBase();
    virtual void ReflectData(CSerializer& serializer) override;
    virtual bool ExecuteImp(SActionContext* pContext) override;
    virtual bool Exam(SActionContext* pContext) = 0;

protected:
    CActionBase* m_pSuccessAction;
    CActionBase* m_pFailedAction;
};
#endif