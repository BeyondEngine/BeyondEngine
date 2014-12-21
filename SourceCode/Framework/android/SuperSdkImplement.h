#ifndef BEYOND_ENGINE_FRAMEWORK_SUPERSDK_IMPEMENT_H__INCLUDE
#define BEYOND_ENGINE_FRAMEWORK_SUPERSDK_IMPEMENT_H__INCLUDE
#include "stdafx.h"

class CSuperSdkImplement
{
    BEATS_DECLARE_SINGLETON(CSuperSdkImplement);
    
public:
    void OnInitPlatform();
    void OnInitPlatformSuccess();
    void OnInitPlatformFailed();


    void OnCheckWithNewVersion();
    void OnCheckWithoutNewVersion();
    void OnCheckWithoutCheckVersion();
    
    void ShowPlatformLogin();
    void OnLoginPlatformSuccess();
    void OnLoginPlatformFailed();
    
    void ShowFloatWindow(const int& x,const int& y);
    void CloseFloatWindow();
    
    void OnShowLogoutAlert();
    void LogoutGame();
    void OnLogoutSuccess();
    void OnLogoutFailed();
    
    void PayProduct(const int& nPrice, const std::string& strProductId, const std::string& strProductName, const std::string& strProductDesc, const std::string& strPointRate, const std::string& strPointName, const std::string& strOrderTitle, const std::string& strCustomData);
    void OnGetOrderIdSuccess(const int& nOrderId);
    void OnPaySuccess();
    void OnPayFailed(const std::string& strParam);

    void PerformGameEventOfGameInitEnd();
    void PerformGameEventOfGameCheckNewVersionBegin();
    void PerformGameEventOfGameCheckNewVersionEnd(bool bSuccess);
    void PerformGameEventOfLoginView();
    void PerformGameEventOfEnterGame();
    void PerformGameEventOfCreateRole();
    void PerformGameEventOfOpenMainPage();
    void PerformGameEventOfLevelUp();

    TString GetPlatformExtend();

    void SetGameDataServerId(const std::string& strServerId);
    void SetGameDataServerName(const std::string& strServerName);
    void SetGameDataAccountId(const std::string& strAccountId);
    void SetGameDataRoleId(const std::string& strId);
    void SetGameDataRoleName(const std::string& strName);
    void SetGameDataRoleLevel(const std::string& strLevel);
    void SetGameDataLoginData(const std::string& strLoginData);
    void SetGameDataExByKey(const std::string& strKey,const std::string& strValue);
    const std::string GetGameDataExByKey(const std::string& strKey,const std::string& strDefaultValue);

    bool HasForum() const;
    void OpenForum();
    bool HasPersonalCenter() const;
    void OpenPersonalCenter();
    bool HasCustomerService() const;
    void OpenCustomerService();
    const std::string GetDeviceID();
    void SetState(int nState);  //for test need remove
};

#endif
