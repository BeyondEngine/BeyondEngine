#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_DATAVIEW__DATAVIEWTREEMODELNODE_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_DATAVIEW__DATAVIEWTREEMODELNODE_H__INCLUDE

class CDataViewTreeModelNode;
WX_DEFINE_ARRAY_PTR( CDataViewTreeModelNode*, DataViewTreeModelNodePtrArray );

class CDataViewTreeModelNode
{
public:
    CDataViewTreeModelNode( CDataViewTreeModelNode* parent, const wxString &title, const bool bView = true, const bool bLock = false );

    ~CDataViewTreeModelNode();

    CDataViewTreeModelNode* GetParent();
    DataViewTreeModelNodePtrArray& GetChildren();
    CDataViewTreeModelNode* GetNthChild( unsigned int n );
    void Insert( CDataViewTreeModelNode* child, unsigned int n);
    void Append( CDataViewTreeModelNode* child );
    unsigned int GetChildCount() const;

public:     // public to avoid getters/setters
    wxString                m_strTitle;
    wxChar                  m_cVisible;
    wxChar                  m_cLock;

private:
    CDataViewTreeModelNode*         m_pParent;
    DataViewTreeModelNodePtrArray   m_children;
};

#endif