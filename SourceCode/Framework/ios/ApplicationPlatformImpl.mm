#include "Framework/Application.h"
#import "Framework/ios/AppDelegate.h"

int CApplication::Run(void *)
{
    @autoreleasepool {
    return UIApplicationMain(0, nullptr, nil, NSStringFromClass([AppDelegate class]));
    }
}
