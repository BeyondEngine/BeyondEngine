#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_EditAnimationDialog_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_EditAnimationDialog_H__INCLUDE

#include "Render/Animation3D.h"
#include "Render/Skin.h"
#include "Render/Skeleton.h"
#include "Render/Model.h"
#include "EditDialogBase.h"
#include <wx/treectrl.h>
#include <wx/dialog.h>
#include <wx/aui/aui.h>

class CAnimationGLWindow;
class CTimeBarFrame;
class wxTreeCtrl;
class CModel;
class CEditAnimationDialog : public CEditDialogBase
{
    enum
    {
        ID_BUTTON_SELECTANIDIR,
        ID_BUTTON_SELECTSKEDIR,
        ID_BUTTON_SELECTSKIDIR,
        ID_BUTTON_CLEANLOG,
        ID_BUTTON_PLAY,
        ID_BUTTON_STOP,
        ID_BUTTON_PAUSE,
        ID_CHECKBOX_LOOP,
        ID_LISTBOX_ANIMATION,
        ID_LISTBOX_SKELETON,
        ID_LISTBOX_SKIN,
        ID_CHOICE_SKELETON,
        ID_BUTTON_SELECT,
        ID_CHECKBOX_SKELETONVISIBLE,
        ID_CHECKBOX_SKELETONBONEVISIBLE,
        ID_TREECTRL_MODELS,
        ID_MENU_COPY,
        ID_MENU_CREATE,
        ID_MENU_PAST,
        ID_MENU_DELETE
    };
    typedef CEditDialogBase super;
public:
    CEditAnimationDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style =  wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);
    virtual ~CEditAnimationDialog();
    virtual void LanguageSwitch() override;
    virtual int ShowModal() override;
    void InitWindow();
    void CreateNewModel();
    void DeleteModel();
    
    void GetResourceList(wxString path);
    void ClearResourceLoadFlag();
    void ShowAnima();
    void InitTimeBar();
    bool IsLoadAllAnimationResource();
    void SetAllSkeletonAndBoneVisible(bool bSke = false, bool bSkeBone = false);
    void SetSkeletonAndBoneVisible(bool bSke = false, bool bSkeBone = false);
    bool CheckIsNameValid( const TString& strWindowName );
    TString GetAvailableName( const TString& strWindowName );
    void DelListboxSelect();
    void SetLanguage();
    CAnimationGLWindow* GetCanvas() const;
    CTimeBarFrame* GetTimeBar();
    wxTreeItemId AddModelToTree(TString ModelName);
    SharePtr<CAnimation3D> GetAnimation();

protected:
    void DeleteModel(wxTreeItemId& item);
    void SelectModel(wxTreeItemId item);

    void OnPopMenu(wxCommandEvent& event);
    void OnSelectFile(wxCommandEvent& event);
    void OnClearLog(wxCommandEvent& event);
    void OnPlayAnimation(wxCommandEvent& event);
    void OnPauseAnimation(wxCommandEvent& event);
    void OnStopAnimation(wxCommandEvent& event);
    void OnLoopPlayAnimation(wxCommandEvent& event);
    void OnAnimationListBox(wxCommandEvent& event);
    void OnSkeketonListBox(wxCommandEvent& event);
    void OnSkinListBox(wxCommandEvent& event);
    void OnSkeletonChoice(wxCommandEvent& event);
    void OnSelectBoneButton(wxCommandEvent& event);
    void OnSkeletonCheckbox(wxCommandEvent& event);
    void OnSkeletonBoneCheckbox(wxCommandEvent& event);
    void OnModelTreeRightClick(wxTreeEvent& event);
    void OnModelTreeItemChanged(wxTreeEvent& event);

private:

    bool                m_bAnimation;
    bool                m_bSkeleton;
    bool                m_bSkin;
    bool                m_bIsLoop;
    bool                m_bIsVisibleBone;
    bool                m_bIsVisibleCoordinate;
    bool                m_bIsSelectAll;
    bool                m_bISLanguageSwitch;

    wxMenu*             m_pModelTreeMenu;
    wxLog*              m_pLogOld;
    wxButton*           m_pButtonPlay;
    wxButton*           m_pButtonPause;
    wxButton*           m_pButtonStop;
    wxButton*           m_pButtonAnimaton;
    wxButton*           m_pButtonSkeleton;
    wxButton*           m_pButtonSkin;
    wxCheckBox*         m_pAnimationCheckBox;
    wxCheckBox*         m_pSKECheckBox;
    wxCheckBox*         m_pSKEBCheckBox;
    wxCheckBox*         m_pSkeletonCheckBox;
    wxCheckBox*         m_pSkinCheckBox;
    wxCheckBox*         m_pLoopCheckBox;
    wxTextCtrl*         m_pLog;
    wxTreeCtrl*         m_pModelTree;
    wxListBox*          m_pAnimitionListBox;
    wxListBox*          m_pSkeletonListBox;
    wxListBox*          m_pSkinListBox;
    wxChoice*           m_pSkeletonChoice;
    wxButton*           m_pSkeSelectButton;
    CTimeBarFrame*       m_pTimeBar;
    CAnimationGLWindow* m_pAnimationGLWindow;

    std::map<wxTreeItemId, CModel*> m_ModelNameMap;
    SharePtr<CAnimation3D>    m_pAnimation;
    SharePtr<CSkin>         m_pSkin;
    SharePtr<CSkeleton>     m_pSkeleton;
    wxAuiManager            m_Manager;
    std::vector<ESkeletonBoneType> m_skeletonBoneType;
DECLARE_EVENT_TABLE();
};
#endif