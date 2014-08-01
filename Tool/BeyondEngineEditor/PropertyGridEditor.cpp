#include "stdafx.h"
#include "PropertyGridEditor.h"
#include <wx/grid.h>
#include "ListPropertyDescription.h"
#include "PtrPropertyDescription.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "EnumPropertyDescription.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "wxPtrButtonEditor.h"

CPropertyGridEditor::CPropertyGridEditor(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : super(parent, id, title, pos, size, style, name)
    , m_pGridProperty(NULL)
{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    m_pGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0));
    m_pGrid->CreateGrid( 0, 0);

    pMainSizer->Add(m_pGrid, 1, wxALL, 0);
    wxBoxSizer* pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pAppendButton = new wxButton(this, wxID_ANY, _T("添加"));
    m_pDeleteButton = new wxButton(this, wxID_ANY, _T("删除"));
    m_pConfirmButton = new wxButton(this, wxID_ANY, _T("确定"));
    m_pCancelButton = new wxButton(this, wxID_ANY, _T("取消"));
    pButtonSizer->Add(m_pAppendButton, 1, wxALL, 0);
    pButtonSizer->Add(m_pDeleteButton, 1, wxALL, 0);
    pButtonSizer->Add(m_pConfirmButton, 1, wxALL, 0);
    pButtonSizer->Add(m_pCancelButton, 1, wxALL, 0);

    pMainSizer->Add(pButtonSizer, 0, wxALL, 0);
    this->SetSizerAndFit(pMainSizer);
    this->Layout();

    m_pAppendButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnAppendBtnClicked), NULL, this);
    m_pDeleteButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnDeleteBtnClicked), NULL, this);
    m_pConfirmButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnConfirmBtnClicked), NULL, this);
    m_pCancelButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnCancelBtnClicked), NULL, this);
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CPropertyGridEditor::OnCloseBtnClicked), NULL, this);
}

CPropertyGridEditor::~CPropertyGridEditor()
{
    m_pAppendButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnAppendBtnClicked), NULL, this);
    m_pDeleteButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnDeleteBtnClicked), NULL, this);
    m_pConfirmButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnConfirmBtnClicked), NULL, this);
    m_pCancelButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CPropertyGridEditor::OnCancelBtnClicked), NULL, this);
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CPropertyGridEditor::OnCloseBtnClicked), NULL, this);
}

int CPropertyGridEditor::ShowModal()
{
    wxLocale locale;
    locale.Init(wxLANGUAGE_CHINESE_SIMPLIFIED);

    BEATS_ASSERT(m_pGridProperty != NULL);
    CListPropertyDescription* pListProperty = (CListPropertyDescription*)m_pGridProperty->GetClientData();
    // When first time show an instance in the list, we have a chance to select a derived instance.
    CPtrPropertyDescription* pTemplateProperty = down_cast<CPtrPropertyDescription*>(pListProperty->GetTemplateProperty());
    if (pListProperty->GetChildrenCount() == 0)
    {
        wxPtrButtonEditor::SelectDerivedInstanceInEditor(pTemplateProperty);
    }
    else
    {
        // If we already got some data in XML to indicate what the instance really is, let the pTemplateProperty to know.
        // So we know what cols should be add in InsertProperty
        CPtrPropertyDescription* pPtrChildProperty = down_cast<CPtrPropertyDescription*>(pListProperty->GetChild(0));
        pTemplateProperty->SetDerivedGuid(pPtrChildProperty->GetDerivedGuid());
    }
    BEATS_ASSERT(pTemplateProperty != NULL);
    InsertProperty(pTemplateProperty, false);

    size_t uChildrenCount = pListProperty->GetChildrenCount();
    m_pGrid->AppendRows(uChildrenCount);
    for (size_t i = 0; i < uChildrenCount; ++i)
    {
        CPropertyDescriptionBase* pProperty = pListProperty->GetChild(i);
        std::vector<CPropertyDescriptionBase*> propertiesToAddInRow;
        BEATS_ASSERT(pProperty != NULL);
        if (pProperty->GetType() == eRPT_Ptr)
        {
            CPtrPropertyDescription* pPtrProperty = down_cast<CPtrPropertyDescription*>(pProperty);
            BEATS_ASSERT(pPtrProperty != NULL && pPtrProperty->GetInstanceComponent() != NULL);
            CComponentProxy* pProxy = pPtrProperty->GetInstanceComponent();
            propertiesToAddInRow = *pProxy->GetPropertyPool();
        }
        else
        {
            propertiesToAddInRow.push_back(pProperty);
        }
        for (size_t j = 0; j < propertiesToAddInRow.size(); ++j)
        {
            char szResult[10240];
            propertiesToAddInRow[j]->GetValueAsChar(eVT_CurrentValue, szResult);
            if (propertiesToAddInRow[j]->GetType() == eRPT_Bool)
            {
                bool bValue = *((bool*)propertiesToAddInRow[j]->GetValue(eVT_CurrentValue));
                szResult[0] = bValue ? '1' : 0;
                szResult[1] = 0;
            }
            else if (propertiesToAddInRow[j]->GetType() == eRPT_Enum)
            {
                CEnumPropertyDescription* pEnumProperty = down_cast<CEnumPropertyDescription*>(propertiesToAddInRow[j]);
                InitEnumCell(i, j, pEnumProperty);
                int nCurValue = *((int*)(pEnumProperty->GetValue(eVT_CurrentValue)));
                strcpy(szResult, pEnumProperty->QueryStringByValue(nCurValue).c_str());
            }
            m_pGrid->SetCellValue(i, j, szResult);
        }
    }
    m_pGrid->AutoSize();
    this->Fit();
    this->Layout();
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
    for (size_t k = 0; k < pData->m_pData->size(); ++k)
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
    wxString strValue = m_pGrid->GetCellValue(nRow, nCol);
    if (pProperty->GetType() == eRPT_Bool)
    {
        strValue = strValue.empty() ? _T("false") : _T("true");
    }
    else if (pProperty->GetType() == eRPT_Enum)
    {
        CEnumPropertyDescription* pEnumProperty = down_cast<CEnumPropertyDescription*>(pProperty);
        TString strTmp = strValue;
        int nRet = pEnumProperty->QueryValueByString(strTmp);
        strValue = wxString::Format(_T("%d"), nRet);
    }
    pProperty->GetValueByTChar(strValue, pProperty->GetValue(eVT_CurrentValue));
}

void CPropertyGridEditor::OnConfirmBtnClicked(wxCommandEvent& /*event*/)
{
    CListPropertyDescription* pListProperty = (CListPropertyDescription*)m_pGridProperty->GetClientData();
    pListProperty->DeleteAllChild();
    size_t uGridRows = m_pGrid->GetRows();
    for (size_t i = 0; i < uGridRows; ++i)
    {
        CWxwidgetsPropertyBase* pNewProperty = down_cast<CWxwidgetsPropertyBase*>(pListProperty->AddChild(NULL));
        pNewProperty->Initialize();
        BEATS_ASSERT(m_cols.size() > 0 && pNewProperty != NULL);
        if (pNewProperty->GetType() == eRPT_Ptr)
        {
            CPtrPropertyDescription* pPtrProperty = down_cast<CPtrPropertyDescription*>(pNewProperty);
            BEATS_ASSERT(pPtrProperty != NULL && pPtrProperty->GetInstanceComponent() == NULL);
            pPtrProperty->CreateInstance();
            const std::vector<CPropertyDescriptionBase*>* pProperties = pPtrProperty->GetInstanceComponent()->GetPropertyPool();
            BEATS_ASSERT(pProperties->size() == m_cols.size());
            for (size_t j = 0; j < pProperties->size(); ++j)
            {
                CPropertyDescriptionBase* pProperty = pProperties->at(j);
                ApplyCellValueToProperty(pProperty, i, j);
            }
        }
        else
        {
            BEATS_ASSERT(m_cols.size() == 1);
            ApplyCellValueToProperty(pNewProperty, i, 0);
        }
    }
    char valueStr[256];
    pListProperty->GetValueAsChar(eVT_CurrentValue, valueStr);
    m_pGridProperty->SetValue(valueStr);
    m_pGridProperty->SetModifiedStatus(pListProperty->GetChildrenCount() > 0);
    m_pGridProperty->RecreateEditor();
    CloseDialog();
}

void CPropertyGridEditor::OnCancelBtnClicked(wxCommandEvent& /*event*/)
{
    CloseDialog();
}

void CPropertyGridEditor::OnAppendBtnClicked(wxCommandEvent& /*event*/)
{
    m_pGrid->AppendRows();
    size_t uRowCount = m_pGrid->GetRows();
    for (size_t i = 0; i < m_cols.size(); ++i)
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
    this->Fit();
    this->Layout();
}

void CPropertyGridEditor::OnDeleteBtnClicked(wxCommandEvent& /*event*/)
{
    wxArrayInt rows = m_pGrid->GetSelectedRows();
    if(rows.size() == 1)
    {
        m_pGrid->DeleteRows(rows[0]);
        this->Fit();
        this->Layout();
    }
    else
    {
        wxMessageBox(_T("删除前请先选中单独一行"));
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
        m_pGrid->SetColLabelValue(m_pGrid->GetCols() - 1, pProperty->GetBasicInfo()->m_displayName.c_str());
        m_cols.push_back(pProperty);
        break;
    case eRPT_Float:
    case eRPT_Double:
        m_pGrid->AppendCols();
        m_pGrid->SetColFormatFloat(m_pGrid->GetCols() - 1);
        m_pGrid->SetColLabelValue(m_pGrid->GetCols() - 1, pProperty->GetBasicInfo()->m_displayName.c_str());
        m_cols.push_back(pProperty);
        break;
    case eRPT_Int:
    case eRPT_UInt:
        m_pGrid->AppendCols();
        m_pGrid->SetColFormatNumber(m_pGrid->GetCols() - 1);
        m_pGrid->SetColLabelValue(m_pGrid->GetCols() - 1, pProperty->GetBasicInfo()->m_displayName.c_str());
        m_cols.push_back(pProperty);
        break;
    case eRPT_Enum:
        m_pGrid->AppendCols();
        m_pGrid->SetColLabelValue(m_pGrid->GetCols() - 1, pProperty->GetBasicInfo()->m_displayName.c_str());
        m_cols.push_back(pProperty);
        break;
    case eRPT_Ptr:
        {
            BEATS_ASSERT(!InPtrProperty, _T("ptr property Can't nested in grid mode!"));
            BEYONDENGINE_UNUSED_PARAM(InPtrProperty);
            CPtrPropertyDescription* pPtrProperty = down_cast<CPtrPropertyDescription*>(pProperty);
            BEATS_ASSERT(pPtrProperty != NULL);
            size_t uGuid = pPtrProperty->GetDerivedGuid() == 0 ? pPtrProperty->GetPtrGuid() : pPtrProperty->GetDerivedGuid();
            CComponentProxy* pProxy = down_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentTemplate(uGuid));
            const std::vector<CPropertyDescriptionBase*>* pPropertyPool = pProxy->GetPropertyPool();
            for (size_t i = 0; i < pPropertyPool->size(); ++i)
            {
                InsertProperty(pPropertyPool->at(i), true);
            }
        }
        break;
    default:
        BEATS_ASSERT(false, _T("Unsupport col format of property type %d in list property grid mode!"), type);
        break;
    }
}

void CPropertyGridEditor::CloseDialog()
{
    m_pGrid->ClearGrid();
    size_t uRowCount = m_pGrid->GetRows();
    if (uRowCount > 0)
    {
        m_pGrid->DeleteRows(0, uRowCount);
    }
    size_t uColCount = m_pGrid->GetCols();
    if (uColCount > 0)
    {
        m_pGrid->DeleteCols(0, uColCount);
    }
    m_cols.clear();
    this->EndDialog(0);
}
