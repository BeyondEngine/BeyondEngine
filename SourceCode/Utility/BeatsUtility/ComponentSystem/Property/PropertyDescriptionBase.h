#ifndef BEATS_COMPONENTS_PROPERTY_PROPERTYDESCRIPTIONBASE_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_PROPERTYDESCRIPTIONBASE_H__INCLUDE

#include "PropertyPublic.h"

class CComponentProxy;
class CSerializer;
class TiXmlElement;
template<typename T>
class SharePtr;

class CPropertyDescriptionBase
{
public:
    CPropertyDescriptionBase(EReflectPropertyType type);
    CPropertyDescriptionBase(const CPropertyDescriptionBase& rRef);

    virtual ~CPropertyDescriptionBase();

    EReflectPropertyType GetType() const;
    void SetType(EReflectPropertyType type);

    CComponentProxy* GetOwner() const;
    void SetOwner(CComponentProxy* pOwner);

    CPropertyDescriptionBase* GetParent() const;
    void SetParent(CPropertyDescriptionBase* pParent);

    void SetHide(bool bHide);
    bool IsHide() const;

    // m_pBasicInfo is a shareptr, be care that all the basic info will be changed when call
    // SetBasicInfo, to avoid impact the other basic info ,call ResetBasicInfo.
    void ResetBasicInfo(const SharePtr<SBasicPropertyInfo>& pInfo);
    void SetBasicInfo(const SBasicPropertyInfo& info);
    const SharePtr<SBasicPropertyInfo>& GetBasicInfo() const;

    virtual CPropertyDescriptionBase* AddChild(CPropertyDescriptionBase* pProperty);
    virtual bool DeleteChild(CPropertyDescriptionBase* pProperty, bool bKeepChildOrder = false);
    virtual void DeleteAllChild();
    CPropertyDescriptionBase* GetChild(size_t i) const;
    std::vector<CPropertyDescriptionBase*>& GetChildren();
    size_t GetChildrenCount() const;
    void Save();
    template<typename T>
    void InitializeValue(const T& value)
    {
        for (size_t i = 0; i < eVT_Count; ++i)
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
        for (size_t i = 0; i < eVT_Count; ++i)
        {
            T* pValue = (T*)m_valueArray[i];
            BEATS_SAFE_DELETE(pValue);
        }
    }

    void* GetValue(EValueType type) const;
    void SetValueWithType(void* pValue, EValueType type, bool bForceUpdateHostComponent = false);

    virtual void Initialize();
    virtual void Uninitialize();
    virtual bool IsContainerProperty();
    //Editor mode only
    virtual void SetValueList(const std::vector<TString>& valueList);
    virtual CComponentProxy* GetInstanceComponent() const;

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

    virtual void SaveToXML(TiXmlElement* pParentNode) = 0;
    virtual void LoadFromXML(TiXmlElement* pNode) = 0;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) = 0;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) = 0;
    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) = 0;

protected:
    bool DeserializeBasicInfo(CSerializer& serializer);

protected:
    bool m_bInitialized;
    bool m_bHide;
    EReflectPropertyType m_type;
    CComponentProxy* m_pOwner;
    SharePtr<SBasicPropertyInfo>* m_pBasicInfo;
    void* m_valueArray[eVT_Count];
    CPropertyDescriptionBase* m_pParent;
    std::vector<CPropertyDescriptionBase*>* m_pChildren;
};

#endif