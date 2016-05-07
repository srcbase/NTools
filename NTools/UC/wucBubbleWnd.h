#ifndef __WUCBUBBLEWND_H__
#define __WUCBUBBLEWND_H__
#include "wucdef.h"
#include "windowWnd.h"
#include "drawManager.h"
#include "wucBase.h"
namespace wuc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	bubbleWnd  类		2015-8-10
//	气泡式窗口基类
//	关闭方式：关闭按钮、失去焦点、倒计时三种，其中“关闭按钮”和“失去焦点”互斥
////////////////////////////////////////////////////////////////////////////////////////////////////////
class bubbleWnd : public windowWnd, public INotifyUC
{
public:
	enum {	BUBBLE_WND_CLOSE_TIMEID		};
public:
	bubbleWnd();
	~bubbleWnd();

	PCWSTR getWindowClassName() const {				return L"BubbleWnd";								}
	//virtual PCWSTR getWindowClassName() const = 0;
	//virtual PCWSTR getWindowCaption() const {		return NULL;										}
	virtual UINT getClassStyle() const {			return CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS;			}
	virtual UINT getWindowStyle() const {			return WS_POPUP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;	}
	virtual UINT getWindowExStyle() const {			return WS_EX_TOOLWINDOW|WS_EX_TOPMOST;				}

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void notify(wuc::BSNotifyUC &msg);
	virtual void onFinalMessage(HWND hWnd);

public:
	// 创建并初始化窗口
	void init(controlUC *pOwner, RECT rcPos, UINT timeElapse = 0);

public:
	virtual void onInitDialog();
	virtual void onClose();
	LRESULT onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
public:
	//buttonUC* getButtonClose() const;
	verticalLayoutUC* getRoot() const;
	void setExitAtLostfocus(bool bExit) {				m_bExitAtLostfocus = bExit;							}
	bool isExitAtLostfocus() const {					return m_bExitAtLostfocus;							}
protected:
	drawManager			m_drawManager;
	//IWindowUC			*m_pOwner;
	controlUC			*m_pOwner;			// 触发本窗口的控件，一般属于别的窗口
	UINT				m_uiTimeElapse;			// >0时启动倒计时退出
	bool				m_bExitAtLostfocus;		// 失去焦点时是否退出本窗口
	verticalLayoutUC	*m_pRoot;
	//buttonUC			*m_buttonClose;		// “关闭”按钮
};

}	// wuc

#endif	// __WUCBUBBLEWND_H__