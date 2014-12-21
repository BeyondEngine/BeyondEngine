#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_SCROLLABLETREECTRL_H_INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_SCROLLABLETREECTRL_H_INCLUDE

#include <wx/treectrl.h>
class CScrollableTreeCtrl : public wxTreeCtrl
{
public:
    CScrollableTreeCtrl(wxWindow *parent, wxWindowID id = wxID_ANY);
    virtual ~CScrollableTreeCtrl();
    void SetSyncScrollWindow(wxScrolledWindow* pWindow);
    void OnScroll(wxScrollWinEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void GetItemsRecursively(std::vector<wxTreeItemId>& list, const wxTreeItemId& idParent, wxTreeItemIdValue cookie = 0);

private:
    wxScrolledWindow* m_pSyncWindow;
    DECLARE_EVENT_TABLE();
};

#endif