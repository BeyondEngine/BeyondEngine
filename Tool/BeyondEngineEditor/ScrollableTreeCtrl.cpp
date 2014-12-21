#include "stdafx.h"
#include "ScrollableTreeCtrl.h"
#include "wx/msw/private.h"

BEGIN_EVENT_TABLE(CScrollableTreeCtrl, wxTreeCtrl)
EVT_SCROLLWIN(CScrollableTreeCtrl::OnScroll)
EVT_MOUSEWHEEL(CScrollableTreeCtrl::OnMouseWheel)
END_EVENT_TABLE()

CScrollableTreeCtrl::CScrollableTreeCtrl(wxWindow *parent, wxWindowID id)
: wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxTR_EDIT_LABELS | wxTR_FULL_ROW_HIGHLIGHT | wxTR_TWIST_BUTTONS )
, m_pSyncWindow(NULL)
{

}

CScrollableTreeCtrl::~CScrollableTreeCtrl()
{

}

void CScrollableTreeCtrl::SetSyncScrollWindow(wxScrolledWindow* pWindow)
{
    m_pSyncWindow = pWindow;
}

void CScrollableTreeCtrl::OnScroll(wxScrollWinEvent& event)
{
    wxEventType eType = event.GetEventType();
    if (eType != wxEVT_SCROLLWIN_THUMBRELEASE)
    {
        int nAdjust = eType == wxEVT_SCROLLWIN_LINEUP ? 1 : -1;
        int nPos = eType == wxEVT_SCROLLWIN_THUMBTRACK
            ? event.GetPosition()
            : GetScrollPos(wxVERTICAL) - nAdjust;
        HWND hWnd = GetHwnd();
        WinStruct<SCROLLINFO> info;
        info.fMask = SIF_PAGE;
        ::GetScrollInfo(hWnd, SB_VERT, &info);
        if (nPos >= 0 && nPos <= GetScrollRange(wxVERTICAL) - (int)info.nPage)
        {
            wxPoint point = m_pSyncWindow->GetViewStart();
            point.y = nPos;
            m_pSyncWindow->Scroll(point);
        }
    }
    event.Skip();
}

void CScrollableTreeCtrl::OnMouseWheel(wxMouseEvent& event)
{
    ScrollLines(event.GetWheelRotation() / -120 * event.GetLinesPerAction());
}

void CScrollableTreeCtrl::GetItemsRecursively(std::vector<wxTreeItemId>& list, const wxTreeItemId& idParent, wxTreeItemIdValue cookie)
{
    wxTreeItemId id = !cookie ? GetFirstChild(idParent, cookie) : GetNextChild(idParent, cookie);

    if (id.IsOk())
    {
        list.push_back(id);
        if (ItemHasChildren(id))
        {
            if (IsExpanded(id))
            {
                GetItemsRecursively(list, id);
            }
        }
        GetItemsRecursively(list, idParent, cookie);
    }
}