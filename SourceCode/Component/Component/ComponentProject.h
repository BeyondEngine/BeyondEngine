#ifndef BEYONDENGINE_COMPONENT_COMPONENT_COMPONENTPROJECT_H__INCLUDE
#define BEYONDENGINE_COMPONENT_COMPONENT_COMPONENTPROJECT_H__INCLUDE

class CComponentProjectDirectory;
#include "RapidXML/rapidxml.hpp"

struct SFileDataLayout
{
    SFileDataLayout()
        : m_uStartPos(0)
        , m_uDataSize(0)
    {

    }
    SFileDataLayout(uint32_t uStartPos, uint32_t uDataSize)
        : m_uStartPos(uStartPos)
        , m_uDataSize(uDataSize)
    {

    }
    uint32_t m_uStartPos;
    uint32_t m_uDataSize;
};

class  CComponentProject
{
public:
    CComponentProject();
    ~CComponentProject();

    CComponentProjectDirectory* LoadProject(const TCHAR* pszProjectFile, std::map<uint32_t, std::vector<uint32_t> >& conflictIdMap);
    uint32_t GetLoadProjectProgress(TString& strCurrLoadingFile) const;

    bool CloseProject();
    void SaveProject();

    // Register file only when: 1. Load Project. 2.Add File. 3. Reload the file when close file(No matter save or not).
    // Unregister file only when: 1. Delete file. 2. Reset the id when close file.
    uint32_t RegisterFile(CComponentProjectDirectory* pDirectory, const TString& strFileName, std::map<uint32_t, std::vector<uint32_t> >& failedId, uint32_t uSpecifyFileId = 0xFFFFFFFF);
    bool AnalyseFile(const TString& strFileName, uint32_t uFileID, std::map<uint32_t, std::vector<uint32_t> >& outResult);
    bool UnregisterFile(uint32_t uFileID);
    void ReloadFile(uint32_t uFileID);

    // Register/Unregister component only when we need to change some info in static record.
    // Register component only when: 1. Register file. 2. Resolve conflict.
    // Unregister component only when: Resolve conflict.
    void RegisterComponent(uint32_t uFileID, uint32_t componentGuid, uint32_t componentId);
    void UnregisterComponent(uint32_t componentId);

    CComponentProjectDirectory* GetRootDirectory() const;
    void SetRootDirectory(CComponentProjectDirectory* pDirectory);

    const TString& GetProjectFilePath() const;
    const TString& GetProjectFileName() const;

    const TString& GetComponentFileName(uint32_t id) const;
    uint32_t GetComponentFileId(const TString& strFileName) const;

    uint32_t QueryFileId(uint32_t uComponentId, bool bOnlyInProjectFile);
    void ResolveIdForFile(uint32_t uFileId, uint32_t idToResolve, bool bKeepId);

    void RegisterPropertyMaintainInfo(uint32_t uComponentGuid, const TString& strOriginPropertyName, const TString& strReplacePropertyName);
    bool GetReplacePropertyName(uint32_t uComponentGuid, const TString& strOriginPropertyName, TString& strResult);

    const std::vector<TString>* GetFileList() const;

    void RegisterFileLayoutInfo(uint32_t uFileId, uint32_t uStartPos, uint32_t uDataLength);
    bool QueryFileLayoutInfo(uint32_t uFileId, uint32_t& uStartPos, uint32_t& uDataLength) const;

    void SetStartFile(uint32_t uFileId);
    uint32_t GetStartFile() const;
    CComponentProjectDirectory* FindProjectDirectory(const TString& strPath, bool bAbsoluteOrLogicPath) const;
    CComponentProjectDirectory* FindProjectDirectoryById(uint32_t uFileId);

    uint32_t QueryComponentGuid(uint32_t uId);

    const std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* GetFileToComponentMap() const;
    std::map<uint32_t, CComponentProjectDirectory*>* GetFileToDirectoryMap() const;
    const std::map<uint32_t, uint32_t>* GetComponentToFileMap() const;
    const std::map<uint32_t, std::vector<uint32_t> >* GetTypeToComponentMap() const;
    const std::map<uint32_t, std::set<uint32_t> >* GetTypeRefInComponentMap() const;

private:
    uint32_t GetTotalFileCount(rapidxml::xml_node<>* pNode) const;
    void LoadXMLProject(rapidxml::xml_node<>* pNode, CComponentProjectDirectory* pProjectDirectory, TString& strStartFilePath, std::map<uint32_t, std::vector<uint32_t> >& conflictIdMap);
    void SaveProjectFile(rapidxml::xml_node<>* pParentNode, const CComponentProjectDirectory* p);
    void AnalyseForTypeRef(rapidxml::xml_node<>* pVariableNode, uint32_t uComponentId);
private:
    uint32_t m_uLoadedFileCount; // For progress
    uint32_t m_uTotalFileCount; //For progress
    TString m_strCurrLoadingFile; // For progress

    CComponentProjectDirectory* m_pProjectDirectory;
    uint32_t m_uStartFileId;
    std::vector<TString>* m_pComponentFiles;
    std::map<uint32_t, uint32_t>* m_pComponentToTypeMap;
    std::map<uint32_t, uint32_t>* m_pComponentToFileMap;
    std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* m_pFileToComponentMap;
    std::map<uint32_t, CComponentProjectDirectory*>* m_pFileToDirectoryMap;
    std::map<uint32_t, std::vector<uint32_t> >* m_pTypeToComponentMap;
    std::map<uint32_t, std::set<uint32_t> >* m_pTypeRefInComponentMap;
    // Store property replace info, uint32_t is the guid of component, map is the old property name and new property name.
    std::map<uint32_t, std::map<TString, TString> >* m_pPropertyMaintainMap;
    // This member only available in game mode, to save the info about file data layout in the export file.
    std::map<uint32_t, SFileDataLayout>* m_pFileDataLayout;

    TString m_strProjectFilePath;
    TString m_strProjectFileName;
};

#endif