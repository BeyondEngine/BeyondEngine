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

    void DrawBackGround(wxDC* pDc);
    void SetBmp(wxBitmap& colorBmp, wxBitmap& alphaBmp);
    wxColor GetColor(int nPositionX, ECursorType eType);
    wxColor GetColorWithAlpha(wxPoint point);
    const wxImage& GetImage() const;

protected:
    void OnPaint(wxPaintEvent& event);

private:
    wxBitmap m_foreGroundBmp;
    wxBitmap m_alphaBmp;
    wxImage m_mergeImage;
    DECLARE_EVENT_TABLE()
};
#endif