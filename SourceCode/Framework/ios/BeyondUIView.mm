#include "stdafx.h"
#import "BeyondUIVIew.h"
#include "GUI/IMEManager.h"

@implementation BeyondView

-(void)ShowKeyboard
{
    if (m_pInputView == nil)
    {
        m_pInputView = [InputView new];
        [m_pInputView showBlock:^(NSString *contentStr) {
            [self endEdit:contentStr];
        }];
    }
    TString strText = CIMEManager::GetInstance()->GetCurrText();
    NSString* nstrText = [NSString stringWithUTF8String:strText.c_str()];
    [m_pInputView setStrText:nstrText];
    [m_pInputView show];
}

-(void)HideKeyboard
{
    if(m_pInputView != nil)
    {
        [m_pInputView close];
    }
}

-(void)endEdit:(NSString*)str{
    NSRegularExpression *regularExpression = [NSRegularExpression regularExpressionWithPattern:@"\u2006" options:0 error:nil];
    str = [regularExpression stringByReplacingMatchesInString:str options:0 range:NSMakeRange(0, str.length) withTemplate:@""];
    const char * pszText =[str UTF8String];
    CIMEManager::GetInstance()->OnCharReplace(pszText);
}

-(void)dealloc{
    if(m_pInputView != nil)
    {
        [m_pInputView release];
        m_pInputView = nil;
    }
    [super dealloc];
}

@end