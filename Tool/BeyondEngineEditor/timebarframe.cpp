#include "stdafx.h"
#include "timebarframe.h"
#include "wx/frame.h"
#include "wx/dcbuffer.h"
#include "wx/sizer.h"
#include "Render/Animation3D.h"
#include "DataViewCtrl.h"
#include "DataViewListModel.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "EditAnimationDialog.h"
#include "Render/AnimationController.h"
#include "AnimationGLWindow.h"
#include "NodeAnimation/NodeAnimationElement.h"
#include "NodeAnimation/NodeAnimationData.h"
#include "NodeAnimation/NodeAnimation.h"
#include "wx/generic/numdlgg.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"

#define BUTTONSIZE 24
#define SASHPOSITION 130
#define HEIGHTADJUST 7

BEGIN_EVENT_TABLE(CTimeBarFrame, wxSplitterWindow)
    EVT_SIZE(CTimeBarFrame::OnSize)
    EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, CTimeBarFrame::OnPositionChanged)
    EVT_SPLITTER_SASH_POS_CHANGING(wxID_ANY, CTimeBarFrame::OnPositionChanging)
    EVT_DATAVIEW_SELECTION_CHANGED(wxID_ANY, CTimeBarFrame::OnSelect)
END_EVENT_TABLE()

CTimeBarFrame::CTimeBarFrame(wxWindow *parent)
: wxSplitterWindow(parent,
                   wxID_ANY,
                   wxDefaultPosition, 
                   wxDefaultSize,
                   wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN)
, m_iCellWidth(CELLWIDTH)
, m_pAnimationData(NULL)
{
    m_pAnimation = new CNodeAnimation();
    m_pAnimation->SetPlayType(eNAPT_ONCE);
    m_pAnimation->SetFPS(60);
    SetMinimumPaneSize(150);
    SetSashGravity(0.1);
    m_iCursorPositionX = 1;
    m_pFrame = parent;
    wxBoxSizer* rightpanelsizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* righttopsizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* leftpanelsizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* leftdownsizer = new wxBoxSizer(wxHORIZONTAL);
    wxPanel* rightpanelbase = new wxPanel(this);
    wxPanel* leftpanelbase = new wxPanel(this);
    wxPanel* pSpacePanel = new wxPanel(rightpanelbase, wxID_ANY, wxDefaultPosition, wxSize(DEFAULTSCROLLBARWIDTH, SCALEBARHIGHT));
    pSpacePanel->SetMinSize(wxSize(DEFAULTSCROLLBARWIDTH, SCALEBARHIGHT));
    pSpacePanel->SetMaxSize(wxSize(DEFAULTSCROLLBARWIDTH, SCALEBARHIGHT));
    m_pButtonAdd = new wxButton(leftpanelbase, wxID_ANY, "+");
    m_pButtonDelete = new wxButton(leftpanelbase, wxID_ANY, "-");
    m_pFPSSettingBtn = new wxButton(leftpanelbase, wxID_ANY, wxString::Format(_T("%d"), m_pAnimation->GetFPS()));
    m_pPlayBtn = new wxButton(leftpanelbase, wxID_ANY, "|>");
    m_pPlayTypeBtn = new wxButton(leftpanelbase, wxID_ANY, wxString::Format(_T("%d"), m_pAnimation->GetPlayType()));

    m_pElementList = new CDataViewCtrl(leftpanelbase, wxID_ANY );
    m_pElementList->AssociateListModel();
    m_pScalebar = new CTimeBarScale(rightpanelbase);
    m_pFrameContainer = new CTimeBarItemContainer(rightpanelbase);
    m_pScalebar->SetTimeBarFrameWindow(this);
    m_pFrameContainer->SetTimeBarFrameWindow(this);
    wxSize size(BUTTONSIZE, BUTTONSIZE);
    m_pButtonAdd->SetMinSize(size);
    m_pButtonDelete->SetMinSize(size);
    m_pFPSSettingBtn->SetMinSize(size);
    m_pPlayBtn->SetMinSize(size);
    m_pPlayTypeBtn->SetMinSize(size);

    SplitVertically(leftpanelbase, rightpanelbase, SASHPOSITION);
    rightpanelbase->SetSizer(rightpanelsizer);
    leftpanelbase->SetSizer(leftpanelsizer);
    rightpanelsizer->Add(righttopsizer, 0, wxGROW|wxALL, 0 );
    righttopsizer->Add(m_pScalebar, 1, wxGROW|wxALL, 0 );
    righttopsizer->Add(pSpacePanel, 0, wxGROW|wxALL, 0 );
    rightpanelsizer->Add(m_pFrameContainer, 1, wxGROW|wxALL, 0 );
    rightpanelsizer->AddSpacer(HEIGHTADJUST);
    leftpanelsizer->Add(m_pElementList, 1, wxGROW|wxALL, 0 );
    leftpanelsizer->Add(leftdownsizer, 0, wxGROW|wxALL, 0);
    leftdownsizer->Add(m_pButtonAdd, 0, wxGROW|wxALL, 0);
    leftdownsizer->Add(m_pButtonDelete, 0, wxGROW|wxALL, 0);
    leftdownsizer->Add(m_pFPSSettingBtn, 0, wxGROW|wxALL, 0);
    leftdownsizer->Add(m_pPlayBtn, 0, wxGROW|wxALL, 0);
    leftdownsizer->Add(m_pPlayTypeBtn, 0, wxGROW|wxALL, 0);

    leftdownsizer->AddStretchSpacer(1);

    m_pFrameContainer->SyncWith(m_pElementList,m_pScalebar);
    m_pElementList->SyncWith(m_pFrameContainer);

    SetFocus();
    m_pFPSSettingBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CTimeBarFrame::OnFpsSettingBtnClicked), NULL, this);
    m_pPlayBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CTimeBarFrame::OnPlayBtnClicked), NULL, this);
    m_pPlayTypeBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CTimeBarFrame::OnPlayTypeBtnClicked), NULL, this);

}

CTimeBarFrame::~CTimeBarFrame()
{
    m_pFPSSettingBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CTimeBarFrame::OnFpsSettingBtnClicked), NULL, this);
    m_pPlayBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CTimeBarFrame::OnPlayBtnClicked), NULL, this);
    m_pPlayTypeBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CTimeBarFrame::OnPlayTypeBtnClicked), NULL, this);
    BEATS_SAFE_DELETE(m_pAnimation);
}

void CTimeBarFrame::OnPositionChanged(wxSplitterEvent& event)
{
    ResetLeft();
    event.Skip();
}

void CTimeBarFrame::OnPositionChanging(wxSplitterEvent& event)
{
    ResetLeft();
    event.Skip();
}

void CTimeBarFrame::OnSize(wxSizeEvent & event)
{
    __super::OnSize(event);
    ResetLeft();
    GetWindow1()->Refresh(false);
}

void CTimeBarFrame::ResetLeft()
{
    m_pElementList->RefreshSize();
}

CTimeBarItem* CTimeBarFrame::AddItem(wxString name, wxUIntPtr userData, int nStartPos, int nLength, bool bVisible , bool bEditable)
{
    CTimeBarItem* pRet = NULL;
    if (IsNameValid(name))
    {
        pRet = new CTimeBarItem(m_pFrameContainer);
        pRet->SetCellsDataRange(nStartPos, nStartPos + nLength);
        m_pElementList->AddItem(name, bVisible, bEditable, userData);
        m_pFrameContainer->AddItem(pRet);
    }
    else
    {
        wxMessageBox(name + _T(" has already existed"));
    }
    return pRet;
}

void CTimeBarFrame::DeleteItem(int index)
{
    if (index != -1)
    {
        ((CDataViewListModel*)m_pElementList->GetModel())->DeleteItem(index);
        m_pFrameContainer->DeleteItem(index);
    }
}

void CTimeBarFrame::ClickOnScaleBar()
{
    if (m_bclickonscalebar)
    {
        m_pFrameContainer->GetCurrentSelect().m_iSelectColumnBegin = -1;
        m_pFrameContainer->GetCurrentSelect().m_iSelectColumnEnd = -1;
        m_pFrameContainer->GetCurrentSelect().m_iSelectRowBegin = -1;
        m_pFrameContainer->GetCurrentSelect().m_iSelectRowEnd = -1;
    }

    m_pFrameContainer->SelectItems();
    m_pScalebar->Refresh(false);
}

void CTimeBarFrame::SetCursorPositionX(int pos)
{
    m_iCursorPositionX = pos < 0 ? 0 : pos;
    m_pFrameContainer->SelectItems();

    m_pScalebar->Refresh(false);
}

void CTimeBarFrame::SetSelectedRow(int nRowIndex)
{
    if (INVALID_DATA != nRowIndex)
    {
        m_pElementList->SetFocus();
        m_pElementList->SelectRow(nRowIndex);
    }
}

int CTimeBarFrame::GetSelectedRow() const
{
    return m_pElementList->GetSelectedRow();
}

CTimeBarScale* CTimeBarFrame:: GetScalebar()
{
    return m_pScalebar;
}

void CTimeBarFrame::SetClickOnScalebar(bool bIsClicked)
{
    m_bclickonscalebar = bIsClicked;
}

bool CTimeBarFrame::IsClickOnScalebar() const
{
    return m_bclickonscalebar;
}

CTimeBarItemContainer* CTimeBarFrame::GetItemContainer()
{
    return m_pFrameContainer;
}

size_t CTimeBarFrame::GetFrameCount() const
{
    size_t uRet = 0;
    if (m_pAnimationData != NULL)
    {
        uRet = m_pAnimationData->GetFrameCount();
    }
    return uRet;
}

void CTimeBarFrame::SetAnimationData(CNodeAnimationData* pData)
{
    if (m_pAnimation->IsPlaying())
    {
        m_pAnimation->Stop();
    }
    m_pAnimationData = pData;
    m_pAnimation->SetData(m_pAnimationData);
    GetScalebar()->Refresh();
    GetItemContainer()->ResetScrollBar();
}

CNodeAnimation* CTimeBarFrame::GetAnimation() const
{
    return m_pAnimation;
}

CNodeAnimationData* CTimeBarFrame::GetAnimationData() const
{
    return m_pAnimationData;
}

void CTimeBarFrame::SetFrameWindow( wxWindow* pWindow )
{
    m_pFrame = pWindow;
}

wxWindow* CTimeBarFrame::GetFrameWindow()
{
    return m_pFrame;
}

void CTimeBarFrame::SetCurrentCursor( int pos )
{
    wxPoint point = m_pFrameContainer->GetViewStart();
    if ((pos + 0.5F) * m_iCellWidth != m_iCursorPositionX)
    {
        m_iCursorPositionX = (pos + 0.5F) * m_iCellWidth;
        int iRange = m_pFrameContainer->GetSize().x / m_pFrameContainer->GetCellWidth();

        if (pos >= iRange + point.x)
        {
            point.x += pos - point.x - 10;
            m_pFrameContainer->Scroll(point);
        }
        else if (pos < point.x)
        {
            point.x = pos;
            m_pFrameContainer->Scroll(point);
        }
        m_pFrameContainer->RefreshItems();
        m_pScalebar->Refresh(false);
    }
}

int CTimeBarFrame::GetCursorPositionX()
{
    return m_iCursorPositionX;
}

int CTimeBarFrame::GetItemCount()
{
    return ((CDataViewListModel*)m_pElementList->GetModel())->GetCount();
}

void CTimeBarFrame::OnSelect(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    int row = m_pElementList->ItemToRow(item);
    SetSelectedRow(row);
    event.Skip(true);
}

void CTimeBarFrame::OnFpsSettingBtnClicked(wxCommandEvent& /*event*/)
{
    long uFPS = wxGetNumberFromUser(_T("请输入FPS值"), _T("FPS"), _T("FPS Input"), m_pAnimation->GetFPS(), 0, 60);
    if (uFPS > 0 && uFPS != (long)m_pAnimation->GetFPS())
    {
        m_pAnimation->SetFPS(uFPS);
        m_pFPSSettingBtn->SetLabel(wxString::Format(_T("%d"), uFPS));
    }
}

void CTimeBarFrame::OnPlayBtnClicked(wxCommandEvent& /*event*/)
{
    if (m_pAnimationData != NULL)
    {
        CComponentProxy* pProxy = ((CEngineEditor*)CEngineEditor::GetInstance())->GetMainFrame()->GetSelectedComponent();
        if (pProxy != NULL && pProxy->GetHostComponent() != NULL)
        {
            CNode* pNode = dynamic_cast<CNode*>(pProxy->GetHostComponent());
            if (pNode != NULL)
            {
                BEATS_ASSERT(m_pAnimation->GetAnimationData() == m_pAnimationData);
                m_pAnimation->SetOwner(pNode);
            }
        }
        if (m_pAnimation->GetOwner() != NULL)
        {
            if (m_pAnimation->IsPlaying())
            {
                m_pAnimation->Stop();
                m_pPlayBtn->SetLabel(_T("|>"));
            }
            else
            {
                m_pAnimation->Play();
                m_pPlayBtn->SetLabel(_T("||"));
            }
        }
    }
}

void CTimeBarFrame::OnPlayTypeBtnClicked(wxCommandEvent& /*event*/)
{
    long nPlayType = wxGetNumberFromUser(_T("请输入播放类型的序号:\n0. 单次播放\n1. 循环播放\n2. 倒序播放\n3. 倒序循环播放\n4. 来回播放"), _T("PlayType"), _T("PlayType Input"), m_pAnimation->GetPlayType(), -1, eNAPT_Count -1);
    if (nPlayType != -1)
    {
        m_pAnimation->SetPlayType((ENodeAnimationPlayType)nPlayType);
        m_pPlayTypeBtn->SetLabel(wxString::Format(_T("%d"), nPlayType));
    }
}

int CTimeBarFrame::GetCellWidth()
{
    return m_iCellWidth;
}

void CTimeBarFrame::Clear()
{
    SetSelectedRow(INVALID_DATA);
    int nCount = m_pFrameContainer->GetItemCount();
    for (int i = 0; i < nCount; i++)
    {
        m_pFrameContainer->DeleteItem(0);
    }
    SetAnimationData(NULL);
    m_pElementList->Clear();
}

bool CTimeBarFrame::IsNameValid(wxString name)
{
    bool bRet = true;
    size_t uItemCount = m_pElementList->GetItemCount();
    for (size_t i = 0; i < uItemCount; ++i)
    {
        void* pItemData = m_pElementList->GetItemDataByRow(i);
        // HACK: Currently we assume it is CNodeAnimationElement.
        CNodeAnimationElement* pElement = (CNodeAnimationElement*)(pItemData);
        if (strNodeAnimationElementType[pElement->GetType()] == name)
        {
            bRet = false;
            break;
        }
    }
    return bRet;
}

wxUIntPtr CTimeBarFrame::GetItemData(wxDataViewItem& item)
{
    wxUIntPtr ret = NULL;
    if (item.IsOk())
    {
        ret = m_pElementList->GetItemData(item);
    }
    return ret;
}

int CTimeBarFrame::GetCursorIndex(int pos)
{
    return (pos - m_iCellWidth * 0.5F) / m_iCellWidth;
}

CDataViewCtrl* CTimeBarFrame::GetElementList()
{
    return m_pElementList;
}

wxString CTimeBarFrame::GetItemLabel(wxDataViewItem& item)
{
    int nRow = m_pElementList->GetRow(item);
    return m_pElementList->GetTextValue(nRow, 0);
}

void CTimeBarFrame::SetItemDataRange(int nDataIndex, int nStart, int nEnd)
{
    m_pFrameContainer->GetItem(nDataIndex)->SetCellsDataRange(nStart, nEnd);
}

wxUIntPtr CTimeBarFrame::GetItemDataByRow(int nRow)
{
    wxDataViewItem item = m_pElementList->RowToItem(nRow);
    return GetItemData(item);
}

void CTimeBarFrame::SetCtrlsId(int nIdButtonAdd, int nIdButtonDelete)
{
    m_pButtonAdd->SetId(nIdButtonAdd);
    m_pButtonDelete->SetId(nIdButtonDelete);
}

