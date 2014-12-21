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
#include "SceneViewAgent.h"
#include "AniViewAgent.h"
#include "BeyondEngineEditorComponentWindow.h"
#include "wxDialogEditor.h"
#include "Resource/ResourceManager.h"
#include "GradientDialog.h"
#include "TexturePreviewDialog.h"
#include "Framework/Application.h"

#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Component/Component/ComponentProxyManager.h"
#include "EditPerformanceDialog.h"
#include "PropertyGridEditor.h"
#include "EditorConfig.h"
#include <tlhelp32.h>
#include "Utility/BeatsUtility/FilePathTool.h"
#include "wxSoundPreviewEditor.h"
#include "WaitingForProcessDialog.h"
#include "RandomPropertyDialog.h"
#include "MenuEditor.h"
#include "Component/Component/ComponentProject.h"

HWND BEYONDENGINE_HWND = NULL;

#define SPLASHWIDTH 800
#define SPLASHHEIGHT 600
#define DIALOGSIZE wxSize(480, 600)

CEngineEditor::CEngineEditor()
    : m_glRC(NULL)
    , m_pMainFrame(NULL)
    , m_pPtrButtonEditor(NULL)
    , m_pTextureEditor(NULL)
    , m_pGradientDialog(NULL)
    , m_pGradientColorEditor(NULL)
    , m_pGridEditor(NULL)
    , m_pSoundFileEditor(NULL)
    , m_pWaitingDialog(NULL)
    , m_pTexturePreviewDialog(NULL)
    , m_pRandomValueEditor(nullptr)
{
    m_pApplication = new CApplication;
}

CEngineEditor::~CEngineEditor()
{
    BEATS_SAFE_DELETE(m_glRC);
    BEATS_SAFE_DELETE(m_pWaitingDialog);
    BEATS_SAFE_DELETE(m_pTextureEditor);
    BEATS_SAFE_DELETE(m_pGradientDialog);
    BEATS_SAFE_DELETE(m_pGradientColorEditor);
    BEATS_SAFE_DELETE(m_pGridEditor);
}

void EngineLaunch();
void ExportLanguage();
void InitScanData()
{
    const TString& strSourceCodePath = CResourceManager::GetInstance()->GetResourcePath(eRT_SourceCode);
    std::vector<TString> scanPath;
    scanPath.push_back(strSourceCodePath);
    TString fullPathStr = CFilePathTool::GetInstance()->ParentPath(CUtilityManager::GetInstance()->GetModuleFileName().c_str());
    fullPathStr.append(_T("/")).append(ENGINE_ENUM_INFO_FILE);
    CEnumStrGenerator::GetInstance()->Init(scanPath, fullPathStr.c_str());
}
bool CEngineEditor::OnInit()
{
    // Close any other same process.
    PROCESSENTRY32 pe32 = { 0 };
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap != INVALID_HANDLE_VALUE)
    {
        if (Process32First(hProcessSnap, &pe32))
        {
            do
            {
                if (pe32.th32ProcessID != GetCurrentProcessId() &&
                    (_tcsicmp(pe32.szExeFile, _T("BeyondEngineEditor_d.exe")) == 0 ||
                    _tcsicmp(pe32.szExeFile, _T("BeyondEngineEditor.exe")) == 0))
                {
                    HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                    TerminateProcess(handle, 0);
                    CloseHandle(handle);
                }
            } while (Process32Next(hProcessSnap, &pe32));
        }
    }
    CloseHandle(hProcessSnap);
    // Set working path.
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    int iStrPos = CStringHelper::GetInstance()->FindLastString(szPath, _T("\\"), false);
    BEATS_ASSERT(iStrPos >= 0);
    szPath[iStrPos] = 0;
    m_strWorkingPath.assign(szPath);

    for (int32_t i = 1; i < argc; ++i)
    {
        if (_stricmp(argv[i], "language") == 0)
        {
            CLanguageManager::GetInstance()->LoadFromFile(eLT_Chinese);
            ExportLanguage();
            TString fullPathStr = CFilePathTool::GetInstance()->ParentPath(CUtilityManager::GetInstance()->GetModuleFileName().c_str());
            fullPathStr.append(_T("/")).append(ENGINE_ENUM_INFO_FILE);
            ::DeleteFile(fullPathStr.c_str());
            InitScanData();
            if (!CFilePathTool::GetInstance()->Exists(fullPathStr.c_str()))
            {
                BEATS_PRINT("Generate engine scan data failed!\n");
                _exit(1);
            }
            BEATS_PRINT("Engine scan data is generated!\n");
        }
        else if (_stricmp(argv[i], "export") == 0)
        {
            InitScanData();
            CLanguageManager::GetInstance()->SetCurrentLanguage(eLT_Chinese, true);
            //Open project bcp file.
            CEngineCenter::GetInstance()->m_bExportMode = true;
            EngineLaunch();
            CEngineCenter::GetInstance()->m_bExportMode = false;
            CComponentProxyManager::GetInstance()->InitializeAllTemplate();
            TString strWorkingDirectory = CFilePathTool::GetInstance()->ParentPath(CFilePathTool::GetInstance()->ParentPath(argv[0]).c_str());
            TString strBcfPath = strWorkingDirectory + "/resource/bcf/FCGame.bcp";
            CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
            std::map<uint32_t, std::vector<uint32_t>> conflictIdMap;
            pProject->LoadProject(strBcfPath.c_str(), conflictIdMap);
            BEATS_ASSERT(conflictIdMap.size() == 0);
            if (conflictIdMap.size() > 0)
            {
                BEATS_PRINT("detect component id conflict! please resolve them manualy!\n");
                _exit(1);
            }
            TString strAIDatabinPath = strWorkingDirectory + "/resource/" + BINARIZE_FILE_NAME;
            CComponentProxyManager::GetInstance()->Export(strAIDatabinPath.c_str(), [=](uint32_t /*uFileID*/, CComponentProxy* pProxy)
            {
                CComponentInstance* pInstance = (CComponentInstance*)CComponentInstanceManager::GetInstance()->GetComponentTemplate(pProxy->GetGuid());
                CComponentProxy* pOldProxy = pInstance->GetProxyComponent();
                pInstance->SetProxyComponent(nullptr);
                pInstance->SetProxyComponent(pProxy);
                bool bShouldSave = pInstance->OnExport();
                pInstance->SetProxyComponent(nullptr);
                pInstance->SetProxyComponent(pOldProxy);
                return bShouldSave;
            });
        }
        else if (_stricmp(argv[i], "android") == 0)
        {
            //HACK: pack android need wxImage so m_pMainFrame is needed to show
            InitScanData();
            CEditorConfig::GetInstance()->LoadFromFile();
            CLanguageManager::GetInstance()->SetCurrentLanguage(eLT_English);

            m_pMainFrame = new CEditorMainFrame(wxEmptyString);
            m_pMainFrame->InitFrame();
            m_pMainFrame->Show();
            m_pMainFrame->PostInitialize();

            m_pMainFrame->SetConvertPngTypeString("1");
            m_pMainFrame->ExportResourcePack(true);
            BEATS_SAFE_DELETE(m_pMainFrame);
        }
        else if (_stricmp(argv[i], "ios") == 0)
        {
            m_pMainFrame = new CEditorMainFrame(wxEmptyString);
            m_pMainFrame->SetConvertPngTypeString("2");
            m_pMainFrame->ExportResourcePack(true);
            BEATS_SAFE_DELETE(m_pMainFrame);
        }
        if (true)//Disable warning
        {
            _exit(9527);
        }
    }
    bool ret = wxApp::OnInit();
    if (ret)
    {
        InitScanData();
        CEditorConfig::GetInstance()->LoadFromFile();
        CLanguageManager::GetInstance()->SetCurrentLanguage(eLT_English);
        const TCHAR* pszFrameTitle = wxT("BeyondEngineEditor");
#ifdef _DEBUG
        pszFrameTitle = wxT("BeyondEngineEditor_d");
#endif
        m_pMainFrame = new CEditorMainFrame(pszFrameTitle);
        m_pMainFrame->InitFrame();
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
    BEYONDENGINE_HWND = m_pMainFrame->GetHWND();
    return ret;
}

int CEngineEditor::OnExitImpl()
{
    CApplication::GetInstance()->Uninitialize();
    CApplication::Destroy();
    CEditorConfig::Destroy();
    CVec2PropertyDescription::m_pXBasicPropertyInfo.Destroy();
    CVec2PropertyDescription::m_pYBasicPropertyInfo.Destroy();
    CVec3PropertyDescription::m_pZBasicPropertyInfo.Destroy();
    CVec4PropertyDescription::m_pWBasicPropertyInfo.Destroy();
    CAniViewAgent::Destroy();
    CGameViewAgent::Destroy();
    CSceneViewAgent::Destroy();
    CTerrainViewAgent::Destroy();
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

CGradientDialog* CEngineEditor::GetGradientDialog()
{
    if (m_pGradientDialog == NULL)
    {
        m_pGradientDialog = new CGradientDialog(NULL, wxID_ANY, wxT("GradientColor"), wxDefaultPosition, wxDefaultSize, wxCAPTION);
    }
    return m_pGradientDialog;
}

CTexturePreviewDialog* CEngineEditor::GetTexturePreviewDialog()
{
    if (m_pTexturePreviewDialog == NULL)
    {
        m_pTexturePreviewDialog = new CTexturePreviewDialog(NULL, wxID_ANY, wxT("Texture"), wxDefaultPosition, DIALOGSIZE, wxCLOSE_BOX | wxRESIZE_BORDER | wxCAPTION);
    }
    return m_pTexturePreviewDialog;
}

wxPGEditor* CEngineEditor::GetTextureEditor()
{
    if (!m_pTextureEditor)
    {
        wxDialog* pDialog = GetTexturePreviewDialog();
        m_pTextureEditor = wxPropertyGrid::RegisterEditorClass(new wxDialogEditor(pDialog));
    }
    return m_pTextureEditor;
}

wxPGEditor* CEngineEditor::GetGradientColorEditor()
{
    if (!m_pGradientColorEditor)
    {
        wxDialog* pDialog = GetGradientDialog();
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

wxPGEditor* CEngineEditor::GetSoundFileEditor()
{
    if (!m_pSoundFileEditor)
    {
        m_pSoundFileEditor = wxPropertyGrid::RegisterEditorClass(new wxSoundPreviewEditor());
    }
    return m_pSoundFileEditor;
}

wxPGEditor* CEngineEditor::GetRandomValueEditor()
{
    if (!m_pRandomValueEditor)
    {
        wxDialog* pDialog = new CRandomPropertyDialog(NULL, wxID_ANY, wxT("RandomValue"), wxDefaultPosition, DIALOGSIZE, wxCLOSE_BOX | wxRESIZE_BORDER | wxCAPTION);
        m_pRandomValueEditor = wxPropertyGrid::RegisterEditorClass(new wxDialogEditor(pDialog));
    }
    return m_pRandomValueEditor;
}

wxPGEditor* CEngineEditor::GetMenuEditor()
{
    if (!m_pMenuEditor)
    {
        m_pMenuEditor = wxPropertyGrid::RegisterEditorClass(new CMenuEditor());
    }
    return m_pMenuEditor;
}

CWaitingForProcessDialog* CEngineEditor::GetWaitingDialog()
{
    if (!m_pWaitingDialog)
    {
        m_pWaitingDialog = new CWaitingForProcessDialog(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCAPTION);
    }
    return m_pWaitingDialog;
}

void CEngineEditor::RegisterPGEditor(wxPGEditorBase* pPGEditor)
{
    BEATS_ASSERT(m_editorRegisterSet.find(pPGEditor) == m_editorRegisterSet.end());
    m_editorRegisterSet.insert(pPGEditor);
}

void CEngineEditor::UnRegisterPGEditor(wxPGEditorBase* pPGEditor)
{
    if (m_editorRegisterSet.find(pPGEditor) != m_editorRegisterSet.end())
    {
        m_editorRegisterSet.erase(pPGEditor);
    }
}

const std::set<const wxPGEditor*>& CEngineEditor::GetEditorRegisterSet()
{
    return m_editorRegisterSet;
}

IMPLEMENT_APP(CEngineEditor)