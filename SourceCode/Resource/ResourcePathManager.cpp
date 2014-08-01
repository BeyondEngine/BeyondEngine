#include "stdafx.h"
#include "ResourcePathManager.h"
#include "Utility/BeatsUtility/FilePathTool.h"

CResourcePathManager* CResourcePathManager::m_pInstance = NULL;

CResourcePathManager::CResourcePathManager()
{
}

CResourcePathManager::~CResourcePathManager()
{
}

CResourcePathManager::EResourcePathType CResourcePathManager::GetResourcePathType(EResourceType type) const
{
    EResourcePathType ret = eRPT_Count;
    switch (type)
    {
    case eRT_Texture:
        ret = eRPT_Texture;
        break;
    case eRT_TextureAtlas:
        ret = eRPT_Texture;
        break;
    case eRT_Skeleton:
        ret = eRPT_Skeleton;
        break;
    case eRT_Skin:
        ret = eRPT_Skin;
        break;
    case eRT_Material:
        ret = eRPT_Material;
        break;
    case eRT_Shader:
        ret = eRPT_Shader;
        break;
    case eRT_SpriteAnimation:
        ret = eRPT_SpriteAnimation;
        break;
    case eRT_Animation:
        ret = eRPT_Animation;
        break;
    case eRT_ParticleScript:
        ret = eRPT_Particle;
        break;
    case eRT_Font:
        ret = eRPT_Font;
        break;
    case eRT_Language:
        ret = eRPT_Language;
        break;
    case eRT_StaticMesh:
        ret = eRPT_StaticMesh;
        break;
    default:
        break;
    }
    BEATS_ASSERT(ret != eRPT_Count, _T("Unknown type of resource %d"), type);
    return ret;
}

TString CResourcePathManager::GetResourcePath( CResourcePathManager::EResourcePathType type )
{
    TString strModulePath = CFilePathTool::GetInstance()->FileFullPath(pszCocosResourcePathName[type]);
#if (BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    strModulePath = strModulePath.c_str() + _tcslen("assets/");
#endif
    return strModulePath;
}
