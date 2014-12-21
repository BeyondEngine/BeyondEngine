#include "stdafx.h"
#include "PropertyGridEditor.h"
#include <wx/grid.h>
#include "ListPropertyDescription.h"
#include "PtrPropertyDescription.h"
#include "Component/Component/ComponentProxyManager.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "EnumPropertyDescription.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "wxPtrButtonEditor.h"
#include "wx/msw/private.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include <wx/clipbrd.h>

CPropertyGridEditor::CPropertyGridEditor(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : super(parent, id, title, pos, size, style, name)
    , m_pGridProperty(NULL)
{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    m_pGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0));
    m_pGrid->CreateGrid( 0, 0);

    pMainSizer->Add(m_pGrid, 1, wxEXPAND|wxALL, 0);
    wxBoxSizer* pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pAppendButton = new wxButton(this, wxID_ANY, _T("添加"));
    m_pDeleteButton = new wxButton(this, wxID_ANY, _T("删除"));
    m_pImportButton = new wxButton(this, wxID_ANY, _T("导入"));
    m_pExportButton = new wxButton(this, wxID_ANY, _T("导出"));
    pButtonSizer->Add(m_pAppendButton, 1, wxALL, 0);
    pButtonSizer->Add(m_pDeleteButton, 1, wxALL, 0);
    pButtonSizer->Add(m_pImportButton, 1, wxALL, 0);
    pButtonSizer->Add(m_pExportButton, 1, wxALL, 0);

    pMainSizer->Add(pButtonSizer, 0, wxALL, 0);
    SetMaxClientSize(wxSize(1800, 800));
    SetSizerAndFit(pMainSizer);
    Layout();
    m_pAppendButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnAppendBtnClicked), NULL, this);
    m_pDeleteButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnDeleteBtnClicked), NULL, this);
    m_pImportButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnImportBtnClicked), NULL, this);
    m_pExportButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnExportBtnClicked), NULL, this);
    m_pGrid->Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler(CPropertyGridEditor::OnGridValueChanged), NULL, this);
    m_pGrid->Connect(wxEVT_GRID_EDITOR_SHOWN, wxGridEventHandler(CPropertyGridEditor::OnEditorShown), NULL, this);
    m_pGrid->Connect(wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler(CPropertyGridEditor::OnPasteData), NULL, this);
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CPropertyGridEditor::OnCloseBtnClicked), NULL, this);
}

CPropertyGridEditor::~CPropertyGridEditor()
{
    m_pAppendButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnAppendBtnClicked), NULL, this);
    m_pDeleteButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnDeleteBtnClicked), NULL, this);
    m_pImportButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnImportBtnClicked), NULL, this);
    m_pExportButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnExportBtnClicked), NULL, this);
    m_pGrid->Disconnect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler(CPropertyGridEditor::OnGridValueChanged), NULL, this);
    m_pGrid->Disconnect(wxEVT_GRID_EDITOR_SHOWN, wxGridEventHandler(CPropertyGridEditor::OnEditorShown), NULL, this);
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CPropertyGridEditor::OnCloseBtnClicked), NULL, this);
}

int CPropertyGridEditor::ShowModal()
{
    wxLocale locale;
    locale.Init(wxLANGUAGE_CHINESE_SIMPLIFIED);

    BEATS_ASSERT(m_pGridProperty != NULL);
    CListPropertyDescription* pListProperty = (CListPropertyDescription*)m_pGridProperty->GetClientData();
    CPropertyDescriptionBase* pTemplateProperty = pListProperty->GetTemplateProperty();
    BEATS_ASSERT(pTemplateProperty != NULL);
    if (pTemplateProperty->GetType() == eRPT_Ptr)
    {
        CPtrPropertyDescription* pPtrTemplateProperty = down_cast<CPtrPropertyDescription*>(pTemplateProperty);
        // When first time show an instance in the list, we have a chance to select a derived instance.
        if (pListProperty->GetChildren().size() == 0)
        {
            wxPtrButtonEditor::SelectDerivedInstanceInEditor(pPtrTemplateProperty);
        }
        else
        {
            // If we already got some data in XML to indicate what the instance really is, let the pTemplateProperty to know.
            // So we know what cols should be add in InsertProperty
            CPtrPropertyDescription* pPtrChildProperty = down_cast<CPtrPropertyDescription*>(pListProperty->GetChildren()[0]);
            pPtrTemplateProperty->SetDerivedGuid(pPtrChildProperty->GetDerivedGuid());
        }
    }
    InsertProperty(pTemplateProperty, false);

    uint32_t uChildrenCount = pListProperty->GetChildren().size();
    m_pGrid->AppendRows(uChildrenCount);
    for (uint32_t i = 0; i < uChildrenCount; ++i)
    {
        CPropertyDescriptionBase* pProperty = pListProperty->GetChildren()[i];
        std::vector<CPropertyDescriptionBase*> propertiesToAddInRow;
        BEATS_ASSERT(pProperty != NULL);
        if (pProperty->GetType() == eRPT_Ptr)
        {
            CPtrPropertyDescription* pPtrProperty = down_cast<CPtrPropertyDescription*>(pProperty);
            BEATS_ASSERT(pPtrProperty != NULL);
            CComponentProxy* pProxy = pPtrProperty->GetInstanceComponent();
            if (pProxy != NULL)
            {
                const std::vector<CPropertyDescriptionBase*>* pPropertyPool = pProxy->GetPropertyPool();
                for (uint32_t k = 0; k < pPropertyPool->size(); ++k)
                {
                    if (!pPropertyPool->at(k)->IsHide())
                    {
                        propertiesToAddInRow.push_back(pPropertyPool->at(k));
                    }
                }
            }
        }
        else
        {
            propertiesToAddInRow.push_back(pProperty);
        }
        for (uint32_t j = 0; j < propertiesToAddInRow.size(); ++j)
        {
            UpdateCellValue(i, j, propertiesToAddInRow[j]);
        }
    }
    m_pGrid->AutoSize();
    Fit();
    Layout();
    return super::ShowModal();
}

void CPropertyGridEditor::SetProperty(wxPGProperty* pProperty)
{
    m_pGridProperty = pProperty;
}

wxPGProperty* CPropertyGridEditor::GetProperty() const
{
    return m_pGridProperty;
}

void CPropertyGridEditor::InitEnumCell(int row, int col, CEnumPropertyDescription* pEnumProperty)
{
    const SEnumPropertyData* pData = pEnumProperty->GetEnumPropertyData();
    wxArrayString enumStr;
    for (uint32_t k = 0; k < pData->m_pData->size(); ++k)
    {
        SEnumData* pEnumData = pData->m_pData->at(k);
        BEATS_ASSERT(pEnumData != NULL);
        enumStr.Add(pEnumData->m_str);
    }
    m_pGrid->SetCellEditor(row, col, new wxGridCellChoiceEditor(enumStr));
    m_pGrid->SetCellOverflow(row, col, false);
}

void CPropertyGridEditor::ApplyCellValueToProperty(CPropertyDescriptionBase* pProperty, int nRow, int nCol)
{
    if (*wxRED == m_pGrid->GetCellTextColour(nRow, nCol))
    {

    }
    else
    {
        wxString strValue = m_pGrid->GetCellValue(nRow, nCol);
        if (pProperty->GetType() == eRPT_Bool)
        {
            strValue = strValue.empty() ? _T("false") : _T("true");
        }
        pProperty->GetValueByTChar(strValue, pProperty->GetValue(eVT_CurrentValue));
        pProperty->SetValueWithType(pProperty->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
    }
}

CPropertyDescriptionBase* CPropertyGridEditor::GetPropertyByRowAndCol(int nRow, int nCol) const
{
    CPropertyDescriptionBase* pRet = nullptr;
    CListPropertyDescription* pListProperty = (CListPropertyDescription*)m_pGridProperty->GetClientData();
    BEATS_ASSERT(nRow < (int)pListProperty->GetChildren().size());
    CWxwidgetsPropertyBase* pChildProperty = down_cast<CWxwidgetsPropertyBase*>(pListProperty->GetChildren()[nRow]);
    if (pChildProperty->GetType() == eRPT_Ptr)
    {
        CPtrPropertyDescription* pPtrProperty = down_cast<CPtrPropertyDescription*>(pChildProperty);
        BEATS_ASSERT(pPtrProperty != NULL && pPtrProperty->GetInstanceComponent() != NULL);
        const std::vector<CPropertyDescriptionBase*>* pProperties = pPtrProperty->GetInstanceComponent()->GetPropertyPool();
        std::vector<CPropertyDescriptionBase*> visibleProperties;
        for (uint32_t i = 0; i < pProperties->size(); ++i)
        {
            if (!pProperties->at(i)->IsHide())
            {
                visibleProperties.push_back(pProperties->at(i));
            }
        }
        BEATS_ASSERT(visibleProperties.size() == m_cols.size());
        pRet = visibleProperties[nCol];
    }
    else
    {
        BEATS_ASSERT(m_cols.size() == 1 && nCol == 0);
        pRet = pChildProperty;
    }
    return pRet;
}

void CPropertyGridEditor::OnGridValueChanged(wxGridEvent& event)
{
    uint32_t uRow = event.GetRow();
    uint32_t uCol = event.GetCol();
    CWxwidgetsPropertyBase* pChildProperty = down_cast<CWxwidgetsPropertyBase*>(GetPropertyByRowAndCol(uRow, uCol));
    BEATS_ASSERT(pChildProperty != nullptr);
    ApplyCellValueToProperty(pChildProperty, uRow, uCol);
}

void CPropertyGridEditor::OnExportBtnClicked(wxCommandEvent& /*event*/)
{
    TString szBinaryPath;
    CUtilityManager::GetInstance()->AcquireSingleFilePath(false, NULL, szBinaryPath, _T("选择要导出的文件"), _T("excel file(*.csv)\0*.csv\0\0"), NULL);
    if (szBinaryPath.length() > 0)
    {
        TString strExtensionStr = CFilePathTool::GetInstance()->Extension(szBinaryPath.c_str());
        if (strExtensionStr.compare(wxT(".csv")) != 0)
        {
            szBinaryPath.append(wxT(".csv"));
        }
        uint32_t uRowCount = m_pGrid->GetRows();
        uint32_t uCollomCount = m_pGrid->GetCols();
        wxString data = "";
        for (uint32_t i = 0; i < uRowCount; ++i)
        {
            for (uint32_t j = 0; j < uCollomCount; ++j)
            {
                data.Append(m_pGrid->GetCellValue(i, j));
                if (j != uCollomCount - 1)
                {
                    data.Append(wxT(","));
                }
            }
            data.Append(wxT("\r\n"));
        }
        wxFile file;
        if (file.Open(szBinaryPath, wxFile::write))
        {
            file.Write(data);
        }
        wxMessageBox(wxT("导出完毕!"));
    }
}

void CPropertyGridEditor::OnImportBtnClicked(wxCommandEvent& /*event*/)
{
    TString szBinaryPath;
    CUtilityManager::GetInstance()->AcquireSingleFilePath(false, NULL, szBinaryPath, _T("选择要导入的文件"), _T("excel file(*.csv)\0*.csv\0\0"), NULL);
    if (szBinaryPath.length() > 0)
    {
        wxString data = "";
        wxFile file;
        if (file.Open(szBinaryPath, wxFile::read))
        {
            if (file.ReadAll(&data))
            {
                m_pGrid->ClearGrid();
                ImportData(data);
            }
        }
    }
}

void CPropertyGridEditor::OnAppendBtnClicked(wxCommandEvent& /*event*/)
{
    CListPropertyDescription* pListProperty = (CListPropertyDescription*)m_pGridProperty->GetClientData();
    CPropertyDescriptionBase* pNewProperty = pListProperty->InsertChild(NULL);
    if (pNewProperty->GetType() == eRPT_Ptr)
    {
        down_cast<CPtrPropertyDescription*>(pNewProperty)->CreateInstance(true);
    }
    m_pGrid->AppendRows();
    uint32_t uRowCount = m_pGrid->GetRows();
    for (uint32_t i = 0; i < m_cols.size(); ++i)
    {
        char szBuffer[10240];
        m_cols[i]->GetValueAsChar(eVT_DefaultValue, szBuffer);
        if (m_cols[i]->GetType() != eRPT_Bool)
        {
            if (m_cols[i]->GetType() == eRPT_Enum)
            {
                CEnumPropertyDescription* pEnumProperty = down_cast<CEnumPropertyDescription*>(m_cols[i]);
                InitEnumCell(uRowCount - 1, i, pEnumProperty);
                sprintf(szBuffer, pEnumProperty->GetEnumPropertyData()->m_pData->at(0)->m_str.c_str());
            }
            m_pGrid->SetCellValue(uRowCount - 1, i, szBuffer);
        }
    }
    m_pGrid->AutoSize();
    Fit();
    Layout();
    m_pGrid->MakeCellVisible(uRowCount - 1, 0);
}

void CPropertyGridEditor::OnDeleteBtnClicked(wxCommandEvent& /*event*/)
{
    CListPropertyDescription* pListProperty = (CListPropertyDescription*)m_pGridProperty->GetClientData();
    if (wxIsCtrlDown())
    {
        if (wxMessageBox(_T("要删除所有数据吗？"), _T("删除所有数据"), wxYES_NO) == wxYES)
        {
            m_pGrid->DeleteRows(0, m_pGrid->GetRows());
            pListProperty->RemoveAllChild(true);
            this->Fit();
            this->Layout();
        }
    }
    else
    {
        wxArrayInt rows = m_pGrid->GetSelectedRows();
        if (rows.size() == 1)
        {
            m_pGrid->DeleteRows(rows[0]);
            pListProperty->RemoveChild(pListProperty->GetChildren()[rows[0]], true);
            this->Fit();
            this->Layout();
        }
        else
        {
            wxMessageBox(_T("删除前请先选中单独一行,删除所有请按住ctrl"));
        }
    }
}

void CPropertyGridEditor::OnPasteData(wxKeyEvent& event)
{
    if ((event.GetKeyCode() == 'V') && (event.ControlDown() == true)) 
    {
        wxString copy_data;
        wxString cur_line;

        wxOpenClipboard();
        copy_data = (char *)wxGetClipboardData(wxDF_TEXT);
        wxCloseClipboard();

        int i = m_pGrid->GetGridCursorRow();
        int j = m_pGrid->GetGridCursorCol();
        while (!copy_data.IsEmpty())
        {
            cur_line = copy_data.BeforeFirst('\r');
            if (!cur_line.IsEmpty())
            {
                m_pGrid->SetCellValue(i, j, cur_line);
                CWxwidgetsPropertyBase* pChildProperty = down_cast<CWxwidgetsPropertyBase*>(GetPropertyByRowAndCol(i, j));
                BEATS_ASSERT(pChildProperty != nullptr);
                ApplyCellValueToProperty(pChildProperty, i, j);
                ++i;
                if (i == m_pGrid->GetRows())
                {
                    break;
                }
            }
            copy_data = copy_data.AfterFirst('\n');
        }
    }
}

void CPropertyGridEditor::OnCloseBtnClicked(wxCloseEvent& /*event*/)
{
    CloseDialog();
}

void CPropertyGridEditor::InsertProperty(CPropertyDescriptionBase* pProperty, bool InPtrProperty)
{
    EReflectPropertyType type = pProperty->GetType();
    switch (type)
    {
    case eRPT_Bool:
        m_pGrid->AppendCols();
        m_pGrid->SetColFormatBool(m_pGrid->GetCols() - 1);
        m_pGrid->SetColLabelValue(m_pGrid->GetCols() - 1, wxString::FromUTF8(pProperty->GetBasicInfo()->m_displayName.c_str()));
        m_cols.push_back(pProperty);
        break;
    case eRPT_Float:
    case eRPT_Double:
        m_pGrid->AppendCols();
        m_pGrid->SetColFormatFloat(m_pGrid->GetCols() - 1);
        m_pGrid->SetColLabelValue(m_pGrid->GetCols() - 1, wxString::FromUTF8(pProperty->GetBasicInfo()->m_displayName.c_str()));
        m_cols.push_back(pProperty);
        break;
    case eRPT_Int:
    case eRPT_UInt:
        m_pGrid->AppendCols();
        m_pGrid->SetColFormatNumber(m_pGrid->GetCols() - 1);
        m_pGrid->SetColLabelValue(m_pGrid->GetCols() - 1, wxString::FromUTF8(pProperty->GetBasicInfo()->m_displayName.c_str()));
        m_cols.push_back(pProperty);
        break;
    case eRPT_Enum:
    case eRPT_Str:
        m_pGrid->AppendCols();
        m_pGrid->SetColLabelValue(m_pGrid->GetCols() - 1, wxString::FromUTF8(pProperty->GetBasicInfo()->m_displayName.c_str()));
        m_cols.push_back(pProperty);
        break;
    case eRPT_Ptr:
        {
            if (InPtrProperty)
            {
                m_pGrid->AppendCols();
                m_pGrid->SetColLabelValue(m_pGrid->GetCols() - 1, wxString::FromUTF8(pProperty->GetBasicInfo()->m_displayName.c_str()));
                wxGridCellAttr *attr = new wxGridCellAttr;
                attr->SetTextColour(*wxRED);
                m_pGrid->SetColAttr(m_pGrid->GetCols() - 1, attr);
                m_cols.push_back(pProperty);
            }
            else
            {
                CPtrPropertyDescription* pPtrProperty = down_cast<CPtrPropertyDescription*>(pProperty);
                BEATS_ASSERT(pPtrProperty != NULL);
                uint32_t uGuid = pPtrProperty->GetDerivedGuid() == 0 ? pPtrProperty->GetPtrGuid() : pPtrProperty->GetDerivedGuid();
                CComponentProxy* pProxy = down_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentTemplate(uGuid));
                pProxy->UpdateHostComponent(); // Force call reflect data, so the HIDE_PROPERTY will work.
                const std::vector<CPropertyDescriptionBase*>* pPropertyPool = pProxy->GetPropertyPool();
                for (uint32_t i = 0; i < pPropertyPool->size(); ++i)
                {
                    CPropertyDescriptionBase* pProperty = pPropertyPool->at(i);
                    if (!pProperty->IsHide())
                    {
                        InsertProperty(pPropertyPool->at(i), true);
                    }
                }
            }
        }
        break;
    default:
        {
            m_pGrid->AppendCols();
            m_pGrid->SetColLabelValue(m_pGrid->GetCols() - 1, wxString::FromUTF8(pProperty->GetBasicInfo()->m_displayName.c_str()));
            wxGridCellAttr *attr = new wxGridCellAttr;
            attr->SetTextColour(*wxRED);
            m_pGrid->SetColAttr(m_pGrid->GetCols() - 1, attr);
            m_cols.push_back(pProperty);
        }
        break;
    }
}

void CPropertyGridEditor::CloseDialog()
{
    m_pGrid->ClearGrid();
    uint32_t uRowCount = m_pGrid->GetRows();
    if (uRowCount > 0)
    {
        m_pGrid->DeleteRows(0, uRowCount);
    }
    uint32_t uColCount = m_pGrid->GetCols();
    if (uColCount > 0)
    {
        m_pGrid->DeleteCols(0, uColCount);
    }
    m_cols.clear();
    this->EndDialog(0);
    BEATS_ASSERT(m_pGridProperty != NULL);
    CListPropertyDescription* pListProperty = (CListPropertyDescription*)m_pGridProperty->GetClientData();
    CEngineCenter::GetInstance()->m_editorPropertyGridSyncList.insert(pListProperty);
}

void CPropertyGridEditor::ImportData(wxString &data)
{
    wxString rowFlag = _T("\r\n");
    wxString strFlag = _T(",");
    std::vector<wxString> rows = SplitString(data, rowFlag);
    if (rows.size() != 0)
    {
        std::vector<wxString> rowStrs = SplitString(rows[0], strFlag);
        if (rowStrs.size() != m_cols.size())
        {
            wxMessageBox(_T("列数不匹配, 导入失败"));
            return;
        }
        else
        {
            CListPropertyDescription* pListProperty = (CListPropertyDescription*)m_pGridProperty->GetClientData();
            BEATS_ASSERT(pListProperty != nullptr);
            if (m_pGrid->GetRows() > 0)
            {
                BEATS_ASSERT((int)pListProperty->GetChildren().size() == m_pGrid->GetRows());
                m_pGrid->DeleteRows(0, m_pGrid->GetRows());
                pListProperty->RemoveAllChild(true);
            }
            CEngineCenter::GetInstance()->m_editorPropertyGridSyncList.insert(pListProperty);
            for (size_t i = 0; i < rows.size(); i++)
            {
                m_pGrid->AppendRows();
                CPropertyDescriptionBase* pNewChild = pListProperty->InsertChild(nullptr);
                if (pNewChild->GetType() == eRPT_Ptr)
                {
                    down_cast<CPtrPropertyDescription*>(pNewChild)->CreateInstance(true);
                }
                std::vector<wxString> rowStrs = SplitString(rows[i], strFlag);
                for (size_t j = 0; j < rowStrs.size(); j++)
                {
                    if (m_cols[j]->GetType() == eRPT_Enum)
                    {
                        CEnumPropertyDescription* pEnumProperty = down_cast<CEnumPropertyDescription*>(m_cols[j]);
                        InitEnumCell(i, j, pEnumProperty);
                    }
                    m_pGrid->SetCellValue(i, j, rowStrs[j]);
                    CWxwidgetsPropertyBase* pChildProperty = down_cast<CWxwidgetsPropertyBase*>(GetPropertyByRowAndCol(i, j));
                    BEATS_ASSERT(pChildProperty != nullptr);
                    ApplyCellValueToProperty(pChildProperty, i, j);
                }
            }
            this->Fit();
            this->Layout();
        }
    }
    wxMessageBox(wxT("导入成功!"));
}

std::vector<wxString> CPropertyGridEditor::SplitString(wxString& data, wxString& str)
{
    std::vector<wxString> vec = {};
    int index = 0;
    while (true)
    {
        int start = index;
        index = data.find(str, index);
        if (index != wxNOT_FOUND)
        {
            vec.push_back(data.SubString(start, index - 1));
        }
        else
        {
            if (!data.EndsWith(str))
            {
                vec.push_back(data.SubString(start, data.Length()));
            }
            break;
        }
        index += str.Length();
    }
    return vec;
}

void CPropertyGridEditor::UpdateCellValue(int row, int col, CPropertyDescriptionBase* pProperty)
{
    BEATS_ASSERT(pProperty != nullptr);
    char szResult[10240];
    pProperty->GetValueAsChar(eVT_CurrentValue, szResult);
    if (pProperty->GetType() == eRPT_Bool)
    {
        bool bValue = *((bool*)pProperty->GetValue(eVT_CurrentValue));
        szResult[0] = bValue ? '1' : 0;
        szResult[1] = 0;
    }
    else if (pProperty->GetType() == eRPT_Enum)
    {
        CEnumPropertyDescription* pEnumProperty = down_cast<CEnumPropertyDescription*>(pProperty);
        InitEnumCell(row, col, pEnumProperty);
        const TString& strCurValue = *((TString*)(pEnumProperty->GetValue(eVT_CurrentValue)));
        strcpy(szResult, strCurValue.c_str());
    }
    m_pGrid->SetCellValue(row, col, wxString::FromUTF8(szResult));
}

void CPropertyGridEditor::ShowModal(CPropertyDescriptionBase* pProperty, wxDialog* pParent)
{
    int style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPG_TOOLTIPS;
    int extraStyle = wxPG_EX_MODE_BUTTONS | wxPG_EX_MULTIPLE_SELECTION;
    std::vector<CPropertyDescriptionBase*> vec{ pProperty };
    CEnginePropertyGridManager* pPropGridManager = new CEnginePropertyGridManager();
    pPropGridManager->Create(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    pParent->GetSizer()->Add(pPropGridManager, 1, wxEXPAND | wxALL, 0);
    pPropGridManager->SetExtraStyle(extraStyle);
    pPropGridManager->SetValidationFailureBehavior(wxPG_VFB_MARK_CELL | wxPG_VFB_SHOW_MESSAGEBOX);
    pPropGridManager->GetGrid()->SetVerticalSpacing(2);
    pPropGridManager->AddPage(wxT("page"));
    pPropGridManager->ClearPage(0);
    pPropGridManager->InsertInPropertyGrid(vec, NULL);
    pPropGridManager->ExpandAll(false);
    pParent->SetMaxClientSize(wxSize(1800, 800));
    pParent->ShowModal();
}

void CPropertyGridEditor::OnEditorShown(wxGridEvent& event)
{
    uint32_t uRow = event.GetRow();
    uint32_t uCol = event.GetCol();
    CListPropertyDescription* pListProperty = (CListPropertyDescription*)m_pGridProperty->GetClientData();
    if (uRow < pListProperty->GetChildren().size())
    {
        CPropertyDescriptionBase* pChildProperty = down_cast<CWxwidgetsPropertyBase*>(pListProperty->GetChildren()[uRow]);
        if (*wxRED == m_pGrid->GetCellTextColour(uRow, uCol))
        {
            CPropertyDescriptionBase* pProperty = pChildProperty;
            if (pChildProperty->GetType() == eRPT_Ptr)
            {
                auto vec = *(pChildProperty->GetInstanceComponent()->GetPropertyPool());
                pProperty = vec[uCol];
                wxDialog* pDlg = new wxDialog(this, wxID_ANY, wxT("Grid"), wxDefaultPosition, wxSize(480, 600), wxCLOSE_BOX | wxRESIZE_BORDER | wxCAPTION);
                wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
                pDlg->SetSizer(pMainSizer);
                ShowModal(pProperty, pDlg);
                UpdateCellValue(uRow, uCol, pProperty);
                event.Skip();
            }
        }
    }
}
