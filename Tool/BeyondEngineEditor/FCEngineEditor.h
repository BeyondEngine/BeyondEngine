#ifndef FCENGINEEDITOR_FCENGINEEDITOR_H__INCLUDE
#define FCENGINEEDITOR_FCENGINEEDITOR_H__INCLUDE

#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/aui/aui.h>
#include "EngineProperGridManager.h"
#include "GLCanvas.h"

class wxSplitterWindow;
class TimeBarFrame;
class CEditorMainFrame : public wxFrame
{
    enum
    {
        ID_CustomizeToolbar = wxID_HIGHEST+1,
        ID_ToolBar1_Radio1,
        ID_ToolBar1_Radio2,
        ID_ToolBar1_Radio3,
        ID_ToolBar1_Radio4,
        ID_ToolBar2_Button1,
        ID_ToolBar2_Button2

    };
public:
    CEditorMainFrame(const wxString& title);
    virtual ~CEditorMainFrame();
    void InitMenu();
    void InitCtrls();
    void CreateAuiToolBar();
    void CreateAuiNoteBook();
    void CreateTreeCtrl();
    void CreateGLCanvas();
    void CreateTimeBar();
    void CreatePropertyGrid();
    void CreatSplitter();
    void AddPageToBook();
    void AddTreeItem();
    void OnAuiButton(wxCommandEvent& event);
    void OnEditAnimationMenuItem(wxCommandEvent& event);
    void OnTreeClick(wxTreeEvent& event);
    void GetEditAnimationDialog();
private:
    void AddChilditemToItem(wxTreeItemId& idParent, std::vector<TString>& vecName, size_t iLevel);

    wxAuiNotebook*      m_pLeft;
    wxAuiNotebook*      m_pRight;
    wxAuiNotebook*      m_pBottom;
    wxAuiNotebook*      m_pCenter;
    wxTreeCtrl*         m_pComponentModelTC;
    wxTreeCtrl*         m_pCurComponentTC;
    wxAuiManager        m_Manager;
    GLAnimationCanvas*  m_pGLCanvas;
    TimeBarFrame*       m_pTimeBar;
    EnginePropertyGirdManager* m_pPropGridManager;
    wxPropertyGrid*     m_propGrid;
    wxDialog*           m_pWEditAnimation;
    wxSplitterWindow*   m_pSplitter;
    wxWindow            *m_pSplTop, *m_pSplBottom;
DECLARE_EVENT_TABLE();
};

#endif