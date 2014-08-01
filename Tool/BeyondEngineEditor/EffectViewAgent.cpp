#include "stdafx.h"
#include "EffectViewAgent.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "ComponentInstanceTreeItemData.h"
#include "wx/artprov.h"
#include "timebarframe.h"
#include "ParticlesSystem/ParticleSystem.h"
#include "ParticlesSystem/Emitter.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescriptionLine.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"

CEffectViewAgent* CEffectViewAgent::m_pInstance = nullptr;
CEffectViewAgent::CEffectViewAgent()
    : m_pEffectTreeCtrl(nullptr)
    , m_pEmitButton(nullptr)
    , m_pEffectTreeMenu(nullptr)
    , m_nPatricleSystemGuid(INVALID_DATA)
    , m_nPatricleEmitterGuid(INVALID_DATA)
    , m_n3DNodeGuid(INVALID_DATA)
{
    m_pEffectTreeMenu = new wxMenu;
    m_pEffectTreeMenu->Append(ePM_EffectTree_Add, _T("添加"));
    m_pEffectTreeMenu->Append(ePM_EffectTree_Delete, _T("删除"));

    InitializeCtrls();
    auto pTemplateMap = CComponentProxyManager::GetInstance()->GetComponentTemplateMap();
    CComponentProxy* pProxy = NULL;
    for (auto iter = pTemplateMap->begin(); iter != pTemplateMap->end(); ++iter)
    {
        pProxy = (CComponentProxy*)iter->second;
        wxString name = pProxy->GetDisplayName();
        if (name == _T("粒子系统"))
        {
            m_nPatricleSystemGuid = pProxy->GetGuid();
        }
        else if (name == _T("发射器"))
        {
            m_nPatricleEmitterGuid = pProxy->GetGuid();
        }
        else if(name == _T("3D节点"))
        {
            m_n3DNodeGuid = pProxy->GetGuid();
        }
    }
    BEATS_ASSERT(m_nPatricleSystemGuid != INVALID_DATA);
    BEATS_ASSERT(m_nPatricleEmitterGuid != INVALID_DATA);
    BEATS_ASSERT(m_n3DNodeGuid != INVALID_DATA);
}

CEffectViewAgent::~CEffectViewAgent()
{

}

void CEffectViewAgent::ProcessMouseEvent( wxMouseEvent& event )
{
    event.Skip();
}

void CEffectViewAgent::InView()
{
    CreateTools();
    m_pMainFrame->m_nSashPosition = m_pMainFrame->m_pSplitter->GetSashPosition();
    m_pMainFrame->m_pSplitter->Unsplit();
    
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pLeftPanel).Hide();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pResourcePanel).Hide();

    m_pMainFrame->m_Manager.GetPane(m_pEffectsPanel).Show();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pBottomPanel).Show();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pToolPanel).Show();

    OnOpenComponentFile();
    m_pMainFrame->m_Manager.Update();
}

void CEffectViewAgent::OutView()
{
    m_pMainFrame->m_pToolBook->DeleteAllPages();
    m_pMainFrame->m_pSplitter->SplitHorizontally((wxWindow*)m_pMainFrame->m_pViewScreen, (wxWindow*)m_pMainFrame->m_pComponentRenderWindow, m_pMainFrame->m_nSashPosition);

    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pLeftPanel).Show();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pResourcePanel).Show();
    
    m_pMainFrame->m_Manager.GetPane(m_pEffectsPanel).Hide();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pBottomPanel).Hide();
    m_pMainFrame->m_Manager.GetPane(m_pMainFrame->m_pToolPanel).Hide();

    m_pMainFrame->m_Manager.Update();
}

void CEffectViewAgent::UpdateEffectTree()
{
    m_pEffectTreeCtrl->DeleteChildren(m_pEffectTreeCtrl->GetRootItem());
    const std::map<size_t, std::map<size_t, CComponentBase*>*>* pInstanceMap = CComponentProxyManager::GetInstance()->GetComponentInstanceMap();
    for (std::map<size_t, std::map<size_t, CComponentBase*>*>::const_iterator iter = pInstanceMap->begin(); iter != pInstanceMap->end(); ++iter)
    {
        TString strName = CComponentProxyManager::GetInstance()->QueryComponentName(iter->first);
        for (std::map<size_t, CComponentBase*>::const_iterator subIter = iter->second->begin(); subIter != iter->second->end(); ++subIter)
        {
            CComponentProxy* pProxy = (CComponentProxy*)subIter->second;
            wxString strIdLabel = pProxy->GetDisplayName();
            if(strIdLabel == _T("粒子系统"))
            {
                wxString name = pProxy->GetUserDefineDisplayName();
                if (name.IsNull())
                {
                    name = pProxy->GetDisplayName();
                }
                CComponentInstanceTreeItemData* pComponentInstanceItemData = new CComponentInstanceTreeItemData(subIter->second);
                m_pEffectTreeCtrl->AppendItem(m_pEffectTreeCtrl->GetRootItem(), name, eTCIT_File, -1, pComponentInstanceItemData);
            }
        }
    }
    m_pEffectTreeCtrl->ExpandAll();
}

void CEffectViewAgent::UpdateTimeBar()
{
    m_pMainFrame->m_pTimeBar->Clear();
    wxTreeItemId id = m_pEffectTreeCtrl->GetSelection();
    if (id.IsOk())
    {
        CComponentInstanceTreeItemData* pData = (CComponentInstanceTreeItemData*)m_pEffectTreeCtrl->GetItemData(id);
        CComponentBase* pComponentBase = pData->GetComponentBase();
        if (pComponentBase)
        {
            CComponentProxy* pProxy = (CComponentProxy*)pComponentBase;
            //search emitter dependency from DependencyList
            CDependencyDescription* pDependency = GetDependency(pProxy, _T("粒子发射器"));
            BEATS_ASSERT(pDependency);
            wxString dependencyName = pDependency->GetDisplayName();
            int nDependencyLineCount = pDependency->GetDependencyLineCount();
            for (int j = 0; j < nDependencyLineCount; j++)
            {
                //add all emitter ComponentProxy from dependency to TimeBar
                CDependencyDescriptionLine* pLine = pDependency->GetDependencyLine(j);
                CComponentProxy* pComponentEmitter = pLine->GetConnectedComponent();
                wxString name = pComponentEmitter->GetUserDefineDisplayName();
                if (name.IsNull())
                {
                    name = pComponentEmitter->GetDisplayName();
                }
                CPropertyDescriptionBase* pProperty = pComponentEmitter->GetPropertyDescription(_T( "m_fDelayTime" ));
                float fDelayTime = *(float*)pProperty->GetValue(eVT_CurrentValue);
                pProperty = pComponentEmitter->GetPropertyDescription(_T( "m_fEmitterLiveTime" ));
                float fLifeTime = *(float*)pProperty->GetValue(eVT_CurrentValue);
                m_pMainFrame->m_pTimeBar->AddItem(name, wxPtrToUInt(pComponentEmitter), fDelayTime * 10, fLifeTime * 10);
            }
        }
    }
}

void CEffectViewAgent::OnOpenComponentFile()
{
    UpdateCtrl();
}

void CEffectViewAgent::OnTreeCtrlSelect(wxTreeEvent& event)
{
    using namespace NBeyondEngine;
    bool bEnable = false;
    if (event.GetId() == Ctrl_Tree_Effects)
    {
        wxTreeItemId id = m_pEffectTreeCtrl->GetSelection();
        if (id.IsOk())
        {
            CComponentInstanceTreeItemData* pData = (CComponentInstanceTreeItemData*)m_pEffectTreeCtrl->GetItemData(id);
            CComponentBase* pComponentBase = pData->GetComponentBase();
            if (pComponentBase)
            {
                bEnable = true;
                
            }
        }
        UpdateTimeBar();
    }
    m_pEmitButton->Enable(bEnable);
}

void CEffectViewAgent::OnCloseComponentFile()
{
    wxTreeItemId rootItem = m_pEffectTreeCtrl->GetRootItem();
    m_pEffectTreeCtrl->DeleteChildren(rootItem);
    m_pEffectTreeCtrl->SetItemImage(rootItem, eTCIT_Folder);
    m_pMainFrame->HideTreeCtrl(m_pEffectTreeCtrl);
}

void CEffectViewAgent::CreateTools()
{
    wxPanel* pPanel = new wxPanel(m_pMainFrame->m_pToolBook);
    wxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    pPanel->SetSizer(pSizer);

    m_pEmitButton = new wxButton(pPanel, Ctrl_Button_BeginEmit, wxT("Begin Emit"));
    m_pEmitButton->Enable(false);
    m_pEmitButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CEditorMainFrame::OnCommandEvent), NULL, m_pMainFrame);
    pSizer->Add(m_pEmitButton, 0, wxGROW|wxALL, 0);

    m_pMainFrame->m_pToolBook->AddPage(pPanel, wxT("Tools"));
    m_pMainFrame->m_Manager.Update();
}

void CEffectViewAgent::OnCommandEvent(wxCommandEvent& event)
{
    int nID = event.GetId();
    switch (nID)
    {
    case Ctrl_Button_BeginEmit:
        OnBeginButton();
        break;
    case ePM_EffectTree_Add:
        {
            CComponentProxy* pProxy = CreateNewParticleSystem();
            wxString name = pProxy->GetUserDefineDisplayName();
            if (name.IsNull())
            {
                name = pProxy->GetDisplayName();
            }
            CComponentInstanceTreeItemData* pComponentInstanceItemData = new CComponentInstanceTreeItemData(pProxy);
            m_pEffectTreeCtrl->AppendItem(m_pEffectTreeCtrl->GetRootItem(), name, eTCIT_File, -1, pComponentInstanceItemData);
        }
        break;
    case ePM_EffectTree_Delete:
        DeleteSelectedParticleSystem();
        UpdateCtrl();
        break;
    case Ctrl_TimeBar_Button_Add:
        CreateNewParticleEmitter();
        UpdateTimeBar();
        break;
    case Ctrl_TimeBar_Button_Delete:
        DeleteSelectedParticleEmitter();
        UpdateTimeBar();
        break;
    default:
        break;
    }
}

void CEffectViewAgent::OnBeginButton()
{
    wxTreeItemId id = m_pEffectTreeCtrl->GetSelection();
    if (id.IsOk())
    {
        CComponentInstanceTreeItemData* pData = (CComponentInstanceTreeItemData*)m_pEffectTreeCtrl->GetItemData(id);
        CComponentBase* pComponentBase = pData->GetComponentBase();
        if (pComponentBase)
        {
            CComponentProxy* pProxy = (CComponentProxy*)pComponentBase;
            NBeyondEngine::CParticleSystem* pParticleSystem = (NBeyondEngine::CParticleSystem*)pProxy->GetHostComponent();
            pParticleSystem->BeginShoot();
        }
    }
}

void CEffectViewAgent::OnUpdateComponentInstance()
{
    UpdateCtrl();
}

void CEffectViewAgent::UpdateCtrl()
{
    UpdateEffectTree();
    UpdateTimeBar();
}

void CEffectViewAgent::SelectComponent(CComponentProxy* pComponentInstance)
{
    if (!pComponentInstance)
    {
        m_pEmitButton->Enable(false);
    }
}

void CEffectViewAgent::OnPropertyChanged(wxPropertyGridEvent& /*event*/)
{
    int nCurrent = m_pMainFrame->m_pTimeBar->GetSelectedRow();
    wxUIntPtr data = m_pMainFrame->m_pTimeBar->GetItemDataByRow(nCurrent);
    CComponentProxy* pComponentEmitter = (CComponentProxy*)wxUIntToPtr(data);
    BEATS_ASSERT(pComponentEmitter);
    NBeyondEngine::CParticleEmitter* pParticleEmitter = (NBeyondEngine::CParticleEmitter*)pComponentEmitter->GetHostComponent();
    int nStart = pParticleEmitter->GetDelayTime();
    int nEnd = pParticleEmitter->GetEmitterLiveTime();
    m_pMainFrame->m_pTimeBar->SetItemDataRange(nCurrent, nStart * 10, nEnd * 10);
}

void CEffectViewAgent::OnTreeCtrlRightClick(wxTreeEvent& event)
{
    switch (event.GetId())
    {
    case Ctrl_Tree_Effects:
        m_pMainFrame->PopupMenu(m_pEffectTreeMenu);
        break;
    default:
        break;
    }
}

void CEffectViewAgent::InitializeCtrls()
{
    wxSize client_size = m_pMainFrame->GetClientSize();
    long lStyle = wxAUI_NB_TOP | wxAUI_NB_TAB_FIXED_WIDTH;
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    wxSize panelSize = wxSize(client_size.GetWidth() * 0.1f, client_size.GetHeight() * 0.1f);
    m_pEffectsPanel = new wxPanel(m_pMainFrame, wxID_ANY, wxDefaultPosition, panelSize);
    m_pEffectsBook = new wxAuiNotebook(m_pEffectsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle | wxAUI_NB_SCROLL_BUTTONS);
    pSizer->Add(m_pEffectsBook, 1, wxGROW|wxALL, 0);
    m_pEffectsPanel->SetSizer(pSizer);

    wxImageList *pFileIconImages = new wxImageList(TREEICONSIZE, TREEICONSIZE, true);
    wxIcon fileIcons[eTCIT_Count];
    wxSize iconSize(TREEICONSIZE, TREEICONSIZE);
    fileIcons[eTCIT_File] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_LIST, iconSize);
    fileIcons[eTCIT_FileSelected] = wxArtProvider::GetIcon(wxART_FILE_OPEN, wxART_LIST, iconSize);
    fileIcons[eTCIT_Folder] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_LIST, iconSize);
    fileIcons[eTCIT_FolderSelected] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_LIST, iconSize);
    fileIcons[eTCIT_FolderOpened] = wxArtProvider::GetIcon(wxART_FOLDER_OPEN, wxART_LIST, iconSize);

    lStyle = wxTR_DEFAULT_STYLE | wxTR_FULL_ROW_HIGHLIGHT | wxTR_NO_LINES | wxNO_BORDER | wxTR_TWIST_BUTTONS | wxTR_EDIT_LABELS;
    for (size_t i = 0; i < eTCIT_Count; ++i)
    {
        pFileIconImages->Add(fileIcons[i]);
    }

    m_pEffectTreeCtrl = new wxTreeCtrl(m_pEffectsBook, Ctrl_Tree_Effects, wxPoint(0,0), wxDefaultSize, lStyle);
    m_pEffectTreeCtrl->AssignImageList(pFileIconImages);
    m_pEffectTreeCtrl->AddRoot(wxT("Effects"), eTCIT_Folder, -1, new CComponentInstanceTreeItemData(NULL));
    m_pEffectTreeCtrl->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(CEditorMainFrame::OnTreeCtrlFocused), NULL, m_pMainFrame);
    m_pEffectsBook->AddPage(m_pEffectTreeCtrl, wxT("Effects"));
    m_pMainFrame->m_Manager.AddPane(m_pEffectsPanel, wxAuiPaneInfo().CenterPane().Name(wxT("Effects")).Left().Position(0).Show(false));
}

void CEffectViewAgent::OnEditTreeItemLabel(wxTreeEvent& event)
{
    wxString oldName;
    wxString newName = event.GetLabel();
    if (event.GetId() == Ctrl_Tree_Effects)
    {
        oldName = m_pEffectTreeCtrl->GetItemText(event.GetItem());
        if (oldName != newName)
        {
            CComponentInstanceTreeItemData* pInstanceData = (CComponentInstanceTreeItemData*)m_pEffectTreeCtrl->GetItemData(event.GetItem());
            BEATS_ASSERT(pInstanceData);
            CComponentProxy* pProxy = (CComponentProxy*)pInstanceData->GetComponentBase();
            if (pProxy)
            {
                pProxy->SetUserDefineDisplayName(newName);
            }
        }
    }
}

void CEffectViewAgent::OnEditDataViewItem(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    wxString strName = event.GetString();
    wxUIntPtr data = m_pMainFrame->m_pTimeBar->GetItemData(item);
    CComponentProxy* pComponentEmitter = (CComponentProxy*)wxUIntToPtr(data);
    if (strName != m_pMainFrame->m_pTimeBar->GetItemLabel(item))
    {
        pComponentEmitter->SetUserDefineDisplayName(strName);
    }
}

CComponentProxy* CEffectViewAgent::CreateNewParticleSystem()
{
    CComponentProxyManager* pComponentProxyManager = CComponentProxyManager::GetInstance();
    CComponentProxy* pEmitterProxy = (CComponentProxy*)pComponentProxyManager->CreateComponent(m_nPatricleEmitterGuid, true);
    CComponentProxy* pSystemProxy = (CComponentProxy*)pComponentProxyManager->CreateComponent(m_nPatricleSystemGuid, true);
    CComponentProxy* p3DNodeProxy = (CComponentProxy*)pComponentProxyManager->CreateComponent(m_n3DNodeGuid, true);
    int nSceneId = CSceneManager::GetInstance()->GetCurrentScene()->GetId();
    int nSceneGuid = CSceneManager::GetInstance()->GetCurrentScene()->GetGuid();

    CComponentProxy* pSceneProxy = (CComponentProxy*)pComponentProxyManager->GetComponentInstance(nSceneId, nSceneGuid);
    BEATS_ASSERT(pSceneProxy);
    CDependencyDescription* pDependency = NULL;
    pDependency = GetDependency(pSceneProxy, _T("节点"));
    BEATS_ASSERT(pDependency);
    pDependency->AddDependency(p3DNodeProxy);
    pDependency = GetDependency(p3DNodeProxy, _T("可渲染对象"));
    BEATS_ASSERT(pDependency);
    pDependency->AddDependency(pSystemProxy);
    pDependency = GetDependency(pSystemProxy, _T("粒子发射器"));
    BEATS_ASSERT(pDependency);
    pDependency->AddDependency(pEmitterProxy);

    return pSystemProxy;
}

CComponentProxy* CEffectViewAgent::CreateNewParticleEmitter()
{
    wxTreeItemId id = m_pEffectTreeCtrl->GetSelection();
    BEATS_ASSERT(id.IsOk());
    CComponentInstanceTreeItemData* pData = (CComponentInstanceTreeItemData*)m_pEffectTreeCtrl->GetItemData(id);
    BEATS_ASSERT(pData);
    CComponentProxy* pSystemProxy = (CComponentProxy*)pData->GetComponentBase();
    BEATS_ASSERT(pSystemProxy);

    CComponentProxyManager* pComponentProxyManager = CComponentProxyManager::GetInstance();
    CComponentProxy* pEmitterProxy = (CComponentProxy*)pComponentProxyManager->CreateComponent(m_nPatricleEmitterGuid, true);

    CDependencyDescription* pDependency = GetDependency(pSystemProxy, _T("粒子发射器"));
    BEATS_ASSERT(pDependency);
    pDependency->AddDependency(pEmitterProxy);

    return pSystemProxy;
}

CDependencyDescription* CEffectViewAgent::GetDependency(CComponentProxy* pProxy, wxString dependencyName)
{
    CDependencyDescription* pDependency = NULL;
    const std::vector<CDependencyDescription*>* pDependencyList = pProxy->GetDependencies();
    if (pDependencyList != NULL)
    {
        int nDependencyCount = pDependencyList->size();
        for (int i = 0; i < nDependencyCount; i++)
        {
            CDependencyDescription* pCurDependency = pDependencyList->at(i);
            if (pCurDependency->GetDisplayName() == dependencyName)
            {
                pDependency = pCurDependency;
                break;
            }
        }
        BEATS_ASSERT(pDependency);
    }
    return pDependency;
}

void CEffectViewAgent::DeleteComponent(CComponentProxy* pProxy)
{
    if (pProxy != NULL)
    {
        m_pMainFrame->SelectComponent(NULL);
        CComponentInstance* pHostComponent = pProxy->GetHostComponent();
        if (pHostComponent != NULL)
        {
            pHostComponent->Uninitialize();
            BEATS_SAFE_DELETE(pHostComponent);
        }
    }
}

void CEffectViewAgent::DeleteSelectedParticleSystem()
{
    wxTreeItemId id = m_pEffectTreeCtrl->GetSelection();
    if (id.IsOk())
    {
        std::vector<CComponentProxy*> vecProxys;
        CComponentInstanceTreeItemData* pData = (CComponentInstanceTreeItemData*)m_pEffectTreeCtrl->GetItemData(id);
        if (pData)
        {
            CComponentProxy* pSystemProxy = (CComponentProxy*)pData->GetComponentBase();
            BEATS_ASSERT(pSystemProxy);
            vecProxys.push_back(pSystemProxy);

            //get 3DNode that depend on the SystemProxy
            const std::vector<CDependencyDescriptionLine*>* vecDependencyLines = pSystemProxy->GetBeConnectedDependencyLines();
            BEATS_ASSERT(1 == vecDependencyLines->size());
            CComponentProxy* p3DNodeProxy = vecDependencyLines->at(0)->GetOwnerDependency()->GetOwner();
            BEATS_ASSERT(p3DNodeProxy);
            BEATS_ASSERT(p3DNodeProxy->GetDisplayName() == _T("3D节点"));
            vecProxys.push_back(p3DNodeProxy);

            //get all Emitters of SystemProxy, and delete them
            CDependencyDescription* pDependency = GetDependency(pSystemProxy, _T("粒子发射器"));
            BEATS_ASSERT(pDependency);
            wxString dependencyName = pDependency->GetDisplayName();
            int nDependencyLineCount = pDependency->GetDependencyLineCount();
            for (int j = 0; j < nDependencyLineCount; j++)
            {
                CDependencyDescriptionLine* pLine = pDependency->GetDependencyLine(j);
                CComponentProxy* pEmitterProxy = pLine->GetConnectedComponent();
                BEATS_ASSERT(pEmitterProxy);
                vecProxys.push_back(pEmitterProxy);
            }

            //delete all proxy
            for (auto itr : vecProxys)
            {
                DeleteComponent(itr);
            }
        }
    }
}

void CEffectViewAgent::DeleteSelectedParticleEmitter()
{
    int nRow = m_pMainFrame->m_pTimeBar->GetSelectedRow();
    wxUIntPtr data = m_pMainFrame->m_pTimeBar->GetItemDataByRow(nRow);
    CComponentProxy* pComponentEmitter = (CComponentProxy*)wxUIntToPtr(data);
    BEATS_ASSERT(pComponentEmitter);
    DeleteComponent(pComponentEmitter);
}

