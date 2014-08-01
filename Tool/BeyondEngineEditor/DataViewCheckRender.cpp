#include "stdafx.h"
#include "DataViewCheckRender.h"
#include "DataViewListModel.h"
#include "timebarevent.h"
#include "DataViewCtrl.h"

#define RADIUS 3
#define DECREASECOORD 2
#define ICONSIZE 20
CDataViewCheckRenderer::CDataViewCheckRenderer(int type)
                : wxDataViewToggleRenderer(_T("string"), wxDATAVIEW_CELL_ACTIVATABLE, wxALIGN_CENTER)
                , m_iconRect(wxSize(ICONSIZE,ICONSIZE))
                , m_nType(type)
{

}

CDataViewCheckRenderer::~CDataViewCheckRenderer()
{

}

bool CDataViewCheckRenderer::Render( wxRect rect, wxDC *dc, int state )
{
    dc->SetBrush( *wxLIGHT_GREY_BRUSH );
    dc->SetPen( *wxTRANSPARENT_PEN );

    //rect.Deflate(DECREASECOORD);
    m_iconRect = rect;
    dc->DrawRectangle(rect);
    wxString drawText = m_bValue ?  _T("O") : _T("X");
    RenderText(drawText,
                0, // no offset
                wxRect(dc->GetTextExtent(drawText)).CentreIn(rect),
                dc,
                state);
    return true;
}

bool CDataViewCheckRenderer::WXActivateCell(const wxRect& cell,
                                      wxDataViewModel *model,
                                      const wxDataViewItem &item,
                                      unsigned int col,
                                      const wxMouseEvent *mouseEvent)
{
    wxPoint pos = wxPoint(mouseEvent->m_x + cell.x, mouseEvent->m_y + cell.y);
    wxRect hotRect(cell.x, cell.y, m_iconRect.width, m_iconRect.height);
    if (hotRect.Contains(pos))
    {
        model->ChangeValue(!m_bValue, item, col);

//         wxVariant variant;
//         CTimeBarEvent event(TIMEBAR_EVENT_TYPE, 0);
//         CDataViewCtrl* pDataViewCtrl = (CDataViewCtrl*)GetOwner()->GetOwner();
//         if (pDataViewCtrl->GetUserListModel())
//         {
//             CDataViewListModel* pMyModel = (CDataViewListModel*)model;
//             int row = pMyModel->GetRow(item);
//             variant = m_bValue;
//             pMyModel->SetValueByRow( variant, row, col );
//             event.SetRow(row);
//             event.SetType(m_nType);
//         }
//         pDataViewCtrl->GetEventHandler()->ProcessEvent(event);
    }

    return true;
}

bool CDataViewCheckRenderer::SetValue( const wxVariant &value )
{
    m_bValue = value.GetBool();

    return true;
}