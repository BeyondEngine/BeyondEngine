#ifndef BEYONDENGINE_COMPONENT_PROPERTY_PROPERTYDESCRIPTIONBASE_H__INCLUDE
#define BEYONDENGINE_COMPONENT_PROPERTY_PROPERTYDESCRIPTIONBASE_H__INCLUDE

#include "PropertyPublic.h"
#include "Utility/RapidXML/rapidxml.hpp"

class CComponentProxy;
class CSerializer;
class TiXmlElement;
template<typename T>
class SharePtr;

class  CPropertyDescriptionBase
{
public:
    CPropertyDescriptionBase(EReflectPropertyType type);
    CPropertyDescriptionBase(const CPropertyDescriptionBase& rRef);

    virtual ~CPropertyDescriptionBase();

    EReflectPropertyType GetType() const;
    void SetType(EReflectPropertyType type);

    CComponentProxy* GetOwner() const;
    virtual void SetOwner(CComponentProxy* pOwner);
    CComponentProxy* GetRootOwner() const;

    CPropertyDescriptionBase* GetParent() const;
    void SetParent(CPropertyDescriptionBase* pParent);

    void SetHide(bool bHide);
    bool IsHide() const;

    // m_pBasicInfo is a shareptr, be care that all the basic info will be changed when call
    // SetBasicInfo, to avoid impact the other basic info ,call ResetBasicInfo.
    void ResetBasicInfo(const SharePtr<SBasicPropertyInfo>& pInfo);
    void SetBasicInfo(const SBasicPropertyInfo& info);
    const SharePtr<SBasicPropertyInfo>& GetBasicInfo() const;

    virtual CPropertyDescriptionBase* InsertChild(CPropertyDescriptionBase* pProperty, uint32_t uPreIndex = 0xFFFFFFFF);
    virtual bool RemoveChild(CPropertyDescriptionBase* pProperty, bool bDelete = true);
    virtual void RemoveAllChild(bool bDelete = true);
    const std::vector<CPropertyDescriptionBase*>& GetChildren() const;
    void Save();
    template<typename T>
    void InitializeValue(const T& value)
    {
        for (uint32_t i = 0; i < eVT_Count; ++i)
        {
            if (m_valueArray[i] == NULL)
            {
                m_valueArray[i] = new T(value);
            }
            else
            {
                (*(T*)m_valueArray[i]) = value;
            }
        }
    }
    template<typename T>
    void DestroyValue()
    {
        for (uint32_t i = 0; i < eVT_Count; ++i)
        {
            T* pValue = (T*)m_valueArray[i];
            BEATS_SAFE_DELETE(pValue);
        }
    }

    void* GetValue(EValueType type) const;
    bool CanSyncToHost() const;
    void SetValueWithType(void* pValue, EValueType type, bool bForceUpdateHostComponent = false);
    void SetValueByString(const TCHAR* pszStr, EValueType type = eVT_CurrentValue, bool bUpdateHost = true);
    bool DeserializeBasicInfo(CSerializer& serializer);

    virtual void Initialize();
    virtual void Uninitialize();
    bool IsInitialized() const;
    virtual bool IsContainerProperty();
    virtual void SerializeContainerElementLocation(CSerializer& serializer, CPropertyDescriptionBase* pChildProperty);

    //Editor mode only
    virtual bool OnChildChanged(uint32_t uChildIndex);

    virtual void SetValueList(const std::vector<TString>& valueList);
    virtual CComponentProxy* GetInstanceComponent() const;
    virtual uint32_t HACK_GetPtrReflectGuid() const;
    // When we delete a component instance, it may be one instance of ptrproperty, so we need to inform the ptr property not to delete its instance again.
    virtual void HACK_InformPtrPropertyToDeleteInstance();

    virtual CPropertyDescriptionBase* Clone(bool bCloneValue);
    virtual CPropertyDescriptionBase* CreateNewInstance() = 0;

    virtual void GetValueAsChar(EValueType type, char* pOut) const = 0;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) = 0;

    // It's very important to know what this function mean.
    // Compare current data(property value in your property grid) to Default Value(which is determined in code of macro DECLARE_PROPERTY) and
    // XML data(the data you saved last time).
    // Notice: Default Value stores in wxPGProperty's defaultValue; current value stores in wxPGProperty's value; XML value stores in m_refVariableAddr.
    virtual bool IsDataSame(bool bWithDefaultOrXML) = 0;

    // This function will be called when Serialize from macro to data.
    virtual void AnalyseUIParameter(const TCHAR* parameter) = 0;

    virtual void SaveToXML(rapidxml::xml_node<>* pParentNode) = 0;
    virtual void LoadFromXML(rapidxml::xml_node<>* pNode) = 0;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) = 0;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) = 0;
    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) = 0;

    void SetNoSyncHost(bool bValue);
    uint32_t GetChildIndex(const CPropertyDescriptionBase* pChildProperty) const;

protected:
    bool m_bInitialized;
    bool m_bHide;
    bool m_bNoSyncToHost;
    EReflectPropertyType m_type;
    CComponentProxy* m_pOwner;
    SharePtr<SBasicPropertyInfo>* m_pBasicInfo;
    void* m_valueArray[eVT_Count];
    CPropertyDescriptionBase* m_pParent;
    std::vector<CPropertyDescriptionBase*>* m_pChildren;
};

#endif