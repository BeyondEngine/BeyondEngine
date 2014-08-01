#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GRADIENT_GRADIENTCOLORBOARD_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GRADIENT_GRADIENTCOLORBOARD_H__INCLUDE

#include "wx/panel.h"
#include "GradientCursor.h"

class CGradientColorBoard : public wxPanel
{
    typedef wxPanel super;
public:
    CGradientColorBoard(wxWindow *parent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("BeyondEngineEditorGLWindow"));
    virtual ~CGradientColorBoard();

    void InitCtrl();
    void DrawBackGround(wxDC* pDc);
    void SetBmp(wxBitmap& colorBmp, wxBitmap& maskBmp);
    wxColor GetColor(int nPositionX, ECursorType eType);
    wxColor GetColorWithAlpha(wxPoint point);
    wxBitmap& GetForeGroundBmp();

protected:
    void OnPaint(wxPaintEvent& event);

private:
    wxBitmap m_drawBmp;
    wxBitmap m_foreGroundBmp;
    wxBitmap m_maskBmp;
    DECLARE_EVENT_TABLE()
};
#endif