#include "stdafx.h"
#include "SceneGridWnd.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "BeyondEngineEditorGLWindow.h"
#include "Render/GridRenderObject.h"
#include "Render/RenderManager.h"
#include "EditorSceneWindow.h"
#include "Scene/SceneManager.h"
#include "Render/Material.h"
#include "Render/RenderTarget.h"
#include "Render/Viewport.h"
#include "EditorConfig.h"
#include "ParticleSystem/ParticleManager.h"
#include "GUI/Font/FontManager.h"
#include "GUI/Font/FontFace.h"

CSceneGridWnd::CSceneGridWnd(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : super(parent, id, title, pos, size, style, name)
    , m_pPropertyGrid(NULL)
    , m_pBGColor(NULL)
    , m_pGridStartPosX(NULL)
    , m_pGridStartPosY(NULL)
    , m_pGridWidth(NULL)
    , m_pGridHeight(NULL)
    , m_pGridDistance(NULL)
    , m_pGridLineWidth(NULL)
    , m_pRenderPositiveDirectionLine(NULL)
    , m_pRenderMapGrid(NULL)
    , m_pRenderPathFindingGrid(NULL)
    , m_pRenderBuilding(NULL)
    , m_pRenderSprite(NULL)
    , m_pRenderSpriteEdge(NULL)
    , m_pRenderAttackRange(NULL)
    , m_pRenderSkeleton(nullptr)
    , m_pEnableSpriteDepthTest(NULL)
    , m_pRenderSpriteUserDefinePos(NULL)
    , m_pRenderGameObjectPosPoint(NULL)
    , m_pEnableUI(NULL)
    , m_pVolume(NULL)
    , m_pSfxVolume(NULL)
    , m_pRenderLandingshipSelectArea(NULL)
    , m_pEnableShakeCamera(NULL)
    , m_pEnableUpdateParticle(NULL)
    , m_pEnableRenderParticle(NULL)
    , m_pFPS(NULL)
    , m_pLanguage(NULL)
    , m_pGlobalColorFactor(NULL)
{
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(pMainSizer);
    m_pPropertyGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxSize(400,400),
        wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED );

    wxPGProperty* pCatalogGridProperty = new wxStringProperty(_T("网格"), wxPG_LABEL);
    m_pPropertyGrid->GetState()->DoInsert(NULL, -1, pCatalogGridProperty);
    pCatalogGridProperty->ChangeFlag(wxPG_PROP_READONLY, true);
    pCatalogGridProperty->SetBackgroundColour(0xFF777777, 0);

    m_pGridStartPosX = new wxIntProperty(wxT("网格起点X"), wxT("GridStartPosX"));
    AddSpinCtrl(m_pGridStartPosX);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogGridProperty, -1, m_pGridStartPosX);
    m_pGridStartPosX->SetBackgroundColour(0xFFFFFFFF, 0);
    m_pGridStartPosY = new wxIntProperty(wxT("网格起点Y"), wxT("GridStartPosY"));
    AddSpinCtrl(m_pGridStartPosY);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogGridProperty, -1, m_pGridStartPosY);
    m_pGridStartPosY->SetBackgroundColour(0xFFFFFFFF, 0);
    m_pGridWidth = new wxUIntProperty(wxT("网格宽度"), wxT("GridWidth"));
    AddSpinCtrl(m_pGridWidth);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogGridProperty, -1, m_pGridWidth);
    m_pGridWidth->SetBackgroundColour(0xFFFFFFFF, 0);
    m_pGridHeight = new wxUIntProperty(wxT("网格高度"), wxT("GridHeight"));
    AddSpinCtrl(m_pGridHeight);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogGridProperty, -1, m_pGridHeight);
    m_pGridHeight->SetBackgroundColour(0xFFFFFFFF, 0);
    m_pGridDistance = new wxFloatProperty(wxT("网格密度"), wxT("GridDistance"));
    m_pGridDistance->SetAttribute( wxPG_ATTR_MIN, 0.1f );
    m_pGridDistance->SetAttribute( wxPG_ATTR_MAX, 100);
    AddSpinCtrl(m_pGridDistance, 0.2f);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogGridProperty, -1, m_pGridDistance);
    m_pGridDistance->SetBackgroundColour(0xFFFFFFFF, 0);
    m_pGridLineWidth = new wxFloatProperty(wxT("网格粗细"), wxT("GridLineWidth"));
    m_pGridLineWidth->SetAttribute( wxPG_ATTR_MIN, 0.1f );
    AddSpinCtrl(m_pGridLineWidth, 0.2f);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogGridProperty, -1, m_pGridLineWidth);
    m_pGridLineWidth->SetBackgroundColour(0xFFFFFFFF, 0);


    wxPGProperty* pCatalogDrawProperty = new wxStringProperty(_T("绘制"), wxPG_LABEL);
    m_pPropertyGrid->GetState()->DoInsert(NULL, -1, pCatalogDrawProperty);
    pCatalogDrawProperty->ChangeFlag(wxPG_PROP_READONLY, true);
    pCatalogDrawProperty->SetBackgroundColour(0xFF777777, 0);

    m_pBGColor = new wxColourProperty(wxT("背景颜色"), wxT("BGColor"), *wxBLUE);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pBGColor);
    m_pBGColor->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pRenderPositiveDirectionLine = new wxBoolProperty(wxT("绘制正向线"), wxT("RenderPositiveLine"));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderPositiveDirectionLine, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pRenderPositiveDirectionLine);
    m_pRenderPositiveDirectionLine->SetBackgroundColour(0xFFFFFFFF, 0);
    m_pRenderMapGrid = new wxBoolProperty(wxT("绘制地图网格"), wxT("RenderMapGrid"));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderMapGrid, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pRenderMapGrid);
    m_pRenderMapGrid->SetBackgroundColour(0xFFFFFFFF, 0);
    m_pRenderPathFindingGrid = new wxBoolProperty(wxT("绘制寻路网格"), wxT("RenderPathFindingGrid"));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderPathFindingGrid, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pRenderPathFindingGrid);
    m_pRenderPathFindingGrid->SetBackgroundColour(0xFFFFFFFF, 0);
    m_pRenderBuilding = new wxBoolProperty(wxT("绘制建筑"), wxT("RenderBuilding"));
    m_pRenderBuilding->SetValue(wxVariant(true));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderBuilding, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pRenderBuilding);
    m_pRenderBuilding->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pRenderSprite = new wxBoolProperty(wxT("绘制精灵"), wxT("RenderSprite"));
    m_pRenderSprite->SetValue(wxVariant(true));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderSprite, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pRenderSprite);
    m_pRenderSprite->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pRenderSpriteEdge = new wxBoolProperty(wxT("绘制精灵边框"), wxT("RenderSpriteEdge"));
    m_pRenderSpriteEdge->SetValue(wxVariant(false));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderSpriteEdge, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pRenderSpriteEdge);
    m_pRenderSpriteEdge->SetBackgroundColour(0xFFFFFFFF, 0);
    
    m_pRenderSkeleton = new wxBoolProperty(wxT("绘制所有骨骼"), wxT("RenderSkeleton"));
    m_pRenderSkeleton->SetValue(wxVariant(false));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderSkeleton, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pRenderSkeleton);
    m_pRenderSkeleton->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pRenderAttackRange = new wxBoolProperty(wxT("绘制攻击范围"), wxT("RenderAttackRange"));
    m_pRenderAttackRange->SetValue(wxVariant(false));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderAttackRange, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pRenderAttackRange);
    m_pRenderAttackRange->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pRenderSpriteUserDefinePos = new wxBoolProperty(wxT("绘制自定义点"), wxT("RenderSpriteUserDefinePos"));
    m_pRenderSpriteUserDefinePos->SetValue(wxVariant(false));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderSpriteUserDefinePos, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pRenderSpriteUserDefinePos);
    m_pRenderSpriteUserDefinePos->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pRenderGameObjectPosPoint = new wxBoolProperty(wxT("绘制物体坐标点"), wxT("RenderGameObjectPosPoint"));
    m_pRenderGameObjectPosPoint->SetValue(wxVariant(false));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderGameObjectPosPoint, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pRenderGameObjectPosPoint);
    m_pRenderGameObjectPosPoint->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pRenderLandingshipSelectArea = new wxBoolProperty(wxT("渲染战舰选择区"), wxT("RenderShipSelectArea"));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderLandingshipSelectArea, wxPG_BOOL_USE_CHECKBOX, true);
    m_pRenderLandingshipSelectArea->SetValue(wxVariant(false));
    m_pPropertyGrid->GetState()->DoInsert(pCatalogDrawProperty, -1, m_pRenderLandingshipSelectArea);
    m_pRenderLandingshipSelectArea->SetBackgroundColour(0xFFFFFFFF, 0);

    wxPGProperty* pCatalogParticleProperty = new wxStringProperty(_T("粒子"), wxPG_LABEL);
    m_pPropertyGrid->GetState()->DoInsert(NULL, -1, pCatalogParticleProperty);
    pCatalogParticleProperty->ChangeFlag(wxPG_PROP_READONLY, true);
    pCatalogParticleProperty->SetBackgroundColour(0xFF777777, 0);

    m_pEnableRenderParticle = new wxBoolProperty(wxT("粒子渲染开关"), wxT("EnableRenderParticle"));
    m_pPropertyGrid->SetPropertyAttribute(m_pEnableRenderParticle, wxPG_BOOL_USE_CHECKBOX, true);
    m_pEnableRenderParticle->SetValue(wxVariant(true));
    m_pPropertyGrid->GetState()->DoInsert(pCatalogParticleProperty, -1, m_pEnableRenderParticle);
    m_pEnableRenderParticle->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pRenderParticleWireFrame = new wxBoolProperty(wxT("渲染粒子边框"), wxT("RenderParticleWireFrame"));
    m_pPropertyGrid->SetPropertyAttribute(m_pRenderParticleWireFrame, wxPG_BOOL_USE_CHECKBOX, true);
    m_pRenderParticleWireFrame->SetValue(wxVariant(false));
    m_pPropertyGrid->GetState()->DoInsert(pCatalogParticleProperty, -1, m_pRenderParticleWireFrame);
    m_pRenderParticleWireFrame->SetBackgroundColour(0xFFFFFFFF, 0);    

    m_pEnableUpdateParticle = new wxBoolProperty(wxT("粒子更新开关"), wxT("EnableUpdateParticle"));
    m_pPropertyGrid->SetPropertyAttribute(m_pEnableUpdateParticle, wxPG_BOOL_USE_CHECKBOX, true);
    m_pEnableUpdateParticle->SetValue(wxVariant(true));
    m_pPropertyGrid->GetState()->DoInsert(pCatalogParticleProperty, -1, m_pEnableUpdateParticle);
    m_pEnableUpdateParticle->SetBackgroundColour(0xFFFFFFFF, 0);

    wxPGProperty* pCatalogSetProperty = new wxStringProperty(_T("设置"), wxPG_LABEL);
    m_pPropertyGrid->GetState()->DoInsert(NULL, -1, pCatalogSetProperty);
    pCatalogSetProperty->ChangeFlag(wxPG_PROP_READONLY, true);
    pCatalogSetProperty->SetBackgroundColour(0xFF777777, 0);

    m_pEnableSpriteDepthTest = new wxBoolProperty(wxT("开启精灵深度检测"), wxT("EnableSpriteDepthTest"));
    m_pEnableSpriteDepthTest->SetValue(wxVariant(false));
    m_pPropertyGrid->SetPropertyAttribute(m_pEnableSpriteDepthTest, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogSetProperty, -1, m_pEnableSpriteDepthTest);
    m_pEnableSpriteDepthTest->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pEnableUI = new wxBoolProperty(wxT("开启UI"), wxT("EnableUI"));
    m_pEnableUI->SetValue(wxVariant(true));
    m_pPropertyGrid->SetPropertyAttribute(m_pEnableUI, wxPG_BOOL_USE_CHECKBOX, true);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogSetProperty, -1, m_pEnableUI);
    m_pEnableUI->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pVolume = new wxFloatProperty(wxT("音量"), wxT("Volume"), 1.0f);
    m_pVolume->SetAttribute(wxPG_ATTR_MIN, 0);
    m_pVolume->SetAttribute(wxPG_ATTR_MAX, 1.0f);
    AddSpinCtrl(m_pVolume, 0.02f);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogSetProperty, -1, m_pVolume);
    m_pVolume->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pSfxVolume = new wxFloatProperty(wxT("音效音量"), wxT("SfxVolume"), 1.0f);
    m_pSfxVolume->SetAttribute(wxPG_ATTR_MIN, 0);
    m_pSfxVolume->SetAttribute(wxPG_ATTR_MAX, 1.0f);
    AddSpinCtrl(m_pSfxVolume, 0.02f);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogSetProperty, -1, m_pSfxVolume);
    m_pSfxVolume->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pEnableShakeCamera = new wxBoolProperty(wxT("允许相机震动"), wxT("EnableShakeCamera"));
    m_pPropertyGrid->SetPropertyAttribute(m_pEnableShakeCamera, wxPG_BOOL_USE_CHECKBOX, true);
    m_pEnableShakeCamera->SetValue(wxVariant(true));
    m_pPropertyGrid->GetState()->DoInsert(pCatalogSetProperty, -1, m_pEnableShakeCamera);
    m_pEnableShakeCamera->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pFPS = new wxUIntProperty(wxT("FPS"), wxT("FPS"));
    m_pFPS->SetAttribute( wxPG_ATTR_MIN, 1 );
    m_pPropertyGrid->GetState()->DoInsert(pCatalogSetProperty, -1, m_pFPS);
    m_pFPS->SetBackgroundColour(0xFFFFFFFF, 0);

    wxArrayString labels;
    wxArrayInt values;
    for (int i = 0; i < eLT_Count; ++i)
    {
        labels.push_back(pszLanguageTypeString[i]);
        values.push_back(i);
    }
    m_pLanguage = new wxEnumProperty(wxT("语言设置"), _T("Language"), labels, values, 0);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogSetProperty, -1, m_pLanguage);
    m_pLanguage->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pGlobalColorFactor = new wxFloatProperty(wxT("全屏亮度"), wxT("GlobalColorFactor"));
    m_pGlobalColorFactor->SetAttribute(wxPG_ATTR_MIN, 0);
    m_pGlobalColorFactor->SetValue(1.0f);
    AddSpinCtrl(m_pGlobalColorFactor, 0.02f);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogSetProperty, -1, m_pGlobalColorFactor);
    m_pGlobalColorFactor->SetBackgroundColour(0xFFFFFFFF, 0);


    m_pFontSize = new wxFloatProperty(wxT("字体库大小"), wxT("fontSize"));
    m_pFontSize->SetAttribute(wxPG_ATTR_MIN, 20.0f);
    m_pFontSize->SetAttribute(wxPG_ATTR_MAX, 1000.0f);
    AddSpinCtrl(m_pFontSize, 0.2f);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogSetProperty, -1, m_pFontSize);
    m_pFontSize->SetBackgroundColour(0xFFFFFFFF, 0);

    m_pFontBorderSize = new wxFloatProperty(wxT("描边大小"), wxT("fontBorderSize"));
    m_pFontBorderSize->SetAttribute(wxPG_ATTR_MIN, 1.0f);
    m_pFontBorderSize->SetAttribute(wxPG_ATTR_MAX, 100.0f);
    AddSpinCtrl(m_pFontBorderSize, 0.2f);
    m_pPropertyGrid->GetState()->DoInsert(pCatalogSetProperty, -1, m_pFontBorderSize);
    m_pFontBorderSize->SetBackgroundColour(0xFFFFFFFF, 0);

    pMainSizer->Add(m_pPropertyGrid, 1, wxALL, 5);
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CSceneGridWnd::OnClose), NULL, this);
    m_pPropertyGrid->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CSceneGridWnd::OnPropertyChanged), NULL, this);
}

CSceneGridWnd::~CSceneGridWnd()
{
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(CSceneGridWnd::OnClose), NULL, this);
    m_pPropertyGrid->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CSceneGridWnd::OnPropertyChanged), NULL, this);
}

bool CSceneGridWnd::Show(bool bShow/* = true*/)
{
    bool bRet = false;
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    CEditorSceneWindow* pSceneWnd = pMainFrame->GetSceneWindow();
    CGridRenderObject* pGrid = pSceneWnd->GetGrid();
    if (pGrid != NULL)
    {
        if (bShow)
        {
            int nStartPosX, nStartPosY;
            pGrid->GetGridStartPos(nStartPosX, nStartPosY);
            m_pGridStartPosX->SetValue(nStartPosX);
            m_pGridStartPosY->SetValue(nStartPosY);
            uint32_t uGridWidth = pGrid->GetGridWidth();
            m_pGridWidth->SetValue((int)uGridWidth);
            uint32_t uGridHeight = pGrid->GetGridHeight();
            m_pGridHeight->SetValue((int)uGridHeight);
            float fGridDistance = pGrid->GetGridDistance();
            m_pGridDistance->SetValue(fGridDistance);
            float fLineWidth = pGrid->GetLineWidth();
            m_pGridLineWidth->SetValue(fLineWidth);
            uint32_t curFPS = CEngineCenter::GetInstance()->GetFPS();
            m_pFPS->SetValue((int)curFPS);
        }
        bRet = super::Show(bShow);
    }
    else
    {
        CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
        pMainFrame->GetAuiToolBarPerformPtr()->ToggleTool(ID_SettingBtn, false);
        pMainFrame->GetAuiToolBarPerformPtr()->Refresh(false);
    }
    return bRet;
}

void CSceneGridWnd::OnClose(wxCloseEvent& /*event*/)
{
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    pMainFrame->GetAuiToolBarPerformPtr()->ToggleTool(ID_SettingBtn, false);
    pMainFrame->GetAuiToolBarPerformPtr()->Refresh(false);
    CEditorConfig::GetInstance()->SaveToFile();
    this->Show(false);
}

void CSceneGridWnd::OnPropertyChanged(wxPropertyGridEvent &event)
{
    wxPGProperty* pChangedProperty = event.GetProperty();
    CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
    CEditorSceneWindow* pSceneWnd = pMainFrame->GetSceneWindow();
    CGridRenderObject* pGrid = pSceneWnd->GetGrid();
    if (pChangedProperty == m_pGridDistance)
    {
        pGrid->SetGridDistance((float)(m_pGridDistance->GetValue().GetDouble()));
    }
    else if (pChangedProperty == m_pGridLineWidth)
    {
        pGrid->SetLineWidth((float)(m_pGridLineWidth->GetValue().GetDouble()));
    }
    else if (pChangedProperty == m_pRenderPositiveDirectionLine)
    {
        pGrid->SetPositiveLineLength(m_pRenderPositiveDirectionLine->GetValue().GetBool() ? 1000.0f : 0);
    }
    else if (pChangedProperty == m_pRenderSpriteEdge)
    {
        CRenderManager::GetInstance()->m_bRenderSpriteEdge = m_pRenderSpriteEdge->GetValue().GetBool();
    }
    else if (pChangedProperty == m_pRenderSprite)
    {
        CRenderManager::GetInstance()->m_bRenderSprite = m_pRenderSprite->GetValue().GetBool();
    }
    else if (pChangedProperty == m_pRenderSpriteUserDefinePos)
    {
        CRenderManager::GetInstance()->m_bRenderSpriteUserDefinePos = m_pRenderSpriteUserDefinePos->GetValue().GetBool();
    }
    else if (pChangedProperty == m_pEnableSpriteDepthTest)
    {
        CRenderManager::GetInstance()->GetSpriteMaterial(true)->SetDepthTestEnable(m_pEnableSpriteDepthTest->GetValue().GetBool());
        CRenderManager::GetInstance()->GetSpriteMaterial(false)->SetDepthTestEnable(m_pEnableSpriteDepthTest->GetValue().GetBool());
    }
    else if (pChangedProperty == m_pBGColor)
    {
        wxColor colourValue;
        colourValue << m_pBGColor->GetValue();
        uint32_t uNewValue = (colourValue.Red() << 24) + (colourValue.Green() << 16) + (colourValue.Blue() << 8) + colourValue.Alpha();
        CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetViewport()->SetClearColor(uNewValue);
    }
    else if (pChangedProperty == m_pGridStartPosX ||
        pChangedProperty == m_pGridStartPosY)
    {
        pGrid->SetGridStartPos(m_pGridStartPosX->GetValue().GetLong(), m_pGridStartPosY->GetValue().GetLong());
    }
    else if (pChangedProperty == m_pGridWidth)
    {
        pGrid->SetGridWidth((uint32_t)(m_pGridWidth->GetValue().GetLong()));
    }
    else if (pChangedProperty == m_pGridHeight)
    {
        pGrid->SetGridHeight((uint32_t)(m_pGridHeight->GetValue().GetLong()));
    }
    else if (pChangedProperty == m_pFPS)
    {
        CEngineCenter::GetInstance()->SetFPS(m_pFPS->GetValue().GetInteger());
    }
    else if (pChangedProperty == m_pLanguage)
    {
        CLanguageManager::GetInstance()->SetCurrentLanguage((ELanguageType)(m_pLanguage->GetValue().GetLong()));
    }
    else if (pChangedProperty == m_pRenderMapGrid)
    {
        CRenderManager::GetInstance()->m_bRenderMapGrid = m_pRenderMapGrid->GetValue().GetBool();
    }
    else if (pChangedProperty == m_pRenderPathFindingGrid)
    {
        CRenderManager::GetInstance()->m_bRenderPathFindingGrid = m_pRenderPathFindingGrid->GetValue().GetBool();
    }
    else if (pChangedProperty == m_pEnableUpdateParticle)
    {
        CParticleManager::GetInstance()->m_bUpdateSwitcher = m_pEnableUpdateParticle->GetValue().GetBool();
    }
    else if (pChangedProperty == m_pEnableRenderParticle)
    {
        CParticleManager::GetInstance()->m_bRenderSwitcher = m_pEnableRenderParticle->GetValue().GetBool();
    }
    else if (m_pRenderParticleWireFrame == pChangedProperty)
    {
        CParticleManager::GetInstance()->m_bRenderParticleEdge = m_pRenderParticleWireFrame->GetValue().GetBool();
    }
    else if (pChangedProperty == m_pRenderBuilding)
    {
        CRenderManager::GetInstance()->m_bRenderBuilding = m_pRenderBuilding->GetValue().GetBool();
    }
    else if (pChangedProperty == m_pRenderSkeleton)
    {
        CRenderManager::GetInstance()->m_bRenderSkeletonDbgLine = m_pRenderSkeleton->GetValue().GetBool();
    }
    else if (pChangedProperty == m_pGlobalColorFactor)
    {
        CRenderManager::GetInstance()->SetGlobalColorFactor(m_pGlobalColorFactor->GetValue().GetDouble());
    }
    else if (pChangedProperty == m_pFontSize)
    {
        const std::map<TString, CFontFace *>& fontFaceMap = CFontManager::GetInstance()->GetFontFaceMap();
        for (auto iter = fontFaceMap.begin(); iter != fontFaceMap.end(); ++iter)
        {
            iter->second->SetFontSize(m_pFontSize->GetValue().GetDouble());
            iter->second->Clear();
        }
    }
    else if (pChangedProperty == m_pFontBorderSize)
    {
        const std::map<TString, CFontFace *>& fontFaceMap = CFontManager::GetInstance()->GetFontFaceMap();
        for (auto iter = fontFaceMap.begin(); iter != fontFaceMap.end(); ++iter)
        {
            iter->second->SetBorderWeight(m_pFontBorderSize->GetValue().GetDouble());
            iter->second->Clear();
        }
    }
}

void CSceneGridWnd::AddSpinCtrl(wxPGProperty* pProperty, float fStep /*=1.0f*/)
{
    pProperty->SetEditor(wxPGEditor_SpinCtrl);
    pProperty->SetAttribute(wxT("Step"), fStep);
    pProperty->SetAttribute( wxT("MotionSpin"), true );
}

void CSceneGridWnd::InitByConfig()
{
    SSceneGridParam* pSceneGridParam = CEditorConfig::GetInstance()->GetSceneGridParam();
    wxColour colourValue(pSceneGridParam->BGColor.r, pSceneGridParam->BGColor.g, pSceneGridParam->BGColor.b, pSceneGridParam->BGColor.a);
    wxString strColorValue = static_cast<wxColourProperty*>(m_pBGColor)->ColourToString(colourValue, wxNOT_FOUND, wxPG_FULL_VALUE);
    m_pBGColor->SetValueFromString(strColorValue);
    m_pPropertyGrid->ChangePropertyValue(m_pGridStartPosX, pSceneGridParam->iGridStartPosX);
    m_pPropertyGrid->ChangePropertyValue(m_pGridStartPosY, pSceneGridParam->iGridStartPosY);
    m_pPropertyGrid->ChangePropertyValue(m_pGridWidth, pSceneGridParam->iGridWidth);
    m_pPropertyGrid->ChangePropertyValue(m_pGridHeight, pSceneGridParam->iGridHeight);
    m_pPropertyGrid->ChangePropertyValue(m_pGridDistance, pSceneGridParam->fGridDistance);
    m_pPropertyGrid->ChangePropertyValue(m_pGridLineWidth, pSceneGridParam->fGridLineWidth);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderPositiveDirectionLine, pSceneGridParam->bRenderPositiveDirectionLine);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderMapGrid, pSceneGridParam->bRenderMapGrid);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderPathFindingGrid, pSceneGridParam->bRenderPathFindingGrid);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderBuilding, pSceneGridParam->bRenderBuilding);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderSprite, pSceneGridParam->bRenderSprite);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderSpriteEdge, pSceneGridParam->bRenderSpriteEdge);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderAttackRange, pSceneGridParam->bRenderAttackRange);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderSkeleton, pSceneGridParam->bRenderSkeleton);
    m_pPropertyGrid->ChangePropertyValue(m_pEnableSpriteDepthTest, pSceneGridParam->bEnableSpriteDepthTest);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderSpriteUserDefinePos, pSceneGridParam->bRenderSpriteUserDefinePos);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderGameObjectPosPoint, pSceneGridParam->bRenderGameObjectPosPoint);
    m_pPropertyGrid->ChangePropertyValue(m_pEnableUI, pSceneGridParam->bEnableUI);
    m_pPropertyGrid->ChangePropertyValue(m_pVolume, pSceneGridParam->fVolume);
    m_pPropertyGrid->ChangePropertyValue(m_pSfxVolume, pSceneGridParam->fSfxVolume);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderLandingshipSelectArea, pSceneGridParam->bRenderLandingshipSelectArea);
    m_pPropertyGrid->ChangePropertyValue(m_pEnableShakeCamera, pSceneGridParam->bEnableShakeCamera);
    m_pPropertyGrid->ChangePropertyValue(m_pEnableUpdateParticle, pSceneGridParam->bEnableUpdateParticle);
    m_pPropertyGrid->ChangePropertyValue(m_pEnableRenderParticle, pSceneGridParam->bEnableRenderParticle);
    m_pPropertyGrid->ChangePropertyValue(m_pRenderParticleWireFrame, pSceneGridParam->bRenderParticleWireFrame);

    m_pPropertyGrid->ChangePropertyValue(m_pFPS, (int)(pSceneGridParam->uFPS));
    m_pPropertyGrid->ChangePropertyValue(m_pLanguage, (int)(pSceneGridParam->uLanguage));
    m_pPropertyGrid->ChangePropertyValue(m_pGlobalColorFactor, pSceneGridParam->fGlobalColorFactor);

    m_pPropertyGrid->ChangePropertyValue(m_pFontSize, pSceneGridParam->fFontSize);
    m_pPropertyGrid->ChangePropertyValue(m_pFontBorderSize, pSceneGridParam->fFontBorderSize);
}

float CSceneGridWnd::GetGridDistanceValue() const
{
    return (float)(m_pGridDistance->GetValue().GetDouble());
}

float CSceneGridWnd::GetGridLineWidthValue() const
{
    return (float)(m_pGridLineWidth->GetValue().GetDouble());
}

bool CSceneGridWnd::GetRenderPositiveDirectionLineValue() const
{
    return m_pRenderPositiveDirectionLine->GetValue().GetBool();
}

bool CSceneGridWnd::GetRenderSpriteEdgeValue() const
{
    return m_pRenderSpriteEdge->GetValue().GetBool();
}

bool CSceneGridWnd::GetRenderSpriteValue() const
{
    return m_pRenderSprite->GetValue().GetBool();
}

bool CSceneGridWnd::GetRenderSpriteUserDefinePosValue() const
{
    return m_pRenderSpriteUserDefinePos->GetValue().GetBool();
}

bool CSceneGridWnd::GetEnableSpriteDepthTestValue() const
{
    return m_pEnableSpriteDepthTest->GetValue().GetBool();
}

bool CSceneGridWnd::GetEnableUIValue() const
{
    return m_pEnableUI->GetValue().GetBool();
}

float CSceneGridWnd::GetVolumeValue() const
{
    return m_pVolume->GetValue().GetDouble();
}

float CSceneGridWnd::GetSfxVolumeValue() const
{
    return m_pSfxVolume->GetValue().GetDouble();
}

uint32_t CSceneGridWnd::GetBGColorValue() const
{
    wxColor colourValue;
    colourValue << m_pBGColor->GetValue();
    return (colourValue.Red() << 24) + (colourValue.Green() << 16) + (colourValue.Blue() << 8) + colourValue.Alpha();
}

int CSceneGridWnd::GetGridStartPosXValue() const
{
    return (int)m_pGridStartPosX->GetValue().GetLong();
}

int CSceneGridWnd::GetGridStartPosYValue() const
{
    return (int)m_pGridStartPosY->GetValue().GetLong();
}

int CSceneGridWnd::GetGridWidthValue() const
{
    return (uint32_t)m_pGridWidth->GetValue().GetLong();
}

int CSceneGridWnd::GetGridHeightValue() const
{
    return (uint32_t)m_pGridHeight->GetValue().GetLong();
}

uint32_t CSceneGridWnd::GetFPSValue() const
{
    return m_pFPS->GetValue().GetInteger();
}

uint32_t CSceneGridWnd::GetLanguageValue() const
{
    return m_pLanguage->GetValue().GetLong();
}

bool CSceneGridWnd::GetRenderLandingshipSelectAreaValue() const
{
    return m_pRenderLandingshipSelectArea->GetValue().GetBool();
}

bool CSceneGridWnd::GetEnableShakeCameraValue() const
{
    return m_pEnableShakeCamera->GetValue().GetBool();
}

bool CSceneGridWnd::GetRenderMapGridValue() const
{
    return m_pRenderMapGrid->GetValue().GetBool();
}

bool CSceneGridWnd::GetRenderPathFindingGridValue() const
{
    return m_pRenderPathFindingGrid->GetValue().GetBool();
}

bool CSceneGridWnd::GetEnableUpdateParticleValue() const
{
    return m_pEnableUpdateParticle->GetValue().GetBool();
}

bool CSceneGridWnd::GetEnableRenderParticleValue() const
{
    return m_pEnableRenderParticle->GetValue().GetBool();
}

bool CSceneGridWnd::GetRenderParticleWireFrameValue() const
{
    return m_pRenderParticleWireFrame->GetValue().GetBool();
}

bool CSceneGridWnd::GetRenderBuildingValue() const
{
    return m_pRenderBuilding->GetValue().GetBool();
}

bool CSceneGridWnd::GetRenderGameObjectCenterPosPointValue() const
{
    return m_pRenderGameObjectPosPoint->GetValue().GetBool();
}

bool CSceneGridWnd::GetRenderAttackRangeValue() const
{
    return m_pRenderAttackRange->GetValue().GetBool();
}

bool CSceneGridWnd::GetRenderSkeleton() const
{
    return m_pRenderSkeleton->GetValue().GetBool();
}

float CSceneGridWnd::GetGlobalColorFactorValue() const
{
    return m_pGlobalColorFactor->GetValue().GetDouble();
}

float CSceneGridWnd::GetFontSize() const
{
    return m_pFontSize->GetValue().GetDouble();
}

float CSceneGridWnd::GetFontBorderSize() const
{
    return m_pFontBorderSize->GetValue().GetDouble();
}

