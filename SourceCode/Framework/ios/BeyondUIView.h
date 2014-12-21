
#import <UIKit/UIView.h>
#import <GLKit/GLKit.h>
#import <CoreFoundation/CoreFoundation.h>
#import "InputView.h"

@interface BeyondView : GLKView
{
    InputView* m_pInputView;
}
-(void)ShowKeyboard;
-(void)HideKeyboard;
@end