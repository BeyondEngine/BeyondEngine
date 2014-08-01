#ifndef BEYOND_ENGINE_EDITOR_VIEWAGENT_TERRAINVIEWAGENT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_VIEWAGENT_TERRAINVIEWAGENT_H__INCLUDE

#include "ViewAgentBase.h"

class CTerrainViewAgent : public CViewAgentBase
{
    BEATS_DECLARE_SINGLETON(CTerrainViewAgent);
public:
    virtual void ProcessMouseEvent(wxMouseEvent& event) override;
    virtual void CreateTools() override;
    virtual void InView() override;
    virtual void OutView() override;
    virtual void OnCommandEvent(wxCommandEvent& event) override;

    void SetBrushTypeByIndex(int nIndex);
    void SetBrushSize( int nSize );

private:
    int m_nBrushSize;
    int m_nTerrainType;
    wxTextCtrl* m_pTextCtrl;
    
};
#endif