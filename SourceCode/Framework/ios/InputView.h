#import <UIKit/UIKit.h>
typedef void(^InputAccessoryBlock)(NSString *contentStr);

@interface InputView : UIView<UITextFieldDelegate>
{
    bool mbShow;
}
@property (strong, nonatomic) IBOutlet UIView *contentView;
@property (strong, nonatomic) IBOutlet UITextField *textField;
@property (strong, nonatomic) IBOutlet UIButton *btn;
@property (nonatomic,strong) InputAccessoryBlock block;

- (void)showBlock:(InputAccessoryBlock)block;
- (void)setStrText:(NSString *)strText;
-(void)close;
-(void)show;
@end
