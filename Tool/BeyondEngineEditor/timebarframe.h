#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMEBARFRAME_H_INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMEBARFRAME_H_INCLUDE

#include "wx/splitter.h"
#include "wx/scrolwin.h"
#include "timebaritemcontainer.h"
#include "timebarscale.h"
#include "wx/button.h"
#include "DataViewCtrl.h"
#include <wx/spinctrl.h>

#define FREQUENCYREDUCTIONFACTOR 4
#define DIFFERENTDRAWLENGTH 5
    
class CDataViewCtrl;
class CTimeBarItemContainer;
class CNodeAnimationData;
class CNodeAnimation;
class CTimeBarFrame : public wxSplitterWindow
{
public:
    CTimeBarFrame(wxWindow *parent);
    virtual ~CTimeBarFrame();

    void            Clear();
    void            ResetLeft();
    CTimeBarItem*   AddItem(wxString name, wxUIntPtr userData, int nStartPos = 0, int nLength = 0, bool bVisible = true, bool bEditable = true);
    void            DeleteItem(int index);
    void            SetItemDataRange(int nDataIndex, int nStart, int nEnd);
    void            ClickOnScaleBar();
    void            SetCursorPositionX(int pos);
    int             GetCursorPositionX();
    int             GetCursorIndex(int pos);
    void            SetCurrentCursor(int pos);
    void            SetSelectedRow(int nRowIndex);
    int             GetSelectedRow() const;
    void            SetClickOnScalebar(bool bIsClicked);
    bool            IsClickOnScalebar() const;
    bool            IsNameValid(wxString name);
    void            SetFrameWindow(wxWindow* pWindow);
    int             GetItemCount();
    int             GetCellWidth();
    void            SetCtrlsId(int nIdButtonAdd, int nIdButtonDelete);
    wxString        GetItemLabel(wxDataViewItem& item);
    wxUIntPtr       GetItemDataByRow(int nRow);
    wxUIntPtr       GetItemData(wxDataViewItem& item);
    wxWindow*       GetFrameWindow();
    CDataViewCtrl*  GetElementList();
    CTimeBarScale*  GetScalebar();
    CTimeBarItemContainer*   GetItemContainer();
    size_t          GetFrameCount() const;
    void SetAnimationData(CNodeAnimationData* pData);
    CNodeAnimationData* GetAnimationData() const;
    CNodeAnimation* GetAnimation() const;

protected:
    void OnSize(wxSizeEvent& event);
    void OnPositionChanged(wxSplitterEvent& event);
    void OnPositionChanging(wxSplitterEvent& event);
    void OnSelect(wxDataViewEvent& event);

    void OnFpsSettingBtnClicked(wxCommandEvent& event);
    void OnPlayBtnClicked(wxCommandEvent& event);
    void OnPlayTypeBtnClicked(wxCommandEvent& event);
private:
    int                     m_iCellWidth;
    int                     m_iItemId;
    int                     m_iCursorPositionX;
    bool                    m_bclickonscalebar;
    CDataViewCtrl*          m_pElementList;
    CTimeBarItemContainer*  m_pFrameContainer;
    CTimeBarScale*          m_pScalebar;
    wxButton*               m_pButtonAdd;
    wxButton*               m_pButtonDelete;
    wxButton*               m_pFPSSettingBtn;
    wxButton*               m_pPlayBtn;
    wxButton*               m_pPlayTypeBtn;
    wxWindow*               m_pFrame;
    CNodeAnimationData*     m_pAnimationData;
    CNodeAnimation* m_pAnimation;
    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(CTimeBarFrame);
};

#endif