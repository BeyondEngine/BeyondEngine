#include "stdafx.h"
#include "EditorConfig.h"
#include "TinyXML/tinyxml.h"
#include "EngineEditor.h"
#include "Utility/BeatsUtility/FilePathTool.h"

CEditorConfig* CEditorConfig::m_pInstance = NULL;

#define CONFIG_FILE_NAME _T("EditorConfig.xml")

CEditorConfig::CEditorConfig()
    : m_bShowAboutDlgAfterLaunch(true)
    , m_pConfigXML(NULL)
    , m_currLanguage(eLT_English)
{
    TString strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
    strWorkingPath.append(_T("\\")).append(CONFIG_FILE_NAME);
    m_pConfigXML = new TiXmlDocument(strWorkingPath.c_str());

    bool loadSuccess = m_pConfigXML->LoadFile(TIXML_ENCODING_LEGACY);
    BEATS_WARNING(loadSuccess, _T("Load config file %s faled!"), CONFIG_FILE_NAME);
    BEYONDENGINE_UNUSED_PARAM(loadSuccess);
}

CEditorConfig::~CEditorConfig()
{
    BEATS_SAFE_DELETE(m_pConfigXML);
}

void CEditorConfig::LoadFromFile()
{
    TiXmlElement* pRootElement = m_pConfigXML->RootElement();
    if (pRootElement)
    {
        TiXmlElement* pConfigNode = pRootElement->FirstChildElement("Config");
        if (pConfigNode)
        {
            m_strLastOpenProject = pConfigNode->Attribute("LastOpenProject");
            m_strLastOpenFile = pConfigNode->Attribute("LastOpenFile");
            pConfigNode->Attribute("Language", (int*)&m_currLanguage);
            int nShowDlg = 0;
            pConfigNode->Attribute("ShowAboutDlg", &nShowDlg);
            m_bShowAboutDlgAfterLaunch = nShowDlg != 0;
        }
    }
}

void CEditorConfig::SaveToFile()
{
    TiXmlDocument document;
    TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","","");
    document.LinkEndChild(pDeclaration);
    TiXmlElement* pRootElement = new TiXmlElement("Root");
    document.LinkEndChild(pRootElement);

    TiXmlElement* ConfigNode = new TiXmlElement("Config");
    ConfigNode->SetAttribute("LastOpenProject", m_strLastOpenProject.c_str());
    ConfigNode->SetAttribute("LastOpenFile", m_strLastOpenFile.c_str());
    ConfigNode->SetAttribute("Language", m_currLanguage);
    ConfigNode->SetAttribute("ShowAboutDlg", m_bShowAboutDlgAfterLaunch ? 1 : 0);
    pRootElement->LinkEndChild(ConfigNode);
    TString strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
    strWorkingPath.append(_T("\\")).append(CONFIG_FILE_NAME);
    document.SaveFile(strWorkingPath.c_str());
}

bool CEditorConfig::IsShowAboutDlgAfterLaunch() const
{
    return m_bShowAboutDlgAfterLaunch;
}

void CEditorConfig::SetShowAboutDlgAfterlaunch(bool bFlag)
{
    m_bShowAboutDlgAfterLaunch = bFlag;
    CEditorConfig::GetInstance()->SaveToFile();
}

const TString& CEditorConfig::GetLastOpenProject() const
{
    return m_strLastOpenProject;
}

void CEditorConfig::SetLastOpenProject(const TCHAR* pszProjectPath)
{
    const TString& strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
    m_strLastOpenProject = CFilePathTool::GetInstance()->MakeRelative(strWorkingPath.c_str(), pszProjectPath);
    CEditorConfig::GetInstance()->SaveToFile();
}

const TString& CEditorConfig::GetLastOpenFile() const
{
    return m_strLastOpenFile;
}

void CEditorConfig::SetLastOpenFile(const TCHAR* pszFilePath)
{
    const TString& strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
    m_strLastOpenFile = CFilePathTool::GetInstance()->MakeRelative(strWorkingPath.c_str(), pszFilePath);
    CEditorConfig::GetInstance()->SaveToFile();
}

ELanguage CEditorConfig::GetCurrLanguage() const
{
    return m_currLanguage;
}

void CEditorConfig::SetCurrLanguage(ELanguage language)
{
    m_currLanguage = language;
    CEditorConfig::GetInstance()->SaveToFile();
}