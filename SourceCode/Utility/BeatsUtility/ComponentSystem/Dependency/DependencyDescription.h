#ifndef BEATS_COMPONENTS_DEPENDENCYDESCRIPTION_H__INCLUDE
#define BEATS_COMPONENTS_DEPENDENCYDESCRIPTION_H__INCLUDE

class CDependencyDescriptionLine;
class CComponentProxy;
class TiXmlElement;
class CSerializer;

template class std::allocator<CDependencyDescriptionLine*>;
template class std::vector<CDependencyDescriptionLine*, std::allocator<CDependencyDescriptionLine*> >;

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
    CDependencyDescription(EDependencyType type, size_t dependencyGuid, CComponentProxy* pOwner, size_t uIndex, bool bIsList);
    ~CDependencyDescription();

    CDependencyDescriptionLine* GetDependencyLine(size_t uIndex = 0) const;
    CDependencyDescriptionLine* SetDependency(size_t uIndex, CComponentProxy* pComponent);
    size_t GetDependencyLineCount() const;
    void SetOwner(CComponentProxy* pOwner);
    CComponentProxy* GetOwner();
    size_t GetDependencyGuid() const;
    void Hide();
    void Show();
    bool IsVisible() const;
    bool IsListType() const;
    size_t GetIndex() const;

    const TCHAR* GetDisplayName();
    void SetDisplayName(const TCHAR* pszName);
    const TCHAR* GetVariableName();
    void SetVariableName(const TCHAR* pszName);
    CDependencyDescriptionLine* AddDependency(CComponentProxy* pComponentInstance);
    void RemoveDependencyLine(CDependencyDescriptionLine* pComponentInstance);
    void RemoveDependencyByIndex(size_t uIndex);
    void SwapLineOrder(size_t uSourceIndex, size_t uTargetIndex);
    bool IsInDependency(CComponentProxy* pComponentInstance);
    void SaveToXML(TiXmlElement* pParentNode);
    void LoadFromXML(TiXmlElement* pNode);
    EDependencyType GetType();
    size_t GetSelectedDependencyIndex() const;
    void SetSelectedDependencyIndex(size_t index);
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
    size_t m_uIndex;
    CComponentProxy* m_pOwner;
    size_t m_uDependencyGuid;
    bool m_bHideRender;
    bool m_bIsListType;
    TString m_displayName;
    TString m_variableName;
    std::vector<CDependencyDescriptionLine*> m_dependencyLine;

};
#endif