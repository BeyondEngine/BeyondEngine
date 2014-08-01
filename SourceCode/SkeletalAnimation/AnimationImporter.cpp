#include "stdafx.h"
#include "AnimationImporter.h"
#include "SkeletonAnimationManager.h"
#include "ArmatureData.h"
#include "AnimationData.h"
#include "TextureData.h"
#include "BoneData.h"
#include "DisplayData.h"
#include "MoveData.h"
#include "Json.h"
#include "Utility/BeatsUtility/StringHelper.h"

static const char *VERSION = "version";
static const char *ARMATURES = "armatures";
static const char *ARMATURE = "armature";
static const char *BONE = "b";
static const char *DISPLAY = "d";
static const char *ANIMATIONS = "animations";
static const char *ANIMATION = "animation";
static const char *MOVEMENT = "mov";
static const char *FRAME = "f";
static const char *TEXTURE_ATLAS = "TextureAtlas";
static const char *SUB_TEXTURE = "SubTexture";
static const char *A_NAME = "name";
static const char *A_DURATION = "dr";
static const char *A_FRAME_INDEX = "fi";
static const char *A_DURATION_TO = "to";
static const char *A_DURATION_TWEEN = "drTW";
static const char *A_LOOP = "lp";
static const char *A_MOVEMENT_SCALE = "sc";
static const char *A_MOVEMENT_DELAY = "dl";
static const char *A_DISPLAY_INDEX = "dI";
static const char *A_PARENT = "parent";
static const char *A_SKEW_X = "kX";
static const char *A_SKEW_Y = "kY";
static const char *A_SCALE_X = "cX";
static const char *A_SCALE_Y = "cY";
static const char *A_Z = "z";
static const char *A_EVENT = "evt";
static const char *A_SOUND = "sd";
static const char *A_SOUND_EFFECT = "sdE";
static const char *A_TWEEN_EASING = "twE";
static const char *A_EASING_PARAM = "twEP";
static const char *A_TWEEN_ROTATE = "twR";
static const char *A_IS_ARMATURE = "isArmature";
static const char *A_DISPLAY_TYPE = "displayType";
static const char *A_MOVEMENT = "mov";
static const char *A_X = "x";
static const char *A_Y = "y";
static const char *A_WIDTH = "width";
static const char *A_HEIGHT = "height";
static const char *A_PIVOT_X = "pX";
static const char *A_PIVOT_Y = "pY";
static const char *A_COCOS2D_PIVOT_X = "cocos2d_pX";
static const char *A_COCOS2D_PIVOT_Y = "cocos2d_pY";
static const char *A_BLEND_TYPE = "bd";
static const char *A_BLEND_SRC = "bd_src";
static const char *A_BLEND_DST = "bd_dst";
static const char *A_ALPHA = "a";
static const char *A_RED = "r";
static const char *A_GREEN = "g";
static const char *A_BLUE = "b";
static const char *A_ALPHA_OFFSET = "aM";
static const char *A_RED_OFFSET = "rM";
static const char *A_GREEN_OFFSET = "gM";
static const char *A_BLUE_OFFSET = "bM";
static const char *A_COLOR_TRANSFORM = "colorTransform";
static const char *A_TWEEN_FRAME = "tweenFrame";
static const char *CONTOUR = "con";
static const char *CONTOUR_VERTEX = "con_vt";
static const char *FL_NAN = "NaN";
static const char *FRAME_DATA = "frame_data";
static const char *MOVEMENT_BONE_DATA = "mov_bone_data";
static const char *MOVEMENT_DATA = "mov_data";
static const char *ANIMATION_DATA = "animation_data";
static const char *DISPLAY_DATA = "display_data";
static const char *SKIN_DATA = "skin_data";
static const char *BONE_DATA = "bone_data";
static const char *ARMATURE_DATA = "armature_data";
static const char *CONTOUR_DATA = "contour_data";
static const char *TEXTURE_DATA = "texture_data";
static const char *VERTEX_POINT = "vertex";
static const char *COLOR_INFO = "color";
static const char *CONFIG_FILE_PATH = "config_file_path";
static const char *CONTENT_SCALE = "content_scale";

 using namespace _2DSkeletalAnimation;

CAnimationImporter* CAnimationImporter::m_pInstance = nullptr;

CAnimationImporter::CAnimationImporter()
{

}

CAnimationImporter::~CAnimationImporter()
{

}
void CAnimationImporter::ImportData(const TString& strFilePath)
{
    if(CheckFile(strFilePath))
    {
        TString strContent = GetStringFromFile(strFilePath.c_str());
        size_t uBufferSize = strContent.length() + 1;
        char* pBuffer = new char[uBufferSize];
        CStringHelper::GetInstance()->ConvertToCHAR(strContent.c_str(), pBuffer, uBufferSize);
        Json* jsonRoot = Json_create (pBuffer);
        BEATS_SAFE_DELETE_ARRAY(pBuffer);
        Json* jsonChild = jsonRoot->child;
        while (jsonChild) 
        {
            if(jsonChild->type == Json_Number)
            {
                BEATS_ASSERT(jsonChild->valueFloat > 0.f)
            }
            else if(jsonChild->type == Json_Array)
            {
                if (strcmp(jsonChild->name, ARMATURE_DATA) == 0)
                {
                    DecodeArmature(jsonChild);
                }
                else if (strcmp(jsonChild->name, ANIMATION_DATA) == 0)
                {
                    DecodeAnimation(jsonChild);
                }
                else if (strcmp(jsonChild->name, TEXTURE_DATA) == 0)
                {
                    DecodeTexture(jsonChild);
                }
            }
            jsonChild = jsonChild->next;
        }
        Json_dispose(jsonRoot);
    }
}

void CAnimationImporter::DecodeArmature(const TString& strFilePath, CArmatureData* pArmatureData)
{
    BEATS_ASSERT(pArmatureData);
    TString strContent = GetStringFromFile(strFilePath.c_str());
    size_t uBufferSize = strContent.length() + 1;
    char* pBuffer = new char[uBufferSize];
    CStringHelper::GetInstance()->ConvertToCHAR(strContent.c_str(), pBuffer, uBufferSize);
    Json* jsonRoot = Json_create (pBuffer);
    BEATS_SAFE_DELETE_ARRAY(pBuffer);
    Json* jsonChild = jsonRoot->child;
    while (jsonChild) 
    {
        if(jsonChild->type == Json_Number)
        {
            BEATS_ASSERT(jsonChild->valueFloat > 0.f)
        }
        else if(jsonChild->type == Json_Array)
        {
            if (strcmp(jsonChild->name, ARMATURE_DATA) == 0)
            {
                Json* childArmature = jsonChild->child;
                while (childArmature)
                {
                    if(childArmature->type == Json_Object)
                    {
                        Json* name = Json_getItem(childArmature,A_NAME);
                        Json* bonedata = Json_getItem(childArmature,BONE_DATA);
                        pArmatureData->SetName(name->valueString);
                        DecodeBoneData(bonedata, pArmatureData);
                    }
                    childArmature = childArmature->next;
                }
            }
        }
        jsonChild = jsonChild->next;
    }
    Json_dispose(jsonRoot);
}

void CAnimationImporter::DecodeAnimation(const TString& strFilePath, CAnimationData* pAnimationData)
{
    BEATS_ASSERT(pAnimationData);
    TString jsonstr = GetStringFromFile(strFilePath.c_str());
    size_t uBufferSize = jsonstr.length() + 1;
    char* pBuffer = new char[uBufferSize];
    CStringHelper::GetInstance()->ConvertToCHAR(jsonstr.c_str(), pBuffer, uBufferSize);
    Json* jsonRoot = Json_create (pBuffer);
    BEATS_SAFE_DELETE_ARRAY(pBuffer);
    Json* jsonChild = jsonRoot->child;
    while (jsonChild) 
    {
        if(jsonChild->type == Json_Number)
        {
            BEATS_ASSERT(jsonChild->valueFloat > 0.f)
        }
        else if(jsonChild->type == Json_Array)
        {
            if (strcmp(jsonChild->name, ANIMATION_DATA) == 0)
            {
                Json* childAnimation = jsonChild->child;
                while (childAnimation)
                {
                    if(childAnimation->type == Json_Object)
                    {
                        Json* name = Json_getItem(childAnimation,A_NAME);
                        Json* movdata = Json_getItem(childAnimation,MOVEMENT_DATA);
                        pAnimationData->SetName(name->valueString);
                        DecodeMovData(movdata, pAnimationData);
                    }
                    childAnimation = childAnimation->next;
                }
            }
        }
        jsonChild = jsonChild->next;
    }
    Json_dispose(jsonRoot);
}

void CAnimationImporter::DecodeArmature(Json* json)
{
    BEATS_ASSERT(json);
    Json* child = json->child;
    while (child)
    {
        if(child->type == Json_Object)
        {
            Json* name = Json_getItem(child,A_NAME);
            Json* bonedata = Json_getItem(child,BONE_DATA);
            CArmatureData* pArmatureData = new CArmatureData();
            pArmatureData->SetName(name->valueString);
            CSkeletonAnimationManager::GetInstance()->AddArmatureData(pArmatureData);
            DecodeBoneData(bonedata, pArmatureData);
        }
        child = child->next;
    }
}

void CAnimationImporter::DecodeBoneData(Json* json, CArmatureData* pArmatureData)
{
    BEATS_ASSERT(json);
    BEATS_ASSERT(json->type == Json_Array);

    Json* child = json->child;
    while (child)
    {
        if(child->type == Json_Object)
        {
            Json* name = Json_getItem(child,A_NAME);
            Json* x = Json_getItem(child,A_X);
            Json* y = Json_getItem(child,A_Y);
            Json* z = Json_getItem(child,A_Z);
            Json* cX = Json_getItem(child,A_SCALE_X);
            Json* cY = Json_getItem(child,A_SCALE_Y);
            Json* kX = Json_getItem(child,A_SKEW_X);
            Json* kY = Json_getItem(child,A_SKEW_Y);

            Json* display_data = Json_getItem(child, DISPLAY_DATA);

            CBoneData* pBoneData = new CBoneData();

            pBoneData->SetName(name->valueString);
            pBoneData->SetPositionX(x->valueFloat);
            pBoneData->SetPositionY(y->valueFloat);
            pBoneData->SetZOrder(z->valueInt);
            pBoneData->SetScaleX(cX->valueFloat);
            pBoneData->SetScaleY(cY->valueFloat);
            pBoneData->SetSkewX(kX->valueFloat);
            pBoneData->SetSkewY(kY->valueFloat);

            pArmatureData->AddBoneData(pBoneData);

            DecodeDisplayData(display_data, pBoneData);
        }
        child = child->next;
    }
}

void CAnimationImporter::DecodeDisplayData(Json* json, CBoneData* pBoneData)
{
    BEATS_ASSERT(json && pBoneData);
    BEATS_ASSERT(json->type == Json_Array);

    Json* child = json->child;
    while (child)
    {
        if(child->type == Json_Object)
        {
            Json* name = Json_getItem(child,A_NAME);
            Json* displayType = Json_getItem(child,A_DISPLAY_TYPE);
            Json* skin_data = Json_getItem(child,SKIN_DATA);
            CDisplayData displaydata;

            displaydata.SetName(name->valueString);
            displaydata.SetType((EDisplayType)displayType->valueInt);

            DecodeSkinData(skin_data,&displaydata);
            pBoneData->AddDisplayData(displaydata);
        }
        child =child->next;
    }
}

void CAnimationImporter::DecodeSkinData(Json* json, CDisplayData* pDisplayData)
{
    BEATS_ASSERT(json && pDisplayData);
    BEATS_ASSERT(json->type == Json_Array);

    Json* child = json->child;
    while (child)
    {
        if(child->type == Json_Object)
        {
            Json* x = Json_getItem(child,A_X);
            Json* y = Json_getItem(child,A_Y);

            Json* cX = Json_getItem(child,A_SCALE_X);
            Json* cY = Json_getItem(child,A_SCALE_Y);

            Json* kX = Json_getItem(child, A_SKEW_X);
            Json* kY = Json_getItem(child, A_SKEW_Y);

            pDisplayData->SetPositionX(x->valueFloat);
            pDisplayData->SetPositionY(y->valueFloat);
            pDisplayData->SetScaleX(cX->valueFloat);
            pDisplayData->SetScaleY(cY->valueFloat);
            pDisplayData->SetSkewX(kX->valueFloat);
            pDisplayData->SetSkewY(kY->valueFloat);
        }
        child =child->next;
    }
}

void CAnimationImporter::DecodeAnimation(Json* json)
{
    BEATS_ASSERT(json);
    Json* child = json->child;
    while (child)
    {
        if(child->type == Json_Object)
        {
            Json* name = Json_getItem(child,A_NAME);
            Json* movdata = Json_getItem(child,MOVEMENT_DATA);
            CAnimationData* pAnimationData = new CAnimationData;
            pAnimationData->SetName(name->valueString);
            CSkeletonAnimationManager::GetInstance()->AddAnimationData(pAnimationData);
            DecodeMovData(movdata, pAnimationData);
        }
        child = child->next;
    }
}

void CAnimationImporter::DecodeMovData(Json* json, CAnimationData* pAnimationData)
{
    BEATS_ASSERT(json);
    BEATS_ASSERT(json->type == Json_Array);

    Json* child = json->child;
    while (child)
    {
        if(child->type == Json_Object)
        {
            Json* name = Json_getItem(child,A_NAME);
            Json* dr = Json_getItem(child,A_DURATION);
            Json* to = Json_getItem(child,A_DURATION_TO);
            Json* drTW = Json_getItem(child,A_DURATION_TWEEN);
            Json* movebonedata = Json_getItem(child,MOVEMENT_BONE_DATA);
            
            CMoveData* pMoveData = new CMoveData;
            pMoveData->SetName(name->valueString);
            pMoveData->SetDuration(dr->valueInt);
            pMoveData->SetDurationTo(to->valueInt);
            pMoveData->SetDuraionTween(drTW->valueInt);
            pAnimationData->AddMoveData(pMoveData);

            DecodeMovBoneData(movebonedata, pMoveData);
        }
        child = child->next;
    }
}

void CAnimationImporter::DecodeMovBoneData(Json *json, CMoveData* pMoveData)
{
    BEATS_ASSERT(json);
    Json* child = json->child;
    while (child)
    {
        if(child->type == Json_Object)
        {
            Json* name = Json_getItem(child,A_NAME);
            Json* dl = Json_getItem(child,A_MOVEMENT_DELAY);
            Json* framedata = Json_getItem(child,FRAME_DATA);
            SMoveBoneData* pMoveBoneData = new SMoveBoneData;
            pMoveBoneData->name = name->valueString;
            pMoveBoneData->duration = dl->valueFloat;
            pMoveData->AddMoveBoneData(pMoveBoneData);

            DecodeFrameData(framedata, pMoveBoneData);
        }
        child = child->next;
    }
}

void CAnimationImporter::DecodeFrameData(Json* json, SMoveBoneData* pMoveBoneData)
{
    BEATS_ASSERT(json);
    Json* child = json->child;
    while (child)
    {
        if(child->type == Json_Object)
        {
            Json* dI = Json_getItem(child,A_DISPLAY_INDEX);
            Json* x = Json_getItem(child,A_X);
            Json* y = Json_getItem(child,A_Y);
            Json* z = Json_getItem(child,A_Z);
            Json* cX = Json_getItem(child,A_SCALE_X);
            Json* cY = Json_getItem(child,A_SCALE_Y);
            Json* kX = Json_getItem(child,A_SKEW_X);
            Json* kY = Json_getItem(child,A_SKEW_Y);
            Json* fi = Json_getItem(child,A_FRAME_INDEX);

            SFrameData* framedata = new SFrameData;
            framedata->dI = dI->valueInt;
            framedata->x = x->valueFloat;
            framedata->y = y->valueFloat;
            framedata->zOrder = z->valueInt;
            framedata->scaleX = cX->valueFloat;
            framedata->scaleY = cY->valueFloat;
            framedata->skewX = kX->valueFloat;
            framedata->skewY = kY->valueFloat;
            framedata->frameID = fi->valueInt;

            pMoveBoneData->frameList.push_back(framedata);
        }
        child = child->next;
    }
}

void CAnimationImporter::DecodeTexture(Json* json)
{
    BEATS_ASSERT(json);
    Json* child = json->child;
    while (child)
    {
        if(child->type == Json_Object)
        {
            Json* name = Json_getItem(child,A_NAME);
            Json* width = Json_getItem(child,A_WIDTH);
            Json* height = Json_getItem(child,A_HEIGHT);
            Json* pX = Json_getItem(child,A_PIVOT_X);
            Json* pY = Json_getItem(child,A_PIVOT_Y);
            CTextureData* texturedata = new CTextureData;
            texturedata->m_strName = name->valueString;
            texturedata->m_fWidth = width->valueFloat;
            texturedata->m_fHeight = height->valueFloat;
            texturedata->m_fpX = pX->valueFloat;
            texturedata->m_fpY = pY->valueFloat;

            std::vector<CTextureData*>& texturelist= CSkeletonAnimationManager::GetInstance()->GetTextureData();
            texturelist.push_back(texturedata);

        }
        child = child->next;
    }
}

TString CAnimationImporter::GetStringFromFile(const TCHAR* pszFilePath)
{
    CSerializer serializer(pszFilePath, _T("r"));
    serializer << _T("/0");
    TString strContent;
    serializer >> strContent;
    return strContent;
}

bool CAnimationImporter::CheckFile(const TString& file)
{
    bool bRet = false;
    std::map<TString, TString> ::const_iterator iter = m_filesMap.find(file);
    if(iter == m_filesMap.end())
    {
        m_filesMap.insert(std::map<TString, TString>::value_type(file,file));
        bRet = true;
    }

    return bRet;
}
