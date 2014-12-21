#ifndef BEYOND_ENGINE_EDITOR_EDITORMAINFRAME_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EDITORMAINFRAME_H__INCLUDE

#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/aui/aui.h>
#include "EngineProperGridManager.h"
#include "GLCanvas.h"
#include "wx/splitter.h"
#include "wx/dataview.h"
#include "Language/LanguageManager.h"

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
class CCoordinateSettingWnd;
class CScanFileDialog;
class CParticleControlWnd;
class wxChoice;
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
    ID_ViewButton_Game,

    ID_TB_PerformBtn,

    ID_CameraBtn,
    ID_ViewAllBtn,
    ID_CaptureBtn,
    ID_SettingBtn,
    ID_GMBtn,
    ID_ComponentInfoBtn,
    ID_BatchEditBtn,
    ID_PauseApplication,
    ID_SetCoordinateBtn,
    ID_UIEditMode,
    ID_PropertyGridManager,
    ID_ReloadResource,

    Menu_File_OpenProject,
    Menu_File_CloseProject,
    Menu_File_SaveProject,
    Menu_File_Undo,
    Menu_File_Redo,
    Menu_File_Export,
    Menu_File_PackResource,

    Menu_Open_TexturePreview,
    Menu_Edit_UI,
    Menu_Edit_Language,

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
    ePMS_CopyProperty,
    ePMS_PasteProperty,
    ePMS_Import,

    Ctrl_Button_LayerUp,
    Ctrl_Button_LayerDown,
    Ctrl_Button_LayerTop,
    Ctrl_Button_LayerBottom,

    Ctrl_Button_BrushSize1,
    Ctrl_Button_BrushSize2,
    Ctrl_Button_BrushSize3,
    Ctrl_Button_BrushSizeN,
    Ctrl_Button_Brush_LandingArea,
    Ctrl_Button_Brush_BattleArea,
    Ctrl_Button_Brush_ObjectBrush,
    Ctrl_Button_Brush_Eraser,

    Ctrl_Tree_CompontentFile,
    Ctrl_Tree_CompontentTemplate,
    Ctrl_Tree_CompontentProxy,
    Ctrl_Tree_LastOpenFiles,
    Ctrl_Tree_Resource,
    Ctrl_Tree_Effects,

    Ctrl_Search_TreeSearch,
    Ctrl_CheckBox_ShowGuidId,

    Ctrl_TimeBar_Button_Add,
    Ctrl_TimeBar_Button_Delete,
    Ctrl_TimeBar_Spin_Fps,

    Ctrl_Choice_Universe_Type,
    Ctrl_CheckBox_InitUniverseEdit,
    Ctrl_CheckBox_HideCover,
    Ctrl_CheckBox_HideMovableCell,
    Ctrl_CheckBox_ShowIndexText,
    Ctrl_Button_ApplyMapSize,
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
    struct SComvertPngFileSizeInfo
    {
        uint32_t uOriginSize = 0;
        uint32_t uConvertSize = 0;
        uint32_t uCompressSize = 0;
    };

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
    void ExportResourcePack(bool bConsoleMode = false);
    void SaveProject();
    void Undo();
    void Redo();
    void InitComponentsPage();
    void GetTexturePreviewDialog();
    void GetEditLanguageDialog();
    void ShowScanFilesDialog();
    void GetPerformanceDialog();
    void AddComponentFile();
    void AddComponentFileFolder();
    void OpenComponentFileDirectory();
    void HideTreeCtrl(wxTreeCtrl* pTreeCtrl);
    void SearchInCurrentTree();
    void DeleteComponentFile();
    void CreatePathDir(TString& strPath);
    void ConvertPNG(TString& strPath, TString& strETCDestPath, TString& strPVRDestPath);
    void OpenProjectFile(const TCHAR* pPath);
    void ActivateFile(const TCHAR* pszFileName);
    void OpenComponentFile( const TCHAR* pFilePath, wxTreeItemId* itemId, bool bCloseLoadedFile);
    void CloseComponentFile(bool bRemindSave = true);
    void SelectComponent(CComponentProxy* pComponentInstance);
    void InitializeComponentFileTree(CComponentProjectDirectory* pProjectData, const wxTreeItemId& id);
    void OnSetStartFile(wxTreeItemId item);
    void ResolveIdConflict(const std::map<uint32_t, std::vector<uint32_t>>& conflictIdMap);
    void DeleteItemInComponentFileList(wxTreeItemId itemId, bool bDeletePhysicalFile);
    void LanguageSwitch(ELanguageType language, bool bForceLoad = false);
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
    void OnKeyInCurrentView(wxKeyEvent& event);
    void OnMouseInCurrentView(wxMouseEvent& event);
    void OnSelectTreeCtrl(wxTreeEvent& event);
    void ResetPropertyValue(wxPGProperty* pProperty);
    void SetCursor();
    void SetCursor(wxStockCursor cursorType);
    void Update();
    virtual bool Show(bool show = true) override;
    CEditorSceneWindow* GetSceneWindow() const;
    CViewAgentBase* GetCurrentViewAgent();
    CViewAgentBase* GetViewAgent(int nViewID);
    wxAuiToolBar* GetAuiToolBarPerformPtr();
    CComponentProxy* GetSelectedComponent();
    CEditPerformanceDialog* GetPerformanceDialogPtr() const;
    CEditCameraWnd* GetCameraEditWnd() const;
    CSceneGridWnd* GetSceneGridWnd() const;
    CBeyondEngineEditorComponentWindow* GetComponentWindow();
    CEnginePropertyGridManager* GetPropGridManager();
    wxSplitterWindow* GetSplitter() const;
    CParticleControlWnd* GetParticleControlWnd() const;
    void UpdateComponentInstanceTreeCtrl();
    void UpdateLastOpenFilesTreeCtrl();
    void UpdateLastOpenFilesList(const TCHAR* pFilePath);
    void ConvertPNGThread();
    void OnComponentPropertyChange(CComponentBase* pComponent);
    void JumpToComponent(uint32_t uComponentId);
    void AddExportFileFullPathList(TString strFullPath);
    const std::set<TString>& GetExportFileFullPathList() const;
    void AddLastOpenFilesTreeCtrl();
    void ExportUINodeHeadFile();
    void SetConvertPngTypeString(const TString& strType);

protected:
    void OnTimeBarDraggingFrame(wxCommandEvent& event);
    void OnTimeBarCursorChange(wxCommandEvent& event);
    void OnTimeBarAddButtonClick(wxCommandEvent& event);
    void OnTimeBarSelectFrameChange(wxCommandEvent& event);
    void OnTimeBarMinusButtonClick(wxCommandEvent& event);
    void OnTimeBarItemContainerRClick(wxCommandEvent& event);
    void OnTimeBarChoice(wxCommandEvent& event);
    void OnTimeBarTreeItemSelected(wxCommandEvent& event);
    void OnTimeBarTreeItemDragEnd(wxCommandEvent& event);
    void OnPropertyChanged(wxPropertyGridEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnActivateTreeItem(wxTreeEvent& event);
    void OnActivateComponentFile(wxTreeEvent& event);
    void OnActivateLastFiles(wxTreeEvent& event);
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
    void OnPropertyGridSelect(wxPropertyGridEvent& event);
    void OnPropertyGridMenuClick(wxMenuEvent& event);
    void OnDataViewSelectionChange(wxDataViewEvent& event);
    void CompressData(const unsigned char* pSource, unsigned long uSourceLength, CSerializer& compressedData);
    void GenerateResourceList();
    void EncryptXMLFiles(const TCHAR* pszTargetDirectoryName);
    void StartConvertPng();
    void CollectPngFiles();
    bool GenerateVersionInfo();
    bool CanPasteProperty(const CPropertyDescriptionBase* pSource, const CPropertyDescriptionBase* pTarget) const;
    void OnViewChoiceChanged(wxCommandEvent& event);
    void CopyEtcAndPvrFile(wxArrayString& files, TString& strPath, TString& strAndroidAssetsResourcePath, const TCHAR* pszTargetDirectoryName);
    void OpenComponentFileTreeClick(const TCHAR* pszFileName, wxTreeItemId* pItemId);

private:
    void ConvertPNG2ETC(const TString& strPath, TString& strETCDestPath);
    void ConvertPNG2PVR(const TString& strPath, TString& strPVRDestPath);
    void RecordPackPngFileSize();
private:
    wxStockCursor       m_nCursorIconID;
    EMouseType          m_eMouseType;
    int                 m_nSashPosition;
    int                 m_nCurrentViewID;
    int                 m_nPng2EtcFinishCount;
    int                 m_nTaskCurCount;
    bool                m_bIsShowGuidId;
    bool                m_bSearchTextUpdate;
    TString             m_strConverPNGType;  // 1 : ETC 2 : PVR
    std::mutex          m_nPng2EtcProcessLock;
    wxTimer             m_Timer;
    LARGE_INTEGER       m_uTimeFrequency;
    uint32_t            m_uLastSearchTextUpdateTime;
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
    wxTreeCtrl*         m_pComponentProxyTreeCtrl;
    wxTreeCtrl*         m_pLastOpenFilesTreeCtrl = nullptr;
    std::map<TString, wxTreeItemId> m_lastOpenFilesTreeItemIdMap;
    wxTreeCtrl*         m_pEffectTreeCtrl;
    wxTreeCtrl*         m_pResourceTreeCtrl;
    CTimeBarFrame*      m_pTimeBar;
    wxPropertyGrid*     m_pPropGrid;
    wxCheckBox*         m_pShowGUIDCheckBox;
    wxChoice*           m_pCoordinateRenderTypeChoice;
    wxToggleButton*     m_pSceneViewBtn;
    wxToggleButton*     m_pUIViewBtn;
    wxToggleButton*     m_pAniViewBtn;
    wxToggleButton*     m_pTerrainViewBtn;
    wxToggleButton*     m_pGameViewBtn;
    CEditDialogBase*            m_pTexturePreviewDialog;
    CEditDialogBase*            m_pWEditLanguage;
    CScanFileDialog*            m_pScanFileDialog;
    CAboutDlg*                  m_pAboutDialog;
    CEditPerformanceDialog*     m_pPerformanceDialog;
    CEditAnimationDialog*       m_pSkeletonAnimationDialog;
    CEditCameraWnd*             m_pCameraWnd;
    CSceneGridWnd*              m_pSceneGridWnd;
    CComponentInfoWnd*          m_pComponentInfoWnd;
    CBatchEditWnd*              m_pBatchEditWnd;
    CParticleControlWnd*        m_pParticleControlWnd = nullptr;
    wxSplitterWindow*           m_pSplitter;
    CComponentProxy*            m_pSelectedComponentProxy;
    CPropertyDescriptionBase* m_pCopyProperty = nullptr;
    CEditorSceneWindow*         m_pViewScreen;
    CBeyondEngineEditorComponentWindow*   m_pComponentRenderWindow;
    CCoordinateSettingWnd*  m_pSetCoordinateRenderObjectWnd;
    CEnginePropertyGridManager*     m_pPropGridManager = nullptr;
    std::map<TString, TString> m_ETCFileMd5Map;
    std::map<TString, TString> m_PVRFileMd5Map;
    std::map<TString, TString> m_oldETCFileMd5Map;
    std::map<TString, TString> m_oldPVRFileMd5Map;
    std::vector<std::pair<TString, std::map<TString, TString>>> m_pngFiles;
    std::vector<CBeyondEngineEditorGLWindow*> m_updateWindowVector;
    std::map<uint32_t, wxTreeItemId>  m_componentTreeIdMap; //use this map to quick find the wxItemId for a specific id of component
    std::map<uint32_t, wxTreeItemId>  m_componentTreeIdSearchMap;
    std::map<TString, wxTreeItemId> m_componentCatalogNameMap; //use this map to quick find the wxItemId for a catalog
    std::map<TString, wxTreeItemId> m_componentFileListMap; //use this map to quick find the wxItemId for a component file name.
    std::map<TString, wxTreeItemId> m_componentFileListSearchMap;
    std::set<TString> m_exportFileFullPathList;
    std::set<TString> m_compressTextureSet;
    std::mutex m_compressLock;
    std::map<TString, SComvertPngFileSizeInfo> m_pngConvertFileSizeMap;
    DECLARE_EVENT_TABLE();
};

#endif