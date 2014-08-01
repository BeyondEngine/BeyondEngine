#ifndef BEYOND_ENGINE_EDITOR_VIEWAGENT_UIVIEWAGENT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_VIEWAGENT_UIVIEWAGENT_H__INCLUDE

#include "ViewAgentBase.h"

class CWxwidgetsPropertyBase;
class CUIViewAgent : public CViewAgentBase
{
    BEATS_DECLARE_SINGLETON(CUIViewAgent);
public:
    virtual void InView() override;
    virtual void OutView() override;
    virtual void ProcessMouseEvent(wxMouseEvent& event) override;
    virtual void SelectComponent(CComponentProxy* pComponentInstance) override;

protected:
    void UpdateCursorIcon(wxPoint& mousePos);
    void SetkmVec2PropertyValue(CWxwidgetsPropertyBase* pPropertyDescription, kmVec2& value);

};
#endif