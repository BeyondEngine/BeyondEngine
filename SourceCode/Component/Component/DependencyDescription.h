#ifndef BEYONDENGINE_COMPONENT_DEPENDENCYDESCRIPTION_H__INCLUDE
#define BEYONDENGINE_COMPONENT_DEPENDENCYDESCRIPTION_H__INCLUDE

class CDependencyDescriptionLine;
class CComponentProxy;
class CSerializer;
#include "Utility/RapidXML/rapidxml.hpp"

enum EDependencyType
{
    eDT_Strong,
    eDT_Weak,

    eDT_Count,
    eDT_Force32Bit = 0xFFFFFFFF
};

enum EDependencyChangeAction
{
    eDCA_Add,
    eDCA_Delete,
    eDCA_Change,
    eDCA_Ordered,

    eDCA_Count,
    eDCA_Force32Bit = 0xFFFFFFFF
};

class CDependencyDescription
{
public:
    CDependencyDescription(EDependencyType type, uint32_t dependencyGuid, CComponentProxy* pOwner, uint32_t uIndex, bool bIsList);
    ~CDependencyDescription();

    CDependencyDescriptionLine* GetDependencyLine(uint32_t uIndex = 0) const;
    CDependencyDescriptionLine* SetDependency(uint32_t uIndex, CComponentProxy* pComponent);
    uint32_t GetDependencyLineCount() const;
    void SetOwner(CComponentProxy* pOwner);
    CComponentProxy* GetOwner();
    uint32_t GetDependencyGuid() const;
    void Hide();
    void Show();
    bool IsVisible() const;
    bool IsListType() const;
    uint32_t GetIndex() const;

    const TCHAR* GetDisplayName();
    void SetDisplayName(const TCHAR* pszName);
    const TCHAR* GetVariableName();
    void SetVariableName(const TCHAR* pszName);
    CDependencyDescriptionLine* AddDependency(CComponentProxy* pComponentInstance);
    void RemoveDependencyLine(CDependencyDescriptionLine* pComponentInstance);
    void RemoveDependencyByIndex(uint32_t uIndex);
    void SwapLineOrder(uint32_t uSourceIndex, uint32_t uTargetIndex);
    bool IsInDependency(CComponentProxy* pComponentInstance);
    void SaveToXML(rapidxml::xml_node<>* pParentNode);
    void LoadFromXML(rapidxml::xml_node<>* pNode);
    EDependencyType GetType();
    uint32_t GetSelectedDependencyIndex() const;
    void SetSelectedDependencyIndex(uint32_t index);
    bool IsMatch(CComponentProxy* pDependencyComponent);
    void Serialize(CSerializer& serializer);

    //Get the last dependency change action record.
    void GetCurrActionParam(EDependencyChangeAction& action, CComponentProxy*& pProxy);

private:
    void OnDependencyChanged();

private:
    EDependencyType m_type;
    EDependencyChangeAction m_changeAction;
    CComponentProxy* m_pChangeActionProxy;
    uint32_t m_uIndex;
    CComponentProxy* m_pOwner;
    uint32_t m_uDependencyGuid;
    bool m_bHideRender;
    bool m_bIsListType;
    TString m_displayName;
    TString m_variableName;
    std::vector<CDependencyDescriptionLine*> m_dependencyLine;

};
#endif