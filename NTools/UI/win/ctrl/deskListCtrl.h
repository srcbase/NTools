#ifndef __DESKLISTCTRL_H__
#define __DESKLISTCTRL_H__
#pragma once
#pragma warning (disable : 4251) // 
#include "UI/win/winUI/winUI.h"
using namespace wui;
#include "KP/KPDeskInfo.h"
namespace gtc{

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	deskListCtrl类	2014-06-10
//	桌面信息列表控件
//	可对控件类的图标排序等操作
////////////////////////////////////////////////////////////////////////////////////////////////////////
class deskListCtrl : public wui::tileLayoutUI
{
public:
	enum { SCROLL_TIMERID = 10 };
	deskListCtrl() ;


	void doEvent(wui::TEventUI& event);

	void setPos(RECT rc);

	void redrawNodes();
	/*
	virtual void lineUp(){
	__super::lineUp();
	SEND_LOG(L"lineUp");
	redrawNodes();
	}
	virtual void lineDown(){
	__super::lineDown();
	SEND_LOG(L"lineDown");
	redrawNodes();
	}
	virtual void pageUp(){
	__super::pageUp();
	SEND_LOG(L"pageUp");
	redrawNodes();
	}
	virtual void pageDown(){
	__super::pageDown();
	SEND_LOG(L"pageDown");
	redrawNodes();
	}
	virtual void homeUp(){
	__super::homeUp();
	SEND_LOG(L"homeUp");
	}
	virtual void endDown(){
	__super::endDown();
	SEND_LOG(L"endDown");
	}
	virtual void lineLeft(){
	__super::lineLeft();
	SEND_LOG(L"lineLeft");
	}
	virtual void lineRight(){
	__super::lineRight();
	SEND_LOG(L"lineRight");
	}
	virtual void pageLeft(){
	__super::pageLeft();
	SEND_LOG(L"pageLeft");
	}
	virtual void pageRight(){
	__super::pageRight();
	SEND_LOG(L"");
	}
	virtual void homeLeft(){
	__super::homeLeft();
	SEND_LOG(L"homeLeft");
	}
	virtual void endRight() {
	__super::endRight();
	SEND_LOG(L"endRight");
	}
	*/
	virtual void setScrollPos(SIZE szPos)
	{
		static SIZE oldPos = {0};
		if (oldPos.cx == szPos.cx && oldPos.cy == szPos.cy)
		{
			return;
		}

		__super::setScrollPos(szPos);
		redrawNodes();
		SEND_LOG(L"setScrollPos");
		oldPos = szPos;
	}
	bool getNodePOS(const std::wstring &nodeName, RECT &rt, bool isScreen = false);
public:
	void setNodes(gtc::idlNode::IDINodeMap *pmapNode) {			m_pmapNodes = pmapNode;		}
	void setHWND(HWND hWnd) {			m_hWnd = hWnd;										}
private:
	UINT m_uButtonState;
	POINT m_ptLastMouse;
	LONG m_dwDelayDeltaY;
	DWORD m_dwDelayNum;
	DWORD m_dwDelayLeft;

	HWND	m_hWnd;
	gtc::idlNode::IDINodeMap *m_pmapNodes;
};

}

#endif