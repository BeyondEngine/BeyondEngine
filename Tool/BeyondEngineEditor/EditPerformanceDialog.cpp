#include "stdafx.h"
#include "EditPerformanceDialog.h"
#include "wx\sizer.h"
#include "Utility\PerformDetector\PerformResultTreeItemData.h"
#include "EditorMainFrame.h"
#include "EngineEditor.h"

static const char *FLOAT_FORMAT = "%.3f";

enum
{
    ID_Perform_tree,
    ID_Reset_Peak_Button,
    ID_Pause_Monitor_Button,
    ID_Again_Collect_Button,
};
BEGIN_EVENT_TABLE(CEditPerformanceDialog, CEditDialogBase)
    EVT_TREE_SEL_CHANGED(ID_Perform_tree, CEditPerformanceDialog::OnTreeItemChanged)
    EVT_BUTTON(ID_Reset_Peak_Button, CEditPerformanceDialog::OnClickClearPeakValueBtn)
    EVT_BUTTON(ID_Again_Collect_Button, CEditPerformanceDialog::OnClickPerformanceClearBtn)
    EVT_BUTTON(ID_Pause_Monitor_Button, CEditPerformanceDialog::OnClickPerformancePauseBtn)
    EVT_CLOSE(CEditPerformanceDialog::OnClose)
END_EVENT_TABLE()
CEditPerformanceDialog::CEditPerformanceDialog( wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name )
    :CEditDialogBase(parent, id, title, pos, size, style, name)
    , m_pPerformanceTreeCtrl(NULL)
    , m_pCurResultLabel(NULL)
    , m_pCurResultTxtLabel(NULL)
    , m_pAverageValueLabel(NULL)
    , m_pAverageValueTxtLabel(NULL)
    , m_pPeakValueLabel(NULL)
    , m_pPeakValueTxtLabel(NULL)
    , m_pTotalRunTimeLabel(NULL)
    , m_pDrawCallLabel(NULL)
    , m_pDrawCallTxtLabel(NULL)
    , m_pTotalRunTimeTxtLabel(NULL)
    , m_pFrameCountLabel(NULL)
    , m_pFrameCountTxtLabel(NULL)
    , m_pPerformLocationLabel(NULL)
    , m_pPerformLocationTxtLabel(NULL)
    , m_pClearPeakValueBtn(NULL)
    , m_pPausePerformanceBtn(NULL)
    , m_pClearPerformanceBtn(NULL)
    , m_pSubItemInfoGrid(NULL)
    , m_pPerformanceInfoLabel(NULL)
    , m_pFunctionAeraLabel(NULL)
    , m_pSubItemInfoLabel(NULL)
    , m_bIsPaused(FALSE)
    , m_bIsResult(false)
{
    wxBoxSizer* pPerformanceSizer = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(pPerformanceSizer);

    wxBoxSizer* pPerFormanceTreeSizer = new wxBoxSizer(wxVERTICAL);
    pPerformanceSizer->Add(pPerFormanceTreeSizer, 0, wxGROW|wxALL, 5);

    m_pPerformanceTreeCtrl = new wxTreeCtrl(this, ID_Perform_tree, wxDefaultPosition, wxSize( 260,-1 ), wxTR_DEFAULT_STYLE);
    pPerFormanceTreeSizer->Add(m_pPerformanceTreeCtrl, 1,  wxGROW|wxALL, 5);

    wxBoxSizer* pPerformanceRightSideSizer = new wxBoxSizer(wxVERTICAL);
    pPerformanceSizer->Add( pPerformanceRightSideSizer, 1, wxALL | wxEXPAND, 5 );
    wxBoxSizer* pFunctionAeraSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pPerformanceInfoLabel = new wxStaticBox(this, wxID_ANY, wxT("Statistical information"));
    wxStaticBoxSizer* pPerformanceInfoSizer = new wxStaticBoxSizer(m_pPerformanceInfoLabel, wxVERTICAL);
    m_pFunctionAeraLabel = new wxStaticBox( this, wxID_ANY, wxT("Functional areas"));
    wxStaticBoxSizer* pFunctionButtonAeraSizer = new wxStaticBoxSizer( m_pFunctionAeraLabel, wxVERTICAL );
    pPerformanceRightSideSizer->Add(pFunctionAeraSizer, 0, wxALL | wxEXPAND, 5);
    pFunctionAeraSizer->Add(pPerformanceInfoSizer, 4, wxALL | wxEXPAND, 5);
    pFunctionAeraSizer->Add(pFunctionButtonAeraSizer, 1, wxALL | wxEXPAND, 5);
    m_pSubItemInfoLabel = new wxStaticBox( this, wxID_ANY, wxT("Child node information"));
    wxStaticBoxSizer* pSubItemInfoSizer = new wxStaticBoxSizer( m_pSubItemInfoLabel, wxVERTICAL );
    pPerformanceRightSideSizer->Add(pSubItemInfoSizer, 1, wxALL | wxEXPAND, 5);

    wxBoxSizer* p1stRowInfoSizer = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* pCurResultSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pCurResultLabel = new wxStaticText(this, wxID_ANY, wxT("Elapsed time:"));
    m_pCurResultLabel->Wrap(-1);
    pCurResultSizer->Add(m_pCurResultLabel, 0, wxALL, 5);

    m_pCurResultTxtLabel = new wxStaticText( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pCurResultTxtLabel->Wrap( -1 );
    pCurResultSizer->Add( m_pCurResultTxtLabel, 0, wxALL, 5 );

    p1stRowInfoSizer->Add(pCurResultSizer, 1, wxEXPAND, 5);

    wxBoxSizer* pAverageSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pAverageValueLabel = new wxStaticText( this, wxID_ANY, wxT("Average time:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pAverageValueLabel->Wrap( -1 );
    pAverageSizer->Add( m_pAverageValueLabel, 0, wxALL, 5 );

    m_pAverageValueTxtLabel = new wxStaticText( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pAverageValueTxtLabel->Wrap( -1 );
    pAverageSizer->Add( m_pAverageValueTxtLabel, 0, wxALL, 5 );

    p1stRowInfoSizer->Add( pAverageSizer, 1, wxEXPAND, 5 );

    wxBoxSizer* pPeakValueSizer;
    pPeakValueSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pPeakValueLabel = new wxStaticText( this, wxID_ANY, wxT("Peek time:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pPeakValueLabel->Wrap( -1 );
    pPeakValueSizer->Add( m_pPeakValueLabel, 0, wxALL, 5 );

    m_pPeakValueTxtLabel = new wxStaticText( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pPeakValueTxtLabel->Wrap( -1 );
    pPeakValueSizer->Add( m_pPeakValueTxtLabel, 0, wxALL, 5 );

    pPeakValueSizer->Add( 0, 0, 1, wxEXPAND, 5 );

    p1stRowInfoSizer->Add( pPeakValueSizer, 1, wxFIXED_MINSIZE, 5 );

    pPerformanceInfoSizer->Add(p1stRowInfoSizer, 1, wxEXPAND, 5);

    wxBoxSizer* p2ndRowInfoSizer = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* pTotalRunTimeSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pTotalRunTimeLabel = new wxStaticText( this, wxID_ANY, wxT("Total run time:"));
    m_pTotalRunTimeLabel->Wrap( -1 );
    pTotalRunTimeSizer->Add( m_pTotalRunTimeLabel, 0, wxALL, 5 );

    m_pTotalRunTimeTxtLabel = new wxStaticText( this, wxID_ANY, wxT("0"));
    m_pTotalRunTimeTxtLabel->Wrap( -1 );
    pTotalRunTimeSizer->Add( m_pTotalRunTimeTxtLabel, 0, wxALL, 5 );

    p2ndRowInfoSizer->Add( pTotalRunTimeSizer, 1, wxEXPAND, 5 );

    wxBoxSizer* pDrawCallSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pDrawCallLabel = new wxStaticText(this, wxID_ANY, wxT("Draw call:"));
    m_pDrawCallLabel->Wrap( -1 );
    pDrawCallSizer->Add( m_pDrawCallLabel, 0, wxALL, 5 );

    m_pDrawCallTxtLabel = new wxStaticText(this, wxID_ANY, wxT("0"));
    m_pDrawCallTxtLabel->Wrap( -1 );
    pDrawCallSizer->Add( m_pDrawCallTxtLabel, 0, wxALL, 5 );

    p2ndRowInfoSizer->Add( pDrawCallSizer, 1, wxEXPAND, 5 );

    wxBoxSizer* pFrameCountSizer;
    pFrameCountSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pFrameCountLabel = new wxStaticText( this, wxID_ANY, wxT("Total run frames:"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
    m_pFrameCountLabel->Wrap( -1 );
    pFrameCountSizer->Add( m_pFrameCountLabel, 0, wxALL, 5 );

    m_pFrameCountTxtLabel = new wxStaticText( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pFrameCountTxtLabel->Wrap( -1 );
    pFrameCountSizer->Add( m_pFrameCountTxtLabel, 0, wxALL, 5 );

    p2ndRowInfoSizer->Add( pFrameCountSizer, 1, wxFIXED_MINSIZE, 5 );

    pPerformanceInfoSizer->Add( p2ndRowInfoSizer, 1, wxEXPAND, 5 );

    wxBoxSizer* p3rdRowInfoSizer;
    p3rdRowInfoSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pPerformLocationLabel = new wxStaticText( this, wxID_ANY, wxT("Location:"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
    m_pPerformLocationLabel->Wrap( -1 );
    p3rdRowInfoSizer->Add( m_pPerformLocationLabel, 0, wxALL, 5 );
    m_pPerformLocationTxtLabel = new wxStaticText( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_pPerformLocationTxtLabel->Wrap(-1);
    p3rdRowInfoSizer->Add(m_pPerformLocationTxtLabel, 0, wxALL, 5);

    pPerformanceInfoSizer->Add( p3rdRowInfoSizer, 1, wxEXPAND, 5 );

    m_pClearPeakValueBtn = new wxButton( this, ID_Reset_Peak_Button, wxT("Reset peak"), wxPoint( -1,-1 ), wxSize( 110,-1 ), 0 );
    pFunctionButtonAeraSizer->Add( m_pClearPeakValueBtn, 0, wxCENTER, 5 );
    m_pPausePerformanceBtn = new wxButton( this, ID_Pause_Monitor_Button, wxT("Pause monitor"), wxPoint( -1,-1 ), wxSize( 110,-1 ), 0 );
    pFunctionButtonAeraSizer->Add( m_pPausePerformanceBtn, 0, wxCENTER, 5 );
    m_pClearPerformanceBtn = new wxButton( this, ID_Again_Collect_Button, wxT("Recollect"), wxPoint( -1,-1 ), wxSize( 110,-1 ), 0 );
    pFunctionButtonAeraSizer->Add( m_pClearPerformanceBtn, 0, wxCENTER, 5 );

    m_pSubItemInfoGrid = new wxGrid(this, wxID_ANY);
    // Grid
    m_pSubItemInfoGrid->CreateGrid( 8, 5 );
    m_pSubItemInfoGrid->EnableEditing( false );
    m_pSubItemInfoGrid->EnableGridLines( true );
    m_pSubItemInfoGrid->EnableDragGridSize( false );
    // Columns
    m_pSubItemInfoGrid->EnableDragColMove( false );
    m_pSubItemInfoGrid->EnableDragColSize( true );
    m_pSubItemInfoGrid->SetColLabelValue( 0, wxT("Elapsed time") );
    m_pSubItemInfoGrid->SetColLabelValue( 1, wxT("Average time") );
    m_pSubItemInfoGrid->SetColLabelValue( 2, wxT("Peek time") );
    m_pSubItemInfoGrid->SetColLabelValue( 3, wxT("Total run time") );
    m_pSubItemInfoGrid->SetColLabelValue( 4, wxT("Total run frames") );
    m_pSubItemInfoGrid->SetColLabelAlignment( wxALIGN_LEFT, wxALIGN_CENTRE );
    // Rows
    m_pSubItemInfoGrid->EnableDragRowSize( false );
    m_pSubItemInfoGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

    m_pSubItemInfoGrid->AutoSize();
    m_pSubItemInfoGrid->SetDefaultCellBackgroundColour(wxColour(240, 240, 240));
    pSubItemInfoSizer->Add(m_pSubItemInfoGrid, 1, wxEXPAND, 5);

    m_performTreeItemMap[0] = m_pPerformanceTreeCtrl->AddRoot("Performance");
}

CEditPerformanceDialog::~CEditPerformanceDialog()
{

}

void CEditPerformanceDialog::LanguageSwitch()
{
    m_pPerformanceInfoLabel->SetLabel(L10N_T(eL_StatisticalInfo));
    m_pFunctionAeraLabel->SetLabel(L10N_T(eL_FunctionAreas));
    m_pSubItemInfoLabel->SetLabel(L10N_T(eL_SubItemInfo));
    m_pCurResultLabel->SetLabel(L10N_T(eL_ElapsedTime) + wxT(":"));
    m_pAverageValueLabel->SetLabel(L10N_T(eL_AvgTime) + wxT(":"));
    m_pPeakValueLabel->SetLabel(L10N_T(eL_PeakTime) + wxT(":"));
    m_pTotalRunTimeLabel->SetLabel(L10N_T(eL_TotalRunTime) + wxT(":"));
    m_pFrameCountLabel->SetLabel(L10N_T(eL_TotalRunFrames) + wxT(":"));
    m_pPerformLocationLabel->SetLabel(L10N_T(eL_Location) + wxT(":"));
    m_pClearPeakValueBtn->SetLabel(L10N_T(eL_ResetPeak));
    m_pPausePerformanceBtn->SetLabel(L10N_T(m_bIsPaused ? eL_ContinueMonitor : eL_PauseMonitor));
    m_pClearPerformanceBtn->SetLabel(L10N_T(eL_ReCollect));

    m_pSubItemInfoGrid->SetColLabelValue( 0, L10N_T(eL_ElapsedTime) );
    m_pSubItemInfoGrid->SetColLabelValue( 1, L10N_T(eL_AvgTime) );
    m_pSubItemInfoGrid->SetColLabelValue( 2, L10N_T(eL_PeakTime) );
    m_pSubItemInfoGrid->SetColLabelValue( 3, L10N_T(eL_TotalRunTime) );
    m_pSubItemInfoGrid->SetColLabelValue( 4, L10N_T(eL_TotalRunFrames) );
    m_pSubItemInfoGrid->Refresh();

    m_pPerformLocationTxtLabel->SetLabel(m_bIsResult ? wxString::Format("%s %d ", m_info.FileName, m_info.LineNumber) +
        L10N_T(eL_Line): L10N_T(eL_UnCodeInfo));

}

void CEditPerformanceDialog::UpdatePerformData()
{
    m_updateValueMap.clear();
    std::vector<SPerformanceResult*> outResult;
    CPerformDetector::GetInstance()->GetResultThisFrame(outResult);
    for (size_t i = 0; i < outResult.size(); ++i)
    {
        SPerformanceResult* pCurResult = outResult[i];
        std::map<size_t, wxTreeItemId>::iterator iter = m_performTreeItemMap.find(pCurResult->id);
        if (iter == m_performTreeItemMap.end())//that's a new node!
        {
            AddPerformNode(CPerformDetector::GetInstance()->GetRecord(pCurResult->id));
        }
        m_updateValueMap[pCurResult->id] = pCurResult->result;
    }
    SPerformanceRecord* pRecord = GetRecord(m_pPerformanceTreeCtrl->GetSelection());
    if (pRecord != NULL && IsShown() && !m_bIsPaused)
    {
        RefreshPerformBoard(pRecord);
        RefreshItemGridCell(pRecord);
    }
}

void CEditPerformanceDialog::AddPerformNode( const SPerformanceRecord* pRecordToAdd )
{
    BEATS_ASSERT(m_performTreeItemMap.find(pRecordToAdd->id) == m_performTreeItemMap.end(), _T("Can't add perform record twice for id : %d"), pRecordToAdd->id);
    SPerformanceRecord* validParentInTree = pRecordToAdd->pParent;
    size_t validParentId = validParentInTree == NULL ? 0 : validParentInTree->id;
    std::map<size_t, wxTreeItemId>::iterator iter = m_performTreeItemMap.find(validParentId);
    std::vector<SPerformanceRecord*> parentsNeedToAddAlso;
    while (iter == m_performTreeItemMap.end() && validParentInTree != NULL)
    {
        parentsNeedToAddAlso.push_back(validParentInTree);
        validParentInTree = validParentInTree->pParent;
        validParentId = validParentInTree == NULL ? 0 : validParentInTree->id;
        iter = m_performTreeItemMap.find(validParentId);
    }
    wxTreeItemId parentId = validParentInTree == NULL ? m_performTreeItemMap[0] : iter->second;
    while (parentsNeedToAddAlso.size() != 0)
    {
        parentId = m_pPerformanceTreeCtrl->AppendItem(parentId
            ,parentsNeedToAddAlso.back()->typeStr, 
            -1, -1, new CPerformResultTreeItemData(parentsNeedToAddAlso.back()->id));
        m_performTreeItemMap[parentsNeedToAddAlso.back()->id] = parentId;
        parentsNeedToAddAlso.pop_back();
    }
    m_performTreeItemMap[pRecordToAdd->id] = 
        m_pPerformanceTreeCtrl->AppendItem(parentId,
        pRecordToAdd->typeStr, 
        -1,
        -1,
        new CPerformResultTreeItemData(pRecordToAdd->id));
    //if this is the first node we add, we will select it.
    if (m_pPerformanceTreeCtrl->GetChildrenCount(m_pPerformanceTreeCtrl->GetRootItem()) == 1)
    {
        m_pPerformanceTreeCtrl->SelectItem(m_performTreeItemMap[pRecordToAdd->id]);
    }
}

void CEditPerformanceDialog::RefreshPerformBoard( const SPerformanceRecord* pRecord )
{
    if (pRecord != NULL)
    {
        RefreshInfo(pRecord);
    }
#ifdef _DEBUG
    m_pDrawCallTxtLabel->SetLabelText(wxString::Format("%d", CEngineCenter::GetInstance()->GetDrawCallLastFrame()));
#endif
}

void CEditPerformanceDialog::RefreshInfo(const SPerformanceRecord* pRecord )
{
    std::map<size_t, float>::iterator iter = m_updateValueMap.find(pRecord->id);
    float curValue = iter == m_updateValueMap.end() ? 0.f : iter->second;
    m_pCurResultTxtLabel->SetLabelText(wxString::Format(FLOAT_FORMAT, curValue));
    m_pTotalRunTimeTxtLabel->SetLabelText(wxString::Format(FLOAT_FORMAT, pRecord->totalValue));
    m_pFrameCountTxtLabel->SetLabelText(wxString::Format("%d", pRecord->updateCount));
    m_pAverageValueTxtLabel->SetLabelText(wxString::Format(FLOAT_FORMAT, pRecord->updateCount != 0 ? pRecord->totalValue / pRecord->updateCount : 0));
    m_pPeakValueTxtLabel->SetLabelText(wxString::Format(FLOAT_FORMAT, pRecord->maxValue));
}

void CEditPerformanceDialog::RefreshItemGridCell( const SPerformanceRecord* pRecord)
{
    SetGridRowsCount(m_pSubItemInfoGrid, pRecord->children.size());
    for (size_t i = 0; i < pRecord->children.size(); ++i)
    {
        SPerformanceRecord* curRecord = pRecord->children[i];
        std::map<size_t, float>::iterator iter = m_updateValueMap.find(curRecord->id);
        m_pSubItemInfoGrid->SetCellValue(wxString::Format(FLOAT_FORMAT, iter == m_updateValueMap.end() ? 0 : iter->second), i, 0);
        m_pSubItemInfoGrid->SetCellValue(wxString::Format(FLOAT_FORMAT, curRecord->updateCount != 0 ? curRecord->totalValue / curRecord->updateCount : 0), i, 1);
        m_pSubItemInfoGrid->SetCellValue(wxString::Format(FLOAT_FORMAT, curRecord->maxValue), i, 2);
        m_pSubItemInfoGrid->SetCellValue(wxString::Format(FLOAT_FORMAT, curRecord->totalValue), i, 3);
        m_pSubItemInfoGrid->SetCellValue(wxString::Format("%d", curRecord->updateCount), i, 4);
    }
}

SPerformanceRecord* CEditPerformanceDialog::GetRecord( const wxTreeItemId& treeItemId )
{
    SPerformanceRecord* result = NULL;
    if (treeItemId.IsOk())
    {
        CPerformResultTreeItemData* pItemData = static_cast<CPerformResultTreeItemData*>(m_pPerformanceTreeCtrl->GetItemData(treeItemId));
        if (pItemData != NULL)
        {
            size_t recordId = pItemData->GetRecordId();
            result = CPerformDetector::GetInstance()->GetRecord(recordId);
        }
    }
    return result;
}

void CEditPerformanceDialog::OnTreeItemChanged( wxTreeEvent& event )
{
    m_pPerformanceTreeCtrl->Expand(event.GetItem());
    SPerformanceRecord* pRecord = GetRecord(event.GetItem());
    m_pSubItemInfoGrid->Scroll(0,0);
    if (pRecord)
    {
        RefreshItemGridRowLab(pRecord);
        RefreshInfo(pRecord);
        m_pSubItemInfoGrid->SetRowLabelSize( wxGRID_AUTOSIZE);
        IMAGEHLP_LINEW64 info;
        info.SizeOfStruct = sizeof(IMAGEHLP_LINEW64);
        DWORD displacement;
        m_bIsResult = SymGetLineFromAddrW64(GetCurrentProcess(), pRecord->id, &displacement, &info) != FALSE;
        m_info = info;
        m_pPerformLocationTxtLabel->SetLabel(m_bIsResult ?
            wxString::Format("%s %d ", info.FileName, info.LineNumber) + L10N_T(eL_Line) :
            L10N_T(eL_UnCodeInfo));
    }
    else
    {
        ClearGrid();
    }
}

void CEditPerformanceDialog::RefreshItemGridRowLab(const SPerformanceRecord* pRecord)
{
    size_t childCount = pRecord->children.size();
    if (childCount > (size_t)m_pSubItemInfoGrid->GetRows() )
    {
        m_pSubItemInfoGrid->AppendRows(childCount - m_pSubItemInfoGrid->GetRows());
    }
    size_t i = 0;
    for (; i < childCount; ++i)
    {
        SPerformanceRecord* curRecord = pRecord->children[i];
        m_pSubItemInfoGrid->SetRowLabelValue(i, curRecord->typeStr);
    }
    ClearGridRow(i, m_pSubItemInfoGrid->GetRows() - i);
}

void CEditPerformanceDialog::ClearGrid()
{
    m_pSubItemInfoGrid->ClearGrid();
    size_t rowCount = m_pSubItemInfoGrid->GetRows();
    for (size_t i = 0; i < rowCount; ++i)
    {
        m_pSubItemInfoGrid->SetRowLabelValue(i, "");
    }
}

void CEditPerformanceDialog::ClearGridRow( size_t startRowIndex, size_t count )
{
    size_t colNum = m_pSubItemInfoGrid->GetCols();
    for (size_t i = 0; i < count; ++i)
    {
        m_pSubItemInfoGrid->SetRowLabelValue(startRowIndex + i, "");
        for (size_t j = 0; j < colNum; ++j)
        {
            m_pSubItemInfoGrid->SetCellValue("", startRowIndex + i, j);
        }
    }
}

void CEditPerformanceDialog::OnClickClearPeakValueBtn( wxCommandEvent& /*event*/ )
{
    wxTreeItemId selectItem = m_pPerformanceTreeCtrl->GetSelection();
    if (selectItem.IsOk())
    {
        CPerformResultTreeItemData* pItemData = static_cast<CPerformResultTreeItemData*>(m_pPerformanceTreeCtrl->GetItemData(selectItem));
        SPerformanceRecord* pRecord = CPerformDetector::GetInstance()->GetRecord(pItemData->GetRecordId());
        ClearPeakValue(pRecord);
    }
}

void CEditPerformanceDialog::ClearPeakValue( SPerformanceRecord* pRecord )
{
    if (pRecord != NULL)
    {
        pRecord->maxValue = 0;
        for (size_t i = 0; i < pRecord->children.size(); ++i)
        {
            ClearPeakValue(pRecord->children[i]);
        }
    }
}

void CEditPerformanceDialog::OnClickPerformanceClearBtn(wxCommandEvent& /*event*/)
{
    CPerformDetector::GetInstance()->ClearAllResult();
}

void CEditPerformanceDialog::OnClickPerformancePauseBtn( wxCommandEvent& /*event*/ )
{
    bool bPauseOrResume = CPerformDetector::GetInstance()->PauseSwitcher();
    m_bIsPaused = bPauseOrResume;
    m_pPausePerformanceBtn->SetLabel(L10N_T(bPauseOrResume ? eL_ContinueMonitor : eL_PauseMonitor));
}

void CEditPerformanceDialog::OnClose( wxCloseEvent& /*event*/ )
{
    CEditorMainFrame* pWFram = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    pWFram->GetAuiToolBarPerformPtr()->ToggleTool(ID_TB_PerformBtn, false);
    pWFram->GetAuiToolBarPerformPtr()->Refresh(false);
    this->Show(false);
}

void CEditPerformanceDialog::SetGridRowsCount( wxGrid* pGrid, size_t count )
{
    int delta = pGrid->GetRows() - count;
    if (delta > 0)
    {
        pGrid->DeleteRows(0, delta);
    }
    else if (delta < 0)
    {
        pGrid->AppendRows(-delta);
    }
}
