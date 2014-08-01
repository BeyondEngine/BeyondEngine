#ifndef __TIMELINE
#define __TIMELINE

#include "wx/dataview.h"
#include "timebarevent.h"
#include <wx/dynarray.h>

#define CELLWIDTH 10
#define CELLHIGHT 23
#define SCROLLUNITSX 80
#define SCROLLUNITSY 4
#define INITCELLNUM 300
#define SCALEBARHIGHT 24
#define SCALECELLWIDTH 10
#define CURSORWIDTH 2
#define DEFAULTSCROLLBARWIDTH 17

struct TimeLineBarItemAnimation
{
    int     m_iStartFrame;
    int     m_iEndFrame;
    int     m_iFrameID;
};

WX_DECLARE_OBJARRAY(TimeLineBarItemAnimation, ArrayOfAnimation);

class TimeBarListModel;
class TimeBarDataViewCtrl : public wxDataViewCtrl
{
    typedef wxDataViewCtrl super;
public:
    TimeBarDataViewCtrl( wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxDataViewCtrlNameStr );

    void            SyncWith(wxScrollHelperBase *win);
    int             GetRow(const wxDataViewItem & item);
    void            AddItem( const wxString &text, bool visible, bool islock );
    bool            InitCtrl();
    void            RefreshSize();
    TimeBarListModel*    GetUserModel()const;

protected:
    void OnScroll(wxScrollWinEvent& event);

private:
    wxScrollHelperBase*             m_pSyncWnd;
    wxObjectDataPtr<TimeBarListModel>    m_list_model;
    wxString                        m_ItemName;

    DECLARE_EVENT_TABLE()
};

#endif



