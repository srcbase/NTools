#ifndef __WUCWND_H__
#define __WUCWND_H__
#pragma once
#include "windowWnd.h"
#include "wucBase.h"

namespace wuc {
class wucWnd;
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-3-20 总的接口，给控件拥有者的窗体（或控件）控件实现。一般由wucWnd类触发
class IWndXCallback
{
public:
	// IWndXCallback 接口
	// 当窗口收到 WM_SHOWWINDOW，且wParam=TRUE 消息后触发。可以给控件提供一些初始数据等
	virtual void onxInit(wuc::wucWnd *pWnd) {}
	// 当窗口受到 WM_SETFOCUS 时触发。可以设定光标位置，选中文本等
	virtual void onxSetFocus(wuc::wucWnd *pWnd) {}
	// 当窗口受到 WM_KILLFOCUS 时触发。可以得到控件数据，或阻止退出(bExit = false)
	virtual void onxKillFocus(wuc::wucWnd *pWnd, bool &bExit /* 内部初始为true */) {}
	// 当窗口受到 WM_DESTROY 时触发。可以得到控件数据，或阻止退出(bExit = false)
	virtual void onxExit(wuc::wucWnd *pWnd, bool &bExit /* 内部初始为true */) {}
	// 当窗口受到 WM_NCDESTROY 时触发。
	virtual void onxFinal(wuc::wucWnd *pWnd) {}

	//// IWndXCallback 接口
	//virtual void onxInit(wuc::wucWnd *pWnd);
	//virtual void onxSetFocus(wuc::wucWnd *pWnd);
	//virtual void onxKillFocus(wuc::wucWnd *pWnd, bool &bExit /* 内部初始为true */);
	//virtual void onxExit(wuc::wucWnd *pWnd, bool &bExit /* 内部初始为true */);
	//virtual void onxFinal(wuc::wucWnd *pWnd, HWND hWnd);

};
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-3-20 返回给调用者的接口
class IEditWndXCallback : public IWndXCallback
{
public:
	// IEditWndXCallback接口
	// 当受到EN_CHANGE消息时触发
	virtual void onxEditChanged(wuc::wucWnd *pWnd) {}
	// 当接收到VK_RETURN键时触发
	virtual void onxEditReturn(wuc::wucWnd *pWnd, bool &bExit /* 内部初始为true */) { }
	// 当接收到WM_CHAR键时触发，可以过滤某些无效字符
	virtual void onxChar(wuc::wucWnd *pWnd, UINT nChar, UINT nFlag, UINT nRepCnt, bool& bHandled) {}

	//// IEditWndXCallback:IWndXCallback 接口
	//virtual void onxEditChanged(wuc::wucWnd *pWnd);
	//virtual void onxEditReturn(wuc::wucWnd *pWnd, bool &bExit /* 内部初始为true */);
	//virtual void onxChar(UINT nChar, UINT nFlag, UINT nRepCnt, bool& bHandled);
};

//////////////////////////////////////////////////////////////////////////////////////////
//	2015-3-26 所有子类化控件公共接口	
class IWndX
{
public:
	// IWndX 接口
	// 子类化控件的实现者
	virtual IWndXCallback* getCallback() const = 0;
	virtual void setCallback(IWndXCallback *pCallback) = 0;
	// 得到子类化控件（即：本接口的实现者）
	virtual controlUC* getSelf() = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	wucWnd  类		2015-3-20
//	对立于任何wuc控件的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class wucWnd : public windowWnd
{
public:
	wucWnd();
	~wucWnd();
protected:
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void onFinalMessage(HWND hWnd);
//public:
//	virtual LRESULT sendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0L*/) {
//		return sendMessage(uMsg, wParam, lParam);
//	}
//	virtual LRESULT postMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0L*/) {
//		return postMessage(uMsg, wParam, lParam);
//	}
//
//public:
//	HWND getHWND() const {					return __super::getHWND();						}
//	operator HWND() const {					return __super::operator HWND();				}
//	bool isVisible() const {				return __super::isVisible();					}
//	bool isEnabled() const {				return __super::isEnabled();					}
//	void setEnabled(bool enabled = true) {	__super::setEnabled(enabled);					}

public:
	bool modifyStyle(DWORD dwRemove, DWORD dwAdd);
	bool create(IWndX *pOwner, HWND hParent, RECT rcPos, DWORD dwStyle, DWORD dwExStyle = 0);
	IWndX* getOwner() const;

protected:
	IWndX	*m_pOwner;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	editUC类		2015-2-11
//	普通文本编辑控件
////////////////////////////////////////////////////////////////////////////////////////////////////////
class editWnd;

class editUC : public labelUC, public IWndX
{
	friend editWnd;
public:
	editUC();
	~editUC();
	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual UINT getControlFlags() const;

	virtual void setEnabled(bool bEnable = true);
	virtual void setText(PCWSTR pstrText);

	virtual void setPosition(RECT pos);
	virtual void setVisible(bool bVisible = true);
	virtual void setInternVisible(bool bVisible = true);

	virtual SIZE estimateSize(SIZE szAvailable);
	virtual void Event(BSEventUC &event);

	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);

	virtual void paintStatusImage(HDC hdc);
	virtual void paintText(HDC hdc);
public:
	// IWndX 接口
	// 子类化控件的实现者
	virtual IWndXCallback* getCallback() const;
	virtual void setCallback(IWndXCallback *pCallback);
	// 得到子类化控件（即：本接口的实现者）
	virtual controlUC* getSelf();
public:
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-4 选中指定范围内的字符
	void setSelRange(int iStart, int iEnd);
public:
	//void setOwner(IEditWndXCallback *pOwner);
	//IEditWndXCallback* getOwner() const;
	void setMaxChar(UINT uMax);
	UINT getMaxChar();
	void setReadOnly(bool bReadOnly);
	bool isReadOnly() const;
	void setPasswordMode(bool bPasswordMode);
	bool isPasswordMode() const;
	void setPasswordChar(wchar_t cPasswordChar);
	wchar_t getPasswordChar() const;
	void setMultiLine(bool bMultiLine);
	bool isMultiLine() const;
	void setBorder(bool bBorder);
	bool supportBorder() const;

	bool getNormalImage(RECT &rc);
	void setNormalImage(RECT rcImgCell);
	bool getHotImage(RECT &rc);
	void setHotImage(RECT rcImgCell);
	bool getFocusedImage(RECT &rc);
	void setFocusedImage(RECT rcImgCell);
	bool getDisabledImage(RECT &rc);
	void setDisabledImage(RECT rcImgCell);

	void setNativeEditBkColor(DWORD dwBkColor);
	DWORD getNativeEditBkColor() const;

	void setPromptInfo(PCWSTR pPrompt);
	PCWSTR getPromptInfo() const;
	void setPromptTextColor(DWORD dwColor);
	DWORD getPromptTextColor() const;
protected:
	editWnd*			m_pWindow;
	IEditWndXCallback	*m_pOwner;

	UINT			m_uMaxChar;
	bool			m_bBorder;
	bool			m_bReadOnly;
	bool			m_bPasswordMode;
	bool			m_bMultiLine;
	wchar_t			m_cPasswordChar;
	UINT			m_uButtonState;
	DWORD			m_dwEditbkColor;

	std::wstring	m_wsPromptInfo;			// 提示用户如何输入，如：请输入...
	DWORD			m_dwPromptTextColor;	// 提示文件显示色
};


}	// end wuc
#endif	// end __WUCWND_H__
