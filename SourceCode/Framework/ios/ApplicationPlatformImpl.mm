#include "Framework/Application.h"
#import "Framework/ios/AppDelegate.h"

int CApplication::Run(int argc, char * argv[], void *)
{
    @autoreleasepool {
    return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}

void CApplication::OnSwitchToBackground()
{
    Pause();
}

void CApplication::OnSwitchToForeground()
{
    Resume();
}