#ifndef __TIMELINEBARITEMCELL_H
#define __TIMELINEBARITEMCELL_H

#include "wx/dcmemory.h"

class TimeBarItemCell
{
public:
    enum
    {
        BACKGROUNDCOLOUR1,
        BACKGROUNDCOLOUR2,
        BORDERCOLOUR,
        DATACELLCOLOUR,
        SELECTEDCOLOUR
    };

    TimeBarItemCell();
    ~TimeBarItemCell();

    void        OnDraw(wxMemoryDC& dc, int width);
    wxColour    GetColour(int ColourID);
    void        SetBgColour(int iColour);
    void        SetPositionX(int iPositionX);
    void        SetData(int iData);
    void        SetIsSelected(bool bSelected);

private:

    bool    m_bIsSelected;
    int     m_iPositionX;
    int     m_iData;
    int     m_iBgColour;

};
WX_DECLARE_OBJARRAY(TimeBarItemCell, ArrayOfCell);

#endif