#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_TREEITEMDATA_COMPONENTFILETREEITEMDATA_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_TREEITEMDATA_COMPONENTFILETREEITEMDATA_H__INCLUDE

#include <wx/treectrl.h>
class CComponentProjectDirectory;

class CComponentFileTreeItemData : public wxTreeItemData
{
public:
    CComponentFileTreeItemData(CComponentProjectDirectory* pData, const TString& fileName);
    virtual ~CComponentFileTreeItemData();
    bool IsDirectory();
    const TString& GetFileName() const;
    CComponentProjectDirectory* GetProjectDirectory() const;
    void SetData(CComponentProjectDirectory* pData);
private:
    TString m_strFileName;
    CComponentProjectDirectory* m_pData;
};

#endif