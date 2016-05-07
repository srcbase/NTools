#pragma once
// 注入的服务窗口名，用来等待用户的消息命令；参考FindWindow
#define _KP_HOST_SERVICE_NAME	_T("KPDesk Service")


// 自定义事件
/*****************************************************************************************
	<< --- 自定义事件：WM_KPDESK_HOOKINIT	2014-05-20 --- >>
	说明：当某个绘图对象发生动作时触发
	参数：
		WPARAM	= 
		LPARAM	= 用来接收dll消息的客户端句柄HWND
	接收者：
		对话框		= IDD_SERVICE
	返回值：
*****************************************************************************************/
#define WM_KPDESK_HOOKINIT		WM_USER + 10

/*****************************************************************************************
	<< --- 自定义事件：WM_KPDESK_DEBUGLOG	2014-05-20 --- >>
	说明：用来接收dll的调试信息
	参数：
		WPARAM	= 调试信息字符数
		LPARAM	= 调试信息wchar_t*
	接收者：
		客户端	= 
	返回值：
*****************************************************************************************/
#define WM_KPDESK_DEBUGLOG		WM_USER + 11

 
//////////////////////////////////////////////////////////////////////////////////////////
// 
// 对应皮肤文件中的对象名称
// 
//////////////////////////////////////////////////////////////////////////////////////////
// 窗口标题区
#define kpButtonLock			L"btnLock"		// 锁定按钮
#define kpButtonMenu 			L"btnMenu"		// 菜单总按钮
#define kpScrollbarV			L"srlVertical"
#define kpButtonTitle 			L"btnTitle"		// 标题显示按钮，双击时可编辑标题
#define kpREditTitle 			L"reditTitle"	// 标题内容编辑框
#define kpButtonNode 			L"btnNode"		// 容器节点按钮
#define kpTextTitle 			L"txtTitle"		// 标题显示文本

