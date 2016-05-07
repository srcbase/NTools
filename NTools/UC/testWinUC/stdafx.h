// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#pragma warning (disable : 4251) // STLÄ£°åÀà¾¯¸æ

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
//#include <windows.h>
//
//// C RunTime Header Files
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
//#include <tchar.h>
#include "AC/basedef.h"
#include "AC/sysupport.h"
#include "AC/baseobj.h"
#include "AC/resourceManager.h"
#include "AC/image.h"
#include "AC/draw/drawer.h"



#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <olectl.h>

// stl
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <algorithm>
#include <cwchar>
#include <map>
//#include "UC/winUC/UCwin.h"
////using namespace wuc;
//#pragma comment(lib, "D:/NTools/UC/bin/winUC.lib")
#include "UC/winUC/UCwindow.h"
#include "UC/winUC/UCmanager.h"
#include "UC/winUC/UCcontrol.h"
#include "UC/winUC/UCcommonControls.h"

#include <GdiPlus.h>
#pragma comment( lib, "gdiplus.lib")
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )

#pragma comment( lib, "Msimg32.lib")	// AlphaBlend

#include "kpdeskDialog.h"
#include "kpdeskNotify.h"
// TODO: reference additional headers your program requires here
