#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_EDITCAMERAWND_H_INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_EDITCAMERAWND_H_INCLUDE

class wxPropertyGrid;
class wxPropertyGridEvent;
class wxPGProperty;
class CEditCameraWnd : public wxFrame
{
    typedef wxFrame super;
public:
    CEditCameraWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name = wxDialogNameStr);
    virtual ~CEditCameraWnd();

    void UpdateInfo(bool bForceUpdate = false);
    virtual bool Show(bool bShow = true) override;

private:
    void OnClose(wxCloseEvent& event);
    void OnPropertyChanged(wxPropertyGridEvent& event);
    void OnSaveAsSceneInitClicked(wxCommandEvent& event);
    void OnLoadSceneInitClicked(wxCommandEvent& event);
    void AddSpinCtrl(wxPGProperty* pProperty);

private:
    wxPropertyGrid* m_pPropertyGrid;
    wxPGProperty* m_pPosX;
    wxPGProperty* m_pPosY;
    wxPGProperty* m_pPosZ;
    wxPGProperty* m_pRotationX;
    wxPGProperty* m_pRotationY;
    wxPGProperty* m_pRotationZ;
    wxPGProperty* m_pClipNear;
    wxPGProperty* m_pClipFar;
    wxPGProperty* m_pSpeed;
    wxPGProperty* m_pFov;
    wxButton* m_pSaveAsSceneInitBtn;
    wxButton* m_pLoadSceneInitBtn;
};
#endif