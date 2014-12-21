#ifndef BEYOND_ENGINE_EDITOR_VIEWAGENT_GAMEVIEWAGENT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_VIEWAGENT_GAMEVIEWAGENT_H__INCLUDE

#include "ViewAgentBase.h"

class CGameViewAgent : public CViewAgentBase
{
    typedef CViewAgentBase super;
    BEATS_DECLARE_SINGLETON(CGameViewAgent);
public:
    virtual void InView();
    virtual void OutView();
    virtual void ProcessKeyboardEvent(wxKeyEvent& event) override;

};

#endif