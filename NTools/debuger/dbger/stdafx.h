// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")// SysListView32...

// gtc
#include "AC/sysupport.h"			// basedef.h  singleton.h  baseException.h  stringUtil.h
									// debugger.h datetime.h
#include "AC/resourceManager.h"		// image.h
#include "AC/resourceFile.h"		// 
#include "AC/draw/drawer.h"			// drawdef.h  baseobj.h  image.h
#include "AC/file.h"				// shellExt.h highLevelAPI.h baseReg.h
#include "AC/delegateBase.h"
// acm
#include "AC/baseReg.h"
// WUC
#include "UC/wucdef.h"				// sysupport.h  baseobj.h  resourceManager.h  image.h  drawer.h
#include "UC/windowWnd.h"			// 
#include "UC/drawManager.h"			// 
#include "UC/wucControl.h"
#include "UC/wucContainer.h"
#include "UC/wucBase.h"
#include "UC/wucList.h"
#include "UC/wucMenu.h"
#include "UC/wucWkeWebkit.h"


// APP
#include "dbger.h"


// TODO: reference additional headers your program requires here
// 很重要哦，指定了common控件的版本，否则tooltip无法正确实现
#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif