#define Screen_Width [[UIScreen mainScreen] bounds].size.width
#define Screen_Height [[UIScreen mainScreen] bounds].size.height
#include "stdafx.h"
#import "InputView.h"
#include "GUI/IMEManager.h"

@implementation InputView
-(void)awakeFromNib{
    [super awakeFromNib];
    [self loadNibFile];
    [self loadUI];
}

-(instancetype)initWithCoder:(NSCoder *)aDecoder{
    if (self = [super initWithCoder:aDecoder]) {
        [self loadNibFile];
        [self loadUI];
    }
    return self;
}
-(instancetype)init{
    mbShow = false;
    if (self = [super init]) {
        [self loadNibFile];
        [self loadUI];
        [self addKeyBoardObserver];
    }
    return self;
}

-(void)loadNibFile{
    [[NSBundle mainBundle]loadNibNamed:@"InputView" owner:self options:nil];
}

-(void)loadUI{
    [self addSubview:self.contentView];
}

-(void)addKeyBoardObserver{
    //监听键盘事件
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardWillShow:)
                                                 name:UIKeyboardWillShowNotification
                                               object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardDidShow:)
                                                 name:UIKeyboardDidShowNotification
                                               object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardWillHide:)
                                                 name:UIKeyboardWillHideNotification
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(keyboardDidHide:)
                                                 name:UIKeyboardDidHideNotification
                                               object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(inputModeDidChange:)
                                                 name:UITextInputCurrentInputModeDidChangeNotification
                                               object:nil];

    
    [self.textField addTarget:self action:@selector(textChanged:) forControlEvents:UIControlEventEditingChanged];
}

- (void) inputModeDidChange:(NSNotification*) notif {
    CGRect rect = [[notif.userInfo objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
    CGFloat y = rect.size.height;
    self.frame = CGRectMake(0, Screen_Height - y - 46, Screen_Width, 46);
}

-(void)layoutSubviews{
    [super layoutSubviews];
    self.contentView.frame = self.bounds;
}

-(void)show{
    self.textField.delegate = self;
    [self.textField becomeFirstResponder];
    UIWindow* window = [UIApplication sharedApplication].keyWindow;
    if (!mbShow) {
        self.frame = CGRectMake(0, Screen_Height, Screen_Width, 46);
    }
    [window addSubview:self];
}

-(void)close{
    if (mbShow) {
        mbShow = false;
        [self removeFromSuperview];
        [self.textField resignFirstResponder];
    }
}

- (void)showBlock:(InputAccessoryBlock)block{
    self.block = block;
}

- (void)setStrText:(NSString *)strText{
    self.textField.text = strText;
}


#pragma mark - 监听键盘事件
- (void)keyboardWillShow:(NSNotification *)notif {
    if (!mbShow) {
        CGRect rect = [[notif.userInfo objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
        CGFloat y = rect.size.height;
        float animationTime = [[notif.userInfo objectForKey:UIKeyboardAnimationDurationUserInfoKey] floatValue];
        [UIView animateWithDuration:animationTime animations:^{
            self.frame = CGRectMake(0, Screen_Height - y - 46, Screen_Width, 46);
        }];
    }
}

- (void)keyboardDidShow:(NSNotification *)notif {
    CGRect rect = [[notif.userInfo objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
    CGFloat y = rect.size.height;
    self.frame = CGRectMake(0, Screen_Height - y - 46, Screen_Width, 46);
    mbShow = true;
}

- (void)keyboardWillHide:(NSNotification *)notif {
    
    [UIView animateWithDuration:0.25 animations:^{
        self.frame = CGRectMake(0, Screen_Height, Screen_Width, 46);
    }];
    
}

- (void)keyboardDidHide:(NSNotification *)notif {
    
    if (self.block&&self.textField.text.length) {
        self.block(self.textField.text);
    }
    [self close];
}

- (void)textChanged:(NSNotification *)notif {
    if (self.block) {
        self.block(self.textField.text);
    }
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string{
    
    int length = CIMEManager::GetInstance()->GetTextLimitLength();
    TString strLimit = CIMEManager::GetInstance()->GetFilterString();
    NSString* nstrLimit = [NSString stringWithUTF8String:strLimit.c_str()];
    NSCharacterSet *doNotWant = [NSCharacterSet characterSetWithCharactersInString:nstrLimit];
    NSString *tempString = [[string componentsSeparatedByCharactersInSet: doNotWant]componentsJoinedByString: @""];
    if (tempString.length != string.length) {
        return NO;
    }
    
    if (string.length == 0){
        return YES;
    }
    if([self isHidden])
    {
        TString strText = CIMEManager::GetInstance()->GetCurrText();
        const char * pszReplace =[string UTF8String];
        int lenExisted = strText.length();
        int lenSelected = range.length;
        int lenReplace = strlen(pszReplace);
        if (lenExisted - lenSelected + lenReplace > length) {
            return NO;
        }
    }
    return YES;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField{
    [self close];
    return YES;
}

- (IBAction)onTapBtn:(id)sender {
    [[UIApplication sharedApplication].keyWindow endEditing:YES];
}

@end
