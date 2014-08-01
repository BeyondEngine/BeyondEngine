#ifndef BEYOND_ENGINE_ACTION_ACTIONBASE_H__INCLUDE
#define BEYOND_ENGINE_ACTION_ACTIONBASE_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentInstance.h"

class CGameObject;
struct SActionContext
{
    SActionContext()
        : m_pSerializer(NULL)
        , m_pUserData(NULL)
        , m_fDeltaTime(0)
        , m_pGameObject(NULL)
    {

    }
    ~SActionContext()
    {

    }
    CSerializer* m_pSerializer;
    void* m_pUserData;
    float m_fDeltaTime;
    CGameObject* m_pGameObject;
};

class CActionBase : public CComponentInstance
{
    DECLARE_REFLECT_GUID_ABSTRACT( CActionBase, 0x741BC05A, CComponentInstance )
public:
    CActionBase();
    virtual ~CActionBase();
    bool Execute(SActionContext* pContext);
    virtual void ReflectData(CSerializer& serializer) override;

private:
    virtual bool ExecuteImp(SActionContext* pContext) = 0;

protected:
    CActionBase* m_pNextAction;
};
#endif