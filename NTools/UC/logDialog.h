#ifndef __LOGDIALOG_H__
#define __LOGDIALOG_H__
#pragma once
#include "AC/singleton.h"
#include "UC/wucdef.h"
#include "UC/windowWnd.h"
#include "UC/drawManager.h"

namespace wuc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	logDialog  类    2015-2-7
//	调试信息的输出窗口
//	如果想要启动本功能，请定义_WUC_LOG_DIALOG__宏
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class logDialog : public windowWnd, public wuc::INotifyUC, public gtc::singleton<logDialog>
{
public:
	logDialog(void);
	~logDialog(void);

	virtual PCWSTR getWindowClassName() const {		return L"LOGDialogClass";							}
	virtual UINT getClassStyle() const {			return UC_CLASSSTYLE_DIALOG;						}
	virtual UINT getWindowStyle() const {			return UC_WNDSTYLE_DIALOG;							}
	virtual UINT getWindowExStyle() const {			return WS_EX_TOOLWINDOW|WS_EX_TOPMOST;				}
public:
	// gtc::singleton
	static logDialog& getSingleton();
	static logDialog* getSingletonPtr();
protected:
	virtual void notify(wuc::BSNotifyUC& msg);
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void onFinalMessage(HWND hWnd);

	LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
public:
	//////////////////////////////////////////////////////////////////////////
	//	2015-2-8	
	//bool create();
protected:
	wuc::drawManager m_drawManager;
	bool m_bHold;		// 本窗口是否不随调用者一起退出
};



}	// end wuc
#endif	// end __LOGDIALOG_H__