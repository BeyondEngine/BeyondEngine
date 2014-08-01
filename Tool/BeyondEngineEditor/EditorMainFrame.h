#ifndef BEYOND_ENGINE_EDITOR_EDITORMAINFRAME_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EDITORMAINFRAME_H__INCLUDE

#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/aui/aui.h>
#include "EngineProperGridManager.h"
#include "GLCanvas.h"
#include "wx/splitter.h"
#include "wx/dataview.h"

class wxSearchCtrl;
class wxSplitterWindow;
class wxToggleButton;
class CTimeBarFrame;
class CComponentGraphic;
class CBeyondEngineEditorGLWindow;
class CBeyondEngineEditorComponentWindow;
class CEditDialogBase;
class CEditPerformanceDialog;
class CEditCameraWnd;
class CTerrainViewAgent;
class CGameViewAgent;
class CUIViewAgent;
class CViewAgentBase;
class CSceneViewAgent;
class CAniViewAgent;
class CEffectViewAgent;
class CSceneGridWnd;
class CComponentInfoWnd;
class CBatchEditWnd;
class CEditAnimationDialog;
class CEditorSceneWindow;
class CAboutDlg;

enum EMouseType
{
    eMT_Type_Normal,
    eMT_Type_Brush,
    eMT_Type_Drag
};

enum ECtrlID
{
    ID_CustomizeToolbar = wxID_HIGHEST+1,
    ID_TB_Reopen,

    ID_ViewButton_Scene,
    ID_ViewButton_UI,
    ID_ViewButton_Ani,
    ID_ViewButton_Terrain,
    ID_ViewButton_Effect,
    ID_ViewButton_Game,

    ID_TB_PerformBtn,

    ID_CameraBtn,
    ID_ViewAllBtn,
    ID_CaptureBtn,
    ID_SettingBtn,
    ID_ComponentInfoBtn,
    ID_BatchEditBtn,
    ID_SkeletonAnimationEditBtn,

    Menu_File_OpenProject,
    Menu_File_CloseProject,
    Menu_File_SaveProject,
    Menu_File_Export,

    Menu_Open_TexturePreview,
    Menu_Edit_UI,
    Menu_Edit_Language,
    Menu_Edit_SkeletonAnimation,

    Menu_Window_Check_Scene,
    Menu_Window_Check_UI,
    Menu_Window_Check_Ani,
    Menu_Window_Check_Terrain,
    Menu_Window_Check_Effect,
    Menu_Window_Check_Game,

    Menu_Help_Language,
    Menu_Help_About,
    Menu_Help_Language_Chinese,
    Menu_Help_Language_English,

    eFLMS_FileTree_AddFile,
    eFLMS_FileTree_AddFileFolder,
    eFLMS_FileTree_Delete,
    eFLMS_FileTree_OpenFileDirectory,
    eFLMS_FileTree_SetStartFile,

    ePM_EffectTree_Add,
    ePM_EffectTree_Delete,

    ePMS_ResetValue,
    ePMS_Expand,
    ePMS_Collapsed,

    Ctrl_Button_LayerUp,
    Ctrl_Button_LayerDown,
    Ctrl_Button_LayerTop,
    Ctrl_Button_LayerBottom,
    Ctrl_Button_BrushModel,
    Ctrl_Button_BeginEmit,

    Ctrl_Text_BrushSize,
    Ctrl_Choice_BrushType,

    Ctrl_Tree_CompontentFile,
    Ctrl_Tree_CompontentTemplate,
    Ctrl_Tree_CompontentInstance,
    Ctrl_Tree_Resource,
    Ctrl_Tree_Effects,

    Ctrl_Search_TreeSearch,
    Ctrl_CheckBox_ShowGuidId,

    Ctrl_TimeBar_Button_Add,
    Ctrl_TimeBar_Button_Delete,
    Ctrl_TimeBar_Spin_Fps,
};

class CEditorMainFrame : public wxFrame
{
    friend CGameViewAgent;
    friend CAniViewAgent;
    friend CUIViewAgent;
    friend CTerrainViewAgent;
    friend CViewAgentBase;
    friend CSceneViewAgent;
    friend CEffectViewAgent;

public:
    CEditorMainFrame(const wxString& title);
    virtual ~CEditorMainFrame();
    void InitFrame();
    void InitMenu();
    void InitCtrls();
    // We can only do something after all controls are ready and the OPENGL is initialized.
    void PostInitialize();
    void CreateExtraWindow();
    void CreateAuiToolBar();
    void CreateAuiNoteBook();
    void CreateTreeCtrl();
    void CreateTimeBar();
    void CreatePropertyGrid();
    void CreateSplitter();
    void AddPageToBook();
    void UpdatePropertyGrid();
    void CloseProjectFile();
    void OpenProject();
    void CloseProject();
    void Export();
    void SaveProject();
    void InitComponentsPage();
    void GetTexturePreviewDialog();
    void GetEditLanguageDialog();
    void GetSplineDialog();
    void GetPerformanceDialog();
    void AddComponentFile();
    void AddComponentFileFolder();
    void OpenComponentFileDirectory();
    void HideTreeCtrl(wxTreeCtrl* pTreeCtrl);
    void SearchInCurrentTree();
    void DeleteComponentFile();
    void OpenProjectFile( const TCHAR* pPath );
    void ActivateFile(const TCHAR* pszFileName);
    void OpenComponentFile( const TCHAR* pFilePath, wxTreeItemId* itemId = NULL);
    void CloseComponentFile(bool bRemindSave = true);
    void SelectComponent(CComponentProxy* pComponentInstance);
    void InitializeComponentFileTree(CComponentProjectDirectory* pProjectData, const wxTreeItemId& id);
    void OnSetStartFile(wxTreeItemId item);
    void ResolveIdConflict(const std::map<size_t, std::vector<size_t>>& conflictIdMap);
    void DeleteItemInComponentFileList(wxTreeItemId itemId, bool bDeletePhysicalFile);
    void LanguageSwitch(ELanguage language);
    void ToggleViewButton();
    void RefreshLanguage();
    void ShowPerformDlg();
    void ShowAboutDlg();
    void SetChildWindowStyle(CEditDialogBase* pDlg);
    bool GetShowGuidId();
    void StartRenderUpdate();
    void RegisterUpdateWindow(CBeyondEngineEditorGLWindow* pWindow);
    void SelectViewID(int nViewID);
    int GetSelectdViewID();
    void SetMouseType(EMouseType iType);
    EMouseType GetMouseType() const;
    void OnMouseInCurrentView( wxMouseEvent& event );
    void OnTreeCtrlFocused( wxFocusEvent& event );
    void ResetPropertyValue( wxPGProperty* pProperty );
    void SetCursor();
    void Update();
    virtual bool Show(bool show = true) override;
    CEditorSceneWindow* GetSceneWindow() const;
    CViewAgentBase* GetViewAgent(int nViewID);
    wxAuiToolBar* GetAuiToolBarPerformPtr();
    CComponentProxy* GetSelectedComponent();
    CEditPerformanceDialog* GetPerformanceDialogPtr() const;
    CEditCameraWnd* GetCameraEditWnd() const;
    CBeyondEngineEditorComponentWindow* GetComponentWindow();
    CEnginePropertyGirdManager* GetPropGridManager();
    wxSplitterWindow* GetSplitter() const;
    void UpdateComponentInstanceTreeCtrl();

protected:
    void OnPropertyChanged(wxPropertyGridEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnActivateTreeItem(wxTreeEvent& event);
    void OnActivateComponentFile(wxTreeEvent& event);
    void OnActivateComponentTemplate(wxTreeEvent& event);
    void OnTreeCtrlRightClick( wxTreeEvent& event );
    void OnComponentFileStartDrag( wxTreeEvent& event );
    void OnComponentFileEndDrag( wxTreeEvent& event );
    void OnComponentStartDrag( wxTreeEvent& event );
    void OnComponentEndDrag( wxTreeEvent& event );
    void OnIdle(wxIdleEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnSearchTextUpdate(wxCommandEvent& event );
    void OnCommandEvent(wxCommandEvent& event);
    void OnSelectComponentTemplateTreeItem(wxTreeEvent& event);
    void OnTreeCtrlExpanded(wxTreeEvent& event);
    void OnTreeCtrlCollapsed(wxTreeEvent& event);
    void OnEditTreeItemLabel(wxTreeEvent& event);
    void OnSelectComponentInstanceTreeItem(wxTreeEvent& event);
    void OnSelectTreeItem(wxTreeEvent& event);
    void OnCheckBoxGuidId(wxCommandEvent& event);
    void OnPropertyGridRightClick(wxPropertyGridEvent& event);
    void OnPropertyGridMenuClick(wxMenuEvent& event);
    void OnDataViewSelectionChange(wxDataViewEvent& event);
    void OnEditDataViewItem(wxDataViewEvent& event);

private:
    int                 m_nHitTestRet;
    wxStockCursor       m_nCursorIconID;
    EMouseType          m_eMouseType;
    int                 m_nSashPosition;
    int                 m_nCurrentViewID;
    bool                m_bIsShowGuidId;
    bool                m_bSearchTextUpdate;
    wxPoint             m_activeGUIPosOffset;
    wxTimer             m_Timer;
    LARGE_INTEGER       m_uTimeFrequency;
    size_t              m_uLastSearchTextUpdateTime;
    wxAuiManager        m_Manager;
    wxColor             m_itemTextcolor;
    wxTreeItemId        m_activeFileItemId;
    wxTreeItemId        m_startFileItemId;
    wxMenuBar*          m_pMenuBar;
    wxMenu*             m_pFileMenu;
    wxMenu*             m_pEditMenu;
    wxMenu*             m_pWindowMenu;
    wxMenu*             m_pHelpMenu;
    wxMenu*             m_pHelpLanguageMenu;
    wxMenu*             m_pComponentFileMenu;
    wxMenu*             m_pPropertyMenu;

    wxPanel*            m_pLeftPanel;
    wxAuiNotebook*      m_pLeftBook;
    wxPanel*            m_pRightPanel;
    wxAuiNotebook*      m_pRightBook;
    wxPanel*            m_pBottomPanel;
    wxAuiNotebook*      m_pBottomBook;
    wxPanel*            m_pCenterPanel;
    wxAuiNotebook*      m_pCenterBook;
    wxPanel*            m_pResourcePanel;
    wxAuiNotebook*      m_pResourceBook;
    wxPanel*            m_pToolPanel;
    wxAuiNotebook*      m_pToolBook;

    wxAuiToolBar*       m_pToolBar;
    wxSearchCtrl*       m_pSearch;
    wxTreeCtrl*         m_pComponentFileTreeCtrl;
    wxTreeCtrl*         m_pComponentTemplateTreeCtrl;
    wxTreeCtrl*         m_pComponentInstanceTreeCtrl;
    wxTreeCtrl*         m_pEffectTreeCtrl;
    wxTreeCtrl*         m_pResourceTreeCtrl;
    CTimeBarFrame*      m_pTimeBar;
    wxPropertyGrid*     m_pPropGrid;
    wxCheckBox*         m_pShowGUIDCheckBox;
    wxToggleButton*     m_pSceneViewBtn;
    wxToggleButton*     m_pUIViewBtn;
    wxToggleButton*     m_pAniViewBtn;
    wxToggleButton*     m_pTerrainViewBtn;
    wxToggleButton*     m_pEffectViewBtn;
    wxToggleButton*     m_pGameViewBtn;
    CEditDialogBase*            m_pTexturePreviewDialog;
    CEditDialogBase*            m_pWEditLanguage;
    CEditDialogBase*            m_pSplineDialog;
    CAboutDlg*                  m_pAboutDialog;
    CEditPerformanceDialog*     m_pPerformanceDialog;
    CEditAnimationDialog*       m_pSkeletonAnimationDialog;
    CEditCameraWnd*             m_pCameraWnd;
    CSceneGridWnd*              m_pSceneGridWnd;
    CComponentInfoWnd*          m_pComponentInfoWnd;
    CBatchEditWnd*              m_pBatchEditWnd;
    wxSplitterWindow*           m_pSplitter;
    CComponentProxy*            m_pSelectedComponentProxy;
    CEditorSceneWindow*         m_pViewScreen;
    CBeyondEngineEditorComponentWindow*   m_pComponentRenderWindow;
    CEnginePropertyGirdManager*     m_pPropGridManager;
    std::vector<CBeyondEngineEditorGLWindow*> m_updateWindowVector;
    std::map<size_t, wxTreeItemId>  m_componentTreeIdMap; //use this map to quick find the wxItemId for a specific id of component
    std::map<size_t, wxTreeItemId>  m_componentTreeIdSearchMap;
    std::map<TString, wxTreeItemId> m_componentCatalogNameMap; //use this map to quick find the wxItemId for a catalog
    std::map<TString, wxTreeItemId> m_componentFileListMap; //use this map to quick find the wxItemId for a component file name.
    std::map<TString, wxTreeItemId> m_componentFileListSearchMap;

    DECLARE_EVENT_TABLE();
};

#endif