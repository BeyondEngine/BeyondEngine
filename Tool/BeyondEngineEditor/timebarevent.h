#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_USERDEFINEEVENT_TIMEBAREVENT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_USERDEFINEEVENT_TIMEBAREVENT_H__INCLUDE

#include "wx/event.h"
#include <wx/dataview.h>

class CTimeBarEvent : public wxCommandEvent
{
public:
    CTimeBarEvent(wxEventType cmdType = wxEVT_NULL,int id = 0);
    CTimeBarEvent(const CTimeBarEvent& event);
    ~CTimeBarEvent();

    virtual wxEvent* Clone() const;

public:
    void    SetRow(const int msg);
    int     GetRow() const;

    void    SetItem(wxDataViewItem& msg);
    wxDataViewItem& GetItem();

    void    SetType(const int msg);
    int     GetType() const;

    void    SetStrMsg(const wxString& msg);
    const wxString& GetStrMsg() const ;

protected:
    int         m_nIndex;
    int         m_nType;
    wxDataViewItem m_item;
    wxString    m_strStringMsg;
private:
    DECLARE_DYNAMIC_CLASS(CTimeBarEvent)
};


#define ID_CUSTOM_EVENT_TYPE (wxID_HIGHEST + 1)

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(TIMEBAR_EVENT_TYPE, ID_CUSTOM_EVENT_TYPE)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*TimeBarEventFunction)(CTimeBarEvent&);

#define EVT_DATAVIEW_CUSTRENDER_ACTIVE(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
    TIMEBAR_EVENT_TYPE, id, -1, \
    (wxObjectEventFunction)(wxEventFunction)(TimeBarEventFunction)&fn, \
    (wxObject*)NULL),

#endif