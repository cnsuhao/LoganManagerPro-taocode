// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

#include "resource.h"
#include <queue>
#include <stack>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>


#include "../LoganUI/UIlib.h"
#include "tinystr.h"
#include "tinyxml.h"


using namespace std;							
using namespace DuiLib;

#ifdef _DEBUG									
#pragma comment(lib, "..\\lib\\DuiLib_d.lib")
#else											
#pragma comment(lib, "..\\lib\\DuiLib.lib")
#endif



#include "../VideoPlayer/IVideoPlayer.h"
#ifdef _DEBUG
#pragma  comment(lib,"../lib/VideoPlayerD.lib")
#else
#pragma  comment(lib,"../lib/VideoPlayer.lib")
#endif


#define WM_SINGLE_WINDOW					WM_USER+300
#define WM_MESSAGEBOX						WM_USER+301
