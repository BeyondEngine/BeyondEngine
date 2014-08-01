#include "stdafx.h"
#include "DataViewTreeModelNode.h"


CDataViewTreeModelNode::CDataViewTreeModelNode( CDataViewTreeModelNode* parent, const wxString &title, const bool bView, const bool bLock )
{
    m_pParent = parent;
    m_strTitle = title;
    m_cVisible = bView;
    m_cLock = bLock;
}

CDataViewTreeModelNode::~CDataViewTreeModelNode()
{
    size_t count = m_children.GetCount();
    for (size_t i = 0; i < count; i++)
    {
        CDataViewTreeModelNode *child = m_children[i];
        BEATS_SAFE_DELETE(child)
    }
}

CDataViewTreeModelNode* CDataViewTreeModelNode::GetParent()
{ 
    return m_pParent; 
}
DataViewTreeModelNodePtrArray& CDataViewTreeModelNode::GetChildren()
{ 
    return m_children; 
}
CDataViewTreeModelNode* CDataViewTreeModelNode::GetNthChild( unsigned int n )
{ 
    return m_children.Item( n ); 
}
void CDataViewTreeModelNode::Insert( CDataViewTreeModelNode* child, unsigned int n)
{ 
    m_children.Insert( child, n); 
}
void CDataViewTreeModelNode::Append( CDataViewTreeModelNode* child )
{ 
    m_children.Add( child ); 
}
unsigned int CDataViewTreeModelNode::GetChildCount() const
{ 
    return m_children.GetCount(); 
}
