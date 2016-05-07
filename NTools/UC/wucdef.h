#pragma once

#include <cwchar>

#include <windows.h>
#include <windowsx.h>

#include <commctrl.h>
#include <stddef.h>
//#include <richedit.h>
//#include <tchar.h>
#include <assert.h>

#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")

#include "AC/sysupport.h"
#include "AC/baseobj.h"
#include "AC/resourceManager.h"
#include "AC/image.h"
#include "AC/draw/drawer.h"
#include "AC/delegateBase.h"
namespace wuc {
// 对话框常用命令按钮名称
#define UC_DIALOG_BUTTON_OK			L"_buttonOk"				// 确认
#define UC_DIALOG_BUTTON_CANCEL		L"_buttonCancel"			// 取消
#define UC_DIALOG_BUTTON_CLOSE		L"_buttonClose"				// 关闭，一般为“x”按钮
#define UC_DIALOG_BUTTON_MIN		L"_buttonMin"				// 最小化

///////////////////////////////////////////////////////////////////////////////////////////////////
//	窗口样式	2015-1-19
#define UC_WNDSTYLE_CONTAINER  (0)
#define UC_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UC_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UC_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UC_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UC_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UC_CLASSSTYLE_CONTAINER  (0)
#define UC_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UC_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UC_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)

///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-20	将WTL消息传递给WUC，以便进行预处理
#define WTL_TO_WUC_MSG_HANDLE()				\
	if (true)														\
	{																\
		bHandled = FALSE;											\
		lResult = handleMessage(uMsg, wParam, lParam, bHandled);	\
		if(bHandled) return TRUE;									\
	}
//	2015-1-20	对应WM_NCDESTROY消息的默认处理函数
#define WTL_TO_WUC_MSG_FINAL()				\
	if (uMsg == WM_NCDESTROY)							\
	{													\
		onFinalMessage(hWnd);							\
		return TRUE;									\
	}
//	2015-1-20	对应WM_INITDIALOG消息的默认处理函数
#define WTL_TO_WUC_MSG_INITDIALOG()			\
	if (uMsg == WM_INITDIALOG)							\
	{													\
		return onInitDialog();							\
	}
//	2015-1-20	对应WM_DESTROY消息的默认处理函数
#define WTL_TO_WUC_MSG_DESTROY()			\
	if (uMsg == WM_DESTROY)								\
	{													\
		onDestroy();									\
		return TRUE;									\
	}
/////////////////////////////////////////////////////////////////////////////////////
// UC控件事件
typedef enum BEUCEvent
{
	BEUCEventFirst = 1,
	BEUCEventKeyBegin,
	BEUCEventKeyDown,
	BEUCEventKeyUp,
	BEUCEventChar,
	BEUCEventSysKey,
	BEUCEventKeyEnd,
	BEUCEventMouseBegin,
	BEUCEventMouseMove,
	BEUCEventMouseLeave,
	BEUCEventMouseEnter,
	BEUCEventMouseHover,
	BEUCEventButtonDown,
	BEUCEventButtonUp,
	BEUCEventRButtonDown,
	BEUCEventDBLClick,
	BEUCEventContextMenu,
	BEUCEventScrollWheel,
	BEUCEventMouseEnd,
	BEUCEventKillFocus,
	BEUCEventSetFocus,
	BEUCEventWindowSize,
	BEUCEventSetCursor,
	BEUCEventTimer,
	BEUCEventNotify,
	BEUCEventCommand,
	BEUCEventLast,
}BEUCEvent;
static std::wstring toString(BEUCEvent eventFlag)
{
	switch(eventFlag)
	{
	case BEUCEventFirst: //= 1,
		return L"BEUCEventFirst";
		break;
	case BEUCEventKeyBegin:
		return L"BEUCEventKeyBegin";
		break;
	case BEUCEventKeyDown:
		return L"BEUCEventKeyDown";
		break;
	case BEUCEventKeyUp:
		return L"BEUCEventKeyUp";
		break;
	case BEUCEventChar:
		return L"BEUCEventChar";
		break;
	case BEUCEventSysKey:
		return L"BEUCEventSysKey";
		break;
	case BEUCEventKeyEnd:
		return L"BEUCEventKeyEnd";
		break;
	case BEUCEventMouseBegin:
		return L"BEUCEventMouseBegin";
		break;
	case BEUCEventMouseMove:
		return L"BEUCEventMouseMove";
		break;
	case BEUCEventMouseLeave:
		return L"BEUCEventMouseLeave";
		break;
	case BEUCEventMouseEnter:
		return L"BEUCEventMouseEnter";
		break;
	case BEUCEventMouseHover:
		return L"BEUCEventMouseHover";
		break;
	case BEUCEventButtonDown:
		return L"BEUCEventButtonDown";
		break;
	case BEUCEventButtonUp:
		return L"BEUCEventButtonUp";
		break;
	case BEUCEventRButtonDown:
		return L"BEUCEventRButtonDown";
		break;
	case BEUCEventDBLClick:
		return L"BEUCEventDBLClick";
		break;
	case BEUCEventContextMenu:
		return L"BEUCEventContextMenu";
		break;
	case BEUCEventScrollWheel:
		return L"BEUCEventScrollWheel";
		break;
	case BEUCEventMouseEnd:
		return L"BEUCEventMouseEnd";
		break;
	case BEUCEventKillFocus:
		return L"BEUCEventKillFocus";
		break;
	case BEUCEventSetFocus:
		return L"BEUCEventSetFocus";
		break;
	case BEUCEventWindowSize:
		return L"BEUCEventWindowSize";
		break;
	case BEUCEventSetCursor:
		return L"BEUCEventSetCursor";
		break;
	case BEUCEventTimer:
		return L"BEUCEventTimer";
		break;
	case BEUCEventNotify:
		return L"BEUCEventNotify";
		break;
	case BEUCEventCommand:
		return L"BEUCEventCommand";
		break;
	case BEUCEventLast:
		return L"BEUCEventLast";
		break;
	}
	return L"";
}
// 对话框响应状态
typedef enum BEAnswerState 
{
	BEAnswerStateNone		= 0,	// 没有应答
	BEAnswerStateOK			,		
	BEAnswerStateCancel		,
	BEAnswerStateAbort		, 
	BEAnswerStateRetry		,
	BEAnswerStateIgnore		, 
	BEAnswerStateYes		, 
	BEAnswerStateNo			, 
	BEAnswerStateClose		,
	BEAnswerStateHelp		, 
	BEAnswerStateTryAgain	, 
	BEAnswerStateContinue	, 
	BEAnswerStateTimeout
}BEAnswerState;
// Flags for controlUC::GetControlFlags()
#define UCFLAG_TABSTOP       0x00000001
#define UCFLAG_SETCURSOR     0x00000002
#define UCFLAG_WANTRETURN    0x00000004

// Flags for FindControl()
#define UCFIND_ALL           0x00000000
#define UCFIND_VISIBLE       0x00000001
#define UCFIND_ENABLED       0x00000002
#define UCFIND_HITTEST       0x00000004
#define UCFIND_TOP_FIRST     0x00000008
#define UCFIND_ME_FIRST      0x80000000

// Flags for the CDialogLayout stretching
#define UCSTRETCH_NEWGROUP   0x00000001
#define UCSTRETCH_NEWLINE    0x00000002
#define UCSTRETCH_MOVE_X     0x00000004
#define UCSTRETCH_MOVE_Y     0x00000008
#define UCSTRETCH_SIZE_X     0x00000010
#define UCSTRETCH_SIZE_Y     0x00000020
// Flags used for controlling the paint
#define UCSTATE_NONE		0x00000000
#define UCSTATE_FOCUSED		0x00000001
#define UCSTATE_SELECTED	0x00000002
#define UCSTATE_DISABLED	0x00000004
#define UCSTATE_HOT			0x00000008
#define UCSTATE_PUSHED		0x00000010
#define UCSTATE_READONLY	0x00000020
#define UCSTATE_CAPTURED	0x00000040
#define UCSTATE_CUTED		0x00000080		//	2015-1-29	剪切状态

///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-23	为图片而增加的标记
#define UCIMAGE_STATE_BACKGROUND	0x00000001		// 控件背景图
#define UCIMAGE_STATE_NORMAL		0x00000002		// 正常显示时
#define UCIMAGE_STATE_FOCUSED		0x00000004		// 得到焦点时
#define UCIMAGE_STATE_SELECTED		0x00000008		// 选中时
#define UCIMAGE_STATE_DISABLED		0x00000010		// 控件无效时
#define UCIMAGE_STATE_HOT			0x00000020		// 得到鼠标时
#define UCIMAGE_STATE_PUSHED		0x00000040		// 按下时
#define UCIMAGE_STATE_READONLY		0x00000080		// 控件处于只读时
#define UCIMAGE_STATE_CAPTURED		0x00000100		// 捕获鼠标时
#define UCIMAGE_STATE_CUTED			0x00000200		// 被剪切时
#define UCIMAGE_STATE_CHECKED		0x00000400		// check按钮选中时

#define UCIMAGE_SCROLL_BUTTON1		0x00010000		// 滚动条的button1，与UCIMAGE_STATE_xxx组合使用
#define UCIMAGE_SCROLL_BUTTON2		0x00020000		// 滚动条的button2，与UCIMAGE_STATE_xxx组合使用
//#define UCIMAGE_SCROLL_BK			0x00040000		// 《直接使用UCIMAGE_STATE_xxx》
#define UCIMAGE_SCROLL_THUMB		0x00040000		// 滚动条滑块的背景，与UCIMAGE_STATE_xxx组合使用
#define UCIMAGE_SCROLL_RAIL			0x00080000		// 滚动条滑块的图案，与UCIMAGE_STATE_xxx组合使用

#define UCIMAGE_PROGRESS_FORE		0x00100000		// 进度条滑动块图案



}