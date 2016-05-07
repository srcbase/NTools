// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WINUC_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WINUC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef WINUC_EXPORTS
#define WINUC_API __declspec(dllexport)
#else
#define WINUC_API __declspec(dllimport)
#endif

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


#include "UCwindow.h"
#include "UCmanager.h"
#include "UCcontrol.h"