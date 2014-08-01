#ifndef BEYONDENGINEEDITOR_EditMemoryDialog_H__INCLUDE
#define BEYONDENGINEEDITOR_EditMemoryDialog_H__INCLUDE
#include "EditDialogBase.h"
#include "wx\generic\grid.h"
#include <wx/spinbutt.h>
#include <dbghelp.h>

enum EMemoryViewType
{
    eMVT_Location,
    eMVT_Size,
    eMVT_AllocTime,
    eMVT_Addr,
};

class EditMemoryDialog:public CEditDialogBase
{
    typedef CEditDialogBase super;
public:
    EditMemoryDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style =  wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~EditMemoryDialog();
    virtual void LanguageSwitch();
    void ClearAllColumnLabel();
    void OnViewMemoryByAddr();
    void OnViewMemoryByLocation();
    void OnViewMemoryBySize();
    void OnViewMemoryByAllocTime();
    void AdjustMemoryGridColumnCount(size_t count);
    void UpdateMemoryData();
    void RefreshMemoryGrid(size_t memoryViewtype);
    void RefreshViewMemoryByAddr();
    void RefreshViewMemoryByLocation();
    void RefreshViewMemoryBySize();
    void RefreshViewMemoryByTime();
    bool FilterTest(size_t eip, IMAGEHLP_LINE& info, bool& getAddrSuccess);
    void SetGridRowsCount(wxGrid* pGrid, size_t count);
    int GetMemoryGridViewStartPos();
    int GetMemoryGridViewStartOffset();
    int GetMemoryGridItemCountLimit();
    bool IsMemoryGridNeedUpdateByScroll();
    void SelectMemoryChoice();

private:
    void OnMemoryChoiceChanged(wxCommandEvent& event);
    void OnFilterTextEnter(wxCommandEvent& event);
    void OnFilterTextSelected(wxCommandEvent& event);
    void OnGridCellSelected(wxGridEvent& event);
    void OnSetMemoryStackOffset(wxSpinEvent& event);
    void OnCallStackListDoubleClicked(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

private:
    wxChoice*           m_pMemoryViewChoice;
    wxStaticText*       m_pFilterLabel;
    wxComboBox*         m_pFilterTextComboBox;
    wxCheckBox*         m_pAutoUpdateMemoryCheckBox;
    wxGrid*             m_pMemoryDataGrid;
    wxStaticBox*        m_pMemoryInfoStaticBox;
    wxStaticText*       m_pTotalAllocSizeLabel;
    wxStaticText*       m_pAllocCountLabel;
    wxStaticText*       m_pMaxAllocSizeLabel;
    wxStaticText*       m_pStackOffsetLabel;
    wxTextCtrl*         m_pStackOffsetTextCtrl;
    wxSpinButton*       m_pStackOffsetSpinBtn;
    wxStaticBox*        m_pMemoryCallStackStaticBox;
    wxListBox*          m_pCallStackListBox;
    size_t              m_memoryViewType;
    bool                m_bMemoryUINeedUpdate;
    int                 m_lastSelectedFilterPos;
    int                 m_memoryRecordCacheStartPos;
    DECLARE_EVENT_TABLE();
};
#endif