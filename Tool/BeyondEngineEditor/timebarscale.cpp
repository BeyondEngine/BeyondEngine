#include "stdafx.h"
#include "timebarscale.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/dcmirror.h"
#include "timebarframe.h"
#include "wx/dcbuffer.h"

const static int SCALELINELENGTHOFFSET = 5;
const static int SCALETEXTOFFSETY  = -23;

CTimeBarScale::CTimeBarScale( wxWindow *parent )
    : wxPanel (parent)
    , m_iCursorPositionX(0)
{
    SetMinSize(wxSize(0, SCALEBARHIGHT));
}

BEGIN_EVENT_TABLE(CTimeBarScale, wxPanel)
EVT_MOUSE_EVENTS(CTimeBarScale::OnMouse)
EVT_PAINT(CTimeBarScale::OnPaint)
END_EVENT_TABLE()

void CTimeBarScale::DrawScale()
{
    int iCount = m_pTimeBarFrame->GetFrameCount();
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxAutoBufferedPaintDC ScaleDC(this); 
    if ( iCount > 0 )
    {
        static const size_t ScaleTextOffset = 5;
        wxPoint ViewStart = m_pTimeBarFrame->GetItemContainer()->GetViewStart();
        wxRect ScaleRect = GetRect();
        int iCellWidth = m_pTimeBarFrame->GetCellWidth();
        int iPositionX = m_pTimeBarFrame->GetCursorPositionX() - ViewStart.x * iCellWidth;
        int iTextPositionX = -1;
        int iTextPositionY = ScaleRect.height + SCALETEXTOFFSETY;
        int iScaleLineLength = ScaleRect.height - SCALELINELENGTHOFFSET;
        ScaleDC.DrawRectangle(ScaleRect);
        ScaleDC.SetFont(*wxSMALL_FONT);

        for(int i = 0; i < iCount; ++i)
        {
            iTextPositionX = i*iCellWidth;
            int iLineHeight = iScaleLineLength;
            if((ViewStart.x + i) % ScaleTextOffset == 0 || i == iCount -1)
            {
                ScaleDC.DrawText(wxString::Format(_T("%d"),i + ViewStart.x), iTextPositionX, iTextPositionY);
                iLineHeight -= 4;
            }
            ScaleDC.DrawLine(iTextPositionX , ScaleRect.height, iTextPositionX , iLineHeight);
        }
        if (iPositionX >= 0)
        {
            wxPen pen = *wxRED_PEN;
            pen.SetWidth(CURSORWIDTH);
            ScaleDC.SetPen(pen);
            ScaleDC.DrawLine(iPositionX, ScaleRect.height, iPositionX, 0);
        }
    }
    else
    {
        ScaleDC.Clear();
    }
}

void CTimeBarScale::OnMouse( wxMouseEvent& event )
{
    CTimeBarItemContainer* pContiner = m_pTimeBarFrame->GetItemContainer();
    int iCellWidth = m_pTimeBarFrame->GetCellWidth();
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        if (!HasCapture())
        {
            CaptureMouse();
        }
        m_pTimeBarFrame->SetClickOnScalebar(true);
        wxPoint pstn = event.GetPosition();
        int iScale = PointToScale(pstn);
        int iPositionX = iScale + pContiner->GetViewStart().x;
        iPositionX *= iCellWidth;
        iPositionX += iCellWidth/2;
        m_pTimeBarFrame->SetCursorPositionX(iPositionX);
        m_pTimeBarFrame->ClickOnScaleBar();
    }
    else if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        if (HasCapture())
        {
            ReleaseMouse();
        }
    }
    else if (event.Dragging())
    {   
        wxPoint pstn= event.GetPosition();
        if (pstn.x % FREQUENCYREDUCTIONFACTOR == 0)
        {
            int iScale = PointToScale(pstn);
            int iPositionX = iScale + pContiner->GetViewStart().x;
            iPositionX *= iCellWidth;
            iPositionX += iCellWidth/2;
            m_pTimeBarFrame->SetCursorPositionX(iPositionX);
            m_pTimeBarFrame->ClickOnScaleBar();
        } 
    }
    event.Skip();
}

void CTimeBarScale::SetTimeBarFrameWindow( CTimeBarFrame* pSplitterWindow )
{
    m_pTimeBarFrame = pSplitterWindow;
}

CTimeBarFrame* CTimeBarScale::GetTimeBarFrameWindow()
{
    return m_pTimeBarFrame;
}

int CTimeBarScale::PointToScale( wxPoint point )
{
    int iCellWidth = m_pTimeBarFrame->GetCellWidth();
    return point.x / iCellWidth;
}

wxPoint CTimeBarScale::ScaleToPoint( int iScale )
{
    int iCellWidth = m_pTimeBarFrame->GetCellWidth();
    return wxPoint(iScale * iCellWidth, 0);
}

void CTimeBarScale::OnPaint( wxPaintEvent& /*event*/ )
{
    DrawScale();
}
