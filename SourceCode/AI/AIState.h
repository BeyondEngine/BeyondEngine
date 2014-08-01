#ifndef BEYOND_ENGINE_AI_AISTATE_H__INCLUDE
#define BEYOND_ENGINE_AI_AISTATE_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

class CActionBase;
class CAIScheme;
struct SActionContext;
class CAIState : public CComponentInstance
{
    DECLARE_REFLECT_GUID( CAIState, 0x77B01ACD, CComponentInstance )

public:
    CAIState();
    virtual ~CAIState();

    virtual void ReflectData(CSerializer& serializer) override;
    virtual void Enter(SActionContext* pActionContext);
    virtual void Update(SActionContext* pActionContext);
    virtual void Leave(SActionContext* pActionContext);
    void SetScheme(CAIScheme* pAIScheme);
    CAIScheme* GetScheme() const;

private:
    CActionBase* m_pEnterAction;
    CActionBase* m_pUpdateAction;
    CActionBase* m_pLeaveAction;
    CAIScheme* m_pOwnerScheme;
};

#endif