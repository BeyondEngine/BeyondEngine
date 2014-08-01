#include "stdafx.h"
#include "EngineEditor.h"
#include "timebarframe.h"
#include "EditUIDialog.h"
#include "DataViewCtrl.h"
#include "UIGLWindow.h"
#include "EngineProperGridManager.h"
#include "GUI/Window/WindowDefs.h"
#include "Resource/ResourceManager.h"
#include "Render/Texture.h"
#include "Render/TextureFragManager.h"
#include "GUI/WindowManager.h"
#include "GUI/Window/Window.h"
#include "GUI/Renderer/WindowRenderer.h"
#include "ComponentTreeItemData.h"
#include "wx/dataview.h"
#include "wx/variant.h"
#include "DataViewListModel.h"
#include "timebarevent.h"

#include <wx/artprov.h>
#include "DataViewTreeModel.h"
#include "EditorMainFrame.h"


static const TCHAR* FCGUIName[] = 
{
    _T("WindowBase"),
    _T("Button"),
    _T("CheckBox"),
    _T("Slider"),
    _T("Progress"),
    _T("TextEdiit"),
    _T("TextLabel"),
    _T("ListBox"),
    _T("ComboBox"),
    _T("ListControl")
};
enum EMenuObject
{
    Menu_ObjectView_Delete,
    Menu_ObjectView_Paste,
    Menu_ObjectView_Copy
};

BEGIN_EVENT_TABLE(CEditUIDialog, CEditDialogBase)
EVT_MENU(wxID_ANY, CEditUIDialog::OnPopMenu)
EVT_AUITOOLBAR_BEGIN_DRAG(wxID_ANY, CEditUIDialog::OnToolBarDrag)
EVT_TREE_BEGIN_DRAG(wxID_ANY,CEditUIDialog::OnCtrlsTreeBeginDrag)
EVT_TREE_END_DRAG(wxID_ANY,CEditUIDialog::OnCtrlsTreeEndDrag)
EVT_DATAVIEW_SELECTION_CHANGED(ID_DataView_ObjectView, CEditUIDialog::OnObjectViewSeletionChange)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(ID_DataView_ObjectView, CEditUIDialog::OnRightClick)
EVT_DATAVIEW_CUSTRENDER_ACTIVE(wxID_ANY, CEditUIDialog::OnDataViewRenderer)
END_EVENT_TABLE()

CEditUIDialog::CEditUIDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : CEditDialogBase(parent, id, title, pos, size, style, name)
    , m_pTempGUI(NULL)
    , m_pSelectedGUI(NULL)
{
    m_manager.SetManagedWindow(this);
    CreateAuiToolBar();
    InitCtrls();
    m_manager.Update();

}

CEditUIDialog::~CEditUIDialog()
{
    m_manager.UnInit();
}

void CEditUIDialog::CreateAuiToolBar()
{
    wxAuiToolBarItemArray prepend_items;
    wxAuiToolBarItemArray append_items;
    wxAuiToolBarItem item;
    item.SetKind(wxITEM_SEPARATOR);
    append_items.Add(item);
    item.SetKind(wxITEM_NORMAL);
    item.SetId(ID_CustomizeToolbar);
    item.SetLabel(_("Customize..."));
    append_items.Add(item);

    wxAuiToolBar* tb1 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_HORIZONTAL);
    tb1->SetToolBitmapSize(wxSize(16,16));
    tb1->AddTool(ID_ToolBar1_Radio1, wxT("Radio 1"), wxArtProvider::GetBitmap(wxART_QUESTION), wxT("Radio 1"), wxITEM_RADIO);
    tb1->AddTool(ID_ToolBar1_Radio2, wxT("Radio 2"), wxArtProvider::GetBitmap(wxART_QUESTION), wxT("Radio 2"), wxITEM_RADIO);
    tb1->AddTool(ID_ToolBar1_Radio3, wxT("Radio 3"), wxArtProvider::GetBitmap(wxART_QUESTION), wxT("Radio 3"), wxITEM_RADIO);
    tb1->AddTool(ID_ToolBar1_Radio4, wxT("Radio 4"), wxArtProvider::GetBitmap(wxART_QUESTION), wxT("Radio 4"), wxITEM_RADIO);
    tb1->SetCustomOverflowItems(prepend_items, append_items);
    tb1->Realize();

    wxAuiToolBar* tb2 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_HORIZONTAL);
    tb2->SetToolBitmapSize(wxSize(16,16));
    tb2->AddTool(ID_ToolBar2_Button1, wxT("Disabled"), wxArtProvider::GetBitmap(wxART_QUESTION));
    tb2->AddTool(ID_ToolBar2_Button2, wxT("Test"), wxArtProvider::GetBitmap(wxART_QUESTION));
    tb2->SetCustomOverflowItems(prepend_items, append_items);
    tb2->Realize();

    wxAuiToolBar* tb3 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_VERTICAL);
    tb3->SetToolBitmapSize(wxSize(16,16));
    tb3->AddTool(ID_ToolBar2_Button1, wxT("Disabled"), wxArtProvider::GetBitmap(wxART_QUESTION));
    tb3->AddTool(ID_ToolBar2_Button2, wxT("Test"), wxArtProvider::GetBitmap(wxART_QUESTION));
    tb3->SetCustomOverflowItems(prepend_items, append_items);
    tb3->Realize();

    m_manager.AddPane(tb1,
        wxAuiPaneInfo().Name(wxT("tb1")).
        ToolbarPane().Top().Position(0).Dockable(false));
    m_manager.AddPane(tb2,
        wxAuiPaneInfo().Name(wxT("tb2")).
        ToolbarPane().Top().Position(1).Dockable(false));
    m_manager.AddPane(tb3,
        wxAuiPaneInfo().Name(wxT("tb3")).
        ToolbarPane().Left().Position(0).Dockable(false));
}

void CEditUIDialog::CreateAuiNoteBook()
{
    wxSize client_size = GetClientSize();
    long lStyle = wxAUI_NB_DEFAULT_STYLE & ~wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
    
    client_size = wxSize(client_size.GetWidth() * 0.4, client_size.GetHeight() * 0.3);
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    m_pCanvasPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pCanvasPanel->SetSizer(pSizer);
    m_pCanvasNotebook = new wxAuiNotebook(m_pCanvasPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pCanvasNotebook, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pObjectViewPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pObjectViewPanel->SetSizer(pSizer);
    m_pObjectViewNotebook = new wxAuiNotebook(m_pObjectViewPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pObjectViewNotebook, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pGLWindowPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pGLWindowPanel->SetSizer(pSizer);
    m_pGLWindowNotebook = new wxAuiNotebook(m_pGLWindowPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pGLWindowNotebook, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pPropertyPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pPropertyPanel->SetSizer(pSizer);
    m_pPropertyNotebook = new wxAuiNotebook(m_pPropertyPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pPropertyNotebook, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pTimeBarPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pTimeBarPanel->SetSizer(pSizer);
    m_pTimeBarNotebook = new wxAuiNotebook(m_pTimeBarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pTimeBarNotebook, 1, wxGROW|wxALL, 0);
    
    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pCtrlsPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pCtrlsPanel->SetSizer(pSizer);
    m_pCtrlsNotebook = new wxAuiNotebook(m_pCtrlsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pCtrlsNotebook, 1, wxGROW|wxALL, 0);
}

void CEditUIDialog::CreatePropertyGrid()
{
    int style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPG_TOOLTIPS;
    int extraStyle = wxPG_EX_MODE_BUTTONS | wxPG_EX_MULTIPLE_SELECTION;

    m_pPropGridManager = new CEnginePropertyGirdManager();
    m_pPropGridManager->Create(m_pPropertyNotebook, wxID_ANY, wxDefaultPosition, wxSize(100, 100), style );
    m_pPropGrid = m_pPropGridManager->GetGrid();
    m_pPropGridManager->SetExtraStyle(extraStyle);
    m_pPropGridManager->SetValidationFailureBehavior( wxPG_VFB_MARK_CELL | wxPG_VFB_SHOW_MESSAGEBOX );
    m_pPropGrid->SetVerticalSpacing( 2 );
    m_pPropGridManager->AddPage(wxT("page"));

    m_pPropertyNotebook->Freeze();
    m_pPropertyNotebook->AddPage( m_pPropGridManager, wxT("PropGrid") );
    m_pPropertyNotebook->Thaw();

    m_manager.AddPane(m_pPropertyPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("PropertyPane")).
        Right());
}

void CEditUIDialog::InitCtrls()
{
    wxGLContext* pContext = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetGLContext();
    CreateMenu();
    CreateAuiNoteBook();
    CreatePropertyGrid();
    CreateCtrlsTree();
    CreateGUITexture(_T("TestGUI.xml"));
    m_pTimeBar = new CTimeBarFrame(m_pTimeBarNotebook);
    m_pGLWindow = new CUIGLWindow(m_pGLWindowNotebook, pContext);
    m_pCanvasList = new wxListBox(m_pCanvasNotebook, wxID_ANY);
    m_pObjectView = new CDataViewCtrl(m_pObjectViewNotebook, ID_DataView_ObjectView, wxDefaultPosition, wxDefaultSize, wxLB_NO_SB);
    //m_pObjectView->AssociateTreeModel();
    static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame()->RegisterUpdateWindow(m_pGLWindow);

    m_pCanvasNotebook->Freeze();
    m_pCanvasNotebook->AddPage( m_pCanvasList, wxT("Canvas") );
    m_pCanvasNotebook->Thaw();

    m_pObjectViewNotebook->Freeze();
    m_pObjectViewNotebook->AddPage( m_pObjectView, wxT("ObjectView") );
    m_pObjectViewNotebook->Thaw();

    m_pGLWindowNotebook->Freeze();
    m_pGLWindowNotebook->AddPage( m_pGLWindow, wxT("GLWindow") );
    m_pGLWindowNotebook->Thaw();

    m_pTimeBarNotebook->Freeze();
    m_pTimeBarNotebook->AddPage( m_pTimeBar, wxT("TimeBar") );
    m_pTimeBarNotebook->Thaw(); 

    m_manager.AddPane(m_pCanvasPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("CanvasPane")).
        Left().Layer(1).Position(0));
    m_manager.AddPane(m_pObjectViewPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("ObjectViewPane")).
        Left().Layer(1).Position(1)); 
    m_manager.AddPane(m_pGLWindowPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("GLWindowPane")).
        Center());
    m_manager.AddPane(m_pTimeBarPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("TimeBarPane")).
        Bottom());
}

void CEditUIDialog::OnToolBarDrag( wxAuiToolBarEvent& event )
{
    int iID = event.GetToolId();
    switch (iID)
    {
    case ID_ToolBar2_Button1:
        break;
    case ID_ToolBar2_Button2:
        break;
    default:
        break;
    }
}

void CEditUIDialog::OnCtrlsTreeBeginDrag( wxTreeEvent& event )
{
    using namespace FCGUI;
    CComponentTreeItemData* pData = static_cast<CComponentTreeItemData*>(m_pCtrlsTree->GetItemData(event.GetItem()));
    switch (pData->GetGUID())
    {
    case eWT_WINDOW_BUTTON :
        m_pTempGUI = CWindowManager::GetInstance()->Create<CButton>(GetAvailableName(_T("TestButton")), nullptr);
        m_pTempGUI->Renderer()->AddLayer(_T("ButtonNormal"), CButton::eST_STATE_NORMAL);
        m_pTempGUI->Renderer()->AddLayer(_T("ButtonPressed"), CButton::eST_STATE_PRESSED);
        m_pTempGUI->Renderer()->AddLayer(_T("ButtonDisabled"), CButton::eST_STATE_DISABLED);
        m_pTempGUI->SetSize(100, 50);
        break;
    case eWT_WINDOW_CHECKBOX :
        m_pTempGUI = CWindowManager::GetInstance()->Create<CButton>(GetAvailableName(_T("TestCheckBox")), nullptr);
        m_pTempGUI->Renderer()->AddLayer(_T("SlotArmour"));
        m_pTempGUI->SetSize(100, 50);
        break;
    case eWT_WINDOW_SLIDER :

        break;
    case eWT_WINDOW_PROGRESS :

        break;
    case eWT_WINDOW_TEXTEDIT :

        break;
    case eWT_WINDOW_TEXTLABEL :

        break;
    case eWT_WINDOW_LISTBOX :

        break;
    case eWT_WINDOW_COMBOBOX :

        break;
    case eWT_WINDOW_LISTCONTROL :

        break;
    default:
        break;
    }
    event.Allow();
    m_pGLWindow->SetTempGUI(m_pTempGUI);
}

void CEditUIDialog::OnCtrlsTreeEndDrag( wxTreeEvent& /*event*/ )
{
    if (m_pGLWindow->IsMouseInWindow())
    {
        m_pGLWindow->SetActiveGUI(m_pTempGUI);
        //m_pObjectView->AppendItem(NULL, m_pTempGUI->Name(), true, false);
    }
    else
    {
        FCGUI::CWindowManager::GetInstance()->DestroyWindow(m_pTempGUI);
    }
    m_pGLWindow->SetTempGUI(nullptr);
    m_pTempGUI = nullptr;
}

void CEditUIDialog::CreateCtrlsTree()
{
    m_pCtrlsTree = new wxTreeCtrl(m_pCtrlsNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxTR_NO_LINES | wxTR_EDIT_LABELS | wxSUNKEN_BORDER);
    CComponentTreeItemData* itemData = NULL;
    wxTreeItemId rootItem = m_pCtrlsTree->AddRoot(_T("root"));
    
    for (int i = 0; i < FCGUI::eWT_WINDOW_COUNT; i++)
    {
        itemData = new CComponentTreeItemData(false, i);
        m_pCtrlsTree->AppendItem(rootItem, FCGUIName[i], -1, -1, itemData);
    }

    m_pCtrlsNotebook->Freeze();
    m_pCtrlsNotebook->AddPage( m_pCtrlsTree, wxT("Ctrls") );
    m_pCtrlsNotebook->Thaw();

    m_manager.AddPane(m_pCtrlsPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("CtrlsPane")).
        Left().Layer(0));
}

void CEditUIDialog::CreateGUITexture(const TString& strFilePath)
{
    CResourceManager::GetInstance()->GetResource<CTexture>(strFilePath, false);
}

TString CEditUIDialog::GetAvailableName( const TString& strWindowName )
{
    using namespace FCGUI;
    CWindow* pRoot = CWindowManager::GetInstance()->RootWindow();
    TString ResultName;
    int iWindowCount = CWindowManager::GetInstance()->GetWindowCount() + 1;
    for (int i = 0; i < iWindowCount; i++)
    {
        TCHAR szBuffer[32];
        _stprintf(szBuffer, _T("%d"), i);
        ResultName = strWindowName + szBuffer;
        if (pRoot->GetChild(ResultName, true) == nullptr)
        {
            break;
        }
    }
    return ResultName;
}

void CEditUIDialog::OnObjectViewSeletionChange( wxDataViewEvent& /*event*/ )
{
   // using namespace FCGUI;
   // CWindow* pRoot = CWindowManager::GetInstance()->RootWindow();
   // TString str = m_pObjectView->GetUserTreeModel()->GetTitle(event.GetItem());
   // m_pSelectedGUI = pRoot->GetChild(str, true);
}

void CEditUIDialog::OnRightClick( wxDataViewEvent& /*event*/ )
{
    PopupMenu(m_pObjectViewMenu, wxDefaultPosition);
}

void CEditUIDialog::CreateMenu()
{
    m_pObjectViewMenu = new wxMenu;
    m_pObjectViewMenu->Append(Menu_ObjectView_Copy, _T("Copy"));
    m_pObjectViewMenu->Append(Menu_ObjectView_Paste, _T("Paste"));
    m_pObjectViewMenu->Append(Menu_ObjectView_Delete, _T("Delete"));
}

void CEditUIDialog::OnPopMenu( wxCommandEvent& event )
{
    int eID = event.GetId();
    switch (eID)
    {
    case Menu_ObjectView_Copy:
        break;
    case Menu_ObjectView_Paste:
        break;
    case Menu_ObjectView_Delete:
        RemoveSelectedGUI();
        break;
    default:
        break;
    }

}

void CEditUIDialog::RemoveSelectedGUI()
{
    using namespace FCGUI;
    if (m_pSelectedGUI)
    {
//        m_pObjectView->GetUserTreeModel()->Delete(m_pObjectView->GetSelection());
        CWindowManager::GetInstance()->DestroyWindow(m_pSelectedGUI);
        m_pSelectedGUI = NULL;
    }
}

void CEditUIDialog::OnDataViewRenderer( CTimeBarEvent& event )
{
    wxDataViewItem item = event.GetItem();
    CDataViewTreeModelNode *pNode = (CDataViewTreeModelNode*) item.GetID();
    switch (event.GetType())
    {
    case Col_Visible :
        if (pNode->m_cVisible)
        {
            //TODO: show model
        }
        else
        {
            //TODO: hide model
        }
        break;
    case Col_Lock :

        break;
    default:
        break;
    }
}

void CEditUIDialog::LanguageSwitch()
{
    CLanguageManager* pLanguageManager = CLanguageManager::GetInstance()->GetInstance();
    m_pCtrlsNotebook->SetPageText(0, pLanguageManager->GetText(eL_Ctrls));
    m_pTimeBarNotebook->SetPageText(0, pLanguageManager->GetText(eL_TimeBar));
    m_pPropertyNotebook->SetPageText(0, pLanguageManager->GetText(eL_PropGrid));
    m_pGLWindowNotebook->SetPageText(0, pLanguageManager->GetText(eL_GLWindow));
    m_pObjectViewNotebook->SetPageText(0, pLanguageManager->GetText(eL_ObjectView));
    m_pCanvasNotebook->SetPageText(0, pLanguageManager->GetText(eL_Canvas));

    m_pObjectViewMenu->SetLabel(Menu_ObjectView_Copy, pLanguageManager->GetText(eL_Copy));
    m_pObjectViewMenu->SetLabel(Menu_ObjectView_Paste, pLanguageManager->GetText(eL_Paste));
    m_pObjectViewMenu->SetLabel(Menu_ObjectView_Delete, pLanguageManager->GetText(eL_Delete));
}

