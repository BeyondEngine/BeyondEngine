#ifndef BEYOND_ENGINE_EDITOR_VIEWAGENT_SCENEVIEWAGENT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_VIEWAGENT_SCENEVIEWAGENT_H__INCLUDE

#include "ViewAgentBase.h"

class CSceneViewAgent : public CViewAgentBase
{
    typedef CViewAgentBase super;
    BEATS_DECLARE_SINGLETON(CSceneViewAgent);
public:
    virtual void InView() override;
};

#endif