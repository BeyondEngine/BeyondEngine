#ifndef BEYOND_ENGINE_RESOURCE_RESOURCEPATHMANAGER_H__INCLUDE
#define BEYOND_ENGINE_RESOURCE_RESOURCEPATHMANAGER_H__INCLUDE

 
static const TCHAR* pszCocosResourcePathName[] =
{
#if (BEYONDENGINE_PLATFORM == PLATFORM_IOS)||(BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    _T("SourceCode"),
    _T("Resource"),
    _T("Resource/Shader"),
    _T("Resource/Animation"),
    _T("Resource/SpriteAnimation"),
    _T("Resource/Skin"),
    _T("Resource/Skeleton"),
    _T("Resource/Material"),
    _T("Resource/Texture"),
    _T("Resource/Particle"),
    _T("Resource/Font"),
    _T("Resource/Language"),
    _T("Resource/TerrainData"),
    _T("Resource/Scene"),
    _T("Resource/Audio"),

    _T(""),
#else
    _T("../SourceCode"),
    _T("../Resource"),
    _T("../Resource/Shader"),
    _T("../Resource/Animation"),
    _T("../Resource/SpriteAnimation"),
    _T("../Resource/Skin"),
    _T("../Resource/Skeleton"),
    _T("../Resource/Material"),
    _T("../Resource/Texture"),
    _T("../Resource/Particle"),
    _T("../Resource/Font"),
    _T("../Resource/Language"),
    _T("../Resource/TerrainData"),
    _T("../Resource/Scene"),
    _T("../Resource/Audio"),

    _T(""),
#endif
};

class CResourcePathManager
{
    BEATS_DECLARE_SINGLETON(CResourcePathManager);
public:
    enum EResourcePathType
    {
        eRPT_SourceCode,
        eRPT_Resource,
        eRPT_Shader,
        eRPT_Animation,
        eRPT_SpriteAnimation,
        eRPT_Skin,
        eRPT_Skeleton,
        eRPT_Material,
        eRPT_Texture,
        eRPT_Particle,
        eRPT_Font,
        eRPT_Language,
        eRPT_TerrainData,
        eRPT_StaticMesh,
        eRPT_Audio,

        eRPT_Count,
    };

    TString GetResourcePath(CResourcePathManager::EResourcePathType type);
    CResourcePathManager::EResourcePathType GetResourcePathType(EResourceType type) const;

private:
    TString m_strResourcePath;
};
#endif