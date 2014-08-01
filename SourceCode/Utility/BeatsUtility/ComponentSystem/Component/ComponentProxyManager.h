#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTPROXYMANAGER_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTPROXYMANAGER_H__INCLUDE

#include "ComponentManagerBase.h"

class CComponentProxy;
class CPropertyDescriptionBase;
class CComponentGraphic;
class CComponentReference;
class CComponentProjectDirectory;

typedef CComponentProxy* (*TCreateComponentEditorProxyFunc)(CComponentGraphic* pGraphics, size_t guid, size_t parentGuid, TCHAR* className);
typedef CComponentGraphic* (*TCreateGraphicFunc)();

class CComponentProxyManager : public CComponentManagerBase
{
    typedef CComponentManagerBase super;
    BEATS_DECLARE_SINGLETON(CComponentProxyManager);
    typedef CPropertyDescriptionBase* (*TCreatePropertyFunc)(CSerializer* serializer);
public:
    // param bOpenAsCopy means we will create a new file from template file.
    void OpenFile(const TCHAR* pFilePath, bool bOpenAsCopy = false);
    void LoadFile(const TCHAR* pszFilePath, std::vector<CComponentProxy*>* pComponentContainer);
    void LoadFileFromDirectory(CComponentProjectDirectory* pDirectory, std::vector<CComponentProxy*>* pComponentContainer);

    void CloseFile(const TCHAR* pszFilePath);
    const TString& GetCurrentWorkingFilePath() const;
    const TString& GetCurrentViewFilePath() const;

    void Export(const TCHAR* pSavePath);

    void QueryDerivedClass(size_t uBaseClassGuid, std::vector<size_t>& result, bool bRecurse) const;
    void RegisterClassInheritInfo(size_t uDerivedClassGuid, size_t uBaseClassGuid);
    TString QueryComponentName(size_t uGuid) const;

    void SaveTemplate(const TCHAR* pszFilePath);
    void SaveCurFile();

    void RegisterPropertyCreator(size_t enumType, TCreatePropertyFunc func);
    CPropertyDescriptionBase* CreateProperty(size_t propertyType, CSerializer* serializer);

    void DeserializeTemplateData(const TCHAR* pszPath,
                                 const TCHAR* pszEDSFileName,
                                 const TCHAR* pszPatchXMLFileName,
                                TCreateComponentEditorProxyFunc func,
                                TCreateGraphicFunc pGraphicFunc);
    virtual void ResolveDependency() override;

    CPropertyDescriptionBase* GetCurrReflectDescription() const;
    void SetCurrReflectDescription(CPropertyDescriptionBase* pPropertyDescription);
    void SetReflectCheckFlag(bool bFlag);
    bool GetReflectCheckFlag() const;

    CDependencyDescription* GetCurrReflectDependency() const;
    void SetCurrReflectDependency(CDependencyDescription* pDependency);

    const std::map<size_t, TString>& GetAbstractComponentNameMap() const;

    // We won't call OnPropertyChange when loading files.
    bool IsLoadingFile() const;

    bool IsParent(size_t uParentGuid, size_t uChildGuid) const;

    void RegisterComponentReference(CComponentReference* pReference);
    void UnregisterComponentReference(CComponentReference* pReference);
    const std::map<size_t, std::vector<CComponentReference*>>& GetReferenceIdMap() const;
    const std::map<size_t, CComponentReference*>& GetReferenceMap() const;

    CComponentReference* CreateReference(size_t uProxyId, size_t uReferenceGuid, size_t uId = 0xFFFFFFFF);

    const std::map<size_t, CComponentProxy*>& GetComponentsInCurScene() const;
    void OnCreateComponentInScene(CComponentProxy* pProxy);
    void OnDeleteComponentInScene(CComponentProxy* pProxy);

    bool IsExporting() const;
    const std::set<size_t>& GetLoadedFiles() const;

    // Useful for exporting, at that phase, no need to create any component instance.
    bool IsEnableCreateInstanceWithProxy() const;
    void SetEnableCreateInstanceWithProxy(bool bFlag);

private:
    void LoadTemplateDataFromXML(const TCHAR* pWorkingPath);
    void LoadTemplateDataFromSerializer(CSerializer& serializer, TCreateComponentEditorProxyFunc func, TCreateGraphicFunc pGraphicFunc);

private:
    bool m_bCreateInstanceWithProxy;
    bool m_bLoadingFilePhase;
    bool m_bExportingPhase;
    bool m_bReflectCheckFlag;
    TString m_currentWorkingFilePath;
    TString m_currentViewFilePath;
    CPropertyDescriptionBase* m_pCurrReflectPropertyDescription;
    CDependencyDescription* m_pCurrReflectDependency;
    std::map<size_t, TCreatePropertyFunc>* m_pPropertyCreatorMap;
    std::map<size_t, TString> m_abstractComponentNameMap;
    // This map save the inherit relationship for all components. so when we instance a component pointer, we can decide which instance to generate.
    std::map<size_t, std::vector<size_t> >* m_pComponentInheritMap;

    // This map store all reference info, key value is the real component id.
    std::map<size_t, std::vector<CComponentReference*>> m_referenceIdMap;
    std::map<size_t, CComponentReference*> m_referenceMap;

    std::map<size_t, CComponentProxy*> m_proxyInCurScene;
    std::set<size_t> m_loadedFiles;
};



#endif