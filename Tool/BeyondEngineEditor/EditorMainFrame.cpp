#include "stdafx.h"
#include "EditorMainFrame.h"
#include "timebarframe.h"
#include "EngineEditor.h"
#include "ComponentTreeItemData.h"
#include "ComponentFileTreeItemData.h"
#include "BeyondEngineEditorComponentWindow.h"
#include "ComponentGraphics_GL.h"
#include "EditLanguageDialog.h"
#include "EditPerformanceDialog.h"
#include "Render/Camera.h"
#include "EnginePublic/TimeMeter.h"
#include "EnginePublic/EngineCenter.h"
#include "Framework/Application.h"
#include "ListPropertyDescription.h"
#include "PtrPropertyDescription.h"
#include "TerrainViewAgent.h"
#include "GameViewAgent.h"
#include "SceneViewAgent.h"
#include "AniViewAgent.h"
#include "ViewAgentBase.h"
#include "TexturePreviewDialog.h"
#include "Utility/BeatsUtility/UtilityManager.h"
#include "Component/Component/ComponentProxyManager.h"
#include "Component/Component/ComponentProxy.h"
#include "Component/Component/ComponentProject.h"
#include "Component/Component/ComponentGraphic.h"
#include "Component/Component/ComponentProjectDirectory.h"
#include "Component/ComponentPublic.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Utility/BeatsUtility/FileFilter.h"

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
#include <wx/wfstream.h>
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
#include "Resource/ResourceManager.h"
#include "ComponentProxyTreeItemData.h"
#include "external/Image.h"
#include "Render/Texture.h"
#include "wx/dir.h"
#include "external/etc1/etc1.h"
#include "WaitingForProcessDialog.h"
#include "Utility/BeatsUtility/md5.h"
#include "external/ZipUtils.h"
#include "SetVersionDialog.h"
#include "Render/CoordinateRenderObject.h"
#include "SelectResolutionDialog.h"
#include "CoordinateSettingWnd.h"
#include "ScanFileDialog.h"

#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "OperationRecordManager.h"
#include "StringPropertyDescription.h"
#include "MapPropertyDescription.h"
#include "ParticleControlWnd.h"
#include "ParticleSystem/ParticleEmitter.h"
#include "Vec3fPropertyDescription.h"
#include "RandomValuePropertyDescription.h"
#include "Resource/Resource.h"
#include "Render/TextureAtlas.h"
#include "EnumPropertyDescription.h"

#define MAINFRAMENORMALSIZE wxSize(1024, 800)
#define MAINFRAMEPOSITION wxPoint(40, 40)
#define BUTTONSIZE wxSize(20, 20)
#define SIZERBORDERWIDTH 5
#define PANELSIZESCALE 0.13
#define RIGHTPANELSIZESCALE 0.19
#define IDBEGIN 1111

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
EVT_PG_SELECTED(ID_PropertyGridManager, CEditorMainFrame::OnPropertyGridSelect)
EVT_PG_CHANGED(wxID_ANY, CEditorMainFrame::OnPropertyChanged)
EVT_COMMAND(wxID_ANY, TIMTBAR_DRAGITEMEND_EVENT, CEditorMainFrame::OnTimeBarTreeItemDragEnd)
EVT_COMMAND(wxID_ANY, TIMTBAR_SELECTITEM_EVENT, CEditorMainFrame::OnTimeBarTreeItemSelected)
EVT_COMMAND(wxID_ANY, TIMTBAR_CHOICE_EVENT, CEditorMainFrame::OnTimeBarChoice)
EVT_COMMAND(wxID_ANY, TIMTBAR_ADDBUTTONCLICK_EVENT, CEditorMainFrame::OnTimeBarAddButtonClick)
EVT_COMMAND(wxID_ANY, TIMTBAR_MINUSBUTTONCLICK_EVENT, CEditorMainFrame::OnTimeBarMinusButtonClick)
EVT_COMMAND(wxID_ANY, TIMTBAR_ITEMCONTAINERRCLICK_EVENT, CEditorMainFrame::OnTimeBarItemContainerRClick)
EVT_COMMAND(wxID_ANY, TIMTBAR_CURSORCHANGE_EVENT, CEditorMainFrame::OnTimeBarCursorChange)
EVT_COMMAND(wxID_ANY, TIMTBAR_SELECTFRAMECHANGE_EVENT, CEditorMainFrame::OnTimeBarSelectFrameChange)
EVT_COMMAND(wxID_ANY, TIMTBAR_DRAGGINGFRAME_EVENT, CEditorMainFrame::OnTimeBarDraggingFrame)
END_EVENT_TABLE()
#ifndef _DEBUG
#pragma warning(disable:4722)
#endif

CEditorMainFrame::CEditorMainFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, MAINFRAMENORMALSIZE, wxDEFAULT_FRAME_STYLE | wxMAXIMIZE)
    , m_nCursorIconID(wxCURSOR_ARROW)
    , m_nSashPosition(0)
    , m_nPng2EtcFinishCount(0)
    , m_pTexturePreviewDialog(NULL)
    , m_pWEditLanguage(NULL)
    , m_pScanFileDialog(NULL)
    , m_bSearchTextUpdate(false)
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
    , m_nTaskCurCount(0)
    , m_pSetCoordinateRenderObjectWnd(NULL)
    , m_pCoordinateRenderTypeChoice(nullptr)
{
    SetIcon(wxICON(sample));
    QueryPerformanceFrequency(&m_uTimeFrequency);
}

CEditorMainFrame::~CEditorMainFrame()
{
    // Designers don't wanna see any crash.
    exit(0);
    //static_cast<CEngineEditor*>(wxApp::GetInstance())->OnExitImpl();
    //m_Timer.Stop();
    //BEATS_SAFE_DELETE(m_pComponentFileMenu);
    //BEATS_SAFE_DELETE(m_pPropertyMenu);
    //m_pTexturePreviewDialog->Destroy();
    //m_Manager.UnInit();
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

    // TODO: remove all hard-code string, use L10N_T instead.
    m_pFileMenu->Append(Menu_File_OpenProject, wxT("Open"));
    m_pFileMenu->Append(Menu_File_CloseProject, wxT("Close"));
    m_pFileMenu->Append(Menu_File_SaveProject, wxT("&Save\tCtrl-S"));
    m_pFileMenu->Append(Menu_File_Undo, wxT("&Undo\tCtrl-Z"));
    m_pFileMenu->Append(Menu_File_Redo, wxT("&Redo\tCtrl-Y"));
    m_pFileMenu->Append(Menu_File_Export, wxT("Export"))->Enable(false);
    m_pFileMenu->Append(Menu_File_PackResource, wxT("PackResource"))->Enable(false);

    m_pEditMenu->Append(Menu_Open_TexturePreview, wxT("View Texture"));
    m_pEditMenu->Append(Menu_Edit_Language, wxT("Edit Language"));

    m_pHelpMenu->Append(Menu_Help_Language, wxT("Language"), m_pHelpLanguageMenu);
    m_pHelpMenu->Append(Menu_Help_About, L10N_T(eLTT_Editor_Menu_About));
    m_pHelpLanguageMenu->Append(Menu_Help_Language_Chinese, wxT("Chinese"));
    m_pHelpLanguageMenu->Append(Menu_Help_Language_English, wxT("English"));

    wxMenu *pWindowSubMenu = new wxMenu;
    pWindowSubMenu->Append(Menu_Window_Check_Scene, L10N_T(eLTT_Editor_Menu_Scene), wxT("Show or hide Scene view button"), true);
    pWindowSubMenu->Append(Menu_Window_Check_UI, L10N_T(eLTT_Editor_Menu_UI), wxT("Show or hide UI view button"), true);
    pWindowSubMenu->Append(Menu_Window_Check_Ani, L10N_T(eLTT_Editor_Menu_AnimationView), wxT("Show or hide Ani view button"), true);
    pWindowSubMenu->Append(Menu_Window_Check_Terrain, L10N_T(eLTT_Editor_Common_Terrain), wxT("Show or hide Terrain view button"), true);
    pWindowSubMenu->Append(Menu_Window_Check_Effect, L10N_T(eLTT_Editor_Menu_Effect), wxT("Show or hide Effect view button"), true);
    pWindowSubMenu->Append(Menu_Window_Check_Game, L10N_T(eLTT_Editor_Menu_GameView), wxT("Show or hide Game view button"), true);
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
    m_pPropertyMenu->Append(ePMS_CopyProperty, _T("拷贝"));
    m_pPropertyMenu->Append(ePMS_PasteProperty, _T("粘贴"));
    m_pPropertyMenu->Append(ePMS_Import, _T("导入"));
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
    const TString& strLastOpenFile = CEditorConfig::GetInstance()->GetLastOpenFile();
    m_pToolBar->AddTool(ID_TB_Reopen, L10N_T(eLTT_Editor_Menu_Reopen), wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK), strLastOpenProject.c_str());
    if (strLastOpenProject.length() == 0 || !CFilePathTool::GetInstance()->Exists(strLastOpenProject.c_str()) ||
        strLastOpenFile.length() == 0 || !CFilePathTool::GetInstance()->Exists(strLastOpenFile.c_str()))
    {
        m_pToolBar->EnableTool(ID_TB_Reopen, false);
    }
    m_pToolBar->AddSeparator();

    m_pToolBar->AddTool(ID_TB_PerformBtn, L10N_T(eLTT_Editor_Menu_Performance), wxArtProvider::GetBitmap(wxART_FILE_OPEN), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_CameraBtn, L10N_T(eLTT_Editor_Menu_Camera), wxArtProvider::GetBitmap(wxART_GO_HOME), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_ViewAllBtn, L10N_T(eLTT_Editor_Menu_SimulateView), wxArtProvider::GetBitmap(wxART_REPORT_VIEW), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_CaptureBtn, L10N_T(eLTT_Editor_Menu_Capture), wxArtProvider::GetBitmap(wxART_HELP_SETTINGS), wxEmptyString, wxITEM_NORMAL);
    m_pToolBar->AddTool(ID_SettingBtn, L10N_T(eLTT_Editor_Common_Setting), wxArtProvider::GetBitmap(wxART_GO_FORWARD), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_GMBtn, L10N_T(eLTT_Editor_Common_GM), wxArtProvider::GetBitmap(wxART_INFORMATION), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_ComponentInfoBtn, L10N_T(eLTT_Editor_Menu_Component), wxArtProvider::GetBitmap(wxART_HARDDISK), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_BatchEditBtn, L10N_T(eLTT_Editor_Menu_BatchEdit), wxArtProvider::GetBitmap(wxART_HELP_BOOK), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_PauseApplication, L10N_T(eLTT_Editor_Common_Pause), wxArtProvider::GetBitmap(wxART_PRINT), "press ctrl to set the time scale", wxITEM_CHECK);
    m_pToolBar->AddTool(ID_SetCoordinateBtn, _T("坐标系设置"), wxArtProvider::GetBitmap(wxART_GO_DOWN), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_UIEditMode, _T("UI编辑模式"), wxArtProvider::GetBitmap(wxART_CDROM), wxEmptyString, wxITEM_CHECK);
    m_pToolBar->AddTool(ID_ReloadResource, _T("重新加载资源"), wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE), wxEmptyString, wxITEM_NORMAL);

    m_pToolBar->ToggleTool(ID_UIEditMode, true);
    m_pCameraWnd = new CEditCameraWnd(this, wxID_ANY, wxT("Camera"), wxPoint(500, 200), wxSize(300, 500), wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);
    m_pSceneGridWnd = new CSceneGridWnd(this, wxID_ANY, wxT("Grid"), wxPoint(500, 200), wxSize(350, 500), wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);
    m_pComponentInfoWnd = new CComponentInfoWnd(this, wxID_ANY, wxT("Grid"), wxPoint(500, 200), wxSize(300, 500), wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT ^ wxMINIMIZE_BOX);
    m_pBatchEditWnd = new CBatchEditWnd(this, wxID_ANY, wxT("BatchEdit"), wxPoint(500, 200), wxSize(300, 500), wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);
    m_pParticleControlWnd = new CParticleControlWnd(this, wxID_ANY, wxT("Particle Effect"), wxPoint(0, 0), wxSize(220, 90), wxCAPTION | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR | wxNO_BORDER);
    m_pSetCoordinateRenderObjectWnd = new CCoordinateSettingWnd(this, wxID_ANY, wxT("坐标系设置"), wxPoint(500, 200), wxSize(300, 400), wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);
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
    pSizer->Add(m_pLeftBook, 1, wxGROW | wxALL, 0);
    pSizer->Add(m_pSearch, 0, wxGROW | wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pRightTopSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pRightPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(client_size.GetWidth() * RIGHTPANELSIZESCALE, client_size.GetHeight() * RIGHTPANELSIZESCALE));
    m_pRightPanel->SetSizer(pSizer);
    m_pShowGUIDCheckBox = new wxCheckBox(m_pRightPanel, Ctrl_CheckBox_ShowGuidId, wxT("ShowGI"));
    pRightTopSizer->Add(m_pShowGUIDCheckBox, 0, wxALIGN_CENTER | wxALL, 0);

    wxString strViewChoiceString[] = { wxT("Local"), wxT("Parent"), wxT("World") };
    int uChoiceCount = sizeof(strViewChoiceString) / sizeof(wxString);
    m_pCoordinateRenderTypeChoice = new wxChoice(m_pRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, uChoiceCount, strViewChoiceString, 0);
    pRightTopSizer->Add(m_pCoordinateRenderTypeChoice, 0, wxALIGN_CENTER | wxALL, 0);

    pSizer->Add(pRightTopSizer, 0, wxGROW | wxALL, 0);

    m_pRightBook = new wxAuiNotebook(m_pRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pRightBook, 1, wxGROW | wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pBottomPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(client_size.GetWidth() * RIGHTPANELSIZESCALE, client_size.GetHeight() * RIGHTPANELSIZESCALE));
    m_pBottomPanel->SetSizer(pSizer);
    m_pBottomBook = new wxAuiNotebook(m_pBottomPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pBottomBook, 1, wxGROW | wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pResourcePanel = new wxPanel(this);
    m_pResourcePanel->SetSizer(pSizer);
    m_pResourceBook = new wxAuiNotebook(m_pResourcePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pResourceBook, 1, wxGROW | wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pCenterPanel = new wxPanel(this);
    m_pCenterPanel->SetSizer(pSizer);
    m_pCenterBook = new wxAuiNotebook(m_pCenterPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    m_pSceneViewBtn = new wxToggleButton(m_pCenterPanel, ID_ViewButton_Scene, L10N_T(eLTT_Editor_Menu_Scene));
    m_pUIViewBtn = new wxToggleButton(m_pCenterPanel, ID_ViewButton_UI, L10N_T(eLTT_Editor_Menu_UI));
    m_pAniViewBtn = new wxToggleButton(m_pCenterPanel, ID_ViewButton_Ani, L10N_T(eLTT_Editor_Menu_AnimationView));
    m_pTerrainViewBtn = new wxToggleButton(m_pCenterPanel, ID_ViewButton_Terrain, L10N_T(eLTT_Editor_Common_Terrain));
    m_pGameViewBtn = new wxToggleButton(m_pCenterPanel, ID_ViewButton_Game, L10N_T(eLTT_Editor_Menu_GameView));

    wxBoxSizer* pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    pButtonSizer->Add(m_pSceneViewBtn, 0, wxALIGN_CENTER | wxALL, 0);
    pButtonSizer->Add(m_pUIViewBtn, 0, wxALIGN_CENTER | wxALL, 0);
    pButtonSizer->Add(m_pAniViewBtn, 0, wxALIGN_CENTER | wxALL, 0);
    pButtonSizer->Add(m_pTerrainViewBtn, 0, wxALIGN_CENTER | wxALL, 0);
    pButtonSizer->AddSpacer(10);
    pButtonSizer->Add(m_pGameViewBtn, 0, wxALIGN_CENTER | wxALL, 0);

    pSizer->Add(pButtonSizer, 0, wxALIGN_CENTER | wxALL, 0);
    pSizer->Add(m_pCenterBook, 1, wxGROW | wxALL, 0);

    m_pToolPanel = new wxPanel(this);
    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pToolPanel->SetSizer(pSizer);
    m_pToolBook = new wxAuiNotebook(m_pToolPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pToolBook, 1, wxGROW | wxALL, 0);

    m_pCoordinateRenderTypeChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(CEditorMainFrame::OnViewChoiceChanged), NULL, this);
}

void CEditorMainFrame::CreateTimeBar()
{
    m_pTimeBar = new CTimeBarFrame(m_pBottomBook);
}

void CEditorMainFrame::CreatePropertyGrid()
{
    int style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPG_TOOLTIPS;
    int extraStyle = wxPG_EX_MODE_BUTTONS | wxPG_EX_MULTIPLE_SELECTION;

    m_pPropGridManager = new CEnginePropertyGridManager();
    m_pPropGridManager->Create(m_pRightBook, ID_PropertyGridManager, wxDefaultPosition, wxDefaultSize, style);
    m_pPropGrid = m_pPropGridManager->GetGrid();
    m_pPropGridManager->SetExtraStyle(extraStyle);
    m_pPropGridManager->SetValidationFailureBehavior(wxPG_VFB_MARK_CELL | wxPG_VFB_SHOW_MESSAGEBOX);
    m_pPropGrid->SetVerticalSpacing(2);
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
    for (uint32_t i = 0; i < eTCIT_Count; ++i)
    {
        pFileIconImages->Add(fileIcons[i]);
    }
    m_pComponentFileTreeCtrl = new wxTreeCtrl(m_pLeftBook, Ctrl_Tree_CompontentFile, wxPoint(0, 0), wxDefaultSize, lStyle | wxTR_EDIT_LABELS);
    m_pComponentFileTreeCtrl->AssignImageList(pFileIconImages);
    m_pComponentFileTreeCtrl->AddRoot(wxT("Root"), eTCIT_Folder, -1, new CComponentFileTreeItemData(NULL, _T("")));

    pFileIconImages = new wxImageList(TREEICONSIZE, TREEICONSIZE, true);
    for (uint32_t i = 0; i < eTCIT_Count; ++i)
    {
        pFileIconImages->Add(fileIcons[i]);
    }
    m_pComponentProxyTreeCtrl = new wxTreeCtrl(m_pLeftBook, Ctrl_Tree_CompontentProxy, wxPoint(0, 0), wxDefaultSize, lStyle | wxTR_EDIT_LABELS);
    m_pComponentProxyTreeCtrl->AssignImageList(pFileIconImages);
    m_pComponentProxyTreeCtrl->Connect(wxEVT_TREE_SEL_CHANGED, wxTreeEventHandler(CEditorMainFrame::OnSelectTreeCtrl), NULL, this);
    m_pComponentProxyTreeCtrl->AddRoot(wxT("ComponentInstance"), eTCIT_Folder, -1, new CComponentProxyTreeItemData(NULL));
    m_pComponentProxyTreeCtrl->Hide();

    m_pLastOpenFilesTreeCtrl = new wxTreeCtrl(m_pLeftBook, Ctrl_Tree_LastOpenFiles, wxPoint(0, 0), wxDefaultSize, lStyle | wxTR_EDIT_LABELS);
    m_pLastOpenFilesTreeCtrl->AssignImageList(pFileIconImages);
    m_pLastOpenFilesTreeCtrl->Connect(wxEVT_TREE_SEL_CHANGED, wxTreeEventHandler(CEditorMainFrame::OnSelectTreeCtrl), NULL, this);
    m_pLastOpenFilesTreeCtrl->AddRoot(wxT("Last Open Files"), eTCIT_Folder, -1);
    m_pLastOpenFilesTreeCtrl->Hide();

    pFileIconImages = new wxImageList(TREEICONSIZE, TREEICONSIZE, true);
    for (uint32_t i = 0; i < eTCIT_Count; ++i)
    {
        pFileIconImages->Add(fileIcons[i]);
    }
    m_pComponentTemplateTreeCtrl = new wxTreeCtrl(m_pResourceBook, Ctrl_Tree_CompontentTemplate, wxPoint(0, 0), wxDefaultSize, lStyle);
    m_pComponentTemplateTreeCtrl->AssignImageList(pFileIconImages);
    m_pComponentTemplateTreeCtrl->Connect(wxEVT_TREE_SEL_CHANGED, wxTreeEventHandler(CEditorMainFrame::OnSelectTreeCtrl), NULL, this);
    wxTreeItemId rootId = m_pComponentTemplateTreeCtrl->AddRoot(wxT("Components"), eTCIT_Folder, -1, new CComponentTreeItemData(true, 0));
    m_componentCatalogNameMap[_T("Root")] = rootId;
    m_componentTreeIdMap[0] = rootId;

    //TODO: use ResourceTreeCtrl to contain all resource
    pFileIconImages = new wxImageList(15, 15, true);
    for (uint32_t i = 0; i < eTCIT_Count; ++i)
    {
        pFileIconImages->Add(fileIcons[i]);
    }
    m_pResourceTreeCtrl = new wxTreeCtrl(m_pResourceBook, Ctrl_Tree_Resource, wxPoint(0, 0), wxSize(160, 250), lStyle | wxTR_EDIT_LABELS);
    m_pResourceTreeCtrl->AssignImageList(pFileIconImages);
    m_pResourceTreeCtrl->AddRoot(wxT("Resource"), eTCIT_Folder, -1, new CComponentProxyTreeItemData(NULL));
}

void CEditorMainFrame::AddPageToBook()
{
    m_pLeftBook->Freeze();
    m_pLeftBook->AddPage(m_pComponentFileTreeCtrl, L10N_T(eLTT_Editor_Common_File));
    m_pLeftBook->Thaw();

    m_pRightBook->Freeze();
    m_pRightBook->AddPage(m_pPropGridManager, L10N_T(eLTT_Editor_Common_Property));
    m_pRightBook->Thaw();

    m_pBottomBook->Freeze();
    m_pBottomBook->AddPage(m_pTimeBar, L10N_T(eLTT_Editor_Common_Animation));
    m_pBottomBook->Thaw();

    m_pCenterBook->Freeze();
    m_pCenterBook->AddPage(m_pSplitter, L10N_T(eLTT_Editor_Common_View));
    m_pCenterBook->Thaw();

    m_pResourceBook->Freeze();
    m_pResourceBook->AddPage(m_pComponentTemplateTreeCtrl, L10N_T(eLTT_Editor_Menu_Component));
    m_pResourceBook->AddPage(m_pResourceTreeCtrl, wxT("Resource"));
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
    info6.dock_proportion = 3;
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
        m_pWEditLanguage = new CEditLanguageDialog(this, wxID_ANY, wxT("Edit Language"), wxPoint(500, 200), wxSize(600, 700));
    }
    m_pWEditLanguage->LanguageSwitch();
    m_pWEditLanguage->wxDialog::Show();
}

void CEditorMainFrame::ShowScanFilesDialog()
{
    if (m_pScanFileDialog == NULL)
    {
        m_pScanFileDialog = new CScanFileDialog(this, wxID_ANY, wxT("未引用的文件"), wxPoint(500, 200), wxSize(600, 700));
    }
    m_pScanFileDialog->Show(true);
}

void CEditorMainFrame::GetPerformanceDialog()
{
    if (m_pPerformanceDialog == NULL)
    {
        m_pPerformanceDialog = new CEditPerformanceDialog(this, wxID_ANY, wxT("Performance"), wxPoint(100, 30), wxSize(1000, 700), wxDEFAULT_DIALOG_STYLE ^ wxMINIMIZE_BOX);
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
    case Menu_File_Undo:
        Undo();
        break;
    case Menu_File_Redo:
        Redo();
        break;
    case Menu_File_Export:
        CComponentProxyManager::GetInstance()->SetCheckUselessResourceValue(false);
        Export();
        break;
    case Menu_File_PackResource:
        CComponentProxyManager::GetInstance()->SetCheckUselessResourceValue(true);
        Export();
        ExportResourcePack();
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
            uint32_t uFileId = pProject->GetComponentFileId(strFileName);
            pProject->SetStartFile(uFileId);
            OnSetStartFile(item);
        }
        break;
    case ID_TB_Reopen:
        {
            TString strLastOpenProject = CEditorConfig::GetInstance()->GetLastOpenProject();
            bool bFileExists = !strLastOpenProject.empty() && CFilePathTool::GetInstance()->Exists(strLastOpenProject.c_str());
            if (bFileExists)
            {
                OpenProjectFile(strLastOpenProject.c_str());
                TString strFullPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
                TString strLastOpenFile = CEditorConfig::GetInstance()->GetLastOpenFile();
                strLastOpenFile = CFilePathTool::GetInstance()->MakeAbsolute(strFullPath.c_str(), strLastOpenFile.c_str());
                AddLastOpenFilesTreeCtrl();
                UpdateLastOpenFilesTreeCtrl();
                bool bOpenLastBCF = !strLastOpenFile.empty() && CFilePathTool::GetInstance()->Exists(strLastOpenFile.c_str()) && wxMessageBox(wxString::Format(_T("Open last BCF file with the project?\n%s"), strLastOpenFile.c_str()), _T("Reopen"), wxYES_NO) == wxYES;
                if (bOpenLastBCF)
                {
                    m_pComponentFileTreeCtrl->CollapseAll();
                    OpenComponentFile(strLastOpenFile.c_str(), NULL, false);
                }
                m_pToolBar->EnableTool(ID_TB_Reopen, false);
            }
        }
        break;
    case ID_ViewButton_Scene:
    case ID_ViewButton_UI:
    case ID_ViewButton_Ani:
    case ID_ViewButton_Terrain:
    case ID_ViewButton_Game:
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
            if (bViewAll)
            {
                CSelectResolutionDialog resolutionDialog(this, wxID_ANY, wxT("Select Resolution"), wxPoint(0, 0), wxSize(200, 200), wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);
                wxPoint pos = m_pToolBar->GetScreenPosition();
                wxRect rect = m_pToolBar->GetToolRect(ID_ViewAllBtn);
                pos.x += rect.x + rect.width;
                pos.y += rect.y + rect.height;
                resolutionDialog.SetPosition(pos);
                resolutionDialog.ShowModal();
            }
            else
            {
                m_pViewScreen->GetRenderWindow()->UseFBO(bViewAll);
                m_pViewScreen->SendSizeEvent(); // In order to call SetFBOViewPort, so force call a on size callback.
            }
        }
        break;
    case ID_CaptureBtn:
        {
            static const TCHAR* pszFilePath = _T("C:/TestPng.png");
            bool bCapture = m_pToolBar->GetToolToggled(ID_ViewAllBtn);
            if (bCapture)
            {
                bool bRet = CRenderManager::GetInstance()->SaveTextureToFile(CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetFBOTexture(), pszFilePath);
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
    case ID_GMBtn:
        {
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
    case ID_PauseApplication:
        {
            if (wxIsAltDown())
            {
                float dt = 0.016f;
                CEngineCenter::GetInstance()->Update(dt);
                CApplication::GetInstance()->Pause();
            }
            else
            {
                static uint32_t uCurrentSpeedPercent = 100;
                if (wxIsCtrlDown())
                {
                    uint32_t uRet = wxGetNumberFromUser("输入运行速度百分比", "%", "输入运行速度百分比", uCurrentSpeedPercent, 0, 1000);
                    if (uRet != 0xFFFFFFFF)
                    {
                        uCurrentSpeedPercent = uRet;
                    }
                }
                if (uCurrentSpeedPercent == 100)
                {
                    if (CApplication::GetInstance()->IsRunning())
                    {
                        CApplication::GetInstance()->Pause();
                        m_pToolBar->SetToolLabel(ID_PauseApplication, "Resume");
                    }
                    else
                    {
                        CApplication::GetInstance()->Resume();
                        m_pToolBar->SetToolLabel(ID_PauseApplication, "Pause");
                    }
                }
                else
                {
                    if (!CApplication::GetInstance()->IsRunning())
                    {
                        CApplication::GetInstance()->Resume();
                    }
                    bool bViewAll = m_pToolBar->GetToolToggled(ID_PauseApplication);
                    if (bViewAll)
                    {
                        CApplication::GetInstance()->m_fTimeScale = uCurrentSpeedPercent * 0.01f;
                    }
                    else
                    {
                        CApplication::GetInstance()->m_fTimeScale = 1.0f;
                    }
                    m_pToolBar->SetToolLabel(ID_PauseApplication, wxString::Format("x %d%%", uCurrentSpeedPercent));
                }
            }
        }
        break;
    case ID_SetCoordinateBtn:
        {
            wxPoint pos = m_pToolBar->GetScreenPosition();
            wxRect rect = m_pToolBar->GetToolRect(ID_SetCoordinateBtn);
            pos.x += rect.x + rect.width;
            pos.y += rect.y + rect.height;
            m_pSetCoordinateRenderObjectWnd->SetPosition(pos);
            m_pSetCoordinateRenderObjectWnd->Show(!m_pSetCoordinateRenderObjectWnd->IsShown());
        }
        break;
    case ID_UIEditMode:
    {
    }
        break;
    case ID_ReloadResource:
        CResourceManager::GetInstance()->Reload();
        break;
    default:
        GetCurrentViewAgent()->OnCommandEvent(event);
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

void CEditorMainFrame::OnActivateComponentFile(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTreeCtrl->GetItemData(itemId);
    BEATS_ASSERT(pData != NULL)
    if (pData->IsDirectory())
    {
        m_pComponentFileTreeCtrl->Toggle(itemId);
    }
    else
    {
        OpenComponentFileTreeClick(pData->GetFileName().c_str(), &itemId);
    }
}

void CEditorMainFrame::OnActivateLastFiles(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    for (auto iter : m_lastOpenFilesTreeItemIdMap)
    {
        if (iter.second == itemId)
        {
            OpenComponentFileTreeClick(iter.first.c_str(), &itemId);
            break;
        }
    }
}

void CEditorMainFrame::OpenComponentFileTreeClick(const TCHAR* pszFileName, wxTreeItemId* pItemId)
{
    bool bOpenFile = true;
    uint32_t uCurrViewId = CComponentProxyManager::GetInstance()->GetCurrentViewFileId();
    if (uCurrViewId != 0xFFFFFFFF)
    {
        const TString curViewFilePath = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(uCurrViewId);
        if (_tcsicmp(pszFileName, curViewFilePath.c_str()) == 0)
        {
            bOpenFile = false;
        }
    }
    if (bOpenFile)
    {
        SelectComponent(NULL);
        if (CFilePathTool::GetInstance()->Exists(pszFileName))
        {
            bool bNewAddThisFile = false;
            std::vector<uint32_t> loadFiles;
            std::vector<uint32_t> unloadFiles;
            uint32_t uFileId = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileId(pszFileName);
            BEATS_ASSERT(uFileId != 0xFFFFFFFF);
            CComponentProxyManager::GetInstance()->CalcSwitchFile(uFileId, loadFiles, unloadFiles, bNewAddThisFile);
            int iResult = wxNO;
            // If we are switching to an already open file
            if (unloadFiles.size() > 0)
            {
                iResult = wxMessageBox(_T("需要关闭当前文件吗？"), _T("关闭确认"), wxYES_NO | wxCANCEL);
            }
            if (iResult != wxCANCEL)
            {
                OpenComponentFile(pszFileName, pItemId, iResult == wxYES);
                CEditorConfig::GetInstance()->SetLastOpenFile(pszFileName);
                m_pComponentRenderWindow->GetCamera()->SetViewPos(CVec3(0, 0, 0));
            }
        }
        else
        {
            wxMessageBox(wxString::Format("文件不存在, 请检查路径\n%s", pszFileName));
        }
    }
}

void CEditorMainFrame::OnActivateComponentTemplate(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    CComponentTreeItemData* pData = (CComponentTreeItemData*)m_pComponentTemplateTreeCtrl->GetItemData(itemId);
    BEATS_ASSERT(pData != NULL);
    if (pData->IsDirectory())
    {
        m_pComponentTemplateTreeCtrl->Toggle(itemId);
    }
}

void CEditorMainFrame::OnComponentFileStartDrag(wxTreeEvent& event)
{
    CComponentFileTreeItemData* pData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(event.GetItem()));
    if (pData != NULL)
    {
        m_pComponentFileTreeCtrl->SelectItem(event.GetItem());
        event.Allow();
    }
}

void CEditorMainFrame::OnComponentFileEndDrag(wxTreeEvent& event)
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
                    uint32_t uFileId = pProject->GetComponentFileId(pDraggingItemData->GetFileName());
                    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
                    bool bRemoveFile = pDraggingItemParentData->GetProjectDirectory()->RemoveFile(uFileId);
                    BEYONDENGINE_UNUSED_PARAM(bRemoveFile);
                    BEATS_ASSERT(bRemoveFile, _T("Remove file %d failed!"), uFileId);
                    uint32_t uPreviousFileId = 0xFFFFFFFF;
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
                    uint32_t uStartFileId = pProject->GetStartFile();
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

void CEditorMainFrame::OnComponentStartDrag(wxTreeEvent& event)
{
    GetCurrentViewAgent()->OnComponentStartDrag();
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

void CEditorMainFrame::OnComponentEndDrag(wxTreeEvent& event)
{
    GetCurrentViewAgent()->OnComponentEndDrag();
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
    if (pComponentInstance != m_pSelectedComponentProxy)
    {
        m_pPropGridManager->ClearPage(0);
        m_pSelectedComponentProxy = pComponentInstance;
        GetCurrentViewAgent()->SelectComponent(pComponentInstance);
        if (pComponentInstance != NULL)
        {
            // If we select an instance(not a template).
            if (pComponentInstance->GetId() != -1)
            {
                m_pComponentFileTreeCtrl->Unselect();

                int x = 0;
                int y = 0;
                pComponentInstance->GetGraphics()->GetPosition(&x, &y);
                CVec2 worldPos;
                m_pComponentRenderWindow->ConvertGridPosToWorldPos(x, y, &worldPos.X(), &worldPos.Y());

                CCamera* pComponentWndCamera = m_pComponentRenderWindow->GetCamera();
                const CVec3& viewPos = pComponentWndCamera->GetViewPos();
                CVec2 offset = pComponentWndCamera->GetCenterOffset();
                bool bOutOfHorizontal = worldPos.X() < viewPos.X() - fabs(offset.X()) || worldPos.X() > viewPos.X() + fabs(offset.X());
                bool bOutOfVertical = worldPos.Y() < viewPos.Y() - fabs(offset.Y()) || worldPos.Y() > viewPos.Y() + fabs(offset.Y());
                if (bOutOfHorizontal || bOutOfVertical)
                {
                    pComponentWndCamera->SetViewPos(CVec3(worldPos.X(), worldPos.Y(), 0));
                }
            }
            m_pPropGridManager->InsertComponentsInPropertyGrid(pComponentInstance);
        }
    }
}

CComponentProxy* CEditorMainFrame::GetSelectedComponent()
{
    return m_pSelectedComponentProxy;
}

void CEditorMainFrame::ResolveIdConflict(const std::map<uint32_t, std::vector<uint32_t>>& conflictIdMap)
{
    if (conflictIdMap.size() > 0)
    {
        TCHAR szConflictInfo[10240];
        _stprintf(szConflictInfo, _T("工程中有%d个ID发生冲突，点击\"是\"开始解决,点击\"否\"退出."), conflictIdMap.size());
        int iRet = wxMessageBox(szConflictInfo, _T("解决ID冲突"), wxYES_NO);
        if (iRet == wxYES)
        {
            CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
            uint32_t lAnswer = 0;
            std::map<uint32_t, std::vector<uint32_t>>::const_iterator iter = conflictIdMap.begin();
            for (; iter != conflictIdMap.end(); ++iter)
            {
                _stprintf(szConflictInfo, _T("Id为%d的组件出现于文件:\n"), iter->first);
                for (uint32_t i = 0; i < iter->second.size(); ++i)
                {
                    TCHAR szFileName[1024];
                    _stprintf(szFileName, _T("%d.%s\n"), i, pProject->GetComponentFileName(iter->second[i]).c_str());
                    _tcscat(szConflictInfo, szFileName);
                }
                _tcscat(szConflictInfo, _T("需要保留ID的文件序号是(填-1表示全部分配新ID):"));
                lAnswer = wxGetNumberFromUser(szConflictInfo, _T("promote"), _T("请填入序号"), lAnswer, -1, iter->second.size() - 1);
                for (uint32_t i = 0; i < iter->second.size(); ++i)
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

void CEditorMainFrame::OpenProjectFile(const TCHAR* pPath)
{
    if (pPath != NULL && _tcslen(pPath) > 0 && CFilePathTool::GetInstance()->Exists(pPath))
    {
        CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
        CWaitingForProcessDialog* pWaidtinDialog = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWaitingDialog();
        pWaidtinDialog->SetTitle(_T("正在打开"));
        pWaidtinDialog->ShowUseTime(false);
        pWaidtinDialog->SetTotalCount(100);
        pWaidtinDialog->SetTask([=]()
        {
            TString strFile;
            uint32_t progress = pProject->GetLoadProjectProgress(strFile);
            pWaidtinDialog->SetFileName(_T(strFile));
            return progress;
        });
        std::map<uint32_t, std::vector<uint32_t>> conflictIdMap;
        CComponentProjectDirectory* pProjectData = NULL;
        std::thread t1([=, &conflictIdMap, &pProjectData](){
            pProjectData = pProject->LoadProject(pPath, conflictIdMap);
        });
        t1.detach();
        pWaidtinDialog->ShowModal();

        bool bEmptyProject = pProjectData == NULL;
        if (!bEmptyProject)
        {
            m_pFileMenu->FindItem(Menu_File_Export)->Enable(true);
            m_pFileMenu->FindItem(Menu_File_PackResource)->Enable(true);
            CEditorConfig::GetInstance()->SetLastOpenProject(pPath);
            CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTreeCtrl->GetItemData(m_pComponentFileTreeCtrl->GetRootItem());
            if (pData)
            {
                BEATS_SAFE_DELETE(pData)
            }
            InitializeComponentFileTree(pProjectData, m_pComponentFileTreeCtrl->GetRootItem());
            uint32_t uStartFileId = pProject->GetStartFile();
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
    m_exportFileFullPathList.clear();
    m_pFileMenu->FindItem(Menu_File_Export)->Enable(false);
    m_pFileMenu->FindItem(Menu_File_PackResource)->Enable(false);
    SelectComponent(NULL);
    if (CComponentProxyManager::GetInstance()->GetCurrLoadFileId() != 0xFFFFFFFF)
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
    m_exportFileFullPathList.clear();
    SaveProject();
    TString szBinaryPath;
    CUtilityManager::GetInstance()->AcquireSingleFilePath(false, NULL, szBinaryPath, _T("选择要导出的文件"), BINARIZE_FILE_EXTENSION_FILTER, NULL);
    if (szBinaryPath.length() > 0)
    {
        TString strExtensionStr = CFilePathTool::GetInstance()->Extension(szBinaryPath.c_str());
        if (strExtensionStr.compare(BINARIZE_FILE_EXTENSION) != 0)
        {
            szBinaryPath.append(BINARIZE_FILE_EXTENSION);
        }
        CWaitingForProcessDialog* pWaidtinDialog = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWaitingDialog();
        pWaidtinDialog->SetTitle(_T("正在导出"));
        pWaidtinDialog->ShowUseTime(true);
        pWaidtinDialog->SetTotalCount(100);
        pWaidtinDialog->SetTask([=]()
        {
            TString strFile;
            uint32_t progress = CComponentProxyManager::GetInstance()->GetOperateProgress(strFile);
            pWaidtinDialog->SetFileName(strFile);
            return progress;
        });
        std::function<void()> pExportFunc = [=]()
        {
            CComponentProxyManager::GetInstance()->Export(szBinaryPath.c_str(), [=](uint32_t uFileID, CComponentProxy* pProxy)
            {
                BEYONDENGINE_UNUSED_PARAM(uFileID);
                BEYONDENGINE_UNUSED_PARAM(pProxy);
                return false;
            });
        };
        std::thread t1([=](){
#ifndef _DEBUG
            __try
            {
#endif
                pExportFunc();
#ifndef _DEBUG
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                MessageBox(BEYONDENGINE_HWND, "导出遇到未知错误，请立刻联系研发部门寻找原因！", "导出失败", MB_OK);
                exit(0);
            }
#endif
        });
        t1.detach();
        pWaidtinDialog->ShowModal();
        //Export function will cause these operation: open->export->close->change file->open->...->restore open the origin file.
        //So we will update the dependency line as if we have just open a new file.
        m_pComponentRenderWindow->UpdateAllDependencyLine();

        ExportUINodeHeadFile();
        wxMessageBox(wxString::Format(_T("导出完毕!\n耗时%d秒"), pWaidtinDialog->GetToatalTime()));
        if (CComponentProxyManager::GetInstance()->IsCheckUselessResource())
        {
            ShowScanFilesDialog();
        }
    }
}

void CEditorMainFrame::ExportUINodeHeadFile()
{
}

void CEditorMainFrame::SetConvertPngTypeString(const TString& strType)
{
    m_strConverPNGType = strType;
}

void CEditorMainFrame::CreatePathDir(TString& strPath)
{
    if (!wxDirExists(strPath))
    {
        wxFileName fn(strPath);
        TString path = fn.GetPath(wxPATH_GET_VOLUME | wxPATH_NO_SEPARATOR);
        CreatePathDir(path);
        wxString strPathLower = strPath;
        strPathLower.MakeLower();
        wxMkdir(strPathLower);
    }
}

void CEditorMainFrame::ExportResourcePack(bool bConsoleMode)
{
    TString strCurrentPath = wxGetCwd();
    TString strResourcePath = strCurrentPath + _T("\\..\\Resource");
    TString strBCFResourcePath = strResourcePath + _T("\\bcf");
    TString strTexturePath = strResourcePath + _T("\\Texture");
    TString strETCPath = strTexturePath + _T("\\ETC_temp");
    TString strPVRPath = strTexturePath + _T("\\PVR");
    TString strETCTexturePath = strETCPath + _T("\\Texture");
    TString strPVRTexturePath = strPVRPath + _T("\\Texture");

    TString strAndroidAssetsPath = strCurrentPath + _T("\\..\\Sample\\AndroidProject\\Sample\\Assets");
    TString strAndroidAssetsResourcePath = strAndroidAssetsPath + _T("\\Resource");
    TString strETCProcessRecordList = strETCPath + _T("\\info.bin");
    TString strPVRProcessRecordList = strPVRPath + _T("\\info.bin");
    TString strVersionInfoFile = strCurrentPath + _T("\\..\\VersionInfo.bin");

    TString strFilePath = strResourcePath + _T("\\copyfiles.txt");
    std::vector<std::string> copyFiles;
    char strFile[MAX_PATH];
    std::ifstream stream(strFilePath.c_str());
    if (stream.is_open())
    {
        while (!stream.eof()) {
            stream.getline(strFile, MAX_PATH);
            copyFiles.push_back(strFile);
        }
    }
    BEATS_ASSERT(!copyFiles.empty());

    wxArrayString files;
    if (bConsoleMode || GenerateVersionInfo())
    {
        AllocConsole();
        ShowWindow(GetConsoleWindow(), SW_HIDE);
        if (wxDirExists(strAndroidAssetsPath))
        {
            wxDir::Remove(strAndroidAssetsPath, wxPATH_RMDIR_RECURSIVE);
        }

        for (uint32_t n = 0; n < (uint32_t)copyFiles.size(); n++)
        {
            TString strCurrentResourcePath = strResourcePath + "\\" + copyFiles[n].c_str();
            files.clear();
            if (CFilePathTool::GetInstance()->IsDirectory(strCurrentResourcePath.c_str()))
            {
                wxDir::GetAllFiles(strCurrentResourcePath, &files);
                for (size_t i = 0; i < files.size(); i++)
                {
                    wxFileName fn(files[i]);
                    TString path = fn.GetPath(wxPATH_GET_VOLUME | wxPATH_NO_SEPARATOR);
                    if (path.find(strBCFResourcePath) == -1 && path.find(strTexturePath) == -1)
                    {
                        path.replace(path.find(strCurrentResourcePath), strCurrentResourcePath.size(), strAndroidAssetsResourcePath + "\\" + copyFiles[n]);
                        CreatePathDir(path);
                        wxString copyFile = path + _T("\\") + fn.GetFullName();
                        copyFile.LowerCase();
                        wxCopyFile(files[i], copyFile);
                    }
                }
            }
            else
            {
                wxString copyFile = strAndroidAssetsResourcePath + _T("\\") + copyFiles[n];
                copyFile.LowerCase();
                wxCopyFile(strCurrentResourcePath, copyFile);
            }
        }
        
        TString strVersionInfoFileCopyPath = strAndroidAssetsPath + _T("\\VersionInfo.bin");
        wxCopyFile(strVersionInfoFile, strVersionInfoFileCopyPath);

        CollectPngFiles();
        TString strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
        strWorkingPath.append(_T("\\")).append(COMPRESS_TEXTURE_CONFIG_NAME);
        m_compressTextureSet.clear();
        CSerializer configData;
        if (CFilePathTool::GetInstance()->LoadFile(&configData, strWorkingPath.c_str(), "rt") && configData.GetWritePos() > 0)
        {
            TCHAR szBuffer[10240];
            configData.Deserialize(szBuffer, configData.GetWritePos());
            szBuffer[configData.GetWritePos()] = 0;
            BEATS_ASSERT(configData.GetReadPos() == configData.GetWritePos());
            std::vector<std::string> fileList;
            CStringHelper::GetInstance()->SplitString(szBuffer, "\n", fileList, true);
            for (size_t i = 0; i < fileList.size(); ++i)
            {
                TString strFileName = fileList[i];
                strFileName = strFileName.substr(0, strFileName.rfind(_T("."))) + _T(".png");
                strFileName = strTexturePath + "\\" + strFileName;
                if (CFilePathTool::GetInstance()->Exists(strFileName.c_str()))
                {
                    strFileName = CFilePathTool::GetInstance()->FileName(strFileName.c_str());
                    m_compressTextureSet.insert(strFileName);
                }
            }
        }

        TString strTexturePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Texture);
        wxDir textureDir(strTexturePath);
        wxArrayString arrayFileName;
        wxArrayString arrayXmlFileName;
        textureDir.GetAllFiles(strTexturePath, &arrayFileName, wxEmptyString, wxDIR_FILES);
        textureDir.GetAllFiles(strTexturePath, &arrayXmlFileName, _T("*.xml"), wxDIR_FILES);
        for (auto iter : arrayFileName)
        {
            TString strFileName = iter;
            iter = iter.substr(0, iter.find(_T("."))) + _T(".xml");
            bool bFoundFile = false;
            for (uint32_t uIndex = 0; uIndex < arrayXmlFileName.size(); ++uIndex)
            {
                if (arrayXmlFileName[uIndex] == iter)
                {
                    bFoundFile = true;
                    break;
                }
            }
            if (!bFoundFile)
            {
                strFileName = CFilePathTool::GetInstance()->FileName(strFileName.c_str());
                m_compressTextureSet.insert(strFileName);
            }
        }

        StartConvertPng();
        FreeConsole();
        for (auto itr : m_oldETCFileMd5Map)
        {
            wxFileName fn(itr.first);
            TString strEtcSource = strETCTexturePath + _T("\\") + fn.GetFullName();
            if (wxFileExists(strEtcSource))
            {
                wxRemoveFile(strEtcSource);
            }
        }

        for (auto itr : m_oldPVRFileMd5Map)
        {
            wxFileName fn(itr.first);
            TString strPvrSource = strPVRTexturePath + _T("\\") + fn.GetFullName();
            if (wxFileExists(strPvrSource))
            {
                wxRemoveFile(strPvrSource);
            }
        }

        TCHAR szCanonicalPath[MAX_PATH];
        if (m_strConverPNGType == "1")
        {
            CSerializer infoSerializer;
            size_t uMapSize = m_ETCFileMd5Map.size();
            infoSerializer << uMapSize;
            for (auto itr : m_ETCFileMd5Map)
            {
                infoSerializer << itr.first;
                infoSerializer << itr.second;
            }

            CFilePathTool::GetInstance()->Canonical(szCanonicalPath, strETCProcessRecordList.c_str());
            infoSerializer.Deserialize(szCanonicalPath, _T("wb+"));
            infoSerializer.Reset();
        }

        if (m_strConverPNGType == "2")
        {
            CSerializer infoSerializer;
            size_t uMapSize = m_PVRFileMd5Map.size();
            infoSerializer << uMapSize;
            for (auto itr : m_PVRFileMd5Map)
            {
                infoSerializer << itr.first;
                infoSerializer << itr.second;
            }
            CFilePathTool::GetInstance()->Canonical(szCanonicalPath, strPVRProcessRecordList.c_str());
            infoSerializer.Deserialize(szCanonicalPath, _T("wb+"));
            infoSerializer.Reset();
        }

        if (m_strConverPNGType == "1")
        {
            files.Clear();
            wxDir::GetAllFiles(strETCTexturePath, &files);
            CopyEtcAndPvrFile(files, strETCPath, strAndroidAssetsResourcePath, "etc_texture");
            EncryptXMLFiles(_T("\\texture"));
        }
        if (m_strConverPNGType == "2")
        {
            files.Clear();
            wxDir::GetAllFiles(strPVRPath, &files);
            CopyEtcAndPvrFile(files, strPVRPath, strAndroidAssetsResourcePath, "pvr_texture");
            EncryptXMLFiles(_T("\\texture"));
        }
        CWaitingForProcessDialog* pWaidtinDialog = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWaitingDialog();
        pWaidtinDialog->SetTask([this]()
        {
            return m_nTaskCurCount;
        });
        std::thread t1(&CEditorMainFrame::GenerateResourceList, this);
        t1.detach();
        pWaidtinDialog->ShowModal();
        if (!bConsoleMode)
        {
            RecordPackPngFileSize();
            wxMessageBox(_T("打包完毕!"));
            TString strResourceCompressLog = _T("notepad ") + strCurrentPath + _T("\\..\\ResourceCompressLog.txt");
            _tsystem(strResourceCompressLog.c_str());
        }
    }
}

void CEditorMainFrame::CopyEtcAndPvrFile(wxArrayString& files, TString& strPath, TString& strAndroidAssetsResourcePath, const TCHAR*)
{
    for (size_t i = 0; i < files.size(); i++)
    {
        wxFileName fn(files[i]);
        TString path = fn.GetPath(wxPATH_GET_VOLUME | wxPATH_NO_SEPARATOR);
        {
            path.replace(path.find(strPath), strPath.size(), strAndroidAssetsResourcePath);
            std::transform(path.begin(), path.end(), path.begin(), tolower);
            CreatePathDir(path);
            wxString copyFile = path + _T("\\") + fn.GetFullName();
            copyFile.MakeLower();
            wxCopyFile(files[i], copyFile);
        }
    }
}

void CEditorMainFrame::SaveProject()
{
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    if (pProject->GetRootDirectory() != NULL)
    {
        // Save Instance File
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
    wxMessageBox(_T("Save Successed!"), _T("Save"), wxOK);
}


void CEditorMainFrame::Undo()
{
    COperationRecordManager::GetInstance()->UndoRecord();
}

void CEditorMainFrame::Redo()
{
    COperationRecordManager::GetInstance()->DoRecord();
}

void CEditorMainFrame::AddLastOpenFilesTreeCtrl()
{
    uint32_t uPageCount = m_pLeftBook->GetPageCount();
    int iLastOpenFilesPageIndex = -1;
    for (uint32_t i = 0; i < uPageCount; ++i)
    {
        if (m_pLeftBook->GetPage(i) == m_pLastOpenFilesTreeCtrl)
        {
            iLastOpenFilesPageIndex = i;
            break;
        }
    }
    if (iLastOpenFilesPageIndex == -1)
    {
        m_pLeftBook->InsertPage(1, m_pLastOpenFilesTreeCtrl, L10N_T(eLTT_Editor_Common_Last_Open_Files), true);
    }
    m_pLeftBook->SetSelection(0);
}

void CEditorMainFrame::OpenComponentFile(const TCHAR* pFilePath, wxTreeItemId* pItemId, bool bCloseLoadedFile)
{
    BEATS_ASSERT(pFilePath != NULL && pFilePath[0] != 0, _T("Invalid file path"));
    TString strFileLower = CStringHelper::GetInstance()->ToLower(pFilePath);
    if (!m_Manager.GetPane(m_pResourcePanel).IsShown())
    {
        m_Manager.GetPane(m_pResourcePanel).Show();
        m_Manager.Update();
    }
    CComponentProxyManager::GetInstance()->OpenFile(strFileLower.c_str(), bCloseLoadedFile);
    uint32_t uPageCount = m_pLeftBook->GetPageCount();
    int iComponentListPageIndex = -1;
    for (uint32_t i = 0; i < uPageCount; ++i)
    {
        if (m_pLeftBook->GetPage(i) == m_pComponentProxyTreeCtrl)
        {
            iComponentListPageIndex = i;
            break;
        }
    }
    if (iComponentListPageIndex == -1)
    {
        m_pLeftBook->InsertPage(uPageCount, m_pComponentProxyTreeCtrl, L10N_T(eLTT_Editor_Common_Component), true);
    }
    m_pLeftBook->SetSelection(0);
    UpdateComponentInstanceTreeCtrl();
    UpdateLastOpenFilesList(pFilePath);
    UpdateLastOpenFilesTreeCtrl();
    GetCurrentViewAgent()->OnOpenComponentFile();
    m_pComponentRenderWindow->UpdateAllDependencyLine();
    if (pItemId == NULL)
    {
        auto iter = m_componentFileListMap.find(strFileLower.c_str());
        BEATS_ASSERT(iter != m_componentFileListMap.end());
        if ((iter != m_componentFileListMap.end()))
        {
            pItemId = &iter->second;
        }
    }
    BEATS_ASSERT(pItemId != NULL, _T("Can't find the file %s we are trying to open!"), strFileLower.c_str());
    if (pItemId != NULL)
    {
        m_itemTextcolor = m_pComponentFileTreeCtrl->GetItemTextColour(*pItemId);
        if (m_activeFileItemId)
        {
            m_pComponentFileTreeCtrl->SetItemBackgroundColour(m_activeFileItemId, m_itemTextcolor);
        }
        m_pComponentFileTreeCtrl->SetItemBackgroundColour(*pItemId, wxColor(150, 33, 184));
        m_pComponentFileTreeCtrl->EnsureVisible(*pItemId);
        m_pCenterBook->SetPageText(0, strFileLower.substr(strFileLower.rfind(_T("\\")) + 1));
        m_activeFileItemId = *pItemId;
    }
    CResourceManager::GetInstance()->CleanUp();
}

void CEditorMainFrame::CloseComponentFile(bool bRemindSave /*= true*/)
{
    CComponentProxyManager* pComponentProxyManager = CComponentProxyManager::GetInstance();
    uint32_t uFileId = pComponentProxyManager->GetCurrLoadFileId();
    if (uFileId != 0xFFFFFFFF)
    {
        const TString& strCurWorkingFile = pComponentProxyManager->GetProject()->GetComponentFileName(uFileId);
        if (bRemindSave)
        {
            int iRet = wxMessageBox(wxString::Format(_T("是否要保存当前文件?\n%s"), strCurWorkingFile.c_str()), _T("保存文件"), wxYES_NO);
            if (iRet == wxYES)
            {
                pComponentProxyManager->SaveCurFile();
            }
        }
        pComponentProxyManager->CloseFile(uFileId);
    }
    HideTreeCtrl(m_pComponentProxyTreeCtrl);
    GetCurrentViewAgent()->OnCloseComponentFile();
}

void CEditorMainFrame::OpenProject()
{
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    TString strProjectFullPath = pProject->GetProjectFilePath();

    if (strProjectFullPath.length() == 0)
    {
        TString strWorkingPath = CResourceManager::GetInstance()->GetResourcePath(eRT_Resource);
        strWorkingPath.append("\\bcf");
        CUtilityManager::GetInstance()->AcquireSingleFilePath(false, this->GetHWND(), strProjectFullPath, _T("选择要读取的文件"), COMPONENT_PROJECT_EXTENSION, strWorkingPath.c_str());
        if (strProjectFullPath.length() > 0)
        {
            std::transform(strProjectFullPath.begin(), strProjectFullPath.end(), strProjectFullPath.begin(), tolower);
            OpenProjectFile(strProjectFullPath.c_str());
            AddLastOpenFilesTreeCtrl();
            UpdateLastOpenFilesTreeCtrl();
        }
    }
    else
    {
        CloseProjectFile();
    }
}

void CEditorMainFrame::CloseProject()
{
    CSceneManager::GetInstance()->RemoveScene(CSceneManager::GetInstance()->GetCurrentScene());
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

void CEditorMainFrame::OnTreeCtrlRightClick(wxTreeEvent& event)
{
    int id = event.GetId();
    switch (id)
    {
    case Ctrl_Tree_CompontentFile:
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
        GetCurrentViewAgent()->OnTreeCtrlRightClick(event);
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
            TString strExtensionStr = CFilePathTool::GetInstance()->Extension(result.c_str());
            if (strExtensionStr.compare(COMPONENT_FILE_EXTENSION) != 0)
            {
                result.append(COMPONENT_FILE_EXTENSION);
            }
            std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.find(result);
            if (iter != m_componentFileListMap.end())
            {
                int iRet = wxMessageBox(_T("该文件已经存在于项目之中！不能重复添加！是否跳转到该文件？"), _T("文件已存在"), wxYES_NO);
                if (iRet == wxYES)
                {
                    m_pComponentFileTreeCtrl->SelectItem(iter->second);
                }
            }
            else if (CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileId(result) != 0xFFFFFFFF)
            {
                int iRet = wxMessageBox(_T("项目中已经有同名文件，是否跳转到该文件？"), _T("文件名重复"), wxYES_NO);
                if (iRet == wxYES)
                {
                    uint32_t uFileId = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileId(result);
                    TString strFileName = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(uFileId);
                    BEATS_ASSERT(m_componentFileListMap.find(strFileName) != m_componentFileListMap.end());
                    m_pComponentFileTreeCtrl->SelectItem(m_componentFileListMap[strFileName]);
                }
            }
            else
            {
                if (!CFilePathTool::GetInstance()->Exists(result.c_str()))
                {
                    rapidxml::xml_document<> doc;
                    rapidxml::xml_node<>* pDecl = doc.allocate_node(rapidxml::node_declaration);
                    rapidxml::xml_attribute<>* pDecl_ver = doc.allocate_attribute("version", "1.0");
                    pDecl->append_attribute(pDecl_ver);
                    doc.append_node(pDecl);
                    rapidxml::xml_node<>* pRootElement = doc.allocate_node(rapidxml::node_element, "Root");
                    doc.append_node(pRootElement);
                    // TCHAR to char trick.
                    TString strOut;
                    rapidxml::print(std::back_inserter(strOut), doc, 0);
                    std::ofstream out(result.c_str());
                    out << strOut;
                    out.close();
                }
                CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(NULL, result);
                wxString fileName = wxFileNameFromPath(result.c_str());
                wxTreeItemId newItemId = m_pComponentFileTreeCtrl->AppendItem(item, fileName, CEnginePropertyGridManager::eTCIT_File, -1, pData);
                m_pComponentFileTreeCtrl->Expand(item);
                m_componentFileListMap[result] = newItemId;

                // When Add a new file in project, we must validate the id in this file. if id is conflicted, we have to resolve it.
                CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeCtrl->GetItemData(item));
                BEATS_ASSERT(pCurItemData != NULL && pCurItemData->IsDirectory());
                std::map<uint32_t, std::vector<uint32_t>> conflictMap;
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
            m_pComponentFileTreeCtrl->AppendItem(item, strNewName, CEnginePropertyGridManager::eTCIT_Folder, -1, pData);
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
            uint32_t uFileId = pProject->GetComponentFileId(pCurItemData->GetFileName());
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
        uint32_t uFileId = CComponentProxyManager::GetInstance()->GetCurrLoadFileId();
        CComponentProxyManager::GetInstance()->CloseFile(uFileId);
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

void CEditorMainFrame::DeleteItemInComponentFileList(wxTreeItemId itemId, bool bDeletePhysicalFile)
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

void CEditorMainFrame::OnSelectComponentTemplateTreeItem(wxTreeEvent& /*event*/)
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

void CEditorMainFrame::InitComponentsPage()
{
    CComponentProxyManager::GetInstance()->InitializeAllTemplate();
    const std::map<uint32_t, CComponentBase*>* pComponentsMap = CComponentProxyManager::GetInstance()->GetComponentTemplateMap();
    for (std::map<uint32_t, CComponentBase*>::const_iterator componentIter = pComponentsMap->begin(); componentIter != pComponentsMap->end(); ++componentIter)
    {
        CComponentProxy* pComponent = static_cast<CComponentProxy*>(componentIter->second);
        const TString& catalogName = pComponent->GetCatalogName();
        if (!catalogName.empty())
        {
            std::map<TString, wxTreeItemId>::iterator iter = m_componentCatalogNameMap.find(catalogName);
            //Build catalog
            if (iter == m_componentCatalogNameMap.end())
            {
                std::vector<TString> result;
                CStringHelper::GetInstance()->SplitString(catalogName.c_str(), _T("\\"), result);
                BEATS_ASSERT(result.size() > 0);
                TString findStr;
                wxTreeItemId parentId = m_componentTreeIdMap[0];
                for (uint32_t i = 0; i < result.size(); ++i)
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
                        parentId = m_pComponentTemplateTreeCtrl->AppendItem(parentId, wxString::FromUTF8(result[i].c_str()), eTCIT_Folder, -1, pComponentItemData);
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
            uint32_t guid = pComponent->GetGuid();
            CComponentTreeItemData* pComponentItemData = new CComponentTreeItemData(false, guid);
            m_componentTreeIdMap[guid] = m_pComponentTemplateTreeCtrl->AppendItem(iter->second, wxString::FromUTF8(pComponent->GetDisplayName().c_str()), eTCIT_File, -1, pComponentItemData);
        }
    }
    m_pComponentTemplateTreeCtrl->Expand(m_pComponentTemplateTreeCtrl->GetRootItem());
}

void CEditorMainFrame::InitializeComponentFileTree(CComponentProjectDirectory* pProjectData, const wxTreeItemId& id)
{
    CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(pProjectData, pProjectData->GetName());
    m_pComponentFileTreeCtrl->SetItemData(id, pData);
    const std::vector<CComponentProjectDirectory*>& children = pProjectData->GetChildren();
    for (std::vector<CComponentProjectDirectory*>::const_iterator iter = children.begin(); iter != children.end(); ++iter)
    {
        const TString& nameStr = (*iter)->GetName();
        wxTreeItemId newDirectoryId = m_pComponentFileTreeCtrl->AppendItem(id, wxString::FromUTF8(nameStr.c_str()), eTCIT_Folder, -1, NULL);
        InitializeComponentFileTree(*iter, newDirectoryId);
    }

    const std::vector<uint32_t>& files = pProjectData->GetFileList();
    for (std::vector<uint32_t>::const_iterator iter = files.begin(); iter != files.end(); ++iter)
    {
        TString strComopnentFileName = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(*iter);
        CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(NULL, strComopnentFileName);
        wxString pFileName = wxFileNameFromPath(strComopnentFileName);
        wxTreeItemId newFileId = m_pComponentFileTreeCtrl->AppendItem(id, wxString::FromUTF8(pFileName), eTCIT_File, -1, pData);
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
    SetStatusText(L10N_T(eLTT_Editor_TextInfo_Welcome));
    SetMinSize(wxSize(MINWINDOWSIZE, MINWINDOWSIZE));

    SelectViewID(ID_ViewButton_Scene);
    Centre();

    ELanguageType currLanguage = CEditorConfig::GetInstance()->GetCurrLanguage();
    LanguageSwitch(currLanguage);
}

void CEditorMainFrame::LanguageSwitch(ELanguageType language, bool bForceLoad)
{
    CLanguageManager::GetInstance()->SetCurrentLanguage(language, bForceLoad);
    RefreshLanguage();
}

void CEditorMainFrame::RefreshLanguage()
{
    m_pMenuBar->SetMenuLabel(eMB_File, L10N_T(eLTT_Editor_Common_File));
    m_pMenuBar->SetMenuLabel(eMB_Edit, L10N_T(eLTT_Editor_Common_Edit));
    m_pMenuBar->SetMenuLabel(eMB_Window, L10N_T(eLTT_Editor_Common_Window));
    m_pMenuBar->SetMenuLabel(eMB_Help, L10N_T(eLTT_Editor_Common_Help));

    m_pFileMenu->SetLabel(Menu_File_OpenProject, L10N_T(eLTT_Editor_Common_Open));
    m_pFileMenu->SetLabel(Menu_File_CloseProject, L10N_T(eLTT_Editor_Common_Close));
    TCHAR szBuffer[128];
    _stprintf(szBuffer, _T("%s\tCtrl-S"), L10N_T(eLTT_Editor_Common_Save).c_str());
    m_pFileMenu->SetLabel(Menu_File_SaveProject, szBuffer);
    m_pFileMenu->SetLabel(Menu_File_Export, L10N_T(eLTT_Editor_Common_Export));

    m_pEditMenu->SetLabel(Menu_Open_TexturePreview, L10N_T(eLTT_Editor_Menu_ViewTexture));
    m_pEditMenu->SetLabel(Menu_Edit_Language, L10N_T(eLTT_Editor_Common_Language));

    m_pHelpMenu->SetLabel(Menu_Help_Language, L10N_T(eLTT_Editor_Common_Language));
    m_pHelpMenu->SetLabel(Menu_Help_About, L10N_T(eLTT_Editor_Menu_About));
    m_pHelpMenu->SetLabel(Menu_Help_Language_Chinese, L10N_T(eLTT_Editor_Common_Chinese));
    m_pHelpMenu->SetLabel(Menu_Help_Language_English, L10N_T(eLTT_Editor_Common_English));

    m_pLeftBook->SetPageText(0, L10N_T(eLTT_Editor_Common_File));
    m_pLeftBook->SetPageText(1, L10N_T(eLTT_Editor_Common_Component));
    if (!m_activeFileItemId)
    {
        m_pCenterBook->SetPageText(0, L10N_T(eLTT_Editor_Common_View));
    }
    m_pShowGUIDCheckBox->SetLabel(L10N_T(eLTT_Editor_Component_ShowGuidId));
    m_pRightBook->SetPageText(0, L10N_T(eLTT_Editor_Common_Property));
    m_pBottomBook->SetPageText(0, L10N_T(eLTT_Editor_Common_View));
    m_pBottomBook->SetPageText(1, L10N_T(eLTT_Editor_Common_View));
    m_pToolBar->SetToolLabel(ID_TB_Reopen, L10N_T(eLTT_Editor_Menu_Reopen));
    m_pToolBar->SetToolLabel(ID_TB_PerformBtn, L10N_T(eLTT_Editor_Menu_Performance));
    m_pToolBar->SetToolLabel(ID_CameraBtn, L10N_T(eLTT_Editor_Menu_Camera));
    m_pToolBar->SetToolLabel(ID_ViewAllBtn, L10N_T(eLTT_Editor_Menu_SimulateView));
    m_pToolBar->SetToolLabel(ID_CaptureBtn, L10N_T(eLTT_Editor_Menu_Capture));
    m_pToolBar->SetToolLabel(ID_SettingBtn, L10N_T(eLTT_Editor_Common_Setting));
    m_pToolBar->SetToolLabel(ID_ComponentInfoBtn, L10N_T(eLTT_Editor_Menu_Component));
    m_pToolBar->SetToolLabel(ID_BatchEditBtn, L10N_T(eLTT_Editor_Menu_BatchEdit));
    m_pToolBar->SetToolLabel(ID_PauseApplication, L10N_T(eLTT_Editor_Common_Pause));

    m_pResourceBook->SetPageText(0, L10N_T(eLTT_Editor_Common_Component));
    m_pResourceBook->SetPageText(1, L10N_T(eLTT_Editor_Common_Resource));
    m_pSceneViewBtn->SetLabelText(L10N_T(eLTT_Editor_Menu_Scene));
    m_pUIViewBtn->SetLabelText(L10N_T(eLTT_Editor_Menu_UI));
    m_pAniViewBtn->SetLabelText(L10N_T(eLTT_Editor_Menu_AnimationView));
    m_pTerrainViewBtn->SetLabelText(L10N_T(eLTT_Editor_Common_Terrain));
    m_pGameViewBtn->SetLabelText(L10N_T(eLTT_Editor_Menu_GameView));

    m_pToolBar->Refresh(false);

    if (m_pPerformanceDialog != NULL)
    {
        m_pPerformanceDialog->LanguageSwitch();
    }
    SetStatusText(L10N_T(eLTT_Editor_TextInfo_Welcome));
}

void CEditorMainFrame::OnSearchTextUpdate(wxCommandEvent& /*event*/)
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
        wxString szText = m_pSearch->GetValue().Lower();
        pTreeCtrl->CollapseAll();
        if (pTreeCtrl == m_pComponentFileTreeCtrl)
        {
            for (auto iter = m_componentFileListMap.begin(); iter != m_componentFileListMap.end(); ++iter)
            {
                wxString lableText = pTreeCtrl->GetItemText(iter->second).Lower();
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
            for (std::map<uint32_t, wxTreeItemId>::iterator iter = m_componentTreeIdMap.begin(); iter != m_componentTreeIdMap.end(); ++iter)
            {
                wxString lableText = pTreeCtrl->GetItemText(iter->second).Lower();
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

void CEditorMainFrame::SetChildWindowStyle(CEditDialogBase* pDlg)
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

void CEditorMainFrame::OnTreeCtrlExpanded(wxTreeEvent& event)
{
    wxTreeCtrl* pTreeCtrl = (wxTreeCtrl*)event.GetEventObject();
    pTreeCtrl->SetItemImage(event.GetItem(), eTCIT_FileSelected);
}

void CEditorMainFrame::OnTreeCtrlCollapsed(wxTreeEvent& event)
{
    wxTreeCtrl* pTreeCtrl = (wxTreeCtrl*)event.GetEventObject();
    pTreeCtrl->SetItemImage(event.GetItem(), eTCIT_Folder);
}

void CEditorMainFrame::UpdateComponentInstanceTreeCtrl()
{
    m_pComponentProxyTreeCtrl->DeleteChildren(m_pComponentProxyTreeCtrl->GetRootItem());
    const std::map<uint32_t, CComponentProxy*>& curSceneComponentsMap = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
    std::map<wxString, wxTreeItemId> componentTreeItemIdMap;
    for (auto iter = curSceneComponentsMap.begin(); iter != curSceneComponentsMap.end(); ++iter)
    {
        uint32_t uGuid = iter->second->GetGuid();
        wxString strName = wxString::FromUTF8(CComponentProxyManager::GetInstance()->QueryComponentName(uGuid).c_str());
        if (componentTreeItemIdMap.find(strName) == componentTreeItemIdMap.end())
        {
            //Add a directory to tree
            componentTreeItemIdMap[strName] = m_pComponentProxyTreeCtrl->AppendItem(m_pComponentProxyTreeCtrl->GetRootItem(), strName, eTCIT_Folder, -1, new CComponentProxyTreeItemData(NULL));
        }
        //Add a file node to the directory.
        wxString strIdLabel = wxString::FromUTF8(iter->second->GetUserDefineDisplayName().c_str());
        if (strIdLabel.IsNull())
        {
            strIdLabel = wxString::FromUTF8(iter->second->GetDisplayName().c_str());
        }
        CComponentProxyTreeItemData* pComponentInstanceItemData = new CComponentProxyTreeItemData(iter->second);
        m_pComponentProxyTreeCtrl->AppendItem(componentTreeItemIdMap[strName], strIdLabel, eTCIT_File, -1, pComponentInstanceItemData);

    }
    m_pComponentProxyTreeCtrl->ExpandAll();
    GetCurrentViewAgent()->OnUpdateComponentInstance();
}

void CEditorMainFrame::UpdateLastOpenFilesTreeCtrl()
{
    m_pLastOpenFilesTreeCtrl->DeleteChildren(m_pLastOpenFilesTreeCtrl->GetRootItem());
    m_lastOpenFilesTreeItemIdMap.clear();
    for (auto iter : CEditorConfig::GetInstance()->GetLastOpenFiles())
    {
        TString strName = iter.substr(iter.rfind(_T("\\")) + 1);
        m_lastOpenFilesTreeItemIdMap[iter] = m_pLastOpenFilesTreeCtrl->AppendItem(m_pLastOpenFilesTreeCtrl->GetRootItem(), strName, eTCIT_File, -1);
    }
    m_pLastOpenFilesTreeCtrl->ExpandAll();
    GetCurrentViewAgent()->OnUpdateComponentInstance();
}


void CEditorMainFrame::UpdateLastOpenFilesList(const TCHAR* pFilePath)
{
    auto& lastOpenFilesVector = CEditorConfig::GetInstance()->GetLastOpenFiles();
    if (lastOpenFilesVector.size() == 0 || _tcsicmp(pFilePath, lastOpenFilesVector[0].c_str()) != 0)
    {
        for (auto iter = lastOpenFilesVector.begin(); iter != lastOpenFilesVector.end();)
        {
            if (_tcsicmp((*iter).c_str(), pFilePath) == 0)
            {
                iter = lastOpenFilesVector.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        lastOpenFilesVector.insert(lastOpenFilesVector.begin(), pFilePath);
        if (lastOpenFilesVector.size() > 30)
        {
            lastOpenFilesVector.pop_back();
        }
        CEditorConfig::GetInstance()->SaveToFile();
    }
}

void CEditorMainFrame::OnSelectComponentInstanceTreeItem(wxTreeEvent& /*event*/)
{
    wxTreeItemId activeId = m_pComponentProxyTreeCtrl->GetSelection();
    if (activeId.IsOk())
    {
        CComponentProxyTreeItemData* pItemData = (CComponentProxyTreeItemData*)m_pComponentProxyTreeCtrl->GetItemData(activeId);
        BEATS_ASSERT(pItemData != NULL);
        if (!pItemData->IsDirectory())
        {
            CComponentProxy* pComponentProxy = pItemData->GetComponentProxy();
            SelectComponent(pComponentProxy);
        }
    }
}

void CEditorMainFrame::OnSelectTreeItem(wxTreeEvent& event)
{
    switch (event.GetId())
    {
    case Ctrl_Tree_CompontentProxy:
        OnSelectComponentInstanceTreeItem(event);
        break;
    case Ctrl_Tree_CompontentTemplate:
        OnSelectComponentTemplateTreeItem(event);
        break;
    default:
        GetCurrentViewAgent()->OnTreeCtrlSelect(event);
        break;
    }
}

void CEditorMainFrame::OnCheckBoxGuidId(wxCommandEvent& event)
{
    m_bIsShowGuidId = event.IsChecked();
    UpdatePropertyGrid();
}

bool CEditorMainFrame::GetShowGuidId()
{
    return m_bIsShowGuidId;
}

void CEditorMainFrame::OnIdle(wxIdleEvent& event)
{
    if (m_pPropGridManager)
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
}

void CEditorMainFrame::OnTimer(wxTimerEvent& /*event*/)
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
    if (CEngineCenter::GetInstance()->ShouldUpdateThisFrame())
    {
        for (auto iter : m_updateWindowVector)
        {
            if (m_nCurrentViewID != ID_ViewButton_Game || iter != m_pComponentRenderWindow)
            {
                iter->Update();
            }
        }
    }
    BEYONDENGINE_PERFORMDETECT_STOP(ePNT_Editor)
    BEYONDENGINE_PERFORMDETECT_RESET();
}

bool CEditorMainFrame::Show(bool show)
{
    bool bRet = wxFrame::Show(show);
    if (show && bRet)
    {
        m_pViewScreen->SetContextToCurrent();

        wxCommandEvent event;
        event.SetInt(0);
        m_pCoordinateRenderTypeChoice->SetSelection(0);
        this->OnViewChoiceChanged(event);
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

void CEditorMainFrame::RegisterUpdateWindow(CBeyondEngineEditorGLWindow* pWindow)
{
    m_updateWindowVector.push_back(pWindow);
}

void CEditorMainFrame::HideTreeCtrl(wxTreeCtrl* pTreeCtrl)
{
    if (pTreeCtrl->IsShown())
    {
        uint32_t uPageCount = m_pLeftBook->GetPageCount();
        for (uint32_t i = 0; i < uPageCount; ++i)
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

void CEditorMainFrame::SelectViewID(int nViewID)
{
    m_pSceneViewBtn->SetValue(ID_ViewButton_Scene == nViewID);
    m_pUIViewBtn->SetValue(ID_ViewButton_UI == nViewID);
    m_pAniViewBtn->SetValue(ID_ViewButton_Ani == nViewID);
    m_pTerrainViewBtn->SetValue(ID_ViewButton_Terrain == nViewID);
    m_pGameViewBtn->SetValue(ID_ViewButton_Game == nViewID);

    if (m_nCurrentViewID != nViewID)
    {
        if (m_nCurrentViewID != INVALID_DATA)
        {
            GetCurrentViewAgent()->OutView();
        }
        BEATS_ASSERT(m_pViewScreen);
        GetViewAgent(nViewID)->SetGLWindow(m_pViewScreen);
        m_pViewScreen->SetViewAgent(GetViewAgent(nViewID));
        GetViewAgent(nViewID)->InView();
        m_nCurrentViewID = nViewID;
        SelectComponent(NULL);
        m_Manager.Update();
    }
}

int CEditorMainFrame::GetSelectdViewID()
{
    return m_nCurrentViewID;
}

void CEditorMainFrame::OnEditTreeItemLabel(wxTreeEvent& event)
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
    case Ctrl_Tree_CompontentProxy:
        oldName = m_pComponentProxyTreeCtrl->GetItemText(itemId);
        if (oldName != newName)
        {
            CComponentProxyTreeItemData* pInstanceData = (CComponentProxyTreeItemData*)m_pComponentProxyTreeCtrl->GetItemData(event.GetItem());
            BEATS_ASSERT(pInstanceData);
            CComponentProxy* pProxy = pInstanceData->GetComponentProxy();
            if (pProxy)
            {
                pProxy->SetUserDefineDisplayName(newName);
            }
        }
        break;
    default:
        GetCurrentViewAgent()->OnEditTreeItemLabel(event);
        break;
    }

}

void CEditorMainFrame::SetMouseType(EMouseType iType)
{
    m_eMouseType = iType;
}

EMouseType CEditorMainFrame::GetMouseType() const
{
    return m_eMouseType;
}

void CEditorMainFrame::OnMouseInCurrentView(wxMouseEvent& event)
{
    CViewAgentBase* pViewAgent = GetCurrentViewAgent();
    pViewAgent->ProcessMouseEvent(event);
}

void CEditorMainFrame::OnSelectTreeCtrl(wxTreeEvent& treeEvent)
{
    switch (treeEvent.GetId())
    {
    case Ctrl_Tree_CompontentTemplate:
        OnSelectComponentTemplateTreeItem(treeEvent);
        break;
    case Ctrl_Tree_CompontentProxy:
        OnSelectComponentInstanceTreeItem(treeEvent);
        break;
    default:
        break;
    }
    treeEvent.Skip();//call skip() is necessary to process the focuse event in default way
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
    case Ctrl_Tree_LastOpenFiles:
        OnActivateLastFiles(event);
        break;
    default:
        GetCurrentViewAgent()->OnActivateTreeItem(event);
        break;
    }
}

void CEditorMainFrame::OnPropertyGridRightClick(wxPropertyGridEvent& event)
{
    wxPGProperty* pProperty = event.GetProperty();
    m_pPropertyMenu->Enable(ePMS_ResetValue, pProperty->HasFlag(wxPG_PROP_MODIFIED) != 0);
    m_pPropertyMenu->Enable(ePMS_Expand, pProperty->GetChildCount() > 0 && pProperty->HasFlag(wxPG_PROP_COLLAPSED) != 0);
    m_pPropertyMenu->Enable(ePMS_Collapsed, pProperty->GetChildCount() > 0 && pProperty->HasFlag(wxPG_PROP_COLLAPSED) == 0);
    m_pPropertyMenu->Enable(ePMS_CopyProperty, true);
    CPropertyDescriptionBase* pSelectProperty = (CPropertyDescriptionBase*)pProperty->GetClientData();
    if (pSelectProperty)
    {
        bool bIsPasteEnable = CanPasteProperty(m_pCopyProperty, pSelectProperty);
        m_pPropertyMenu->Enable(ePMS_PasteProperty, bIsPasteEnable);
        m_pPropertyMenu->Enable(ePMS_Import, true);
        PopupMenu(m_pPropertyMenu, wxDefaultPosition);
    }
}

void CEditorMainFrame::OnPropertyGridSelect(wxPropertyGridEvent& event)
{
    GetViewAgent(m_nCurrentViewID)->OnPropertyGridSelect(event);
}

void CEditorMainFrame::OnPropertyGridMenuClick(wxMenuEvent& event)
{
    CEngineCenter::GetInstance()->m_bDelaySync = true;
    wxPGProperty* pProperty = m_pPropGridManager->GetGrid()->GetSelection();
    if (pProperty)
    {
        switch (event.GetId())
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
        case ePMS_CopyProperty:
        {
            CPropertyDescriptionBase* pPropertyDescription = (CPropertyDescriptionBase*)pProperty->GetClientData();
            if (m_pCopyProperty != nullptr)
            {
                BEATS_SAFE_DELETE(m_pCopyProperty);
            }
            m_pCopyProperty = pPropertyDescription->Clone(true);
        }
            break;
        case ePMS_PasteProperty:
        {
            BEATS_ASSERT(m_pCopyProperty != nullptr);
            CPropertyDescriptionBase* pRestoreProperty = CComponentProxyManager::GetInstance()->GetCurrReflectProperty();
            CPropertyDescriptionBase* pPropertyDescription = (CPropertyDescriptionBase*)pProperty->GetClientData();
            CComponentProxyManager::GetInstance()->SetCurrReflectProperty(pPropertyDescription, EReflectOperationType::ChangeValue);
            CSerializer serializer;
            m_pCopyProperty->Serialize(serializer, eVT_CurrentValue);
            pPropertyDescription->Deserialize(serializer, eVT_CurrentValue);
            CComponentProxyManager::GetInstance()->SetCurrReflectProperty(pRestoreProperty, EReflectOperationType::ChangeValue);

            pPropertyDescription->SetValueWithType(pPropertyDescription->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
            m_pPropGridManager->RefreshPropertyInGrid(pPropertyDescription);
            wxPGProperty* pPGProperty = m_pPropGridManager->GetPGPropertyByBase(pPropertyDescription);
            m_pPropGridManager->SelectProperty(pPGProperty, true);
        }
            break;
        case ePMS_Import:
        {
            CPropertyDescriptionBase* pPropertyDescription = (CPropertyDescriptionBase*)pProperty->GetClientData();
            if (pPropertyDescription != nullptr)
            {
                CComponentInstance* pHostComponent = pPropertyDescription->GetOwner()->GetHostComponent();
                if (pHostComponent != nullptr)
                {
                    TString strFilePath = wxFileSelector("选择需要导入的文件");
                    if (!strFilePath.empty())
                    {
                        bool bImportSuccess = pHostComponent->OnPropertyImport(pPropertyDescription->GetBasicInfo()->m_variableName, strFilePath);
                        if (bImportSuccess)
                        {
                            //TODO:HACK: reselect the component proxy to fix wild ptr crash of wxPGProperty's client data.
                            SelectComponent(nullptr);
                            SelectComponent(pPropertyDescription->GetOwner());
                        }
                        wxString msg = bImportSuccess ? "导入成功" : "导入失败";
                        wxMessageBox(msg);
                    }
                }
            }
        }
        break;
        default:
            BEATS_ASSERT(false, _T("Never reach here!"));
            break;
        }
    }
    CEngineCenter::GetInstance()->m_bDelaySync = false;
}

void CEditorMainFrame::ResetPropertyValue(wxPGProperty* pProperty)
{
    CWxwidgetsPropertyBase* pPropertyBase = static_cast<CWxwidgetsPropertyBase*>(pProperty->GetClientData());
    if (pPropertyBase->GetType() == eRPT_Ptr)
    {
        CPtrPropertyDescription* pPtrProperty = static_cast<CPtrPropertyDescription*>(pPropertyBase);
        pPtrProperty->DestroyInstance();
    }
    else if (pPropertyBase->IsContainerProperty())
    {
        pPropertyBase->RemoveAllChild(true);
    }
    pPropertyBase->SetValueWithType(pPropertyBase->GetValue(eVT_DefaultValue), eVT_CurrentValue, true);
    CEngineCenter::GetInstance()->m_editorPropertyGridSyncList.insert(pPropertyBase);
}

void CEditorMainFrame::SetCursor()
{
    m_pViewScreen->SetCursor(m_nCursorIconID);
}

void CEditorMainFrame::SetCursor(wxStockCursor cursorType)
{
    m_nCursorIconID = cursorType;
}

CEnginePropertyGridManager* CEditorMainFrame::GetPropGridManager()
{
    return m_pPropGridManager;
}

wxSplitterWindow* CEditorMainFrame::GetSplitter() const
{
    return m_pSplitter;
}

CParticleControlWnd* CEditorMainFrame::GetParticleControlWnd() const
{
    return m_pParticleControlWnd;
}

CViewAgentBase* CEditorMainFrame::GetViewAgent(int nViewID)
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
        break;
    case ID_ViewButton_Terrain:
        pViewAgent = CTerrainViewAgent::GetInstance();
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
    m_pSceneGridWnd->InitByConfig();
    m_pCameraWnd->InitByConfig();
#ifdef BEYOND_ENGINE_PUBLISH_VERSION
    if (CEditorConfig::GetInstance()->IsShowAboutDlgAfterLaunch())
    {
        ShowAboutDlg();
    }
#endif
}

void CEditorMainFrame::CreateExtraWindow()
{
    m_pTexturePreviewDialog = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetTexturePreviewDialog();
}

void CEditorMainFrame::OnSearch(wxCommandEvent& /*event*/)
{
    SearchInCurrentTree();
}

void CEditorMainFrame::OnPropertyChanged(wxPropertyGridEvent& event)
{
    GetCurrentViewAgent()->OnPropertyChanged(event);
}

void CEditorMainFrame::OnKeyInCurrentView(wxKeyEvent& event)
{
    CViewAgentBase* pViewAgent = GetCurrentViewAgent();
    pViewAgent->ProcessKeyboardEvent(event);
}

CViewAgentBase* CEditorMainFrame::GetCurrentViewAgent()
{
    return GetViewAgent(m_nCurrentViewID);
}

void CEditorMainFrame::ConvertPNGThread()
{
    size_t uCount = 0;
    while (1)
    {
        TString strSource;
        TString strEtcDest;
        TString strPvrDest;
        {
            std::lock_guard<std::mutex> locker(m_nPng2EtcProcessLock);
            if (m_pngFiles.size() > 0)
            {
                auto itr = m_pngFiles.back();
                m_pngFiles.pop_back();
                strSource = itr.first;
                strEtcDest = itr.second["etc"];
                strPvrDest = itr.second["pvr"];
            }
            else
            {
                break;
            }
        }
        if (!strSource.empty() && !strEtcDest.empty() && !strPvrDest.empty())
        {
            ConvertPNG(strSource, strEtcDest, strPvrDest);//contain encrypt operation
            {
                std::lock_guard<std::mutex> locker(m_nPng2EtcProcessLock);
                uCount = ++m_nPng2EtcFinishCount;
                m_nTaskCurCount = uCount;
            }
        }
    }
}

void CEditorMainFrame::EncryptXMLFiles(const TCHAR* pszTargetDirectoryName)
{
    TString strCurrentPath = wxGetCwd();
    TString strResourcePath = strCurrentPath + _T("\\..\\Resource");
    TString strTexturePath = strResourcePath + _T("\\Texture");
    TString strAndroidAssetsPath = strCurrentPath + _T("\\..\\Sample\\AndroidProject\\Sample\\Assets");
    TString strAndroidAssetsResourcePath = strAndroidAssetsPath + _T("\\Resource");
    TCHAR szCanonicalPath[MAX_PATH];
    SDirectory directory(NULL, NULL);
    directory.m_strPath = strTexturePath;
    CFileFilter filter;
    std::vector<TString> fileNameFilterList;
    // Encrypt xml files.
    CSerializer dataSerializer;
    CSerializer encryptData;
    TString strEncryptPathRoot = strAndroidAssetsResourcePath + pszTargetDirectoryName;
    CreatePathDir(strEncryptPathRoot);
    fileNameFilterList.push_back(_T("*.xml"));
    filter.SetFileNameFilter(fileNameFilterList, true);
    directory.m_pFileList->clear();
    CUtilityManager::GetInstance()->FillDirectory(directory, false, &filter);
    for (size_t i = 0; i < directory.m_pFileList->size(); ++i)
    {
        TString strFileFullName = directory.m_strPath;
        strFileFullName += directory.m_pFileList->at(i)->cFileName;
        CFilePathTool::GetInstance()->Canonical(szCanonicalPath, strFileFullName.c_str());
        dataSerializer.Serialize(szCanonicalPath);
        CResourceManager::GetInstance()->EncodeResourceData(dataSerializer, encryptData);
        wxString strEncryptPath = strEncryptPathRoot;
        strEncryptPath += _T("\\");
        strEncryptPath += directory.m_pFileList->at(i)->cFileName;
        strEncryptPath.MakeLower();
        CFilePathTool::GetInstance()->Canonical(szCanonicalPath, strEncryptPath.c_str());
        encryptData.Deserialize(szCanonicalPath, _T("wb+"));
        encryptData.Reset();
        dataSerializer.Reset();
    }
}

void CEditorMainFrame::StartConvertPng()
{
    m_pngConvertFileSizeMap.clear();
    CWaitingForProcessDialog* pWaidtinDialog = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWaitingDialog();
    pWaidtinDialog->SetTitle(_T("正在转换图片格式"));
    pWaidtinDialog->ShowUseTime(false);
    pWaidtinDialog->SetTotalCount(m_pngFiles.size());
    m_nTaskCurCount = 0;
    pWaidtinDialog->SetTask([this]()
    {
        return m_nTaskCurCount;
    });
    std::thread t1(&CEditorMainFrame::ConvertPNGThread, this);
    std::thread t2(&CEditorMainFrame::ConvertPNGThread, this);
    std::thread t3(&CEditorMainFrame::ConvertPNGThread, this);
    t1.detach();
    t2.detach();
    t3.detach();
    pWaidtinDialog->ShowModal();
}

void CEditorMainFrame::CollectPngFiles()
{
    TString strResourcePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Resource);
    TString strTexturePath = strResourcePath + _T("\\Texture");
    TString strPVRPath = strTexturePath + _T("\\PVR");
    TString strETCPath = strTexturePath + _T("\\ETC_temp");
    TString strPVRTexturePath = strPVRPath + _T("\\Texture");
    TString strETCTexturePath = strETCPath + _T("\\Texture");
    TString strProcessRecordListETC = strETCPath + _T("\\info.bin");
    TString strProcessRecordListPVR = strPVRPath + _T("\\info.bin");

    CSerializer oldInfoSerializer;
    TCHAR szCanonicalPath[MAX_PATH];

    m_ETCFileMd5Map.clear();
    m_PVRFileMd5Map.clear();
    m_oldETCFileMd5Map.clear();
    m_oldPVRFileMd5Map.clear();

    wxString strPVRTexturePathLower = strPVRTexturePath;
    wxString strETCTexturePathLower = strETCTexturePath;
    wxString strTexturePathLower = strTexturePath;
    strPVRTexturePathLower.LowerCase();
    strETCTexturePathLower.LowerCase();
    strTexturePathLower.LowerCase();

    if (m_strConverPNGType == "1")
    {
        std::vector<wxString> oldETCFileToRemove;
        if (wxFileExists(strProcessRecordListETC))
        {
            CFilePathTool::GetInstance()->Canonical(szCanonicalPath, strProcessRecordListETC.c_str());
            oldInfoSerializer.Serialize(szCanonicalPath);
            uint32_t uMapSize = 0;
            oldInfoSerializer >> uMapSize;
            for (uint32_t i = 0; i < uMapSize; ++i)
            {
                TString strFile;
                TString strMd5;
                oldInfoSerializer >> strFile;
                oldInfoSerializer >> strMd5;
                if (wxFileExists(strFile))
                {
                    m_oldETCFileMd5Map[strFile] = strMd5;
                }
                else
                {
                    oldETCFileToRemove.push_back(strFile);
                }
            }
        }

        for (auto iter : oldETCFileToRemove)
        {
            wxString strEtcPath = iter;
            strEtcPath.LowerCase();
            strEtcPath.replace(strEtcPath.find(strTexturePathLower), strTexturePathLower.size(), strETCTexturePathLower);
            if (wxFileExists(strEtcPath))
            {
                wxRemoveFile(strEtcPath);
            }
        }
    }

    if (m_strConverPNGType == "2")
    {
        std::vector<wxString> oldPVRFileToRemove;
        if (wxFileExists(strProcessRecordListPVR))
        {
            CFilePathTool::GetInstance()->Canonical(szCanonicalPath, strProcessRecordListPVR.c_str());
            oldInfoSerializer.Serialize(szCanonicalPath);
            uint32_t uMapSize = 0;
            oldInfoSerializer >> uMapSize;
            for (uint32_t i = 0; i < uMapSize; ++i)
            {
                TString strFile;
                TString strMd5;
                oldInfoSerializer >> strFile;
                oldInfoSerializer >> strMd5;
                if (wxFileExists(strFile))
                {
                    m_oldPVRFileMd5Map[strFile] = strMd5;
                }
                else
                {
                    oldPVRFileToRemove.push_back(strFile);
                }
            }
        }

        for (auto iter : oldPVRFileToRemove)
        {
            wxString strPvrPath = iter;
            strPvrPath.LowerCase();
            strPvrPath.replace(strPvrPath.find(strTexturePathLower), strTexturePathLower.size(), strPVRTexturePathLower);
            if (wxFileExists(strPvrPath))
            {
                wxRemoveFile(strPvrPath);
            }
        }
    }

    CreatePathDir(strETCTexturePath);
    CreatePathDir(strPVRTexturePath);
    m_pngFiles.clear();
    m_nPng2EtcFinishCount = 0;
    wxString filename;
    wxDir textureDir(strTexturePath);
    bool cont = textureDir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
    while (cont)
    {
        wxFileName fn(filename);
        if (fn.GetExt().compare(_T("png")) == 0)
        {
            TString strEtcDestPath = strTexturePath + _T("\\") + filename;
            strEtcDestPath.replace(strEtcDestPath.find(strTexturePath), strTexturePath.size(), strETCTexturePath);
            wxFileName destFileName(strEtcDestPath);
            destFileName.Normalize();
            strEtcDestPath = destFileName.GetFullPath();
            wxFileName sourceFileName(strTexturePath + _T("\\") + filename);
            sourceFileName.Normalize();
            TString strSourcePath = sourceFileName.GetFullPath();

            TString strPvrDestPath = strTexturePath + _T("\\") + filename;
            strPvrDestPath.replace(strPvrDestPath.find(strTexturePath), strTexturePath.size(), strPVRTexturePath);
            wxFileName pvrDestFileName(strPvrDestPath);
            pvrDestFileName.Normalize();
            strPvrDestPath = pvrDestFileName.GetFullPath();

            std::map<TString, TString> destPathMap;
            destPathMap["etc"] = strEtcDestPath;
            destPathMap["pvr"] = strPvrDestPath;
            m_pngFiles.push_back(std::pair<TString, std::map<TString, TString>>(strSourcePath, destPathMap));
        }
        cont = textureDir.GetNext(&filename);
    }
    // Exam the png if it really need a alpha channel
    //#define EXAM_PNG_ALPHA_CHANNEL_NECESSARY
#ifdef EXAM_PNG_ALPHA_CHANNEL_NECESSARY
    for (size_t i = 0; i < m_pngFiles.size(); ++i)
    {
        const TString& strFilePath = m_pngFiles[i].first;
        CSerializer fileData(strFilePath.c_str(), "rb");
        CImage imageObj;
        imageObj.InitWithImageData(fileData.GetBuffer(), fileData.GetWritePos(), strFilePath.c_str());
        if (imageObj.HasAlpha())
        {
            wxImage imageWxObj(strFilePath.c_str(), wxBITMAP_TYPE_PNG);
            if (!imageWxObj.HasAlpha())
            {
                BEATS_PRINT("Png don't need alpha channel! %s\n", strFilePath.c_str());
            }
        }
    }
#endif
}

void CEditorMainFrame::ConvertPNG(TString& strPath, TString& strETCDestPath, TString& strPVRDestPath)
{
    if (wxFile::Exists(strPath))
    {
        bool bConvertETC = m_strConverPNGType == _T("1");
        bool bConvertPVR = m_strConverPNGType == _T("2");
        if (bConvertETC)
        {
            CSerializer fileData(strPath.c_str(), _T("rb"));
            CMD5 fileMd5(fileData.GetBuffer(), fileData.GetWritePos());
            TString strFileMd5 = fileMd5.ToString();
            {
                std::lock_guard<std::mutex> locker(m_nPng2EtcProcessLock);
                m_ETCFileMd5Map[strPath] = strFileMd5;

                auto itr = m_oldETCFileMd5Map.find(strPath);
                if (itr != m_oldETCFileMd5Map.end())
                {
                    if ((*itr).second == strFileMd5)
                    {
                        bConvertETC = !wxFileExists(strETCDestPath);
                    }
                    m_oldETCFileMd5Map.erase(itr);
                }
            }
            if (bConvertETC)
            {
                ConvertPNG2ETC(strPath, strETCDestPath);
            }
        }

        if (bConvertPVR)
        {
            CSerializer fileData(strPath.c_str(), _T("rb"));
            CMD5 fileMd5(fileData.GetBuffer(), fileData.GetWritePos());
            TString strFileMd5 = fileMd5.ToString();
            {
                std::lock_guard<std::mutex> locker(m_nPng2EtcProcessLock);
                m_PVRFileMd5Map[strPath] = strFileMd5;

                auto itr = m_oldPVRFileMd5Map.find(strPath);
                if (itr != m_oldPVRFileMd5Map.end())
                {
                    if ((*itr).second == strFileMd5)
                    {
                        bConvertPVR = !wxFileExists(strPVRDestPath);
                    }
                    m_oldPVRFileMd5Map.erase(itr);
                }
            }
            if (bConvertPVR)
            {
                ConvertPNG2PVR(strPath, strPVRDestPath);
            }
        }
    }
}

void CEditorMainFrame::CompressData(const unsigned char* pSource, unsigned long uSourceLength, CSerializer& compressedData)
{
    uint32_t uHeaderLength = sizeof(BEYONDENGINE_ZIP_FILE_HEADER);
    unsigned long uDestLength = CZipUtils::CompressBound(uSourceLength);
    compressedData.ValidateBuffer(uDestLength + uHeaderLength);
    compressedData << BEYONDENGINE_ZIP_FILE_HEADER;
    CZipUtils::Compress(pSource, uSourceLength, (unsigned char*)compressedData.GetWritePtr(), &uDestLength);
    compressedData.SetWritePos(uDestLength + uHeaderLength);
}

void CEditorMainFrame::GenerateResourceList()
{
    TString strCurrentPath = wxGetCwd();
    std::replace(strCurrentPath.begin(), strCurrentPath.end(), '\\', '/');
    TString strAndroidAssetsPath = strCurrentPath + _T("/../Sample/AndroidProject/Sample/Assets");
    TString strAndroidAssetsResourcePath = strAndroidAssetsPath + _T("/Resource");
    TString strAndroidAssetsPathSep = strAndroidAssetsPath + _T("/");
    TString strProcessRecordList = strAndroidAssetsPath + _T("/ResourceList.bin");
    CWaitingForProcessDialog* pWaidtinDialog = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWaitingDialog();
    pWaidtinDialog->SetTitle(_T("正在生成资源列表"));
    pWaidtinDialog->ShowUseTime(false);
    m_nTaskCurCount = 0;
    wxString filename;
    wxArrayString files;
    TCHAR szCanonicalPath[MAX_PATH];
    wxDir::GetAllFiles(strAndroidAssetsResourcePath, &files);
    pWaidtinDialog->SetTotalCount(files.size());
    CSerializer listSerializer;
    listSerializer << (uint32_t)files.size();
    for (size_t i = 0; i < files.size(); i++)
    {
        wxFileName fn(files[i]);
        TString path = fn.GetFullPath();
        std::replace(path.begin(), path.end(), '\\', '/');
        FILE*pFile = fopen(path.c_str(), _T("rb"));
        CMD5 fileMd5(pFile);
        fseek(pFile, 0L, SEEK_END);
        uint32_t uLength = ftell(pFile);
        fclose(pFile);
        TString strFileMd5 = fileMd5.ToString();
        path.replace(path.find(strAndroidAssetsPathSep), strAndroidAssetsPathSep.size(), "");
        CFilePathTool::GetInstance()->Canonical(szCanonicalPath, path.c_str());
        path = szCanonicalPath;
        std::replace(path.begin(), path.end(), '\\', '/');
        std::transform(path.begin(), path.end(), path.begin(), tolower);
        listSerializer << path;
        listSerializer << strFileMd5;
        listSerializer << uLength;
        m_nTaskCurCount = i + 1;
    }
    CFilePathTool::GetInstance()->Canonical(szCanonicalPath, strProcessRecordList.c_str());
    listSerializer.Deserialize(szCanonicalPath);
    listSerializer.Reset();
}

void CEditorMainFrame::ConvertPNG2ETC(const TString& strPath, TString& strETCDestPath)
{
    TString strFileName = CFilePathTool::GetInstance()->FileName(strPath.c_str());
    uint32_t uOriginSize = 0;
    CSerializer serializer;
    if (m_compressTextureSet.find(strFileName) != m_compressTextureSet.end())
    {
        wxLogNull logNo;
        FILE* pFile = _tfopen(strPath.c_str(), "rb");
        BEATS_ASSERT(pFile != nullptr);
        fseek(pFile, 0, SEEK_END);
        uOriginSize = ftell(pFile);
        fclose(pFile);
        wxImage image(strPath, wxBITMAP_TYPE_PNG);
        etc1_uint32 height = image.GetHeight();
        etc1_uint32 width = image.GetWidth();
        etc1_uint32 dataSize = etc1_get_encoded_data_size(width, height);
        uint32_t uFinalDataSize = dataSize + ETC_PKM_HEADER_SIZE;
        bool bHasAlphaData = image.HasAlpha();
        if (bHasAlphaData)
        {
            uFinalDataSize *= 2;
        }
        serializer.ValidateBuffer(uFinalDataSize);
        size_t stride = width * 3;
        etc1_pkm_format_header((etc1_byte*)serializer.GetWritePtr(), width, height);
        serializer.SetWritePos(ETC_PKM_HEADER_SIZE);
        int nRet = etc1_encode_image(image.GetData(), width, height, 3, stride, (etc1_byte*)serializer.GetWritePtr());
        BEATS_ASSERT(nRet == 0, _T("etc1 encode error"));
        serializer.SetWritePos(dataSize + ETC_PKM_HEADER_SIZE);
        if (bHasAlphaData)
        {
            for (size_t i = 0; i < height; i++)
            {
                for (size_t j = 0; j < width; j++)
                {
                    image.SetRGB(j, i, image.GetAlpha(j, i), 0, 0);
                }
            }
            etc1_pkm_format_header((etc1_byte*)serializer.GetWritePtr(), width, height);
            serializer.SetWritePos(dataSize + ETC_PKM_HEADER_SIZE * 2);
            nRet = etc1_encode_image(image.GetData(), width, height, 3, stride, (etc1_byte*)serializer.GetWritePtr());
            BEATS_ASSERT(nRet == 0, _T("etc1 encode error"));
            serializer.SetWritePos((dataSize + ETC_PKM_HEADER_SIZE) * 2);
        }
    }
    else
    {
        serializer.Serialize(strPath.c_str());
        uOriginSize = serializer.GetWritePos();
    }
    TCHAR szCanonicalPath[MAX_PATH];
    CSerializer compressedData;
    CompressData(serializer.GetBuffer(), serializer.GetWritePos(), compressedData);
    m_compressLock.lock();
    m_pngConvertFileSizeMap[strPath].uOriginSize = uOriginSize;
    m_pngConvertFileSizeMap[strPath].uConvertSize = serializer.GetWritePos();
    m_pngConvertFileSizeMap[strPath].uCompressSize = compressedData.GetWritePos();
    m_compressLock.unlock();
    CSerializer encryptData;
    CResourceManager::GetInstance()->EncodeResourceData(compressedData, encryptData);
    CFilePathTool::GetInstance()->Canonical(szCanonicalPath, strETCDestPath.c_str());
    strETCDestPath = CStringHelper::GetInstance()->ToLower(strETCDestPath);
    encryptData.Deserialize(szCanonicalPath, _T("wb+"));
    encryptData.Reset();
    serializer.Reset();
}

void CEditorMainFrame::ConvertPNG2PVR(const TString& strPath, TString& strPVRDestPath)
{
    strPVRDestPath = CStringHelper::GetInstance()->ToLower(strPVRDestPath);
    TString strFileName = CFilePathTool::GetInstance()->FileName(strPath.c_str());
    CSerializer pvrImageData;
    uint32_t uOriginSize = 0;
    if (m_compressTextureSet.find(strFileName) != m_compressTextureSet.end())
    {
        FILE* pFile = _tfopen(strPath.c_str(), "rb");
        BEATS_ASSERT(pFile != nullptr);
        fseek(pFile, 0, SEEK_END);
        uOriginSize = ftell(pFile);
        fclose(pFile);
        wxFileName fn(strPVRDestPath);
        fn.SetExt("pvr");//the tool commond require .pvr file
        TString strPvrFile = fn.GetFullName();
        TCHAR strCmdLine[MAX_PATH * 10];
        _stprintf(strCmdLine, _T("..\\Publish\\PVRTexToolCLI.exe -flip y -f PVRTC1_4,UBN,lRGB -q pvrtcbest -i %s -o %s"), strPath.c_str(), strPvrFile.c_str());
        _tsystem(strCmdLine);
        if (wxFileExists(strPvrFile))
        {
            pvrImageData.Serialize(strPvrFile.c_str());
            wxRemoveFile(strPvrFile);
        }
        else//invalid format
        {
            pvrImageData.Serialize(strPath.c_str());
        }
    }
    else
    {
        pvrImageData.Serialize(strPath.c_str());
        uOriginSize = pvrImageData.GetWritePos();
    }
    CSerializer compressedPvrData;
    CompressData(pvrImageData.GetBuffer(), pvrImageData.GetWritePos(), compressedPvrData);
    m_compressLock.lock();
    m_pngConvertFileSizeMap[strPath].uOriginSize = uOriginSize;
    m_pngConvertFileSizeMap[strPath].uConvertSize = pvrImageData.GetWritePos();
    m_pngConvertFileSizeMap[strPath].uCompressSize = compressedPvrData.GetWritePos();
    m_compressLock.unlock();
    CSerializer pvrEncryptData;
    CResourceManager::GetInstance()->EncodeResourceData(compressedPvrData, pvrEncryptData);

    TCHAR szPvrCanonicalPath[MAX_PATH];
    CFilePathTool::GetInstance()->Canonical(szPvrCanonicalPath, strPVRDestPath.c_str());
    pvrEncryptData.Deserialize(szPvrCanonicalPath, _T("wb+"));
    pvrEncryptData.Reset();
    pvrImageData.Reset();
    compressedPvrData.Reset();
}

bool CEditorMainFrame::GenerateVersionInfo()
{
    bool bRet = false;
    TString strCurrentPath = wxGetCwd();
    TString strVersionInfoFile = strCurrentPath + _T("\\..\\VersionInfo.bin");
    CSetVersionDialog versionDialog(NULL, wxID_ANY, wxT("Set Version"), wxDefaultPosition, wxSize(350, 230), wxCAPTION);
    if (wxFileExists(strVersionInfoFile))
    {
        CSerializer serializer(strVersionInfoFile.c_str());
        TString strData;
        serializer >> strData;
        BEATS_ASSERT(strData.compare(_T("AppVersion")) == 0);
        serializer >> strData;
        versionDialog.SetAppVersion(strData);
        serializer >> strData;
        BEATS_ASSERT(strData.compare(_T("ResVersion")) == 0);
        serializer >> strData;
        versionDialog.SetResVersion(strData);
        versionDialog.SetConvertTypeText(_T("1"));
    }
    if (versionDialog.ShowModal() == wxID_OK)
    {
        CSerializer serializer;
        TString strData = _T("AppVersion");
        serializer << strData;
        serializer << versionDialog.GetAppVersion().ToStdString();
        strData = _T("ResVersion");
        serializer << strData;
        serializer << versionDialog.GetResVersion().ToStdString();
        serializer.Deserialize(strVersionInfoFile.c_str(), _T("wb+"));
        m_strConverPNGType = versionDialog.GetConvertTypeText().ToStdString();
        if (m_strConverPNGType == "1" || m_strConverPNGType == "2")//1 for android 2 for ios
        {
            bRet = true;
        }
    }
    return bRet;
}

bool CEditorMainFrame::CanPasteProperty(const CPropertyDescriptionBase* pSource, const CPropertyDescriptionBase* pTarget) const
{
    bool bIsPasteEnable = pSource != nullptr &&
        pSource->GetType() == pTarget->GetType();
    if (bIsPasteEnable)
    {
        switch (pSource->GetType())
        {
        case eRPT_Ptr:
        {
            uint32_t uSourceGuid = static_cast<const CPtrPropertyDescription*>(pSource)->GetDerivedGuid();
            if (uSourceGuid == 0)
            {
                uSourceGuid = static_cast<const CPtrPropertyDescription*>(pSource)->GetPtrGuid();
            }
            uint32_t uTargetGuid = static_cast<const CPtrPropertyDescription*>(pTarget)->GetDerivedGuid();
            if (uTargetGuid == 0)
            {
                uTargetGuid = static_cast<const CPtrPropertyDescription*>(pTarget)->GetPtrGuid();
            }
            if (uSourceGuid != uTargetGuid)
            {
                std::set<uint32_t> derivedGuid;
                CComponentProxyManager::GetInstance()->QueryDerivedClass(uTargetGuid, derivedGuid, true);
                bIsPasteEnable = derivedGuid.find(uSourceGuid) != derivedGuid.end();
            }
        }
        break;
        case eRPT_List:
        {
            const CListPropertyDescription* pSourceList = down_cast<const CListPropertyDescription*>(pSource);
            const CListPropertyDescription* pTargetList = down_cast<const CListPropertyDescription*>(pTarget);
            bIsPasteEnable = CanPasteProperty(pSourceList->GetTemplateProperty(), pTargetList->GetTemplateProperty());
        }
        break;
        case eRPT_Map:
        {
            const CMapPropertyDescription* pSourceMap = down_cast<const CMapPropertyDescription*>(pSource);
            const CMapPropertyDescription* pTargetMap = down_cast<const CMapPropertyDescription*>(pTarget);
            bIsPasteEnable = CanPasteProperty(pSourceMap->GetKeyPropertyTemplate(), pTargetMap->GetKeyPropertyTemplate()) &&
                CanPasteProperty(pSourceMap->GetValuePropertyTemplate(), pTargetMap->GetValuePropertyTemplate());
        }
        break;
        case eRPT_MapElement:
        {
            bIsPasteEnable = CanPasteProperty(pSource->GetChildren()[0], pTarget->GetChildren()[0]) &&
                CanPasteProperty(pSource->GetChildren()[1], pTarget->GetChildren()[1]);
        }
        break;
        default:
            break;
        }
    }
    return bIsPasteEnable;
}

void CEditorMainFrame::OnTimeBarTreeItemSelected(wxCommandEvent& /*event*/)
{
    GetCurrentViewAgent()->OnTimeBarTreeItemSelect();
}

void CEditorMainFrame::OnTimeBarTreeItemDragEnd(wxCommandEvent& /*event*/)
{
    GetCurrentViewAgent()->OnTimeBarTreeItemDrag();
}

void CEditorMainFrame::OnTimeBarChoice(wxCommandEvent& /*event*/)
{
    GetCurrentViewAgent()->OnTimeBarChoice();
}

void CEditorMainFrame::OnTimeBarItemContainerRClick(wxCommandEvent& event)
{
    GetCurrentViewAgent()->OnTimeBarItemContainerRClick(event);
}

void CEditorMainFrame::OnTimeBarAddButtonClick(wxCommandEvent& /*event*/)
{
    GetCurrentViewAgent()->OnTimeBarAddButtonClick();
}

void CEditorMainFrame::OnTimeBarMinusButtonClick(wxCommandEvent& /*event*/)
{
    GetCurrentViewAgent()->OnTimeBarMinusButtonClick();
}

void CEditorMainFrame::OnTimeBarCursorChange(wxCommandEvent& event)
{
    GetCurrentViewAgent()->OnTimeBarCurSorChange(event.GetInt());
}

void CEditorMainFrame::OnComponentPropertyChange(CComponentBase* pComponent)
{
    GetCurrentViewAgent()->OnComponentPropertyChange(pComponent);
}

void CEditorMainFrame::JumpToComponent(uint32_t uComponentId)
{
    BEATS_ASSERT(uComponentId != 0xFFFFFFFF);
    uint32_t uFileId = CComponentProxyManager::GetInstance()->GetProject()->QueryFileId(uComponentId, false);
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    bool bNeedSwitchFile = true;
    if (CComponentProxyManager::GetInstance()->GetCurrentViewFileId() != uFileId)
    {
        TString filePath = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(uFileId);
        TCHAR szInfo[1024];
        _stprintf(szInfo, _T("搜索到ID为%d的组件位于文件%s,是否跳转到该组件?"), uComponentId, filePath.c_str());
        int iAnswer = MessageBox(BEYONDENGINE_HWND, szInfo, _T("搜索结果"), MB_YESNO);
        bNeedSwitchFile = iAnswer == IDYES;
        if (bNeedSwitchFile)
        {
            ActivateFile(filePath.c_str());
        }
    }
    if (bNeedSwitchFile)
    {
        CComponentBase* pComponent = CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId);
        BEATS_ASSERT(pComponent != nullptr);
        SelectComponent(static_cast<CComponentProxy*>(pComponent));
    }
}

void CEditorMainFrame::OnTimeBarSelectFrameChange(wxCommandEvent& /*event*/)
{
    GetCurrentViewAgent()->OnTimeBarSelectFrameChange();
}

void CEditorMainFrame::OnTimeBarDraggingFrame(wxCommandEvent& /*event*/)
{
    GetCurrentViewAgent()->OnTimeBarDraggingFrame();
}

void CEditorMainFrame::AddExportFileFullPathList(TString strFullPath)
{
    TCHAR szBufferPath[MAX_PATH];
    CFilePathTool::GetInstance()->Canonical(szBufferPath, strFullPath.c_str());
    CFilePathTool::GetInstance()->ConvertToWindowsPath(szBufferPath);
    strFullPath = szBufferPath;
    std::transform(strFullPath.begin(), strFullPath.end(), strFullPath.begin(), tolower);
    m_exportFileFullPathList.insert(strFullPath);
}

const std::set<TString>& CEditorMainFrame::GetExportFileFullPathList() const
{
    return m_exportFileFullPathList;
}

CSceneGridWnd* CEditorMainFrame::GetSceneGridWnd() const
{
    return m_pSceneGridWnd;
}

void CEditorMainFrame::OnViewChoiceChanged(wxCommandEvent& event)
{
    ECoordinateRenderType renderMode = (ECoordinateRenderType)event.GetSelection();
    CRenderManager::GetInstance()->GetCoordinateRenderObject()->SetRenderType(renderMode);
}

void CEditorMainFrame::RecordPackPngFileSize()
{
    TString strCurrentPath = wxGetCwd();
    std::replace(strCurrentPath.begin(), strCurrentPath.end(), '\\', '/');
    TString strResourceCompressLogPath = strCurrentPath + _T("/../ResourceCompressLog.txt");
    CSerializer configData;
    for (auto iter : m_pngConvertFileSizeMap)
    {
        TString strFile = iter.first;
        BEATS_ASSERT(iter.second.uOriginSize > 0);
        if (iter.second.uOriginSize > 0)
        {
            strFile += _T(":") + std::to_string(iter.second.uOriginSize);
            TCHAR szScaleFormat[MAX_PATH];

            TString strCompress = m_strConverPNGType == _T("1") ? _T("  ETC:") : _T("  PVR:");
            if (iter.second.uConvertSize > 0)
            {
                _stprintf(szScaleFormat, "%.2f", (float)iter.second.uConvertSize / (float)iter.second.uOriginSize * 100.f);
                strFile += strCompress + std::to_string(iter.second.uConvertSize) + _T("(") + szScaleFormat + _T("%)");
            }
            if (iter.second.uCompressSize > 0)
            {
                _stprintf(szScaleFormat, "%.2f", (float)iter.second.uCompressSize / (float)iter.second.uOriginSize * 100.f);
                strFile += strCompress + std::to_string(iter.second.uCompressSize) + _T("(") + szScaleFormat + _T("%)");
            }
        }

        TString strCompress = _T("[N]");
        if (m_compressTextureSet.find(CFilePathTool::GetInstance()->FileName(iter.first.c_str())) != m_compressTextureSet.end())
        {
            strCompress = _T("[Y]");
        }
        strFile += strCompress;
        strFile.append("\n");
        configData.Serialize((void*)strFile.c_str(), strFile.length());
    }
    if (!m_pngConvertFileSizeMap.empty())
    {
        configData.Deserialize(strResourceCompressLogPath.c_str(), "wt+");
    }
}

#ifndef _DEBUG
#pragma warning(default:4722)
#endif
