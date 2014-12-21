#ifndef BEYOND_ENGINE_EDITOR_VIEWAGENT_TERRAINVIEWAGENT_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_VIEWAGENT_TERRAINVIEWAGENT_H__INCLUDE

#include "ViewAgentBase.h"

enum class eBrushType
{
    // For island scene map
    eBT_Eraser,
    eBT_BattleArea,
    eBT_LandingArea,
    eBT_ObjectBrush,

    // for universe scene
    eBT_Near,
    eBT_Middle,
    eBT_Far,
    eBT_SuperFar,
    eBT_Border,
    eBT_Dark,
    eBT_Block,

    eBT_Count,
};
class CTerrainViewAgent : public CViewAgentBase
{
    BEATS_DECLARE_SINGLETON(CTerrainViewAgent);
public:
    virtual void ProcessMouseEvent(wxMouseEvent& event) override;
    virtual void CreateTools() override;
    virtual void InView() override;
    virtual void OutView() override;
    virtual void OnCommandEvent(wxCommandEvent& event) override;
    virtual void OnOpenComponentFile() override;

    void BrushOnPos(float fX, float fY);
    void SetBrushSize( int nSize );
    void SetBrushType(eBrushType eType);
    void RefreshTools();
private:
    bool m_bEditUniverseOrIsland;
    bool m_bInitUniverseEditMode;
    bool m_bHideCover = false;
    bool m_bHideMovableCell = false;
    bool m_bShowIndexText = false;
    int m_nBrushSize;
    eBrushType m_eBrushType;
    wxTextCtrl* m_pTextObjectName;
    wxTextCtrl* m_pTextObjectLevel;
    wxTextCtrl* m_pTextObjectNum;
    wxTextCtrl* m_pTextCtrl;
    wxTextCtrl* m_pDirectionType;
    wxTextCtrl* m_pMapSizeX = nullptr;
    wxTextCtrl* m_pMapSizeY = nullptr;
    wxStaticText* m_pCurrentSize;
    wxStaticText* m_pCurrentMapData = nullptr;
    wxSizer* m_pToolMainSizer;
    wxSizer* m_pLandingAreaBrushSettingSizer;
    wxSizer* m_pObjectBrushSettingSizer;
    wxSizer* m_pBrushTypeSizer;
    wxSizer* m_pUniverseBrushSizer;
    wxSizer* m_pMapInfoSizer = nullptr;
};
#endif