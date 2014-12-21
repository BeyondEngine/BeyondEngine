#ifndef BEYOND_ENGINE_GUI_ANDROIDKEYBORADDELEGATE_H__INCLUDE
#define BEYOND_ENGINE_GUI_ANDROIDKEYBORADDELEGATE_H__INCLUDE

#include "GUI/KeyBoardDelegate.h"

class CAndroidKeyBoardDelegate : public CKeyBoardDelegate
{
public:
    CAndroidKeyBoardDelegate();
    virtual ~CAndroidKeyBoardDelegate();

    virtual void ShowKeyBoard() override;

    virtual void HideKeyBoard() override;
};

#endif
