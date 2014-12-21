#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SETCOORDINATERENDEROBJECTDLG_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SETCOORDINATERENDEROBJECTDLG_H__INCLUDE

class wxPropertyGrid;
class wxPropertyGridEvent;
class wxPGProperty;
class CCoordinateSettingWnd : public wxFrame
{
    typedef wxFrame super;
public:
    CCoordinateSettingWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name = wxDialogNameStr);
    virtual ~CCoordinateSettingWnd();
    virtual bool Show(bool bShow = true) override;
    uint32_t GetColorValue(wxPGProperty* property);

private:
    void OnClose(wxCloseEvent& event);
    void OnPropertyChanged(wxPropertyGridEvent& event);
    void AddSpinCtrl(wxPGProperty* pProperty, float fStep = 0.1f);

private:
    wxPropertyGrid* m_pPropertyGrid;
    wxPGProperty* m_pVisibleProperty;
    wxPGProperty* m_axisColorX;
    wxPGProperty* m_axisColorY;
    wxPGProperty* m_axisColorZ;
    wxPGProperty* m_axisSelectColor;
    wxPGProperty* m_panelSelectColor;
    wxPGProperty* m_fAxisLengthFactor;
    wxPGProperty* m_fConeHeightFactor;
    wxPGProperty* m_fConeBottomColorFactor;
    wxPGProperty* m_fConeAngle;
    wxPGProperty* m_fTranslatePanelStartPosRate;
};
#endif