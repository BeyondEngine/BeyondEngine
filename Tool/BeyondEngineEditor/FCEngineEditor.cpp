#include "stdafx.h"
#include <wx/artprov.h>
#include "BeyondEngineEditor.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "ConstantCurveProperty.h"
#include "ConstantCurveEditor.h"
#include "EditAnimationDialog.h"
#include "timebarframe.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentEditorProxy.h"

#define MAINFRAMESIZE wxSize(1024, 800)
#define MAINFRAMEPOSITION wxPoint(40, 40)
#define BUTTONSIZE wxSize(20, 20)
#define SIZERBORDERWIDTH 5
enum 
{
    PARTICLE_SIMULATION_WORLD = 1,
    PARTICLE_SIMULATION_LOCAL
};

enum
{
    Menu_Menu_Append,
    Menu_EditAnimation = 1111
};

BEGIN_EVENT_TABLE(CEditorMainFrame, wxFrame)
EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, CEditorMainFrame::OnAuiButton)
EVT_MENU(Menu_EditAnimation, CEditorMainFrame::OnEditAnimationMenuItem)
EVT_TREE_ITEM_ACTIVATED(wxID_ANY, CEditorMainFrame::OnTreeClick)
END_EVENT_TABLE()

CEditorMainFrame::CEditorMainFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title, MAINFRAMEPOSITION, MAINFRAMESIZE)
{
    SetIcon(wxICON(sample));
    m_Manager.SetManagedWindow(this);
    m_pWEditAnimation = NULL;
    InitMenu();
    InitCtrls();

    SetStatusBar(new wxStatusBar(this));
    SetStatusText(wxT("Welcome!"));
    SetMinSize(wxSize(400,300));

    Centre();
 }

CEditorMainFrame::~CEditorMainFrame()
{
    m_Manager.UnInit();
}

void CEditorMainFrame::InitMenu()
{
    wxMenuBar* pMenuBar     = new wxMenuBar;
    wxMenu* pFileMenu       = new wxMenu;
    wxMenu* pEditMenu       = new wxMenu;
    wxMenu* pAssetsMenu     = new wxMenu;
    wxMenu* pGameObjMenu    = new wxMenu;
    wxMenu* pConponentMenu  = new wxMenu;
    wxMenu* pWindowMenu     = new wxMenu;
    wxMenu* pHelpMenu       = new wxMenu;
    
    pMenuBar->Append(pFileMenu, wxT("&File"));
    pMenuBar->Append(pEditMenu, wxT("&Edit"));
    pMenuBar->Append(pAssetsMenu, wxT("&Assets"));
    pMenuBar->Append(pGameObjMenu, wxT("&GameObject"));
    pMenuBar->Append(pConponentMenu, wxT("&Conponent"));
    pMenuBar->Append(pWindowMenu, wxT("&Window"));
    pMenuBar->Append(pHelpMenu, wxT("&Help"));

    pEditMenu->Append(Menu_EditAnimation, wxT("Edit Animation"));

    SetMenuBar(pMenuBar);
}

void CEditorMainFrame::InitCtrls()
{
    CreateAuiToolBar();
    CreateAuiNoteBook();
    CreateTreeCtrl();
    CreateGLCanvas();
    CreateTimeBar();
    CreatePropertyGrid();
    CreatSplitter();

    AddPageToBook();
    AddTreeItem();
    m_Manager.Update();
}

void CEditorMainFrame::CreateAuiToolBar()
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
        wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
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

    m_Manager.AddPane(tb1,
        wxAuiPaneInfo().Name(wxT("tb1")).
        ToolbarPane().Top().Position(0).
        LeftDockable(false).RightDockable(false).Dockable(false));
    m_Manager.AddPane(tb2,
        wxAuiPaneInfo().Name(wxT("tb2")).
        ToolbarPane().Top().Position(1).
        LeftDockable(false).RightDockable(false).Dockable(false));
}

void CEditorMainFrame::CreateAuiNoteBook()
{
    wxSize client_size = GetClientSize();
    long lStyle = wxAUI_NB_DEFAULT_STYLE & ~wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
    m_pLeft = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, client_size, lStyle);
    m_pRight = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, client_size, lStyle);
    m_pBottom = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, client_size, lStyle);
    m_pCenter = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, client_size, lStyle);
}

void CEditorMainFrame::CreateGLCanvas()
{
}

void CEditorMainFrame::CreateTimeBar()
{
    m_pTimeBar = new TimeBarFrame(m_pBottom);
}

void CEditorMainFrame::CreatePropertyGrid()
{
    int style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPG_TOOLTIPS;
    int extraStyle = wxPG_EX_MODE_BUTTONS | wxPG_EX_MULTIPLE_SELECTION;

    m_pPropGridManager = new EnginePropertyGirdManager();
    m_pPropGridManager->Create(m_pRight, wxID_ANY, wxDefaultPosition, wxSize(100, 100), style );
    m_propGrid = m_pPropGridManager->GetGrid();
    m_pPropGridManager->SetExtraStyle(extraStyle);
    m_pPropGridManager->SetValidationFailureBehavior( wxPG_VFB_MARK_CELL | wxPG_VFB_SHOW_MESSAGEBOX );
    m_propGrid->SetVerticalSpacing( 2 );
    m_pPropGridManager->AddPage(wxT("page"));
    m_pPropGridManager->InitComponentsPage();
}

void CEditorMainFrame::CreateTreeCtrl()
{
    wxImageList* imglist = new wxImageList(16, 16, true, 2);
    imglist->Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16,16)));
    imglist->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16)));
    m_pComponentModelTC = new wxTreeCtrl(m_pLeft, wxID_ANY,
        wxPoint(0,0), wxSize(160,250),
        wxTR_DEFAULT_STYLE | wxNO_BORDER);
    m_pComponentModelTC->AssignImageList(imglist);
    m_pComponentModelTC->AddRoot(wxT("Components"), 0);

    imglist = new wxImageList(16, 16, true, 2);
    imglist->Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16,16)));
    imglist->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16)));
    m_pCurComponentTC = new wxTreeCtrl(m_pLeft, wxID_ANY,
        wxPoint(0,0), wxSize(160,250),
        wxTR_DEFAULT_STYLE | wxNO_BORDER);    
    m_pCurComponentTC->AssignImageList(imglist);
    m_pCurComponentTC->AddRoot(wxT("Components"), 0);
}

void CEditorMainFrame::AddPageToBook()
{
    m_pLeft->Freeze();
    m_pLeft->AddPage( m_pComponentModelTC, wxT("Component Model") );
    m_pLeft->AddPage( m_pCurComponentTC, wxT("Current Conponent") );
    m_pLeft->Thaw();

    m_pRight->Freeze();
    m_pRight->AddPage( m_pPropGridManager, wxT("Inspector") );
    m_pRight->Thaw();

    m_pBottom->Freeze();
    m_pBottom->AddPage( new wxTextCtrl( m_pBottom, wxID_ANY, wxT("Some more text")), wxT("wxTextCtrl 1") );
    m_pBottom->AddPage( m_pTimeBar, wxT("wxTextCtrl 2") );
    m_pBottom->Thaw();

    m_pCenter->Freeze();
    m_pCenter->AddPage( m_pSplitter, wxT("wxTextCtrl 1") );
    m_pCenter->Thaw();

    m_Manager.AddPane(m_pLeft, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane1")).
        Left());
    m_Manager.AddPane(m_pRight, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane2")).
        Right());
    m_Manager.AddPane(m_pBottom, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane3")).Caption(wxT("Pane Caption")).
        Bottom());
    m_Manager.AddPane(m_pCenter, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane4")).Caption(wxT("Pane Caption")).
        Center());
}

void CEditorMainFrame::GetEditAnimationDialog()
{
    if (m_pWEditAnimation == NULL)
    {
        m_pWEditAnimation = new EditAnimationDialog(this, wxID_ANY, wxT("Edit Animation"));
    }
    m_pWEditAnimation->ShowModal();
}

void CEditorMainFrame::OnAuiButton(wxCommandEvent& event)
{
    int id = event.GetId();
    switch (id)
    {
    case Menu_EditAnimation :
        GetEditAnimationDialog();
        break;
    default:
        break;
    }
    return;
}

void CEditorMainFrame::OnEditAnimationMenuItem(wxCommandEvent& /*event*/)
{
    return;
}

void CEditorMainFrame::CreatSplitter()
{
    m_pSplitter = new wxSplitterWindow(m_pCenter);
    m_pSplTop = new wxTextCtrl(m_pSplitter,wxID_ANY);
    m_pSplBottom = new wxTextCtrl(m_pSplitter,wxID_ANY);
    m_pSplitter->SplitHorizontally(m_pSplTop, m_pSplBottom, 200);
}

void CEditorMainFrame::AddTreeItem()
{
    const std::map<size_t, CComponentBase*>* pComponentsMap = CComponentManager::GetInstance()->GetComponentTemplateMap();
    for (auto iter : *pComponentsMap)
    {
        TString strName = ((CComponentEditorProxy*)iter.second)->GetCatalogName();
        std::vector<TString> vecName;
        CStringHelper::GetInstance()->SplitString(strName.c_str(), wxT("\\"), vecName);
        wxTreeItemId idParant = m_pComponentModelTC->GetRootItem();
        AddChilditemToItem(idParant, vecName, 0);
    }
    m_pComponentModelTC->ExpandAll();
}

void CEditorMainFrame::AddChilditemToItem( wxTreeItemId& idParent, std::vector<TString>& vecName, size_t iLevel )
{
    if (vecName.size() > iLevel)
    {
        TString str = vecName[iLevel];
        wxTreeItemIdValue cookie;
        wxTreeItemId idChild = m_pComponentModelTC->GetFirstChild(idParent, cookie);
        wxTreeItemId idLastChild = m_pComponentModelTC->GetLastChild(idParent);
        if (!idChild.IsOk())
        {
            idChild = m_pComponentModelTC->AppendItem(idParent, str, iLevel);
        }
        else
        {
            while (idChild.IsOk() && str != m_pComponentModelTC->GetItemText(idChild))
            {
                if (idLastChild == idChild)
                {
                    idChild = m_pComponentModelTC->AppendItem(idParent, str, iLevel);
                    break;
                }
                idChild = m_pComponentModelTC->GetNextChild(idParent, cookie);
            }
        }
        if (idChild.IsOk())
        {
            AddChilditemToItem(idChild, vecName, ++iLevel);
        }
    }
}

void CEditorMainFrame::OnTreeClick( wxTreeEvent& event )
{
    wxTreeItemId itemId = event.GetItem();
    wxTreeItemId root = m_pComponentModelTC->GetRootItem();
    TString strItemName;
    while (itemId != root)
    {
        if (strItemName == "")
        {
            strItemName = m_pComponentModelTC->GetItemText(itemId);
        }
        else
        {
            strItemName = m_pComponentModelTC->GetItemText(itemId) + "\\" + strItemName;
        }
        
        itemId = m_pComponentModelTC->GetItemParent(itemId);
    }

    const std::map<size_t, CComponentBase*>* pComponentsMap = CComponentManager::GetInstance()->GetComponentTemplateMap();
    for (auto iter : *pComponentsMap)
    {
        TString strName = ((CComponentEditorProxy*)iter.second)->GetCatalogName();
        if (strName == strItemName)
        {
            CComponentEditorProxy* pComProxy = (CComponentEditorProxy*)iter.second;
            m_pPropGridManager->ClearPage(0);
            m_pPropGridManager->InsertComponentsInPropertyGrid(pComProxy);
        }
    }
    
}
