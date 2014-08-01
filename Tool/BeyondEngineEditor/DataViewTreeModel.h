#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_DATAVIEW__DATAVIEWTREEMODEL_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_DATAVIEW__DATAVIEWTREEMODEL_H__INCLUDE

#include <wx/dynarray.h>
#include <wx/dataview.h>
#include "DataViewTreeModelNode.h"

class CDataViewTreeModel : public wxDataViewModel
{
public:
    CDataViewTreeModel();
    ~CDataViewTreeModel();
    wxString GetTitle( const wxDataViewItem &item ) const;
    bool IsVisible( const wxDataViewItem &item ) const;
    bool IsLock( const wxDataViewItem &item ) const;
    CDataViewTreeModelNode* GetRootNode();
    void Delete( const wxDataViewItem &item );
    void Clear();
    int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2, unsigned int column, bool ascending ) const;
    void SetAssociateCtrl(wxDataViewCtrl* pCtrl);
    virtual unsigned int GetColumnCount() const;
    virtual wxString GetColumnType( unsigned int col ) const;
    virtual void GetValue( wxVariant &variant, const wxDataViewItem &item, unsigned int col ) const;
    virtual bool SetValue( const wxVariant &variant, const wxDataViewItem &item, unsigned int col );

    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
    virtual bool IsContainer( const wxDataViewItem &item ) const;
    virtual unsigned int GetChildren( const wxDataViewItem &parent, wxDataViewItemArray &array ) const;

private:
    CDataViewTreeModelNode*  m_pRoot;
    wxDataViewCtrl* m_pAssociatedCtrl;

};

#endif