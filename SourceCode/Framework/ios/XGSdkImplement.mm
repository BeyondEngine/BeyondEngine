
#include "stdafx.h"
#include "XGSdkImplement.h"
#include "StarRaidersCenter.h"
#include "Utility/BeatsUtility/md5.h"
#include "Utility/VerifyToken.h"
#include "Application.h"
#include "PlatformHelper.h"
#include "external/SDKManager.h"
#include "Language/LanguageManager.h"

#import "XGPush.h"
#import "XGSetting.h"


CXGSdkImplement* CXGSdkImplement::m_pInstance = NULL;

CXGSdkImplement::CXGSdkImplement()
{
}

CXGSdkImplement::~CXGSdkImplement()
{
}

void CXGSdkImplement::RegistXGByAccount(const std::string& strAccount)
{
    [XGPush startApp:2200206169 appKey:@"IQ98B8D9S9SI"];
    void (^successCallback)(void) = ^(void){
        NSLog(@"[XGPush] successCallback");
    };
    [XGPush initForReregister:successCallback];

    NSString* nstrAccount = [NSString stringWithUTF8String:strAccount.c_str()];
    [XGPush setAccount:nstrAccount];
    NSLog(@"[XGPush] strAccount is %@",nstrAccount);
}

void CXGSdkImplement::UnRegistXGByAccount()
{

}

void CXGSdkImplement::UnRegistPush()
{
    [XGPush unRegisterDevice];
    BEATS_PRINT("unRegister push");
}

void CXGSdkImplement::AddLocalPush(const std::string& strBody, const std::string& strKey, const std::string& strValue,const uint32_t& uInterval)
{
    NSString* nstrKey = [NSString stringWithUTF8String:strKey.c_str()];
    NSString* nstrValue = [NSString stringWithUTF8String:strValue.c_str()];
    NSString* nstrBody = [NSString stringWithUTF8String:strBody.c_str()];
    NSDate *fireDate = [[NSDate date] dateByAddingTimeInterval:uInterval];
    std::string strConfirm = L10N(eLTT_Game_OnlineTips_Confirm);
    NSString* nstrOK = [NSString stringWithUTF8String:strConfirm.c_str()];
    NSMutableDictionary *dicUserInfo = [[NSMutableDictionary dictionary] init];
    [dicUserInfo setValue:nstrValue forKey:nstrKey];
    NSDictionary *userInfo = dicUserInfo;
    
    [XGPush localNotification:fireDate alertBody:nstrBody badge:1 alertAction:nstrOK userInfo:userInfo];
}

void CXGSdkImplement::ClearLocalPushes()
{
    [XGPush clearLocalNotifications];
}
