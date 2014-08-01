#include "stdafx.h"
#include "EditAnimationDialog.h"
#include "timebarframe.h"
#include "EditorMainFrame.h"
#include "Utility/BeatsUtility/UtilityManager.h"
#include "Render/Texture.h"
#include "Render/SkeletonBone.h"
#include "Render/AnimationController.h"
#include "Resource/ResourceManager.h"
#include "EngineEditor.h"
#include "AnimationGLWindow.h"
#include "Render/RenderManager.h"

#include <wx/statline.h>
#include <wx/artprov.h>

BEGIN_EVENT_TABLE(CEditAnimationDialog, CEditDialogBase)
    EVT_BUTTON(ID_BUTTON_SELECTANIDIR, CEditAnimationDialog::OnSelectFile)
    EVT_BUTTON(ID_BUTTON_SELECTSKEDIR, CEditAnimationDialog::OnSelectFile)
    EVT_BUTTON(ID_BUTTON_SELECTSKIDIR, CEditAnimationDialog::OnSelectFile)
    EVT_BUTTON(ID_BUTTON_CLEANLOG, CEditAnimationDialog::OnClearLog)
    EVT_BUTTON(ID_BUTTON_PLAY, CEditAnimationDialog::OnPlayAnimation)
    EVT_BUTTON(ID_BUTTON_PAUSE, CEditAnimationDialog::OnPauseAnimation)
    EVT_BUTTON(ID_BUTTON_STOP, CEditAnimationDialog::OnStopAnimation)
    EVT_BUTTON(ID_BUTTON_SELECT,CEditAnimationDialog::OnSelectBoneButton)
    EVT_CHECKBOX(ID_CHECKBOX_LOOP,CEditAnimationDialog::OnLoopPlayAnimation)
    EVT_LISTBOX(ID_LISTBOX_ANIMATION, CEditAnimationDialog::OnAnimationListBox)
    EVT_LISTBOX(ID_LISTBOX_SKELETON, CEditAnimationDialog::OnSkeketonListBox)
    EVT_LISTBOX(ID_LISTBOX_SKIN, CEditAnimationDialog::OnSkinListBox)
    EVT_CHOICE(ID_CHOICE_SKELETON, CEditAnimationDialog::OnSkeletonChoice)
    EVT_MENU(wxID_ANY, CEditAnimationDialog::OnPopMenu)
    EVT_CHECKBOX(ID_CHECKBOX_SKELETONVISIBLE,CEditAnimationDialog::OnSkeletonCheckbox)
    EVT_CHECKBOX(ID_CHECKBOX_SKELETONBONEVISIBLE,CEditAnimationDialog::OnSkeletonBoneCheckbox)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_TREECTRL_MODELS, CEditAnimationDialog::OnModelTreeRightClick)
    EVT_TREE_SEL_CHANGED(ID_TREECTRL_MODELS,CEditAnimationDialog::OnModelTreeItemChanged)
END_EVENT_TABLE()

CEditAnimationDialog::CEditAnimationDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : CEditDialogBase(parent, id, title, pos, size, style, name)
    , m_bAnimation(false)
    , m_bSkeleton(false)
    , m_bSkin(false)
    , m_bIsLoop(false)
    , m_bIsVisibleBone(false)
    , m_bIsVisibleCoordinate(false)
    , m_bIsSelectAll(false)
    , m_bISLanguageSwitch(false)
{
    m_Manager.SetManagedWindow(this);
    wxToolTip::Enable(true);
    wxSize ButtonSize(60,0);
    wxSize DefaultButtonSize(DEFAULT_PGBUTTONWIDTH, DEFAULT_PGBUTTONWIDTH);
    wxSize client_size = GetClientSize();
    

    wxBoxSizer* pModelTreeSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTopRightSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTopRightTopSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTopRightCenterSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTopRightBottomSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pLogPanelSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTimeBarPanelSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTimeBarPanelTopSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pTopRightTopBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pTopRightCenterBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pTopRightBottomBottomSizer = new wxBoxSizer(wxHORIZONTAL);

    wxPanel* pLogPanel      = new wxPanel(this);
    wxPanel* pTimeBarPanel  = new wxPanel(this);
    wxPanel* pTopRightPanel = new wxPanel(this);
    wxPanel* pModelTreePanel  = new wxPanel(this);
    wxTextCtrl* pTextCtrl   = new wxTextCtrl(pLogPanel, wxID_ANY, _T(""), wxPoint(0,0), client_size, wxNO_BORDER | wxTE_MULTILINE);
    wxButton* pButton       = new wxButton(pLogPanel, ID_BUTTON_CLEANLOG, _T("X"), wxDefaultPosition, DefaultButtonSize);
    wxImageList *pFileIconImages = new wxImageList(15, 15, true);
    wxIcon fileIcons[eTCIT_Count];
    wxSize iconSize(15, 15);
    fileIcons[eTCIT_File] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_LIST, iconSize);
    fileIcons[eTCIT_FileSelected] = wxArtProvider::GetIcon(wxART_FILE_OPEN, wxART_LIST, iconSize);
    fileIcons[eTCIT_Folder] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_LIST, iconSize);
    fileIcons[eTCIT_FolderSelected] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_LIST, iconSize);
    fileIcons[eTCIT_FolderOpened] = wxArtProvider::GetIcon(wxART_FOLDER_OPEN, wxART_LIST, iconSize);
    for (size_t i = 0; i < eTCIT_Count; ++i)
    {
        pFileIconImages->Add(fileIcons[i]);
    }

    m_pModelTreeMenu    = new wxMenu;
    m_pModelTree        = new wxTreeCtrl(pModelTreePanel, ID_TREECTRL_MODELS, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_NO_LINES | wxTR_EDIT_LABELS | wxSUNKEN_BORDER);
    m_pTimeBar          = new CTimeBarFrame(pTimeBarPanel);
    m_pAnimitionListBox = new wxListBox(pTopRightPanel, ID_LISTBOX_ANIMATION);
    m_pSkeletonListBox  = new wxListBox(pTopRightPanel, ID_LISTBOX_SKELETON, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_MULTIPLE);
    m_pSkinListBox      = new wxListBox(pTopRightPanel, ID_LISTBOX_SKIN);
    m_pSkeletonChoice   = new wxChoice(pTopRightPanel, ID_CHOICE_SKELETON);
    m_pButtonPlay       = new wxButton(pTimeBarPanel, ID_BUTTON_PLAY, _T("|>"), wxDefaultPosition, DefaultButtonSize);
    m_pButtonPause      = new wxButton(pTimeBarPanel, ID_BUTTON_PAUSE, _T("||"), wxDefaultPosition, DefaultButtonSize);
    m_pButtonStop       = new wxButton(pTimeBarPanel, ID_BUTTON_STOP, _T("[]"), wxDefaultPosition, DefaultButtonSize);
    m_pLoopCheckBox     = new wxCheckBox(pTimeBarPanel, ID_CHECKBOX_LOOP, _("Loop"));
    m_pButtonSkin       = new wxButton(pTopRightPanel, ID_BUTTON_SELECTSKIDIR, _T("Skin"));
    m_pButtonAnimaton   = new wxButton(pTopRightPanel, ID_BUTTON_SELECTANIDIR, _T("Animation"));
    m_pSKECheckBox      = new wxCheckBox(pTopRightPanel, ID_CHECKBOX_SKELETONVISIBLE, _T("SKE"));
    m_pSKEBCheckBox     = new wxCheckBox(pTopRightPanel, ID_CHECKBOX_SKELETONBONEVISIBLE, _T("SKEB"));
    m_pSkeSelectButton  = new wxButton(pTopRightPanel, ID_BUTTON_SELECT, _T("SelectAll"), wxDefaultPosition, ButtonSize);
    m_pButtonSkeleton   = new wxButton(pTopRightPanel, ID_BUTTON_SELECTSKEDIR, _T("Skeleton"), wxDefaultPosition, ButtonSize);
    m_pSkeletonCheckBox = new wxCheckBox(pTopRightPanel, wxID_ANY, _T(""));
    m_pSkinCheckBox     = new wxCheckBox(pTopRightPanel, wxID_ANY, _T(""));
    m_pAnimationCheckBox = new wxCheckBox(pTopRightPanel, wxID_ANY, _T(""));
    m_pAnimationGLWindow = new CAnimationGLWindow(this, static_cast<CEngineEditor*>(wxApp::GetInstance())->GetGLContext());
    static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame()->RegisterUpdateWindow(m_pAnimationGLWindow);
    m_pAnimationCheckBox->Disable();
    m_pSkeletonCheckBox->Disable();
    m_pSkinCheckBox->Disable();

    pLogPanel->SetSizer(pLogPanelSizer);
    pTimeBarPanel->SetSizer(pTimeBarPanelSizer);
    pTopRightPanel->SetSizer(pTopRightSizer);
    pModelTreePanel->SetSizer(pModelTreeSizer);

    m_pButtonPlay->SetToolTip(_T("Play"));
    m_pButtonPause->SetToolTip(_T("Pause"));
    m_pButtonStop->SetToolTip(_T("Stop"));
    pButton->SetToolTip(_T("clean log"));

    m_pModelTree->AssignImageList(pFileIconImages);
    m_pModelTree->AddRoot(_T("Models"), eTCIT_Folder);

    m_pModelTreeMenu->Append(ID_MENU_CREATE, _T("Create"));
    m_pModelTreeMenu->Append(ID_MENU_COPY, _T("Copy"));
    m_pModelTreeMenu->Append(ID_MENU_PAST, _T("Paste"));
    m_pModelTreeMenu->Append(ID_MENU_DELETE, _T("Delete"));

    m_pTimeBar->SetFrameWindow(this);
    pTextCtrl->SetEditable(false);
    wxLog::DisableTimestamp();
    m_pLogOld = wxLog::SetActiveTarget(new wxLogTextCtrl(pTextCtrl));
    m_pLog = pTextCtrl;
    
    pModelTreeSizer->Add(m_pModelTree, 1, wxGROW|wxALL, 0);

    pLogPanelSizer->Add(pButton, 0, wxALIGN_RIGHT|wxALL, 0);
    pLogPanelSizer->Add(new wxStaticLine(pLogPanel, wxLI_HORIZONTAL), 0, wxGROW|wxALL, 0);
    pLogPanelSizer->Add(m_pLog, 1, wxGROW|wxALL, 0);
    
    pTimeBarPanelSizer->Add(pTimeBarPanelTopSizer, 0, wxGROW|wxALL, 0);
    pTimeBarPanelSizer->Add(m_pTimeBar, 1, wxGROW|wxALL, 0);
    
    pTimeBarPanelTopSizer->Add(m_pButtonPlay, 0, wxALIGN_RIGHT|wxALL, 0);
    pTimeBarPanelTopSizer->Add(m_pButtonPause, 0, wxALIGN_RIGHT|wxALL, 0);
    pTimeBarPanelTopSizer->Add(m_pButtonStop, 0, wxALIGN_RIGHT|wxALL, 0);
    pTimeBarPanelTopSizer->Add(m_pLoopCheckBox, 0, wxALIGN_RIGHT|wxALL, 0);

    pTopRightSizer->Add(pTopRightTopSizer, 1, wxGROW|wxALL, 5);
    pTopRightSizer->Add(new wxStaticLine(pTopRightPanel, wxLI_HORIZONTAL), 0, wxGROW|wxALL, 5);
    pTopRightSizer->Add(pTopRightCenterSizer, 1, wxGROW|wxALL, 5);
    pTopRightSizer->Add(new wxStaticLine(pTopRightPanel, wxLI_HORIZONTAL), 0, wxGROW|wxALL, 5);
    pTopRightSizer->Add(pTopRightBottomSizer, 1, wxGROW|wxALL, 5);

    pTopRightTopSizer->Add(m_pAnimitionListBox, 1, wxGROW|wxALL, 0);
    pTopRightTopSizer->Add(pTopRightTopBottomSizer, 0, wxGROW|wxALL, 0);

    pTopRightCenterSizer->Add(m_pSkeletonChoice, 0, wxGROW|wxALL, 0);
    pTopRightCenterSizer->Add(m_pSkeletonListBox, 1, wxGROW|wxALL, 1);
    pTopRightCenterSizer->Add(pTopRightCenterBottomSizer, 0, wxGROW|wxALL, 0);

    pTopRightBottomSizer->Add(m_pSkinListBox, 1, wxGROW|wxALL, 0);
    pTopRightBottomSizer->Add(pTopRightBottomBottomSizer, 0, wxGROW|wxALL, 0);

    pTopRightTopBottomSizer->AddStretchSpacer();
    pTopRightTopBottomSizer->Add(m_pAnimationCheckBox, 0, wxGROW|wxALL, 5);
    pTopRightTopBottomSizer->Add(m_pButtonAnimaton, 0, wxGROW|wxALL, 0);
    pTopRightTopBottomSizer->AddStretchSpacer();

    pTopRightCenterBottomSizer->Add(m_pSKECheckBox, 0, wxGROW|wxALL, 0);
    pTopRightCenterBottomSizer->Add(m_pSKEBCheckBox, 0, wxGROW|wxALL, 0);
    pTopRightCenterBottomSizer->AddStretchSpacer();
    pTopRightCenterBottomSizer->Add(m_pSkeletonCheckBox, 0, wxGROW|wxALL, 5);
    pTopRightCenterBottomSizer->Add(m_pButtonSkeleton, 0, wxGROW|wxALL, 0);
    pTopRightCenterBottomSizer->Add(m_pSkeSelectButton, 0, wxGROW|wxALL, 0);
    pTopRightCenterBottomSizer->AddStretchSpacer();

    pTopRightBottomBottomSizer->AddStretchSpacer();
    pTopRightBottomBottomSizer->Add(m_pSkinCheckBox, 0, wxGROW|wxALL, 5);
    pTopRightBottomBottomSizer->Add(m_pButtonSkin, 0, wxGROW|wxALL, 0);
    pTopRightBottomBottomSizer->AddStretchSpacer();

    m_Manager.AddPane(pModelTreePanel, wxAuiPaneInfo().MinSize(260,100).CenterPane().
        Name(wxT("Model")).
        Left());
    m_Manager.AddPane(m_pAnimationGLWindow, wxAuiPaneInfo().CenterPane().
        Name(wxT("GLWindow")).
        Center());
    m_Manager.AddPane(pTopRightPanel, wxAuiPaneInfo().MinSize(260,0).CenterPane().
        Name(wxT("Right")).
        Right());
    m_Manager.AddPane(pTimeBarPanel, wxAuiPaneInfo().MinSize(100,200).CenterPane().
        Name(wxT("Timebar")).
        Bottom());
    m_Manager.AddPane(pLogPanel, wxAuiPaneInfo().CenterPane().
        Name(wxT("Log")).
        Bottom());

    GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Animation));
    GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Skeleton));
    GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Skin));
    wxLogMessage(wxT("This is edit log."));
    m_Manager.Update();
}

CEditAnimationDialog::~CEditAnimationDialog()
{
    m_Manager.UnInit();
    wxLog* p = wxLog::SetActiveTarget(m_pLogOld);
    BEATS_SAFE_DELETE(p);
}

void CEditAnimationDialog::OnSelectFile(wxCommandEvent& event)
{
    if (event.GetId() == ID_BUTTON_SELECTANIDIR)
    {
        m_pAnimitionListBox->Clear();
        GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Animation));
    }
    else if (event.GetId() == ID_BUTTON_SELECTSKEDIR)
    {
        m_pSkeSelectButton->SetLabel(L10N_T(eL_SelectAll));
        SetAllSkeletonAndBoneVisible();
        m_pSkeletonListBox->Clear();
        m_pSkeletonChoice->Clear();
        GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Skeleton));
    }
    else if (event.GetId() == ID_BUTTON_SELECTSKIDIR)
    {
        m_pSkinListBox->Clear();
        GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_Skin));
    }
}

void CEditAnimationDialog::OnClearLog(wxCommandEvent& /*event*/)
{
    m_pLog->Clear();
}

void CEditAnimationDialog::OnPlayAnimation( wxCommandEvent& /*event*/ )
{
    bool bIsOK = true;
    wxString strlog;
    if (!m_pAnimationCheckBox->GetValue())
    {
        bIsOK = false;
        strlog = _T("Animation is not ready \n");
    }
    if (!m_pSkeletonCheckBox->GetValue())
    {
        bIsOK = false;
        strlog = _T("Skeleton is not ready \n");
    }
    if (!m_pSkinCheckBox->GetValue())
    {
        bIsOK = false;
        strlog = _T("Skin is not ready \n");
    }

    if (bIsOK)
    {
        m_pAnimationGLWindow->GetModel()->PlayAnimationById(0, 0, m_bIsLoop);
    }
    else
    {
        wxLogMessage(strlog);
    }
}

void CEditAnimationDialog::OnPauseAnimation( wxCommandEvent& /*event*/ )
{
    CModel* pModel = m_pAnimationGLWindow->GetModel();
    if (pModel)
    {
        if (pModel->GetAnimationController()->IsPlaying())
        {
            pModel->GetAnimationController()->Pause();
        }
        else
        {
            pModel->GetAnimationController()->Resume();
        }
    }
}

void CEditAnimationDialog::OnStopAnimation( wxCommandEvent& /*event*/ )
{
    CModel* pModel = m_pAnimationGLWindow->GetModel();
    if (pModel)
    {
        pModel->GetAnimationController()->Stop();
        m_pTimeBar->SetCurrentCursor(0);
    }
}

void CEditAnimationDialog::OnAnimationListBox( wxCommandEvent& event )
{
    TString strFileName = event.GetString();
    wxString logMessage;
    m_pAnimation = CResourceManager::GetInstance()->GetResource<CAnimation3D>(strFileName);
    if (m_pAnimation)
    {
        CModel* pModel = m_pAnimationGLWindow->GetModel();
        if (pModel)
        {
            pModel->SetAnimaton(m_pAnimation);
            m_pAnimationCheckBox->SetValue(true);
            logMessage = strFileName;
            logMessage.append(_T(" load success"));
            m_bAnimation = true;
            ShowAnima();
        }
        else
        {
            logMessage = "No model is selected!";
        }
    }
    else
    {
        m_pAnimationCheckBox->SetValue(false);
        logMessage = WARNING + strFileName + _T(" load failed");
    }
    wxLogMessage(logMessage);
}

void CEditAnimationDialog::OnSkeketonListBox( wxCommandEvent& /*event*/ )
{
    SetAllSkeletonAndBoneVisible();
    SetSkeletonAndBoneVisible(m_bIsVisibleBone,m_bIsVisibleCoordinate);
}

void CEditAnimationDialog::OnSkeletonChoice( wxCommandEvent& event )
{
    if (m_pSkeletonListBox->GetCount() > 0)
    {
        m_pSkeletonListBox->Clear();
        SetAllSkeletonAndBoneVisible();
    }
    m_skeletonBoneType.clear();
    TString strFileName = event.GetString();
    wxString logMessage;
    m_pSkeleton = CResourceManager::GetInstance()->GetResource<CSkeleton>(strFileName);
    if (m_pSkeleton)
    {
        CModel* pModel = m_pAnimationGLWindow->GetModel();
        if (pModel)
        {
            pModel->SetSkeleton(m_pSkeleton);
            m_pSkeletonCheckBox->SetValue(true);
            logMessage = strFileName + _T(" load success");
            m_bSkeleton = true;
            ShowAnima();
            for (auto iter = m_pSkeleton->GetBoneMap().begin(); iter != m_pSkeleton->GetBoneMap().end(); iter++)
            {
                if (iter->first != eSBT_Null)
                {
                    m_pSkeletonListBox->Append(pszSkeletonBoneName[iter->first + 1]);
                    m_skeletonBoneType.push_back(iter->first);
                }
            }
        }
        else
        {
            logMessage = "No model is selected!";
        }
    }
    else
    {
        m_pSkeletonCheckBox->SetValue(false);
        logMessage = WARNING + strFileName + _T(" load failed");
    }
    wxLogMessage(logMessage);
}

void CEditAnimationDialog::OnSkinListBox( wxCommandEvent& event )
{
    TString strFileName = event.GetString();
    wxString logMessage;
    m_pSkin = CResourceManager::GetInstance()->GetResource<CSkin>(strFileName);
    if (m_pSkin)
    {
        CModel* pModel = m_pAnimationGLWindow->GetModel();
        if (pModel)
        {
            pModel->SetSkin(m_pSkin);
            m_pSkinCheckBox->SetValue(true);
            logMessage = strFileName + _T(" load success");

            m_bSkin = true;
            ShowAnima();
        }
        else
        {
            logMessage = "No model is selected!";
        }
    }
    else
    {
        m_pSkinCheckBox->SetValue(false);
        logMessage = WARNING + strFileName + _T(" load failed");
    }
    wxLogMessage(logMessage);
}

void CEditAnimationDialog::GetResourceList( wxString path)
{
    SDirectory directory(NULL, path.c_str());
    CUtilityManager::GetInstance()->FillDirectory(directory, false, NULL);  
    for (auto iter = directory.m_pFileList->begin(); iter != directory.m_pFileList->end(); iter++)  
    {  
        wxString temp = (*iter)->cFileName;
        temp.MakeLower();
        if (temp.EndsWith(_T(".ani")))
        {
            m_pAnimitionListBox->Append((*iter)->cFileName);
        }
        else if (temp.EndsWith(_T(".ske")))
        {
            m_pSkeletonChoice->Append((*iter)->cFileName);
        }
        else if (temp.EndsWith(_T(".skin")))
        {
            m_pSkinListBox->Append((*iter)->cFileName);
        }
    }
}

void CEditAnimationDialog::InitTimeBar()
{
    if (m_pTimeBar->GetItemCount() > 1)
    {
        m_pTimeBar->DeleteItem(0);
    }
}

void CEditAnimationDialog::ShowAnima()
{
    if (IsLoadAllAnimationResource())
    {
        InitTimeBar();
        m_pAnimationGLWindow->GetModel()->PlayAnimationById(0, 0, true);
        m_pAnimationGLWindow->GetModel()->GetAnimationController()->GoToFrame(0);
    }
}

CTimeBarFrame* CEditAnimationDialog::GetTimeBar()
{
    return m_pTimeBar;
}

void CEditAnimationDialog::OnLoopPlayAnimation( wxCommandEvent& event )
{
    m_bIsLoop = event.IsChecked();
}

SharePtr<CAnimation3D> CEditAnimationDialog::GetAnimation()
{
    return m_pAnimation;
}

bool CEditAnimationDialog::IsLoadAllAnimationResource()
{
    return m_bAnimation && m_bSkeleton && m_bSkin;
}

void CEditAnimationDialog::SetAllSkeletonAndBoneVisible(bool bSke, bool bSkeBone)
{
    for (auto iter = m_pSkeleton->GetBoneMap().begin(); iter != m_pSkeleton->GetBoneMap().end(); iter++)
    {
        m_pSkeleton->GetSkeletonBone(iter->first)->SetVisible(bSke);
        m_pSkeleton->GetSkeletonBone(iter->first)->SetCoordinateVisible(bSkeBone);
    }
}

void CEditAnimationDialog::SetSkeletonAndBoneVisible(bool bSke,bool bSkeBone)
{
    wxArrayInt    selections = NULL;
    int index = 0;
    int count = m_pSkeletonListBox->GetSelections(selections);
    for (auto i = 0; i != count; i++)
    {
        index = selections.Item(i);
        m_pSkeleton->GetSkeletonBone(m_skeletonBoneType[index])->SetVisible(bSke);
        m_pSkeleton->GetSkeletonBone(m_skeletonBoneType[index])->SetCoordinateVisible(bSkeBone);
    }
}

void CEditAnimationDialog::OnSelectBoneButton( wxCommandEvent& event )
{
    wxButton *clicked = (wxButton *)event.GetEventObject();
    if(m_pSkeletonListBox->GetCount() > 0)
    {
        if (clicked->GetLabelText() == _T("SelectAll") ||
            clicked->GetLabelText() == L10N_T(eL_SelectAll))
        {
            clicked->SetLabel( m_bISLanguageSwitch ? L10N_T(eL_DeselectAll) : _T("Deselect"));
            m_bIsSelectAll = true;
            for(unsigned i = 0; i != m_pSkeletonListBox->GetCount(); i++)
            {
                m_pSkeletonListBox->SetSelection(i);
            }
            SetAllSkeletonAndBoneVisible(m_bIsVisibleBone,m_bIsVisibleCoordinate);
        }
        else
        {
            clicked->SetLabel(m_bISLanguageSwitch ? L10N_T(eL_SelectAll) : _T("SelectAll"));
            m_bIsSelectAll = false;
            SetAllSkeletonAndBoneVisible();
            DelListboxSelect();
        }
    }
}

void CEditAnimationDialog::OnSkeletonCheckbox( wxCommandEvent& event )
{
    m_bIsVisibleBone = event.IsChecked();
    SetSkeletonAndBoneVisible(m_bIsVisibleBone, m_bIsVisibleCoordinate);
}

void CEditAnimationDialog::OnSkeletonBoneCheckbox( wxCommandEvent& event )
{
    m_bIsVisibleCoordinate = event.IsChecked();
    SetSkeletonAndBoneVisible(m_bIsVisibleBone, m_bIsVisibleCoordinate);
}

void CEditAnimationDialog::DelListboxSelect()
{
    for (unsigned i = 0; i != m_pSkeletonListBox->GetCount(); i++)
    {
        m_pSkeletonListBox->Deselect(i);
    }
}

CAnimationGLWindow* CEditAnimationDialog::GetCanvas() const
{
    return m_pAnimationGLWindow;
}

void CEditAnimationDialog::LanguageSwitch()
{
    m_bISLanguageSwitch = true;
    m_pButtonAnimaton->SetLabel(L10N_T(eL_Animation));
    m_pSKECheckBox->SetLabel(L10N_T(eL_SKE));
    m_pSKEBCheckBox->SetLabel(L10N_T(eL_SKEB));
    m_pButtonSkeleton->SetLabel(L10N_T(eL_Skeleton));
    m_pButtonSkin->SetLabel(L10N_T(eL_Skin));

    m_pButtonPlay->SetToolTip(L10N_T(eL_Play));
    m_pButtonPause->SetToolTip(L10N_T(eL_Pause));
    m_pButtonStop->SetToolTip(L10N_T(eL_Stop));

    m_pLoopCheckBox->SetLabel(L10N_T(eL_Loop));
    m_pSkeSelectButton->SetLabel(L10N_T(m_bIsSelectAll ? eL_DeselectAll : eL_SelectAll));
}

int CEditAnimationDialog::ShowModal()
{
    wxWindow* pParent = GetParent();
    if (pParent != NULL)
    {
        wxRect rect = pParent->GetRect();
        wxPoint pnt = rect.GetPosition();
        SetSize(rect.GetSize());
        SetPosition(pnt);
    }

    return super::ShowModal(); 
}

void CEditAnimationDialog::SetLanguage()
{
    if (m_bIsSelectAll)
    {
        m_pSkeSelectButton->SetLabel(L10N_T(eL_DeselectAll));
    } 
    else
    {
        m_pSkeSelectButton->SetLabel(L10N_T(eL_SelectAll));
    }
}

void CEditAnimationDialog::OnModelTreeRightClick( wxTreeEvent& /*event*/ )
{
    PopupMenu(m_pModelTreeMenu, wxDefaultPosition);
}

void CEditAnimationDialog::OnPopMenu( wxCommandEvent& event )
{
    int iID = event.GetId();
    switch (iID)
    {
    case ID_MENU_CREATE :
        CreateNewModel();
        break;
    case ID_MENU_COPY :

        break;
    case ID_MENU_PAST :
        
        break;
    case ID_MENU_DELETE :
        DeleteModel();
        break;
    default:
        break;
    }
}


bool CEditAnimationDialog::CheckIsNameValid( const TString& strWindowName )
{
    for (auto iter : m_ModelNameMap)
    {
        if (m_pModelTree->GetItemText(iter.first) == strWindowName)
        {
            return false;
        }
    }
    return true;
}


TString CEditAnimationDialog::GetAvailableName( const TString& strWindowName )
{
    TString ResultName = strWindowName;
    int iCount = m_ModelNameMap.size();
    for (int i = 0; i < iCount; i++)
    {
        if (CheckIsNameValid(ResultName))
        {
            break;
        }
        TCHAR szBuffer[32];
        _stprintf(szBuffer, _T("%d"), i);
        ResultName = strWindowName + szBuffer;
    }
    return ResultName;
}

void CEditAnimationDialog::CreateNewModel()
{
    TString ModelName = _T("NewModel");
    wxTreeItemId item = AddModelToTree(GetAvailableName(ModelName));
    SelectModel(item);
    ClearResourceLoadFlag();
}

wxTreeItemId CEditAnimationDialog::AddModelToTree( TString ModelName )
{
    wxTreeItemId item = m_pModelTree->AppendItem(m_pModelTree->GetRootItem(), ModelName, eTCIT_File);
    CModel* pNewModel = new CModel;
    pNewModel->Initialize();
    m_ModelNameMap[item] = pNewModel;
    return item;
}

void CEditAnimationDialog::DeleteModel( wxTreeItemId& item )
{
    auto iter = m_ModelNameMap.find(item);
    if (iter != m_ModelNameMap.end())
    {
        m_pAnimationGLWindow->SetModel(NULL);
        m_ModelNameMap.erase(iter);
    }
}

void CEditAnimationDialog::DeleteModel()
{
    wxTreeItemId item = m_pModelTree->GetSelection();
    DeleteModel(item);
    m_pModelTree->Delete(item);
}

void CEditAnimationDialog::SelectModel( wxTreeItemId item )
{
    if (m_ModelNameMap.find(item) != m_ModelNameMap.end())
    {
        m_pAnimationGLWindow->SetModel(m_ModelNameMap[item]);
    }
    m_pModelTree->SelectItem(item);
    ClearResourceLoadFlag();
}

void CEditAnimationDialog::OnModelTreeItemChanged( wxTreeEvent& event )
{
    SelectModel(event.GetItem());
}

void CEditAnimationDialog::ClearResourceLoadFlag()
{
    m_bAnimation = m_bSkeleton = m_bSkin = false;
    m_pAnimationCheckBox->SetValue(false);
    m_pSkeletonCheckBox->SetValue(false);
    m_pSkinCheckBox->SetValue(false);
    m_pAnimitionListBox->Select(INVALID_DATA);
    m_pSkeletonListBox->Select(INVALID_DATA);
    m_pSkinListBox->Select(INVALID_DATA);
}
