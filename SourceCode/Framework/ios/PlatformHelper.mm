#include "stdafx.h"
#include "PlatformHelper.h"
#import <Foundation/Foundation.h>
#import "ViewController.h"
#import <CoreMotion/CoreMotion.h>
#include <iostream>
#include <fstream>
#include "StarRaidersCenter.h"
#include "Utility/BeatsUtility/FilePathTool.h"
#include "EnginePublic/BeyondEngineVersion.h"
#include "Config/ServerConfig.h"
#include "Language/LanguageManager.h"

ViewController* GetCurViewController()
{
    UIWindow * window = [[[UIApplication sharedApplication] delegate] window];
    BEATS_ASSERT(window != nil);
    ViewController *result = (ViewController*)window.rootViewController;
    BEATS_ASSERT(result != nil);
    return result;
}

CPlatformHelper* CPlatformHelper::m_pInstance = NULL;
CMMotionManager* g_motionManager = nil;
CPlatformHelper::CPlatformHelper()
{
    
}

CPlatformHelper::~CPlatformHelper()
{
    [g_motionManager release];
    g_motionManager = nil;
}

void CPlatformHelper::InitGameExtrat()
{
    TCHAR szDescription[MAX_PATH];
    _stprintf(szDescription, _T("%d.%d.%d"), BEYONDENGINE_VER_MAJOR, BEYONDENGINE_VER_MINOR, BEYONDENGINE_VER_REVISION);
    TString strResVersion = szDescription;
    CStarRaidersCenter::GetInstance()->SetAppVersion(strResVersion);
    SetAssetFileExtracted(!IsNeedExtract());
}

std::string CPlatformHelper::GetResourceRootPath()
{
    if (m_strResourcePath.empty())
    {
        m_strResourcePath = [[[NSBundle mainBundle] resourcePath] UTF8String];
    }
    return m_strResourcePath;
}

const char* CPlatformHelper::GetPersistentDataPath()
{
    if (m_strPersistentPath.empty())
    {
        NSString* path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES) objectAtIndex:0];
        const char* pPath = [path cStringUsingEncoding:NSUTF8StringEncoding];
        m_strPersistentPath = pPath;
    }
    return m_strPersistentPath.c_str();
}

std::string CPlatformHelper::GetBinaryPath()
{
    return GetResourceRootPath();
}

bool CPlatformHelper::IsAssetFileExtracted() const
{
    return m_bExacted;
}

void CPlatformHelper::SetAssetFileExtracted(bool bExacted)
{
    m_bExacted = bExacted;
}

bool CPlatformHelper::IsNeedExtract()
{
    bool bNeedExact = false;
    TString strVersionFileName = GetPersistentDataPath();
    strVersionFileName.append("/").append("ver.bin");
    FILE* pVerFile = _tfopen(strVersionFileName.c_str(), "rb");
    if (pVerFile != nullptr)//First time run our game.
    {
        char verBuffer[128];
        fgets(verBuffer, 128, pVerFile);
        BEATS_PRINT("Find version file %s version:%s\n", strVersionFileName.c_str(), verBuffer);
        fclose(pVerFile);
        
        TString strVersionName = CStarRaidersCenter::GetInstance()->GetAppVersion();
        if (strcmp(verBuffer, strVersionName.c_str()) != 0)
        {
            bNeedExact = true;
        }
        else
        {
            SetAssetFileExtracted(true);
        }
    }
    else
    {
        bNeedExact = true;
    }
    return bNeedExact;
}

void CPlatformHelper::ShowLoginDialog()
{
    ViewController* pCurViewController = GetCurViewController();
    
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil message:@"请输入账号密码" delegate:pCurViewController cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
    alert.tag = TAG_LOGIN_ALERT;
    alert.alertViewStyle = UIAlertViewStyleLoginAndPasswordInput;
    [alert show];
    [alert release];
    
    NSUserDefaults *user = [NSUserDefaults standardUserDefaults];
    
    UITextField* accountField = [alert textFieldAtIndex:0];
    UITextField* passwordField = [alert textFieldAtIndex:1];
    accountField.keyboardType = UIKeyboardTypeASCIICapable;
    
    [accountField setText:[user objectForKey:@"account"]];
    [passwordField setText:[user objectForKey:@"password"]];
}


void CPlatformHelper::ShowMessage(const std::string& strMsg)
{
    NSString *strTip = [NSString stringWithUTF8String:strMsg.c_str()];
    ViewController* viewController = GetCurViewController();
    [viewController showMessage:strTip];
}

void CPlatformHelper::FadeOutMessage(float fTime)
{
    ViewController* viewController = GetCurViewController();
    [viewController hideMessage:fTime];
}

const ELoginState& CPlatformHelper::GetLoginState() const
{
    return m_eState;
}

void CPlatformHelper::SetLoginState(const ELoginState& eState)
{
    m_eState = eState;
}

void CPlatformHelper::ShowAppraise()
{
    ViewController* viewController = GetCurViewController();
    std::string strOK=L10N(eLTT_Game_GeneralYes);
    std::string strCancel=L10N(eLTT_Game_GeneralNo);
    std::string strTip = "去苹果应用市场评价";
    NSString* nstrOK = [NSString stringWithUTF8String:strOK.c_str()];
    NSString* nstrCancel = [NSString stringWithUTF8String:strCancel.c_str()];
    NSString* nstrTip = [NSString stringWithUTF8String:strTip.c_str()];
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil message:nstrTip delegate:viewController cancelButtonTitle:nstrOK otherButtonTitles:nstrCancel, nil];
    alert.tag = TAG_APPRAISE;
    [alert show];
    [alert release];
}

void CPlatformHelper::ShowLoadingAnimation(int ntime)
{
    ViewController* pViewController = GetCurViewController();
    [NSTimer scheduledTimerWithTimeInterval:ntime target:pViewController selector:@selector(requestPayTimeOut) userInfo:nil repeats:NO];
    
    UIActivityIndicatorView *indicator = nil;
    indicator = (UIActivityIndicatorView *)[pViewController.view viewWithTag:TAG_REQUEST_PAY];
    if (indicator == nil) {
        //初始化:
        UIActivityIndicatorView *indicator = [[UIActivityIndicatorView alloc] initWithFrame:CGRectMake(0, 0, 80, 80)];
        indicator.tag = TAG_REQUEST_PAY;
        //设置显示样式,见UIActivityIndicatorViewStyle的定义
        indicator.activityIndicatorViewStyle = UIActivityIndicatorViewStyleWhiteLarge;
        //设置背景色
        indicator.backgroundColor = [UIColor blackColor];
        //设置背景透明
        indicator.alpha = 0.5;
        //设置背景为圆角矩形
        indicator.layer.cornerRadius = 6;
        indicator.layer.masksToBounds = YES;
        //设置显示位置
        [indicator setCenter:CGPointMake(pViewController.view.frame.size.width / 2.0, pViewController.view.frame.size.height / 2.0)];
        //开始显示Loading动画
        [indicator startAnimating];
        [GetCurViewController().view addSubview:indicator];
        [indicator release];
    }
    //开始显示Loading动画
    [indicator startAnimating];
}

void CPlatformHelper::CloseLoadingAnimation()
{
    ViewController* pViewController = GetCurViewController();
    UIActivityIndicatorView *indicator = (UIActivityIndicatorView *)[pViewController.view viewWithTag:TAG_REQUEST_PAY];
    [indicator stopAnimating];
}

void CPlatformHelper::DisableScreenSleep(bool bDisable)
{
    if (bDisable) {
        [UIApplication sharedApplication].idleTimerDisabled = YES;
    }
    else{
        [UIApplication sharedApplication].idleTimerDisabled = NO;
    }
}

std::string CPlatformHelper::GetDieviceIMEI()
{
    NSString *idfv = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
    const char* pUUID = [idfv cStringUsingEncoding:NSUTF8StringEncoding];
    return pUUID;
}

void CPlatformHelper::SetPlatformFPS(int nFPS)
{
    ViewController* pViewController = GetCurViewController();
    [pViewController setfps:nFPS];
}

//单位M
int CPlatformHelper::GetFreeSpace()
{
    int nFreeSpace = -1;
    NSError *error = nil;  
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);  
    NSDictionary *dictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:[paths lastObject] error: &error];  
    if (dictionary)  
    { 
        NSNumber *_free = [dictionary objectForKey:NSFileSystemFreeSize];  
        nFreeSpace = [_free unsignedLongLongValue]*1.0/(1024 * 1024);
    } 
    else  
    {  
        NSLog(@"Error Obtaining System Memory Info: Domain = %@, Code = %ld", [error domain], (long)[error code]);  
    }  
    return nFreeSpace;
}

void CPlatformHelper::OpenDownloadWeb()
{
    auto config = ServerConfig::GetInstance()->GetDefault();
    std::string strWeb = config.strAppUpdateUrl;
    NSString *urlText = [NSString stringWithUTF8String:strWeb.c_str()];
    [[UIApplication sharedApplication] openURL:[ NSURL URLWithString:urlText]];
}

void CPlatformHelper::InitSenorsManager()
{
    BEATS_ASSERT(g_motionManager == nil);
    g_motionManager = [[CMMotionManager alloc] init];
}

void CPlatformHelper::StartAccelerometerListener()
{
    BEATS_ASSERT(g_motionManager);
    if (g_motionManager.accelerometerAvailable) {
        g_motionManager.accelerometerUpdateInterval = 1.f/60.f;
        [g_motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue mainQueue]
                                withHandler:^(CMAccelerometerData *data, NSError *error) {
            OnAccelerometer(data.acceleration.x, data.acceleration.y, data.acceleration.z);
        }];
    }
}

void CPlatformHelper::StopAccelerometerListener()
{
    BEATS_ASSERT(g_motionManager);
    [g_motionManager stopAccelerometerUpdates];
}

void CPlatformHelper::OnAccelerometer(float x, float y, float z)
{
    x = MAX(x, -1.0f);
    x = MIN(x, 1.0f);
    y = MAX(y, -1.0f);
    y = MIN(y, 1.0f);
    z = MAX(z, -1.0f);
    z = MIN(z, 1.0f);
    m_vecAccelerometer = CVec3(x, y, z);
}

const CVec3& CPlatformHelper::GetAccelerometer() const
{
    return m_vecAccelerometer;
}


void CPlatformHelper::StartGravityListener()
{
    BEATS_ASSERT(g_motionManager);
    if (g_motionManager.accelerometerAvailable) {
        g_motionManager.accelerometerUpdateInterval = 1.f/30.f;
        [g_motionManager startDeviceMotionUpdatesToQueue:[NSOperationQueue mainQueue]
                                withHandler:^(CMDeviceMotion *data, NSError *error) {
            OnGravity(data.gravity.x, data.gravity.y, data.gravity.z);
        }];
    }
}

void CPlatformHelper::StopGravityListener()
{
    BEATS_ASSERT(g_motionManager);
    [g_motionManager stopDeviceMotionUpdates];
}

void CPlatformHelper::OnGravity(float x, float y, float z)
{
    x = MAX(x, -1.0f);
    x = MIN(x, 1.0f);
    y = MAX(y, -1.0f);
    y = MIN(y, 1.0f);
    z = MAX(z, -1.0f);
    z = MIN(z, 1.0f);
    m_vecGravity = CVec3(x, y, z);
}

const CVec3& CPlatformHelper::GetGravity() const
{
    return m_vecGravity;
}