#ifndef BEYOND_ENGINE_GUI_KEYBORADDELEGATE_H__INCLUDE
#define BEYOND_ENGINE_GUI_KEYBORADDELEGATE_H__INCLUDE

class CKeyBoardDelegate
{
public:
    CKeyBoardDelegate();
    virtual ~CKeyBoardDelegate();

    virtual void ShowKeyBoard();

    virtual void HideKeyBoard();
};

#endif
