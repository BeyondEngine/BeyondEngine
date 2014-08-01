#include "wx/wx.h"
#include <wx/dataview.h>

class TimeBarCheckRenderer: public wxDataViewCustomRenderer
{
public:
    TimeBarCheckRenderer(int type);
    virtual ~TimeBarCheckRenderer();

    virtual bool Render( wxRect rect, wxDC *dc, int state );
    virtual bool ActivateCell(const wxRect& WXUNUSED(cell),
        wxDataViewModel *model,
        const wxDataViewItem &item,
        unsigned int col,
        const wxMouseEvent *mouseEvent);

    virtual wxSize GetSize() const;
    virtual bool SetValue( const wxVariant &value );
    virtual bool GetValue( wxVariant &WXUNUSED(value) ) const;

private:

    int         m_type;
    int         m_iSelectedRow;
    wxRect      m_iconRect;
    wxString    m_value;
};