#ifndef __MYLISTMODEL_H
#define __MYLISTMODEL_H

#include <wx/dynarray.h>
#include <wx/dataview.h>

enum
{
    Col_EditableText,
    Col_Visible,
    Col_Lock,
    Col_Max
};

WX_DEFINE_ARRAY_CHAR(char, wxArrayChar);
class TimeBarListModel: public wxDataViewVirtualListModel
{
public:
    TimeBarListModel();
    virtual ~TimeBarListModel();

    // helper methods to change the model
    void DeleteItem( unsigned int index );
    void DeleteItem( wxDataViewItem& index );
    void DeleteItems( const wxDataViewItemArray &items );
    void AddItem( const wxString text, bool visible, bool islock );

    // implementation of base class virtuals to define model
    virtual unsigned int    GetColumnCount() const;
    virtual wxString        GetColumnType( unsigned int col ) const;
    virtual void            GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const;
    virtual bool            SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col );

private:

    wxArrayString    m_textColValues;
    wxArrayString    m_iconColValues;
    wxArrayChar      m_bView;
    wxArrayChar      m_bLock;
    wxBitmap         m_BmpV;
    wxBitmap         m_BmpL;
};


#endif