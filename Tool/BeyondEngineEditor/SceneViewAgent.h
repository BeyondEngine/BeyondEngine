#ifndef BEYOND_ENGINE_EDITOR_VIEWAGENT_SCENEVIEWAGENT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_VIEWAGENT_SCENEVIEWAGENT_H__INCLUDE

#include "ViewAgentBase.h"

class CSceneViewAgent : public CViewAgentBase
{
    BEATS_DECLARE_SINGLETON(CSceneViewAgent);
public:
    virtual void InView() override;
    virtual void OutView() override;
    virtual void ProcessMouseEvent(wxMouseEvent& event) override;
    virtual void SelectComponent(CComponentProxy* pComponentInstance) override;

};

#endif