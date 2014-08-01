#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTPROJECT_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTPROJECT_H__INCLUDE

class TiXmlElement;
class CComponentProjectDirectory;

struct SFileDataLayout
{
    SFileDataLayout()
        : m_uStartPos(0)
        , m_uDataSize(0)
    {

    }
    SFileDataLayout(size_t uStartPos, size_t uDataSize)
        : m_uStartPos(uStartPos)
        , m_uDataSize(uDataSize)
    {

    }
    size_t m_uStartPos;
    size_t m_uDataSize;
};

class CComponentProject
{
public:
    CComponentProject();
    ~CComponentProject();

    CComponentProjectDirectory* LoadProject(const TCHAR* pszProjectFile, std::map<size_t, std::vector<size_t> >& conflictIdMap);
    bool CloseProject();
    void SaveProject();

    // Register file only when: 1. Load Project. 2.Add File. 3. Reload the file when close file(No matter save or not).
    // Unregister file only when: 1. Delete file. 2. Reset the id when close file.
    size_t RegisterFile(CComponentProjectDirectory* pDirectory, const TString& strFileName, std::map<size_t, std::vector<size_t> >& failedId, size_t uSpecifyFileId = 0xFFFFFFFF);
    bool AnalyseFile(const TString& strFileName, std::map<size_t, std::vector<size_t> >& outResult);
    bool UnregisterFile(size_t uFileID);
    void ReloadFile(size_t uFileID);

    // Register/Unregister component only when we need to change some info in static record.
    // Register component only when: 1. Register file. 2. Resolve conflict.
    // Unregister component only when: Resolve conflict.
    void RegisterComponent(size_t uFileID, size_t componentGuid, size_t componentId);
    void UnregisterComponent(size_t componentId);

    CComponentProjectDirectory* GetRootDirectory() const;
    void SetRootDirectory(CComponentProjectDirectory* pDirectory);

    const TString& GetProjectFilePath() const;
    const TString& GetProjectFileName() const;

    const TString& GetComponentFileName(size_t id) const;
    size_t GetComponentFileId(const TString& strFileName) const;

    size_t QueryFileId(size_t uComponentId, bool bOnlyInProjectFile);
    void ResolveIdForFile(size_t uFileId, size_t idToResolve, bool bKeepId);

    void RegisterPropertyMaintainInfo(size_t uComponentGuid, const TString& strOriginPropertyName, const TString& strReplacePropertyName);
    bool GetReplacePropertyName(size_t uComponentGuid, const TString& strOriginPropertyName, TString& strResult);

    const std::vector<TString>* GetFileList() const;

    void RegisterFileLayoutInfo(size_t uFileId, size_t uStartPos, size_t uDataLength);
    bool QueryFileLayoutInfo(size_t uFileId, size_t& uStartPos, size_t& uDataLength);

    void SetStartFile(size_t uFileId);
    size_t GetStartFile() const;
    CComponentProjectDirectory* FindProjectDirectory(const TString& strPath, bool bAbsoluteOrLogicPath) const;
    CComponentProjectDirectory* FindProjectDirectoryById(size_t uFileId);

    size_t QueryComponentGuid(size_t uId);

    std::map<size_t, std::vector<size_t> >* GetFileToComponentMap() const;
    std::map<size_t, std::vector<size_t>>* GetIdToReferenceMap() const;
    std::map<size_t, CComponentProjectDirectory*>* GetFileToDirectoryMap() const;
    std::map<size_t, size_t>* GetComponentToFileMap() const;
private:
    void LoadXMLProject(TiXmlElement* pNode, CComponentProjectDirectory* pProjectDirectory, TString& strStartFilePath, std::map<size_t, std::vector<size_t> >& conflictIdMap);
    void SaveProjectFile( TiXmlElement* pParentNode, const CComponentProjectDirectory* p);

private:
    CComponentProjectDirectory* m_pProjectDirectory;
    size_t m_uStartFileId;
    std::vector<TString>* m_pComponentFiles;
    std::map<size_t, size_t>* m_pComponentToTypeMap;
    std::map<size_t, size_t>* m_pComponentToFileMap;
    std::map<size_t, std::vector<size_t> >* m_pFileToComponentMap;
    std::map<size_t, CComponentProjectDirectory*>* m_pFileToDirectoryMap;
    std::map<size_t, std::vector<size_t> >* m_pTypeToComponentMap;
    // Store property replace info, size_t is the guid of component, map is the old property name and new property name.
    std::map<size_t, std::map<TString, TString> >* m_pPropertyMaintainMap;
    // This member only available in game mode, to save the info about file data layout in the export file.
    std::map<size_t, SFileDataLayout>* m_pFileDataLayout;
    // Key is the proxy id of the real component, and the value is the reference ids
    std::map<size_t, std::vector<size_t>>* m_pReferenceIdMap;

    TString m_strProjectFilePath;
    TString m_strProjectFileName;
};

#endif