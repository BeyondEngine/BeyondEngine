#ifndef BEYOND_ENGINE_EDITOR_VIEWAGENT_ANIVIEWAGENT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_VIEWAGENT_ANIVIEWAGENT_H__INCLUDE

#include "ViewAgentBase.h"

class CEditorMainFrame;
class CAniViewAgent : public CViewAgentBase
{
    BEATS_DECLARE_SINGLETON(CAniViewAgent);
public:
    virtual void InView() override;
    virtual void OutView() override;

    virtual void OnCommandEvent(wxCommandEvent& event) override;
    virtual void SelectComponent(CComponentProxy* pComponentProxy) override;
    virtual void OnPropertyChanged(wxPropertyGridEvent& event) override;
    virtual void Update() override;
};
#endif