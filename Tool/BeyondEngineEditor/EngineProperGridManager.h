#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXPROPERTY_ENGINEPROPERTYGRIDMANAGER_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXPROPERTY_ENGINEPROPERTYGRIDMANAGER_H__INCLUDE

#include <wx/propgrid/manager.h>

class CPropertyDescriptionBase;
class CComponentProxy;
class CWxwidgetsPropertyBase;
class CComponentProjectDirectory;
class wxTreeItemId;
class wxTreeCtrl;

class CEnginePropertyGirdManager : public wxPropertyGridManager
{
public:
    enum ETreeCtrlIconType
    {
        eTCIT_File,
        eTCIT_FileSelected,
        eTCIT_Folder,
        eTCIT_FolderSelected,
        eTCIT_FolderOpened,
        eTCIT_Count,
        eTCIT_Force32Bit = 0xFFFFFFFF
    };

public:
    CEnginePropertyGirdManager();
    virtual ~CEnginePropertyGirdManager();
    virtual wxPropertyGrid* CreatePropertyGrid() const;
    
    void OnComponentPropertyChanged(wxPropertyGridEvent& event);
    void OnPropertyGridSelect(wxPropertyGridEvent& event);

    void InsertInPropertyGrid(const std::vector<CPropertyDescriptionBase*>& pProperties, wxPGProperty* pParent = NULL, bool bIsReference = false);
    void InsertComponentsInPropertyGrid(CComponentProxy* pComponent, wxPGProperty* pParent = NULL);
    void UpdatePropertyVisiblity(CWxwidgetsPropertyBase* pPropertyBase);
    void OnComponentPropertyChangedImpl(wxPGProperty* pProperty);
    bool IsNeedUpdatePropertyGrid();
    void PreparePropertyDelete(wxPGProperty* pProperty);
    void ExecutePropertyDelete();
    wxPGProperty* GetPGPropertyByBase(CPropertyDescriptionBase* pBase);

private:
    bool m_bNeedUpdatePropertyGrid;
    wxPGProperty* m_pProperty;
DECLARE_EVENT_TABLE()
};

#endif