#include "stdafx.h"
#include "timebardataviewctrl.h"
#include "timebarframe.h"
#include "timebarlistmodel.h"
#include "timebarcheckrenderer.h"
#include <wx/dcbuffer.h>

#define COLUMNWIDTH 25

BEGIN_EVENT_TABLE(TimeBarDataViewCtrl, wxDataViewCtrl)
EVT_SCROLLWIN(TimeBarDataViewCtrl::OnScroll)
END_EVENT_TABLE()

ArrayOfAnimation::~ArrayOfAnimation()
{

}

TimeBarDataViewCtrl::TimeBarDataViewCtrl( wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,const wxSize& size, long style,
                     const wxValidator& validator,const wxString& name)
                     :wxDataViewCtrl(parent, id, pos, size, style, validator, name)
{
    m_pSyncWnd = NULL;
    m_list_model = new TimeBarListModel();
    AssociateModel( m_list_model.get() );
    InitCtrl();
}


int TimeBarDataViewCtrl::GetRow(const wxDataViewItem & item)
{
    return GetRowByItem(item);
}

void TimeBarDataViewCtrl::SyncWith(wxScrollHelperBase *win)
{
    m_pSyncWnd = win;
}

void TimeBarDataViewCtrl::AddItem( const wxString &text, bool visible, bool islock )
{
    m_list_model->AddItem(text, visible, islock);
}

TimeBarListModel* TimeBarDataViewCtrl:: GetUserModel()const
{
    return m_list_model.get();
}

bool TimeBarDataViewCtrl::InitCtrl()
{
    ShowScrollbars(wxSHOW_SB_NEVER,wxSHOW_SB_NEVER);
    AppendTextColumn(_T("Layer"),
        Col_EditableText,
        wxDATAVIEW_CELL_EDITABLE,
        wxCOL_WIDTH_AUTOSIZE);

    AppendColumn(
        new wxDataViewColumn(_T("V"),
        new TimeBarCheckRenderer(Col_Visible),
        Col_Visible,
        COLUMNWIDTH,
        wxALIGN_CENTER,
        0)
        );

    AppendColumn(
        new wxDataViewColumn(_T("L"),
        new TimeBarCheckRenderer(Col_Lock),
        Col_Lock,
        COLUMNWIDTH,
        wxALIGN_CENTER,
        0)
        );

    return true;
}

void TimeBarDataViewCtrl::OnScroll(wxScrollWinEvent& /*event*/)
{
    if (m_pSyncWnd != NULL)
    {
        m_pSyncWnd->Scroll(GetViewStart());
    }
}
void TimeBarDataViewCtrl::RefreshSize()
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