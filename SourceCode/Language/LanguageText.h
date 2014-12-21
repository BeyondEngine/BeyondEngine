#ifndef BEYOND_ENGINE_LANGUAGE_LANGUAGETEXT_H__INCLUDE
#define BEYOND_ENGINE_LANGUAGE_LANGUAGETEXT_H__INCLUDE

#include "Language.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Component/ComponentPublic.h"

class CLanguageText
{
public:
    CLanguageText();
    CLanguageText(const CLanguageText& ref);
    CLanguageText(CLanguageText&& rhs); // std::vector<CLanguageText>::resize will cause this.
    ~CLanguageText();

    CLanguageText& operator= (const CLanguageText& rhs);
    ELanguageTextType GetType() const;
    void SetType(ELanguageTextType type);
    const TString& GetValueString() const;
    void SetValueString(const TString& str);
    void RefreshValueString();

private:
    ELanguageTextType m_type;
    TString m_strValue;
};

template<>
inline void DeserializeVariable(CLanguageText& value, CSerializer* pSerializer, CComponentInstance* /*pOwner*/)
{
    ELanguageTextType type;
    *pSerializer >> type;
    value.SetType(type);
}

template<>
inline EReflectPropertyType GetEnumType(CLanguageText& value, CSerializer* pSerializer)
{
    *pSerializer << eRPT_LanguageText;
    *pSerializer << value.GetType();
    return eRPT_LanguageText;
}

#endif