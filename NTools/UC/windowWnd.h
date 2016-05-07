#pragma once
#include "wucdef.h"

//
namespace wuc{


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	windowWnd类	2014-06-21
//	消息窗口的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class windowWnd
{
public:
	windowWnd(void);
	~windowWnd(void);
	HWND getHWND() const;
	operator HWND() const;

	bool registerWindowClass();
	bool registerSuperclass();
	HWND create(HWND hwndParent, const RECT &rc, PCWSTR pstrName = NULL);
	//HWND create(HWND hwndParent, PCWSTR pstrName, const RECT &rc);
	HWND create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT &rc, HMENU hMenu = NULL);
	HWND create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
	//HWND create(HWND hwndParent, WNDPROC fnWndProc, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
	HWND subclass(HWND hWnd);
	void unsubclass();
	void setIcon(UINT nRes);

	virtual void showWindow(bool bShow = true, bool bTakeFocus = true);	// 采用virtual修饰，便于WS_EX_LAYERED窗口控制控件
	BEAnswerState showModal(const RECT *prcParent = NULL);
	void close(BEAnswerState answerState = BEAnswerStateOK);
	void centerWindow(const RECT *prcParent = NULL);
	bool isVisible() const;
	bool isEnabled() const;
	void setEnabled(bool enabled = true);

	virtual RECT getWindowPos() const;
	virtual void setWindowPos(RECT rt);

	void setCapture();
	void releaseCapture();

	bool modifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);
	bool modifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);
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
	//BEAnswerState getAnswerState() const;
	//void setAnswerState(BEAnswerState state);
public:
	virtual LRESULT sendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	virtual LRESULT postMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
public:
	static void messageLoop();
protected:
	virtual PCWSTR getWindowClassName() const = 0;
	virtual UINT getClassStyle() const {			return 0;										}
	virtual PCWSTR getSuperClassName() const {		return nullptr;									}
	virtual UINT getWindowStyle() const {			return 0;										}
	virtual UINT getWindowExStyle() const {			return 0;										}

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void onFinalMessage(HWND hWnd);

	static LRESULT CALLBACK __wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK __controlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	// 调试用
	static bool isOutMsg(UINT uMsg);
	static std::wstring stringMsg(UINT uMsg);
protected:
	HWND			m_hWnd;				// 窗口句柄
	WNDPROC			m_OldWndProc;
	bool			m_bSubclassed;

	//BEAnswerState	m_beAnswerState;	// 对话框的响应状态
};

}