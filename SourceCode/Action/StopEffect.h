#ifndef BEYOND_ENGINE_ACTION_STOPEFFECT_H__INCLUDE
#define BEYOND_ENGINE_ACTION_STOPEFFECT_H__INCLUDE

#include "ActionBase.h"

class CStopEffect : public CActionBase
{
    DECLARE_REFLECT_GUID( CStopEffect, 0x904AD130, CActionBase )
public:
    CStopEffect();
    virtual ~CStopEffect();

    virtual bool ExecuteImp(SActionContext* pContext) override;
    virtual void ReflectData(CSerializer& serializer) override;

private:
    bool m_bStopOrPause;
    size_t m_uEffectId;
};

#endif