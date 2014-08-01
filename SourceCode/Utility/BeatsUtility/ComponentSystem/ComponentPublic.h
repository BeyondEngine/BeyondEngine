#ifndef BEATS_COMPONENTS_COMPONENTPUBLIC_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENTPUBLIC_H__INCLUDE

#include <string>
#include "Property/PropertyPublic.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Component/ComponentProxy.h"
#include "Component/ComponentProxyManager.h"
#include "Component/ComponentInstanceManager.h"
#include "Property/PropertyDescriptionBase.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescription.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/UtilityManager.h"
#include "Utility/BeatsUtility/Platform/BeatsTchar.h"

#define ENGINE_COMPONENT_INFO_FILE _T("EngineLaunch.eds")
#define ENGINE_COMPONENT_INFO_PATCH _T("EnginePatch.XML")
#define GAME_COMPONENT_INFO_FILE _T("GameLaunch.eds")
#define GAME_COMPONENT_INFO_PATCH _T("GamePatch.XML")
#define ENGINE_ENUM_INFO_FILE _T("EngineScanData.bin")

#define COMPONENT_FILE_HEADERSTR _T("ComponentFile")
#define COMPONENT_FILE_EXTENSION _T(".bcf")
#define COMPONENT_FILE_EXTENSION_FILTER _T("Beats Component File(*.bcf)\0*.bcf\0\0")
#define COMPONENT_PROJECT_EXTENSION _T("Beats Component Project File(*.bcp)\0*.bcp\0\0")
#define BINARIZE_FILE_NAME _T("AIData.bin")
#define BINARIZE_FILE_EXTENSION _T(".bin")
#define BINARIZE_FILE_EXTENSION_FILTER _T("Beats binaray File(*.bin)\0*.bin\0\0")
#define PROPERTY_PARAM_SPLIT_STR _T(",")
#define PROPERTY_KEYWORD_SPLIT_STR _T(":")

#define COMPONENT_SYSTEM_VERSION 4

template<typename T>
inline void InitValue(const T& param)
{
    //TODO: I don't know how to init a template value, so this is the HACK way.
    if (sizeof(T) <= 4)
    {
        memset((void*)&param, 0, sizeof(T));
    }
}

struct SSerilaizerExtraInfo
{
    SSerilaizerExtraInfo()
        : m_uPropertyCount(0)
        , m_uDependencyCount(0)
    {

    }
    size_t m_uPropertyCount;
    size_t m_uDependencyCount;
};

class CSerializer;

template<typename T>
inline EReflectPropertyType GetEnumType(T& value, CSerializer* pSerializer)
{
    EReflectPropertyType eRet = eRPT_Invalid;
    const char* pszTypeName = typeid(value).name();
    bool bIsEnum = memcmp(pszTypeName, "enum ", strlen("enum ")) == 0;
    if (bIsEnum)
    {
        eRet = eRPT_Enum;
        TCHAR szNameBuffer[128];
        CStringHelper::GetInstance()->ConvertToTCHAR(&pszTypeName[strlen("enum ")], szNameBuffer, 128);
        (*pSerializer) << (size_t)eRPT_Enum;
        (*pSerializer) << (int)(value) << szNameBuffer;
    }
    BEATS_ASSERT(bIsEnum, _T("Unknown type!"));
    return eRet;
}

template<typename T>
inline void DeserializeVariable(T& value, CSerializer* pSerializer)
{
    *pSerializer >> value;
}

struct SReflectFilePath
{
    TString m_value;
};

template<>
inline void DeserializeVariable(SReflectFilePath& value, CSerializer* pSerializer)
{
    *pSerializer >> value.m_value;
}


template<>
inline void DeserializeVariable(CColorSpline& value, CSerializer* pSerializer)
{
    TString strNewValue;
    *pSerializer >> strNewValue;
    value.m_mapColors.clear();
    std::vector<TString> cache;
    CStringHelper::GetInstance()->SplitString(strNewValue.c_str(), _T("@"), cache);
    BEATS_ASSERT(cache.size() > 0);
    int nCacheCount = _ttoi(cache[0].c_str());
    int nDataCount = (int)cache.size() - 1;
    BEATS_ASSERT(nCacheCount == nDataCount / 2);
    (void)nCacheCount;
    for (int i = 1; i < nDataCount; i += 2)
    {
        float first = 0.0f;
        first = (float)_ttof(cache[i].c_str());
        size_t second = _tcstoul(cache[i + 1].c_str(), 0, 16);
        value.m_mapColors[first] = CColor(second);
    }
}

template<typename T>
inline void DeserializeVariable(T*& value, CSerializer* pSerializer)
{
    bool bHasInstance = false;
    *pSerializer >> bHasInstance;
    if(bHasInstance)
    {
        size_t uDataSize, uGuid, uId, uStartPos;
        uStartPos = pSerializer->GetReadPos();
        *pSerializer >> uDataSize;
        *pSerializer >> uGuid;
        *pSerializer >> uId;
        if (value == NULL)
        {
#ifdef EDITOR_MODE
            CPropertyDescriptionBase* pProperty = CComponentProxyManager::GetInstance()->GetCurrReflectDescription();
            if (pProperty != NULL)
            {
                BEATS_ASSERT(pProperty != NULL && pProperty->GetType() == eRPT_Ptr && pProperty->GetInstanceComponent() != NULL &&  pProperty->GetInstanceComponent()->GetHostComponent() != NULL);
                value = dynamic_cast<T*>(pProperty->GetInstanceComponent()->GetHostComponent());
                BEATS_ASSERT(value != NULL);
                // If we are in editor mode, when a pointer become an instance from NULL
                // it means there is no need to read more info from the pSerializer, because it is a construct operation.
                //This line is useless and unnecessary. But if you call this, the pSerializer will be read finished.
                //value->ReflectData(*pSerializer);
                pSerializer->SetReadPos(uStartPos + uDataSize);
            }
            else
            {
#endif
                value = dynamic_cast<T*>(CComponentInstanceManager::GetInstance()->CreateComponent(uGuid, false, true, 0xFFFFFFFF, false, pSerializer));
                if (CComponentInstanceManager::GetInstance()->GetFileSerializer() == pSerializer)
                {
                    value->SetDataPos(uStartPos);
                    value->SetDataSize(uDataSize);
                }
                BEATS_ASSERT(uStartPos + uDataSize == pSerializer->GetReadPos(),
                    _T("Component Data Not Match!\nGot an error when Deserialize a pointer of component 0x%x %s instance id %d\nRequired size: %d, Actual size: %d"), uGuid, value->GetClassStr(), uId, uDataSize, pSerializer->GetReadPos() - uStartPos);
#ifdef EDITOR_MODE
            }
#endif
        }
        else
        {
#ifdef EDITOR_MODE
            bool bReflectCheckFlag = CComponentProxyManager::GetInstance()->GetReflectCheckFlag();
            CComponentProxyManager::GetInstance()->SetReflectCheckFlag(false);
            value->ReflectData(*pSerializer);
            CComponentProxyManager::GetInstance()->SetReflectCheckFlag(bReflectCheckFlag);
#else
            BEATS_ASSERT(false, _T("A pointer should be initialize to NULL before it is deserialized!"));
#endif
        }
    }
#ifdef EDITOR_MODE
    else
    {
        value = NULL;
    }
#endif
}
template<typename T>
inline void DeserializeVariable(SharePtr<T>& value, CSerializer* pSerializer)
{
    bool bHasInstance = false;
    *pSerializer >> bHasInstance;
    if(bHasInstance)
    {
        size_t uDataSize, uGuid, uId, uStartPos;
        uStartPos = pSerializer->GetReadPos();
        *pSerializer >> uDataSize;
        *pSerializer >> uGuid;
        *pSerializer >> uId;
        if (value.Get() == NULL)
        {
#ifdef EDITOR_MODE
            CPropertyDescriptionBase* pProperty = CComponentProxyManager::GetInstance()->GetCurrReflectDescription();
            if (pProperty != NULL)
            {
                BEATS_ASSERT(pProperty != NULL && pProperty->GetInstanceComponent() != NULL &&  pProperty->GetInstanceComponent()->GetHostComponent() != NULL);
                value = dynamic_cast<T*>(pProperty->GetInstanceComponent()->GetHostComponent());
                BEATS_ASSERT(value != NULL);
            }
            else
            {
#endif
                value = dynamic_cast<T*>(CComponentInstanceManager::GetInstance()->CreateComponent(uGuid, false, true, 0xFFFFFFFF, false, pSerializer));
                BEATS_ASSERT(uStartPos + uDataSize == pSerializer->GetReadPos(),
                    _T("Component Data Not Match!\nGot an error when Deserialize a pointer of component 0x%x %s instance id %d\nRequired size: %d, Actual size: %d"), uGuid, value->GetClassStr(), uId, uDataSize, pSerializer->GetReadPos() - uStartPos);
#ifdef EDITOR_MODE
            }
#endif
        }
        else
        {
#ifdef EDITOR_MODE
            value->ReflectData(*pSerializer);
#else
            BEATS_ASSERT(false, _T("A pointer should be initialize to NULL before it is deserialized!"));
#endif
        }
#ifndef EDITOR_MODE
        if ((uStartPos + uDataSize) != pSerializer->GetReadPos())
        {
            TCHAR szInfo[256];
            _stprintf(szInfo, _T("Got an error when creating data for component"));
            MessageBox(NULL, szInfo, _T("Component Data Not Match!"), MB_OK | MB_ICONERROR);
        }
#endif
    }
#ifdef EDITOR_MODE
    else
    {
        value.Destroy(true);
    }
#endif
}

#ifdef EDITOR_MODE
template<typename T>
inline void ResizeVector(std::vector<T>& value, size_t uCount)
{
    value.resize(uCount);
}

template<typename T>
inline void ResizeVector(std::vector<T*>& value, size_t uCount)
{
    if (value.size() > uCount)
    {
        for (size_t i = 0; i < value.size() - uCount; ++i)
        {
            BEATS_ASSERT(value.back() == NULL || dynamic_cast<CComponentBase*>(value.back()) != NULL);
            value.pop_back();
        }
    }
    else
    {
        value.resize(uCount);
    }
}

template<typename T1, typename T2>
inline void ClearMap(std::map<T1, T2>& value)
{
    value.clear();
}

#endif

template<typename T>
inline void DeserializeVariable(std::vector<T>& value, CSerializer* pSerializer)
{
    size_t childCount = 0;
    *pSerializer >> childCount;
#ifdef EDITOR_MODE
    ResizeVector(value, childCount);
#else
    value.resize(childCount);
#endif
    for (size_t i = 0; i < childCount; ++i)
    {
#ifdef EDITOR_MODE
        CPropertyDescriptionBase* pCurrReflectProperty = CComponentProxyManager::GetInstance()->GetCurrReflectDescription();
        bool bUnderPtrReflection = pCurrReflectProperty != NULL && pCurrReflectProperty->GetType() == eRPT_Ptr && CComponentProxyManager::GetInstance()->GetReflectCheckFlag() == false;
        if (pCurrReflectProperty != NULL && !bUnderPtrReflection)
        {
            BEATS_ASSERT(pCurrReflectProperty->GetType() == eRPT_List);
            CPropertyDescriptionBase* pSubProperty = pCurrReflectProperty->GetChild(i);
            BEATS_ASSERT(pSubProperty != NULL);
            CComponentProxyManager::GetInstance()->SetCurrReflectDescription(pSubProperty);
        }
#endif
        BEATS_ASSERT(typeid(T) != typeid(bool), _T("std::vector<bool> is not supported! it's a very specific version of stl, it's not a container!"));
        DeserializeVariable(value[i], pSerializer);
#ifdef EDITOR_MODE
        CComponentProxyManager::GetInstance()->SetCurrReflectDescription(pCurrReflectProperty);
#endif
    }
}

template<typename T1, typename T2>
inline void DeserializeVariable(std::map<T1, T2>& value, CSerializer* pSerializer)
{
    EReflectPropertyType keyType;
    EReflectPropertyType valueType;
    size_t childCount = 0;
    *pSerializer >> keyType;
    *pSerializer >> valueType;
    *pSerializer >> childCount;
#ifdef EDITOR_MODE
    ClearMap(value);
#else
    BEATS_ASSERT(value.size() == 0, _T("map should be empty when deserialize!"));
#endif
    for (size_t i = 0; i < childCount; ++i)
    {
        T1 key;
        InitValue(key);
#ifdef EDITOR_MODE
        CPropertyDescriptionBase* pCurrReflectProperty = CComponentProxyManager::GetInstance()->GetCurrReflectDescription();
        bool bUnderPtrReflection = pCurrReflectProperty != NULL && pCurrReflectProperty->GetType() == eRPT_Ptr && CComponentProxyManager::GetInstance()->GetReflectCheckFlag() == false;
        if (pCurrReflectProperty != NULL && !bUnderPtrReflection)
        {
            BEATS_ASSERT(pCurrReflectProperty->GetType() == eRPT_Map);
            CPropertyDescriptionBase* pSubProperty = pCurrReflectProperty->GetChild(i);
            BEATS_ASSERT(pSubProperty != NULL);
            CPropertyDescriptionBase* pKeyProperty = pSubProperty->GetChild(0);
            BEATS_ASSERT(pKeyProperty != NULL);
            CComponentProxyManager::GetInstance()->SetCurrReflectDescription(pKeyProperty);
        }
#endif
        DeserializeVariable(key, pSerializer);
        T2 myValue;
        InitValue(myValue);
#ifdef EDITOR_MODE
        if (pCurrReflectProperty != NULL && !bUnderPtrReflection)
        {
            BEATS_ASSERT(pCurrReflectProperty->GetType() == eRPT_Map);
            CPropertyDescriptionBase* pSubProperty = pCurrReflectProperty->GetChild(i);
            CPropertyDescriptionBase* pValueProperty = pSubProperty->GetChild(1);
            BEATS_ASSERT(pValueProperty != NULL);
            CComponentProxyManager::GetInstance()->SetCurrReflectDescription(pValueProperty);
        }
#endif
        DeserializeVariable(myValue, pSerializer);
        BEATS_ASSERT(value.find(key) == value.end() || CComponentProxyManager::GetInstance()->IsLoadingFile(), _T("A map can't have two same key value!"));
        value[key] = myValue;
#ifdef EDITOR_MODE
        CComponentProxyManager::GetInstance()->SetCurrReflectDescription(pCurrReflectProperty);
#endif
    }
}

template<>
inline EReflectPropertyType GetEnumType(SReflectFilePath& value, CSerializer* pSerializer)
{
    *pSerializer << eRPT_File;
    *pSerializer << value.m_value;
    return eRPT_File;
}

template<typename T>
inline EReflectPropertyType GetEnumType(T*& /*value*/, CSerializer* pSerializer)
{
    size_t guid = T::REFLECT_GUID;
    T* pTestParam = (T*)(guid);
    EReflectPropertyType eReturnType = eRPT_Invalid;
    CComponentBase* pReflect = dynamic_cast<CComponentBase*>(pTestParam);
    if (pReflect != NULL)
    {
        eReturnType = eRPT_Ptr;
        *pSerializer << (int)eRPT_Ptr;
        *pSerializer << T::REFLECT_GUID;
    }
    BEATS_ASSERT(eReturnType != eRPT_Invalid, _T("Unknown type!"));
    return eReturnType;
}

#define REGISTER_PROPERTY(classType, enumType)\
template<>\
inline EReflectPropertyType GetEnumType(classType& value, CSerializer* pSerializer)\
{\
    if (pSerializer != NULL)\
    {\
        *pSerializer << ((size_t)enumType);\
        *pSerializer << value;\
    }\
    return enumType;\
}

#define REGISTER_PROPERTY_SHAREPTR(classType, enumType)\
    template<typename T>\
    inline EReflectPropertyType GetEnumType(classType<T>& /*value*/, CSerializer* pSerializer)\
{\
    if (pSerializer != NULL)\
    {\
        T* tmp = NULL;\
        GetEnumType(tmp, pSerializer);\
    }\
    return enumType;\
}

#define REGISTER_PROPERTY_TEMPLATE1(classType, enumType)\
    template<typename T>\
    inline EReflectPropertyType GetEnumType(classType<T>& /*value*/, CSerializer* pSerializer)\
    {\
        if (pSerializer != NULL)\
        {\
            *pSerializer << (size_t)enumType;\
            T tmp;\
            InitValue(tmp);\
            GetEnumType(tmp, pSerializer);\
        }\
        return enumType;\
    }

#define REGISTER_PROPERTY_TEMPLATE2(classType, enumType)\
    template<typename T1, typename T2>\
    inline EReflectPropertyType GetEnumType(classType<T1, T2>& /*value*/, CSerializer* pSerializer)\
    {\
        if (pSerializer != NULL)\
        {\
            *pSerializer << (size_t)enumType;\
            T1 tmp1;\
            InitValue(tmp1);\
            GetEnumType(tmp1, pSerializer);\
            T2 tmp2;\
            InitValue(tmp2);\
            GetEnumType(tmp2, pSerializer);\
        }\
        return enumType;\
    }

#define REGISTER_PROPERTY_DESC(enumType, propertyDescriptionType)\
    CPropertyDescriptionBase* CreateProperty_##propertyDescriptionType(CSerializer* serilaizer)\
    {\
        return new propertyDescriptionType(serilaizer);\
    }\
    struct SRegister_Creator_##enumType\
    {\
        SRegister_Creator_##enumType()\
        {\
            CComponentProxyManager::GetInstance()->RegisterPropertyCreator(enumType, CreateProperty_##propertyDescriptionType);\
        }\
    }enumType##_creator_launcher;

#ifdef EXPORT_TO_EDITOR

inline const TCHAR* GenEnumParamStr(const std::vector<TString>& enumStringArray, const TCHAR* pszParam = NULL)
{
    static TString strEnumParam;
    strEnumParam.clear();
    strEnumParam.append(UIParameterAttrStr[eUIPAT_EnumStringArray]).append(PROPERTY_KEYWORD_SPLIT_STR);
    size_t uCount = enumStringArray.size();
    BEATS_ASSERT(uCount > 0, _T("Enum string array is empty!"));
    for (size_t i = 0; i < uCount; ++i)
    {
        strEnumParam.append(enumStringArray[i]);
        if (i < uCount - 1)
        {
            strEnumParam.append(_T("@"));
        }
    }
    if(pszParam != NULL)
    {
        strEnumParam.append(PROPERTY_PARAM_SPLIT_STR).append(pszParam);
    }
    return strEnumParam.c_str();
}

inline const TCHAR* GenEnumParamStr(const TCHAR* enumStringArray[], const TCHAR* pszParam = NULL)
{
    static TString strEnumParam;
    strEnumParam.clear();
    strEnumParam.append(UIParameterAttrStr[eUIPAT_EnumStringArray]).append(PROPERTY_KEYWORD_SPLIT_STR);
    size_t uCount = sizeof(enumStringArray);
    BEATS_ASSERT(uCount > 0, _T("Enum string array is empty!"));
    for (size_t i = 0; i < uCount; ++i)
    {
        strEnumParam.append(enumStringArray[i]);
        if (i < uCount - 1)
        {
            strEnumParam.append(_T("@"));
        }
    }
    if(pszParam != NULL)
    {
        strEnumParam.append(PROPERTY_PARAM_SPLIT_STR).append(pszParam);
    }
    return strEnumParam.c_str();
}

inline bool CheckIfEnumHasExported(const TString& strEnumName)
{
    static std::set<TString> exportRecord;

    bool bRet = exportRecord.find(strEnumName) == exportRecord.end();
    if (bRet)
    {
        exportRecord.insert(strEnumName);
    }
    return !bRet;
}

#define GEN_ENUM_PARAM(stringarray, propertyParam) GenEnumParamStr(stringarray, propertyParam)

template<typename T>
void DeclareProperty(CSerializer& serializer, T& property, const TCHAR* pszPropertyStr, bool bEditable, int color, const TCHAR* pszDisplayName, const TCHAR* pszCatalog, const TCHAR* pszTip, const TCHAR* pszParameter)
{
    BEATS_ASSERT(serializer.GetUserData() != NULL, _T("Serializer doesn't contain an user data!nPlease make sure you enalbe EDITOR_MODE and Disable EXPORT_TO_EDITOR when launch editor!"));
    serializer << (bool) true;
    EReflectPropertyType propertyType = GetEnumType(property, &serializer);
    size_t nPropertyDataSizeHolder = serializer.GetWritePos();
    serializer << nPropertyDataSizeHolder;
    const TCHAR* pszParam = pszParameter;
    if (propertyType == eRPT_Enum && pszParam != NULL)
    {
        size_t uEnumStringArrayNameLen = _tcslen(UIParameterAttrStr[eUIPAT_EnumStringArray]);
        if (_tcslen(pszParam) > uEnumStringArrayNameLen && memcmp(pszParam, UIParameterAttrStr[eUIPAT_EnumStringArray], uEnumStringArrayNameLen) == 0)
        {
            const char* pszTypeName = typeid(property).name();
            TCHAR szNameBuffer[128];
            CStringHelper::GetInstance()->ConvertToTCHAR(&pszTypeName[strlen("enum ")], szNameBuffer, 128);
            TString strEnumName = szNameBuffer;
            if (CheckIfEnumHasExported(strEnumName))
            {
                pszParam = NULL;/*It's not necessary to export the same enum string array value, so ignore it!*/
            }
        }
    }
    serializer << (bool)bEditable << color << (pszDisplayName ? pszDisplayName : pszPropertyStr) << (pszCatalog ? pszCatalog : _T("")) << (pszTip ? pszTip : _T("")) << pszPropertyStr << (pszParam ? pszParam : _T(""));
    size_t propertyDataSize = serializer.GetWritePos() - nPropertyDataSizeHolder;
    serializer.SetWritePos(nPropertyDataSizeHolder);
    serializer << propertyDataSize;
    serializer.SetWritePos(nPropertyDataSizeHolder + propertyDataSize);
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uPropertyCount);
}

#define DECLARE_PROPERTY(serializer, property, editable, color, displayName, catalog, tip, parameter)\
    DeclareProperty(serializer, property, _T(#property), editable, color, displayName, catalog, tip, parameter)

#define DECLARE_DEPENDENCY(serializer, ptrProperty, displayName, dependencyType)\
{\
    BEATS_ASSERT(serializer.GetUserData() != NULL, _T("Serializer doesn't contain an user data!\nPlease make sure you enalbe EDITOR_MODE and Disable EXPORT_TO_EDITOR when launch editor!"));\
    serializer << (bool) false << (bool)false << dependencyType << ptrProperty->REFLECT_GUID << displayName << _T(#ptrProperty);\
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uDependencyCount);\
}

#define DECLARE_DEPENDENCY_LIST(serializer, ptrProperty, displayName, dependencyType)\
{\
    BEATS_ASSERT(serializer.GetUserData() != NULL, _T("Serializer doesn't contain an user data!\nPlease make sure you enalbe EDITOR_MODE and Disable EXPORT_TO_EDITOR when launch editor!"));\
    ptrProperty.resize(1);\
    serializer << (bool) false << (bool)true << dependencyType << ptrProperty[0]->REFLECT_GUID << displayName << _T(#ptrProperty);\
    ptrProperty.resize(0);\
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uDependencyCount);\
}

#else

#ifndef EDITOR_MODE
    #define DECLARE_PROPERTY(serializer, property, editable, color, displayName, catalog, tip, parameter) DeserializeVariable(property, &serializer);

    #define DECLARE_DEPENDENCY(serializer, ptrProperty, displayName, dependencyType)\
    {\
        size_t uLineCount = 0;\
        serializer >> uLineCount;\
        BEATS_ASSERT(uLineCount <= 1, _T("Data error:\nWe want a dependency data, but got %d line count!"), uLineCount);\
        ptrProperty = NULL;\
        if (uLineCount == 1)\
        {\
            size_t uInstanceId, uGuid;\
            serializer >> uInstanceId >> uGuid;\
            CComponentInstanceManager::GetInstance()->AddDependencyResolver(NULL, 0 , uGuid, uInstanceId, &ptrProperty, false);\
        }\
    }

    #define DECLARE_DEPENDENCY_LIST(serializer, ptrProperty, displayName, dependencyType)\
    {\
        size_t uLineCount = 0;\
        serializer >> uLineCount;\
        ptrProperty.clear();\
        for (size_t i = 0; i < uLineCount; ++i)\
        {\
            size_t uInstanceId, uGuid;\
            serializer >> uInstanceId >> uGuid;\
            CComponentInstanceManager::GetInstance()->AddDependencyResolver(NULL, i , uGuid, uInstanceId, &ptrProperty, true);\
        }\
    }

#else

    template<typename T>
    void DeclareProperty(CSerializer& serializer, CComponentBase* pComponent, T& property, const TCHAR* pszPropertyStr)
    {
        CDependencyDescription* pDependencyDescription = CComponentProxyManager::GetInstance()->GetCurrReflectDependency();
        if (pDependencyDescription == NULL)
        {
            CPropertyDescriptionBase* pDescriptionBase = CComponentProxyManager::GetInstance()->GetCurrReflectDescription();
            bool bReflectCheckFlag = CComponentProxyManager::GetInstance()->GetReflectCheckFlag();
            if (!bReflectCheckFlag || pDescriptionBase->GetBasicInfo()->m_variableName.compare(pszPropertyStr) == 0)
            {
                bool bNeedHandleSync = true;
                if (pDescriptionBase != NULL && !CComponentProxyManager::GetInstance()->IsLoadingFile())
                {
                    bNeedHandleSync = !pComponent->OnPropertyChange(&property, &serializer);
                }
                if (bNeedHandleSync)
                {
                    CComponentProxyManager::GetInstance()->SetReflectCheckFlag(false);
                    DeserializeVariable(property, &serializer);
                    CComponentProxyManager::GetInstance()->SetReflectCheckFlag(bReflectCheckFlag);
                }
                if (pDescriptionBase != NULL && bReflectCheckFlag)
                {
                    return;
                }
            }
        }
    }
    #define DECLARE_PROPERTY(serializer, property, editable, color, displayName, catalog, tip, parameter) \
        DeclareProperty(serializer, this, property, _T(#property))

    template<typename T>
    void DeclareDependency(CSerializer& serializer, CComponentBase* pComponent, T& ptrProperty, const TCHAR* pszPropertyStr)
    {
        if (CComponentInstanceManager::GetInstance()->GetForbidDependencyResolve())
        {
            size_t uLineCount, uInstanceId, uGuid;
            serializer >> uLineCount;
            for (size_t i = 0; i < uLineCount; ++i)
                serializer >> uInstanceId >> uGuid;
        }
        else
        {
            CPropertyDescriptionBase* pDescriptionBase = CComponentProxyManager::GetInstance()->GetCurrReflectDescription();
            CDependencyDescription* pDependency = CComponentProxyManager::GetInstance()->GetCurrReflectDependency();
            bool bReflectCheckFlag = CComponentProxyManager::GetInstance()->GetReflectCheckFlag();
            if (!bReflectCheckFlag || pDescriptionBase  == NULL)
            {
                if (!bReflectCheckFlag || pDependency == NULL || _tcscmp(pDependency->GetVariableName(), pszPropertyStr) == 0)
                {
                    size_t uLineCount = 0;
                    serializer >> uLineCount;
                    BEATS_ASSERT(uLineCount <= 1, _T("Data error:We want a dependency data, but got %d line count!"), uLineCount);
                    if (uLineCount == 1)
                    {
                        size_t uInstanceId, uGuid;
                        serializer >> uInstanceId >> uGuid;
                        bool bNeedSnyc = true;
                        if (pDependency != NULL && !CComponentProxyManager::GetInstance()->IsLoadingFile())
                        {
                            EDependencyChangeAction action;
                            CComponentProxy* pOperateProxy = NULL;
                            pDependency->GetCurrActionParam(action, pOperateProxy);
                            if(pOperateProxy->IsInitialized())
                            {
                                BEATS_ASSERT(CComponentInstanceManager::GetInstance()->GetComponentInstance(uInstanceId, uGuid) == pOperateProxy->GetHostComponent());
                                bNeedSnyc = !pComponent->OnDependencyChange(&ptrProperty, pOperateProxy->GetHostComponent());
                            }
                        }
                        if (bNeedSnyc)
                        {
                            ptrProperty = NULL;
                            size_t uDepGuid = ptrProperty->REFLECT_GUID;
                            bool bIsParent = CComponentProxyManager::GetInstance()->IsParent(uDepGuid, uGuid);
                            BEATS_ASSERT(bIsParent,
                                _T("GUID mismatched. GUID of dependency:0x%x, GUID in datafile:0x%x"),
                                uDepGuid, uGuid);
                            if (bIsParent)
                            {
                                CComponentInstanceManager::GetInstance()->AddDependencyResolver(NULL, 0 , uGuid, uInstanceId, &ptrProperty, false);
                            }
                        }
                    }
                    else
                    {
                        if (ptrProperty != NULL && !CComponentProxyManager::GetInstance()->IsLoadingFile())
                        {
                            pComponent->OnDependencyChange(&ptrProperty, NULL);
                        }
                        ptrProperty = NULL;
                    }
                    if (pDependency != NULL && bReflectCheckFlag)
                    {
                        return;
                    }
                }
            }
        }
    }
    #define DECLARE_DEPENDENCY(serializer, ptrProperty, displayName, dependencyType)\
        DeclareDependency(serializer, this, ptrProperty, _T(#ptrProperty))

    template<typename T>
    void DeclareDependencyList(CSerializer& serializer, CComponentBase* pComponent, T& ptrProperty, const TCHAR* pszPropertyStr)
    {
        if (CComponentInstanceManager::GetInstance()->GetForbidDependencyResolve())
        {
            size_t uLineCount, uInstanceId, uGuid;
            serializer >> uLineCount;
            for (size_t i = 0; i < uLineCount; ++i)
                serializer >> uInstanceId >> uGuid;
        }
        else
        {
            CPropertyDescriptionBase* pDescriptionBase = CComponentProxyManager::GetInstance()->GetCurrReflectDescription();
            CDependencyDescription* pDependencyList = CComponentProxyManager::GetInstance()->GetCurrReflectDependency();
            bool bReflectCheckFlag = CComponentProxyManager::GetInstance()->GetReflectCheckFlag();
            if (!bReflectCheckFlag || pDescriptionBase == NULL)
            {
                if (!bReflectCheckFlag || pDependencyList == NULL || _tcscmp(pDependencyList->GetVariableName(), pszPropertyStr) == 0)
                {
                    size_t uLineCount = 0;
                    serializer >> uLineCount;
                    bool bNeedSnyc = true;
                    if (pDependencyList != NULL && !CComponentProxyManager::GetInstance()->IsLoadingFile())
                    {
                        EDependencyChangeAction action;
                        CComponentProxy* pOperateProxy = NULL;
                        pDependencyList->GetCurrActionParam(action, pOperateProxy);
                        if((pOperateProxy->IsInitialized() || action == eDCA_Delete) && action != eDCA_Ordered)
                        {
                            bNeedSnyc = !pComponent->OnDependencyListChange(&ptrProperty, action, pOperateProxy->GetHostComponent());
                        }
                    }
                    if (bNeedSnyc)
                    {
                        ptrProperty.clear();
                        ptrProperty.resize(1);
                        size_t uDepGuid = ptrProperty[0]->REFLECT_GUID;
                        ptrProperty.clear();
                        for (size_t i = 0; i < uLineCount; ++i)
                        {
                            size_t uInstanceId, uGuid;
                            serializer >> uInstanceId >> uGuid;
                            bool bIsParent = CComponentProxyManager::GetInstance()->IsParent(uDepGuid, uGuid);
                            BEATS_ASSERT(bIsParent,
                                _T("GUID mismatched. GUID of dependency:0x%x, GUID in datafile:0x%x"),
                                uDepGuid, uGuid);
                            if (bIsParent)
                            {
                                CComponentInstanceManager::GetInstance()->AddDependencyResolver(NULL, i , uGuid, uInstanceId, &ptrProperty, true);
                            }
                        }
                    }
                    if (pDependencyList != NULL && bReflectCheckFlag)
                    {
                        return;
                    }
                }
            }
        }
    }

    #define DECLARE_DEPENDENCY_LIST(serializer, ptrProperty, displayName, dependencyType)\
    DeclareDependencyList(serializer, this, ptrProperty, _T(#ptrProperty))

    #endif
#endif

#ifdef EDITOR_MODE
#define UPDATE_PROPERTY_PROXY(property)\
    {\
        CComponentProxy* pProxy = GetProxyComponent();\
        if(pProxy)\
        {\
            CPropertyDescriptionBase* pProperty = pProxy->GetPropertyDescription(_T(#property));\
            BEATS_ASSERT(pProperty != NULL, _T("Can not find property %s in component %s GUID:0x%x"), _T(#property), this->GetClassStr(), this->GetGuid());\
            BEATS_ASSERT((pProperty->GetType() >= eRPT_Bool && pProperty->GetType() <= eRPT_Str) || pProperty->GetType() == eRPT_Color || pProperty->GetType() == eRPT_Enum || (pProperty->GetType() >= eRPT_Vec2F && pProperty->GetType() <= eRPT_Vec4F) );\
            CSerializer serializer;\
            serializer << property;\
            pProperty->Deserialize(serializer);\
        }\
    }
#else
#define UPDATE_PROPERTY_PROXY(property)
#endif

#ifdef EDITOR_MODE
#define HIDE_PROPERTY(property)\
    {\
        CPropertyDescriptionBase* pDescriptionBase = CComponentProxyManager::GetInstance()->GetCurrReflectDescription();\
        CDependencyDescription* pDependencyList = CComponentProxyManager::GetInstance()->GetCurrReflectDependency();\
        if (pDescriptionBase == NULL && pDependencyList == NULL)\
        {\
            CComponentProxy* pProxy = GetProxyComponent();\
            if(pProxy)\
            {\
                CPropertyDescriptionBase* pProperty = pProxy->GetPropertyDescription(_T(#property));\
                BEATS_ASSERT(pProperty != NULL, _T("Can not find property %s in component %s GUID:0x%x"), _T(#property), this->GetClassStr(), this->GetGuid());\
                pProperty->SetHide(true);\
            }\
        }\
    }
#else
#define HIDE_PROPERTY(property)
#endif

    #define GET_VAR_NAME(var) (var, _T(#var))

    REGISTER_PROPERTY(bool, eRPT_Bool);
    REGISTER_PROPERTY(float, eRPT_Float);
    REGISTER_PROPERTY(double, eRPT_Double);
    REGISTER_PROPERTY(int, eRPT_Int);
    REGISTER_PROPERTY(size_t, eRPT_UInt);
    REGISTER_PROPERTY(TString, eRPT_Str);
    REGISTER_PROPERTY(CComponentBase*, eRPT_Ptr);
    REGISTER_PROPERTY(CColor, eRPT_Color);
    REGISTER_PROPERTY(CVec2, eRPT_Vec2F);
    REGISTER_PROPERTY(CVec3, eRPT_Vec3F);
    REGISTER_PROPERTY(CVec4, eRPT_Vec4F);

    REGISTER_PROPERTY_TEMPLATE1(std::vector, eRPT_List);
    REGISTER_PROPERTY_TEMPLATE2(std::map, eRPT_Map);
    REGISTER_PROPERTY_SHAREPTR(SharePtr, eRPT_Ptr);


    template<>
    inline EReflectPropertyType GetEnumType(CColorSpline& /*value*/, CSerializer* pSerializer)
    {
        *pSerializer << (size_t)eRPT_GradientColor;
        return eRPT_GradientColor;
    }

#endif