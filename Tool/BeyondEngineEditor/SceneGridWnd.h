#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SCENEGRIDWND_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SCENEGRIDWND_H__INCLUDE

class wxPropertyGrid;
class wxPropertyGridEvent;
class wxPGProperty;
class CSceneGridWnd : public wxFrame
{
    typedef wxFrame super;
public:
    CSceneGridWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name = wxDialogNameStr);
    virtual ~CSceneGridWnd();
    virtual bool Show(bool bShow = true) override;

private:
    void OnClose(wxCloseEvent& event);
    void OnPropertyChanged(wxPropertyGridEvent& event);
    void AddSpinCtrl(wxPGProperty* pProperty);

private:
    wxPropertyGrid* m_pPropertyGrid;
    wxPGProperty* m_pGridStartPosX;
    wxPGProperty* m_pGridStartPosY;
    wxPGProperty* m_pGridWidth;
    wxPGProperty* m_pGridHeight;
    wxPGProperty* m_pGridDistance;
    wxPGProperty* m_pGridLineWidth;
    wxPGProperty* m_pRenderPositiveDirectionLine;
    wxPGProperty* m_pRenderMapGrid;
    wxPGProperty* m_pRenderPathFindingGrid;
    wxPGProperty* m_pRenderBuilding;

    wxPGProperty* m_pSimulateWidth;
    wxPGProperty* m_pSimulateHeight;
    wxPGProperty* m_pFPS;
};

#endif