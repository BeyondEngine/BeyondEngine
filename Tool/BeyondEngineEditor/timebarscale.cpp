#include "stdafx.h"
#include "timebarscale.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/dcmirror.h"
#include "timebarframe.h"
#include "wx/dcbuffer.h"

const static int SCALELINELENGTHOFFSET = 5;
const static int SCALETEXTOFFSETY  = -23;
#define SCALEBARHIGHT 24

CTimeBarScale::CTimeBarScale( wxWindow *parent )
    : wxPanel (parent)
{
    SetMinSize(wxSize(0, SCALEBARHIGHT));
}

BEGIN_EVENT_TABLE(CTimeBarScale, wxPanel)
EVT_MOUSE_EVENTS(CTimeBarScale::OnMouse)
EVT_PAINT(CTimeBarScale::OnPaint)
EVT_MOUSE_CAPTURE_LOST(CTimeBarScale::OnMouseCaptureLost)
END_EVENT_TABLE()

void CTimeBarScale::DrawScale()
{
    int iCount = m_pTimeBarFrame->GetFrameCountForDraw();
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxAutoBufferedPaintDC ScaleDC(this); 
    static const uint32_t ScaleTextOffset = 5;
    wxPoint ViewStart = m_pTimeBarFrame->GetItemContainer()->GetViewStart();
    wxRect ScaleRect = GetRect();
    int iCellWidth = m_pTimeBarFrame->GetCellWidth();
    int iTextPositionX = 0;
    int iTextPositionY = ScaleRect.height + SCALETEXTOFFSETY;
    int iScaleLineLength = ScaleRect.height - SCALELINELENGTHOFFSET;
    int nOffset = iCellWidth * 0.5f;
    ScaleDC.SetPen(*wxWHITE_PEN);
    ScaleDC.DrawRectangle(ScaleRect);
    ScaleDC.SetPen(*wxBLACK_PEN);
    ScaleDC.SetFont(*wxSMALL_FONT);
    for (int i = 0; i < iCount; ++i)
    {
        iTextPositionX = i*iCellWidth + nOffset;
        int iLineHeight = iScaleLineLength;
        if ((ViewStart.x + i) % ScaleTextOffset == 0)
        {
            wxString strText = wxString::Format(_T("%d"), i + ViewStart.x);
            ScaleDC.DrawText(strText, iTextPositionX - ScaleDC.GetTextExtent(strText).x * 0.5f, iTextPositionY);
            iLineHeight -= 4;
        }
        ScaleDC.DrawLine(iTextPositionX, ScaleRect.height, iTextPositionX, iLineHeight);
    }
    int iPositionX = m_pTimeBarFrame->GetCursorPos() * iCellWidth - ViewStart.x * iCellWidth + nOffset;
    if (iPositionX >= 0)
    {
        wxPen pen = m_pTimeBarFrame->IsEnableFramePanel() ? *wxRED_PEN : *wxGREY_PEN;
        pen.SetWidth(CURSORWIDTH);
        ScaleDC.SetPen(pen);
        ScaleDC.DrawLine(iPositionX, ScaleRect.height, iPositionX, iScaleLineLength - 4);
    }
}

void CTimeBarScale::OnMouse( wxMouseEvent& event )
{
    if (m_pTimeBarFrame->IsEnableFramePanel())
    {
        CTimeBarItemContainer* pContiner = m_pTimeBarFrame->GetItemContainer();
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
            m_pTimeBarFrame->SetCursorPos(iPositionX);
            m_pTimeBarFrame->RefreshControl();
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
            wxPoint pstn = event.GetPosition();
            {
                int iScale = PointToScale(pstn);
                int iPositionX = iScale + pContiner->GetViewStart().x;
                m_pTimeBarFrame->SetCursorPos(iPositionX);
                m_pTimeBarFrame->RefreshControl();
            }
        }
    }
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

void CTimeBarScale::OnMouseCaptureLost(wxMouseCaptureLostEvent& /*event*/)
{
    //don't remove this or here cause a warning
}
