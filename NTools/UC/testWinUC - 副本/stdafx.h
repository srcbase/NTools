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
#include "AC/resourceManager.h"
#include "AC/draw/image.h"


#include "UC/winUC/UCwin.h"
//using namespace wuc;
#pragma comment(lib, "D:/NTools/UC/bin/winUC.lib")

#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")

#include "kpdeskDialog.h"
#include "kpdeskNotify.h"
// TODO: reference additional headers your program requires here
