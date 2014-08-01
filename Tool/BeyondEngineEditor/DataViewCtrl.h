#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_DATAVIEW_DATAVIEWCTRL_H_INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_DATAVIEW_DATAVIEWCTRL_H_INCLUDE

#include "wx/dataview.h"
#include "timebarevent.h"
#include <wx/dynarray.h>

#define CELLWIDTH 10
#define CELLHIGHT 23
#define SCROLLUNITSX 80
#define SCROLLUNITSY 4
#define SCALEBARHIGHT 24
#define SCALECELLWIDTH 10
#define CURSORWIDTH 2
#define DEFAULTSCROLLBARWIDTH 17

struct STimeLineBarItemAnimation
{
    int     m_iStartFrame;
    int     m_iEndFrame;
    int     m_iFrameID;
};

WX_DECLARE_OBJARRAY(STimeLineBarItemAnimation, ArrayOfAnimation);

class CDataViewListModel;
class CDataViewTreeModelNode;
class CDataViewCtrl : public wxDataViewListCtrl
{
    typedef wxDataViewListCtrl super;
public:
    CDataViewCtrl( wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxValidator& validator = wxDefaultValidator);
    virtual ~CDataViewCtrl();

    void            SyncWith( wxScrollHelperBase *win );
    int             GetRow( const wxDataViewItem & item );
    void            AddItem( const wxString &text, bool visible, bool islock, wxUIntPtr userData );
    void*           GetItemDataByRow(size_t uRow);

    bool            InitColumn();
    void            RefreshSize();
    void            Clear();
    void            AssociateListModel();
    CDataViewListModel*    GetUserListModel()const;

protected:
    void OnScroll(wxScrollWinEvent& event);
    void OnMouseWheel(wxMouseEvent& event);

private:
    wxString            m_strItemName;
    wxScrollHelperBase* m_pSyncWnd;
    CDataViewListModel*  m_pListModel;

    DECLARE_EVENT_TABLE()
};

#endif



