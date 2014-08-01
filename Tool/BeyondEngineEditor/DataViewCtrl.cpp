#include "stdafx.h"
#include "DataViewCtrl.h"
#include "timebarframe.h"
#include "DataViewListModel.h"
#include "DataViewCheckRender.h"
#include <wx/dcbuffer.h>
#include "DataViewTreeModelNode.h"
#include "DataViewTreeModel.h"

#define COLUMNWIDTH 25

BEGIN_EVENT_TABLE(CDataViewCtrl, wxDataViewListCtrl)
EVT_SCROLLWIN(CDataViewCtrl::OnScroll)
EVT_MOUSEWHEEL(CDataViewCtrl::OnMouseWheel)
END_EVENT_TABLE()

ArrayOfAnimation::~ArrayOfAnimation()
{

}

CDataViewCtrl::CDataViewCtrl( wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,const wxSize& size, long style,
                     const wxValidator& validator)
                     : wxDataViewListCtrl(parent, id, pos, size, style, validator)
                     , m_pSyncWnd(NULL)
                     , m_pListModel(NULL)
{
    m_pListModel = new CDataViewListModel();
    ShowScrollbars(wxSHOW_SB_NEVER,wxSHOW_SB_NEVER);
}

CDataViewCtrl::~CDataViewCtrl()
{
    AssociateModel(NULL);
    BEATS_SAFE_DELETE(m_pListModel);
}

int CDataViewCtrl::GetRow(const wxDataViewItem & item)
{
    return GetRowByItem(item);
}

void CDataViewCtrl::SyncWith(wxScrollHelperBase *win)
{
    m_pSyncWnd = win;
}

void CDataViewCtrl::AddItem( const wxString &text, bool visible, bool islock, wxUIntPtr userData )
{
    wxVector<wxVariant> data;
    data.push_back( text );
    data.push_back( visible );
    data.push_back( islock );
    AppendItem( data, userData );
}

void* CDataViewCtrl::GetItemDataByRow(size_t uRow)
{
    return (void*)(super::GetItemData(GetItemByRow(uRow)));
}

bool CDataViewCtrl::InitColumn()
{
    AppendTextColumn(_T("Member"), wxDATAVIEW_CELL_EDITABLE);

    AppendColumn(
        new wxDataViewColumn(_T("V"),
        new CDataViewCheckRenderer(Col_Visible),
        Col_Visible,
        COLUMNWIDTH,
        wxALIGN_CENTER,
        0),
        _T("bool")
        );

    AppendColumn(
        new wxDataViewColumn(_T("L"),
        new CDataViewCheckRenderer(Col_Lock),
        Col_Lock,
        COLUMNWIDTH,
        wxALIGN_CENTER,
        0),
        _T("bool")
        );

    return true;
}

void CDataViewCtrl::OnScroll(wxScrollWinEvent& event)
{
    wxPoint point = m_pSyncWnd->GetViewStart();
    if (wxEVT_SCROLLWIN_LINEUP == event.GetEventType())
    {
        point.y -= 1;
        m_pSyncWnd->Scroll(point);
    }
    else if (wxEVT_SCROLLWIN_LINEDOWN == event.GetEventType())
    {
        point.y += 1;
        m_pSyncWnd->Scroll(point);
    }
}

void CDataViewCtrl::RefreshSize()
{
    int iRestFixedWidth = 0;
    wxSize curSize = GetSize();
    int iColumnCount = GetColumnCount();
    for (int i = 1; i < iColumnCount; i++)
    {
        iRestFixedWidth += GetColumn(i)->GetWidth();
    }
    GetColumn(0)->SetWidth(curSize.GetWidth() - iRestFixedWidth);
}

CDataViewListModel* CDataViewCtrl:: GetUserListModel()const
{
    return m_pListModel;
}

void CDataViewCtrl::AssociateListModel()
{
    BEATS_ASSERT(m_pListModel != NULL)
    AssociateModel(m_pListModel);
    InitColumn();
}

void CDataViewCtrl::Clear()
{
    if (m_pListModel)
    {
        int nCount = m_pListModel->GetCount();
        for (int i = 0; i < nCount; i++)
        {
            m_pListModel->DeleteItem(0);
        }
    }
}

void CDataViewCtrl::OnMouseWheel(wxMouseEvent& event)
{
    //set the step when scroll
    event.m_linesPerAction = 1;
}
