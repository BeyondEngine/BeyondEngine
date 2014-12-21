
#include "stdafx.h"
#ifdef _SUPER_SDK

#include "SuperSdkImplement.h"
#include "StarRaidersCenter.h"
#include "Utility/BeatsUtility/md5.h"
#include "Utility/VerifyToken.h"
#include "Application.h"
#include "PlatformHelper.h"
#include "Language/LanguageManager.h"
#import "ViewController.h"

ViewController* GetCurViewController()
{
    UIWindow * window = [[[UIApplication sharedApplication] delegate] window];
    BEATS_ASSERT(window != nil);
    ViewController *result = (ViewController*)window.rootViewController;
    BEATS_ASSERT(result != nil);
    return result;
}

CSuperSdkImplement* CSuperSdkImplement::m_pInstance = NULL;

CSuperSdkImplement::CSuperSdkImplement()
{
}

CSuperSdkImplement::~CSuperSdkImplement()
{
}

void CSuperSdkImplement::OnInitPlatform()
{
    [[SuperSdkOpenApi sharedInstance] initPlatformWithDeleagte:GetCurViewController() withSuperSdkUrlType:SuperSdkUrlTypeDomestic];
}

void CSuperSdkImplement::OnInitPlatformSuccess()
{
    
}

void CSuperSdkImplement::OnInitPlatformFailed()
{
    CPlatformHelper::GetInstance()->ShowMessage(L10N(eLTT_Games_Alert_LoseConnect));
    CPlatformHelper::GetInstance()->FadeOutMessage();
}

//有sdk整包更新
void CSuperSdkImplement::OnCheckWithNewVersion()
{
}

//没有sdk整包更新
void CSuperSdkImplement::OnCheckWithoutNewVersion()
{
}

//没有更新接口，需游戏自己设置更新逻辑
void CSuperSdkImplement::OnCheckWithoutCheckVersion()
{
}

void CSuperSdkImplement::ShowPlatformLogin()
{
    [[SuperSdkOpenApi sharedInstance] showLoginMode2];
}

//渠道登录成功
void CSuperSdkImplement::OnLoginPlatformSuccess()
{
    
}

//渠道登录失败
void CSuperSdkImplement::OnLoginPlatformFailed()
{
    
}

void CSuperSdkImplement::ShowFloatWindow(const int& nX, const int& nY)
{
    NSLog(@"***打开悬浮窗***");
    if ([[SuperSdkOpenApi sharedInstance] hasFloatWindow]) {
        SuperSdkGameData *gameData = [SuperSdkGameData shareInstance];
        [[SuperSdkOpenApi sharedInstance] openFloatWindow:nX withY:nY withGameData:gameData];
    } else {
        NSLog(@"渠道没有悬浮窗功能");
    }
}

void CSuperSdkImplement::CloseFloatWindow()
{
    NSLog(@"***关闭悬浮窗***");
    if ([[SuperSdkOpenApi sharedInstance] hasFloatWindow]) {
        [[SuperSdkOpenApi sharedInstance] closeFloatWindow];
    } else {
        NSLog(@"渠道没有悬浮窗功能");
    }
}

//注销  响应在viewcontroller中
void CSuperSdkImplement::OnShowLogoutAlert()
{
    ViewController* viewController = GetCurViewController();
    NSString* strOK = [NSString stringWithUTF8String:L10N(eLTT_Game_GeneralYes).c_str()];
    NSString* strCancel = [NSString stringWithUTF8String:L10N(eLTT_Game_GeneralNo).c_str()];
    NSString* strTip = @"确认注销账号";
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil message:strTip delegate:viewController cancelButtonTitle:strCancel otherButtonTitles:strOK, nil];
    alert.tag = TAG_LOGOUT_ALERT;
    [alert show];
    [alert release];
}

//注销
void CSuperSdkImplement::LogoutGame()
{
    int logoutType = 0;
    logoutType = [[SuperSdkOpenApi sharedInstance]getLogoutType];
    SuperSdkGameData *gameData = [SuperSdkGameData shareInstance];
    switch (logoutType) {
        case LOGOUT_WITH_NO_TYPE:
        {
            NSLog(@"不知道第三方是否有注销接口，以及在有注销接口时是否有弹框问你是否注销界面");
            [[SuperSdkOpenApi sharedInstance] logout:gameData];
        }
            break;
        case LOGOUT_WITH_NO_IMPLEMENT:
        {
            NSLog(@"第三方没有注销接口,默认注销成功");
            OnShowLogoutAlert();
        }
            break;
        case LOGOUT_WITH_OPEN_LOGOUT_DIALOG:
        {
            NSLog(@"第三方有注销接口,调用注销接口会有弹框问你是否注销");
            [[SuperSdkOpenApi sharedInstance] logout:gameData];
        }
            break;
        case LOGOUT_WITH_NOT_OPEN_LOGOUT_DIALOG:
        {
            NSLog(@"第三方有注销接口,调用注销接口不会有弹框问你是否注销");
            OnShowLogoutAlert();
        }
            break;
            
        default:
            break;
    }
    NSLog(@"***调用注销接口***");
}

//注销成功
void CSuperSdkImplement::OnLogoutSuccess()
{
    CStarRaidersCenter::GetInstance()->SetNeedRestart(true);
}

//注销失败
void CSuperSdkImplement::OnLogoutFailed()
{
    ShowFloatWindow(10, 10);
}

//充值
void CSuperSdkImplement::PayProduct(const int& nPrice, const std::string& strProductId, const std::string& strProductName, const std::string& strProductDesc, const std::string& strPointRate, const std::string& strPointName, const std::string& strOrderTitle, const std::string& strCustomData)
{
    
    SuperSdkGameData *gameData = [SuperSdkGameData shareInstance];
    NSString* nstrProductId = [NSString stringWithUTF8String:strProductId.c_str()];
    NSString* nstrProductName = [NSString stringWithUTF8String:strProductName.c_str()];
    NSString* nstrProductDesc = [NSString stringWithUTF8String:strProductDesc.c_str()];
    NSString* nstrPointRate = [NSString stringWithUTF8String:strPointRate.c_str()];
    NSString* nstrPointName = [NSString stringWithUTF8String:strPointName.c_str()];
    NSString* nstrOrderTitle = [NSString stringWithUTF8String:strOrderTitle.c_str()];
    NSString* nstrCustomData = [NSString stringWithUTF8String:strCustomData.c_str()];
  
#ifdef DEVELOP_VERSION
    [gameData setGameValue:@"true" gameKey:@"sandbox"];//for test pay, must clear it when the app developed over
#endif
    [[SuperSdkOpenApi sharedInstance] pay:nPrice withProductId:nstrProductId withProductName:nstrProductName withProductDesc:nstrProductDesc withPointRate:nstrPointRate withPointName:nstrPointName withOrderTitle:nstrOrderTitle withGameData:gameData withPayExtra:nstrCustomData];
}

void CSuperSdkImplement::OnGetOrderIdSuccess(const int& nOrderId)
{
}

//支付成功
void CSuperSdkImplement::OnPaySuccess()
{
}

//支付失败
void CSuperSdkImplement::OnPayFailed(const std::string& strParam)
{
    CPlatformHelper::GetInstance()->ShowMessage(strParam);
    CPlatformHelper::GetInstance()->FadeOutMessage();
}

void CSuperSdkImplement::PerformGameEventOfGameInitEnd()
{
    [[SuperSdkOpenApi sharedInstance]performGameEventOfGameInitEnd];
}

void CSuperSdkImplement::PerformGameEventOfGameCheckNewVersionBegin()
{
    [[SuperSdkOpenApi sharedInstance]performGameEventOfGameCheckVersionBegin];
}

void CSuperSdkImplement::PerformGameEventOfGameCheckNewVersionEnd(bool bSuccess)
{
    if (bSuccess == true)
    {
        [[SuperSdkOpenApi sharedInstance]performGameEventOfGameCheckVersionEnd:YES];
    }
    else
    {
        [[SuperSdkOpenApi sharedInstance]performGameEventOfGameCheckVersionEnd:NO];
    }
}

void CSuperSdkImplement::PerformGameEventOfLoginView()
{
    [[SuperSdkOpenApi sharedInstance]performGameEventOfLoginView];
}

void CSuperSdkImplement::PerformGameEventOfEnterGame()
{
    SuperSdkGameData *gameData = [SuperSdkGameData shareInstance];
    [[SuperSdkOpenApi sharedInstance]performGameEventOfEnterGame:gameData];
}

void CSuperSdkImplement::PerformGameEventOfCreateRole()
{
    SuperSdkGameData *gameData = [SuperSdkGameData shareInstance];
    [[SuperSdkOpenApi sharedInstance]performGameEventOfCreateRole:gameData];
}

void CSuperSdkImplement::PerformGameEventOfOpenMainPage()
{
    SuperSdkGameData *gameData = [SuperSdkGameData shareInstance];
    [[SuperSdkOpenApi sharedInstance]performGameEventOfOpenMainPage:(SuperSdkGameData*)gameData];
}

void CSuperSdkImplement::PerformGameEventOfLevelUp()
{
    SuperSdkGameData *gameData = [SuperSdkGameData shareInstance];
    [[SuperSdkOpenApi sharedInstance]performGameEventOfLevelUp:gameData];
}

void CSuperSdkImplement::SetGameDataServerId(const std::string& strServerId)
{
    SuperSdkGameData *pGameData = [SuperSdkGameData shareInstance];
    pGameData.serverId = [NSString stringWithUTF8String:strServerId.c_str()];
}

void CSuperSdkImplement::SetGameDataServerName(const std::string& strServerName)
{
    SuperSdkGameData *pGameData = [SuperSdkGameData shareInstance];
    pGameData.serverName = [NSString stringWithUTF8String:strServerName.c_str()];
}

void CSuperSdkImplement::SetGameDataAccountId(const std::string& strAccountId)
{
    SuperSdkGameData *pGameData = [SuperSdkGameData shareInstance];
    pGameData.accountId = [NSString stringWithUTF8String:strAccountId.c_str()];
}

void CSuperSdkImplement::SetGameDataRoleId(const std::string& strId)
{
    SuperSdkGameData *pGameData = [SuperSdkGameData shareInstance];
    pGameData.roleId = [NSString stringWithUTF8String:strId.c_str()];
}

void CSuperSdkImplement::SetGameDataRoleName(const std::string& strName)
{
    SuperSdkGameData *pGameData = [SuperSdkGameData shareInstance];
    pGameData.roleName = [NSString stringWithUTF8String:strName.c_str()];
}

void CSuperSdkImplement::SetGameDataRoleLevel(const std::string& strLevel)
{
    SuperSdkGameData *pGameData = [SuperSdkGameData shareInstance];
    pGameData.roleLevel = [NSString stringWithUTF8String:strLevel.c_str()];
}

void CSuperSdkImplement::SetGameDataLoginData(const std::string& strLoginData)
{
    SuperSdkGameData *pGameData = [SuperSdkGameData shareInstance];
    pGameData.loginData = [NSString stringWithUTF8String:strLoginData.c_str()];
}

void CSuperSdkImplement::SetGameDataExByKey(const std::string& strKey,const std::string& strValue)
{
    SuperSdkGameData *pGameData = [SuperSdkGameData shareInstance];
    auto nsstrKey = [NSString stringWithUTF8String:strKey.c_str()];
    auto nsstrValue = [NSString stringWithUTF8String:strValue.c_str()];
    [pGameData getGameValue:nsstrKey defaultValue:nsstrValue];
}

const std::string CSuperSdkImplement::GetGameDataExByKey(const std::string& strKey,const std::string& strDefaultValue)
{
    std::string strRet;
    SuperSdkGameData *pGameData = [SuperSdkGameData shareInstance];
    
    auto nsstrKey = [NSString stringWithUTF8String:strKey.c_str()];
    auto nsstrDefaultValue = [NSString stringWithUTF8String:strDefaultValue.c_str()];
    
    auto nsstrRet = [pGameData getGameValue:nsstrKey defaultValue:nsstrDefaultValue];
    strRet = [nsstrRet UTF8String];
    
    return strRet;
}

bool CSuperSdkImplement::HasForum() const
{
    return [[SuperSdkOpenApi sharedInstance] hasForum];
}

void CSuperSdkImplement::OpenForum()
{
    SuperSdkGameData* gamedata = [SuperSdkGameData shareInstance];
    
    [[SuperSdkOpenApi sharedInstance] openForum:gamedata];
}

bool CSuperSdkImplement::HasPersonalCenter() const
{
    return [[SuperSdkOpenApi sharedInstance] hasPersonalCenter];
}

void CSuperSdkImplement::OpenPersonalCenter()
{
    SuperSdkGameData* gamedata = [SuperSdkGameData shareInstance];
    [[SuperSdkOpenApi sharedInstance] openPersonalCenter:gamedata];
}

bool CSuperSdkImplement::HasCustomerService() const
{
    return [[SuperSdkOpenApi sharedInstance] hasCustomerService];
}

void CSuperSdkImplement::OpenCustomerService()
{
    SuperSdkGameData* gamedata = [SuperSdkGameData shareInstance];
    [[SuperSdkOpenApi sharedInstance] openCustomerService:gamedata];
}

const std::string CSuperSdkImplement::GetDeviceID()
{
    NSString* nstrDeviceID = [[SuperSdkOpenApi sharedInstance]getDeviceId];
    std::string strDeviceID = [nstrDeviceID UTF8String];
    return strDeviceID;
}

TString CSuperSdkImplement::GetPlatformExtend()
{
    NSString* strExt = [[SuperSdkOpenApi sharedInstance] getConfigParam:kExtend withDefault:@"apple"];
    NSArray *aArray = [strExt componentsSeparatedByString:@"|"];
    int nCount = [aArray count];
    BEATS_ASSERT(nCount > 0);
    TString strChannel = [[aArray objectAtIndex:0] UTF8String];
    TString strGameId = [[aArray objectAtIndex:1] UTF8String];
    return strChannel + strGameId;
}

#endif


