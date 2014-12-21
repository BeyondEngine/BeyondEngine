#include "stdafx.h"
#include "SoundFilePropertyDescription.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Component/Component/ComponentProxyManager.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Component/ComponentPublic.h"
#include "SoundFileProperty.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "EngineEditor.h"
#include "Resource/ResourceManager.h"
#include "EditorMainFrame.h"

static const TString EMPTY_STRING = _T("Empty");

CSoundFilePropertyDescription::CSoundFilePropertyDescription(CSerializer* pSerializer)
: CFilePropertyDescription(pSerializer)
{
    SetType(eRPT_SoundFile);
}

CSoundFilePropertyDescription::CSoundFilePropertyDescription(const CSoundFilePropertyDescription& rRef)
: CFilePropertyDescription(rRef)
{
    SetType(eRPT_SoundFile);
}

CSoundFilePropertyDescription::~CSoundFilePropertyDescription()
{
}

CSoundFilePropertyDescription* CSoundFilePropertyDescription::CreateNewInstance()
{
    CSoundFilePropertyDescription* pNewProperty = new CSoundFilePropertyDescription(*this);
    return pNewProperty;
}

wxPGProperty* CSoundFilePropertyDescription::CreateWxProperty()
{
    CSoundFileProperty* pProperty = new CSoundFileProperty(wxPG_LABEL, wxPG_LABEL, *(TString*)(m_valueArray[eVT_CurrentValue]));
    pProperty->SetClientData(this);
    wxVariant var(((TString*)m_valueArray[eVT_DefaultValue])->c_str());
    pProperty->SetDefaultValue(var);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    TString strValue = _T("../Resource/Audio");
    strValue = CFilePathTool::GetInstance()->FileFullPath(strValue.c_str());
    m_strInitialPath = strValue + "/";
    TCHAR szInitPath[MAX_PATH];
    CFilePathTool::GetInstance()->Canonical(szInitPath, m_strInitialPath.c_str());
    m_strInitialPath = CFilePathTool::GetInstance()->ConvertToWindowsPath(szInitPath);
    return pProperty;
}

void CSoundFilePropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    const TString& strFileName = *(TString*)m_valueArray[eValueType];
    BEATS_ASSERT(strFileName.find('\\') == TString::npos);
    serializer << strFileName;
    if (!strFileName.empty())
    {
        if (CComponentProxyManager::GetInstance()->IsExporting() && CComponentProxyManager::GetInstance()->IsCheckUselessResource())
        {
            CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
            TString strFullPath = CResourceManager::GetInstance()->GetFullPath(strFileName, eRT_Audio);
            pMainFrame->AddExportFileFullPathList(strFullPath);
        }
    }
}