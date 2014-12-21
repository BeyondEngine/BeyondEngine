#ifndef BEYOND_ENGINE_GUI_IOSKEYBORADDELEGATE_H__INCLUDE
#define BEYOND_ENGINE_GUI_IOSKEYBORADDELEGATE_H__INCLUDE
#include "GUI/KeyBoardDelegate.h"
#import <UIKit/UIView.h>
#import "BeyondUIView.h"

class CIOSKeyBoardDelegate : public CKeyBoardDelegate
{
public:
    CIOSKeyBoardDelegate();
    virtual ~CIOSKeyBoardDelegate();

    virtual void ShowKeyBoard() override;

    virtual void HideKeyBoard() override;
    
    void SetUIView( BeyondView* pView );
    
private:
    BeyondView* m_pView;
};

#endif
