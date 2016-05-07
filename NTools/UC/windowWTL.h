#ifndef __WINDOWWTL_H__
#define __WINDOWWTL_H__
#pragma once
#include <atlbase.h>
#include <atlapp.h>
#include "wucdef.h"
namespace wuc
{

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	windowWTL 类	2015-1-19
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class windowWTL : public CWindowImpl<windowWTL>
{
protected:
	BEGIN_MSG_MAP(windowWTL)
		WTL_TO_WUC_MSG_HANDLE()
		WTL_TO_WUC_MSG_FINAL()
	END_MSG_MAP()
public:
	windowWTL(void);
	~windowWTL(void);
	HWND getHWND() const;
	//operator HWND() const;	// CWindow已经实现
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-1-20	创建窗口
	HWND create(HWND hWndParent, RECT rc, PCWSTR pWindowName = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0, _U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-1-20	构建窗口类信息
	ATL::CWndClassInfo GetWndClassInfo();
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	窗口样式	2015-1-19
	virtual PCWSTR getWindowClassName() const = 0;
	virtual PCWSTR getWindowCaption() const {		return NULL;									}
	virtual UINT getClassStyle() const {			return 0;										}
	virtual PCWSTR getSuperClassName() const {		return NULL;									}
	virtual UINT getWindowStyle() const {			return 0;										}
	virtual UINT getWindowExStyle() const {			return 0;										}
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-1-20	相对WUC::handleMessage多了bHandled参数，其用来判断用户是否接收了对消息处理
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	virtual void onFinalMessage(HWND hWnd);

public:
	void close();


	BEAnswerState getAnswerState() const;
	void setAnswerState(BEAnswerState state);

	LRESULT sendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L)
	{
		return SendMessage(uMsg, wParam, lParam);
	}
	LRESULT postMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L)
	{
		return PostMessage(uMsg, wParam, lParam);
	}
	bool modifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0)
	{
		return ModifyStyle(dwRemove, dwAdd, nFlags) == TRUE;
	}
	bool modifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0)
	{
		return ModifyStyleEx(dwRemove, dwAdd, nFlags) == TRUE;
	}
	bool isWindow() const throw()
	{
		return ::IsWindow(m_hWnd) == TRUE;
	}
	bool setWindowPos( HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) throw()
	{
		assert(::IsWindow(m_hWnd));
		return ::SetWindowPos(m_hWnd, hWndInsertAfter, x, y, cx, cy, nFlags) == TRUE;
	}

	bool setWindowPos( HWND hWndInsertAfter, LPCRECT lpRect, UINT nFlags) throw()
	{
		assert(::IsWindow(m_hWnd));
		return ::SetWindowPos(m_hWnd, hWndInsertAfter, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, nFlags) == TRUE;
	}
	bool getWindowRect(LPRECT lpRect) const throw()
	{
		assert(::IsWindow(m_hWnd));
		return ::GetWindowRect(m_hWnd, lpRect) == TRUE;
	}
	HWND setParent(HWND hWndNewParent) throw()
	{
		assert(::IsWindow(m_hWnd));
		return ::SetParent(m_hWnd, hWndNewParent);
	}
	bool isWindowEnabled() const throw()
	{
		assert(::IsWindow(m_hWnd));
		return ::IsWindowEnabled(m_hWnd) == TRUE;
	}
	bool isWindowVisible() const throw()
	{
		assert(::IsWindow(m_hWnd));
		return ::IsWindowVisible(m_hWnd) == TRUE;
	}
	bool updateWindow() throw()
	{
		assert(::IsWindow(m_hWnd));
		return ::UpdateWindow(m_hWnd) == TRUE;
	}
	bool showWindow(_In_ int nCmdShow) throw()
	{
		assert(::IsWindow(m_hWnd));
		return ::ShowWindow(m_hWnd, nCmdShow) == TRUE;
	}
protected:
	BEAnswerState	m_beAnswerState;	// 对话框的响应状态
};


}	// end wuc
#endif