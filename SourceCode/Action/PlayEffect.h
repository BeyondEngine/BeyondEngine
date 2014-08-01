#ifndef BEYOND_ENGINE_ACTION_PLAYEFFECT_H__INCLUDE
#define BEYOND_ENGINE_ACTION_PLAYEFFECT_H__INCLUDE

#include "ActionBase.h"

class CPlayEffect : public CActionBase
{
    DECLARE_REFLECT_GUID( CPlayEffect, 0x09D7C34, CActionBase )
public:
    CPlayEffect();
    virtual ~CPlayEffect();

    virtual bool ExecuteImp(SActionContext* pContext) override;
    virtual void ReflectData(CSerializer& serializer) override;

private:
    bool m_bLoop;
    size_t m_uEffectId;
    float m_fVolume;
    float m_fPan;
    float m_fPitch;
    TString m_strFileName;
};

#endif