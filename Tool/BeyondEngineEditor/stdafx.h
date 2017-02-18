// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <map>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <mutex>
#include <thread>
#include <memory>
#include "wx/wx.h"

#include "EnginePublic/BeyondEnginePublic.h"

#include "glfw3.h"

#include "Resource/ResourcePublic.h"
#include "Render/RenderPublic.h"
#include "Utility/BeatsUtility/SharePtr.h"


#define INVALID_DATA -1
#define DEFAULT_PGBUTTONWIDTH 20
#define DEFAULT_DEPTH 32
#define MINWINDOWSIZE 50
#define TREEICONSIZE 15

#define SHIFTMOUSESPEED
#define WHEELSPEED 3.0F * 0.016F
#define SHIFTWHEELSPEED 6.0F * 0.016F

#define MAXFPS 60
#define UPDATEFPS 30

#define WARNING "ERROR : "

enum ETreeCtrlIconType
{
    eTCIT_File,
    eTCIT_FileSelected,
    eTCIT_Folder,
    eTCIT_FolderSelected,
    eTCIT_FolderOpened,

    eTCIT_Count,
    eTCIT_Force32Bit = 0xFFFFFFFF
};
