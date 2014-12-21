#ifndef BEYOND_ENGINE_UTILITY_BEATSUTILITY_COMPONENTSYSTEM_PROPERTY_PERFORMRESULTTREEITEMData_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_BEATSUTILITY_COMPONENTSYSTEM_PROPERTY_PERFORMRESULTTREEITEMData_H__INCLUDE
#include <wx/treebase.h>

class CPerformResultTreeItemData : public wxTreeItemData
{
public:
    CPerformResultTreeItemData(const uint32_t id)
        : m_recordId(id)
    {

    }
    virtual ~CPerformResultTreeItemData()
    {

    }
    uint32_t GetRecordId(){return m_recordId;}
    void SetRecordId(const uint32_t& id){m_recordId = id;}


private:
    uint32_t m_recordId;
};

#endif