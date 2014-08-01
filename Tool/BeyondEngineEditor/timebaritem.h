#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMELINEBARITEM_H_INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TIMEFRAMEBAR_TIMELINEBARITEM_H_INCLUDE

#define CELLWIDTH 10

#include "wx/panel.h"
class CNodeAnimationElement;
class wxPopupTransientWindow;
class CTimeBarItem : public wxPanel
{
    typedef wxPanel super;
public:
    enum EItemColor
    {
        eIC_BACKGROUNDCOLOUR1,
        eIC_BACKGROUNDCOLOUR2,
        eIC_BORDERCOLOUR,
        eIC_DATACELLCOLOUR,
        eIC_SELECTEDCOLOUR
    };
    CTimeBarItem( wxWindow *parent, wxWindowID id=wxID_ANY, 
        const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
        long style= wxTAB_TRAVERSAL, const wxString &name=wxPanelNameStr );
    ~CTimeBarItem();
    void        UnSelecteAll();
    void        SelectCell( int index, bool bselect );
    void        SelectCells(int start, int end = -1);
    int         PointToCell(int pos);
    wxPoint     CellToPoint(int index);
    void        OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
    void        OnPaint( wxPaintEvent& event );
    void        OnMouse(wxMouseEvent& event);
    int         GetRowID();
    void        DrawDefaultItem(wxDC* pDC, int iIndex, int iPositionX, int iWidth, int iHeight);
    void        SetCellsDataRange(int iDataBegin, int iDataEnd);
    wxColour    GetColour(int ColourID);
    bool        HasSelected(int iFrameIndex);
    void        AddData(size_t uFrame, const CVec3& data);
    void        RemoveData(size_t uFrame);
    bool        GetData(size_t uFrame, CVec3& data);
    void        SetAnimationElement(CNodeAnimationElement* pElement);
    CNodeAnimationElement* GetAnimationElement() const;
private:
    int         m_iSelectionBegin;
    int         m_iSelectionEnd;
    int         m_iDataBegin;
    int         m_iDataEnd;
    CNodeAnimationElement* m_pElement;
    wxPopupTransientWindow* m_pPopUpWindow;
    wxStaticText* m_pPopUpText;
    std::map<size_t, CVec3> m_dataMap;
    DECLARE_EVENT_TABLE()
};
WX_DECLARE_OBJARRAY(CTimeBarItem*, ArrayOfTimeBarItem);

#endif