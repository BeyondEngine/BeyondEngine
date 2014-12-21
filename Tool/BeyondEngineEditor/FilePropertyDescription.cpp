#include "stdafx.h"
#include "FilePropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "Component/ComponentPublic.h"
#include <wx/propgrid/propgrid.h>
#include "Resource/Resource.h"
#include "Resource/ResourceManager.h"
#include "Render/Model.h"
#include "Render/Skin.h"
#include "Render/StaticMesh.h"
#include "EditorMainFrame.h"
#include "EngineEditor.h"
#include "Render/StaticMeshData.h"
#include "Render/Material.h"
#include "Render/RenderBatch.h"
#include "render/Texture.h"

CFilePropertyDescription::CFilePropertyDescription(CSerializer* pSerializer)
    : super(eRPT_File)
{
    TString strValue;
    if (pSerializer != NULL)
    {
        (*pSerializer) >> strValue;
    }
    InitializeValue<TString>(strValue);
}

CFilePropertyDescription::CFilePropertyDescription(const CFilePropertyDescription& rRef)
    : super(rRef)
{
    TString strValue;
    InitializeValue<TString>(strValue);
    m_attributeMap = rRef.m_attributeMap;
}

CFilePropertyDescription::~CFilePropertyDescription()
{
    DestroyValue<TString>();
}

bool CFilePropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& result)
{
    std::vector<TString> cache;
    for (uint32_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache);
        BEATS_ASSERT(cache.size() == 2);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            std::vector<TString> fileAttribute;
            CStringHelper::GetInstance()->SplitString(cache[1].c_str(), _T("@"), fileAttribute);
            BEATS_ASSERT(fileAttribute.size() % 2 == 0);
            for (uint32_t j = 0; j < fileAttribute.size(); j+=2)
            {
                const TString& attributeName = fileAttribute[j];
                const TString& attributeValue = fileAttribute[j + 1];
                BEATS_ASSERT(m_attributeMap.find(attributeName) == m_attributeMap.end());
                m_attributeMap[attributeName] = attributeValue;
            }
        }
    }

    return true;
}

wxPGProperty* CFilePropertyDescription::CreateWxProperty()
{
    wxFileProperty* pProperty = new wxFileProperty(wxPG_LABEL, wxPG_LABEL, *(TString*)(m_valueArray[eVT_CurrentValue]));
    pProperty->SetClientData(this);
    wxVariant var(((TString*)m_valueArray[eVT_DefaultValue])->c_str());
    pProperty->SetDefaultValue(var);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    for (auto iter = m_attributeMap.begin(); iter != m_attributeMap.end(); ++iter)
    {
        TString strValue = iter->second;
        if (iter->first == _T("InitialPath"))
        {
            strValue = CFilePathTool::GetInstance()->FileFullPath(iter->second.c_str());
            m_strInitialPath = strValue + "/";
            TCHAR szInitPath[MAX_PATH];
            CFilePathTool::GetInstance()->Canonical(szInitPath, m_strInitialPath.c_str());
            m_strInitialPath = CFilePathTool::GetInstance()->ConvertToWindowsPath(szInitPath);
        }
        pProperty->SetAttribute(iter->first, strValue);
    }
    return pProperty;
}

void CFilePropertyDescription::SetValue(wxVariant& value, bool bSaveValue /*= true*/)
{
    TString strNewValue = CFilePathTool::GetInstance()->FileName(value.GetString().c_str());
    strNewValue = CStringHelper::GetInstance()->ToLower(strNewValue);
    if (!m_strInitialPath.empty())
    {
        TString strRelativePath = CFilePathTool::GetInstance()->MakeRelative(m_strInitialPath.c_str(), value.GetString().c_str());
        TCHAR szInitPath[MAX_PATH];
        CFilePathTool::GetInstance()->Canonical(szInitPath, strRelativePath.c_str());
        strNewValue = szInitPath;
    }
    std::replace(strNewValue.begin(), strNewValue.end(), '\\', '/');
    SetValueWithType(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&strNewValue, eVT_SavedValue);
    }
}

bool CFilePropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    bool bRet = *(TString*)pTargetValue != *(TString*)pSourceValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *(TString*)pSourceValue;
    }
    return bRet;
}

bool CFilePropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = _tcscmp(((TString*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)])->c_str(), ((TString*)m_valueArray[eVT_CurrentValue])->c_str()) == 0;
    return bRet;
}

CPropertyDescriptionBase* CFilePropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CFilePropertyDescription(*this);
    return pNewProperty;
}

void CFilePropertyDescription::GetValueAsChar( EValueType type, char* pOut ) const
{
    TString* pStr = (TString*)m_valueArray[type];
    _tcscpy(pOut, pStr->c_str());
}

bool CFilePropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    TString strValue = CStringHelper::GetInstance()->ToLower(pIn);
    std::replace(strValue.begin(), strValue.end(), '\\', '/');
    ((TString*)pOutValue)->assign(strValue);
    return true;
}

void CFilePropertyDescription::Serialize(CSerializer& serializer, EValueType eValueType /*= eVT_SavedValue*/)
{
    const TString& strFileName = *(TString*)m_valueArray[eValueType];
    BEATS_ASSERT(strFileName.find('\\') == TString::npos);
    serializer << strFileName;
    if (CComponentProxyManager::GetInstance()->IsExporting() && CComponentProxyManager::GetInstance()->IsCheckUselessResource())
    {
        bool bFileExists = false;
        CEditorMainFrame* pMainFrame = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetMainFrame();
        if (m_pOwner != nullptr && !strFileName.empty())
        {
            TString strFullPath;
            CResource* pResource = dynamic_cast<CResource*>(m_pOwner);
            if (pResource != nullptr)
            {
                strFullPath = CResourceManager::GetInstance()->GetFullPath(strFileName, pResource->GetType());
            }
            else if (m_pOwner->GetGuid() == CModel::REFLECT_GUID || CFilePathTool::GetInstance()->Extension(strFileName.c_str()) == ".model")
            {
                strFullPath = CResourceManager::GetInstance()->GetFullPath(strFileName, eRT_Model);
                bFileExists = CFilePathTool::GetInstance()->Exists(strFullPath.c_str());
                if (bFileExists)
                {
                    CSerializer modeldata;
                    CFilePathTool::GetInstance()->LoadFile(&modeldata, strFullPath.c_str(), _T("rb"));
                    BEATS_ASSERT(modeldata.GetWritePos() > 0, _T("Read empty file %s !"), strFullPath.c_str());
                    CModelData tmpModelData;
                    CSkin* pSkin = tmpModelData.GetSkin();
                    pSkin->Load(modeldata);
                    const std::map<std::string, SSkinBatchInfo>& batchInfoMap = pSkin->GetBatchInfoMap();
                    for (auto iter = batchInfoMap.begin(); iter != batchInfoMap.end(); ++iter)
                    {
                        TString strTextureFullPath = CResourceManager::GetInstance()->GetFullPath(iter->first, eRT_Texture);
                        pMainFrame->AddExportFileFullPathList(strTextureFullPath);
                    }
                }
            }
            else if (m_pOwner->GetGuid() == CStaticMesh::REFLECT_GUID)
            {
                strFullPath = CResourceManager::GetInstance()->GetFullPath(strFileName, eRT_StaticMesh);
                bFileExists = CFilePathTool::GetInstance()->Exists(strFullPath.c_str());
                if (bFileExists)
                {
                    CStaticMeshData tmpdata;
                    tmpdata.SetFilePath(strFullPath);
                    tmpdata.Load();
                    const std::vector<SSubMesh*>& subMeshes = tmpdata.GetSubMeshes();
                    for (size_t i = 0; i < subMeshes.size(); ++i)
                    {
                        CRenderBatch* pRenderBatch = subMeshes[i]->m_pRenderBatch;
                        if (pRenderBatch != nullptr)
                        {
                            const std::map<unsigned char, SharePtr<CTexture> >& textureMap = pRenderBatch->GetTextureMap();
                            for (auto iter = textureMap.begin(); iter != textureMap.end(); ++iter)
                            {
                                pMainFrame->AddExportFileFullPathList(iter->second->GetFilePath());
                            }
                        }
                    }
                    tmpdata.Unload();
                }
            }
            else
            {
                BEATS_ASSERT(false, "Never reach here!");
            }
            BEATS_ASSERT(bFileExists, "File %s doesn't exist! Check component %s with id %d!", strFullPath.c_str(), m_pOwner->GetClassStr(), m_pOwner->GetId());
            pMainFrame->AddExportFileFullPathList(strFullPath);
        }
    }
}

void CFilePropertyDescription::Deserialize(CSerializer& serializer, EValueType eValueType /*= eVT_CurrentValue*/)
{
    TString& strValue = *(TString*)m_valueArray[eValueType];
    serializer >> strValue;
    BEATS_ASSERT(strValue.find('\\') == TString::npos);
}
