// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
// Windows Header Files:

#include "AC/basedef.h"
#include "AC/sysupport.h"
#include "AC/baseobj.h"
#include "AC/resourceManager.h"
#include "AC/draw/image.h"
#include "AC/draw/drawer.h"


#include "UCWin.h"
#include <olectl.h>


#define CLAMP(x,a,b) (min(b,max(a,x)))



// TODO: reference additional headers your program requires here
