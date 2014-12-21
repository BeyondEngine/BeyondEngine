#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_PROPERTYGRIDEDITOR_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_PROPERTYGRIDEDITOR_H__INCLUDE

#include "EditDialogBase.h"

class wxGrid;
class wxGridEvent;
class wxPGProperty;
class CPropertyDescriptionBase;
class CEnumPropertyDescription;

class CPropertyGridEditor : public CEditDialogBase
{
    typedef CEditDialogBase super;
public:
    CPropertyGridEditor(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style =  wxRESIZE_BORDER | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~CPropertyGridEditor();

public:
    virtual int ShowModal() override;
    void ShowModal(CPropertyDescriptionBase* pProperty, wxDialog* pParent);
    void SetProperty(wxPGProperty* pProperty);
    wxPGProperty* GetProperty() const;

private:
    void InitEnumCell(int row, int col, CEnumPropertyDescription* pEnumProperty);
    void ApplyCellValueToProperty(CPropertyDescriptionBase* pProperty, int nRow, int nCol);
    CPropertyDescriptionBase* GetPropertyByRowAndCol(int nRow, int nCol) const;
private:
    void OnGridValueChanged(wxGridEvent& event);
    void OnImportBtnClicked(wxCommandEvent& event);
    void OnExportBtnClicked(wxCommandEvent& event);
    void OnAppendBtnClicked(wxCommandEvent& event);
    void OnDeleteBtnClicked(wxCommandEvent& event);
    void OnPasteData(wxKeyEvent& event);
    void OnCloseBtnClicked(wxCloseEvent& event);
    void OnEditorShown(wxGridEvent& event);
    void InsertProperty(CPropertyDescriptionBase* pProperty, bool InPtrProperty);
    void CloseDialog();
    void ImportData(wxString& data);
    std::vector<wxString> SplitString(wxString& data, wxString& str);
    void UpdateCellValue(int row, int col, CPropertyDescriptionBase* pProperty);
private:
    wxGrid* m_pGrid;
    wxButton* m_pAppendButton;
    wxButton* m_pDeleteButton;
    wxButton* m_pImportButton;
    wxButton* m_pExportButton;
    wxPGProperty* m_pGridProperty;
    std::vector<CPropertyDescriptionBase*> m_cols;
};

#endif