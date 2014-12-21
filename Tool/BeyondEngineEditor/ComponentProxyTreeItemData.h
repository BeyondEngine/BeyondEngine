#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TREEITEMDATA_COMPONENTPROXYTREEITEMDATA_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TREEITEMDATA_COMPONENTPROXYTREEITEMDATA_H__INCLUDE

#include <wx/treectrl.h>

class CComponentProxy;
class CComponentProxyTreeItemData : public wxTreeItemData
{
public:
    CComponentProxyTreeItemData(CComponentProxy* pComponentProxy);
    virtual ~CComponentProxyTreeItemData();

    bool IsDirectory();
    CComponentProxy* GetComponentProxy() const;

private:
    CComponentProxy* m_pComponentProxy;
};

#endif