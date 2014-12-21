#ifndef BEYOND_ENGINE_FRAMEWORK_XGSDK_IMPEMENT_H__INCLUDE
#define BEYOND_ENGINE_FRAMEWORK_XGSDK_IMPEMENT_H__INCLUDE
#include "stdafx.h"

class CXGSdkImplement
{
    BEATS_DECLARE_SINGLETON(CXGSdkImplement);
    
public:
    void RegistXGByAccount(const std::string& strAccount);
    void UnRegistXGByAccount();
    void UnRegistPush();
    
    void AddLocalPush(const std::string& strBody, const std::string& strKey, const std::string& strValue,const uint32_t& uInterval);
    void ClearLocalPushes();
};

#endif
