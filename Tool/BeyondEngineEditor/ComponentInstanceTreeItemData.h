#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TREEITEMDATA_COMPONENTINSTANCETREEITEMDATA_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TREEITEMDATA_COMPONENTINSTANCETREEITEMDATA_H__INCLUDE

#include <wx/treectrl.h>

class CComponentBase;
class CComponentInstanceTreeItemData : public wxTreeItemData
{
public:
    CComponentInstanceTreeItemData(CComponentBase* pComponentBase);
    virtual ~CComponentInstanceTreeItemData();

    bool IsDirectory();
    CComponentBase* GetComponentBase() const;

private:
    CComponentBase* m_pComponentBase;
};

#endif