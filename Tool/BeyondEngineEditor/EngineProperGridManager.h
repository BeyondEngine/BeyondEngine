#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXPROPERTY_ENGINEPROPERTYGRIDMANAGER_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXPROPERTY_ENGINEPROPERTYGRIDMANAGER_H__INCLUDE

#include <wx/propgrid/manager.h>

class CPropertyDescriptionBase;
class CComponentProxy;
class CWxwidgetsPropertyBase;
class CComponentProjectDirectory;
class wxTreeItemId;
class wxTreeCtrl;

class CEnginePropertyGridManager : public wxPropertyGridManager
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
    CEnginePropertyGridManager();
    virtual ~CEnginePropertyGridManager();
    virtual wxPropertyGrid* CreatePropertyGrid() const;
    
    void OnComponentPropertyChanged(wxPropertyGridEvent& event);
    void OnScrollChanged(int nNewPos);
    void OnPropertyGridSelect(wxPropertyGridEvent& event);
    void OnPropertyExpand(wxPropertyGridEvent& event);
    void OnPropertyCollapsed(wxPropertyGridEvent& event);
    void RefreshPropertyInGrid(CPropertyDescriptionBase* pPropertyDesc);
    void InsertInPropertyGrid(const std::vector<CPropertyDescriptionBase*>& pProperties, wxPGProperty* pParent = NULL, bool bRefresh = true);
    wxPGProperty* InsertInPropertyGrid(CPropertyDescriptionBase* pProperty, wxPGProperty* pParent = NULL, int nIndex = -1, bool bRefresh = true);
    void RemovePropertyFromGrid(CPropertyDescriptionBase* pPropertyDesc, bool OnlyRemoveChild = false);
    void InsertComponentsInPropertyGrid(CComponentProxy* pComponent);
    void UpdatePropertyVisiblity(CWxwidgetsPropertyBase* pPropertyBase);
    void OnComponentPropertyChangedImpl(wxPGProperty* pProperty);
    bool IsNeedUpdatePropertyGrid();
    void PreparePropertyDelete(wxPGProperty* pProperty);
    void ExecutePropertyDelete();
    wxPGProperty* GetPGPropertyByBase(CPropertyDescriptionBase* pBase);
    void ClearGrid();
private:
    bool m_bNeedUpdatePropertyGrid;
    wxPGProperty* m_pPropertyToDelete;
    CComponentProxy* m_pCurrentPropertyOwner = nullptr;
    std::map<CComponentProxy*, int32_t> m_viewStartPosMap;
    std::map<CPropertyDescriptionBase*, wxPGProperty*> m_propertyMap;
DECLARE_EVENT_TABLE()
};

#endif