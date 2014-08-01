#ifndef BEYOND_ENGINE_EDITOR_EDITORCONFIG_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_EDITORCONFIG_H__INCLUDE

class TiXmlDocument;
class CEditorConfig
{
    BEATS_DECLARE_SINGLETON(CEditorConfig)
public:
    void LoadFromFile();
    void SaveToFile();

    bool IsShowAboutDlgAfterLaunch() const;
    void SetShowAboutDlgAfterlaunch(bool bFlag);

    const TString& GetLastOpenProject() const;
    void SetLastOpenProject(const TCHAR* pszProjectPath);

    const TString& GetLastOpenFile() const;
    void SetLastOpenFile(const TCHAR* pszFilePath);

    ELanguage GetCurrLanguage() const;
    void SetCurrLanguage(ELanguage language);

private:
    bool m_bShowAboutDlgAfterLaunch;
    TiXmlDocument* m_pConfigXML;
    ELanguage m_currLanguage;
    TString m_strLastOpenProject;
    TString m_strLastOpenFile;
};

#endif
