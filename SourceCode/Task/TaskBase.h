#ifndef BEYOND_ENGINE_TASK_TASKBASE_H__INCLUDE
#define BEYOND_ENGINE_TASK_TASKBASE_H__INCLUDE

#include "Component/Component/ComponentInstance.h"
class CScene;
class CTaskBase : public CComponentInstance
{
    DECLARE_REFLECT_GUID_ABSTRACT(CTaskBase, 0x9A7B0041, CComponentInstance)

public:
    CTaskBase();
    virtual ~CTaskBase();

    virtual void ReflectData(CSerializer& serializer) override;
    virtual uint32_t GetProgress() = 0;
    virtual void Execute(float ddt) = 0;
    virtual void Reset();
    void Stop();
    void SetDelayExecuteTimeMS(uint32_t uDelayTime);
    uint32_t GetDelayExecuteTime() const;
    void SetAddToMgrTimeStamp(uint32_t uTimeStamp);
    uint32_t GetAddToMgrTimeStamp() const;
    CScene* GetOwnerScene() const;
    void SetOwnerScene(CScene* pScene);
    bool IsBindToScene() const;
    bool IsImmediateCall() const;
    void SetBindToScene(bool bBindToScene);
    void SetFinishedCallback(std::function<void()> pFunc);
    std::function<void()> GetFinishedCallback() const;
protected:
    bool m_bStopFlag = false;
    bool m_bImmediateCall = false;//When add to task manager, we will call this immediately with delta time 0.
    bool m_bBindToCurrScene;
    uint32_t m_uDelayExecuteMS = 0;
    uint32_t m_uAddToMgrTimeStamp = 0;
    CScene* m_pSceneOwner;
    std::function<void()> m_pFinishedCallback = nullptr;
};

#endif