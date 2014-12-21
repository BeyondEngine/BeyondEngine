#ifndef STAR_RAIDERS_ACTION_SETNODEVISIBLE_H__INCLUDE
#define STAR_RAIDERS_ACTION_SETNODEVISIBLE_H__INCLUDE

#include "Action/ActionBase.h"

class CSetNodeVisible : public CActionBase
{
    DECLARE_REFLECT_GUID( CSetNodeVisible, 0x1B4C2E01, CActionBase )

public:
    CSetNodeVisible();
    virtual ~CSetNodeVisible();

    virtual void ReflectData(CSerializer& serializer) override;

private:
    virtual bool ExecuteImp(SActionContext* pContext) override;

private:
    bool m_bVisible;
};
#endif
