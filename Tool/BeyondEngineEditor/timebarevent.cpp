#include "stdafx.h"
#include "timebarevent.h"

DEFINE_EVENT_TYPE(TIMEBAR_EVENT_TYPE)
IMPLEMENT_DYNAMIC_CLASS(CTimeBarEvent, wxCommandEvent)

CTimeBarEvent::CTimeBarEvent(wxEventType cmdType ,int id)
:wxCommandEvent(cmdType, id)
{
    m_nIndex = 0;
    m_nType = 0;
}

CTimeBarEvent::CTimeBarEvent(const CTimeBarEvent& event)
:wxCommandEvent(event)
{

}

CTimeBarEvent::~CTimeBarEvent()
{

}

wxEvent* CTimeBarEvent::Clone() const 
{
    return new CTimeBarEvent(*this);
}

void CTimeBarEvent::SetRow(const int msg) 
{ 
    m_nIndex = msg;
}

int CTimeBarEvent::GetRow() const 
{
    return m_nIndex;
}

void CTimeBarEvent::SetType(const int msg) 
{
    m_nType = msg;
}

int CTimeBarEvent::GetType() const 
{ 
    return m_nType;
}

void CTimeBarEvent::SetStrMsg(const wxString& msg) 
{ 
    m_strStringMsg = msg;
}

const wxString& CTimeBarEvent::GetStrMsg() const 
{
    return m_strStringMsg;
}

void CTimeBarEvent::SetItem( wxDataViewItem& msg )
{
    m_item = msg;
}

wxDataViewItem& CTimeBarEvent::GetItem()
{
    return m_item;
}
