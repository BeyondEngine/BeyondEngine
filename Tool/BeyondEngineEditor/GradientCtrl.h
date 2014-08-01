#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_GRADIENT_GRADIENTCTRL_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_GRADIENT_GRADIENTCTRL_H__INCLUDE

#include "wx/panel.h"
#include "GradientCursor.h"
#include <wx/clrpicker.h>
#include "wx/slider.h"

class CGradientColorBoard;
class CGradientCtrl : public wxPanel
{
    typedef wxPanel super;
public:
    CGradientCtrl(wxWindow *pParent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT(""));
    virtual ~CGradientCtrl();

    void InitCtrl();
    void AddCursor(CGradientCursor* pCursor);
    void DeleteCursor(CGradientCursor* pCursor);
    void DrawColorBoard();
    void DrawCursorDataToBmp(std::vector<CGradientCursor*>& list, wxBitmap& bmp);
    void DrawCursorIcons();
    void SetSelectedCursorPos(wxPoint point);
    void ShowCtrl(ECursorType eType);
    void UpdateSelectedCursorPos();
    float GetNearestCursorPos(CGradientCursor* pCursor);
    float CalCursorPos(wxPoint point);
    wxColor GetColorByPos(float fPos);
    std::vector<CGradientCursor*>& GetColorList();
    std::vector<CGradientCursor*>& GetMaskList();

protected:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouse(wxMouseEvent& event);
    void OnChooseColor(wxColourPickerEvent& event);
    void OnSliderScroll(wxScrollEvent& event);
    void OnInputPos(wxCommandEvent& event);
    void OnSetPos(wxCommandEvent& event);

    CGradientCursor* GetGradientCursor(wxPoint point);

private:
    wxRect m_colorIconRect;
    wxRect m_maskIconRect;
    wxPanel* m_pCtrlPanel;
    wxSlider* m_pMaskSlider;
    wxTextCtrl* m_pPosEdit;
    wxButton* m_pSetPosButton;
    wxColourPickerCtrl* m_pColoutPicker;
    CGradientCursor* m_pSelectedCursor;
    CGradientColorBoard* m_pGradientColorBoard;
    std::vector<CGradientCursor*> m_colorCursorList;
    std::vector<CGradientCursor*> m_maskCursorList;

    DECLARE_EVENT_TABLE()
};
#endif