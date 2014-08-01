#ifndef BEYOND_ENGINE_ACTION_SWITCHSCENE_H__INCLUDE
#define BEYOND_ENGINE_ACTION_SWITCHSCENE_H__INCLUDE

#include "ActionBase.h"

class CSwitchScene : public CActionBase
{
    DECLARE_REFLECT_GUID( CSwitchScene, 0x401BC8A4, CActionBase )

public:
    CSwitchScene();
    virtual ~CSwitchScene();

    virtual bool ExecuteImp(SActionContext* pContext) override;
    virtual void ReflectData(CSerializer& serializer) override;
private:
    size_t m_uFileId;
};

#endif