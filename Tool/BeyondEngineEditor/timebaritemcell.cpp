#include "stdafx.h"
#include "timebaritemcell.h"

#define DATACELLHIGHT 18
#define NODATACELLHIGHT 24
#define DATACELLOFFSET 3
#define WIDTHADJUSTMENT 1
#define POSITIONADJUSTMENT -1

TimeBarItemCell::TimeBarItemCell()
: m_bIsSelected(false)
{

}

TimeBarItemCell::~TimeBarItemCell()
{

}

void TimeBarItemCell::OnDraw(wxMemoryDC& dc, int width)
{
    bool bHasData = m_iData == 1;
    int iDrawCellWidth = width + WIDTHADJUSTMENT;
    int iDrawPositionX = m_iPositionX + POSITIONADJUSTMENT;
    wxColour PenColour = GetColour(BORDERCOLOUR);
    wxColour BrushColour = GetColour(m_iBgColour);
    
    dc.SetPen(PenColour);
    dc.SetBrush(wxBrush(BrushColour));
    dc.DrawRectangle(iDrawPositionX, 0, iDrawCellWidth, NODATACELLHIGHT);

    if (m_bIsSelected)
    {
        PenColour = GetColour(BORDERCOLOUR);
        BrushColour = GetColour(SELECTEDCOLOUR);
    }
    else if (bHasData)
    {
        PenColour = GetColour(BORDERCOLOUR);
        BrushColour = GetColour(DATACELLCOLOUR);
    }

    dc.SetPen(PenColour);
    dc.SetBrush(wxBrush(BrushColour));

    if (bHasData)
    {
        dc.DrawRectangle(iDrawPositionX, DATACELLOFFSET, iDrawCellWidth, DATACELLHIGHT);
    }
    else
    {
        dc.DrawRectangle(iDrawPositionX, 0, iDrawCellWidth, NODATACELLHIGHT);
    }
}

wxColour TimeBarItemCell::GetColour(int ColourID)
{
    wxColour color = *wxWHITE;
    switch (ColourID)
    {
    case BACKGROUNDCOLOUR2:
        color = *wxLIGHT_GREY;
        break;

    case DATACELLCOLOUR:
        color = *wxGREEN;
        break;

    case SELECTEDCOLOUR:
        color = *wxBLUE;
        break;

    case BORDERCOLOUR:
        color = *wxLIGHT_GREY;
        break;

    case BACKGROUNDCOLOUR1:
    default:
        break;
    }

    return color;
}

void TimeBarItemCell::SetBgColour(int iColour)
{
    m_iBgColour = iColour;
}

void TimeBarItemCell::SetPositionX(int iPositionX)
{
    m_iPositionX = iPositionX;
}

void TimeBarItemCell::SetData(int iData)
{
    m_iData = iData;
}

void TimeBarItemCell::SetIsSelected(bool bSelected)
{
    m_bIsSelected = bSelected;
}

ArrayOfCell::~ArrayOfCell()
{

}

