#include "stdafx.h"
#include "LoadComponentFiles.h"

CLoadComponentFiles::CLoadComponentFiles()
    : m_uCurrentLoadingComponentIndex(0)
{
}

CLoadComponentFiles::~CLoadComponentFiles()
{

}

std::vector<CComponentBase*>& CLoadComponentFiles::GetLoadedComponents()
{
    return m_loadedComponents;
}

std::vector<size_t>& CLoadComponentFiles::GetFiles()
{
    return m_componentFiles;
}

size_t CLoadComponentFiles::GetProgress()
{
    size_t uRet = 0;
    if (m_loadedComponents.size() > 0)
    {
        uRet = m_uCurrentLoadingComponentIndex * 100 / m_loadedComponents.size();
    }
    return uRet;
}

void CLoadComponentFiles::Execute(float /*ddt*/)
{
    CComponentInstanceManager* pInstanceManager = CComponentInstanceManager::GetInstance();
    for (size_t i = 0; i < m_componentFiles.size(); ++i)
    {
        pInstanceManager->LoadFile(m_componentFiles[i], m_loadedComponents);
    }
    for (; m_uCurrentLoadingComponentIndex < m_loadedComponents.size(); ++m_uCurrentLoadingComponentIndex)
    {
        m_loadedComponents[m_uCurrentLoadingComponentIndex]->Load();
        BEYONDENGINE_SLEEP(5); 
    }
}