#include "stdafx.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "EngineSplash.h"
#include "wxPtrButtonEditor.h"
#include "ComponentGraphics_GL.h"
#include "Vec2fPropertyDescription.h"
#include "Vec3fPropertyDescription.h"
#include "Vec4fPropertyDescription.h"
#include "TerrainViewAgent.h"
#include "GameViewAgent.h"
#include "EffectViewAgent.h"
#include "UIViewAgent.h"
#include "SceneViewAgent.h"
#include "AniViewAgent.h"
#include "BeyondEngineEditorComponentWindow.h"
#include "wxDialogEditor.h"
#include "Resource/ResourcePathManager.h"
#include "GradientDialog.h"
#include "TexturePreviewDialog.h"
#include "Framework/Application.h"

#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProxyManager.h"
#include "EditPerformanceDialog.h"
#include "PropertyGridEditor.h"
#include "EditorConfig.h"

#define SPLASHWIDTH 800
#define SPLASHHEIGHT 600
#define DIALOGSIZE wxSize(480, 600)

CEngineEditor::CEngineEditor()
    : m_glRC(NULL)
    , m_pMainFrame(NULL)
    , m_pPtrButtonEditor(NULL)
    , m_pTextureEditor(NULL)
    , m_pGradientColorEditor(NULL)
    , m_pGridEditor(NULL)
{

}

CEngineEditor::~CEngineEditor()
{
    BEATS_SAFE_DELETE(m_glRC);
}

bool CEngineEditor::OnInit()
{
    bool ret = wxApp::OnInit();
    if ( ret )
    {
        // Set working path.
        TCHAR szPath[MAX_PATH];
        GetModuleFileName(NULL, szPath, MAX_PATH);
        int iStrPos = CStringHelper::GetInstance()->FindLastString(szPath, _T("\\"), false);
        BEATS_ASSERT(iStrPos >= 0);
        szPath[iStrPos] = 0;
        m_strWorkingPath.assign(szPath);
        CEditorConfig::GetInstance()->LoadFromFile();
        CLanguageManager::GetInstance()->LoadFromFile(eLT_English);
        m_pMainFrame = new CEditorMainFrame(wxT("BeyondEngineEditor"));
        m_pMainFrame->InitFrame();
        const TString& strSourceCodePath = CResourcePathManager::GetInstance()->GetResourcePath(CResourcePathManager::eRPT_SourceCode);
        std::vector<TString> scanPath;
        scanPath.push_back(strSourceCodePath);

        CEnumStrGenerator::GetInstance()->Init(scanPath, ENGINE_ENUM_INFO_FILE);
#ifndef _DEBUG
        m_pMainFrame->Freeze();
        wxBitmap bmp;
        if(bmp.LoadFile(wxT("../resource/splash.png"), wxBITMAP_TYPE_PNG))
        {
            wxImage image = bmp.ConvertToImage();
            image.Rescale(SPLASHWIDTH, SPLASHHEIGHT);
            wxBitmap splashBitmap(image);
            CEngineSplash* pSplash = new CEngineSplash(splashBitmap,
                wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
                3000, m_pMainFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                wxSIMPLE_BORDER|wxSTAY_ON_TOP);
            pSplash->Instance();
        }
        else
        {
            m_pMainFrame->Thaw();
#endif
            m_pMainFrame->Show(true);
            // HACK: we can only do some initialize logic after the opengl is initialized.
            // We initialize OPENGL in m_pViewScreen->SetContextToCurrent() of Show() That's why we call PostInitialize here.
            m_pMainFrame->PostInitialize();
#ifndef _DEBUG
            m_pMainFrame->StartRenderUpdate();//Debug model use OnIdle instead
        }
#endif
    }
    return ret;
}
int CEngineEditor::OnExit()
{
    wxApp::OnExit();
    CEditorConfig::Destroy();
    CVec2PropertyDescription::m_pXBasicPropertyInfo.Destroy();
    CVec2PropertyDescription::m_pYBasicPropertyInfo.Destroy();
    CVec3PropertyDescription::m_pZBasicPropertyInfo.Destroy();
    CVec4PropertyDescription::m_pWBasicPropertyInfo.Destroy();
    CAniViewAgent::Destroy();
    CEffectViewAgent::Destroy();
    CGameViewAgent::Destroy();
    CSceneViewAgent::Destroy();
    CUIViewAgent::Destroy();
    CTerrainViewAgent::Destroy();
    CApplication::GetInstance()->Uninitialize();
    CApplication::Destroy();
    return 0;
}

wxPGEditor* CEngineEditor::GetPtrEditor()
{
    if (!m_pPtrButtonEditor)
    {
        m_pPtrButtonEditor = wxPropertyGrid::RegisterEditorClass(new wxPtrButtonEditor());
    }
    return m_pPtrButtonEditor;
}

CEditorMainFrame* CEngineEditor::GetMainFrame() const
{
    return m_pMainFrame;
}

wxGLContext* CEngineEditor::CreateGLContext(wxGLCanvas* pCanvas)
{
    m_glRC = new wxGLContext(pCanvas);
    return m_glRC;
}

wxGLContext* CEngineEditor::GetGLContext() const
{
    return m_glRC;
}

const TString& CEngineEditor::GetWorkingPath()
{
    return m_strWorkingPath;
}

wxPGEditor* CEngineEditor::GetTextureEditor()
{
    if (!m_pTextureEditor)
    {
        wxDialog* pDialog = new CTexturePreviewDialog(NULL, wxID_ANY, wxT("Texture"), wxDefaultPosition, DIALOGSIZE, wxCLOSE_BOX | wxRESIZE_BORDER | wxCAPTION);
        m_pTextureEditor = wxPropertyGrid::RegisterEditorClass(new wxDialogEditor(pDialog));
    }
    return m_pTextureEditor;
}

wxPGEditor* CEngineEditor::GetGradientColorEditor()
{
    if (!m_pGradientColorEditor)
    {
        wxDialog* pDialog = new CGradientDialog(NULL, wxID_ANY, wxT("GradientColor"), wxDefaultPosition, wxDefaultSize, wxCLOSE_BOX | wxCAPTION);
        m_pGradientColorEditor = wxPropertyGrid::RegisterEditorClass(new wxDialogEditor(pDialog));
    }
    return m_pGradientColorEditor;
}

wxPGEditor* CEngineEditor::GetGridEditor()
{
    if (!m_pGridEditor)
    {
        wxDialog* pDialog = new CPropertyGridEditor(NULL, wxID_ANY, wxT("Grid"), wxDefaultPosition, DIALOGSIZE, wxCLOSE_BOX | wxRESIZE_BORDER | wxCAPTION);
        m_pGridEditor = wxPropertyGrid::RegisterEditorClass(new wxDialogEditor(pDialog));
    }
    return m_pGridEditor;
}


IMPLEMENT_APP(CEngineEditor)