#ifndef __KPDESKGROUPWND_H__
#define __KPDESKGROUPWND_H__
#pragma once

#include "UI/win/ctrl/deskListCtrl.h"

class dialogBuilderCallbackEx : public IDialogBuilderCallback
{

public:
	controlUI* createControl(PCWSTR pstrClass) 
	{
		if( _tcscmp(pstrClass, L"deskListCtrl") == 0 ) return new gtc::deskListCtrl();
		return NULL;
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	kpdeskWnd类	2014-06-04
//	桌面信息类
//	前期用来作为与用户交互用，后期可能用来做为墙纸
////////////////////////////////////////////////////////////////////////////////////////////////////////
class kpdeskGroupWnd  : public wui::windowWnd, public wui::INotifyUI
{
public:
	kpdeskGroupWnd(void);
	~kpdeskGroupWnd(void);
public:
	// wui
	virtual PCWSTR getWindowClassName() const { return L"WUIDeskGroupWnd";							}
	virtual UINT getClassStyle() const {
		return CS_VREDRAW | CS_HREDRAW;
		return CS_DBLCLKS;
	}
	UINT getWindowStyle() const {				
		return WS_POPUP;
		return WS_THICKFRAME|WS_CLIPSIBLINGS;
	}
	UINT getWindowExStyle() const {				
		return WS_EX_LAYERED|WS_EX_NOACTIVATE|WS_EX_TRANSPARENT;
		return WS_EX_WINDOWEDGE|WS_EX_CONTROLPARENT|WS_EX_TOOLWINDOW;
	}
	virtual void onFinalMessage(HWND hWnd);
	virtual void notify(wui::TNotifyUI& msg);
	virtual LRESULT onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT onSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool textTitleNotify(void *param)
	{
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if (pMsg)
		{
			SEND_LOG(L"txt:%s", pMsg->sType.c_str());
		}
		else
			SEND_LOG(L"txt");
		return true;
	}
public:

	/*****************************************************************************************
		<< ---  kpdeskGroupWnd::setWindowPos		2014-06-04 --- >>
		说明：设定本窗口相对于桌面区域索引位置；桌面预先用行列划分一些区域
		参数：
		idx		= 桌面区域索引，索引基于0
		返回值：
	*****************************************************************************************/
	void setWindowPos(int32_t idx);

	/*****************************************************************************************
		<< ---  kpdeskGroupWnd::setTitle		2014-06-05 --- >>
		说明：设置窗口标题
		参数：
		ptitle	= 标题内容
		返回值：
	*****************************************************************************************/
	void setTitle(PCWSTR ptitle);

	/*****************************************************************************************
		<< ---  kpdeskGroupWnd::allocNodes		2014-06-09 --- >>
		说明：根据m_mapDeskNodes节点数量，在deskListCtrl中分配布局空间
		参数：
		返回值：
	*****************************************************************************************/
	void allocNodes();
	/*****************************************************************************************
		<< ---  kpdeskGroupWnd::redrawNodes		2014-06-09 --- >>
		说明：重新绘制节点信息
		参数：
		返回值：
	*****************************************************************************************/
	void redrawNodes();
	void redrawNode(const std::wstring &nodeName, HDC sourceDC);
	/*****************************************************************************************
		<< ---  kpdeskGroupWnd::getContainScreenPOS		2014-06-14 --- >>
		说明：得到容器的屏幕坐标
		参数：
		返回值：
	*****************************************************************************************/
	RECT getContainScreenPOS();
	bool getNodePos(const std::wstring &nodeName, RECT &rt);
public:
	std::wstring getGroupGuid() const {			return m_wsGuid;						}
	gtc::idlNode::IDINodeMap& getNodes() {		return m_mapDeskNodes;					}
	HDC getPaintDC() const {					return m_paintManager.getPaintDC();		}
	void setPaintDC(HDC hdc) {					
		m_paintManager.setPaintDC(hdc);
		m_paintManager.needUpdate();
		sendMessage(WM_PAINT);
	}
protected:
	wui::paintManagerUI m_paintManager;

	bool m_bWinInit;
	std::wstring	m_wsGuid;	// 用来区分不同的窗口
	gtc::idlNode::IDINodeMap	m_mapDeskNodes;	// 分组窗口内的图标，无需释放，同一由kpdeskAdmin释放
};


#endif
