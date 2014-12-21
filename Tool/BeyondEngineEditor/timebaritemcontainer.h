#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMELINEBARCONTAINER_H_INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMELINEBARCONTAINER_H_INCLUDE

#include "wx/wx.h"

class CTimeBarFrame;
class CTimeBarScale;
class CTimeBarFrameData;

struct SSelection
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
    void                SyncWith(wxWindow* pDataViewCtrl, CTimeBarScale* pScaleBarCtrl);
    void                AddItem(CTimeBarFrameData* pitem);
    void                RemoveItem(int index);
    void                SetCellWidth(int width);
    void                SetCellHeight(int height);
    void                SetTimeBarFrameWindow(CTimeBarFrame* pSplitterWindow);
    int                 GetCellWidth() const;
    int                 GetItemCount();
    int                 GetItemIndex(CTimeBarFrameData* pItem);
    SSelection&        GetCurrentSelect();
    CTimeBarFrameData*  GetItem(int nIndex);
    CTimeBarFrame*      GetTimeBarFrameWindow();
    void ResetScrollBar();
    void Clear();
    void AddKey(CTimeBarFrameData* pData, int nFrameIndex);
    void RemoveKey(CTimeBarFrameData* pData, int nFrameIndex);

private:
    void                OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
    void                OnMouse(wxMouseEvent& event);
    void                OnPaint(wxPaintEvent& event);
    void                DrawItem(wxDC& ScaleDC);
    void                DrawSelection(wxDC& ScaleDC);
    int                         m_nDraggingFrameIndex;
    int                         m_iCellWidth;
    int                         m_iCellHeight;
    int                         m_iCursorPositionX;
    wxBoxSizer*                 m_pMainSizer;
    CTimeBarScale*              m_pSyncScale;
    CTimeBarFrame*              m_pTimeBarFrame;
    std::vector<CTimeBarFrameData*>  m_items;
    SSelection                 m_currentSelect;
    SSelection                 m_lastSelect;

    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(CTimeBarItemContainer);
};


#endif