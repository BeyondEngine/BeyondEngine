#ifndef BEYOND_ENGINE_FRAMEWORK_SUPERSDKIMPEMENT_H__INCLUDE
#define BEYOND_ENGINE_FRAMEWORK_SUPERSDKIMPEMENT_H__INCLUDE
#include "stdafx.h"
#ifdef _SUPER_SDK

class CSuperSdkImplement
{
    BEATS_DECLARE_SINGLETON(CSuperSdkImplement);
    
public:

    //初始化渠道
    void OnInitPlatform();

    //初始化渠道成功
    void OnInitPlatformSuccess();
    //初始化渠道失败
    void OnInitPlatformFailed();

    //检查到有更新
    void OnCheckWithNewVersion();
    //检查没有更新
    void OnCheckWithoutNewVersion();
    //没有更新接口，需游戏自己设置更新逻辑
    void OnCheckWithoutCheckVersion();
    
    //显示渠道登录
    void ShowPlatformLogin();
    //渠道登录成功
    void OnLoginPlatformSuccess();
    //渠道登录失败
    void OnLoginPlatformFailed();
    
    //显示悬浮窗口 x，y是坐标
    void ShowFloatWindow(const int& x,const int& y);
    //关闭悬浮窗口
    void CloseFloatWindow();
    
    //显示确认注销的对话框
    void OnShowLogoutAlert();
    //注销
    void LogoutGame();
    //注销成功
    void OnLogoutSuccess();
    //注销失败
    void OnLogoutFailed();
    
    //充值
    void PayProduct(const int& nPrice, const std::string& strProductId, const std::string& strProductName, const std::string& strProductDesc, const std::string& strPointRate, const std::string& strPointName, const std::string& strOrderTitle, const std::string& strCustomData);
    //获取id成功
    void OnGetOrderIdSuccess(const int& nOrderId);
    //支付成功
    void OnPaySuccess();
    //支付失败
    void OnPayFailed(const std::string& strParam);

    //游戏初始化结束
    void PerformGameEventOfGameInitEnd();
    //版本更新开始
    void PerformGameEventOfGameCheckNewVersionBegin();
    //版本更新结束
    void PerformGameEventOfGameCheckNewVersionEnd(bool bSuccess);
    //进入登陆界面
    void PerformGameEventOfLoginView();
    //进入游戏
    void PerformGameEventOfEnterGame();
    //创建角色
    void PerformGameEventOfCreateRole();
    //进入主界面
    void PerformGameEventOfOpenMainPage();
    //等级上升
    void PerformGameEventOfLevelUp();

    TString GetPlatformExtend();
    //设置gamedata
    void SetGameDataServerId(const std::string& strServerId);
    void SetGameDataServerName(const std::string& strServerName);
    void SetGameDataAccountId(const std::string& strAccountId);
    void SetGameDataRoleId(const std::string& strId);
    void SetGameDataRoleName(const std::string& strName);
    void SetGameDataRoleLevel(const std::string& strLevel);
    void SetGameDataLoginData(const std::string& strLoginData);

    //设置额外的属性
    void SetGameDataExByKey(const std::string& strKey,const std::string& strValue);
    //获取额外的属性
    const std::string GetGameDataExByKey(const std::string& strKey,const std::string& strDefaultValue);
    //论坛
    bool HasForum() const;
    void OpenForum();
    //用户中心
    bool HasPersonalCenter() const;
    void OpenPersonalCenter();
    //客服
    bool HasCustomerService() const;
    void OpenCustomerService();

    const std::string GetDeviceID();

};

#endif

#endif
