#pragma once
#include "wucdef.h"
#include "wucContainer.h"

namespace wuc {
//////////////////////////////////////////////////////////////////////////
// 边界标记
typedef enum BEUCBorderFlag {
	BEUCBorderFlagLeft		= 1,	
	BEUCBorderFlagTop		,		
	BEUCBorderFlagRight		,		
	BEUCBorderFlagBottom				
}BEUCBorderFlag;
//////////////////////////////////////////////////////////////////////////
// 自定义键盘操作接口
class IKeyEvent
{	// nFlag(shift ctrl alt等是否被按下
	virtual void onKeyDown(UINT nChar, UINT nFlag, UINT nRepCnt, bool& bHandled) = 0;
	virtual void onKeyUp(UINT nChar, UINT nFlag, UINT nRepCnt, bool& bHandled) = 0;
	virtual void onChar(UINT nChar, UINT nFlag, UINT nRepCnt, bool& bHandled) = 0;
};
//////////////////////////////////////////////////////////////////////////
// 自定义鼠标操作接口
class IMouseEvent
{
	virtual void onLButtonDown(UINT nFlags, POINT pt, bool& bHandled) = 0;
	virtual void onLButtonUp(UINT nFlags, POINT pt, bool& bHandled) = 0;
	virtual void onLButtonDBClick(UINT nFlags, POINT pt, bool& bHandled) = 0;
	virtual void onRButtonDown(UINT nFlags, POINT pt, bool& bHandled) = 0;
	virtual void onRButtonUp(UINT nFlags, POINT pt, bool& bHandled) = 0;
	virtual void onMouseMove(UINT nFlags, POINT pt, bool& bHandled) = 0;
	virtual void onMouseWheel(UINT nFlags, short zDelta, POINT pt, bool& bHandled) = 0;
	virtual void onMouseEntry(bool& bHandled) = 0;
	virtual void onMouseLeave(bool& bHandled) = 0;
	virtual void onMouseHover(UINT nFlags, POINT pt, bool& bHandled) = 0;
	virtual LRESULT onNcHitTest(POINT pt) = 0;

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////	windowTitleUC类	2015-1-22
////	windowUC窗口的标题区，内可放置最大、最小、关闭等按钮
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//class windowTitleUC : public horizontalLayoutUC
//{
//public:
//	windowTitleUC();
//	~windowTitleUC();
//	virtual PCWSTR getClass() const;
//	virtual LPVOID getInterface(PCWSTR pstrName);
//
//protected:
//	int getHeight() const;
//	void setHeight(int height);
//	PCWSTR getTitle() const;
//	void setTitle(std::wstring wsTitle);
//protected:
//	std::wstring	m_wsTitle;
//};
///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-22	窗口操作
enum BESelectMode {
	BESelectModeNone	= 0,	// 
	BESelectModeSize	,		// 边框此次改变
	BESelectModeMove			// 窗口移动
};
///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-23	当前操作
enum BEUCActiveFlag
{
	BEUCActiveFlagNone			= 0,	// 
	BEUCActiveFlagSize			,		// 改变窗口位置或尺寸
	//BEUCActiveFlagSysMenu		,		// 菜单
	BEUCActiveFlagEditTitle		,		// 重命名标题
	BEUCActiveFlagScroll		,		// 滚动条操作
	BEUCActiveFlagUserMenu		,		// 组窗口右键菜单
	BEUCActiveFlagDrapDrop				// 托拽图标
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	windowUC类	2015-1-22
//	模拟的窗口基类
//	窗口功能区域共有3部分：标题（可以隐藏），滚动条（水平、垂直，可以隐藏），客户区
//	注意：
//		1、滚动条来至窗体本身，但其属性由客户区设置
//		2、客户区的大小根据标题、滚动条的可见性进行布局，并可随客户区内而改变尺寸
////////////////////////////////////////////////////////////////////////////////////////////////////////
class windowUC : public verticalLayoutUC, public IMouseEvent
{
public:
public:
	windowUC(void);
	~windowUC(void);

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);

	void Event(BSEventUC &event);
	virtual void setPosition(RECT pos);
	virtual void paintStatusImage(HDC hdc);

public:
	// IMouseEvent
	virtual void onLButtonDown(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onLButtonUp(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onLButtonDBClick(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onRButtonDown(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onRButtonUp(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onMouseMove(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onMouseWheel(UINT nFlags, short zDelta, POINT pt, bool& bHandled);
	virtual void onMouseEntry(bool& bHandled);
	virtual void onMouseLeave(bool& bHandled);
	virtual void onMouseHover(UINT nFlags, POINT pt, bool& bHandled);
	virtual LRESULT onNcHitTest(POINT pt);
	// 
	/*****************************************************************************************
		<< --- windowUC::ptInSizeBox		2014-06-23 --- >>
		说明：查看光标是否在窗口边界的改变尺寸的拖动区，如果在相应改变光标状态
		参数：
		pt	= 被检测光标点
		返回值：得到测试点标记；如：HTNOWHERE、HTCLIENT...
	*****************************************************************************************/
	//virtual int hitTest(const POINT &pt);
	/*****************************************************************************************
		<< --- windowUC::moveTo		2014-06-24 --- >>
		说明：建窗体移动到新的位置
		参数：
		position	= 新的位置（屏幕坐标）
		返回值：
	*****************************************************************************************/
	virtual void moveTo(const RECT &position, bool mustMove = false);
	/*****************************************************************************************
		<< --- windowUC::moveHandleTo		2014-06-24 --- >>
		说明：通过边界改变窗口的尺寸
		参数：
		handle	= 边界位置
		pt		= 边界新的位置（屏幕坐标）
		返回值：
	*****************************************************************************************/
	virtual void moveHandleTo(int handle, const POINT &pt);

	/*****************************************************************************************
		<< --- windowUC::close		2014-07-17 --- >>
		说明：退出窗口，并释放资源
		参数：
		返回值：
	*****************************************************************************************/
	//virtual void close();
	/*****************************************************************************************
		<< --- windowUC::getCursor		2014-06-23 --- >>
		说明：得到测试点所在位置的光标
		参数：
		handle	= 测试点标记，一般由hitTest得到
		返回值：光标句柄
	*****************************************************************************************/
	virtual HCURSOR getCursor(int handle);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//	2015-1-26	包含非客户区的测试
	bool ptIn(const POINT &pt, bool inclusionNc);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-19 坐标点(屏幕坐标)是否落在客户区
	bool ptInClient(POINT pt);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-19 坐标点(屏幕坐标)是否落在标题区
	bool ptInCaption(POINT pt);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-3-19 	
	void clientToScreen(RECT &rc);
	void clientToScreen(POINT &pt);
	void screenToClient(RECT &rc);
	void screenToClient(POINT &pt);
	// 包含移动区的窗口位置
	RECT getWindowNcPos() const;
public:
	horizontalLayoutUC* getTitleBar() const;
	horizontalLayoutUC* getClientUC() const;
	horizontalLayoutUC* getTaskBar() const;
	virtual void setLockPos(bool lock);
	bool isLockPos() const;
	BESelectMode getSelectMode() const;
	bool isActivated() const;
	void setActivated(bool activate);
	bool isSelected() const;
	void selected(bool bSelected = true);
	BEUCActiveFlag getActiveFlag() const;
	void clearActiveFlag();
	bool isInitial() const;
	int getOperateHandle() const;
	PCWSTR getTitle() const;
	void setTitle(const std::wstring &wsTitle);
	int getTitlebarHeight() const;
	void setTitlebarHeight(int height);
	RECT getTitlebarPos() const;
	int getTaskbarHeight() const;
	void setTaskbarHeight(int height);
	RECT getTaskbarPos() const;
protected:
	horizontalLayoutUC	*m_pTitleBar;		// 标题栏
	horizontalLayoutUC	*m_pClientUC;		// 客户区
	horizontalLayoutUC	*m_pTaskBar;		// 任务栏或状态栏

	BEUCActiveFlag	m_beActive;			// 当前操作标记
	int				m_iOperateHandle;	// 当前操作区域点标记
	POINT			m_lastMousePos;		// 鼠标当前坐标
	RECT			m_rcSizeBox;		// 窗口尺寸可以改变，哪一边不能拖动就设置为0
	bool			m_bInitial;			// 处于没有操作前的状态，当鼠标entry后就false
	bool			m_bActivated;		// 当前是否处于活动状态；鼠标进入和离开时改变该值
	bool			m_bSelected;		// 当前窗口是否被选中；如：鼠标点击该窗体
	bool			m_bLockPos;			// 锁定的窗口不能移位也不能改变大小
	BESelectMode	m_beSelectMode;		// 当前的操作模式
	std::wstring	m_wsTitle;
};

}
