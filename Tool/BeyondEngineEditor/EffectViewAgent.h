#ifndef BEYOND_ENGINE_EDITOR_VIEWAGENT_EFFECTVIEWAGENT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_VIEWAGENT_EFFECTVIEWAGENT_H__INCLUDE

#include "ViewAgentBase.h"
#include <wx/treectrl.h>

class CEditorMainFrame;
class wxAuiNotebook;
class CDependencyDescription;

class CEffectViewAgent : public CViewAgentBase
{
    BEATS_DECLARE_SINGLETON(CEffectViewAgent);
public:
    virtual void InView() override;
    virtual void OutView() override;
    virtual void CreateTools() override;
    virtual void ProcessMouseEvent(wxMouseEvent& event) override;
    virtual void OnOpenComponentFile() override;
    virtual void OnCloseComponentFile() override;
    virtual void OnTreeCtrlSelect(wxTreeEvent& event) override;
    virtual void OnEditTreeItemLabel(wxTreeEvent& event) override;
    virtual void OnUpdateComponentInstance() override;
    virtual void SelectComponent(CComponentProxy* pComponentInstance) override;
    virtual void OnCommandEvent(wxCommandEvent& event) override;
    virtual void OnTreeCtrlRightClick(wxTreeEvent& event) override;
    virtual void OnPropertyChanged(wxPropertyGridEvent& event) override;
    virtual void OnEditDataViewItem(wxDataViewEvent& event) override;

    void InitializeCtrls();
    void UpdateCtrl();
    void OnBeginButton();
    void UpdateEffectTree();
    void UpdateTimeBar();
    void DeleteComponent(CComponentProxy* pProxy);
    void DeleteSelectedParticleSystem();
    void DeleteSelectedParticleEmitter();
    CComponentProxy* CreateNewParticleSystem();
    CComponentProxy* CreateNewParticleEmitter();
    CDependencyDescription* GetDependency(CComponentProxy* pProxy, wxString dependencyName);

private:
    int                 m_nPatricleSystemGuid;
    int                 m_nPatricleEmitterGuid;
    int                 m_n3DNodeGuid;
    wxPanel*            m_pEffectsPanel;
    wxAuiNotebook*      m_pEffectsBook;
    wxMenu* m_pEffectTreeMenu;
    wxButton* m_pEmitButton;
    wxTreeCtrl* m_pEffectTreeCtrl;
};
#endif