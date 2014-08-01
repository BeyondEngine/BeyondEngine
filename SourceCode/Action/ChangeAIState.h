#ifndef BEYOND_ENGINE_ACTION_CHANGEAISTATE_H__INCLUDE
#define BEYOND_ENGINE_ACTION_CHANGEAISTATE_H__INCLUDE

#include "ActionBase.h"
class CAIState;
class CChangeAIState : public CActionBase
{
    DECLARE_REFLECT_GUID( CChangeAIState, 0x4410BB1A, CActionBase )
public:
    CChangeAIState();
    virtual ~CChangeAIState();

    virtual bool ExecuteImp(SActionContext* pContext) override;
    virtual void ReflectData(CSerializer& serializer) override;

private:
    CAIState* m_pAIState;
};

#endif