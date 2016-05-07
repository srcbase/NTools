// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
//#include <windows.h>
//
//// C RunTime Header Files
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
//#include <tchar.h>
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

#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")
#include <WinGDI.h>
#pragma comment(lib, "Msimg32.lib")	// AlphaBlend

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")// SysListView32...

#include "AC/basedef.h"
#include "AC/baseobj.h"
#include "AC/sysupport.h"
#include "AC/draw/drawer.h"
// WUC
#include "UC/windowWnd.h"
#include "UC/drawManager.h"
#include "UC/wucControl.h"
#include "UC/wucContainer.h"
#include "UC/wucBase.h"
#include "UC/wucAnimate.h"

#include "kpdeskDialog.h"
#include "kpdeskNotify.h"
// TODO: reference additional headers your program requires here
