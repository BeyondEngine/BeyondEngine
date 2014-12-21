#ifndef CCONFIGURATION_H_INCLUDE
#define CCONFIGURATION_H_INCLUDE

enum EDeviceType
{
    eDT_Phone,
    eDT_Pad,
    eDT_UNKNOWN
};

static const TCHAR* strDeviceType[] =
{
    _T("Phone"),
    _T("Pad"),
    _T("Unknown"),
};

enum ENetState
{
    eNS_WIFI,
    eNS_ETH,
    eNS_MOBILE,
    eNS_UNAVAILABLE
};

const static TCHAR* strNetState[] =
{
    _T("eNS_WIFI"),
    _T("eNS_ETH"),
    _T("eNS_MOBILE"),
    _T("eNS_UNAVAILABLE")
};

class CConfiguration
{
    BEATS_DECLARE_SINGLETON(CConfiguration);

public:
    int GetMaxTextureSize() const;
    int GetMaxModelviewStackDepth() const;
    int GetMaxTextureUnits() const;
    int GetMaxVertexUniformVectors() const;
    int GetMaxFragmentUniformVectors() const;
    int GetMaxVertexAttributes() const;
    ENetState GetNetworkState() const;//-1:unavailable 1:WiFi 2:Mobile
    EDeviceType GetDeviceType() const;
    TString GetNetworkStateString() const;
    TString GetDeviceTypeString() const;
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

    const TString GetOsType();
    const TString& GetOsInfo();
    const TString& GetModelInfo();
    void GatherGPUInfo();
    void GatherDeviceInfo();

protected:
    bool m_supportsPVRTC;
    bool m_supportsETC1;
    bool m_supportsS3TC;
    bool m_supportsATITC;
    bool m_supportsNPOT;
    bool m_supportsBGRA8888;
    bool m_supportsDiscardFramebuffer;
    bool m_supportsShareableVAO;
    GLint m_maxTextureSize;
    GLint m_maxSamplesAllowed;
    GLint m_maxTextureUnits;
    GLint m_maxVertexAttributes;
    GLint m_maxVertexUniformVectors;
    GLint m_maxFragmentUniformVectors;
    GLint m_maxVaryingVectors;

    EDeviceType m_eDeviceType;
    const char* m_strGL_Extensions;

    TString m_osInfo;
    TString m_modelInfo;
};

#endif//CCONFIGURATION_H_INCLUDE