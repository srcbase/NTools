#ifndef __DLLHOOK_H__
#define __DLLHOOK_H__
#pragma once

#include <string>
#include <vector>
#include <cwchar>
#if defined(_AFXDLL)
	#include <afxwin.h>         // MFC 核心组件和标准组件
	#include <afxext.h>         // MFC 扩展
	#include <afxframewndex.h>
	#include <ATLComTime.h>
	#include <comutil.h>
	#include <afxdisp.h>
	#include <comdef.h>
	#include <assert.h>
#endif
#if defined(_WIN32) && !defined(_AFXDLL)
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <olectl.h>
#endif
namespace acm {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	<< --- dllhook	2014-05-20 --- >>
//	dll注入相关操作
////////////////////////////////////////////////////////////////////////////////////////////////////////
class dllhook {
private:
	dllhook(const dllhook &hook)
	{
		m_vecDllFullPath = hook.m_vecDllFullPath;
		m_dwProcessID = hook.m_dwProcessID;
	}
	dllhook &operator=(const dllhook &hook)
	{
		return *this;
	}
public:
	dllhook() : m_dwProcessID(0)
	{

	}
	dllhook(DWORD processID)
		: m_dwProcessID(processID)
	{

	}
	virtual ~dllhook()
	{
		//remoteUnhook();
	}
public:
	/*****************************************************************************************
		<< --- dllhook::remoteHook		2014-05-20 --- >>
		说明：采用远程注入方式注入到进程中
		参数：
		返回值：
		= 0 成功
		> 0 失败
	*****************************************************************************************/
	bool remoteHook();

	/*****************************************************************************************
		<< --- dllhook::remoteUnhook		2014-05-20 --- >>
		说明：将dll从远端进程中卸载
		参数：
		返回值：
		= 0 成功
		> 0 失败
	*****************************************************************************************/
	bool remoteUnhook();
public:
	//std::wstring	getDllFullPath() const {						return m_wsDllFullPath;									}
	//void			setDllFullPath(const wchar_t *pfullpath) {		m_wsDllFullPath = (pfullpath==nullptr?L"":pfullpath);	}
	//void			setDllFullPath(const std::wstring &fullpath) {	m_wsDllFullPath = fullpath;								}
	void			addDllFullPath(PCWSTR pdllpath) {
		assert(pdllpath != nullptr && std::wcslen(pdllpath) > 0);
		m_vecDllFullPath.push_back(pdllpath);
	}
	void			addDllFullPath(const std::wstring &dllpath) {
		assert(dllpath.length() > 0);
		m_vecDllFullPath.push_back(dllpath);
	}
	const std::vector<std::wstring>& getDllFullPath() const {		return m_vecDllFullPath;				}
	DWORD			getProcessID() const {							return m_dwProcessID;									}
	void			setProcessID(DWORD processID) {					m_dwProcessID = processID;								}
public:
	// 
	/*****************************************************************************************
		<< --- dllhook::findWindow		2014-09-02 --- >>
		说明：查找指定窗口句柄
		参数：
		pparentClasssName	= 被找窗口所属父窗口注册类名（如：WorkerW，注意可能有多个哦！）
		pclassName			= 被找窗口注册类名
		返回值：窗口句柄，如果没找到返回NULL
	*****************************************************************************************/
	static HWND findWindow(PCWSTR pparentClasssName, PCWSTR pclassName);
	// 被findWindow使用的回调函数
	static BOOL CALLBACK enumWindowsProc( HWND hWnd, LPARAM lParam );
private:
	// 被注入dll的全路径
	std::vector<std::wstring>	m_vecDllFullPath;
	// 被注入进程的ID
	DWORD			m_dwProcessID;
};


}
#endif