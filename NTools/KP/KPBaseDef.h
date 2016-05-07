#ifndef __KPBASEDEF_H__
#define __KPBASEDEF_H__
#pragma once

/****************************************************************************************
	与"快屏桌面工具栏"共享的信息配置文件
	段[deskset]：
		HideIcon：int 是否支持双击隐藏桌面功能，=1支持，否则不支持
	段[SHARE]：
		ShowDesk		：int 桌面是否处于显示状态，=1显示
		KPDESK			：int 桌面整理软件是否处于运行中，=1运行中
		rundesktop		：int 是否运行桌面整理软件，=1运行
		againkpdttool	：int KPDTTool.exe退出后是否再次运行
*****************************************************************************************/
//#define KPDESK_SHARE_INI	L"kpzm.ini"
#define KPDESK_SHARE_INI	L"jzzm.ini"

// 快屏软件间注册消息
// KPDesktop.exe
static UINT gc_uiKPDesktopExit			= RegisterWindowMessageW(L"KPDesktopExit");			// 快屏桌面整理软件接收到的请求退出消息，如：KPDTTool工具栏发出的“关闭桌面整理”
static UINT gc_uiKPDesktopRun			= RegisterWindowMessageW(L"KPDesktopRun");			// 桌面整理软件开启（LPARAM=1）或退出（LPARAM=0）时发送的消息如：桌面整理启动后往KPDTTool.exe发送的消息
// KPDesktop.exe
static UINT gc_uiKPDTToolRunAgain		= RegisterWindowMessageW(L"KPDTToolRunAgain");		// 接收到重启命令
static UINT gc_uiKPDTToolExit			= RegisterWindowMessageW(L"KPDTToolExit");			// 接收到退出命令
// 同步对象
// KPSglw.exe
#define KPGSLW_MUTEX_ONLY				L"KPSGLWMutexRunning"			// 保证只有一个在运行
// KPDesktop.exe
#define KPDESKTOP_MUTEX_ONLY			L"KPDesktopMutexRunning"		// 保证只有一个在运行
// KPDTTool.exe
#define KPDTTOOL_MUTEX_ONLY				L"KPDTToolMutexRunning"			// 保证只有一个在运行
// favoriter.exe
#define KPFAVORITER_MUTEX_ONLY			L"KPFavoriterMutexRunning"		// 保证只有一个在运行
// KPDown.exe
#define KPDOWN_MUTEX_ONLY				L"KPDownMutexRunning"			// 保证只有一个在运行

// 窗口类
// KPSglw.exe
#define	KPSGLW_WND_MAIN_CAPTION			L"KPSglwCaption"				// 主窗口
#define	KPSGLW_WND_MAIN_CLASS			L"KPSglwClass"
#define	KPSGLW_WND_BKGD_CAPTION			L"KPSglwBackgroundCaption"		// 主窗口的父窗口
#define	KPSGLW_WND_BKGD_CLASS			L"KPSglwBackgroundClass"
// KPSglwEx.exe
#define	SGLWEX_WND_MAIN_CAPTION			L"KPSglwCaption"				// 主窗口
#define	SGLWEX_WND_MAIN_CLASS			L"KPSglwClass"

// KPDesktop.exe
#define	KPDESKTOP_WND_MAIN_CAPTION		L"KPDesktopCaption"				// 主窗口
#define	KPDESKTOP_WND_MAIN_CLASS		L"KPDesktopClass"
#define	KPDESKTOP_WND_BKGD_CAPTION		L"KPDesktopBackgroundCaption"	// 主窗口的父窗口
#define	KPDESKTOP_WND_BKGD_CLASS		L"KPDesktopBackgroundClass"
// KPDTTool.exe
#define	KPDTTOOL_WND_MAIN_CAPTION		L"KPDTToolCaption"				// 主窗口
#define	KPDTTOOL_WND_MAIN_CLASS			L"KPDTToolClass"
#define	KPDTTOOL_WND_BKGD_CAPTION		L"KPDTToolBackgroundCaption"	// 主窗口的父窗口
#define	KPDTTOOL_WND_BKGD_CLASS			L"KPDTToolBackgroundClass"
// jzzm.exe
#define KPJZZM_WND_CAPTION				L"桔子桌面"
#define KPJZZM_WND_CLASS				L"JuziDesktopDui"
// favoriter.exe
#define KPFAVORITER_WND_MAIN_CAPTION	L"KPFavoriterCaption"
#define KPFAVORITER_WND_MAIN_CLASS		L"KPFavoriterClass"

// KPDown.exe
#define KPDOWN_WND_MAIN_CAPTION			L"KPDownCaption"
#define KPDOWN_WND_MAIN_CLASS			L"KPDownClass"
#endif