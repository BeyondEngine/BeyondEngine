#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMELINEBARCONTAINER_H_INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMELINEBARCONTAINER_H_INCLUDE

#include "wx/wx.h"
#include "timebaritem.h"

class CDataViewCtrl;
class CTimeBarFrame;
class CTimeBarScale;
class CTimeBarItem;

struct SSelectCtrl
{
    int m_iSelectRowBegin;
    int m_iSelectRowEnd;
    int m_iSelectColumnBegin;
    int m_iSelectColumnEnd;
};

class CTimeBarItemContainer: public wxScrolledWindow
{
    typedef wxScrolledWindow super;
public:
    CTimeBarItemContainer(wxWindow* parent);
    virtual ~CTimeBarItemContainer();

    virtual void        ScrollWindow(int x, int y, const wxRect *rect = NULL);
    void                SyncWith(CDataViewCtrl* pDataViewCtrl, CTimeBarScale* pScaleBarCtrl);
    void                AddItem(CTimeBarItem* pitem);
    void                RefreshItems();
    void                SelectItems();
    void                DeleteItem(int index);
    void                SetCellWidth(int width);
    void                SetTimeBarFrameWindow(CTimeBarFrame* pSplitterWindow);
    int                 GetCellWidth() const;
    int                 GetItemCount();
    SSelectCtrl&        GetCurrentSelect();
    CTimeBarItem*       GetItem(int nIndex);
    CTimeBarFrame*      GetTimeBarFrameWindow();
    void ResetScrollBar();

private:
    int                         m_iCellWidth;
    int                         m_iCursorPositionX;
    wxBoxSizer*                 m_pMainSizer;
    CDataViewCtrl*              m_pSyncWnd;
    CTimeBarScale*              m_pSyncScale;
    CTimeBarFrame*              m_pTimeBarFrame;
    std::vector<CTimeBarItem*>  m_items;
    SSelectCtrl                 m_currentSelect;
    SSelectCtrl                 m_lastSelect;

    wxDECLARE_NO_COPY_CLASS(CTimeBarItemContainer);
};


#endif