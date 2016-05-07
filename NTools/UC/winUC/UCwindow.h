#pragma once
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
	HWND create(HWND hwndParent, PCWSTR pstrName, const RECT rc);
	HWND create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
	HWND create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
	//HWND create(HWND hwndParent, WNDPROC fnWndProc, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
	HWND subclass(HWND hWnd);
	void unsubclass();
	void setIcon(UINT nRes);

	void showWindow(bool bShow = true, bool bTakeFocus = true);
	bool showModal();
	void close();
	void centerWindow();
	bool isVisible() const;
	bool isEnabled() const;
	void setEnabled(bool enabled = true);

	virtual RECT getWindowPos() const;
	virtual void setWindowPos(RECT rt);

public:
	virtual LRESULT sendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	virtual LRESULT postMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
protected:
	virtual PCWSTR getWindowClassName() const = 0;
	virtual PCWSTR getSuperClassName() const {		return nullptr;													}
	virtual UINT getClassStyle() const {			return 0;														}
	virtual UINT getWindowStyle() const {			return 0;														}
	virtual UINT getWindowExStyle() const {			return 0;														}

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void onFinalMessage(HWND hWnd);

	static LRESULT CALLBACK __wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK __controlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	HWND			m_hWnd;				// 窗口句柄

	WNDPROC			m_OldWndProc;
	bool			m_bSubclassed;

};

}