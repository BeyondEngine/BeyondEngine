#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_PARTICLECONTROLWND_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_PARTICLECONTROLWND_H__INCLUDE

class CParticleEmitter;
class CParticleControlWnd : public wxFrame
{
    typedef wxFrame super;
public:
    CParticleControlWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name = wxDialogNameStr);
    virtual ~CParticleControlWnd();

    void AttachEmitter(CParticleEmitter* pEmitter);
    virtual bool Show(bool bShow = true) override;
    void Update(float dtt);
private:
    void OnStopBtnClicked(wxCommandEvent& event);
    void OnPlayBtnClicked(wxCommandEvent& event);

private:
    CParticleEmitter* m_pAttachedEmitter = nullptr;
    wxTextCtrl* m_pPlaybackSpeedTextCtrl = nullptr;
    wxTextCtrl* m_pPlaybackTimeTextCtrl = nullptr;
    wxButton* m_pPlayBtn = nullptr;
};


#endif