#ifndef BEYOND_ENGINE_RESOURCE_RESOURCEPUBLIC_H__INCLUDE
#define BEYOND_ENGINE_RESOURCE_RESOURCEPUBLIC_H__INCLUDE

enum EResourceType
{
    eRT_SourceCode,
    eRT_Resource,
    eRT_Texture,
    eRT_TextureAtlas,
    eRT_Material,
    eRT_Shader,
    eRT_SpriteAnimation,
    eRT_Particle,
    eRT_Font,
    eRT_Language,
    eRT_StaticMesh,
    eRT_Model,
    eRT_TerrainData,
    eRT_Audio,
    eRT_Script,
    eRT_BCF,

    eRT_Count,
    eRT_Force32Bit = 0xFFFFFFFF
};

static const TCHAR* pszResourceTypeString[] =
{
    _T("SourceCode"),
    _T("Resource"),
    _T("Texture"),
    _T("TextureAtlas"),
    _T("Material"),
    _T("Shader"),
    _T("SpriteAnimation"),
    _T("Particle"),
    _T("Font"),
    _T("Language"),
    _T("StaticMesh"),
    _T("Model"),
    _T("TerrainData"),
    _T("Audio"),
    _T("Script"),
    _T("BCF"),
};

static const TCHAR* pszResourcePathName[] =
{
#if (BEYONDENGINE_PLATFORM == PLATFORM_IOS)||(BEYONDENGINE_PLATFORM == PLATFORM_ANDROID)
    _T("sourcecode"),
    _T("resource"),
    _T("resource/texture"),
    _T("resource/texture"),
    _T("resource/material"),
    _T("resource/shader"),
    _T("resource/spriteanimation"),
    _T("resource/particle"),
    _T("resource/font"),
    _T("resource/language"),
    _T("resource/scene"),
    _T("resource/model"),
    _T("resource/terraindata"),
    _T("resource/audio"),
    _T("resource/script"),
    _T("resource/bcf"),
#else
    _T("../sourcecode"),
    _T("../resource"),
    _T("../resource/texture"),
    _T("../resource/texture"),
    _T("../resource/material"),
    _T("../resource/shader"),
    _T("../resource/spriteanimation"),
    _T("../resource/particle"),
    _T("../resource/font"),
    _T("../resource/language"),
    _T("../resource/scene"),
    _T("../resource/model"),
    _T("../resource/terraindata"),
    _T("../resource/audio"),
    _T("../resource/script"),
    _T("../resource/bcf"),
#endif
};
#endif