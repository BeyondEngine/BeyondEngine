﻿#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTPROJECTDIRECTORY_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTPROJECTDIRECTORY_H__INCLUDE

class CComponentProjectDirectory
{
public:
    CComponentProjectDirectory(CComponentProjectDirectory* pParent, const TString& strName);
    ~CComponentProjectDirectory();

    bool AddFile(const TString& strFileName, std::map<size_t, std::vector<size_t> >& conflictMap);
    CComponentProjectDirectory* AddDirectory(const TString& strDirectoryName);
    void InsertDirectory(CComponentProjectDirectory* pDirectory, CComponentProjectDirectory* pPrevDirectory = NULL);
    void InsertFile(size_t uFileId, size_t uPrevFileId = 0xFFFFFFFF);
    bool DeleteFile(size_t uFileId);
    bool RemoveFile(size_t uFileId);
    bool DeleteDirectory(CComponentProjectDirectory* pDirectory);
    bool RemoveDirectory(CComponentProjectDirectory* pDirectory);
    bool DeleteAll(bool bUpdateProject);

    const TString& GetName() const;
    const std::vector<CComponentProjectDirectory*>& GetChildren() const;
    const std::vector<size_t>& GetFileList() const;
    void SetParent(CComponentProjectDirectory* pParent);
    CComponentProjectDirectory* GetParent() const;
    CComponentProjectDirectory* FindChild(const TCHAR* pszChildName) const;

    void Serialize(CSerializer& serializer) const;
    void Deserialize(CSerializer& serializer);

    TString GenerateLogicPath() const;
    TString MakeRelativeLogicPath(CComponentProjectDirectory* pFromDirectory);

private:
    CComponentProjectDirectory* m_pParent;
    TString m_strName;
    std::vector<size_t>* m_pFilesList;
    std::vector<CComponentProjectDirectory*>* m_pChildrenVec;
};


#endif

