#ifndef __DIALOGWND_H__
#define __DIALOGWND_H__
#pragma once
#include "UC/windowWnd.h"
#include "UC/drawManager.h"
#include "UC/wucBase.h"
namespace wuc 
{
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	dialogWnd  类		2015-5-25
//	对话框封装类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class dialogWnd : public windowWnd, public INotifyUC
{
public:
	dialogWnd();
	~dialogWnd();

	virtual PCWSTR getWindowClassName() const {		return L"UCDialogClass";						}
	virtual UINT getClassStyle() const {			return UC_CLASSSTYLE_DIALOG;					}
	virtual UINT getWindowStyle() const {			return WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS;	}
	virtual UINT getWindowExStyle() const {			return WS_EX_TOOLWINDOW;						}

	virtual void notify(wuc::BSNotifyUC& msg);
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void onFinalMessage(HWND hWnd);

public:
	// 加载WUC对象及资源
	virtual void onInitDialog();
	LRESULT onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
public:
	BEAnswerState showModal(HWND hWndParent /*= ::GetActiveWindow()*/, const RECT *prcParent = NULL);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-5-11 
	verticalLayoutUC* getRoot();
	//////////////////////////////////////////////////////////////////////////
	//	2015-5-10	设定对话框大小
	void setWindowSize(SIZE sz);
	void setWindowSize(int cx, int cy);
	//////////////////////////////////////////////////////////////////////////
	//	2015-5-10	设定窗口是否支持透明
	void setBackgroundTransparent(bool bTrans = true);
	void setLayerWindowTransparent(bool bTrans = true);
	//////////////////////////////////////////////////////////////////////////
	//	2015-5-10	设定标题区
	void setCaption(RECT rcCaption);
	void setCaption(int height);
	//////////////////////////////////////////////////////////////////////////
	//	2015-5-10	
	void setBackgroupImage(PCWSTR pbkImage);
	//////////////////////////////////////////////////////////////////////////
	//	2015-5-10	添加控件
	void addControl(controlUC *pControl);
	//////////////////////////////////////////////////////////////////////////
	//	2015-5-10	按控件名称找控件
	controlUC* findControl(PCWSTR pName) const;
protected:
	wuc::drawManager m_drawManager;

};


}	// end wuc
#endif	// end __DIALOGWND_H__