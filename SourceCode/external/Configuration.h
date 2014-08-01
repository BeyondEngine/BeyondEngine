#ifndef CCONFIGURATION_H_INCLUDE
#define CCONFIGURATION_H_INCLUDE

class CConfiguration
{
    BEATS_DECLARE_SINGLETON(CConfiguration);

public:

    int GetMaxTextureSize() const;
    int GetMaxModelviewStackDepth() const;
    int GetMaxTextureUnits() const;
    bool SupportsNPOT() const;
    bool SupportsPVRTC() const;
    bool SupportsETC() const;
    bool SupportsS3TC() const;
    bool SupportsATITC() const;
    bool SupportsBGRA8888() const;
    bool SupportsDiscardFramebuffer() const;
    bool SupportsShareableVAO() const;
    void SetSupportShareableVAO(bool bEnable);
    bool CheckForGLExtension( const TString& searchName ) const;

    const TString& GetInfo();
    void GatherGPUInfo();

protected:
    GLint   m_maxTextureSize;
    GLint   m_maxModeviewStackDepth;
    bool    m_supportsPVRTC;
    bool    m_supportsETC1;
    bool    m_supportsS3TC;
    bool    m_supportsATITC;
    bool    m_supportsNPOT;
    bool    m_supportsBGRA8888;
    bool    m_supportsDiscardFramebuffer;
    bool    m_supportsShareableVAO;
    GLint   m_maxSamplesAllowed;
    GLint   m_maxTextureUnits;
    const char*   m_strGL_Extensions;

    TString m_strInfo;
    std::string m_strGL_Vendor;
    std::string m_strGL_Renderer;
    std::string m_strGL_Version;
};

#endif//CCONFIGURATION_H_INCLUDE