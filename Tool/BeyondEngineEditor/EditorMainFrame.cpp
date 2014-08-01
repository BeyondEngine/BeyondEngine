#include "stdafx.h"
#include "EditorMainFrame.h"
#include "EditAnimationDialog.h"
#include "timebarframe.h"
#include "EngineEditor.h"
#include "ComponentTreeItemData.h"
#include "ComponentFileTreeItemData.h"
#include "ComponentInstanceTreeItemData.h"
#include "BeyondEngineEditorComponentWindow.h"
#include "ComponentGraphics_GL.h"
#include "Resource/ResourcePathManager.h"
#include "EditLanguageDialog.h"
#include "SplineDialog.h"
#include "EditPerformanceDialog.h"
#include "Render/Camera.h"
#include "TimeMeter.h"
#include "EngineCenter.h"
#include "Framework/Application.h"
#include "ListPropertyDescription.h"
#include "PtrPropertyDescription.h"
#include "TerrainViewAgent.h"
#include "GameViewAgent.h"
#include "UIViewAgent.h"
#include "SceneViewAgent.h"
#include "AniViewAgent.h"
#include "ViewAgentBase.h"
#include "EffectViewAgent.h"
#include "TexturePreviewDialog.h"
#include "Utility/TinyXML/tinyxml.h"
#include "Utility/BeatsUtility/UtilityManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxy.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProject.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentGraphic.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProjectDirectory.h"
#include "Utility/BeatsUtility/ComponentSystem/ComponentPublic.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include <wx/msw/private.h>
#include <wx/choicebk.h>
#include <wx/tglbtn.h>
#include <wx/artprov.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/numdlg.h>
#include <wx/srchctrl.h>
#include <wx/timer.h>
#include <wx/filename.h>
#include <wx/filesys.h>
#include "GradientDialog.h"
#include "EditCameraWnd.h"
#include "wxGLRenderWindow.h"
#include "Render/RenderTarget.h"
#include "Render/RenderManager.h"
#include "SceneGridWnd.h"
#include "EditorConfig.h"
#include "ComponentInfoWnd.h"
#include "BatchEditWnd.h"
#include "GUI/Font/FontManager.h"
#include "GUI/Font/FontFace.h"
#include "EditorSceneWindow.h"
#include "AboutDlg.h"

#define MAINFRAMENORMALSIZE wxSize(1024, 800)
#define MAINFRAMEPOSITION wxPoint(40, 40)
#define BUTTONSIZE wxSize(20, 20)
#define SIZERBORDERWIDTH 5
#define PANELSIZESCALE 0.10
#define RIGHTPANELSIZESCALE 0.19
#define IDBEGIN 1111

enum
{
    PARTICLE_SIMULATION_WORLD = 1,
    PARTICLE_SIMULATION_LOCAL
};

enum EMenuID
{
    eMB_File,
    eMB_Edit,
    eMB_Window,
    eMB_Help
};

BEGIN_EVENT_TABLE(CEditorMainFrame, wxFrame)
EVT_IDLE(CEditorMainFrame::OnIdle)
EVT_TIMER(wxID_ANY, CEditorMainFrame::OnTimer)
EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, CEditorMainFrame::OnCommandEvent)
EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, CEditorMainFrame::OnCommandEvent)
EVT_TREE_ITEM_ACTIVATED(wxID_ANY, CEditorMainFrame::OnActivateTreeItem)
EVT_TREE_BEGIN_DRAG(Ctrl_Tree_CompontentFile,CEditorMainFrame::OnComponentFileStartDrag)
EVT_TREE_END_DRAG(Ctrl_Tree_CompontentFile,CEditorMainFrame::OnComponentFileEndDrag)
EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY,CEditorMainFrame::OnTreeCtrlRightClick)
EVT_TREE_BEGIN_DRAG(Ctrl_Tree_CompontentTemplate,CEditorMainFrame::OnComponentStartDrag)
EVT_TREE_END_DRAG(Ctrl_Tree_CompontentTemplate,CEditorMainFrame::OnComponentEndDrag)
EVT_SEARCHCTRL_SEARCH_BTN(wxID_ANY, CEditorMainFrame::OnSearch)
EVT_TEXT_ENTER(wxID_ANY, CEditorMainFrame::OnSearch)
EVT_TEXT(Ctrl_Search_TreeSearch, CEditorMainFrame::OnSearchTextUpdate)
EVT_TREE_ITEM_EXPANDED(wxID_ANY, CEditorMainFrame::OnTreeCtrlExpanded)
EVT_TREE_ITEM_COLLAPSED(wxID_ANY, CEditorMainFrame::OnTreeCtrlCollapsed)
EVT_TREE_SEL_CHANGED(wxID_ANY, CEditorMainFrame::OnSelectTreeItem)
EVT_TREE_END_LABEL_EDIT(wxID_ANY, CEditorMainFrame::OnEditTreeItemLabel)
EVT_CHECKBOX(Ctrl_CheckBox_ShowGuidId, CEditorMainFrame::OnCheckBoxGuidId)
EVT_TOGGLEBUTTON(wxID_ANY, CEditorMainFrame::OnCommandEvent)
EVT_PG_RIGHT_CLICK(wxID_ANY, CEditorMainFrame::OnPropertyGridRightClick)
EVT_PG_CHANGED(wxID_ANY, CEditorMainFrame::OnPropertyChanged)
EVT_DATAVIEW_ITEM_EDITING_DONE(wxID_ANY, CEditorMainFrame::OnEditDataViewItem)
END_EVENT_TABLE()

CEditorMainFrame::CEditorMainFrame(const wxString& title) 
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, MAINFRAMENORMALSIZE, wxDEFAULT_FRAME_STYLE | wxMAXIMIZE)
    , m_nHitTestRet(0)
    , m_nCursorIconID(wxCURSOR_ARROW)
    , m_nSashPosition(0)
    , m_pTexturePreviewDialog(NULL)
    , m_pWEditLanguage(NULL)
    , m_bSearchTextUpdate(false)
    , m_pSplineDialog(NULL)
    , m_pAboutDialog(NULL)
    , m_pPerformanceDialog(NULL)
    , m_pSkeletonAnimationDialog(NULL)
    , m_pCameraWnd(NULL)
    , m_pSceneGridWnd(NULL)
    , m_pSelectedComponentProxy(NULL)
    , m_pToolBar(NULL)
    , m_activeFileItemId(NULL)
    , m_pResourcePanel(NULL)
    , m_pResourceBook(NULL)
    , m_pToolPanel(NULL)
    , m_pToolBook(NULL)
    , m_pResourceTreeCtrl(NULL)
    , m_pComponentFileMenu(NULL)
    , m_bIsShowGuidId(false)
    , m_Timer(this, wxID_ANY)
    , m_nCurrentViewID(INVALID_DATA)
    , m_eMouseType(eMT_Type_Normal)
    , m_pViewScreen(nullptr)
    , m_pComponentRenderWindow(nullptr)
    , m_pMenuBar(NULL)
{
    SetIcon(wxICON(sample));
    QueryPerformanceFrequency(&m_uTimeFrequency);
}

CEditorMainFrame::~CEditorMainFrame()
{
    m_Timer.Stop();
    BEATS_SAFE_DELETE(m_pComponentFileMenu);
    BEATS_SAFE_DELETE(m_pPropertyMenu);
    m_pTexturePreviewDialog->Destroy();
    m_Manager.UnInit();
}

void CEditorMainFrame::InitMenu()
{
    m_pMenuBar              = new wxMenuBar;
    m_pFileMenu             = new wxMenu;
    m_pEditMenu             = new wxMenu;
    m_pWindowMenu           = new wxMenu;
    m_pHelpMenu             = new wxMenu;
    m_pHelpLanguageMenu     = new wxMenu;
    m_pComponentFileMenu    = new wxMenu;
    m_pPropertyMenu         = new wxMenu;

    m_pFileMenu->Append(Menu_File_OpenProject, wxT("Open"));
    m_pFileMenu->Append(Menu_File_CloseProject, wxT("Close"));
    m_pFileMenu->Append(Menu_File_SaveProject, wxT("&Save\tCtrl-S"));
    m_pFileMenu->Append(Menu_File_Export, wxT("Export"));
    m_pEditMenu->Append(Menu_Open_TexturePreview, wxT("Edit Animation"));
    m_pEditMenu->Append(Menu_Edit_Language, wxT("Edit Language"));
    m_pEditMenu->Append(Menu_Edit_SkeletonAnimation, wxT("Edit skeleton animation"));

    m_pHelpMenu->Append(Menu_Help_Language, wxT("Language"), m_pHelpLanguageMenu);
    m_pHelpMenu->Append(Menu_Help_About, L10N_T(eL_EditorMenu_About));
    m_pHelpLanguageMenu->Append(Menu_Help_Language_Chinese, wxT("Chinese"));
    m_pHelpLanguageMenu->Append(Menu_Help_Language_English, wxT("English"));

    wxMenu *pWindowSubMenu = new wxMenu;
    pWindowSubMenu->Append(Menu_Window_Check_Scene, L10N_T(eL_SceneView), wxT("Show or hide Scene view button"), true);
    pWindowSubMenu->Append(Menu_Window_Check_UI, L10N_T(eL_UIView), wxT("Show or hide UI view button"), true);
    pWindowSubMenu->Append(Menu_Window_Check_Ani, L10N_T(eL_AnimationView), wxT("Show or hide Ani view button"), true);
    pWindowSubMenu->Append(Menu_Window_Check_Terrain, L10N_T(eL_TerrainView), wxT("Show or hide Terrain view button"), true);
    pWindowSubMenu->Append(Menu_Window_Check_Effect, L10N_T(eL_EffectView), wxT("Show or hide Effect view button"), true);
    pWindowSubMenu->Append(Menu_Window_Check_Game, L10N_T(eL_GameView), wxT("Show or hide Game view button"), true);
    m_pWindowMenu->AppendSubMenu(pWindowSubMenu, _T("View Radio"));

    m_pMenuBar->Append(m_pFileMenu, wxT("&File"));
    m_pMenuBar->Append(m_pEditMenu, wxT("&Edit"));
    m_pMenuBar->Append(m_pWindowMenu, wxT("&Window"));
    m_pMenuBar->Append(m_pHelpMenu, wxT("&Help"));

    m_pMenuBar->Check(Menu_Window_Check_Scene, true);
    m_pMenuBar->Check(Menu_Window_Check_UI, true);
    m_pMenuBar->Check(Menu_Window_Check_Ani, true);
    m_pMenuBar->Check(Menu_Window_Check_Terrain, true);
    m_pMenuBar->Check(Menu_Window_Check_Effect, true);
    m_pMenuBar->Check(Menu_Window_Check_Game, true);
    SetMenuBar(m_pMenuBar);

    m_pComponentFileMenu->Append(eFLMS_FileTree_AddFile, _T("添加文件"));
    m_pComponentFileMenu->Append(eFLMS_FileTree_AddFileFolder, _T("添加文件夹"));
    m_pComponentFileMenu->Append(eFLMS_FileTree_Delete, _T("删除文件"));
    m_pComponentFileMenu->Append(eFLMS_FileTree_OpenFileDirectory, _T("打开所在的文件夹"));
    m_pComponentFileMenu->Append(eFLMS_FileTree_SetStartFile, _T("设置为启动文件"));

    m_pPropertyMenu->Append(ePMS_ResetValue, _T("撤销修改"));
    m_pPropertyMenu->Append(ePMS_Expand, _T("展开"));
    m_pPropertyMenu->Append(ePMS_Collapsed, _T("收拢"));
    m_pPropertyMenu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CEditorMainFrame::OnPropertyGridMenuClick), NULL, this);
}

void CEditorMainFrame::InitCtrls()
{
    CreateAuiToolBar();
    CreateExtraWindow();
    CreateAuiNoteBook();
    CreateTreeCtrl();
    CreateTimeBar();
    CreatePropertyGrid();
    CreateSplitter();

    AddPageToBook();
    m_Manager.Update();
}

void CEditorMainFrame::CreateAuiToolBar()
{
    m_pToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_TEXT | wxAUI_TB_HORIZONTAL);
    const TString& strLastOpenProject = CEditorConfig::GetInstance()->GetLastOpenProject();
    m_pToolBar->AddTool(ID_TB_Reopen, L10N_T(eL_ReopenProject), wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK), strLastOpenProject.c_str());
    if (strLastOpenProject.length() == 0)
    {
        m_pToolBar->EnableTool(ID_TB_Reopen, false);
    }
    m_pToolBar->AddSeparator();

    m_pToolBar->AddTool(ID_TB_PerformBtn, L10N_T(eL_Performance), wxArtProvider::GetBitmap(wxART_FILE_OPEN), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_CameraBtn, L10N_T(eL_Camera), wxArtProvider::GetBitmap(wxART_GO_HOME), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_ViewAllBtn, L10N_T(eL_ViewAll), wxArtProvider::GetBitmap(wxART_REPORT_VIEW), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_CaptureBtn, L10N_T(eL_Capture), wxArtProvider::GetBitmap(wxART_HELP_SETTINGS), wxEmptyString, wxITEM_NORMAL);
    m_pToolBar->AddTool(ID_SettingBtn, L10N_T(eL_Setting), wxArtProvider::GetBitmap(wxART_GO_FORWARD), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_ComponentInfoBtn,L10N_T(eL_Component), wxArtProvider::GetBitmap(wxART_HARDDISK), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_BatchEditBtn,L10N_T(eL_BatchEdit), wxArtProvider::GetBitmap(wxART_HELP_BOOK), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_SkeletonAnimationEditBtn,L10N_T(eL_Edit_Animation), wxArtProvider::GetBitmap(wxART_HELP_SETTINGS), wxEmptyString, wxITEM_NORMAL);

    m_pCameraWnd = new CEditCameraWnd(this, wxID_ANY, wxT("Camera"), wxPoint(500,200), wxSize(300,500), wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);
    m_pSceneGridWnd = new CSceneGridWnd(this, wxID_ANY, wxT("Grid"), wxPoint(500,200), wxSize(300,500), wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);
    m_pComponentInfoWnd = new CComponentInfoWnd(this, wxID_ANY, wxT("Grid"), wxPoint(500,200), wxSize(300,500), wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);
    m_pBatchEditWnd = new CBatchEditWnd(this, wxID_ANY, wxT("BatchEdit"), wxPoint(500,200), wxSize(300,500), wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);
    wxAuiPaneInfo info;
    info.Name(wxT("Perform")).ToolbarPane().Top().Movable(false).Resizable(true).Dockable(false);
    m_Manager.AddPane(m_pToolBar, info);
}

void CEditorMainFrame::CreateAuiNoteBook()
{
    wxSize client_size = GetClientSize();
    long lStyle = wxAUI_NB_TOP | wxAUI_NB_TAB_FIXED_WIDTH;
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);

    wxSize panelSize = wxSize(client_size.GetWidth() * PANELSIZESCALE, client_size.GetHeight() * PANELSIZESCALE);
    m_pLeftPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, panelSize);
    m_pLeftPanel->SetSizer(pSizer);
    m_pSearch = new wxSearchCtrl(m_pLeftPanel, Ctrl_Search_TreeSearch, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    m_pSearch->ShowCancelButton(true);
    m_pLeftBook = new wxAuiNotebook(m_pLeftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle | wxAUI_NB_SCROLL_BUTTONS);
    pSizer->Add(m_pLeftBook, 1, wxGROW|wxALL, 0);
    pSizer->Add(m_pSearch, 0, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pRightPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(client_size.GetWidth() * RIGHTPANELSIZESCALE, client_size.GetHeight() * RIGHTPANELSIZESCALE));
    m_pRightPanel->SetSizer(pSizer);
    m_pShowGUIDCheckBox = new wxCheckBox(m_pRightPanel, Ctrl_CheckBox_ShowGuidId, wxT("ShowGI"));
    pSizer->Add(m_pShowGUIDCheckBox, 0, wxGROW|wxALL, 0);
    m_pRightBook = new wxAuiNotebook(m_pRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pRightBook, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pBottomPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(client_size.GetWidth() * RIGHTPANELSIZESCALE, client_size.GetHeight() * RIGHTPANELSIZESCALE));
    m_pBottomPanel->SetSizer(pSizer);
    m_pBottomBook = new wxAuiNotebook(m_pBottomPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pBottomBook, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pResourcePanel = new wxPanel(this);
    m_pResourcePanel->SetSizer(pSizer);
    m_pResourceBook = new wxAuiNotebook(m_pResourcePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pResourceBook, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pCenterPanel = new wxPanel(this);
    m_pCenterPanel->SetSizer(pSizer);
    m_pCenterBook = new wxAuiNotebook(m_pCenterPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    m_pSceneViewBtn = new wxToggleButton(m_pCenterPanel, ID_ViewButton_Scene, L10N_T(eL_SceneView));
    m_pUIViewBtn = new wxToggleButton(m_pCenterPanel, ID_ViewButton_UI, L10N_T(eL_UIView));
    m_pAniViewBtn = new wxToggleButton(m_pCenterPanel, ID_ViewButton_Ani, L10N_T(eL_AnimationView));
    m_pTerrainViewBtn = new wxToggleButton(m_pCenterPanel, ID_ViewButton_Terrain, L10N_T(eL_TerrainView));
    m_pEffectViewBtn = new wxToggleButton(m_pCenterPanel, ID_ViewButton_Effect, L10N_T(eL_EffectView));
    m_pGameViewBtn = new wxToggleButton(m_pCenterPanel, ID_ViewButton_Game, L10N_T(eL_GameView));
    wxBoxSizer* pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    pButtonSizer->Add(m_pSceneViewBtn, 0, wxALIGN_CENTER|wxALL, 0);
    pButtonSizer->Add(m_pUIViewBtn, 0, wxALIGN_CENTER|wxALL, 0);
    pButtonSizer->Add(m_pAniViewBtn, 0, wxALIGN_CENTER|wxALL, 0);
    pButtonSizer->Add(m_pTerrainViewBtn, 0, wxALIGN_CENTER|wxALL, 0);
    pButtonSizer->Add(m_pEffectViewBtn, 0, wxALIGN_CENTER|wxALL, 0);
    pButtonSizer->AddSpacer(10);
    pButtonSizer->Add(m_pGameViewBtn, 0, wxALIGN_CENTER|wxALL, 0);
    pSizer->Add(pButtonSizer, 0, wxALIGN_CENTER|wxALL, 0);
    pSizer->Add(m_pCenterBook, 1, wxGROW|wxALL, 0);

    m_pToolPanel = new wxPanel(this);
    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pToolPanel->SetSizer(pSizer);
    m_pToolBook = new wxAuiNotebook(m_pToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pToolBook, 1, wxGROW|wxALL, 0);
}

void CEditorMainFrame::CreateTimeBar()
{
    m_pTimeBar = new CTimeBarFrame(m_pBottomBook);
    m_pTimeBar->SetCtrlsId(Ctrl_TimeBar_Button_Add, Ctrl_TimeBar_Button_Delete);
}

void CEditorMainFrame::CreatePropertyGrid()
{
    int style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPG_TOOLTIPS;
    int extraStyle = wxPG_EX_MODE_BUTTONS | wxPG_EX_MULTIPLE_SELECTION;

    m_pPropGridManager = new CEnginePropertyGirdManager();
    m_pPropGridManager->Create(m_pRightBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, style );
    m_pPropGrid = m_pPropGridManager->GetGrid();
    m_pPropGridManager->SetExtraStyle(extraStyle);
    m_pPropGridManager->SetValidationFailureBehavior( wxPG_VFB_MARK_CELL | wxPG_VFB_SHOW_MESSAGEBOX );
    m_pPropGrid->SetVerticalSpacing( 2 );
    m_pPropGridManager->AddPage(wxT("page"));
}

void CEditorMainFrame::CreateTreeCtrl()
{
    wxImageList *pFileIconImages = new wxImageList(TREEICONSIZE, TREEICONSIZE, true);
    wxIcon fileIcons[eTCIT_Count];
    wxSize iconSize(TREEICONSIZE, TREEICONSIZE);
    fileIcons[eTCIT_File] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_LIST, iconSize);
    fileIcons[eTCIT_FileSelected] = wxArtProvider::GetIcon(wxART_FILE_OPEN, wxART_LIST, iconSize);
    fileIcons[eTCIT_Folder] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_LIST, iconSize);
    fileIcons[eTCIT_FolderSelected] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_LIST, iconSize);
    fileIcons[eTCIT_FolderOpened] = wxArtProvider::GetIcon(wxART_FOLDER_OPEN, wxART_LIST, iconSize);

    long lStyle = wxTR_DEFAULT_STYLE | wxTR_FULL_ROW_HIGHLIGHT | wxTR_NO_LINES | wxNO_BORDER | wxTR_TWIST_BUTTONS;
    for (size_t i = 0; i < eTCIT_Count; ++i)
    {
        pFileIconImages->Add(fileIcons[i]);
    }
    m_pComponentFileTreeCtrl = new wxTreeCtrl(m_pLeftBook, Ctrl_Tree_CompontentFile, wxPoint(0,0), wxDefaultSize, lStyle | wxTR_EDIT_LABELS);
    m_pComponentFileTreeCtrl->AssignImageList(pFileIconImages);
    m_pComponentFileTreeCtrl->AddRoot(wxT("Root"), eTCIT_Folder, -1, new CComponentFileTreeItemData(NULL, _T("")));

    pFileIconImages = new wxImageList(TREEICONSIZE, TREEICONSIZE, true);
    for (size_t i = 0; i < eTCIT_Count; ++i)
    {
        pFileIconImages->Add(fileIcons[i]);
    }
    m_pComponentInstanceTreeCtrl = new wxTreeCtrl(m_pLeftBook, Ctrl_Tree_CompontentInstance, wxPoint(0,0), wxDefaultSize, lStyle | wxTR_EDIT_LABELS);
    m_pComponentInstanceTreeCtrl->AssignImageList(pFileIconImages);
    m_pComponentInstanceTreeCtrl->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(CEditorMainFrame::OnTreeCtrlFocused), NULL, this);
    m_pComponentInstanceTreeCtrl->AddRoot(wxT("ComponentInstance"), eTCIT_Folder, -1, new CComponentInstanceTreeItemData(NULL));
    m_pComponentInstanceTreeCtrl->Hide();

    pFileIconImages = new wxImageList(TREEICONSIZE, TREEICONSIZE, true);
    for (size_t i = 0; i < eTCIT_Count; ++i)
    {
        pFileIconImages->Add(fileIcons[i]);
    }
    m_pComponentTemplateTreeCtrl = new wxTreeCtrl(m_pResourceBook, Ctrl_Tree_CompontentTemplate, wxPoint(0,0), wxDefaultSize, lStyle);
    m_pComponentTemplateTreeCtrl->AssignImageList(pFileIconImages);
    m_pComponentTemplateTreeCtrl->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(CEditorMainFrame::OnTreeCtrlFocused), NULL, this);
    wxTreeItemId rootId = m_pComponentTemplateTreeCtrl->AddRoot(wxT("Components"), eTCIT_Folder, -1, new CComponentTreeItemData(true, 0));
    m_componentCatalogNameMap[_T("Root")] = rootId;
    m_componentTreeIdMap[0] = rootId;

    //TODO: use ResourceTreeCtrl to contain all resource
    pFileIconImages = new wxImageList(15, 15, true);
    for (size_t i = 0; i < eTCIT_Count; ++i)
    {
        pFileIconImages->Add(fileIcons[i]);
    }
    m_pResourceTreeCtrl = new wxTreeCtrl(m_pResourceBook, Ctrl_Tree_Resource, wxPoint(0,0), wxSize(160,250), lStyle| wxTR_EDIT_LABELS );
    m_pResourceTreeCtrl->AssignImageList(pFileIconImages);
    m_pResourceTreeCtrl->AddRoot(wxT("Resource"), eTCIT_Folder, -1, new CComponentInstanceTreeItemData(NULL));
}

void CEditorMainFrame::AddPageToBook()
{
    m_pLeftBook->Freeze();
    m_pLeftBook->AddPage( m_pComponentFileTreeCtrl, L10N_T(eL_ComponentFile) );
    m_pLeftBook->Thaw();

    m_pRightBook->Freeze();
    m_pRightBook->AddPage( m_pPropGridManager, L10N_T(eL_Property) );
    m_pRightBook->Thaw();

    m_pBottomBook->Freeze();
    m_pBottomBook->AddPage( m_pTimeBar, L10N_T(eL_Animation) );
    m_pBottomBook->Thaw();

    m_pCenterBook->Freeze();
    m_pCenterBook->AddPage( m_pSplitter, L10N_T(eL_View) );
    m_pCenterBook->Thaw();

    m_pResourceBook->Freeze();
    m_pResourceBook->AddPage( m_pComponentTemplateTreeCtrl, L10N_T(eL_Component) );
    m_pResourceBook->AddPage( m_pResourceTreeCtrl, wxT("Resource") );
    m_pResourceBook->Thaw();

    m_Manager.AddPane(m_pLeftPanel, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane1")).
        Left());
    wxAuiPaneInfo info2;
    info2.CenterPane().Name(wxT("Pane2")).Right();
    info2.dock_proportion = 10;

    m_Manager.AddPane(m_pRightPanel, info2);
    m_Manager.AddPane(m_pBottomPanel, wxAuiPaneInfo().CenterPane().Hide().
        Name(wxT("Pane3")).
        Bottom());
    m_Manager.AddPane(m_pCenterPanel, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane4")).
        Center());
    m_Manager.AddPane(m_pResourcePanel, wxAuiPaneInfo().CenterPane().Hide().
        Name(wxT("Pane5")).
        Left());
    wxAuiPaneInfo info6;
    info6.CenterPane().Name(wxT("Pane6")).Right();
    info6.dock_proportion = 1;
    m_Manager.AddPane(m_pToolPanel, info6);
}

void CEditorMainFrame::GetTexturePreviewDialog()
{
    BEATS_ASSERT(m_pTexturePreviewDialog != NULL);
    m_pTexturePreviewDialog->ShowModal();
}

void CEditorMainFrame::GetEditLanguageDialog()
{
    if (m_pWEditLanguage == NULL)
    {
        m_pWEditLanguage = new CEditLanguageDialog(this, wxID_ANY, wxT("Edit Language"), wxPoint(500,200), wxSize(450,700));
    }
    m_pWEditLanguage->LanguageSwitch();
    m_pWEditLanguage->wxDialog::ShowModal();
}

void CEditorMainFrame::GetSplineDialog()
{
    if (m_pSplineDialog == NULL)
    {
        m_pSplineDialog = new CSplineDialog(this, wxID_ANY, wxT("Edit UI"), wxDefaultPosition, MAINFRAMENORMALSIZE);
    }
    SetChildWindowStyle(m_pSplineDialog);
    m_pSplineDialog->ShowModal();
}

void CEditorMainFrame::GetPerformanceDialog()
{
    if (m_pPerformanceDialog == NULL)
    {
        m_pPerformanceDialog = new CEditPerformanceDialog(this, wxID_ANY, wxT("Performance"), wxPoint(100,30), wxSize(1000,700), wxDEFAULT_DIALOG_STYLE ^ wxMINIMIZE_BOX);
    }
    m_pPerformanceDialog->LanguageSwitch();
    m_pPerformanceDialog->Show();
}

void CEditorMainFrame::OnCommandEvent(wxCommandEvent& event)
{
    int nID = event.GetId();
    switch (nID)
    {
    case Menu_File_OpenProject:
        OpenProject();
        break;
    case Menu_File_CloseProject:
        CloseProject();
        break;
    case Menu_File_SaveProject:
        SaveProject();
        break;
    case Menu_File_Export:
        Export();
        break;
    case Menu_Window_Check_Scene:
    case Menu_Window_Check_UI:
    case Menu_Window_Check_Ani:
    case Menu_Window_Check_Terrain:
    case Menu_Window_Check_Effect:
    case Menu_Window_Check_Game:
        ToggleViewButton();
        break;
    case Menu_Open_TexturePreview:
        GetTexturePreviewDialog();
        break;
    case Menu_Edit_Language:
        GetEditLanguageDialog();
        break;
    case Menu_Edit_SkeletonAnimation:
    case ID_SkeletonAnimationEditBtn:
        {
            if (m_pSkeletonAnimationDialog == NULL)
            {
                m_pSkeletonAnimationDialog = new CEditAnimationDialog(this, wxID_ANY, wxT("Edit Animation"), wxDefaultPosition, MAINFRAMENORMALSIZE);
            }
            SetChildWindowStyle(m_pSkeletonAnimationDialog);
            m_pSkeletonAnimationDialog->ShowModal();
        }
        break;
    case ID_TB_PerformBtn:
        ShowPerformDlg();
        break;
    case Menu_Help_About:
        ShowAboutDlg();
        break;
    case Menu_Help_Language_Chinese:
        CEditorConfig::GetInstance()->SetCurrLanguage(eLT_Chinese);
        LanguageSwitch(eLT_Chinese);
        break;
    case Menu_Help_Language_English:
        CEditorConfig::GetInstance()->SetCurrLanguage(eLT_English);
        LanguageSwitch(eLT_English);
        break;
    case eFLMS_FileTree_AddFile:
        AddComponentFile();
        break;
    case eFLMS_FileTree_AddFileFolder:
        AddComponentFileFolder();
        break;
    case eFLMS_FileTree_Delete:
        DeleteComponentFile();
        break;
    case eFLMS_FileTree_OpenFileDirectory:
        OpenComponentFileDirectory();
        break;
    case eFLMS_FileTree_SetStartFile:
        {
            wxTreeItemId item = m_pComponentFileTreeCtrl->GetSelection();
            CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(item));
            CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
            const TString& strFileName = pCurItemData->GetFileName();
            size_t uFileId = pProject->GetComponentFileId(strFileName);
            pProject->SetStartFile(uFileId);
            OnSetStartFile(item);
        }
        break;
    case ID_TB_Reopen:
        {
            TString strLastOpenProject = CEditorConfig::GetInstance()->GetLastOpenProject();
            if (strLastOpenProject.length() > 0)
            {
                TString strFullPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
                strLastOpenProject = CFilePathTool::GetInstance()->MakeAbsolute(strFullPath.c_str(), strLastOpenProject.c_str());
                OpenProjectFile(strLastOpenProject.c_str());
                TString strLastOpenFile = CEditorConfig::GetInstance()->GetLastOpenFile();
                if (strLastOpenFile.length() > 0)
                {
                    strLastOpenFile = CFilePathTool::GetInstance()->MakeAbsolute(strFullPath.c_str(), strLastOpenFile.c_str());
                    OpenComponentFile(strLastOpenFile.c_str());
                }
                m_pToolBar->EnableTool(ID_TB_Reopen, false);
            }
        }
        break;
    case ID_ViewButton_Scene:
    case ID_ViewButton_UI:
    case ID_ViewButton_Ani:
    case ID_ViewButton_Terrain:
    case ID_ViewButton_Effect:
    case ID_ViewButton_Game:
        // TODO: Temp hide effect window.
        if (ID_ViewButton_Effect == nID)
        {
            nID = ID_ViewButton_Game;
        }
        SelectViewID(nID);
        m_pComponentRenderWindow->UpdateAllDependencyLine();
        break;
    case ID_CameraBtn:
        {
            wxPoint pos = m_pToolBar->GetScreenPosition();
            wxRect rect = m_pToolBar->GetToolRect(ID_CameraBtn);
            pos.x += rect.x + rect.width;
            pos.y += rect.y + rect.height;
            m_pCameraWnd->SetPosition(pos);
            m_pCameraWnd->Show(!m_pCameraWnd->IsShown());
        }
        break;
    case ID_ViewAllBtn:
        {
            bool bViewAll = m_pToolBar->GetToolToggled(ID_ViewAllBtn);
            m_pViewScreen->SendSizeEvent(); // In order to call SetFBOViewPort, so force call a on size callback.
            m_pViewScreen->GetRenderWindow()->UseFBO(bViewAll);
        }
        break;
    case ID_CaptureBtn:
        {
            static const TCHAR* pszFilePath = _T("C:/TestPng.png");
            if(wxIsCtrlDown())
            {
                auto fontFaceMap = CFontManager::GetInstance()->GetFontFaceMap();
                for (auto iter = fontFaceMap.begin(); iter != fontFaceMap.end(); ++iter)
                {
                    iter->second->SaveToTextureFile(pszFilePath);
                }
                CComponentGraphic_GL::GetFontFace()->SaveToTextureFile(pszFilePath);
            }
            else
            {
                bool bCapture = m_pToolBar->GetToolToggled(ID_ViewAllBtn);
                if (bCapture)
                {
                    bool bRet = CRenderManager::GetInstance()->SaveTextureToFile(CRenderManager::GetInstance()->GetMainFBOTexture(), pszFilePath);
                    if (bRet)
                    {
                        TCHAR szInfo[MAX_PATH];
                        _stprintf(szInfo, _T("View Picture %s now?"), pszFilePath);
                        if (wxMessageBox(szInfo, _T("Save successfully"), wxYES_NO) == wxYES)
                        {
                            SHELLEXECUTEINFO ShellInfo;
                            memset(&ShellInfo, 0, sizeof(ShellInfo));
                            ShellInfo.cbSize = sizeof(ShellInfo);
                            ShellInfo.hwnd = NULL;
                            ShellInfo.lpVerb = _T("open");
                            ShellInfo.lpFile = _T("C:/TestPng.png");
                            ShellInfo.nShow = SW_SHOWNORMAL;
                            ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

                            ShellExecuteEx(&ShellInfo);
                        }
                    }
                }
                else
                {
                    wxMessageBox(_T("Capture is available only in ViewAll mode."), _T("Not Available"), wxOK);
                }
            }
        }
        break;
    case ID_SettingBtn:
        {
            wxPoint pos = m_pToolBar->GetScreenPosition();
            wxRect rect = m_pToolBar->GetToolRect(ID_SettingBtn);
            pos.x += rect.x + rect.width;
            pos.y += rect.y + rect.height;
            m_pSceneGridWnd->SetPosition(pos);
            m_pSceneGridWnd->Show(!m_pSceneGridWnd->IsShown());
        }
        break;
    case ID_ComponentInfoBtn:
        {
            wxPoint pos = m_pToolBar->GetScreenPosition();
            wxRect rect = m_pToolBar->GetToolRect(ID_ComponentInfoBtn);
            pos.x += rect.x + rect.width;
            pos.y += rect.y + rect.height;
            m_pComponentInfoWnd->SetPosition(pos);
            m_pComponentInfoWnd->Show(!m_pComponentInfoWnd->IsShown());
        }
        break;
    case ID_BatchEditBtn:
        {
            wxPoint pos = m_pToolBar->GetScreenPosition();
            wxRect rect = m_pToolBar->GetToolRect(ID_BatchEditBtn);
            pos.x += rect.x + rect.width;
            pos.y += rect.y + rect.height;
            m_pBatchEditWnd->SetPosition(pos);
            m_pBatchEditWnd->Show(!m_pBatchEditWnd->IsShown());
        }
        break;
    default:
        GetViewAgent(m_nCurrentViewID)->OnCommandEvent(event);
        break;
    }
    event.Skip();
}

void CEditorMainFrame::CreateSplitter()
{
    m_pSplitter = new wxSplitterWindow(m_pCenterBook);
    m_pViewScreen = new CEditorSceneWindow(m_pSplitter);
    wxGLContext* pContext = static_cast<CEngineEditor*>(wxApp::GetInstance())->CreateGLContext(m_pViewScreen);
    m_pViewScreen->SetGLContext(pContext);
    m_pComponentRenderWindow = new CBeyondEngineEditorComponentWindow(m_pSplitter, pContext);
    m_pSplitter->SplitHorizontally(m_pViewScreen, m_pComponentRenderWindow, 500);
    m_pSplitter->SetMinimumPaneSize(MINWINDOWSIZE);
    //TODO: Don't change the order, or the rendering is wrong. Don't know why
    RegisterUpdateWindow(m_pComponentRenderWindow);
    RegisterUpdateWindow(m_pViewScreen);
}

void CEditorMainFrame::OnActivateComponentFile( wxTreeEvent& event )
{
    wxTreeItemId itemId = event.GetItem();
    CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTreeCtrl->GetItemData(itemId);
    const TString& curViewFilePath = CComponentProxyManager::GetInstance()->GetCurrentViewFilePath();
    BEATS_ASSERT(pData != NULL)
    if (pData->IsDirectory())
    {
        m_pComponentFileTreeCtrl->Toggle(itemId);
    }
    else if (_tcsicmp(pData->GetFileName().c_str(), curViewFilePath.c_str()) != 0)
    {
        SelectComponent(NULL);
        const TCHAR* pszFileName = pData->GetFileName().c_str();
        OpenComponentFile(pszFileName, &itemId);
        CEditorConfig::GetInstance()->SetLastOpenFile(pszFileName);
    }
}

void CEditorMainFrame::OnActivateComponentTemplate( wxTreeEvent& event )
{
    wxTreeItemId itemId = event.GetItem();
    CComponentTreeItemData* pData = (CComponentTreeItemData*)m_pComponentTemplateTreeCtrl->GetItemData(itemId);
    BEATS_ASSERT(pData != NULL);
    if (pData->IsDirectory())
    {
        m_pComponentTemplateTreeCtrl->Toggle(itemId);
    }
}

void CEditorMainFrame::OnComponentFileStartDrag( wxTreeEvent& event )
{
    CComponentFileTreeItemData* pData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(event.GetItem()));
    if (pData != NULL)
    {
        m_pComponentFileTreeCtrl->SelectItem(event.GetItem());
        event.Allow();
    }
}

void CEditorMainFrame::OnComponentFileEndDrag( wxTreeEvent& event )
{
    wxTreeItemId currentItemId = event.GetItem();
    wxTreeItemId draggingItemId = m_pComponentFileTreeCtrl->GetSelection();
    if (currentItemId.IsOk())
    {
        wxTreeItemId targetItemId = event.GetItem();
        BEATS_ASSERT(draggingItemId.IsOk() && targetItemId.IsOk());
        if (draggingItemId != targetItemId)
        {
            bool bPressAlt = wxGetKeyState(WXK_ALT);
            bool bNeedRebuildTree = true;
            if (wxMessageBox(_T("要修改目录结构吗？"), _T("移动"), wxYES_NO) == wxYES)
            {
                CComponentFileTreeItemData* pDraggingItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(draggingItemId));
                CComponentFileTreeItemData* pTargetItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(targetItemId));
                CComponentProjectDirectory* pTargetDirectory = pTargetItemData->GetProjectDirectory();
                bool bMovingInParentDirectory = false;
                if (pTargetDirectory == NULL)
                {
                    BEATS_ASSERT(!pTargetItemData->IsDirectory());
                    wxTreeItemId targetItemParentId = m_pComponentFileTreeCtrl->GetItemParent(targetItemId);
                    CComponentFileTreeItemData* pTargetItemParentData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(targetItemParentId));
                    pTargetDirectory = pTargetItemParentData->GetProjectDirectory();
                    BEATS_ASSERT(pTargetDirectory != NULL);
                    bMovingInParentDirectory = true;
                }

                if (pDraggingItemData->IsDirectory())
                {
                    CComponentProjectDirectory* pDraggingDirectory = pDraggingItemData->GetProjectDirectory();
                    BEATS_ASSERT(pDraggingDirectory->GetParent() != NULL);
                    bool bTargetIsPrevious = bPressAlt && !bMovingInParentDirectory;
                    CComponentProjectDirectory* pPreviousDirectory = NULL;
                    CComponentProjectDirectory* pNewTargetDirectory = pTargetDirectory;
                    if (bTargetIsPrevious)
                    {
                        pNewTargetDirectory = pTargetDirectory->GetParent();
                        pPreviousDirectory = pTargetDirectory;
                    }
                    bool bMoveParentToSon = pNewTargetDirectory->GenerateLogicPath().find(pDraggingDirectory->GenerateLogicPath()) != -1;
                    if (!bMoveParentToSon)
                    {
                        bool bRemoveDirectory = pDraggingDirectory->GetParent()->RemoveDirectory(pDraggingDirectory);
                        BEYONDENGINE_UNUSED_PARAM(bRemoveDirectory);
                        BEATS_ASSERT(bRemoveDirectory);
                        pNewTargetDirectory->InsertDirectory(pDraggingDirectory, pPreviousDirectory);
                    }
                    else
                    {
                        bNeedRebuildTree = false;
                        wxMessageBox(_T("Move parent directory to son is not supported!"), _T("Operation invalid"), wxOK);
                    }
                }
                else
                {
                    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
                    wxTreeItemId draggingItemParentId = m_pComponentFileTreeCtrl->GetItemParent(draggingItemId);
                    CComponentFileTreeItemData* pDraggingItemParentData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(draggingItemParentId));
                    BEATS_ASSERT(pDraggingItemParentData->IsDirectory());
                    size_t uFileId = pProject->GetComponentFileId(pDraggingItemData->GetFileName());
                    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
                    bool bRemoveFile = pDraggingItemParentData->GetProjectDirectory()->RemoveFile(uFileId);
                    BEYONDENGINE_UNUSED_PARAM(bRemoveFile);
                    BEATS_ASSERT(bRemoveFile, _T("Remove file %d failed!"), uFileId);
                    size_t uPreviousFileId = 0xFFFFFFFF;
                    if (bMovingInParentDirectory)
                    {
                        pProject->GetComponentFileId(pTargetItemData->GetFileName());
                    }
                    pTargetDirectory->InsertFile(uFileId, uPreviousFileId);
                }
                if (bNeedRebuildTree)
                {
                    m_componentFileListMap.clear();
                    m_pComponentFileTreeCtrl->DeleteAllItems();
                    m_pComponentFileTreeCtrl->AddRoot(wxT("Root"), eTCIT_Folder, -1, new CComponentFileTreeItemData(NULL, _T("")));
                    InitializeComponentFileTree(CComponentProxyManager::GetInstance()->GetProject()->GetRootDirectory(), m_pComponentFileTreeCtrl->GetRootItem());
                    m_pComponentFileTreeCtrl->ExpandAll();
                    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
                    // Refresh the start directory flag, because the logic path may be impacted.
                    size_t uStartFileId = pProject->GetStartFile();
                    const TString& strStartFile = pProject->GetComponentFileName(uStartFileId);
                    BEATS_ASSERT(m_componentFileListMap.find(strStartFile) != m_componentFileListMap.end());
                    wxTreeItemId item = m_componentFileListMap[strStartFile];
                    BEATS_ASSERT(item.IsOk());
                    OnSetStartFile(item);
                }
            }
        }
    }
}

void CEditorMainFrame::OnComponentStartDrag( wxTreeEvent& event )
{
    if (m_activeFileItemId)
    {
        m_pComponentTemplateTreeCtrl->SelectItem(event.GetItem());
        CComponentTreeItemData* pData = static_cast<CComponentTreeItemData*>(m_pComponentTemplateTreeCtrl->GetItemData(event.GetItem()));
        if (pData != NULL && !pData->IsDirectory())
        {
            m_pComponentRenderWindow->SetDraggingComponent(static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentTemplate(pData->GetGUID())));
            event.Allow();
            BEATS_PRINT(_T("StartDrag at Pos: %d %d\n"), event.GetPoint().x, event.GetPoint().y);
        }
    }
}

void CEditorMainFrame::OnComponentEndDrag( wxTreeEvent& event )
{
    if (m_pComponentRenderWindow->IsMouseInWindow())
    {
        CComponentProxy* pDraggingComponent = m_pComponentRenderWindow->GetDraggingComponent();
        CComponentProxy* pNewComponent = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->CreateComponent(pDraggingComponent->GetGuid(), true));
        int x = 0;
        int y = 0;
        pDraggingComponent->GetGraphics()->GetPosition(&x, &y);
        pNewComponent->GetGraphics()->SetPosition(x, y);
        SelectComponent(pNewComponent);
        UpdateComponentInstanceTreeCtrl();
        CComponentProxyManager::GetInstance()->OnCreateComponentInScene(pNewComponent);
    }
    m_pComponentRenderWindow->SetDraggingComponent(NULL);
    event;
    BEATS_PRINT(_T("EndDrag at Pos: %d %d\n"), event.GetPoint().x, event.GetPoint().y);
}

void CEditorMainFrame::UpdatePropertyGrid()
{
    m_pPropGridManager->ClearPage(0);
    if (GetSelectedComponent() != NULL)
    {
        //SelectComponent(GetSelectedComponent()); //it will collapse all
        m_pPropGridManager->InsertComponentsInPropertyGrid(GetSelectedComponent());//it will expand all
    }
}

void CEditorMainFrame::SelectComponent(CComponentProxy* pComponentInstance)
{
    m_pPropGridManager->ClearPage(0);
    GetViewAgent(m_nCurrentViewID)->SelectComponent(pComponentInstance);
    m_pSelectedComponentProxy = pComponentInstance;
    if (pComponentInstance != NULL)
    {
        // If we select an instance(not a template).
        if (pComponentInstance->GetId() != -1)
        {
            m_pComponentFileTreeCtrl->Unselect();

            int x = 0;
            int y = 0;
            pComponentInstance->GetGraphics()->GetPosition(&x, &y);
            kmVec2 worldPos;
            m_pComponentRenderWindow->ConvertGridPosToWorldPos(x, y, &worldPos.x, &worldPos.y);

            CCamera* pComponentWndCamera = m_pComponentRenderWindow->GetCamera();
            const kmVec3& viewPos = pComponentWndCamera->GetViewPos();
            kmVec2 offset = pComponentWndCamera->GetCenterOffset();
            bool bOutOfHorizontal = worldPos.x < -viewPos.x - fabs(offset.x) || worldPos.x > -viewPos.x + fabs(offset.x);
            bool bOutOfVertical = worldPos.y < -viewPos.y - fabs(offset.y) || worldPos.y > -viewPos.y + fabs(offset.y);
            if (bOutOfHorizontal || bOutOfVertical)
            {
                pComponentWndCamera->SetViewPos(-worldPos.x, -worldPos.y, 0);
            }
        }
        m_pPropGridManager->InsertComponentsInPropertyGrid(pComponentInstance);
        m_pPropGridManager->ExpandAll(false);
    }
}

CComponentProxy* CEditorMainFrame::GetSelectedComponent()
{
    return m_pSelectedComponentProxy;
}

void CEditorMainFrame::ResolveIdConflict(const std::map<size_t, std::vector<size_t>>& conflictIdMap)
{
    if (conflictIdMap.size() > 0)
    {
        TCHAR szConflictInfo[10240];
        _stprintf(szConflictInfo, _T("工程中有%d个ID发生冲突，点击\"是\"开始解决,点击\"否\"退出."), conflictIdMap.size());
        int iRet = wxMessageBox(szConflictInfo, _T("解决ID冲突"), wxYES_NO);
        if (iRet == wxYES)
        {
            CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
            size_t lAnswer = 0;
            std::map<size_t, std::vector<size_t>>::const_iterator iter = conflictIdMap.begin();
            for (; iter != conflictIdMap.end(); ++iter)
            {
                _stprintf(szConflictInfo, _T("Id为%d的组件出现于文件:\n"), iter->first);
                for (size_t i = 0; i < iter->second.size(); ++i)
                {
                    TCHAR szFileName[1024];
                    _stprintf(szFileName, _T("%d.%s\n"), i, pProject->GetComponentFileName(iter->second[i]).c_str());
                    _tcscat(szConflictInfo, szFileName);
                }
                _tcscat(szConflictInfo, _T("需要保留ID的文件序号是(填-1表示全部分配新ID):"));
                lAnswer = wxGetNumberFromUser(szConflictInfo, _T("promote"), _T("请填入序号"), lAnswer, -1, iter->second.size() - 1);
                for (size_t i = 0; i < iter->second.size(); ++i)
                {
                    pProject->ResolveIdForFile(iter->second[i], iter->first, i == lAnswer);
                }
            }
        }
        else
        {
            _exit(0);
        }
    }
}

void CEditorMainFrame::OpenProjectFile( const TCHAR* pPath )
{
    if (pPath != NULL && _tcslen(pPath) > 0)
    {
        CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
        std::map<size_t, std::vector<size_t>> conflictIdMap;
        CComponentProjectDirectory* pProjectData = pProject->LoadProject(pPath, conflictIdMap);
        bool bEmptyProject = pProjectData == NULL;
        if (!bEmptyProject)
        {
            CEditorConfig::GetInstance()->SetLastOpenProject(pPath);
            CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTreeCtrl->GetItemData(m_pComponentFileTreeCtrl->GetRootItem());
            if (pData)
            {
                BEATS_SAFE_DELETE(pData)
            }
            InitializeComponentFileTree(pProjectData, m_pComponentFileTreeCtrl->GetRootItem());
            size_t uStartFileId = pProject->GetStartFile();
            const TString& strStartFileName = pProject->GetComponentFileName(uStartFileId);
            BEATS_ASSERT(m_componentFileListMap.find(strStartFileName) != m_componentFileListMap.end());
            wxTreeItemId item = m_componentFileListMap[strStartFileName];
            OnSetStartFile(item);
            ResolveIdConflict(conflictIdMap);
        }
        m_pComponentFileTreeCtrl->ExpandAll();
    }
}

void CEditorMainFrame::CloseProjectFile()
{
    SelectComponent(NULL);
    if (CComponentProxyManager::GetInstance()->GetCurrentWorkingFilePath().length() > 0)
    {
        CloseComponentFile();
    }
    CComponentProxyManager::GetInstance()->GetProject()->CloseProject();
    wxTreeItemId rootItem = m_pComponentFileTreeCtrl->GetRootItem();
    m_pComponentFileTreeCtrl->DeleteChildren(rootItem);
    m_pComponentFileTreeCtrl->SetItemImage(rootItem, eTCIT_Folder);
    m_componentFileListMap.clear();
    m_activeFileItemId = NULL;
    RefreshLanguage();
}

void CEditorMainFrame::Export()
{
    SaveProject();
    TString szBinaryPath;
    CUtilityManager::GetInstance()->AcquireSingleFilePath(false, NULL, szBinaryPath, _T("选择要导出的文件"), BINARIZE_FILE_EXTENSION_FILTER, NULL);
    if (szBinaryPath.length() > 0)
    {
        std::vector<TString> filesToExport;
        for (std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.begin(); iter != m_componentFileListMap.end(); ++iter)
        {
            CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTreeCtrl->GetItemData(iter->second);
            BEATS_ASSERT(pData != NULL && !pData->IsDirectory());
            TString filePath(pData->GetFileName());
            filesToExport.push_back(filePath);
        }
        TString strExtensionStr = CFilePathTool::GetInstance()->Extension(szBinaryPath.c_str());
        if (strExtensionStr.compare(BINARIZE_FILE_EXTENSION) != 0)
        {
            szBinaryPath.append(BINARIZE_FILE_EXTENSION);
        }
        CComponentProxyManager::GetInstance()->Export(szBinaryPath.c_str());
        //Export function will cause these operation: open->export->close->change file->open->...->restore open the origin file.
        //So we will update the dependency line as if we have just open a new file.
        m_pComponentRenderWindow->UpdateAllDependencyLine();

        wxMessageBox(wxT("导出完毕!"));
    }
}

void CEditorMainFrame::SaveProject()
{
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    if (pProject->GetRootDirectory() != NULL)
    {
        TString szSavePath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
        szSavePath.append(_T("\\")).append(GAME_COMPONENT_INFO_PATCH);
        CComponentProxyManager::GetInstance()->SaveTemplate(szSavePath.c_str());
        // Save Instance File
        CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
        TString strProjectFullPath = pProject->GetProjectFilePath();
        if (strProjectFullPath.length() == 0)
        {
            CUtilityManager::GetInstance()->AcquireSingleFilePath(true, NULL, strProjectFullPath, _T("保存文件"), COMPONENT_PROJECT_EXTENSION, NULL);
        }
        else
        {
            strProjectFullPath.append(_T("/")).append(pProject->GetProjectFileName());
        }
        if (strProjectFullPath.length() > 0)
        {
            FILE* pProjectFile = _tfopen(strProjectFullPath.c_str(), _T("w+"));
            BEATS_ASSERT(pProjectFile != NULL);
            if (pProjectFile)
            {
                fclose(pProjectFile);
                CComponentProxyManager::GetInstance()->GetProject()->SaveProject();
                CComponentProxyManager::GetInstance()->SaveCurFile();
            }
        }
    }
}

void CEditorMainFrame::OpenComponentFile( const TCHAR* pFilePath, wxTreeItemId* pItemId /*= NULL*/ )
{
    BEATS_ASSERT(pFilePath != NULL && pFilePath[0] != 0, _T("Invalid file path"));
    if (!m_Manager.GetPane(m_pResourcePanel).IsShown())
    {
        m_Manager.GetPane(m_pResourcePanel).Show();
        m_Manager.Update();
    }
    CComponentProxyManager::GetInstance()->OpenFile(pFilePath);
    size_t uPageCount = m_pLeftBook->GetPageCount();
    int iComponentListPageIndex = -1;
    for (size_t i = 0; i < uPageCount; ++i)
    {
        if (m_pLeftBook->GetPage(i) == m_pComponentInstanceTreeCtrl)
        {
            iComponentListPageIndex = i;
            break;
        }
    }
    if (iComponentListPageIndex == -1)
    {
        m_pLeftBook->InsertPage(1, m_pComponentInstanceTreeCtrl, L10N_T(eL_ComponentInstance), true);
    }
    UpdateComponentInstanceTreeCtrl();
    GetViewAgent(m_nCurrentViewID)->OnOpenComponentFile();
    m_pComponentRenderWindow->UpdateAllDependencyLine();
    if (pItemId == NULL)
    {
        auto iter = m_componentFileListMap.find(pFilePath);
        BEATS_ASSERT(iter != m_componentFileListMap.end());
        if ((iter != m_componentFileListMap.end()))
        {
            pItemId = &iter->second;
        }
    }
    BEATS_ASSERT(pItemId != NULL, _T("Can't find the file %s we are trying to open!"), pFilePath);
    if (pItemId != NULL)
    {
        m_itemTextcolor = m_pComponentFileTreeCtrl->GetItemTextColour(*pItemId);
        if (m_activeFileItemId)
        {
            m_pComponentFileTreeCtrl->SetItemBackgroundColour(m_activeFileItemId, m_itemTextcolor);
        }
        m_pComponentFileTreeCtrl->SetItemBackgroundColour(*pItemId, wxColor(150, 33, 184));
        m_pCenterBook->SetPageText(0, m_pComponentFileTreeCtrl->GetItemText(*pItemId));
        m_activeFileItemId = *pItemId;
    }
}

void CEditorMainFrame::CloseComponentFile(bool bRemindSave /*= true*/)
{
    CComponentProxyManager* pComponentProxyManager = CComponentProxyManager::GetInstance();
    const TString& strCurWorkingFile = pComponentProxyManager->GetCurrentWorkingFilePath();
    if (strCurWorkingFile.length() > 0)
    {
        if (bRemindSave)
        {
            int iRet = wxMessageBox(wxString::Format(_T("是否要保存当前文件?\n%s"), strCurWorkingFile.c_str()), _T("保存文件"), wxYES_NO);
            if (iRet == wxYES)
            {
                pComponentProxyManager->SaveCurFile();
            }
        }
        pComponentProxyManager->CloseFile(strCurWorkingFile.c_str());
    }
    HideTreeCtrl(m_pComponentInstanceTreeCtrl);
    GetViewAgent(m_nCurrentViewID)->OnCloseComponentFile();
}

void CEditorMainFrame::OpenProject()
{
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    TString strProjectFullPath = pProject->GetProjectFilePath();

    if (strProjectFullPath.length() == 0)
    {
        TString strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
        CUtilityManager::GetInstance()->AcquireSingleFilePath(false, this->GetHWND(), strProjectFullPath, _T("选择要读取的文件"), COMPONENT_PROJECT_EXTENSION, strWorkingPath.c_str());
        if (strProjectFullPath.length() > 0)
        {
            OpenProjectFile(strProjectFullPath.c_str());
        }
    }
    else
    {
        CloseProjectFile();
    }
}

void CEditorMainFrame::CloseProject()
{
    CloseProjectFile();
    m_Manager.GetPane(m_pResourcePanel).Hide();
    m_Manager.Update();
}

void CEditorMainFrame::ActivateFile(const TCHAR* pszFileName)
{
    std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.find(pszFileName);
    if (iter != m_componentFileListMap.end())
    {
        m_pComponentFileTreeCtrl->SelectItem(iter->second);
        wxTreeEvent simulateEvent;
        simulateEvent.SetItem(iter->second);
        OnActivateComponentFile(simulateEvent);
    }
}

void CEditorMainFrame::OnTreeCtrlRightClick( wxTreeEvent& event )
{
    int id = event.GetId();
    switch (id)
    {
    case Ctrl_Tree_CompontentFile :
        m_pComponentFileTreeCtrl->SelectItem(event.GetItem());
        if (CComponentProxyManager::GetInstance()->GetProject()->GetRootDirectory() != NULL)
        {
            CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)(m_pComponentFileTreeCtrl->GetItemData(event.GetItem()));
            BEATS_ASSERT(pData);
            m_pComponentFileMenu->Enable(eFLMS_FileTree_AddFile, pData->IsDirectory());
            m_pComponentFileMenu->Enable(eFLMS_FileTree_AddFileFolder, pData->IsDirectory());
            m_pComponentFileMenu->Enable(eFLMS_FileTree_OpenFileDirectory, !pData->IsDirectory());
            m_pComponentFileMenu->Enable(eFLMS_FileTree_SetStartFile, !pData->IsDirectory());
            PopupMenu(m_pComponentFileMenu);
        }
        break;
    default:
        GetViewAgent(m_nCurrentViewID)->OnTreeCtrlRightClick(event);
        break;
    }
}

void CEditorMainFrame::AddComponentFile()
{
    wxTreeItemId item = m_pComponentFileTreeCtrl->GetSelection();
    if (item.IsOk())
    {
        TString result;
        CUtilityManager::GetInstance()->AcquireSingleFilePath(true, NULL, result, _T("添加文件"), COMPONENT_FILE_EXTENSION_FILTER, NULL);
        if (result.length() > 0)
        {
            std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.find(result);
            if (iter != m_componentFileListMap.end())
            {
                int iRet = wxMessageBox(_T("该文件已经存在于项目之中！不能重复添加！是否跳转到该文件？"), _T("文件已存在"), wxYES_NO);
                if (iRet == wxYES)
                {
                    m_pComponentFileTreeCtrl->SelectItem(iter->second);
                }
            }
            else
            {
                if (!CFilePathTool::GetInstance()->Exists(result.c_str()))
                {
                    TString strExtensionStr = CFilePathTool::GetInstance()->Extension(result.c_str());
                    if (strExtensionStr.compare(COMPONENT_FILE_EXTENSION) != 0)
                    {
                        result.append(COMPONENT_FILE_EXTENSION);
                    }

                    TiXmlDocument document;
                    TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","","");
                    document.LinkEndChild(pDeclaration);
                    TiXmlElement* pRootElement = new TiXmlElement("Root");
                    document.LinkEndChild(pRootElement);
                    // TCHAR to char trick.
                    wxString pathTCHAR(result.c_str());
                    document.SaveFile(pathTCHAR);
                }
                CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(NULL, result);
                wxString fileName = wxFileNameFromPath(result.c_str());
                wxTreeItemId newItemId = m_pComponentFileTreeCtrl->AppendItem(item, fileName, CEnginePropertyGirdManager::eTCIT_File, -1, pData);
                m_pComponentFileTreeCtrl->Expand(item);
                m_componentFileListMap[result] = newItemId;

                // When Add a new file in project, we must validate the id in this file. if id is conflicted, we have to resolve it.
                CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(item));
                BEATS_ASSERT(pCurItemData != NULL && pCurItemData->IsDirectory());
                std::map<size_t, std::vector<size_t>> conflictMap;
                pCurItemData->GetProjectDirectory()->AddFile(result, conflictMap);
                ResolveIdConflict(conflictMap);
                ActivateFile(result.c_str());
                CComponentProxyManager::GetInstance()->GetProject()->SaveProject();
            }
        }
    }
}

void CEditorMainFrame::AddComponentFileFolder()
{
    wxTreeItemId item = m_pComponentFileTreeCtrl->GetSelection();
    TString strNewName = wxGetTextFromUser(_T("文件夹名"), _T("添加文件夹"), _T("New File Folder Name"));
    if (strNewName.length() > 0)
    {
        CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(item));
        BEATS_ASSERT(pCurItemData != NULL && pCurItemData->IsDirectory());
        CComponentProjectDirectory* pNewProjectData = pCurItemData->GetProjectDirectory()->AddDirectory(strNewName);
        if (pNewProjectData != NULL)
        {
            CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(pNewProjectData, strNewName);
            m_pComponentFileTreeCtrl->AppendItem(item, strNewName, CEnginePropertyGirdManager::eTCIT_Folder, -1, pData);
            m_pComponentFileTreeCtrl->Expand(item);
        }
        else
        {
            wxMessageBox(_T("The name has already exist!"), _T("Error"), wxOK);
        }
    }
}

void CEditorMainFrame::DeleteComponentFile()
{
    wxTreeItemId item = m_pComponentFileTreeCtrl->GetSelection();
    int iRet = wxMessageBox(_T("是否要删除物理文件？"), _T("删除文件"), wxYES_NO);
    DeleteItemInComponentFileList(item, iRet == wxYES);
    SelectComponent(NULL);
    wxTreeItemId parentItem = m_pComponentFileTreeCtrl->GetItemParent(item);
    CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(item));
    CComponentFileTreeItemData* pParentItemData = NULL;
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    if (parentItem.IsOk())
    {
        pParentItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(parentItem));
    }
    bool bIsRootDirectory = pParentItemData == NULL;
    if (!bIsRootDirectory)
    {
        BEATS_ASSERT(item != m_pComponentFileTreeCtrl->GetRootItem(), _T("Only root doesn't have a parent!"));
        if (pCurItemData->IsDirectory())
        {
            bool bRet = pParentItemData->GetProjectDirectory()->DeleteDirectory(pCurItemData->GetProjectDirectory());
            bRet;
            BEATS_ASSERT(bRet, _T("Delete directory %s failed!"), pCurItemData->GetFileName());
        }
        else
        {
            size_t uFileId = pProject->GetComponentFileId(pCurItemData->GetFileName());
            BEATS_ASSERT(uFileId != 0xFFFFFFFF, _T("Can't find file %s at project"), pCurItemData->GetProjectDirectory()->GetName().c_str());
            bool bRet = pParentItemData->GetProjectDirectory()->DeleteFile(uFileId);
            bRet;
            BEATS_ASSERT(bRet, _T("Delete file %s failed!"), pCurItemData->GetFileName().c_str());
        }
        m_pComponentFileTreeCtrl->Delete(item);
    }
    else
    {
        BEATS_ASSERT(item == m_pComponentFileTreeCtrl->GetRootItem(), _T("Only root doesn't have a parent!"));
        const TString& strCurWorkingFile = CComponentProxyManager::GetInstance()->GetCurrentWorkingFilePath();
        CComponentProxyManager::GetInstance()->CloseFile(strCurWorkingFile.c_str());
        pProject->GetRootDirectory()->DeleteAll(true);
        m_pComponentFileTreeCtrl->DeleteChildren(item);// Never delete the root item.
    }
}

void CEditorMainFrame::OpenComponentFileDirectory()
{
    wxTreeItemId item = m_pComponentFileTreeCtrl->GetSelection();
    TString path = CComponentProxyManager::GetInstance()->GetProject()->GetProjectFilePath();
    CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(item));
    if (m_pComponentFileTreeCtrl->GetRootItem() != item)
    {
        BEATS_ASSERT(!pCurItemData->IsDirectory(), _T("eFLMS_OpenFileDirectory command can only use on file!"));
        if (!pCurItemData->IsDirectory())
        {
            path = CFilePathTool::GetInstance()->ParentPath(pCurItemData->GetFileName().c_str());
            SHELLEXECUTEINFO ShellInfo;
            memset(&ShellInfo, 0, sizeof(ShellInfo));
            ShellInfo.cbSize = sizeof(ShellInfo);
            ShellInfo.hwnd = NULL;
            ShellInfo.lpVerb = _T("explore");
            ShellInfo.lpFile = path.c_str();
            ShellInfo.nShow = SW_SHOWNORMAL;
            ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

            ShellExecuteEx(&ShellInfo);
        }
    }
}

void CEditorMainFrame::DeleteItemInComponentFileList( wxTreeItemId itemId, bool bDeletePhysicalFile )
{
    BEATS_ASSERT(itemId.IsOk());
    CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTreeCtrl->GetItemData(itemId);
    BEATS_ASSERT(pData != NULL);
    if (pData->IsDirectory())
    {
        wxTreeItemIdValue idValue;
        wxTreeItemId childId = m_pComponentFileTreeCtrl->GetFirstChild(itemId, idValue);
        while (childId.IsOk())
        {
            DeleteItemInComponentFileList(childId, bDeletePhysicalFile);
            childId = m_pComponentFileTreeCtrl->GetNextChild(itemId, idValue);
        }
    }
    else
    {
        TString ItemNameStr(pData->GetFileName());
        std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.find(ItemNameStr);
        BEATS_ASSERT(iter != m_componentFileListMap.end());
        m_componentFileListMap.erase(iter);
        if (bDeletePhysicalFile)
        {
            ::DeleteFile(pData->GetFileName().c_str());
        }
    }
}

void CEditorMainFrame::OnSelectComponentTemplateTreeItem( wxTreeEvent& /*event*/ )
{
    wxTreeItemId id = m_pComponentTemplateTreeCtrl->GetSelection();
    if (id.IsOk())
    {
        CComponentTreeItemData* pData = static_cast<CComponentTreeItemData*>(m_pComponentTemplateTreeCtrl->GetItemData(id));
        CComponentProxy* pComponent = NULL;
        BEATS_ASSERT(pData);
        if (!pData->IsDirectory())
        {
            pComponent = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentTemplate(pData->GetGUID()));
        }
        SelectComponent(pComponent);
    }
}

CComponentProxy* CreateComponentProxy(CComponentGraphic* pGraphics, size_t guid, size_t parentGuid, TCHAR* pszClassName)
{
    return new CComponentProxy(pGraphics, guid, parentGuid, pszClassName);
}

CComponentGraphic* CreateGraphic()
{
    return new CComponentGraphic_GL();
}

void CEditorMainFrame::InitComponentsPage()
{
    TString strWorkPath = CUtilityManager::GetInstance()->GetModuleFileName();
    strWorkPath = CFilePathTool::GetInstance()->ParentPath(strWorkPath.c_str());
    CComponentProxyManager::GetInstance()->DeserializeTemplateData(strWorkPath.c_str(),
                                                                    ENGINE_COMPONENT_INFO_FILE,
                                                                    ENGINE_COMPONENT_INFO_PATCH,
                                                                    CreateComponentProxy,
                                                                    CreateGraphic);
    CComponentProxyManager::GetInstance()->InitializeAllTemplate();
    const std::map<size_t, CComponentBase*>* pComponentsMap = CComponentProxyManager::GetInstance()->GetComponentTemplateMap();
    for (std::map<size_t, CComponentBase*>::const_iterator componentIter = pComponentsMap->begin(); componentIter != pComponentsMap->end(); ++componentIter )
    {
        CComponentProxy* pComponent = static_cast<CComponentProxy*>(componentIter->second);
        const TString& catalogName = pComponent->GetCatalogName();
        std::map<TString, wxTreeItemId>::iterator iter = m_componentCatalogNameMap.find(catalogName);
        //Build catalog
        if (iter == m_componentCatalogNameMap.end())
        {
            std::vector<TString> result;
            CStringHelper::GetInstance()->SplitString(catalogName.c_str(), _T("\\"), result);
            BEATS_ASSERT(result.size() > 0);
            TString findStr;
            wxTreeItemId parentId = m_componentTreeIdMap[0];
            for (size_t i = 0; i < result.size(); ++i)
            {
                if (i > 0)
                {
                    findStr.append(_T("\\"));
                }
                findStr.append(result[i]);
                std::map<TString, wxTreeItemId>::iterator iter = m_componentCatalogNameMap.find(findStr);

                if (iter == m_componentCatalogNameMap.end())
                {
                    CComponentTreeItemData* pComponentItemData = new CComponentTreeItemData(true, 0);
                    parentId = m_pComponentTemplateTreeCtrl->AppendItem(parentId, result[i], eTCIT_Folder, -1, pComponentItemData);
                    m_componentCatalogNameMap[findStr] = parentId;
                }
                else
                {
                     parentId = iter->second;
                }
            }
        }
        iter = m_componentCatalogNameMap.find(catalogName);
        BEATS_ASSERT(iter != m_componentCatalogNameMap.end());
        size_t guid = pComponent->GetGuid();
        CComponentTreeItemData* pComponentItemData = new CComponentTreeItemData(false, guid);
        m_componentTreeIdMap[guid] = m_pComponentTemplateTreeCtrl->AppendItem(iter->second, pComponent->GetDisplayName(), eTCIT_File, -1, pComponentItemData);
    }
    m_pComponentTemplateTreeCtrl->Expand(m_pComponentTemplateTreeCtrl->GetRootItem());
}

void CEditorMainFrame::InitializeComponentFileTree( CComponentProjectDirectory* pProjectData, const wxTreeItemId& id )
{
    CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(pProjectData, pProjectData->GetName());
    m_pComponentFileTreeCtrl->SetItemData(id, pData);
    const std::vector<CComponentProjectDirectory*>& children = pProjectData->GetChildren();
    for (std::vector<CComponentProjectDirectory*>::const_iterator iter = children.begin(); iter != children.end(); ++iter)
    {
        const TString& nameStr = (*iter)->GetName();
        wxTreeItemId newDirectoryId = m_pComponentFileTreeCtrl->AppendItem(id, nameStr, eTCIT_Folder, -1, NULL);
        InitializeComponentFileTree(*iter, newDirectoryId);
    }

    const std::vector<size_t>& files = pProjectData->GetFileList();
    for (std::vector<size_t>::const_iterator iter = files.begin(); iter != files.end(); ++iter)
    {
        TString strComopnentFileName = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(*iter);
        CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(NULL, strComopnentFileName);
        wxString pFileName = wxFileNameFromPath(strComopnentFileName);
        wxTreeItemId newFileId = m_pComponentFileTreeCtrl->AppendItem(id, pFileName, eTCIT_File, -1, pData);
        BEATS_ASSERT(m_componentFileListMap.find(strComopnentFileName) == m_componentFileListMap.end(), _T("Same File in a project! %s"), strComopnentFileName.c_str());
        m_componentFileListMap[strComopnentFileName] = newFileId;
    }
}

void CEditorMainFrame::OnSetStartFile(wxTreeItemId item)
{
    BEATS_ASSERT(item.IsOk());
    m_pComponentFileTreeCtrl->SetItemBold(item, true);
    m_pComponentFileTreeCtrl->SetItemTextColour(item, *wxRED);
    if (m_startFileItemId.IsOk())
    {
        m_pComponentFileTreeCtrl->SetItemBold(m_startFileItemId, false);
        m_pComponentFileTreeCtrl->SetItemTextColour(m_startFileItemId, *wxBLACK);
    }
    m_startFileItemId = item;
    m_pComponentFileTreeCtrl->Refresh();
}

void CEditorMainFrame::InitFrame()
{
    m_Manager.SetManagedWindow(this);
    InitMenu();
    InitCtrls();

    SetStatusBar(new wxStatusBar(this));
    SetStatusText(L10N_T(eL_Welcome));
    SetMinSize(wxSize(MINWINDOWSIZE,MINWINDOWSIZE));

    SelectViewID(ID_ViewButton_Scene);
    Centre();

    ELanguage currLanguage = CEditorConfig::GetInstance()->GetCurrLanguage();
    LanguageSwitch(currLanguage);
}

void CEditorMainFrame::LanguageSwitch(ELanguage language)
{
    CLanguageManager::GetInstance()->LoadFromFile(language);
    RefreshLanguage();
}

void CEditorMainFrame::RefreshLanguage()
{
    m_pMenuBar->SetMenuLabel(eMB_File, L10N_T(eL_File));
    m_pMenuBar->SetMenuLabel(eMB_Edit, L10N_T(eL_Edit));
    m_pMenuBar->SetMenuLabel(eMB_Window, L10N_T(eL_Window));
    m_pMenuBar->SetMenuLabel(eMB_Help, L10N_T(eL_Help));

    m_pFileMenu->SetLabel(Menu_File_OpenProject, L10N_T(eL_Open));
    m_pFileMenu->SetLabel(Menu_File_CloseProject, L10N_T(eL_Close));
    TCHAR szBuffer[128];
    _stprintf(szBuffer, _T("%s\tCtrl-S"), L10N_T(eL_Save).c_str());
    m_pFileMenu->SetLabel(Menu_File_SaveProject, szBuffer);
    m_pFileMenu->SetLabel(Menu_File_Export, L10N_T(eL_Export));

    m_pEditMenu->SetLabel(Menu_Open_TexturePreview, L10N_T(eL_ViewTexture));
    m_pEditMenu->SetLabel(Menu_Edit_Language, L10N_T(eL_Edit_Language));
    m_pEditMenu->SetLabel(Menu_Edit_SkeletonAnimation, L10N_T(eL_Edit_Animation));

    m_pHelpMenu->SetLabel(Menu_Help_Language, L10N_T(eL_Language));
    m_pHelpMenu->SetLabel(Menu_Help_About, L10N_T(eL_EditorMenu_About));
    m_pHelpMenu->SetLabel(Menu_Help_Language_Chinese, L10N_T(eL_Chinese));
    m_pHelpMenu->SetLabel(Menu_Help_Language_English, L10N_T(eL_English));

    m_pLeftBook->SetPageText(0, L10N_T(eL_ComponentFile));
    m_pLeftBook->SetPageText(1, L10N_T(eL_ChooseComponent));
    m_pLeftBook->SetPageText(2, L10N_T(eL_ComponentInstance));
    if (!m_activeFileItemId)
    {
        m_pCenterBook->SetPageText(0, L10N_T(eL_View));
    }
    m_pShowGUIDCheckBox->SetLabel(L10N_T(eL_ShowGuidId));
    m_pRightBook->SetPageText(0, L10N_T(eL_Property));
    m_pBottomBook->SetPageText(0, L10N_T(eL_View));
    m_pBottomBook->SetPageText(1, L10N_T(eL_View));
    m_pToolBar->SetToolLabel(ID_TB_Reopen, L10N_T(eL_ReopenProject));
    m_pToolBar->SetToolLabel(ID_TB_PerformBtn, L10N_T(eL_Performance));
    m_pToolBar->SetToolLabel(ID_CameraBtn, L10N_T(eL_Camera));
    m_pToolBar->SetToolLabel(ID_ViewAllBtn, L10N_T(eL_ViewAll));
    m_pToolBar->SetToolLabel(ID_CaptureBtn, L10N_T(eL_Capture));
    m_pToolBar->SetToolLabel(ID_SettingBtn, L10N_T(eL_Setting));
    m_pToolBar->SetToolLabel(ID_ComponentInfoBtn, L10N_T(eL_Component));
    m_pToolBar->SetToolLabel(ID_BatchEditBtn, L10N_T(eL_BatchEdit));
    m_pToolBar->SetToolLabel(ID_SkeletonAnimationEditBtn, L10N_T(eL_Edit_Animation));

    m_pResourceBook->SetPageText(0, L10N_T(eL_ComponentPage));
    m_pResourceBook->SetPageText(1, L10N_T(eL_ResourcePage));
    m_pSceneViewBtn->SetLabelText(L10N_T(eL_SceneView));
    m_pUIViewBtn->SetLabelText(L10N_T(eL_UIView));
    m_pAniViewBtn->SetLabelText(L10N_T(eL_AnimationView));
    m_pTerrainViewBtn->SetLabelText(L10N_T(eL_TerrainView));
    m_pEffectViewBtn->SetLabelText(L10N_T(eL_EffectView));
    m_pGameViewBtn->SetLabelText(L10N_T(eL_GameView));

    m_pToolBar->Refresh(false);

    if (m_pPerformanceDialog != NULL)
    {
        m_pPerformanceDialog->LanguageSwitch();
    }
    SetStatusText(L10N_T(eL_Welcome));
}

void CEditorMainFrame::OnSearchTextUpdate( wxCommandEvent& /*event*/ )
{
    m_bSearchTextUpdate = true;
    m_uLastSearchTextUpdateTime = GetTickCount();
}

CEditPerformanceDialog* CEditorMainFrame::GetPerformanceDialogPtr() const
{
    return m_pPerformanceDialog;
}

CEditCameraWnd* CEditorMainFrame::GetCameraEditWnd() const
{
    return m_pCameraWnd;
}

void CEditorMainFrame::SearchInCurrentTree()
{
    wxTreeCtrl* pTreeCtrl = static_cast<wxTreeCtrl*>(m_pLeftBook->GetCurrentPage());
    BEATS_ASSERT(pTreeCtrl != nullptr)

    if (m_bSearchTextUpdate && GetTickCount() - m_uLastSearchTextUpdateTime > 700)
    {
        m_bSearchTextUpdate = false;
        m_componentTreeIdSearchMap.clear();
        m_componentFileListSearchMap.clear();
        wxString szText = m_pSearch->GetValue();
        pTreeCtrl->CollapseAll();
        if (pTreeCtrl == m_pComponentFileTreeCtrl)
        {
            for (auto iter = m_componentFileListMap.begin(); iter != m_componentFileListMap.end(); ++iter)
            {
                wxString lableText = pTreeCtrl->GetItemText(iter->second);
                bool bMatch = lableText.Find(szText) != -1;
                // Because we can't hide tree control item, I set the text color to bg color to simulate hide.
                pTreeCtrl->SetItemTextColour(iter->second, bMatch ? 0 : 0xFFFFFFFF);
                if (bMatch)
                {
                    pTreeCtrl->EnsureVisible(iter->second);
                    m_componentFileListSearchMap[iter->first] = iter->second;
                }
            }
        }
        else
        {
            for (std::map<size_t, wxTreeItemId>::iterator iter = m_componentTreeIdMap.begin(); iter != m_componentTreeIdMap.end(); ++iter)
            {
                wxString lableText = pTreeCtrl->GetItemText(iter->second);
                bool bMatch = lableText.Find(szText) != -1;
                // Because we can't hide tree control item, I set the text color to bg color to simulate hide.
                pTreeCtrl->SetItemTextColour(iter->second, bMatch ? 0 : 0xFFFFFFFF);
                if (bMatch)
                {
                    pTreeCtrl->EnsureVisible(iter->second);
                    m_componentTreeIdSearchMap[iter->first] = iter->second;
                }
            }
        }
    }
}

void CEditorMainFrame::ShowPerformDlg()
{
    if (m_pPerformanceDialog != NULL && m_pPerformanceDialog->IsShown())
    {
        m_pPerformanceDialog->Show(false);
    }
    else
    {
        GetPerformanceDialog();
    }
}

void CEditorMainFrame::ShowAboutDlg()
{
    if (m_pAboutDialog == NULL)
    {
        m_pAboutDialog = new CAboutDlg(this, wxID_ANY, wxT("About"), wxPoint(0, 0), wxSize(600, 450), wxDEFAULT_DIALOG_STYLE);
    }
    if (!m_pAboutDialog->IsShown())
    {
        m_pAboutDialog->CenterOnScreen();
        m_pAboutDialog->ShowModal();
    }
}

wxAuiToolBar* CEditorMainFrame::GetAuiToolBarPerformPtr()
{
    return m_pToolBar;
}

void CEditorMainFrame::SetChildWindowStyle( CEditDialogBase* pDlg )
{
    pDlg->LanguageSwitch();
    if (IsMaximized())
    {
        pDlg->SetWindowStyle(wxDEFAULT_FRAME_STYLE | wxMAXIMIZE);
    }
    else
    {
        pDlg->SetWindowStyle(wxDEFAULT_FRAME_STYLE);
    }
}

void CEditorMainFrame::OnTreeCtrlExpanded( wxTreeEvent& event )
{
    wxTreeCtrl* pTreeCtrl = (wxTreeCtrl*)event.GetEventObject();
    pTreeCtrl->SetItemImage(event.GetItem(), eTCIT_FileSelected);
}

void CEditorMainFrame::OnTreeCtrlCollapsed( wxTreeEvent& event )
{
    wxTreeCtrl* pTreeCtrl = (wxTreeCtrl*)event.GetEventObject();
    pTreeCtrl->SetItemImage(event.GetItem(), eTCIT_Folder);
}

void CEditorMainFrame::UpdateComponentInstanceTreeCtrl()
{
    m_pComponentInstanceTreeCtrl->DeleteChildren(m_pComponentInstanceTreeCtrl->GetRootItem());
    const std::map<size_t, std::map<size_t, CComponentBase*>*>* pInstanceMap = CComponentProxyManager::GetInstance()->GetComponentInstanceMap();
    for (std::map<size_t, std::map<size_t, CComponentBase*>*>::const_iterator iter = pInstanceMap->begin(); iter != pInstanceMap->end(); ++iter)
    {
        TString strName = CComponentProxyManager::GetInstance()->QueryComponentName(iter->first);
        //Add a directory to tree
        wxTreeItemId itemID = m_pComponentInstanceTreeCtrl->AppendItem(m_pComponentInstanceTreeCtrl->GetRootItem(), strName, eTCIT_Folder, -1, new CComponentInstanceTreeItemData(NULL));
        for (std::map<size_t, CComponentBase*>::const_iterator subIter = iter->second->begin(); subIter != iter->second->end(); ++subIter)
        {
            //Add a file node to the directory.
            CComponentProxy* pProxy = (CComponentProxy*)subIter->second;
            wxString strIdLabel = pProxy->GetUserDefineDisplayName();
            if (strIdLabel.IsNull())
            {
                strIdLabel = pProxy->GetDisplayName();
            }
            CComponentInstanceTreeItemData* pComponentInstanceItemData = new CComponentInstanceTreeItemData(subIter->second);
            m_pComponentInstanceTreeCtrl->AppendItem(itemID, strIdLabel, eTCIT_File, -1, pComponentInstanceItemData);
        }
    }
    m_pComponentInstanceTreeCtrl->ExpandAll();
    GetViewAgent(m_nCurrentViewID)->OnUpdateComponentInstance();
}

void CEditorMainFrame::OnSelectComponentInstanceTreeItem( wxTreeEvent& /*event*/ )
{
    wxTreeItemId activeId = m_pComponentInstanceTreeCtrl->GetSelection();
    if (activeId.IsOk())
    {
        CComponentInstanceTreeItemData* pItemData = (CComponentInstanceTreeItemData*)m_pComponentInstanceTreeCtrl->GetItemData(activeId);
        BEATS_ASSERT(pItemData != NULL);
        if (!pItemData->IsDirectory())
        {
            CComponentProxy* pComponentProxy = dynamic_cast<CComponentProxy*>(pItemData->GetComponentBase());
            SelectComponent(pComponentProxy);
        }
    }
}

void CEditorMainFrame::OnSelectTreeItem( wxTreeEvent& event )
{
    switch (event.GetId())
    {
    case Ctrl_Tree_CompontentInstance:
        OnSelectComponentInstanceTreeItem(event);
        break;
    case Ctrl_Tree_CompontentTemplate:
        OnSelectComponentTemplateTreeItem(event);
        break;
    default:
        GetViewAgent(m_nCurrentViewID)->OnTreeCtrlSelect(event);
        break;
    }
}

void CEditorMainFrame::OnCheckBoxGuidId( wxCommandEvent& event )
{
    m_bIsShowGuidId = event.IsChecked();
    UpdatePropertyGrid();
}

bool CEditorMainFrame::GetShowGuidId()
{
    return m_bIsShowGuidId;
}

void CEditorMainFrame::OnIdle( wxIdleEvent& event )
{
    if (m_pPropGridManager->IsNeedUpdatePropertyGrid())
    {
        m_pPropGridManager->ExecutePropertyDelete();
    }
    SearchInCurrentTree();
    event;
#ifdef _DEBUG
    Update();
    m_pCameraWnd->UpdateInfo();
    if (!event.MoreRequested())
    {
        event.RequestMore();
    }
#endif
}

void CEditorMainFrame::OnTimer( wxTimerEvent& /*event*/ )
{
    Update();
    if (m_pCameraWnd)
    {
        m_pCameraWnd->UpdateInfo();
    }
    wxApp::GetInstance()->ProcessIdle();//don't change the position.//Call this manually in order to handle the ExitEvent, because OnTimer was called too frequently
}

void CEditorMainFrame::Update()
{
    BEYONDENGINE_PERFORMDETECT_START(ePNT_Editor)
    if (CApplication::GetInstance()->ShouldUpdateThisFrame())
    {
        for (auto iter : m_updateWindowVector)
        {
            if (m_nCurrentViewID != ID_ViewButton_Game || iter != m_pComponentRenderWindow)
            {
                iter->Update();
            }
        }
        GetViewAgent(m_nCurrentViewID)->Update();
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_Editor)
    BEYONDENGINE_PERFORMDETECT_RESET();
}

bool CEditorMainFrame::Show(bool show)
{
    bool bRet = wxFrame::Show(show);
    if(show && bRet)
    {
        m_pViewScreen->SetContextToCurrent();
    }
    return bRet;
}

CEditorSceneWindow* CEditorMainFrame::GetSceneWindow() const
{
    return m_pViewScreen;
}

void CEditorMainFrame::StartRenderUpdate()
{
    m_Timer.Start(15);
}

void CEditorMainFrame::RegisterUpdateWindow( CBeyondEngineEditorGLWindow* pWindow )
{
    m_updateWindowVector.push_back(pWindow);
}

void CEditorMainFrame::HideTreeCtrl( wxTreeCtrl* pTreeCtrl )
{
    if (pTreeCtrl->IsShown())
    {
        size_t uPageCount = m_pLeftBook->GetPageCount();
        for (size_t i = 0; i < uPageCount; ++i)
        {
            if (m_pLeftBook->GetPage(i) == pTreeCtrl)
            {
                m_pLeftBook->RemovePage(i);
                pTreeCtrl->Hide();
                break;
            }
        }
    }
}

void CEditorMainFrame::SelectViewID( int nViewID )
{
    m_pSceneViewBtn->SetValue(ID_ViewButton_Scene == nViewID);
    m_pUIViewBtn->SetValue(ID_ViewButton_UI == nViewID);
    m_pAniViewBtn->SetValue(ID_ViewButton_Ani == nViewID);
    m_pTerrainViewBtn->SetValue(ID_ViewButton_Terrain == nViewID);
    m_pEffectViewBtn->SetValue(ID_ViewButton_Effect == nViewID);
    m_pGameViewBtn->SetValue(ID_ViewButton_Game == nViewID);

    if (m_nCurrentViewID != nViewID)
    {
        if (m_nCurrentViewID != INVALID_DATA)
        {
            GetViewAgent(m_nCurrentViewID)->OutView();
        }
        GetViewAgent(nViewID)->InView();
        m_nCurrentViewID = nViewID;
        SelectComponent(NULL);
    }
}

int CEditorMainFrame::GetSelectdViewID()
{
    return m_nCurrentViewID;
}

void CEditorMainFrame::OnEditTreeItemLabel( wxTreeEvent& event )
{
    wxTreeItemId itemId = event.GetItem();
    wxString oldName;
    wxString newName = event.GetLabel();
    int id = event.GetId();
    switch (id)
    {
    case Ctrl_Tree_CompontentFile:
        oldName = m_pComponentFileTreeCtrl->GetItemText(itemId);
        if (oldName != newName)
        {
            CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTreeCtrl->GetItemData(itemId);
            wxString filePath = pData->GetFileName();
            for (auto itr : m_componentFileListMap)
            {
                if (itr.first == filePath)
                {
                    wxString newPath;
                    wxFileName::SplitPath(filePath, &newPath, NULL, NULL);
                    newPath += newName;
                    //TODO:when file name changed, update the name
                    //                rename(filePath, newPath);
                    //                m_componentFileListMap.erase(itr);
                    //                m_componentFileListMap[newPath] = itemId;
                }
            }
        }
        break;
    case Ctrl_Tree_CompontentInstance:
        oldName = m_pComponentInstanceTreeCtrl->GetItemText(itemId);
        if (oldName != newName)
        {
            CComponentInstanceTreeItemData* pInstanceData = (CComponentInstanceTreeItemData*)m_pComponentInstanceTreeCtrl->GetItemData(event.GetItem());
            BEATS_ASSERT(pInstanceData);
            CComponentProxy* pProxy = (CComponentProxy*)pInstanceData->GetComponentBase();
            if (pProxy)
            {
                pProxy->SetUserDefineDisplayName(newName);
            }
        }
        break;
    default:
        GetViewAgent(m_nCurrentViewID)->OnEditTreeItemLabel(event);
        break;
    }

}

void CEditorMainFrame::SetMouseType( EMouseType iType )
{
    m_eMouseType = iType;
}

EMouseType CEditorMainFrame::GetMouseType() const
{
    return m_eMouseType;
}

void CEditorMainFrame::OnMouseInCurrentView( wxMouseEvent& event )
{
    CViewAgentBase* pViewAgent = GetViewAgent(m_nCurrentViewID);
    pViewAgent->ProcessMouseEvent(event);
}

void CEditorMainFrame::OnTreeCtrlFocused( wxFocusEvent& event )
{
    wxTreeEvent treeEvent;
    switch (event.GetId())
    {
    case Ctrl_Tree_CompontentTemplate:
        OnSelectComponentTemplateTreeItem(treeEvent);
        break;
    case Ctrl_Tree_CompontentInstance:
        OnSelectComponentInstanceTreeItem(treeEvent);
        break;
    default:
        break;
    }
    event.Skip();//call skip() is necessary to process the focuse event in default way
}

void CEditorMainFrame::OnActivateTreeItem(wxTreeEvent& event)
{
    switch (event.GetId())
    {
    case Ctrl_Tree_CompontentFile:
        OnActivateComponentFile(event);
        break;
    case Ctrl_Tree_CompontentTemplate:
        OnActivateComponentTemplate(event);
        break;
    default:
        GetViewAgent(m_nCurrentViewID)->OnActivateTreeItem(event);
        break;
    }
}

void CEditorMainFrame::OnPropertyGridRightClick( wxPropertyGridEvent& event )
{
    wxPGProperty* pProperty = event.GetProperty();
    m_pPropertyMenu->Enable(ePMS_ResetValue, pProperty->HasFlag(wxPG_PROP_MODIFIED) != 0);
    m_pPropertyMenu->Enable(ePMS_Expand, pProperty->GetChildCount() > 0 && pProperty->HasFlag(wxPG_PROP_COLLAPSED) != 0);
    m_pPropertyMenu->Enable(ePMS_Collapsed, pProperty->GetChildCount() > 0 && pProperty->HasFlag(wxPG_PROP_COLLAPSED) == 0);
    PopupMenu(m_pPropertyMenu, wxDefaultPosition);
}

void CEditorMainFrame::OnPropertyGridMenuClick(wxMenuEvent& event)
{
    wxPGProperty* pProperty = m_pPropGridManager->GetGrid()->GetSelection();
    if (pProperty)
    {
        switch(event.GetId())
        {
        case ePMS_ResetValue:
            ResetPropertyValue(pProperty);
            break;
        case ePMS_Expand:
            pProperty->SetExpanded(true);
            break;
        case ePMS_Collapsed:
            pProperty->SetExpanded(false);
            break;
        default:
            BEATS_ASSERT(false, _T("Never reach here!"));
            break;
        }
        pProperty->RefreshEditor();
        pProperty->GetGrid()->Refresh();
    }
}

void CEditorMainFrame::ResetPropertyValue(wxPGProperty* pProperty)
{
    CWxwidgetsPropertyBase* pPropertyBase = static_cast<CWxwidgetsPropertyBase*>(pProperty->GetClientData());
    pProperty->SetValue(pProperty->GetDefaultValue());
    if (pProperty->GetChildCount() > 0)
    {
        pProperty->DeleteChildren();
    }
    if (pPropertyBase->GetType() == eRPT_Ptr)
    {
        CPtrPropertyDescription* pPtrProperty = static_cast<CPtrPropertyDescription*>(pPropertyBase);
        pPtrProperty->DestroyInstance(true);
    }
    else if (pPropertyBase->GetType() == eRPT_List)
    {
        CListPropertyDescription* pListProperty = static_cast<CListPropertyDescription*>(pPropertyBase);
        pListProperty->DeleteAllChild();
    }
    pProperty->RecreateEditor();
    m_pPropGridManager->InsertInPropertyGrid(pPropertyBase->GetChildren(), pProperty);
    m_pPropGridManager->OnComponentPropertyChangedImpl(pProperty);
}

void CEditorMainFrame::SetCursor()
{
    m_pViewScreen->SetCursor(m_nCursorIconID);
}

CEnginePropertyGirdManager* CEditorMainFrame::GetPropGridManager()
{
    return m_pPropGridManager;
}

wxSplitterWindow* CEditorMainFrame::GetSplitter() const
{
    return m_pSplitter;
}

CViewAgentBase* CEditorMainFrame::GetViewAgent( int nViewID )
{
    CViewAgentBase* pViewAgent = nullptr;
    switch (nViewID)
    {
    case ID_ViewButton_Scene:
        pViewAgent = CSceneViewAgent::GetInstance();
        break;
    case ID_ViewButton_Ani:
        pViewAgent = CAniViewAgent::GetInstance();
        break;
    case ID_ViewButton_UI:
        pViewAgent = CUIViewAgent::GetInstance();
        break;
    case ID_ViewButton_Terrain:
        pViewAgent = CTerrainViewAgent::GetInstance();
        break;
    case ID_ViewButton_Effect:
        pViewAgent = CEffectViewAgent::GetInstance();
        break;
    case ID_ViewButton_Game:
        pViewAgent = CGameViewAgent::GetInstance();
        break;
    }
    BEATS_ASSERT(pViewAgent != nullptr);
    return pViewAgent;
}

void CEditorMainFrame::ToggleViewButton()
{
    m_pSceneViewBtn->Show(m_pMenuBar->IsChecked(Menu_Window_Check_Scene));
    m_pUIViewBtn->Show(m_pMenuBar->IsChecked(Menu_Window_Check_UI));
    m_pAniViewBtn->Show(m_pMenuBar->IsChecked(Menu_Window_Check_Ani));
    m_pTerrainViewBtn->Show(m_pMenuBar->IsChecked(Menu_Window_Check_Terrain));
    m_pEffectViewBtn->Show(m_pMenuBar->IsChecked(Menu_Window_Check_Effect));
    m_pGameViewBtn->Show(m_pMenuBar->IsChecked(Menu_Window_Check_Game));
    m_Manager.Update();
    m_pCenterPanel->GetSizer()->RecalcSizes();
}

CBeyondEngineEditorComponentWindow* CEditorMainFrame::GetComponentWindow()
{
    return m_pComponentRenderWindow;
}

void CEditorMainFrame::PostInitialize()
{
    CApplication::GetInstance()->Initialize();
    InitComponentsPage();
    GetComponentWindow()->ResetProjectionMode();
#ifdef BEYOND_ENGINE_PUBLISH_VERSION
    if (CEditorConfig::GetInstance()->IsShowAboutDlgAfterLaunch())
    {
        ShowAboutDlg();
    }
#endif
}

void CEditorMainFrame::CreateExtraWindow()
{
    m_pTexturePreviewDialog = new CTexturePreviewDialog(NULL, wxID_ANY, wxT("Texture"), wxDefaultPosition, wxDefaultSize, wxCLOSE_BOX | wxCAPTION);
}

void CEditorMainFrame::OnSearch( wxCommandEvent& /*event*/ )
{
    SearchInCurrentTree();
}

void CEditorMainFrame::OnPropertyChanged(wxPropertyGridEvent& event)
{
    GetViewAgent(m_nCurrentViewID)->OnPropertyChanged(event);
}

void CEditorMainFrame::OnEditDataViewItem(wxDataViewEvent& event)
{
    GetViewAgent(m_nCurrentViewID)->OnEditDataViewItem(event);
}
