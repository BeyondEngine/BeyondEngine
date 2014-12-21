#import "AppDelegate.h"
#import "ViewController.h"
#import "Framework/Application.h"

#include "stdafx.h"
#ifdef _SUPER_SDK
#import <SuperSdk/SuperSdkOpenApi.h>
#import <SuperSdk/SuperSdkGameData.h>
#import <SuperSdk/SuperSdkDefines.h>
#endif

#ifdef _XG_PUSH_SDK
#import "XGPush.h"
#import "XGSetting.h"

#define _IPHONE80_ 80000
#endif

#import <Bugly/Bugly.h>
#ifdef USE_SDK
#include "external/SDKManager.h"
#endif

@interface AppDelegate () <BuglyDelegate>
@end

@implementation AppDelegate
@synthesize window = window;

- (void)registerPushForIOS8{
    //Types
    UIUserNotificationType types = UIUserNotificationTypeBadge | UIUserNotificationTypeSound | UIUserNotificationTypeAlert;
    
    //Actions
    UIMutableUserNotificationAction *acceptAction = [[UIMutableUserNotificationAction alloc] init];
    
    acceptAction.identifier = @"ACCEPT_IDENTIFIER";
    acceptAction.title = @"Accept";
    
    acceptAction.activationMode = UIUserNotificationActivationModeForeground;
    acceptAction.destructive = NO;
    acceptAction.authenticationRequired = NO;
    
    //Categories
    UIMutableUserNotificationCategory *inviteCategory = [[UIMutableUserNotificationCategory alloc] init];
    
    inviteCategory.identifier = @"INVITE_CATEGORY";
    
    [inviteCategory setActions:@[acceptAction] forContext:UIUserNotificationActionContextDefault];
    
    [inviteCategory setActions:@[acceptAction] forContext:UIUserNotificationActionContextMinimal];
    
    [acceptAction release];
    
    NSSet *categories = [NSSet setWithObjects:inviteCategory, nil];
    
    [inviteCategory release];
    
    
    UIUserNotificationSettings *mySettings = [UIUserNotificationSettings settingsForTypes:types categories:categories];
    
    [[UIApplication sharedApplication] registerUserNotificationSettings:mySettings];
    
    
    [[UIApplication sharedApplication] registerForRemoteNotifications];
}

- (void)registerPush{
    [[UIApplication sharedApplication] registerForRemoteNotificationTypes:(UIRemoteNotificationTypeAlert | UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound)];
}


- (void)setupBugly {
    // Get the default config
    BuglyConfig * config = [[BuglyConfig alloc] init];
    // Open the debug mode to print the sdk log message.
    // Default value is NO, please DISABLE it in your RELEASE version.
#if DEBUG
    config.debugMode = YES;
#endif
    // Open the customized log record and report, BuglyLogLevelWarn will report Warn, Error log message.
    // Default value is BuglyLogLevelSilent that means DISABLE it.
    // You could change the value according to you need.
    config.reportLogLevel = BuglyLogLevelWarn;
    
    // Open the STUCK scene data in MAIN thread record and report.
    // Default value is NO
    config.blockMonitorEnable = YES;
    
    // Set the STUCK THRESHOLD time, when STUCK time > THRESHOLD it will record an event and report data when the app launched next time.
    // Default value is 3.5 second.
    config.blockMonitorTimeout = 1.5;
    
    // Set the app channel to deployment
    config.channel = @"Bugly";
    config.delegate = self;
    
    config.unexpectedTerminatingDetectionEnable = YES;
    
    // NOTE:Required
    // Start the Bugly sdk with APP_ID and your config
    [Bugly startWithAppId:@"61311954c1"
#if DEBUG
        developmentDevice:YES
#endif
                   config:config];
    
    // Set the customizd tag thats config in your APP registerd on the  bugly.qq.com
    // [Bugly setTag:1799];
    [Bugly setUserIdentifier:[NSString stringWithFormat:@"User: %@", [UIDevice currentDevice].name]];
    [Bugly setUserValue:[NSProcessInfo processInfo].processName forKey:@"Process"];
    
}

- (NSString *)attachmentForException:(NSException *)exception{
    TString strLogList;
#ifdef DEVELOP_VERSION
    strLogList = CLogManager::GetInstance()->GetLastLogList(500);
#endif
    NSString* nstrLogList = [NSString stringWithUTF8String:strLogList.c_str()];
    return nstrLogList;
}

- (void)application:(UIApplication *)application performActionForShortcutItem:(UIApplicationShortcutItem *)shortcutItem completionHandler:(void (^)(BOOL))completionHandler {
    
    if ([shortcutItem.type isEqualToString:_SHARE_STRING_]) {//进入分享界面
        NSLog(@"clicked share");
    }
    
    if (completionHandler) {
        completionHandler(YES);
    }
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    float sysVer = [[[UIDevice currentDevice] systemVersion] floatValue];
    
    [self setupBugly];
#ifdef _SUPER_SDK
    [[SuperSdkOpenApi sharedInstance]application:application didFinishLaunchingWithOptions:launchOptions];
#endif
#ifdef _XG_PUSH_SDK
    //init xg push
    [XGPush startApp:2200206169 appKey:@"IQ98B8D9S9SI"];
    //for statistics click notify restart game
    [XGPush handleLaunching:launchOptions];
    
    if(sysVer < 8){
        [self registerPush];
    }
    else{
        [self registerPushForIOS8];
    }
    [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
#endif
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    // Override point for customization after application launch.
    self.window.backgroundColor = [UIColor whiteColor];
    
    self.viewController = [[ViewController alloc] initWithNibName:nil bundle:nil];

    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [self.window addSubview: self.viewController.view];
    }
    else
    {
        // use this method on ios6
        [self.window setRootViewController: self.viewController];
    }

    [self.window makeKeyAndVisible];

    [[UIApplication sharedApplication] setStatusBarHidden: YES];

#ifdef _SUPER_SDK
    //开启SDK错误上传
    [[SuperSdkOpenApi sharedInstance]SuperSdkCommitStat:YES];
    //开启日志打印、做调试查询日志使用、上线时、请关闭该接口
    [[SuperSdkOpenApi sharedInstance]OPSuperSdkSetShowSDKLog:NO];
#endif
    if (sysVer >= 9.0f)
    {
        if ([launchOptions valueForKey:UIApplicationLaunchOptionsShortcutItemKey]) {
            return NO;
        }
    }
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
#ifdef _SUPER_SDK
    [[SuperSdkOpenApi sharedInstance]applicationWillResignActive:application];
#endif
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
#ifdef _SUPER_SDK
    [[SuperSdkOpenApi sharedInstance]applicationDidEnterBackground:application];
#endif
    CApplication::GetInstance()->OnSwitchToBackground();
    [self.viewController enterBackGround];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
#ifdef _SUPER_SDK
    [[SuperSdkOpenApi sharedInstance]applicationWillEnterForeground:application];
#endif
#ifdef _XG_PUSH_SDK
    [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
#endif
    CApplication::GetInstance()->OnSwitchToForeground();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
#ifdef _SUPER_SDK
    [[SuperSdkOpenApi sharedInstance]applicationDidBecomeActive:application];
#endif
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
#ifdef _SUPER_SDK
    [[SuperSdkOpenApi sharedInstance]applicationWillTerminate:application];
#endif
    CApplication::GetInstance()->Uninitialize();
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
#ifdef _SUPER_SDK
    //生命周期事件 -> 程序间跳转事件，参数没有的传nil
    return [[SuperSdkOpenApi sharedInstance]application:application openURL:(NSURL *)url sourceApplication:nil annotation:nil];
#else
    return YES;
#endif
}

-(BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation//Ican平台
{
#ifdef _SUPER_SDK
    //生命周期事件 -> 程序间跳转事件，参数没有的传nil
    return [[SuperSdkOpenApi sharedInstance]application:application openURL:(NSURL *)url sourceApplication:nil annotation:nil];
#else
    return YES;
#endif
}

- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options
{
#ifdef _SUPER_SDK
    [UIApplication sharedApplication].statusBarHidden = YES;
    return [[SuperSdkOpenApi sharedInstance]application:app openURL:url sourceApplication:[options objectForKey:UIApplicationOpenURLOptionsSourceApplicationKey] annotation:[options objectForKey:UIApplicationOpenURLOptionsAnnotationKey]];
#else
    return YES;
#endif
}

- (UIInterfaceOrientationMask)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)uiwindow
{
#ifdef _SUPER_SDK
    //生命周期事件 -> 控制屏幕方向
    return [[SuperSdkOpenApi sharedInstance]application:application supportedInterfaceOrientationsForWindow:uiwindow];
#else
    return [[UIApplication sharedApplication]supportedInterfaceOrientationsForWindow:uiwindow];
#endif
}

//for xg push begin

-(void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification{
#ifdef _XG_PUSH_SDK
    [XGPush delLocalNotification:notification];
#endif
}

#if __IPHONE_OS_VERSION_MAX_ALLOWED >= _IPHONE80_

//注册UserNotification成功的回调
- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings
{
    //用户已经允许接收以下类型的推送
    //UIUserNotificationType allowedTypes = [notificationSettings types];
    
}

//按钮点击事件回调
- (void)application:(UIApplication *)application handleActionWithIdentifier:(NSString *)identifier forRemoteNotification:(NSDictionary *)userInfo completionHandler:(void (^)())completionHandler
{
#ifdef _XG_PUSH_SDK
    if([identifier isEqualToString:@"ACCEPT_IDENTIFIER"]){
        NSLog(@"ACCEPT_IDENTIFIER is clicked");
    }
    
    completionHandler();
#endif
}

#endif

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    
#ifdef _XG_PUSH_SDK
    //NSString * deviceTokenStr = [XGPush registerDevice:deviceToken];
    void (^successBlock)(void) = ^(void){
        //成功之后的处理
        NSLog(@"[XGPush Demo]register successBlock");
    };
    void (^errorBlock)(void) = ^(void){
        //失败之后的处理
        NSLog(@"[XGPush Demo]register errorBlock");
    };
    //注册设备
    NSString * deviceTokenStr = [XGPush registerDevice:deviceToken successCallback:successBlock errorCallback:errorBlock];
    std::string strToken = [deviceTokenStr UTF8String];
    CSDKManager::GetInstance()->SetXGToken(strToken);
    //如果不需要回调
    //[XGPush registerDevice:deviceToken];
    //打印获取的deviceToken的字符串
    NSLog(@"[XGPush] deviceTokenStr is %@",deviceTokenStr);
#endif
}

//如果deviceToken获取不到会进入此事件
- (void)application:(UIApplication *)app didFailToRegisterForRemoteNotificationsWithError:(NSError *)err {
#ifdef _XG_PUSH_SDK
    NSString *str = [NSString stringWithFormat: @"Error: %@",err];
    NSLog(@"[XGPush]%@",str);
#endif
}

- (void)application:(UIApplication*)application didReceiveRemoteNotification:(NSDictionary*)userInfo
{
#ifdef _XG_PUSH_SDK
    //推送反馈(app运行时)
    [XGPush handleReceiveNotification:userInfo];
#endif
    
}
// for xg push end
@end
