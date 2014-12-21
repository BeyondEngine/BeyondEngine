#ifndef BEYOND_ENGINE_EDITOR_VIEWAGENT_VIEWAGENTBASE_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_VIEWAGENT_VIEWAGENTBASE_H__INCLUDE
#include "wx/event.h"
#include "wx/html/helpfrm.h"
#include "wx/propgrid/propgrid.h"
#include "wx/dataview.h"
#include "NodeAnimation/NodeAnimationElement.h"

#define ISKEYDOWN(key) ((::GetKeyState(key) & 0x8000) > 0)

class CEditorMainFrame;
class CComponentProxy;
class CBeyondEngineEditorGLWindow;
class CComponentBase;
class CSerializer;

class CViewAgentBase
{
public:
    CViewAgentBase();
    virtual ~CViewAgentBase();

    virtual void InView();
    virtual void OutView();
    virtual void ProcessMouseEvent(wxMouseEvent& event);
    virtual void ProcessKeyboardEvent(wxKeyEvent& event);
    virtual void CreateTools();
    virtual void OnPropertyChanged(wxPropertyGridEvent& event);
    virtual void OnEditTreeItemLabel(wxTreeEvent& event);
    virtual void OnTreeCtrlRightClick(wxTreeEvent& event);
    virtual void SelectComponent(CComponentProxy* pComponentProxy);
    virtual void OnTreeCtrlSelect(wxTreeEvent& event);
    virtual void OnActivateTreeItem(wxTreeEvent& event);
    virtual void OnOpenComponentFile();
    virtual void OnCloseComponentFile();
    virtual void OnUpdateComponentInstance();
    virtual void OnCommandEvent(wxCommandEvent& event);
    virtual void OnTimeBarChoice();
    virtual void OnTimeBarTreeItemDrag();
    virtual void OnTimeBarTreeItemSelect();
    virtual void OnTimeBarItemContainerRClick(wxCommandEvent& event);
    virtual void Update(float dt);
    virtual void OnTimeBarAddButtonClick();
    virtual void OnTimeBarMinusButtonClick();
    virtual void OnTimeBarSelectFrameChange();
    virtual void OnTimeBarCurSorChange(int nCursorPos);
    virtual void OnTimeBarDraggingFrame();
    virtual void OnComponentPropertyChange(CComponentBase* pComponent);
    virtual void OnComponentEndDrag();
    virtual void OnComponentStartDrag();
    virtual void OnPropertyGridSelect(wxPropertyGridEvent& event);

    void SetGLWindow(CBeyondEngineEditorGLWindow* pWindow);

protected:
    void CommonHandleMouse(wxMouseEvent& event);
    void DeliverMouseEventToTouch(wxMouseEvent& event);
protected:
    CEditorMainFrame* m_pMainFrame;
    CBeyondEngineEditorGLWindow* m_pGLWindow;
    CVec2 m_mouseDownPos;
    CVec2 m_mouseCurrPos;
    CVec2 m_mouseLastPos;
    CVec3 m_startMoveWorldPos;
    CVec3 m_recordMovePos;
};
#endif