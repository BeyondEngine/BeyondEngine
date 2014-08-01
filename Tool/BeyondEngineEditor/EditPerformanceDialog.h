#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_EditPerformanceDialog_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_EditPerformanceDialog_H__INCLUDE

#include "EditDialogBase.h"
#include <wx/treectrl.h>
#include <wx/grid.h>
#include "Utility/PerformDetector/PerformDetector.h"
#include "wx/treebase.h"
#include "dbghelp.h"

class CEditPerformanceDialog:public CEditDialogBase
{
    typedef CEditDialogBase super;
public:
    CEditPerformanceDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style =  wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~CEditPerformanceDialog();
    virtual void LanguageSwitch() override;
    void UpdatePerformData();
    void AddPerformNode(const SPerformanceRecord* pRecordToAdd);
    void RefreshPerformBoard( const SPerformanceRecord* pRecord );
    void RefreshInfo(const SPerformanceRecord* pRecord );
    void RefreshItemGridCell( const SPerformanceRecord* pRecord);
    SPerformanceRecord* GetRecord( const wxTreeItemId& treeItemId );
    void RefreshItemGridRowLab(const SPerformanceRecord* pRecord);
    void ClearGridRow( size_t startRowIndex, size_t count );
    void ClearGrid();
    void SetGridRowsCount(wxGrid* pGrid, size_t count);
    void ClearPeakValue( SPerformanceRecord* pRecord );
    void OnClickPerformanceClearBtn(wxCommandEvent& event);
    void OnClickPerformancePauseBtn( wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnTreeItemChanged(wxTreeEvent& event);
    void OnClickClearPeakValueBtn( wxCommandEvent& event);
private:
    wxTreeCtrl*     m_pPerformanceTreeCtrl;
    wxStaticText*   m_pCurResultLabel;
    wxStaticText*   m_pCurResultTxtLabel;
    wxStaticText*   m_pAverageValueLabel;
    wxStaticText*   m_pAverageValueTxtLabel;
    wxStaticText*   m_pPeakValueLabel;
    wxStaticText*   m_pPeakValueTxtLabel;
    wxStaticText*   m_pDrawCallLabel;
    wxStaticText*   m_pDrawCallTxtLabel;
    wxStaticText*   m_pTotalRunTimeLabel;
    wxStaticText*   m_pTotalRunTimeTxtLabel;
    wxStaticText*   m_pFrameCountLabel;
    wxStaticText*   m_pFrameCountTxtLabel;
    wxStaticText*   m_pPerformLocationLabel;
    wxStaticText*   m_pPerformLocationTxtLabel;
    wxButton*       m_pClearPeakValueBtn;
    wxButton*       m_pPausePerformanceBtn;
    wxButton*       m_pClearPerformanceBtn;
    wxGrid*         m_pSubItemInfoGrid;
    wxStaticBox*    m_pPerformanceInfoLabel;
    wxStaticBox*    m_pFunctionAeraLabel;
    wxStaticBox*    m_pSubItemInfoLabel;
    bool            m_bIsPaused;
    bool            m_bIsResult;
    IMAGEHLP_LINEW64 m_info;
    std::map<size_t, wxTreeItemId> m_performTreeItemMap;
    std::map<size_t, float>        m_updateValueMap;
DECLARE_EVENT_TABLE();
};

#endif