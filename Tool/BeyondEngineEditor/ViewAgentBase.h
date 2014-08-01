#ifndef BEYOND_ENGINE_EDITOR_VIEWAGENT_VIEWAGENTBASE_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_VIEWAGENT_VIEWAGENTBASE_H__INCLUDE
#include "wx\event.h"
#include "wx\html\helpfrm.h"
#include "wx\propgrid\propgrid.h"
#include "wx\dataview.h"

class CEditorMainFrame;
class CComponentProxy;

class CViewAgentBase
{
public:
    CViewAgentBase();
    virtual ~CViewAgentBase();

    virtual void InView();
    virtual void OutView();
    virtual void ProcessMouseEvent(wxMouseEvent& event);
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
    virtual void OnEditDataViewItem(wxDataViewEvent& event);
    virtual void Update();
protected:
    CEditorMainFrame* m_pMainFrame;
};
#endif