#include "stdafx.h"
#include "DataViewTreeModel.h"

CDataViewTreeModel::CDataViewTreeModel()
{
    m_pRoot = new CDataViewTreeModelNode( NULL, "Objects View" );
}

CDataViewTreeModel::~CDataViewTreeModel()
{
    delete m_pRoot;
}

int CDataViewTreeModel::Compare( const wxDataViewItem &item1, const wxDataViewItem &item2, unsigned int column, bool ascending ) const
{
    wxASSERT(item1.IsOk() && item2.IsOk());
    // should never happen

    if (IsContainer(item1) && IsContainer(item2))
    {
        wxVariant value1, value2;
        GetValue( value1, item1, 0 );
        GetValue( value2, item2, 0 );

        wxString str1 = value1.GetString();
        wxString str2 = value2.GetString();
        int res = str1.Cmp( str2 );
        if (res) return res;

        // items must be different
        wxUIntPtr litem1 = (wxUIntPtr) item1.GetID();
        wxUIntPtr litem2 = (wxUIntPtr) item2.GetID();

        return litem1-litem2;
    }

    return wxDataViewModel::Compare( item1, item2, column, ascending );
}

void CDataViewTreeModel::SetAssociateCtrl(wxDataViewCtrl* pCtrl)
{
    m_pAssociatedCtrl = pCtrl;
}

unsigned int CDataViewTreeModel::GetColumnCount() const
{
    return 6;
}

wxString CDataViewTreeModel::GetTitle( const wxDataViewItem &item ) const
{
    CDataViewTreeModelNode *node = (CDataViewTreeModelNode*) item.GetID();
    if (!node)
        return wxEmptyString;

    return node->m_strTitle;
}

bool CDataViewTreeModel::IsVisible( const wxDataViewItem &item ) const
{
    CDataViewTreeModelNode *node = (CDataViewTreeModelNode*) item.GetID();
    BEATS_ASSERT(node)

    return node->m_cVisible == '+' ? true : false;
}

bool CDataViewTreeModel::IsLock( const wxDataViewItem &item ) const
{
    CDataViewTreeModelNode *node = (CDataViewTreeModelNode*) item.GetID();
    BEATS_ASSERT(node)

    return node->m_cLock == '+' ? true : false;
}

void CDataViewTreeModel::Delete( const wxDataViewItem &item )
{
    CDataViewTreeModelNode *node = (CDataViewTreeModelNode*) item.GetID();
    if (!node)
        return;

    wxDataViewItem parent( node->GetParent() );
    if (!parent.IsOk())
    {
        wxASSERT(node == m_pRoot);

        wxLogError( "Cannot remove the root item!" );
        return;
    }

    // first remove the node from the parent's array of children;
    // NOTE: MyMusicTreeModelNodePtrArray is only an array of _pointers_
    //       thus removing the node from it doesn't result in freeing it
    node->GetParent()->GetChildren().Remove( node );
    BEATS_SAFE_DELETE(node)
    ItemDeleted( parent, item );
}

void CDataViewTreeModel::GetValue( wxVariant &variant,
                                const wxDataViewItem &item, unsigned int col ) const
{
    wxASSERT(item.IsOk());
    CDataViewTreeModelNode *node = (CDataViewTreeModelNode*) item.GetID();

    switch (col)
    {
    case 0:
        variant = node->m_strTitle;
        break;
    case 1:
        variant = node->m_cVisible;
        break;
    case 2:
        variant = node->m_cLock;
        break;
    default:
        wxLogError( "DataViewTreeModel::GetValue: wrong column %d", col );
    }

}

bool CDataViewTreeModel::SetValue( const wxVariant &variant,
                                const wxDataViewItem &item, unsigned int col )
{
    wxASSERT(item.IsOk());

    CDataViewTreeModelNode *node = (CDataViewTreeModelNode*) item.GetID();
    switch (col)
    {
    case 0:
        node->m_strTitle = variant.GetString();
        return true;
    case 1:
        node->m_cVisible = variant.GetChar();
        return true;
    case 2:
        node->m_cLock = variant.GetChar();
        return true;
    default:
        wxLogError( "DataViewTreeModel::SetValue: wrong column" );
    }
    return false;
}

wxDataViewItem CDataViewTreeModel::GetParent( const wxDataViewItem &item ) const
{
    // the invisible root node has no parent
    if (!item.IsOk())
        return wxDataViewItem(0);

    CDataViewTreeModelNode *node = (CDataViewTreeModelNode*) item.GetID();

    // "MyMusic" also has no parent
    if (node == m_pRoot)
        return wxDataViewItem(0);

    return wxDataViewItem( (void*) node->GetParent() );
}

bool CDataViewTreeModel::IsContainer( const wxDataViewItem &item ) const
{
    return !item.IsOk();
}

unsigned int CDataViewTreeModel::GetChildren( const wxDataViewItem &parent,
                                           wxDataViewItemArray &array ) const
{
    CDataViewTreeModelNode *node = (CDataViewTreeModelNode*) parent.GetID();
    if (!node)
    {
        array.Add( wxDataViewItem( (void*) m_pRoot ) );
        return 1;
    }
    if (node->GetChildCount() == 0)
    {
        return 0;
    }

    unsigned int count = node->GetChildren().GetCount();
    for (unsigned int pos = 0; pos < count; pos++)
    {
        CDataViewTreeModelNode *child = node->GetChildren().Item( pos );
        array.Add( wxDataViewItem( (void*) child ) );
    }

    return count;
}
wxString CDataViewTreeModel::GetColumnType( unsigned int /*col*/ ) const
{
    return wxT("string");
}

CDataViewTreeModelNode* CDataViewTreeModel::GetRootNode()
{
    return m_pRoot;
}

void CDataViewTreeModel::Clear()
{
    int nCount = m_pRoot->GetChildCount();
    for (int i = 0; i < nCount; i++)
    {
//        Delete(m_pRoot->GetNthChild(0));
    }
}
