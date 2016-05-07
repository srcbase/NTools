#pragma once
#include "ucdef.h"

namespace wuc{

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	drawManager类	2014-07-30
//	窗口的绘图管理类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class drawManager
{
public:
	drawManager(void);
	~drawManager(void);


protected:
	static HINSTANCE m_hInstance;	// 内存的资源句柄实例
	HWND		m_hWndPaint;
	HWND		m_hwndTooltip;
	TOOLINFO	m_ToolTip;

	//controlUC*	m_pRoot;
	//controlUC*	m_pFocus;
	//controlUC*	m_pEventHover;
	//controlUC*	m_pEventClick;
	//controlUC*	m_pEventKey;

};

}