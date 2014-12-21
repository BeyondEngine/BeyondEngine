#include "stdafx.h"
#include "LanguageManager.h"
#include "Resource/ResourceManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "LanguageText.h"
#include "RapidXML/rapidxml_print.hpp"
#ifdef EDITOR_MODE
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#endif

CLanguageManager* CLanguageManager::m_pInstance = NULL;

CLanguageManager::CLanguageManager()
: m_currLanguageType(eLT_Chinese)
{

}

CLanguageManager::~CLanguageManager()
{

}

const TString& CLanguageManager::GetUtf8Text(ELanguageTextType textid)  const
{
#ifdef EDITOR_MODE
    const std::vector<SEnumData*>* pEnumDataList = nullptr;
    CEnumStrGenerator::GetInstance()->GetEnumValueData("ELanguageTextType", pEnumDataList);
    TString strEnumStr;
    if (pEnumDataList != nullptr)
    {
        for (size_t i = 0; i < pEnumDataList->size(); ++i)
        {
            if (pEnumDataList->at(i)->m_value == textid)
            {
                strEnumStr = pEnumDataList->at(i)->m_str;
                break;
            }
        }
    }
    if (!strEnumStr.empty())
    {
        auto languageIter = m_languageMap.find(strEnumStr);
        if (languageIter != m_languageMap.end())
        {
            auto realIter = languageIter->second.find(m_currLanguageType);
            if (realIter != languageIter->second.end())
            {
                return realIter->second;
            }
        }
    }
    static TString UNKNOWN_STRING;
    TCHAR szBuffer[64];
    _stprintf(szBuffer, "UnknownString: %d", textid);
    UNKNOWN_STRING = szBuffer;
    return UNKNOWN_STRING;
#else
    auto iter = m_texts.find(textid);
    if (iter != m_texts.end())
    {
        return iter->second;
    }
    else
    {
        static TString UNKNOWN_STRING;
        TCHAR szBuffer[64];
        _stprintf(szBuffer, "UnknownString: %d", textid);
        UNKNOWN_STRING = szBuffer;
        return UNKNOWN_STRING;
    }
#endif
}

#if (PLATFORM_WIN32 == BEYONDENGINE_PLATFORM)
TString CLanguageManager::GetTText(ELanguageTextType textid) const
{
    return CStringHelper::GetInstance()->Utf8ToString(GetUtf8Text(textid).c_str());
}
#endif

void CLanguageManager::RegisterLanguageText(CLanguageText* pText)
{
    m_mutex.lock();
    BEATS_ASSERT(m_refreshLanguageTextList.find(pText) == m_refreshLanguageTextList.end());
    m_refreshLanguageTextList.insert(pText);
    m_mutex.unlock();
}

void CLanguageManager::UnregisterLanguageText(CLanguageText* pText)
{
    m_mutex.lock();

#ifdef DEVELOP_VERSION
    if (m_refreshLanguageTextList.find(pText) == m_refreshLanguageTextList.end())
    {
        BEATS_ASSERT(!CApplication::GetInstance() || CApplication::GetInstance()->IsDestructing(), _T("Language unregister failed only allowed in destructing phase."));
    }
#endif
    m_refreshLanguageTextList.erase(pText);
    m_mutex.unlock();
}

void CLanguageManager::SetCurrentLanguage(ELanguageType type, bool bForceLoad)
{
    if (type != m_currLanguageType || bForceLoad)
    {
        m_currLanguageType = type;
        LoadFromFile(type);
        for (auto iter = m_refreshLanguageTextList.begin(); iter != m_refreshLanguageTextList.end(); ++iter)
        {
            (*iter)->RefreshValueString();
        }
    }
}

ELanguageType CLanguageManager::GetCurrentLanguage() const
{
    return m_currLanguageType;
}

#ifdef EDITOR_MODE
std::map<TString, TString>& CLanguageManager::GetLanguageTagMap()
{
    return m_languageTagMap;
}

std::map<TString, std::map<ELanguageType, TString> >& CLanguageManager::GetLanguageMap()
{
    return m_languageMap;
}

std::set<TString>& CLanguageManager::GetExportLanguageList()
{
    return m_exportLanguageList;
}

void CLanguageManager::SaveLanguageListToFile()
{
    std::map<TString, std::map<ELanguageType, TString> >& languageMap = CLanguageManager::GetInstance()->GetLanguageMap();
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* pLanguageRootNode = doc.allocate_node(rapidxml::node_element, "Language");
    doc.append_node(pLanguageRootNode);
    TCHAR szBuffer[256];
    _stprintf(szBuffer, "%d", languageMap.size());
    pLanguageRootNode->append_attribute(doc.allocate_attribute("Count", doc.allocate_string(szBuffer)));
    for (auto iter = languageMap.begin(); iter != languageMap.end(); ++iter)
    {
        rapidxml::xml_node<>* pLanguageNode = doc.allocate_node(rapidxml::node_element, "LanguageNode");
        pLanguageNode->append_attribute(doc.allocate_attribute("Enum", doc.allocate_string(iter->first.c_str())));
        TString strTagValue;
        if (m_languageTagMap.find(iter->first) != m_languageTagMap.end())
        {
            strTagValue = m_languageTagMap[iter->first];
        }
        pLanguageNode->append_attribute(doc.allocate_attribute("Tag", doc.allocate_string(strTagValue.c_str())));
        for (int nCounter = 0; nCounter < eLT_Count; ++nCounter)
        {
            auto subIter = iter->second.find((ELanguageType)nCounter);
            if (subIter != iter->second.end() && subIter->second.length() > 0)
            {
                rapidxml::xml_node<>* pLanguageTypeNode = doc.allocate_node(rapidxml::node_element, pszLanguageTypeString[nCounter]);
                pLanguageTypeNode->append_attribute(doc.allocate_attribute("Value", doc.allocate_string(subIter != iter->second.end() ? subIter->second.c_str() : "")));
                pLanguageNode->append_node(pLanguageTypeNode);
            }
        }
        pLanguageRootNode->append_node(pLanguageNode);
    }
    TString filePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Resource);
    filePath.append(_T("/LanguageConfig.xml"));
    TString strOut;
    rapidxml::print(std::back_inserter(strOut), doc, 0);
    std::ofstream out(filePath.c_str());
    out << strOut;
    out.close();
}

#endif

void CLanguageManager::LoadFromFile(ELanguageType language)
{
#ifdef EDITOR_MODE
    BEYONDENGINE_UNUSED_PARAM(language);
    m_languageMap.clear();
    TString filePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Resource);
    filePath.append(_T("/LanguageConfig.xml"));
    if (CFilePathTool::GetInstance()->Exists(filePath.c_str()))
    {
        rapidxml::file<> fdoc(filePath.c_str());
        rapidxml::xml_document<> doc;
        try
        {
            doc.parse<rapidxml::parse_default>(fdoc.data());
            doc.m_pszFilePath = filePath.c_str();
        }
        catch (rapidxml::parse_error &e)
        {
            TCHAR info[MAX_PATH];
            _stprintf(info, _T("Load file :%s Failed! error :%s"), filePath.c_str(), e.what());
            MessageBox(BEYONDENGINE_HWND, info, _T("Load File Failed"), MB_OK | MB_ICONERROR);
        }
        uint32_t uCounter = 0;
        rapidxml::xml_node<>* pRootNode = doc.first_node("Language");
        rapidxml::xml_node<>* pLanguageNode = pRootNode->first_node("LanguageNode");
        while (pLanguageNode != nullptr)
        {
            uCounter++;
            TString strEnum = pLanguageNode->first_attribute("Enum")->value();
            BEATS_ASSERT(!strEnum.empty());
            if (pLanguageNode->first_attribute("Tag"))
            {
                TString strTag = pLanguageNode->first_attribute("Tag")->value();
                m_languageTagMap[strEnum] = strTag;
            }
            BEATS_ASSERT(m_languageMap.find(strEnum) == m_languageMap.end());
            std::map<ELanguageType, TString>& curMap = m_languageMap[strEnum];
            rapidxml::xml_node<>* pLanguageValueNode = pLanguageNode->first_node();
            while (pLanguageValueNode != nullptr)
            {
                TString languageTypeStr = pLanguageValueNode->name();
                ELanguageType languageType = eLT_Count;
                for (int j = 0; j < eLT_Count; ++j)
                {
                    if (pszLanguageTypeString[j] == languageTypeStr)
                    {
                        languageType = (ELanguageType)j;
                        break;
                    }
                }
                BEATS_ASSERT(curMap.find(languageType) == curMap.end());
                const TCHAR* pszValue = pLanguageValueNode->first_attribute("Value")->value();
                BEATS_ASSERT(_tcslen(pszValue) > 0);
                curMap[languageType] = pszValue;
                pLanguageValueNode = pLanguageValueNode->next_sibling();
            }
            pLanguageNode = pLanguageNode->next_sibling();
        }
        BEATS_ASSERT((uint32_t)_ttoi(pRootNode->first_attribute("Count")->value()) == uCounter);
    }
#else
    TString filePath = CResourceManager::GetInstance()->GetResourcePath(eRT_Language);
    filePath.append(_T("/")).append(pszLanguageTypeString[language]).append(_T(".bin"));
    bool bFindLanguageFile = CFilePathTool::GetInstance()->Exists(filePath.c_str());
    BEATS_ASSERT(bFindLanguageFile, "Can't Find language file %s", filePath.c_str());
    if (bFindLanguageFile)
    {
        int count = 0;
        CSerializer tmp(filePath.c_str());
        tmp >> count;
        m_texts.clear();
        for (int i = 0; i < count; ++i)
        {
            ELanguageTextType textId = (ELanguageTextType)i;
            std::string strValue;
            tmp >> strValue;
            m_texts.emplace(textId, strValue);
        }
        tmp >> count;
        for (int i = 0; i < count; ++i)
        {
            uint32_t key;
            tmp >> key;
            BEATS_ASSERT(m_texts.find((ELanguageTextType)key) == m_texts.end());
            tmp >> m_texts[(ELanguageTextType)key];
        }
    }
#endif
}
