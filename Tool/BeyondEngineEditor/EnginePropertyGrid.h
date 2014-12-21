#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXPROPERTY_ENGINEPROPERTYGRID_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXPROPERTY_ENGINEPROPERTYGRID_H__INCLUDE

#include <wx/propgrid/propgrid.h>
class CEnginePropertyGridManager;
class CEnginePropertyGrid : public wxPropertyGrid
{
public:
    CEnginePropertyGrid();
    virtual ~CEnginePropertyGrid();
    void FreeEditorCtrl();
    void SetManager(CEnginePropertyGridManager* pManager);
    void OnScrollEvent(wxScrollWinEvent &event);
    CEnginePropertyGridManager* GetManager() const;

private:
    CEnginePropertyGridManager* m_pManager;
    DECLARE_EVENT_TABLE()
};


#endif