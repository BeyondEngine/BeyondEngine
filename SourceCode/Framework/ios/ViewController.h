#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import "BeyondUIView.h"
#include "stdafx.h"

#define TAG_LOGIN_ALERT         1
#define TAG_LOGOUT_ALERT        2
#define TAG_REQUEST_PAY         3
#define TAG_APPRAISE            4

#define _SHARE_STRING_ @"com.chaoyuehudong.3body.share"

#ifdef _SUPER_SDK
#import <SuperSdk/SuperSdkOpenApi.h>
#import <SuperSdk/SuperSdkGameData.h>
#import <SuperSdk/SuperSdkDefines.h>

@interface ViewController : GLKViewController<UIAlertViewDelegate,SuperSdkOpenApiDelegate>
@property (nonatomic, strong) SuperSdkGameData *gameData;
#else
@interface ViewController : GLKViewController<UIAlertViewDelegate>
#endif
@property (nonatomic, strong) UITextView *textView;
- (void)showMessage:(NSString *)message;
- (void)hideMessage:(float)fTime;
- (void)enterBackGround;
- (void)setfps:(int)nfps;
@end
