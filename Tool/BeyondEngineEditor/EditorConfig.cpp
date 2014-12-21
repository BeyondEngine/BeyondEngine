#include "stdafx.h"
#include "EditorConfig.h"
#include "EngineEditor.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "EditorMainFrame.h"
#include "Render/Camera.h"
#include "EditorSceneWindow.h"
#include "Render/RenderManager.h"
#include "Render/RenderTarget.h"
#include "Render/Viewport.h"
#include "Render/GridRenderObject.h"
#include "Scene/SceneManager.h"
#include "Render/Material.h"
#include "SceneGridWnd.h"
#include "EditCameraWnd.h"

CEditorConfig* CEditorConfig::m_pInstance = NULL;

#define CONFIG_FILE_NAME _T("EditorConfig.xml")

CEditorConfig::CEditorConfig()
{
    m_pSceneGridParam = new SSceneGridParam;
    m_pCameraParam = new SCameraParam;
}

CEditorConfig::~CEditorConfig()
{
    BEATS_SAFE_DELETE(m_pSceneGridParam);
    BEATS_SAFE_DELETE(m_pCameraParam);
}

void CEditorConfig::LoadFromFile()
{
    TString strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
    strWorkingPath.append(_T("\\")).append(CONFIG_FILE_NAME);
    if (CFilePathTool::GetInstance()->Exists(strWorkingPath.c_str()))
    {
        rapidxml::file<> fdoc(strWorkingPath.c_str());
        rapidxml::xml_document<> configXML;
        try
        {
            configXML.parse<rapidxml::parse_default>(fdoc.data());
        }
        catch (rapidxml::parse_error err)
        {
            BEATS_ASSERT(false, _T("Load config file %s faled!/n%s/n"), CONFIG_FILE_NAME, err.what());
        }
        rapidxml::xml_node<>* pRootElement = configXML.first_node("Root");
        if (pRootElement)
        {
            rapidxml::xml_node<>* pConfigNode = pRootElement->first_node("Config");
            if (pConfigNode)
            {
                LoadConfigNode(pConfigNode);
            }

            rapidxml::xml_node<>* pCoordinateNode = pRootElement->first_node("coordinate");
            if (pCoordinateNode)
            {
                LoadCoordinateNode(pCoordinateNode);
            }

            rapidxml::xml_node<>* pSceneGridParamNode = pRootElement->first_node("SceneGridParam");
            if (pSceneGridParamNode)
            {
                LoadSceneGridParamNode(pSceneGridParamNode);
            }

            rapidxml::xml_node<>* pCameraParamNode = pRootElement->first_node("CameraParam");
            if (pCameraParamNode)
            {
                LoadCameraParamNode(pCameraParamNode);
            }

            rapidxml::xml_node<>* ResolutionConfigNode = pRootElement->first_node("ResolutionConfig");
            if (ResolutionConfigNode != NULL)
            {
                for (auto element = ResolutionConfigNode->first_node(); element; element = element->next_sibling())
                {             
                    if (element->first_attribute("uWidth") && element->first_attribute("uHeight"))
                    {
                        m_resolutionVector.push_back(CVec2(atoi(element->first_attribute("uWidth")->value()), atoi(element->first_attribute("uHeight")->value())));
                    }
                }
            }

            rapidxml::xml_node<>* LastOpenFilesConfigNode = pRootElement->first_node("LastOpenFilesConfig");
            if (LastOpenFilesConfigNode != NULL)
            {
                for (auto element = LastOpenFilesConfigNode->first_node(); element; element = element->next_sibling())
                {
                    if (element->first_attribute("LastOpenFile"))
                    {
                        m_lastOpenFilesVector.push_back(element->first_attribute("LastOpenFile")->value());
                    }
                }
            }
        }
    }
}

void CEditorConfig::LoadConfigNode(rapidxml::xml_node<>* pConfigNode)
{
    if (pConfigNode->first_attribute("LastOpenProject"))
    {
        m_strLastOpenProject = pConfigNode->first_attribute("LastOpenProject")->value();
    }
    if (pConfigNode->first_attribute("LastOpenFile"))
    {
        m_strLastOpenFile = pConfigNode->first_attribute("LastOpenFile")->value();
    }
    if (pConfigNode->first_attribute("Language"))
    {
        m_currLanguage = (ELanguageType)atoi(pConfigNode->first_attribute("Language")->value());
    }
    if (pConfigNode->first_attribute("ShowAboutDlg"))
    {
        int nShowDlg = 0;
        nShowDlg = atoi(pConfigNode->first_attribute("ShowAboutDlg")->value());
        m_bShowAboutDlgAfterLaunch = nShowDlg != 0;
    }
}

void CEditorConfig::LoadCoordinateNode(rapidxml::xml_node<>* pCoordinateNode)
{
    if (pCoordinateNode->first_attribute("AxisColorX"))
    {
        m_axisColorX = ConvertStringToColor(pCoordinateNode->first_attribute("AxisColorX")->value());
    }
    if (pCoordinateNode->first_attribute("AxisColorY"))
    {
        m_axisColorY = ConvertStringToColor(pCoordinateNode->first_attribute("AxisColorY")->value());
    }
    if (pCoordinateNode->first_attribute("AxisColorZ"))
    {
        m_axisColorZ = ConvertStringToColor(pCoordinateNode->first_attribute("AxisColorZ")->value());
    }
    if (pCoordinateNode->first_attribute("AxisSelectColor"))
    {
        m_axisSelectColor = ConvertStringToColor(pCoordinateNode->first_attribute("AxisSelectColor")->value());
    }
    if (pCoordinateNode->first_attribute("PanelSelectColor"))
    {
        m_panelSelectColor = ConvertStringToColor(pCoordinateNode->first_attribute("PanelSelectColor")->value());
    }
    if (pCoordinateNode->first_attribute("AxisLengthFactor"))
    {
        m_fAxisLengthFactor = (float)_tstof(pCoordinateNode->first_attribute("AxisLengthFactor")->value());
    }
    if (pCoordinateNode->first_attribute("ConeHeightFactor"))
    {
        m_fConeHeightFactor = (float)_tstof(pCoordinateNode->first_attribute("ConeHeightFactor")->value());
    }
    if (pCoordinateNode->first_attribute("ConeBottomColorFactor"))
    {
        m_fConeBottomColorFactor = (float)_tstof(pCoordinateNode->first_attribute("ConeBottomColorFactor")->value());
    }
    if (pCoordinateNode->first_attribute("ConeAngle"))
    {
        m_fConeAngle = (float)_tstof(pCoordinateNode->first_attribute("ConeAngle")->value());
    }
    if (pCoordinateNode->first_attribute("TranslatePanelStartPosRate"))
    {
        m_fTranslatePanelStartPosRate = (float)_tstof(pCoordinateNode->first_attribute("TranslatePanelStartPosRate")->value());
    }
}

void CEditorConfig::LoadSceneGridParamNode(rapidxml::xml_node<>* pSceneGridParamNode)
{
    if (pSceneGridParamNode->first_attribute("BGColor"))
    {
        m_pSceneGridParam->BGColor = ConvertStringToColor(pSceneGridParamNode->first_attribute("BGColor")->value());
    }
    if (pSceneGridParamNode->first_attribute("iGridStartPosX"))
    {
        m_pSceneGridParam->iGridStartPosX = (int)_tstoi(pSceneGridParamNode->first_attribute("iGridStartPosX")->value());
    }
    if (pSceneGridParamNode->first_attribute("iGridStartPosY"))
    {
        m_pSceneGridParam->iGridStartPosY = (int)_tstoi(pSceneGridParamNode->first_attribute("iGridStartPosY")->value());
    }
    if (pSceneGridParamNode->first_attribute("iGridWidth"))
    {
        m_pSceneGridParam->iGridWidth = (int)_tstoi(pSceneGridParamNode->first_attribute("iGridWidth")->value());
    }
    if (pSceneGridParamNode->first_attribute("iGridHeight"))
    {
        m_pSceneGridParam->iGridHeight = (int)_tstoi(pSceneGridParamNode->first_attribute("iGridHeight")->value());
    }
    if (pSceneGridParamNode->first_attribute("fGridDistance"))
    {
        m_pSceneGridParam->fGridDistance = (float)_tstof(pSceneGridParamNode->first_attribute("fGridDistance")->value());
    }
    if (pSceneGridParamNode->first_attribute("fGridLineWidth"))
    {
        m_pSceneGridParam->fGridLineWidth = (float)_tstof(pSceneGridParamNode->first_attribute("fGridLineWidth")->value());
    }
    int nBool = 0;
    if (pSceneGridParamNode->first_attribute("bRenderPositiveDirectionLine"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bRenderPositiveDirectionLine")->value());
        m_pSceneGridParam->bRenderPositiveDirectionLine = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bRenderMapGrid"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bRenderMapGrid")->value());
        m_pSceneGridParam->bRenderMapGrid = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bRenderPathFindingGrid"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bRenderPathFindingGrid")->value());
        m_pSceneGridParam->bRenderPathFindingGrid = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bRenderBuilding"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bRenderBuilding")->value());
        m_pSceneGridParam->bRenderBuilding = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bRenderSprite"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bRenderSprite")->value());
        m_pSceneGridParam->bRenderSprite = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bRenderSpriteEdge"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bRenderSpriteEdge")->value());
        m_pSceneGridParam->bRenderSpriteEdge = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bRenderAttackRange"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bRenderAttackRange")->value());
        m_pSceneGridParam->bRenderAttackRange = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bEnableSpriteDepthTest"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bEnableSpriteDepthTest")->value());
        m_pSceneGridParam->bEnableSpriteDepthTest = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bRenderSpriteUserDefinePos"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bRenderSpriteUserDefinePos")->value());
        m_pSceneGridParam->bRenderSpriteUserDefinePos = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bRenderGameObjectCenterPosPoint"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bRenderGameObjectCenterPosPoint")->value());
        m_pSceneGridParam->bRenderGameObjectPosPoint = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bEnableUI"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bEnableUI")->value());
        m_pSceneGridParam->bEnableUI = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("fVolume"))
    {
        m_pSceneGridParam->fVolume = (float)_tstof(pSceneGridParamNode->first_attribute("fVolume")->value());
    }
    if (pSceneGridParamNode->first_attribute("fSfxVolume"))
    {
        m_pSceneGridParam->fSfxVolume = (float)_tstof(pSceneGridParamNode->first_attribute("fSfxVolume")->value());
    }
    if (pSceneGridParamNode->first_attribute("bRenderLandingshipSelectArea"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bRenderLandingshipSelectArea")->value());
        m_pSceneGridParam->bRenderLandingshipSelectArea = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bEnableShakeCamera"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bEnableShakeCamera")->value());
        m_pSceneGridParam->bEnableShakeCamera = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bEnableUpdateParticle"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bEnableUpdateParticle")->value());
        m_pSceneGridParam->bEnableUpdateParticle = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bEnableRenderParticle"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bEnableRenderParticle")->value());
        m_pSceneGridParam->bEnableRenderParticle = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("bRenderParticleWireFrame"))
    {
        nBool = atoi(pSceneGridParamNode->first_attribute("bRenderParticleWireFrame")->value());
        m_pSceneGridParam->bRenderParticleWireFrame = nBool != 0;
    }
    if (pSceneGridParamNode->first_attribute("uFPS"))
    {
        m_pSceneGridParam->uFPS = (uint32_t)_tstof(pSceneGridParamNode->first_attribute("uFPS")->value());
    }
    if (pSceneGridParamNode->first_attribute("uLanguage"))
    {
        m_pSceneGridParam->uLanguage = (uint32_t)_tstof(pSceneGridParamNode->first_attribute("uLanguage")->value());
    }
    if (pSceneGridParamNode->first_attribute("fGlobalColorFactor"))
    {
        m_pSceneGridParam->fGlobalColorFactor = (float)_tstof(pSceneGridParamNode->first_attribute("fGlobalColorFactor")->value());
    }
    if (pSceneGridParamNode->first_attribute("fontSize"))
    {
        m_pSceneGridParam->fFontSize = (float)_tstof(pSceneGridParamNode->first_attribute("fontSize")->value());
    }
    if (pSceneGridParamNode->first_attribute("fontBorderSize"))
    {
        m_pSceneGridParam->fFontBorderSize = (float)_tstof(pSceneGridParamNode->first_attribute("fontBorderSize")->value());
    }
}

void CEditorConfig::LoadCameraParamNode(rapidxml::xml_node<>* pCameraParamNode)
{
    if (pCameraParamNode->first_attribute("fPosX"))
    {
        m_pCameraParam->fPosX = (float)_tstof(pCameraParamNode->first_attribute("fPosX")->value());
    }
    if (pCameraParamNode->first_attribute("fPosY"))
    {
        m_pCameraParam->fPosY = (float)_tstof(pCameraParamNode->first_attribute("fPosY")->value());
    }
    if (pCameraParamNode->first_attribute("fPosZ"))
    {
        m_pCameraParam->fPosZ = (float)_tstof(pCameraParamNode->first_attribute("fPosZ")->value());
    }
    if (pCameraParamNode->first_attribute("fRotationX"))
    {
        m_pCameraParam->fRotationX = (float)_tstof(pCameraParamNode->first_attribute("fRotationX")->value());
    }
    if (pCameraParamNode->first_attribute("fRotationY"))
    {
        m_pCameraParam->fRotationY = (float)_tstof(pCameraParamNode->first_attribute("fRotationY")->value());
    }
    if (pCameraParamNode->first_attribute("fRotationZ"))
    {
        m_pCameraParam->fRotationZ = (float)_tstof(pCameraParamNode->first_attribute("fRotationZ")->value());
    }
    if (pCameraParamNode->first_attribute("fClipNear"))
    {
        m_pCameraParam->fClipNear = (float)_tstof(pCameraParamNode->first_attribute("fClipNear")->value());
    }
    if (pCameraParamNode->first_attribute("fClipFar"))
    {
        m_pCameraParam->fClipFar = (float)_tstof(pCameraParamNode->first_attribute("fClipFar")->value());
    }
    if (pCameraParamNode->first_attribute("fSpeed"))
    {
        m_pCameraParam->fSpeed = (float)_tstof(pCameraParamNode->first_attribute("fSpeed")->value());
    }
    if (pCameraParamNode->first_attribute("fShiftMoveSpeedRate"))
    {
        m_pCameraParam->fShiftMoveSpeedRate = (float)_tstof(pCameraParamNode->first_attribute("fShiftMoveSpeedRate")->value());
    }
    if (pCameraParamNode->first_attribute("fFov"))
    {
        m_pCameraParam->fFov = (float)_tstof(pCameraParamNode->first_attribute("fFov")->value());
    }
}

void CEditorConfig::SaveToFile()
{
    SaveSceneGridParam();
    SaveCameraParam();

    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* pDecl = doc.allocate_node(rapidxml::node_declaration);
    rapidxml::xml_attribute<>* pDecl_ver = doc.allocate_attribute("version", "1.0");
    pDecl->append_attribute(pDecl_ver);
    doc.append_node(pDecl);
    rapidxml::xml_node<>* pRootElement = doc.allocate_node(rapidxml::node_element, "Root");
    doc.append_node(pRootElement);

    rapidxml::xml_node<>* pConfigNode = doc.allocate_node(rapidxml::node_element, "Config");
    SaveConfigNode(pConfigNode, &doc);
    pRootElement->append_node(pConfigNode);

    rapidxml::xml_node<>* pCoordinateNode = doc.allocate_node(rapidxml::node_element, "coordinate");
    SaveCoordinateNode(pCoordinateNode, &doc);
    pRootElement->append_node(pCoordinateNode);

    rapidxml::xml_node<>* pSceneGridParamNode = doc.allocate_node(rapidxml::node_element, "SceneGridParam");
    SaveSceneGridParamNode(pSceneGridParamNode, &doc);
    pRootElement->append_node(pSceneGridParamNode);

    rapidxml::xml_node<>* pCameraParamNode = doc.allocate_node(rapidxml::node_element, "CameraParam");
    SaveCameraParamNode(pCameraParamNode, &doc);
    pRootElement->append_node(pCameraParamNode);

    rapidxml::xml_node<>* ResolutionConfigNode = doc.allocate_node(rapidxml::node_element, "ResolutionConfig");
    pRootElement->append_node(ResolutionConfigNode);
    for (auto iter : m_resolutionVector)
    {
        rapidxml::xml_node<>* ConfigResoluton = doc.allocate_node(rapidxml::node_element, "Config");
        ResolutionConfigNode->append_node(ConfigResoluton);
        ConfigResoluton->append_attribute(doc.allocate_attribute("uWidth", doc.allocate_string(std::to_string(iter.X()).c_str())));
        ConfigResoluton->append_attribute(doc.allocate_attribute("uHeight", doc.allocate_string(std::to_string(iter.Y()).c_str())));
    }

    rapidxml::xml_node<>* LastOpenFilesConfigNode = doc.allocate_node(rapidxml::node_element, "LastOpenFilesConfig");
    pRootElement->append_node(LastOpenFilesConfigNode);
    for (auto iter : m_lastOpenFilesVector)
    {
        rapidxml::xml_node<>* ConfigLastOpenFile = doc.allocate_node(rapidxml::node_element, "Config");
        LastOpenFilesConfigNode->append_node(ConfigLastOpenFile);
        ConfigLastOpenFile->append_attribute(doc.allocate_attribute("LastOpenFile", doc.allocate_string(iter.c_str())));
    }

    TString strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
    strWorkingPath.append(_T("\\")).append(CONFIG_FILE_NAME);
    
    TString strOut;
    rapidxml::print(std::back_inserter(strOut), doc, 0);
    std::ofstream out(strWorkingPath.c_str());
    out << strOut;
    out.close();
}

void CEditorConfig::SaveConfigNode(rapidxml::xml_node<>* pConfigNode, rapidxml::xml_document<>*pDoc)
{
    pConfigNode->append_attribute(pDoc->allocate_attribute("LastOpenProject", pDoc->allocate_string(m_strLastOpenProject.c_str())));
    pConfigNode->append_attribute(pDoc->allocate_attribute("LastOpenFile", pDoc->allocate_string(m_strLastOpenFile.c_str())));
    pConfigNode->append_attribute(pDoc->allocate_attribute("Language", pDoc->allocate_string(std::to_string(m_currLanguage).c_str())));
    pConfigNode->append_attribute(pDoc->allocate_attribute("ShowAboutDlg", m_bShowAboutDlgAfterLaunch ? "1" : "0"));
}

void CEditorConfig::SaveCoordinateNode(rapidxml::xml_node<>* pCoordinateNode, rapidxml::xml_document<>*pDoc)
{
    pCoordinateNode->append_attribute(pDoc->allocate_attribute("AxisColorX", pDoc->allocate_string(ConvertColorToString(m_axisColorX).c_str())));
    pCoordinateNode->append_attribute(pDoc->allocate_attribute("AxisColorY", pDoc->allocate_string(ConvertColorToString(m_axisColorY).c_str())));
    pCoordinateNode->append_attribute(pDoc->allocate_attribute("AxisColorZ", pDoc->allocate_string(ConvertColorToString(m_axisColorZ).c_str())));
    pCoordinateNode->append_attribute(pDoc->allocate_attribute("AxisSelectColor", pDoc->allocate_string(ConvertColorToString(m_axisSelectColor).c_str())));
    pCoordinateNode->append_attribute(pDoc->allocate_attribute("PanelSelectColor", pDoc->allocate_string(ConvertColorToString(m_panelSelectColor).c_str())));
    pCoordinateNode->append_attribute(pDoc->allocate_attribute("AxisLengthFactor", pDoc->allocate_string(std::to_string(m_fAxisLengthFactor).c_str())));
    pCoordinateNode->append_attribute(pDoc->allocate_attribute("ConeHeightFactor", pDoc->allocate_string(std::to_string(m_fConeHeightFactor).c_str())));
    pCoordinateNode->append_attribute(pDoc->allocate_attribute("ConeBottomColorFactor", pDoc->allocate_string(std::to_string(m_fConeBottomColorFactor).c_str())));
    pCoordinateNode->append_attribute(pDoc->allocate_attribute("ConeAngle", pDoc->allocate_string(std::to_string(m_fConeAngle).c_str())));
    pCoordinateNode->append_attribute(pDoc->allocate_attribute("TranslatePanelStartPosRate", pDoc->allocate_string(std::to_string(m_fTranslatePanelStartPosRate).c_str())));
}

void CEditorConfig::SaveSceneGridParamNode(rapidxml::xml_node<>* pSceneGridParamNode, rapidxml::xml_document<>*pDoc)
{
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("BGColor", pDoc->allocate_string(ConvertColorToString(m_pSceneGridParam->BGColor).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("iGridStartPosX", pDoc->allocate_string(std::to_string(m_pSceneGridParam->iGridStartPosX).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("iGridStartPosY", pDoc->allocate_string(std::to_string(m_pSceneGridParam->iGridStartPosY).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("iGridWidth", pDoc->allocate_string(std::to_string(m_pSceneGridParam->iGridWidth).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("iGridHeight", pDoc->allocate_string(std::to_string(m_pSceneGridParam->iGridHeight).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("fGridDistance", pDoc->allocate_string(std::to_string(m_pSceneGridParam->fGridDistance).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("fGridLineWidth", pDoc->allocate_string(std::to_string(m_pSceneGridParam->fGridLineWidth).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bRenderPositiveDirectionLine", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bRenderPositiveDirectionLine).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bRenderMapGrid", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bRenderMapGrid).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bRenderPathFindingGrid", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bRenderPathFindingGrid).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bRenderBuilding", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bRenderBuilding).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bRenderSprite", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bRenderSprite).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bRenderSpriteEdge", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bRenderSpriteEdge).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bRenderAttackRange", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bRenderAttackRange).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bEnableSpriteDepthTest", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bEnableSpriteDepthTest).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bRenderSpriteUserDefinePos", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bRenderSpriteUserDefinePos).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bRenderGameObjectCenterPosPoint", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bRenderGameObjectPosPoint).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bEnableUI", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bEnableUI).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("fVolume", pDoc->allocate_string(std::to_string(m_pSceneGridParam->fVolume).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("fSfxVolume", pDoc->allocate_string(std::to_string(m_pSceneGridParam->fSfxVolume).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bRenderLandingshipSelectArea", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bRenderLandingshipSelectArea).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bEnableShakeCamera", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bEnableShakeCamera).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bEnableUpdateParticle", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bEnableUpdateParticle).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bEnableRenderParticle", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bEnableRenderParticle).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("bRenderParticleWireFrame", pDoc->allocate_string(std::to_string(m_pSceneGridParam->bRenderParticleWireFrame).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("uFPS", pDoc->allocate_string(std::to_string(m_pSceneGridParam->uFPS).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("uLanguage", pDoc->allocate_string(std::to_string(m_pSceneGridParam->uLanguage).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("fGlobalColorFactor", pDoc->allocate_string(std::to_string(m_pSceneGridParam->fGlobalColorFactor).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("fontSize", pDoc->allocate_string(std::to_string(m_pSceneGridParam->fFontSize).c_str())));
    pSceneGridParamNode->append_attribute(pDoc->allocate_attribute("fontBorderSize", pDoc->allocate_string(std::to_string(m_pSceneGridParam->fFontBorderSize).c_str())));
}

void CEditorConfig::SaveCameraParamNode(rapidxml::xml_node<>* pCameraParamNode, rapidxml::xml_document<>*pDoc)
{
    pCameraParamNode->append_attribute(pDoc->allocate_attribute("fPosX", pDoc->allocate_string(std::to_string(m_pCameraParam->fPosX).c_str())));
    pCameraParamNode->append_attribute(pDoc->allocate_attribute("fPosY", pDoc->allocate_string(std::to_string(m_pCameraParam->fPosY).c_str())));
    pCameraParamNode->append_attribute(pDoc->allocate_attribute("fPosZ", pDoc->allocate_string(std::to_string(m_pCameraParam->fPosZ).c_str())));
    pCameraParamNode->append_attribute(pDoc->allocate_attribute("fRotationX", pDoc->allocate_string(std::to_string(m_pCameraParam->fRotationX).c_str())));
    pCameraParamNode->append_attribute(pDoc->allocate_attribute("fRotationY", pDoc->allocate_string(std::to_string(m_pCameraParam->fRotationY).c_str())));
    pCameraParamNode->append_attribute(pDoc->allocate_attribute("fRotationZ", pDoc->allocate_string(std::to_string(m_pCameraParam->fRotationZ).c_str())));
    pCameraParamNode->append_attribute(pDoc->allocate_attribute("fClipNear", pDoc->allocate_string(std::to_string(m_pCameraParam->fClipNear).c_str())));
    pCameraParamNode->append_attribute(pDoc->allocate_attribute("fClipFar", pDoc->allocate_string(std::to_string(m_pCameraParam->fClipFar).c_str())));
    pCameraParamNode->append_attribute(pDoc->allocate_attribute("fSpeed", pDoc->allocate_string(std::to_string(m_pCameraParam->fSpeed).c_str())));
    pCameraParamNode->append_attribute(pDoc->allocate_attribute("fShiftMoveSpeedRate", pDoc->allocate_string(std::to_string(m_pCameraParam->fShiftMoveSpeedRate).c_str())));
    pCameraParamNode->append_attribute(pDoc->allocate_attribute("fFov", pDoc->allocate_string(std::to_string(m_pCameraParam->fFov).c_str())));
}

bool CEditorConfig::IsShowAboutDlgAfterLaunch() const
{
    return m_bShowAboutDlgAfterLaunch;
}

void CEditorConfig::SetShowAboutDlgAfterlaunch(bool bFlag)
{
    m_bShowAboutDlgAfterLaunch = bFlag;
}

const TString& CEditorConfig::GetLastOpenProject() const
{
    return m_strLastOpenProject;
}

void CEditorConfig::SetLastOpenProject(const TCHAR* pszProjectPath)
{
    m_strLastOpenProject = pszProjectPath;
    SaveToFile();
}

const TString& CEditorConfig::GetLastOpenFile() const
{
    return m_strLastOpenFile;
}

void CEditorConfig::SetLastOpenFile(const TCHAR* pszFilePath)
{
    const TString& strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
    m_strLastOpenFile = CFilePathTool::GetInstance()->MakeRelative(strWorkingPath.c_str(), pszFilePath);
    SaveToFile();
}

ELanguageType CEditorConfig::GetCurrLanguage() const
{
    return m_currLanguage;
}

void CEditorConfig::SetCurrLanguage(ELanguageType language)
{
    m_currLanguage = language;
    SaveToFile();
}

const std::vector<CVec2>& CEditorConfig::GetResolution() const
{
    return m_resolutionVector;
}

void CEditorConfig::SetResolution(std::vector<CVec2>& resolutionVec)
{
    m_resolutionVector = resolutionVec;
    SaveToFile();
}

std::vector<TString>& CEditorConfig::GetLastOpenFiles()
{
    return m_lastOpenFilesVector;
}

void CEditorConfig::SetLastOpenFiles(std::vector<TString>& lastOpenFilesVec)
{
    m_lastOpenFilesVector = lastOpenFilesVec;
    SaveToFile();
}

CColor CEditorConfig::GetAxisColorX() const
{
    return m_axisColorX;
}

void CEditorConfig::SetAxisColorX(CColor color)
{
    m_axisColorX = color;
}

CColor CEditorConfig::GetAxisColorY() const
{
    return m_axisColorY;
}

void CEditorConfig::SetAxisColorY(CColor color)
{
    m_axisColorY = color;
}

CColor CEditorConfig::GetAxisColorZ() const
{
    return m_axisColorZ;
}

void CEditorConfig::SetAxisColorZ(CColor color)
{
    m_axisColorZ = color;
}

CColor CEditorConfig::GetAxisSelectColor() const
{
    return m_axisSelectColor;
}

void CEditorConfig::SetAxisSelectColor(CColor color)
{
    m_axisSelectColor = color;
}

CColor CEditorConfig::GetPanelSelectColor() const
{
    return m_panelSelectColor;
}

void CEditorConfig::SetPanelSelectColor(CColor color)
{
    m_panelSelectColor = color;
}

float CEditorConfig::GetAxisLengthFactor() const
{
    return m_fAxisLengthFactor;
}

void CEditorConfig::SetAxisLengthFactor(float AxisLengthFactor)
{
    m_fAxisLengthFactor = AxisLengthFactor;
}

float CEditorConfig::GetConeHeightFactor() const
{
    return m_fConeHeightFactor;
}

void CEditorConfig::SetConeHeightFactor(float ConeHeightFactor)
{
    m_fConeHeightFactor = ConeHeightFactor;
}

float CEditorConfig::GetConeBottomColorFactor() const
{
    return m_fConeBottomColorFactor;
}

void CEditorConfig::SetConeBottomColorFactor(float ConeBottomColorFactor)
{
    m_fConeBottomColorFactor = ConeBottomColorFactor;
}

float CEditorConfig::GetConeAngle() const
{
    return m_fConeAngle;
}

void CEditorConfig::SetConeAngle(float ConeAngle)
{
    m_fConeAngle = ConeAngle;
}

float CEditorConfig::GetTranslatePanelStartPosRate() const
{
    return m_fTranslatePanelStartPosRate;
}

void CEditorConfig::SetTranslatePanelStartPosRate(float TranslatePanelStartPosRate)
{
    m_fTranslatePanelStartPosRate = TranslatePanelStartPosRate;
}

SSceneGridParam* CEditorConfig::GetSceneGridParam()
{
    return m_pSceneGridParam;
}

SCameraParam* CEditorConfig::GetCameraParam()
{
    return m_pCameraParam;
}

void CEditorConfig::SaveCameraParam()
{
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    CEditCameraWnd* pEditCamera = pMainFrame->GetCameraEditWnd();
    m_pCameraParam->fPosX = pEditCamera->GetPosXValue();
    m_pCameraParam->fPosY = pEditCamera->GetPosYValue();
    m_pCameraParam->fPosZ = pEditCamera->GetPosZValue();
    m_pCameraParam->fRotationX = pEditCamera->GetRotationXValue();
    m_pCameraParam->fRotationY = pEditCamera->GetRotationYValue();
    m_pCameraParam->fRotationZ = pEditCamera->GetRotationZValue();
    m_pCameraParam->fClipNear = pEditCamera->GetClipNearValue();
    m_pCameraParam->fClipFar = pEditCamera->GetClipFarValue();
    m_pCameraParam->fSpeed = pEditCamera->GetSpeedValue();
    m_pCameraParam->fShiftMoveSpeedRate = pEditCamera->GetShiftMoveSpeedRateValue();
    m_pCameraParam->fFov = pEditCamera->GetFovValue();
}

void CEditorConfig::SaveSceneGridParam()
{
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    CSceneGridWnd* pSceneGrid = pMainFrame->GetSceneGridWnd();
    m_pSceneGridParam->BGColor = pSceneGrid->GetBGColorValue();
    m_pSceneGridParam->iGridStartPosX = pSceneGrid->GetGridStartPosXValue();
    m_pSceneGridParam->iGridStartPosY = pSceneGrid->GetGridStartPosYValue();
    m_pSceneGridParam->iGridWidth = pSceneGrid->GetGridWidthValue();
    m_pSceneGridParam->iGridHeight = pSceneGrid->GetGridHeightValue();
    m_pSceneGridParam->fGridDistance = pSceneGrid->GetGridDistanceValue();
    m_pSceneGridParam->fGridLineWidth = pSceneGrid->GetGridLineWidthValue();
    m_pSceneGridParam->bRenderPositiveDirectionLine = pSceneGrid->GetRenderPositiveDirectionLineValue();
    m_pSceneGridParam->bEnableShakeCamera = pSceneGrid->GetEnableShakeCameraValue();
    m_pSceneGridParam->bRenderMapGrid = pSceneGrid->GetRenderMapGridValue();
    m_pSceneGridParam->bRenderPathFindingGrid = pSceneGrid->GetRenderPathFindingGridValue();
    m_pSceneGridParam->bRenderBuilding = pSceneGrid->GetRenderBuildingValue();
    m_pSceneGridParam->bRenderLandingshipSelectArea = pSceneGrid->GetRenderLandingshipSelectAreaValue();
    m_pSceneGridParam->bRenderSpriteEdge = pSceneGrid->GetRenderSpriteEdgeValue();
    m_pSceneGridParam->bRenderSprite = pSceneGrid->GetRenderSpriteValue();
    m_pSceneGridParam->bRenderAttackRange = pSceneGrid->GetRenderAttackRangeValue();
    m_pSceneGridParam->bRenderSkeleton = pSceneGrid->GetRenderSkeleton();
    m_pSceneGridParam->bEnableSpriteDepthTest = pSceneGrid->GetEnableSpriteDepthTestValue();
    m_pSceneGridParam->bRenderSpriteUserDefinePos = pSceneGrid->GetRenderSpriteUserDefinePosValue();
    m_pSceneGridParam->bRenderGameObjectPosPoint = pSceneGrid->GetRenderGameObjectCenterPosPointValue();
    m_pSceneGridParam->bEnableUI = pSceneGrid->GetEnableUIValue();
    m_pSceneGridParam->fVolume = pSceneGrid->GetVolumeValue();
    m_pSceneGridParam->fSfxVolume = pSceneGrid->GetSfxVolumeValue();
    m_pSceneGridParam->bEnableUpdateParticle = pSceneGrid->GetEnableUpdateParticleValue();
    m_pSceneGridParam->bEnableRenderParticle = pSceneGrid->GetEnableRenderParticleValue();
    m_pSceneGridParam->bRenderParticleWireFrame = pSceneGrid->GetRenderParticleWireFrameValue();
    m_pSceneGridParam->uFPS = pSceneGrid->GetFPSValue();
    m_pSceneGridParam->uLanguage = pSceneGrid->GetLanguageValue();
    m_pSceneGridParam->fGlobalColorFactor = pSceneGrid->GetGlobalColorFactorValue();
    m_pSceneGridParam->fFontSize = pSceneGrid->GetFontSize();
    m_pSceneGridParam->fFontBorderSize = pSceneGrid->GetFontBorderSize();
}

TString CEditorConfig::ConvertColorToString(CColor uColor) const
{
    TString strColor = _T("0x");
    TCHAR szEndChar[25];
    _itot(uColor, szEndChar, 16);
    strColor.append(szEndChar);
    return strColor;
}

CColor CEditorConfig::ConvertStringToColor(const TString& strColor) const
{
    CColor ret = 0;
    if (strColor.length() > 0)
    {
        uint32_t uRadix = 10;
        TString strNumber = strColor;
        if (strColor.length() > 2 && 
            strColor.at(0) == '0' &&
            ((strColor.at(1) == 'X') || (strColor.at(1) == 'x')))
        {
            uRadix = 16;
            std::vector<TString> vecColorString;
            CStringHelper::GetInstance()->SplitString(strColor.c_str(), _T("x"), vecColorString);
            BEATS_ASSERT(vecColorString.size() == 2);
            strNumber = vecColorString[1];
        }
        TCHAR* pEndChar = NULL;
        ret = _tcstoul(strNumber.c_str(), &pEndChar, uRadix);
        BEATS_ASSERT(_tcslen(pEndChar) == 0, _T("Read uint from string %s error, stop at %s"), strNumber.c_str(), pEndChar);
    }
    return ret;
}
