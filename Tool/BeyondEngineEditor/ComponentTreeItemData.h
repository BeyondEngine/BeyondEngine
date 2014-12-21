#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TREEITEMDATA_COMPONENTTREEITEMDATA_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TREEITEMDATA_COMPONENTTREEITEMDATA_H__INCLUDE

#include <wx/treectrl.h>

class CComponentTreeItemData : public wxTreeItemData
{
public:
    CComponentTreeItemData(bool bIsDirectory, uint32_t guid);
    virtual ~CComponentTreeItemData();

    bool IsDirectory();
    uint32_t GetGUID();
private:
    bool m_bIsDirectory;
    uint32_t m_uComponentGUID;
};

#endif