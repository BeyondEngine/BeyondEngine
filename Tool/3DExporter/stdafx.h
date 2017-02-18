// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <map>
#include <cstdlib>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "Framework/PlatformDefine.h"
#ifdef BEYONDENGINE_PLATFORM
#undef BEYONDENGINE_PLATFORM
#endif
#define BEYONDENGINE_PLATFORM PLATFORM_WIN32
#include "EnginePublic/PublicDef.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Resource/ResourcePublic.h"