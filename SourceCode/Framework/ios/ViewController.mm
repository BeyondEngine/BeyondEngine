#include "stdafx.h"
#import "ViewController.h"
#include "Framework/Application.h"
#include "Render/RenderManager.h"
#include "Render/Viewport.h"
#include "Render/ios/GLKRenderWindow.h"
#include "Event/TouchDelegate.h"
#include "Event/GestureState.h"
#include "Event/Touch.h"
#include "framework/ios/IOSKeyBoardDelegate.h"
#include "GUI/IMEManager.h"
#include "starraiderscenter.h"
#include "PlatformHelper.h"
#include "Resource/ResourceManager.h"
#include "BeyondEngineUI/UISystemManager.h"

#ifdef _SUPER_SDK
#include "external/SDKManager.h"
#endif


@interface ViewController () {
    GLKRenderWindow *m_pRenderWindow;
    CIOSKeyBoardDelegate* m_pKeyBorad;
    bool m_bNeedUpdateThisFrame;
}
@property (strong, nonatomic) EAGLContext *context;

@end

@implementation ViewController
#define heightSelf  self.view.bounds.size.height
#define widthSelf   self.view.bounds.size.width

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self InitGameContext];
    m_bNeedUpdateThisFrame = true;
}

-(NSUInteger)supportedInterfaceOrientations{
    return UIInterfaceOrientationMaskLandscape;
}

-(void)loadView
{
    [super loadView];
    m_pKeyBorad = new CIOSKeyBoardDelegate();
    BeyondView* pVIew = [[BeyondView alloc] initWithFrame:self.view.bounds];
    m_pKeyBorad->SetUIView(pVIew);
    CIMEManager::GetInstance()->SetKeyBoardDelegate(m_pKeyBorad);
    self.view = pVIew;
}

- (void)dealloc
{
    BEATS_SAFE_DELETE(m_pKeyBorad);
    BEATS_SAFE_DELETE(m_pRenderWindow);
    CRenderManager::GetInstance()->SetCurrentRenderTarget(nullptr);
    
    [super dealloc];
}

- (void)enterBackGround{
    m_pKeyBorad->HideKeyBoard();
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    CResourceManager::GetInstance()->CleanUp();
    BEATS_PRINT("BeyondEngine receive memory warning!");
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    int width = self.view.bounds.size.width * self.view.contentScaleFactor;
    int height = self.view.bounds.size.height * self.view.contentScaleFactor;
    if(CApplication::GetInstance()->IsRunning() && (width != CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetDeviceWidth()||
        height != CRenderManager::GetInstance()->GetCurrentRenderTarget()->GetDeviceHeight() ))
    {
        CRenderManager::GetInstance()->GetCurrentRenderTarget()->SetDeviceSize(width, height);
    }
    CEngineCenter* pEngineCenter = CEngineCenter::GetInstance();
    m_bNeedUpdateThisFrame = pEngineCenter->ShouldUpdateThisFrame();
    if (m_bNeedUpdateThisFrame)
    {
        CApplication::GetInstance()->Update();
    }
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    if (m_bNeedUpdateThisFrame)
    {
        CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
        CApplication::GetInstance()->Render();
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* ids[TOUCH_MAX_NUM] = {0};
    float xs[TOUCH_MAX_NUM] = {0.0f};
    float ys[TOUCH_MAX_NUM] = {0.0f};
    unsigned int tapCount[TOUCH_MAX_NUM] = {0};
    
    int i = 0;
    for(UITouch *touch in touches) {
        ids[i] = touch;
        CGPoint pos = [touch locationInView: self.view];
        xs[i] = pos.x * self.view.contentScaleFactor / m_pRenderWindow->GetScaleFactor();
        ys[i] = pos.y * self.view.contentScaleFactor / m_pRenderWindow->GetScaleFactor();
        tapCount[i] = (unsigned int)touch.tapCount;
        ++i;
        if(i >= TOUCH_MAX_NUM)
            break;
    }
    CTouchDelegate::GetInstance()->OnTouchBegan(i, (size_t*)ids, xs, ys);
#ifdef DEVELOP_VERSION
    NSLog(@"frames perscound:%d\n",self.framesPerSecond);
    NSLog(@"preferred frames perscound:%d\n",self.preferredFramesPerSecond);
    NSLog(@"game fps:%d\n", CEngineCenter::GetInstance()->GetFPS());
    NSLog(@"game actual fps:%.2f\n", CEngineCenter::GetInstance()->GetActualFPS());
#endif
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* ids[TOUCH_MAX_NUM] = {0};
    float xs[TOUCH_MAX_NUM] = {0.0f};
    float ys[TOUCH_MAX_NUM] = {0.0f};
    unsigned int tapCount[TOUCH_MAX_NUM] = {0};
    
    int i = 0;
    for (UITouch *touch in touches) {
        ids[i] = touch;
        CGPoint pos = [touch locationInView: self.view];
        xs[i] = pos.x * self.view.contentScaleFactor / m_pRenderWindow->GetScaleFactor();
        ys[i] = pos.y * self.view.contentScaleFactor / m_pRenderWindow->GetScaleFactor();
        tapCount[i] = (unsigned int)touch.tapCount;
        ++i;
        if(i >= TOUCH_MAX_NUM)
            break;
    }
    CTouchDelegate::GetInstance()->OnTouchEnded(i, (size_t*)ids, xs, ys);
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* ids[TOUCH_MAX_NUM] = {0};
    float xs[TOUCH_MAX_NUM] = {0.0f};
    float ys[TOUCH_MAX_NUM] = {0.0f};
    
    int i = 0;
    for (UITouch *touch in touches) {
        ids[i] = touch;
        CGPoint pos = [touch locationInView: self.view];
        xs[i] = pos.x * self.view.contentScaleFactor / m_pRenderWindow->GetScaleFactor();
        ys[i] = pos.y * self.view.contentScaleFactor / m_pRenderWindow->GetScaleFactor();
        ++i;
        if(i >= TOUCH_MAX_NUM)
            break;
    }
    CTouchDelegate::GetInstance()->OnTouchMoved(i, (size_t*)ids, xs, ys);
}

-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* ids[TOUCH_MAX_NUM] = {0};
    float xs[TOUCH_MAX_NUM] = {0.0f};
    float ys[TOUCH_MAX_NUM] = {0.0f};
    unsigned int tapCount[TOUCH_MAX_NUM] = {0};
    
    int i = 0;
    for (UITouch *touch in touches) {
        ids[i] = touch;
        CGPoint pos = [touch locationInView: self.view];
        xs[i] = pos.x * self.view.contentScaleFactor / m_pRenderWindow->GetScaleFactor();
        ys[i] = pos.y * self.view.contentScaleFactor / m_pRenderWindow->GetScaleFactor();
        tapCount[i] = (unsigned int)touch.tapCount;
        ++i;
        if(i >= TOUCH_MAX_NUM)
            break;
    }
    CTouchDelegate::GetInstance()->OnTouchCancelled(i, (size_t*)ids, xs, ys);
}

-(void)reportPinch:(UIPinchGestureRecognizer *)recognizer
{
    EGestureState state;
    switch(recognizer.state)
    {
        case UIGestureRecognizerStatePossible:
            state = EGestureState::eGS_POSSIBLE;
            break;
        case UIGestureRecognizerStateBegan:
        {
            state = EGestureState::eGS_BEGAN;
        }
            break;
        case UIGestureRecognizerStateChanged:
            state = EGestureState::eGS_CHANGED;
            break;
        case UIGestureRecognizerStateEnded:
            state = EGestureState::eGS_ENDED;
            break;
        case UIGestureRecognizerStateCancelled:
            state = EGestureState::eGS_CANCELLED;
            break;
        case UIGestureRecognizerStateFailed:
            state = EGestureState::eGS_FAILED;
            break;
    }
    if ([recognizer numberOfTouches] > 1 && (state == EGestureState::eGS_BEGAN || state == EGestureState::eGS_CHANGED))
    {
        float fTouch1PosX = [recognizer locationOfTouch:0 inView:self.view].x * self.view.contentScaleFactor;
        float fTouch1PosY = [recognizer locationOfTouch:0 inView:self.view].y * self.view.contentScaleFactor;
        float fTouch2PosX = [recognizer locationOfTouch:1 inView:self.view].x * self.view.contentScaleFactor;
        float fTouch2PosY = [recognizer locationOfTouch:1 inView:self.view].y * self.view.contentScaleFactor;
        
        float fDeltaX = fTouch1PosX - fTouch2PosX;
        float fDeltaY = fTouch1PosY - fTouch2PosY;
        float fLength = sqrtf(fDeltaX * fDeltaX + fDeltaY * fDeltaY);
        CTouchDelegate::GetInstance()->OnPinched(state, fLength);
    }
}

-(void)requestPayTimeOut
{
#ifdef _SUPER_SDK
    UIActivityIndicatorView *indicator = (UIActivityIndicatorView *)[self.view viewWithTag:TAG_REQUEST_PAY];
    if ([indicator isAnimating]) {
        CSDKManager::GetInstance()->OnGetOrderTimeOut();
    }
#endif
}

-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (alertView.tag == TAG_LOGIN_ALERT) {
        UITextField* accountField = [alertView textFieldAtIndex:0];
        UITextField* passwordField = [alertView textFieldAtIndex:1];
    
        CStarRaidersCenter::GetInstance()->SetAccountString([[accountField text] UTF8String]);
        CStarRaidersCenter::GetInstance()->SetPasswordString([[passwordField text] UTF8String]);
        
        NSUserDefaults *user = [NSUserDefaults standardUserDefaults];
        
        [user setObject:[accountField text] forKey:@"account"];
        [user setObject:[passwordField text] forKey:@"password"];
        
        CPlatformHelper::GetInstance()->SetLoginState(eLoginEnd);
    }
    else if (alertView.tag == TAG_LOGOUT_ALERT)
    {
#ifdef _SUPER_SDK
        //确定注销
        if (buttonIndex == 1) {
            [[SuperSdkOpenApi sharedInstance] logout:_gameData];
        }
#endif
    }
    else if (alertView.tag == TAG_APPRAISE)
    {
        if (buttonIndex == 1) {
            NSString *strUrl = [NSString stringWithFormat: @"itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=%@", @""];
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:strUrl]];
        }
    }
}

- (void)InitGameContext
{
    CPlatformHelper::GetInstance()->InitGameExtrat();
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    [self.view setMultipleTouchEnabled:YES];
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    self.preferredFramesPerSecond = 60;
    
    UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc]
                                       initWithTarget:self action:@selector(reportPinch:)];
    [self.view addGestureRecognizer:pinch];
    pinch.cancelsTouchesInView = NO;
    pinch.delaysTouchesEnded = NO;
    
    int width = self.view.bounds.size.width * self.view.contentScaleFactor;
    int height = self.view.bounds.size.height* self.view.contentScaleFactor;
    
    m_pRenderWindow = new GLKRenderWindow(width, height);
    m_pRenderWindow->SetContext(self.context);
    CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
    
    CApplication::GetInstance()->Initialize();
    CApplication::GetInstance()->Resume();
}

- (void)setfps:(int)nfps
{
    self.preferredFramesPerSecond = nfps;
}

- (void)showMessage:(NSString *)message
{
    dispatch_async(dispatch_get_main_queue(), ^{
        
        UIFont *font = [UIFont systemFontOfSize:36.0f];
        if(!self.textView){
            self.textView = [[UITextView alloc]initWithFrame:CGRectZero];
            self.textView.editable = NO;
            self.textView.font = font;
            self.textView.textColor = [UIColor whiteColor];
            self.textView.backgroundColor = [UIColor colorWithRed:20.0f/255.0f green:20.0f/255.0f blue:20.0f/255.0f alpha:0.6];
            self.textView.layer.cornerRadius = 4.0;
            self.textView.layer.masksToBounds = YES;
            self.textView.textAlignment = NSTextAlignmentCenter;
            [self.view addSubview:self.textView];
        }
        self.textView.hidden = NO;
        self.textView.text = message;
        CGSize size = [message sizeWithFont:font constrainedToSize:CGSizeMake(widthSelf-60.0f, 9999)];
        self.textView.frame = CGRectMake(0, 0, size.width + 32.0f, size.height+15);
        self.textView.center = CGPointMake(self.view.center.x, self.view.center.y/2);
    });
}

- (void)hideMessage:(float)fTime
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [UIView animateWithDuration:fTime animations:^{
            self.textView.alpha = 0.1f;
        } completion:^(BOOL finished) {
            self.textView.hidden = YES;
            self.textView.alpha = 1.0f;
        }];
    });
}

#ifdef _SUPER_SDK
// SuperSdk回调函数
//1.初始化回调
- (void)OPInitFinish:(int)ret withParam:(NSString *)param
{
    NSLog(@"***初始化回调***");
    if(ret==OP_SUCCESS){
        NSLog(@"初始化成功");
        CSDKManager::GetInstance()->OnSuperSdkInitSuccess();
    }else{
        NSLog(@"初始化失败，游戏无法在进行下去");
        CSDKManager::GetInstance()->OnSuperSdkInitFailed();
    }
}

//2.版本检查更新回调
- (void)OPCheckVersion:(int)ret withParam:(NSString *)param
{
    NSLog(@"***版本检查回调***");
    
    NSString *checkVersionResult = @"";
    switch (ret)
    {
        case OP_CHECK_WITH_NEW_VERSION:
        {
            checkVersionResult = @"有更新";
            CSDKManager::GetInstance()->OnCheckWithNewVersion();
        }
            break;
        case OP_CHECK_WITHOUT_NEW_VERSION:
        {
            checkVersionResult = @"没有新版本";
            CSDKManager::GetInstance()->OnCheckWithoutNewVersion();
        }
            break;
        case OP_CHECK_WITHOUT_CHECK_VERSION:
        {
            checkVersionResult = @"没有更新接口，需游戏自己设置更新逻辑";
            CSDKManager::GetInstance()->OnCheckWithoutCheckVersion();
        }
        break;
        default:
            break;
    }
    NSLog(@"版本检查结果是 %@",checkVersionResult);
}

//3.登陆回调
- (void)OPLoginPlatform:(int)ret withParam:(NSString *)param
{
    NSLog(@"***登陆结果回调***");
    if(ret==OP_SUCCESS){
        NSLog(@"平台登陆成功");
        
        NSDictionary *paramDic = [NSJSONSerialization JSONObjectWithData:[param dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
        //保存ticket 去游戏服务器做登陆验证
        std::string strTicket = [[paramDic objectForKey:@"osdk_ticket"] UTF8String];
        CSDKManager::GetInstance()->SetTicket(strTicket);

        self.gameData = [SuperSdkGameData shareInstance];
        //此参数为登录验证成功后返回的参数，不能传空。
        self.gameData.loginData = [paramDic objectForKey:@"data"];
        //登陆成功，调用打开悬浮窗接口
        CSDKManager::GetInstance()->OnLoginSuccess();
        //[self ResumeGame];
    }else{
        NSLog(@"平台登陆失败，code=%d ,原因是 %@",ret, param);
        CSDKManager::GetInstance()->OnLoginFailed();
    }
}

//4.注销回调
- (void)OPLogout:(int)ret withType:(int)type withParam:(NSString *)param
{
    NSLog(@"***注销回调***");
    if (ret == OP_SUCCESS) {
        CSDKManager::GetInstance()->CloseFloatWindow();
        NSLog(@"注销成功");
        switch (type) {
            case LOGOUT_WITH_OPEN_OR_NOT_OPEN_LOGIN:
                NSLog(@"不知道注销后是否会弹出登陆框");
                break;
            case LOGOUT_WITH_NOT_OPEN_LOGIN:
                NSLog(@"注销后不会弹出登陆框");
                break;
            case LOGOUT_WITH_OPEN_LOGIN:
                NSLog(@"注销后会弹出登陆框");
                CSDKManager::GetInstance()->SetHaveLoginDialog(true);
                break;
            default:
                break;
        }
        CUISystemManager::GetInstance()->SetSwallowAllTouch(true);
        CSDKManager::GetInstance()->OnLogoutSuccess();
    } else {
        NSLog(@"注销失败");
        CSDKManager::GetInstance()->OnLogoutFailed();
    }
}

//5.支付结果回调 --- 有部分渠道支付失败是不会有回调的
- (void)OPPayResult:(int)ret withParam:(NSString *)param
{
    NSLog(@"***支付结果回调***");
    if(ret==OP_SUCCESS){
        NSLog(@"支付成功");
        CSDKManager::GetInstance()->OnPaySuccess();
    }else{
        std::string strParam;
        NSDictionary *paramDic = [NSJSONSerialization JSONObjectWithData:[param dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
        if  (paramDic != nil)
        {
            strParam = [[paramDic objectForKey:@"desc"] UTF8String];
        }
        NSLog(@"支付失败，原因是%@",[paramDic objectForKey:@"desc"]);
    }
}

//6.获取订单号成功回调，失败不会有此回调
- (void)OPGetOrderIdSuccess:(int)ret withParam:(NSString *)param
{
    if(ret==OP_SUCCESS){
        NSLog(@"平台订单号获取成功，即将进行渠道支付");
        CSDKManager::GetInstance()->OnGetOrderIdSuccess(0);
    }
}

//游客升级结果回调
- (void)OPUpgradeGuest:(int)ret withParam:(NSString *)param
{
    NSLog(@"***游客升级回调***");
    if(ret == OP_SUCCESS){
        NSLog(@"游客账号升级成功");
    }else{
        NSLog(@"游客账号升级失败");
    }
}

#endif

@end

