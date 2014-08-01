#ifndef BEYONDENGINEEDITOR_EditUIDialog_H__INCLUDE
#define BEYONDENGINEEDITOR_EditUIDialog_H__INCLUDE

#include "EditDialogBase.h"
#include <wx/dialog.h>
#include <wx/aui/aui.h>
#include <wx/treectrl.h>
#include "GUI/Window/Button.h"

class CDataViewCtrl;
class CUIGLWindow;
class CEnginePropertyGirdManager;
class wxPropertyGrid;
class CTimeBarFrame;
class CTexture;
class CEditUIDialog : public CEditDialogBase
{
    enum
    {
        ID_CustomizeToolbar = wxID_HIGHEST+1,
        ID_ToolBar1_Radio1,
        ID_ToolBar1_Radio2,
        ID_ToolBar1_Radio3,
        ID_ToolBar1_Radio4,
        ID_ToolBar2_Button1,
        ID_ToolBar2_Button2,
        ID_DataView_ObjectView

    };
    typedef CEditDialogBase super;
public:
    CEditUIDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style =  wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~CEditUIDialog();
    virtual void LanguageSwitch();

    void InitCtrls();
    void CreateAuiToolBar();
    void CreateMenu();
    void CreateAuiNoteBook();
    void CreatePropertyGrid();
    void CreateCtrlsTree();
    void RemoveSelectedGUI();
    void CreateGUITexture(const TString& strFilePath);

protected:
    void OnToolBarDrag(wxAuiToolBarEvent& event);
    void OnCtrlsTreeBeginDrag(wxTreeEvent& event);
    void OnCtrlsTreeEndDrag(wxTreeEvent& event);
    void OnObjectViewSeletionChange(wxDataViewEvent& event);
    void OnRightClick(wxDataViewEvent& event);
    void OnPopMenu(wxCommandEvent& event);
    void OnDataViewRenderer(CTimeBarEvent& event);
    TString GetAvailableName(const TString& strWindowName);

private:
    wxAuiManager            m_manager;
    wxPanel*                m_pCanvasPanel;
    wxAuiNotebook*          m_pCanvasNotebook;
    wxPanel*                m_pObjectViewPanel;
    wxAuiNotebook*          m_pObjectViewNotebook;
    wxPanel*                m_pGLWindowPanel;
    wxAuiNotebook*          m_pGLWindowNotebook;
    wxPanel*                m_pPropertyPanel;
    wxAuiNotebook*          m_pPropertyNotebook;
    wxPanel*                m_pTimeBarPanel;
    wxAuiNotebook*          m_pTimeBarNotebook;
    wxPanel*                m_pCtrlsPanel;
    wxAuiNotebook*          m_pCtrlsNotebook;

    CWindow*         m_pTempGUI;
    CWindow*         m_pSelectedGUI;
    wxMenu*                 m_pObjectViewMenu;
    SharePtr<CTexture>      m_pTexture;
    wxListBox*              m_pCanvasList;
    wxTreeCtrl*             m_pCtrlsTree;
    CUIGLWindow*            m_pGLWindow;
    CDataViewCtrl*   m_pObjectView;
    wxPropertyGrid*         m_pPropGrid;
    CTimeBarFrame*          m_pTimeBar;
    CEnginePropertyGirdManager* m_pPropGridManager;

    DECLARE_EVENT_TABLE()
};

#endif