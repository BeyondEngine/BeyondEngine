#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_DATAVIEW_DATAVIEWCHECKRENDERER_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_DATAVIEW_DATAVIEWCHECKRENDERER_H__INCLUDE

#include "wx/wx.h"
#include <wx/dataview.h>

class CDataViewCheckRenderer: public wxDataViewToggleRenderer
{
public:
    CDataViewCheckRenderer(int type);
    virtual ~CDataViewCheckRenderer();

    virtual bool Render( wxRect rect, wxDC *dc, int state );
    virtual bool WXActivateCell(const wxRect& WXUNUSED(cell),
                                wxDataViewModel *model,
                                const wxDataViewItem &item,
                                unsigned int col,
                                const wxMouseEvent *mouseEvent);

    bool SetValue( const wxVariant &value );

private:
    int         m_nType;
    bool        m_bValue;
    wxRect      m_iconRect;
};
#endif