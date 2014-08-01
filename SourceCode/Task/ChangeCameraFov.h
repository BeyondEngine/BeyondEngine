#ifndef BEYOND_ENGINE_TASK_CHANGECAMERAFOV_H__INCLUDE
#define BEYOND_ENGINE_TASK_CHANGECAMERAFOV_H__INCLUDE

#include "TaskBase.h"

class CChangeCameraFov : public CTaskBase
{
    DECLARE_REFLECT_GUID(CChangeCameraFov, 0x4BEE0F1C, CTaskBase)
public:
    CChangeCameraFov();
    virtual ~CChangeCameraFov();

    virtual size_t GetProgress() override;
    virtual void Execute(float ddt) override;
    virtual void ReflectData(CSerializer& serializer) override;

private:
    float m_fElapsedTimeMs;
    float m_fSourceFov;
    float m_fTargetFov;
    float m_fSpendTimeMs;
};

#endif