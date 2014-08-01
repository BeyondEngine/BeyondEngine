#include "stdafx.h"
#include "timebarcheckrenderer.h"
#include "timebarlistmodel.h"
#include "timebarevent.h"

#define RADIUS 3
#define DECREASECOORD 2
#define ICONSIZE 20
TimeBarCheckRenderer::TimeBarCheckRenderer(int type)
                : wxDataViewCustomRenderer(_T("string"), wxDATAVIEW_CELL_ACTIVATABLE, wxALIGN_CENTER)
                , m_iconRect(wxSize(ICONSIZE,ICONSIZE))
                , m_type(type)
{

}

TimeBarCheckRenderer::~TimeBarCheckRenderer()
{

}

bool TimeBarCheckRenderer::Render( wxRect rect, wxDC *dc, int state )
{
    dc->SetBrush( *wxLIGHT_GREY_BRUSH );
    dc->SetPen( *wxTRANSPARENT_PEN );

    rect.Deflate(DECREASECOORD);
    dc->DrawRoundedRectangle( rect, RADIUS );

    RenderText(m_value,
                0, // no offset
                wxRect(dc->GetTextExtent(m_value)).CentreIn(rect),
                dc,
                state);
    return true;
}

bool TimeBarCheckRenderer::ActivateCell(const wxRect& WXUNUSED(cell),
                                      wxDataViewModel *model,
                                      const wxDataViewItem &item,
                                      unsigned int col,
                                      const wxMouseEvent *mouseEvent)
{
    bool bSelected = GetOwner()->GetOwner()->IsSelected(item);
    bool bExamParameter = mouseEvent->m_x < 0 || mouseEvent->m_y < 0 || mouseEvent->m_x > m_iconRect.width || mouseEvent->m_y > m_iconRect.height;
    if (bSelected)
    {
        if (!bExamParameter)
        {
            TimeBarListModel* pMyModel = (TimeBarListModel*)model;
            int row = pMyModel->GetRow(item);
            wxString position;

            wxVariant variant = (char)true;
            if (m_value == _T("+"))
            {
                variant = (char)false;
            }
            pMyModel->SetValueByRow( variant, row, col );
            TimeBarEvent event(TIMEBAR_EVENT_TYPE, 0);
            event.SetRow(row);
            event.SetType(m_type);
            GetOwner()->GetOwner()->GetEventHandler()->ProcessEvent(event);
        }
    }
    return bExamParameter;
}

wxSize TimeBarCheckRenderer::GetSize() const
{
    return m_iconRect.GetSize();
}

bool TimeBarCheckRenderer::SetValue( const wxVariant &value )
{
    bool bRet = value.IsNull();
    if (!bRet)
    {
        m_value = value.GetChar() == 0 ? _T("-") : _T("+");
    }
    return bRet;
}

bool TimeBarCheckRenderer::GetValue( wxVariant &WXUNUSED(value) ) const
{
    return true;
}