// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#pragma warning (disable : 4251) // STLÄ£°åÀà¾¯¸æ

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <ObjBase.h>
#include <assert.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"shell32.lib")

#include "UI/win/WinUI/winUI.h"
using namespace wui;
#pragma comment(lib, "D:/NTools/UI/win/bin/winUI.lib")

//#include "KP/KPDeskData.h"
//#include "KP/KPDeskInfo.h"
//#include "kpdeskNotify.h"
// TODO: reference additional headers your program requires here
