#ifndef BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SCENEGRIDWND_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EXTRAWINDOW_SCENEGRIDWND_H__INCLUDE

class wxPropertyGrid;
class wxPropertyGridEvent;
class wxPGProperty;
class CSceneGridWnd : public wxFrame
{
    typedef wxFrame super;
public:
    CSceneGridWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name = wxDialogNameStr);
    virtual ~CSceneGridWnd();
    virtual bool Show(bool bShow = true) override;
    void InitByConfig();
    float GetGridDistanceValue() const;
    float GetGridLineWidthValue() const;
    bool GetRenderPositiveDirectionLineValue() const;
    bool GetRenderSpriteEdgeValue() const;
    bool GetRenderSpriteValue() const;
    bool GetRenderSpriteUserDefinePosValue() const;
    bool GetEnableSpriteDepthTestValue() const;
    bool GetEnableUIValue() const;
    float GetVolumeValue() const;
    float GetSfxVolumeValue() const;
    uint32_t GetBGColorValue() const;
    int GetGridStartPosXValue() const;
    int GetGridStartPosYValue() const;
    int GetGridWidthValue() const;
    int GetGridHeightValue() const;
    uint32_t GetFPSValue() const;
    uint32_t GetLanguageValue() const;
    bool GetRenderLandingshipSelectAreaValue() const;
    bool GetEnableShakeCameraValue() const;
    bool GetRenderMapGridValue() const;
    bool GetRenderPathFindingGridValue() const;
    bool GetEnableUpdateParticleValue() const;
    bool GetEnableRenderParticleValue() const;
    bool GetRenderParticleWireFrameValue() const;
    bool GetRenderBuildingValue() const;
    bool GetRenderGameObjectCenterPosPointValue() const;
    bool GetRenderAttackRangeValue() const;
    bool GetRenderSkeleton() const;
    float GetGlobalColorFactorValue() const;
    float GetFontSize() const;
    float GetFontBorderSize() const;
private:
    void OnClose(wxCloseEvent& event);
    void OnPropertyChanged(wxPropertyGridEvent& event);
    void AddSpinCtrl(wxPGProperty* pProperty, float fStep = 1.0f);

private:
    wxPropertyGrid* m_pPropertyGrid;
    wxPGProperty* m_pBGColor;
    wxPGProperty* m_pGridStartPosX;
    wxPGProperty* m_pGridStartPosY;
    wxPGProperty* m_pGridWidth;
    wxPGProperty* m_pGridHeight;
    wxPGProperty* m_pGridDistance;
    wxPGProperty* m_pGridLineWidth;
    wxPGProperty* m_pRenderPositiveDirectionLine;
    wxPGProperty* m_pRenderMapGrid;
    wxPGProperty* m_pRenderPathFindingGrid;
    wxPGProperty* m_pRenderBuilding;
    wxPGProperty* m_pRenderSprite;
    wxPGProperty* m_pRenderSpriteEdge;
    wxPGProperty* m_pRenderAttackRange;
    wxPGProperty* m_pRenderSkeleton;
    wxPGProperty* m_pEnableSpriteDepthTest;
    wxPGProperty* m_pRenderSpriteUserDefinePos;
    wxPGProperty* m_pRenderGameObjectPosPoint;
    wxPGProperty* m_pEnableUI;
    wxPGProperty* m_pVolume;
    wxPGProperty* m_pSfxVolume;
    wxPGProperty* m_pRenderLandingshipSelectArea;
    wxPGProperty* m_pEnableShakeCamera;
    wxPGProperty* m_pEnableUpdateParticle;
    wxPGProperty* m_pEnableRenderParticle;
    wxPGProperty* m_pRenderParticleWireFrame;
    wxPGProperty* m_pFPS;
    wxPGProperty* m_pLanguage;
    // Post Process
    wxPGProperty* m_pGlobalColorFactor;

    wxPGProperty* m_pFontBorderSize;
    wxPGProperty* m_pFontSize;
};

#endif