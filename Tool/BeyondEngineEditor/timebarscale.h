#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMELINEBARSCALE_H_INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMELINEBARSCALE_H_INCLUDE

#include "wx/scrolwin.h"

class CTimeBarFrame;
class CTimeBarScale : public wxPanel
{
public:
    CTimeBarScale( wxWindow *parent );
    
    void                DrawScale();
    int                 PointToScale(wxPoint point);
    wxPoint             ScaleToPoint(int iScale);
    void                SetScaleCount(int iScaleCount);
    int                 GetScaleCount();
    void                SetTimeBarFrameWindow(CTimeBarFrame* pSplitterWindow);
    CTimeBarFrame*      GetTimeBarFrameWindow();

protected:
    void                OnPaint( wxPaintEvent& event );
    void                OnMouse( wxMouseEvent& event );
    void                OnMouseCaptureLost(wxMouseCaptureLostEvent& event);

private:
    int     m_iCursorPositionX = 0;
    CTimeBarFrame* m_pTimeBarFrame = nullptr;
    DECLARE_EVENT_TABLE()
};



#endif