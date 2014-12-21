#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMEBARFRAME_H_INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMEBARFRAME_H_INCLUDE

#include "wx/splitter.h"
#include "wx/scrolwin.h"
#include "timebaritemcontainer.h"
#include "timebarscale.h"
#include "wx/button.h"
#include <wx/spinctrl.h>
#include "ScrollableTreeCtrl.h"

#define FREQUENCYREDUCTIONFACTOR 4
#define DIFFERENTDRAWLENGTH 5
#define CURSORWIDTH 2

class CTimeBarItemContainer;
class CTimeBarFrameData;
class CScrollableTreeCtrl;

class CTimeBarFrame : public wxSplitterWindow
{
public:
    CTimeBarFrame(wxWindow *parent);
    virtual ~CTimeBarFrame();

    void            Clear();
    void            ResetLeft();
    CTimeBarFrameData* AddItem(wxString strName, std::vector<int>& frameList, CTimeBarFrameData* pParent = NULL);
    void            DeleteItem(CTimeBarFrameData* pData);
    void            RefreshControl();
    void            SetCursorPos(int pos, bool bRefresh = true, bool bSendEvent = true);
    int             GetCursorPos();
    int             GetCursorIndex(int pos);
    void            SetCurrentCursor(int pos);
    void            SetClickOnScalebar(bool bIsClicked);
    bool            IsClickOnScalebar() const;
    void            SetFrameWindow(wxWindow* pWindow);
    int             GetCellWidth();
    void            EnableDrag(bool bEnable = true);
    void            ShowChoice(bool bShow = true);
    void            SetFps(int nFps);
    wxString        GetChoiceString();
    void            AddChoiceString(const wxString& strName);
    wxChoice*       GetChoice();
    CTimeBarFrameData*       GetSelectedItem();
    CTimeBarFrameData*       GetBeginDragItem();
    CTimeBarFrameData*       GetEndDragItem();
    void            RClickOnFrame(int nFrameIndex);
    void            SelectItem(CTimeBarFrameData* pItem);
    wxWindow*       GetFrameWindow();
    CTimeBarScale*  GetScalebar();
    CTimeBarItemContainer*   GetItemContainer();
    CScrollableTreeCtrl*     GetItemTreeCtrl();
    uint32_t          GetFrameCountForDraw() const;
    void UpdateFrameContainer();
    void Focuse() const;
    void Play();
    void EnableFramePanel(bool bEnable);
    bool IsEnableFramePanel() const;
protected:
    bool IsParentOfItem(wxTreeItemId& parentItemId, wxTreeItemId& itemId);
    void OnTreeItemRClick(wxTreeEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnChoice(wxCommandEvent& event);
    void OnSelect(wxTreeEvent& event);
    void OnExpanded(wxTreeEvent& event);
    void OnCollapsed(wxTreeEvent& event);
    void OnTreeItemStartDrag(wxTreeEvent& event);
    void OnTreeItemEndDrag(wxTreeEvent& event);

    void OnAddBtnClicked(wxCommandEvent& event);
    void OnMinusBtnClicked(wxCommandEvent& event);
    void OnFpsSettingBtnClicked(wxCommandEvent& event);
    void OnPlayBtnClicked(wxCommandEvent& event);
    void OnPlayTypeBtnClicked(wxCommandEvent& event);
    void Update();
private:
    uint32_t                m_uMaxFrameIndex;
    wxLongLong              m_nBeginTime;
    long                    m_nPlayType;
    int                     m_nIntervalMS;
    int                     m_nFps;
    int                     m_iCellWidth;
    int                     m_iCellHeight;
    int                     m_iItemId;
    int                     m_nCursorPos;
    bool                    m_bQuit;
    bool                    m_bPlaying;
    bool                    m_bEnableFramePanel;
    bool                    m_bclickonscalebar;
    bool                    m_bEnableDrag;
    CTimeBarFrameData*      m_pBeginDragData;
    CTimeBarFrameData*      m_pEndDragData;
    CScrollableTreeCtrl*    m_pItemTreeCtrl;
    CTimeBarItemContainer*  m_pFrameContainer;
    CTimeBarScale*          m_pScalebar;
    wxChoice*               m_pChoice;
    wxButton*               m_pButtonAdd;
    wxButton*               m_pButtonDelete;
    wxButton*               m_pFPSSettingBtn;
    wxButton*               m_pPlayBtn;
    wxButton*               m_pPlayTypeBtn;
    wxWindow*               m_pFrame;
    DECLARE_EVENT_TABLE()
};
wxDECLARE_EVENT(TIMTBAR_SELECTITEM_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TIMTBAR_DRAGITEMBEGIN_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TIMTBAR_DRAGITEMEND_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TIMTBAR_CHOICE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TIMTBAR_ITEMCONTAINERRCLICK_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TIMTBAR_ADDBUTTONCLICK_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TIMTBAR_MINUSBUTTONCLICK_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TIMTBAR_CURSORCHANGE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TIMTBAR_SELECTFRAMECHANGE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(TIMTBAR_DRAGGINGFRAME_EVENT, wxCommandEvent);
#endif