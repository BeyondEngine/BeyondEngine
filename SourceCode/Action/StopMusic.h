#ifndef BEYOND_ENGINE_ACTION_STOPMUSIC_H__INCLUDE
#define BEYOND_ENGINE_ACTION_STOPMUSIC_H__INCLUDE

#include "ActionBase.h"

class CStopMusic : public CActionBase
{
    DECLARE_REFLECT_GUID( CStopMusic, 0x11b40120, CActionBase )
public:
    CStopMusic();
    virtual ~CStopMusic();

    virtual bool ExecuteImp(SActionContext* pContext) override;
    virtual void ReflectData(CSerializer& serializer) override;

private:
    bool m_bStopOrPause;
};

#endif