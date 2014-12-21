#ifndef BEYONDENGINE_COMPONENT_COMPONENTLAUNCHER_H__INCLUDE
#define BEYONDENGINE_COMPONENT_COMPONENTLAUNCHER_H__INCLUDE
#ifdef EDITOR_MODE
#include "BeyondEngineEditor/ComponentGraphics_GL.h"

void ExportComponent(CComponentInstance* pComponent, CSerializer& serializer, const TCHAR* pszComponentName, const std::string& strDisplayName, const std::string& strCatalogName)
{
    uint32_t uGuid = pComponent->GetGuid();
    uint32_t uParentGuid = pComponent->GetParentGuid();
    CComponentProxyManager::GetInstance()->RegisterClassInheritInfo(uGuid, uParentGuid);
    CComponentProxy* pNewProxy = new CComponentProxy(new CComponentGraphic_GL, uGuid, uParentGuid, pszComponentName);
    pNewProxy->SetCatalogName(strCatalogName.c_str());
    pNewProxy->SetDisplayName(strDisplayName.c_str());
    pNewProxy->SetTemplateFlag(true);
    pNewProxy->SetHostComponent(pComponent);
    pComponent->ReflectData(serializer);
    CComponentProxyManager::GetInstance()->RegisterTemplate(pNewProxy);
    CComponentInstanceManager::GetInstance()->RegisterTemplate(pComponent);
}

#define START_REGISTER_COMPONENT(FuncName)\
    void FuncName()\
{\
    CSerializer serializer;

#define REGISTER_COMPONENT(component, displayName, catalogName)\
    ExportComponent(new component(), serializer, _T(#component), TO_UTF8(displayName), TO_UTF8(catalogName));

#define REGISTER_ABSTRACT_COMPONENT(component)\
    CComponentProxyManager::GetInstance()->RegisterClassInheritInfo(component::REFLECT_GUID, component::PARENT_REFLECT_GUID); \
    CComponentProxyManager::GetInstance()->RegisterAbstractComponent(component::REFLECT_GUID, _T(#component)); \

#define END_REGISTER_COMPONENT }
#else

#define START_REGISTER_COMPONENT(FuncName)\
    void FuncName()\
    {

#define END_REGISTER_COMPONENT }

#define REGISTER_COMPONENT(component, displayName, catalogName)\
    CComponentInstanceManager::GetInstance()->RegisterTemplate(new component);

#define REGISTER_ABSTRACT_COMPONENT(component)

#endif

#endif