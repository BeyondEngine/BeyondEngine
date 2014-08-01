#include "stdafx.h"
#include "timebarlistmodel.h"

TimeBarListModel::TimeBarListModel()
: wxDataViewVirtualListModel()
{

}

TimeBarListModel::~TimeBarListModel()
{

}

void TimeBarListModel::DeleteItem( unsigned int index )
{
    if (index < m_textColValues.GetCount())
    {
        m_textColValues.RemoveAt( index );
        m_bView.RemoveAt( index );
        m_bLock.RemoveAt( index );
        RowDeleted( index );
    }
}

void TimeBarListModel::DeleteItem( wxDataViewItem& item )
{
    unsigned int row = GetRow( item );
    DeleteItem( row );
}

void TimeBarListModel::DeleteItems( const wxDataViewItemArray &items )
{
    wxArrayInt rows;
    for (unsigned i = 0; i < items.GetCount(); i++)
    {
        unsigned int row = GetRow( items[i] );
        if (row < m_textColValues.GetCount())
            rows.Add( row );
    }

    if (rows.GetCount() > 0)
    {
        // Sort in descending order so that the last
        // row will be deleted first. Otherwise the
        // remaining indeces would all be wrong.
        rows.Sort([](int *v1, int *v2)
        {
            return *v2-*v1;
        });
        for (unsigned i = 0; i < rows.GetCount(); i++)
        {
            m_textColValues.RemoveAt( rows[i] );
            m_bView.RemoveAt( rows[i] );
            m_bLock.RemoveAt( rows[i] );
        }
        // This is just to test if wxDataViewCtrl can
        // cope with removing rows not sorted in
        // descending order
        rows.Sort([](int *v1, int *v2)
        {
            return *v1-*v2;
        });
        RowsDeleted( rows );
    }
}

void TimeBarListModel::AddItem( const wxString text, bool visible, bool islock )
{
    m_textColValues.push_back(text);
    m_bView.push_back(visible);
    m_bLock.push_back(islock);
    Reset(m_textColValues.Count());
}

void TimeBarListModel::GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const
{
    if ((row < m_textColValues.GetCount()))
    {
        switch ( col )
        {
        case Col_EditableText:
            variant = m_textColValues[ row ];
            break;

        case Col_Visible:
            variant = m_bView[row];
            break;

        case Col_Lock:
            variant = m_bLock[row];
            break;

        case Col_Max:
            wxFAIL_MSG( _T("invalid column") );
            break;

        default:
            break;
        }
    }
}

bool TimeBarListModel::SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col )
{
    if (row < m_textColValues.GetCount())
    {
        switch ( col )
        {
        case Col_EditableText:
            m_textColValues[row] = variant.GetString();
            break;

        case Col_Visible:
            m_bView[row] = variant.GetChar();
            break;

        case Col_Lock:
            m_bLock[row] = variant.GetChar();
            break;

        case Col_Max:
        default:
            wxFAIL_MSG( _T("invalid column") );
            break;
        }
    }
    return false;
}

unsigned int TimeBarListModel::GetColumnCount() const
{
    return Col_Max;
}

wxString TimeBarListModel::GetColumnType( unsigned int /*col*/ ) const
{
    return wxT("string");
}
