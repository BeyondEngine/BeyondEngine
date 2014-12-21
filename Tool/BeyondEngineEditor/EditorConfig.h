#ifndef BEYOND_ENGINE_EDITOR_EDITORCONFIG_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EDITORCONFIG_H__INCLUDE

#include "Language/LanguageManager.h"
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_utils.hpp"
#include "RapidXML/rapidxml_print.hpp"

struct SSceneGridParam
{
    CColor  BGColor = 0x000000FF;
    int     iGridStartPosX = 1;
    int     iGridStartPosY = 0;
    int     iGridWidth = 0;
    int     iGridHeight = 0;
    float   fGridDistance = 1.0f;
    float   fGridLineWidth = 1.0f;
    bool    bRenderPositiveDirectionLine = false;
    bool    bRenderMapGrid = false;
    bool    bRenderPathFindingGrid = false;
    bool    bRenderBuilding = true;
    bool    bRenderSprite = true;
    bool    bRenderSpriteEdge = false;
    bool    bRenderAttackRange = false;
    bool    bRenderSkeleton = false;
    bool    bEnableSpriteDepthTest = false;
    bool    bRenderSpriteUserDefinePos = false;
    bool    bRenderGameObjectPosPoint = false;
    bool    bEnableUI = true;
    float   fVolume = 1.0f;
    float   fSfxVolume = 1.0f;
    bool    bRenderLandingshipSelectArea = false;
    bool    bEnableShakeCamera = true;
    bool    bEnableUpdateParticle = true;
    bool    bEnableRenderParticle = true;
    bool    bRenderParticleWireFrame = false;
    uint32_t uFPS = 60;
    uint32_t uLanguage = 0;
    float   fGlobalColorFactor = 1.0f;
    float   fFontBorderSize = 1.0f;
    float   fFontSize = 24.0f;
};

struct SCameraParam
{
    float fPosX = 0.f;
    float fPosY = 100.f;
    float fPosZ = 0.f;
    float fRotationX = -90.f;
    float fRotationY = 0.f;
    float fRotationZ = 0.f;
    float fClipNear = 0.01f;
    float fClipFar = 1000.f;
    float fSpeed = 1.f;
    float fShiftMoveSpeedRate = 5.f;
    float fFov = 25.f;
};

class CEditorConfig
{
    BEATS_DECLARE_SINGLETON(CEditorConfig)
public:
    void LoadFromFile();
    void SaveToFile();

    void SaveSceneGridParam();
    void SaveCameraParam();

    void LoadConfigNode(rapidxml::xml_node<>* pConfigNode);
    void SaveConfigNode(rapidxml::xml_node<>* pConfigNode, rapidxml::xml_document<>*pDoc);

    void LoadCoordinateNode(rapidxml::xml_node<>* pCoordinateNode);
    void SaveCoordinateNode(rapidxml::xml_node<>* pCoordinateNode, rapidxml::xml_document<>*pDoc);

    void LoadSceneGridParamNode(rapidxml::xml_node<>* pSceneGridParamNode);
    void SaveSceneGridParamNode(rapidxml::xml_node<>* pSceneGridParamNode, rapidxml::xml_document<>*pDoc);

    void LoadCameraParamNode(rapidxml::xml_node<>* pCameraParamNode);
    void SaveCameraParamNode(rapidxml::xml_node<>* pCameraParamNode, rapidxml::xml_document<>*pDoc);

    bool IsShowAboutDlgAfterLaunch() const;
    void SetShowAboutDlgAfterlaunch(bool bFlag);

    const TString& GetLastOpenProject() const;
    void SetLastOpenProject(const TCHAR* pszProjectPath);

    const TString& GetLastOpenFile() const;
    void SetLastOpenFile(const TCHAR* pszFilePath);

    ELanguageType GetCurrLanguage() const;
    void SetCurrLanguage(ELanguageType language);

    const std::vector<CVec2>& GetResolution() const;
    void SetResolution(std::vector<CVec2>& resolutionVec);

    std::vector<TString>& GetLastOpenFiles();
    void SetLastOpenFiles(std::vector<TString>& lastOpenFilesVec);

    CColor GetAxisColorX() const;
    void SetAxisColorX(CColor color);

    CColor GetAxisColorY() const;
    void SetAxisColorY(CColor color);

    CColor GetAxisColorZ() const;
    void SetAxisColorZ(CColor color);

    CColor GetAxisSelectColor() const;
    void SetAxisSelectColor(CColor color);

    CColor GetPanelSelectColor() const;
    void SetPanelSelectColor(CColor color);

    float GetAxisLengthFactor() const;
    void SetAxisLengthFactor(float AxisLengthFactor);

    float GetConeHeightFactor() const;
    void SetConeHeightFactor(float ConeHeightFactor);

    float GetConeBottomColorFactor() const;
    void SetConeBottomColorFactor(float ConeBottomColorFactor);

    float GetConeAngle() const;
    void SetConeAngle(float ConeAngle);

    float GetTranslatePanelStartPosRate() const;
    void SetTranslatePanelStartPosRate(float TranslatePanelStartPosRate);

    SSceneGridParam* GetSceneGridParam();

    SCameraParam* GetCameraParam();

    TString ConvertColorToString(CColor uColor) const;
    CColor ConvertStringToColor(const TString& strColor) const;

private:
    bool m_bShowAboutDlgAfterLaunch = true;
    ELanguageType m_currLanguage = eLT_English;
    TString m_strLastOpenProject;
    TString m_strLastOpenFile;
    std::vector<CVec2> m_resolutionVector;
    std::vector<TString> m_lastOpenFilesVector;
    CColor m_axisColorX = 0xFF0000FF;
    CColor m_axisColorY = 0x00FF00FF;
    CColor m_axisColorZ = 0x0000FFFF;
    CColor m_axisSelectColor = 0xFFFF00FF;
    CColor m_panelSelectColor = 0xFFFF0099;
    float m_fAxisLengthFactor = 0.35f;
    float m_fConeHeightFactor = 0.2f;
    float m_fConeBottomColorFactor = 0.5f;
    float m_fConeAngle = 15.f;
    float m_fTranslatePanelStartPosRate = 0.4f;

    SSceneGridParam* m_pSceneGridParam;
    SCameraParam*   m_pCameraParam;
};

#endif
