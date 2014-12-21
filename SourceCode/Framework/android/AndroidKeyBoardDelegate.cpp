#include "stdafx.h"
#include "AndroidKeyBoardDelegate.h"
#include "Framework/android/JniHelper.h"

CAndroidKeyBoardDelegate::CAndroidKeyBoardDelegate()
{

}

CAndroidKeyBoardDelegate::~CAndroidKeyBoardDelegate()
{

}

void CAndroidKeyBoardDelegate::ShowKeyBoard()
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, _T("ShowKeyBoard"), _T("()V")))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    }
}

void CAndroidKeyBoardDelegate::HideKeyBoard()
{
    JniMethodInfo methodInfo;
    if (JniHelper::getStaticMethodInfo(methodInfo, ACTIVITY_CLASS_NAME, _T("HideKeyBorad"), _T("()V")))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    }
}
