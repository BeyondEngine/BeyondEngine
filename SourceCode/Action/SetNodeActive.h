#ifndef STAR_RAIDERS_ACTION_SETNODEACTIVE_H__INCLUDE
#define STAR_RAIDERS_ACTION_SETNODEACTIVE_H__INCLUDE

#include "Action/ActionBase.h"

class CSetNodeActive : public CActionBase
{
    DECLARE_REFLECT_GUID(CSetNodeActive, 0x71CD002B, CActionBase)

public:
    CSetNodeActive();
    virtual ~CSetNodeActive();

    virtual void ReflectData(CSerializer& serializer) override;

private:
    virtual bool ExecuteImp(SActionContext* pContext) override;

private:
    bool m_bActive;
};
#endif
