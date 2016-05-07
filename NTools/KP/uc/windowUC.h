#pragma once
#include "ucdef.h"

//
namespace gtc{
////////////////////////////////////////////////////////////////////////////////////////////////////////
class controlUC;
//////////////////////////////////////////////////////////////////////////
// 自定义键盘操作接口
class IKeyEvent
{	// nFlag(shift ctrl alt等是否被按下
	virtual void onKeyDown(UINT nChar, UINT nFlag, UINT nRepCnt) = 0;
	virtual void onKeyUp(UINT nChar, UINT nFlag, UINT nRepCnt) = 0;
	virtual void onChar(UINT nChar, UINT nFlag, UINT nRepCnt) = 0;
};
//////////////////////////////////////////////////////////////////////////
// 自定义鼠标操作接口
class IMouseEvent
{
	virtual void onLButtonDown(UINT nFlags, POINT pt, bool& bHandled) = 0;
	virtual void onLButtonUp(UINT nFlags, POINT pt, bool& bHandled) = 0;
	virtual void onMouseMove(UINT nFlags, POINT pt, bool& bHandled) = 0;
	virtual void onMouseWheel(UINT nFlags, short zDelta, POINT pt, bool& bHandled) = 0;
	virtual void onMouseEntry(bool& bHandled) = 0;
	virtual void onMouseLeave(bool& bHandled) = 0;
	virtual void onMouseHover(UINT nFlags, POINT pt, bool& bHandled) = 0;
	virtual LRESULT onNcHitTest(POINT pt, bool& bHandled) = 0;

};
//////////////////////////////////////////////////////////////////////////
// 自定义托拽接口
class IDrapDrop
{
public:
	//virtual void onDragEnter(HGLOBAL hgdata, DWORD keyState, POINT pt, bool &bHandled);
	//virtual void onDragOver(DWORD keyState, POINT pt, bool &bHandled);
	//virtual void onDragLeave(bool &bHandled);
	//virtual void onDrop(HGLOBAL hgdata, DWORD keyState, POINT pt, bool &bHandled);
	/*****************************************************************************************
		<< --- virtual IDrapDrop::dragEnter		2014-07-05 --- >>
		说明：准备开始托拽，当鼠标进入目标窗口时触发
		参数：
		hgdata		= 被托拽的数据
		keyState	= 按键状态MK_CONTROL、MK_SHIFT、MK_ALT、MK_BUTTON、MK_LBUTTON
		pt			= 当前鼠标的屏幕坐标
		bHandled	= 是否继续执行托拽操作
		返回值：
	*****************************************************************************************/
	virtual void onDragEnter(HGLOBAL hgdata, DWORD keyState, POINT pt, bool &bHandled) = 0;
	/*****************************************************************************************
	virtual void onDrop(HGLOBAL hgdata, DWORD keyState, POINT pt, bool &bHandled);
		<< --- virtual IDrapDrop::dragOver		2014-07-05 --- >>
		说明：发生在dragEnter之后，即：鼠标在托拽中移动，可以寻找目标点
		参数：
		keyState	= 按键状态MK_CONTROL、MK_SHIFT、MK_ALT、MK_BUTTON、MK_LBUTTON
		pt			= 当前鼠标的屏幕坐标
		bHandled	= 是否继续执行托拽操作
		返回值：	*****************************************************************************************/
	virtual void onDragOver(DWORD keyState, POINT pt, bool &bHandled) = 0;
	/*****************************************************************************************
		<< --- virtual IDrapDrop::dragLeave		2014-07-05 --- >>
		说明：鼠标离开托拽窗口时触发，可以处理恢复窗口状态等操作
		参数：
		bHandled	= 是否执行成功
		返回值：
	virtual void onDrop(HGLOBAL hgdata, DWORD keyState, POINT pt, bool &bHandled);
	*****************************************************************************************/
	virtual void onDragLeave(bool &bHandled) = 0;
	/*****************************************************************************************
		<< --- virtual IDrapDrop::drop		2014-07-05 --- >>
		说明：托拽操作的收尾工作，如将托拽数据放入目标窗口
		参数：
		hgdata		= 被托拽的数据
		keyState	= 按键状态MK_CONTROL、MK_SHIFT、MK_ALT、MK_BUTTON、MK_LBUTTON
		pt			= 当前鼠标的屏幕坐标
		bHandled	= 是否成功执行托拽操作
		返回值：
	*****************************************************************************************/
	virtual void onDrop(HGLOBAL hgdata, DWORD keyState, POINT pt, bool &bHandled) = 0;
};
//////////////////////////////////////////////////////////////////////////
// 当前操作f
typedef enum BEUCActiveFlag
{
	BEUCActiveFlagNone			= 0,	// 
	BEUCActiveFlagSize			,		// 改变窗口位置或尺寸
	BEUCActiveFlagSysMenu		,		// 菜单
	BEUCActiveFlagEditTitle		,		// 重命名标题
	BEUCActiveFlagScroll		,		// 滚动条操作
	BEUCActiveFlagUserMenu		,		// 组窗口右键菜单
	BEUCActiveFlagDrapDrop				// 托拽图标
}BEUCActiveFlag;

static std::wstring toString(BEUCActiveFlag flag)
{
	switch(flag)
	{
	case BEUCActiveFlagNone:
		return L"ActiveNone";
		break;
	case BEUCActiveFlagSize:
		return L"ActiveSize";
		break;
	case BEUCActiveFlagSysMenu:
		return L"ActiveSysMenu";
		break;
	case BEUCActiveFlagEditTitle:
		return L"ActiveEditTitle";
		break;
	}
	return L"";
}
//////////////////////////////////////////////////////////////////////////
// 边界标记
typedef enum BEUCBorderFlag {
	BEUCBorderFlagLeft		= 1,	
	BEUCBorderFlagTop		,		
	BEUCBorderFlagRight		,		
	BEUCBorderFlagBottom				
}BEUCBorderFlag;
/////////////////////////////////////////////////////////////////////////////////////
// UC控件事件
typedef enum BEUCEvent
{
	BEUCEventFirst = 1,
	BEUCEventKeyBegin,
	BEUCEventKeyDown,
	BEUCEventKeyUp,
	BEUCEventChar,
	BEUCEventSysKey,
	BEUCEventKeyEnd,
	BEUCEventMouseBegin,
	BEUCEventMouseMove,
	BEUCEventMouseLeave,
	BEUCEventMouseEnter,
	BEUCEventMouseHover,
	BEUCEventButtonDown,
	BEUCEventButtonUp,
	BEUCEventRButtonDown,
	BEUCEventDBLClick,
	BEUCEventContextMenu,
	BEUCEventScrollWheel,
	BEUCEventMouseEnd,
	BEUCEventKillFocus,
	BEUCEventSetFocus,
	BEUCEventWindowZize,
	BEUCEventSetCursor,
	BEUCEventTimer,
	BEUCEventNotify,
	BEUCEventCommand,
	BEUCEventLast,
}BEUCEvent;

// Flags used for controlling the paint
#define UCSTATE_FOCUSED      0x00000001
#define UCSTATE_SELECTED     0x00000002
#define UCSTATE_DISABLED     0x00000004
#define UCSTATE_HOT          0x00000008
#define UCSTATE_PUSHED       0x00000010
#define UCSTATE_READONLY     0x00000020
#define UCSTATE_CAPTURED     0x00000040


// Structure for notifications from the system
// to the control implementation.
typedef struct BSEventUC
{
	int Type;
	controlUC* pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	wchar_t chKey;
	WORD wKeyState;
	WPARAM wParam;
	LPARAM lParam;
} BSEventUC;

// Structure for notifications to the outside world
typedef struct BSNotifyUC
{
	std::wstring wsType;
	controlUC* pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	WPARAM wParam;
	LPARAM lParam;
} BSNotifyUC;

// Listener interface
class INotifyUC
{
public:
	virtual void notify(BSNotifyUC& msg) = 0;	// 接收来者控件的通知
};

/////////////////////////////////////////////////
typedef struct BSFontInfoUC
{
	HFONT hFont;
	std::wstring sFontName;
	int iSize;
	bool bBold;
	bool bUnderline;
	bool bItalic;
	TEXTMETRIC tm;
} BSFontInfoUC;

typedef struct BSTimerInfo
{
	controlUC*	pSender;
	UINT		nLocalID;
	HWND		hWnd;
	UINT		uWinTimer;
	bool		bKilled;
}BSTimerInfo;

//// IMessageUC
//class IMessageUC
//{
//public:
//	virtual void onLButtonDown(UINT nFlags, POINT pt, bool& bHandled);
//	virtual void onLButtonUp(UINT nFlags, POINT pt, bool& bHandled);
//	virtual void onMouseMove(UINT nFlags, POINT pt, bool& bHandled);
//	virtual void onMouseWheel(UINT nFlags, short zDelta, POINT pt, bool& bHandled);
//};
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	windowUC类	2014-06-21
//	自绘窗口的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
class windowUC : public IDrapDrop, public IMouseEvent, public IKeyEvent
{
public:
	enum BESelectMode {
		BESelectModeNone	= 0,	// 
		BESelectModeSize	,		// 边框此次改变
		BESelectModeMove			// 窗口移动
	};
public:
	windowUC();
	virtual ~windowUC();

	virtual void draw(HDC hdc);
public:
	// 屏幕坐标
	RECT getWindowPos() const;
	// 屏幕坐标
	void setWindowPos(RECT rt);
	// 包含移动区的窗口位置
	RECT getWindowNcPos() const;

	// 不包括标题区
	RECT getClientRect() const;
	RECT getClientTitle() const;
	void clientToScreen(RECT *prect);
	void clientToScreen(POINT *ppt);
	void screenToClient(RECT *prect);
	void screenToClient(POINT *ppt);
	// 查看点是否在本窗口内
	bool ptIn(const POINT &pt, bool inclusionNc = false);
	/*****************************************************************************************
		<< --- windowUC::invalidate	2014-06-23 --- >>
		说明：自绘窗口依赖的宿主窗口
		参数：
		rcItem		= 需要重画区域
		isClient	= true 客户坐标，false屏幕坐标
		返回值：
	*****************************************************************************************/
	void invalidate(RECT& rcItem, bool isClient = true);
	void invalidate(controlUC *pUC);
	void invalidate();
	void updateWindow();

	void needUpdate();

	bool activate() const;
	void setActivate(bool bActive = true);
	// 窗体中的每个控件都需在基窗口中做个注册
	void addControl(controlUC *pControl);
	// 删除控件在窗口的注册；注意：不需要释放内存
	void removeControl(controlUC *pControl);
	// 
	/*****************************************************************************************
		<< --- windowUC::initControl	2014-06-23 --- >>
		说明：初始化控件，并建立相应关系
		参数：
		pControl	= 被初始化的控件
		pParent		= 所属父控件
		返回值：
	*****************************************************************************************/
	bool initControl(controlUC *pControl, controlUC *pParent = nullptr);
	// 找到光标点（屏幕坐标）所在的控件
	controlUC* findControl(const POINT &pt, bool bLast = true);
	// 根据控件名称找控件
	controlUC* findControl(const std::wstring &wsName);
	/*****************************************************************************************
		<< --- windowUC::getCursor		2014-06-23 --- >>
		说明：得到测试点所在位置的光标
		参数：
		handle	= 测试点标记，一般由hitTest得到
		返回值：光标句柄
	*****************************************************************************************/
	virtual HCURSOR getCursor(int handle);

public:
	virtual void onKeyDown(UINT nChar, UINT nFlag, UINT nRepCnt);
	virtual void onKeyUp(UINT nChar, UINT nFlag, UINT nRepCnt);
	virtual void onChar(UINT nChar, UINT nFlag, UINT nRepCnt);

public:
	virtual void onLButtonDblClk(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onLButtonDown(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onLButtonUp(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onRButtonDown(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onRButtonUp(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onMouseMove(UINT nFlags, POINT pt, bool& bHandled);
	virtual void onMouseWheel(UINT nFlags, short zDelta, POINT pt, bool& bHandled);
	virtual void onMouseEntry(bool& bHandled);
	virtual void onMouseLeave(bool& bHandled);
	virtual void onMouseHover(UINT nFlags, POINT pt, bool& bHandled);
	virtual LRESULT onNcHitTest(POINT pt, bool& bHandled);
public:
	bool addNotifier(INotifyUC* pControl);
	bool removeNotifier(INotifyUC* pControl);   
	void sendNotify(BSNotifyUC& Msg, bool bAsync = false);
	void sendNotify(controlUC* pControl, PCWSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

public:
	bool setTimer(controlUC* pControl, UINT nTimerID, UINT uElapse);
	bool killTimer(controlUC* pControl, UINT nTimerID);
	void killTimer(controlUC* pControl);
	void removeAllTimers();

protected:
	/*****************************************************************************************
		<< --- windowUC::ptInSizeBox		2014-06-23 --- >>
		说明：查看光标是否在窗口边界的改变尺寸的拖动区，如果在相应改变光标状态
		参数：
		pt	= 被检测光标点
		返回值：得到测试点标记
	*****************************************************************************************/
	virtual int hitTest(const POINT &pt);


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
	virtual void close();
public:
	/*****************************************************************************************
		<< --- static windowUC::init		2014-06-23 --- >>
		说明：自绘窗口依赖的宿主窗口
		参数：
		hMain	= 宿主窗口
		返回值：
	*****************************************************************************************/
	static void init(HWND hMain);

	// 时钟，用来处理滚动条的托拽等操作
	static VOID CALLBACK doTimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );
public:
	void setTitle(PCWSTR ptitle);
	void setTitle(std::wstring wstitle);
	PCWSTR getTitle() const {						return m_wsTitle.c_str();				}
	void setTitleHeight(int height) {				m_iTitleHeight = height;				}
	int getTitleHeight() const {					return m_iTitleHeight;					}
	BESelectMode getSelectMode() const {			return m_beSelectMode;					}
	bool isVisible() const;
	bool isEnabled() const;
	void showWindow(bool show = true);
	void setEnabled(bool enabled = true);
	BSFontInfoUC* getDefaultFont();
	void setDefaultFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	bool isLockPos() const {						return m_bLockPos;						}
	void lockPos() {								m_bLockPos = true;						}
	void unlockPos() {								m_bLockPos = false;						}
	BEUCActiveFlag getActiveFlag() const {			return m_beActive;						}
	void clearActiveFlag() {						m_beActive = BEUCActiveFlagNone;		}
	bool isFocus() const;
	void setFocus(bool bFocus = true);
	bool isInitial() const;
	int getOperateHandle() const;
	gtc::imageResource* getBkImage() const;
	void setBkImage(gtc::imageResource *pImage);
	gtc::imageResource* getBkImageSelected() const;
	SIZE getRoundCorner() const;
	void setBkImageSelected(gtc::imageResource *pImage);
protected:
	static HWND		mc_hWnd;	// 依赖的绘图窗口，如：桌面窗口
	static std::vector<BSTimerInfo *>	mc_vecTimers;	// 时钟集合
	static UINT		mc_uTimerID;			// 实际时钟ID
	
	BEUCActiveFlag	m_beActive;			// 当前操作标记

	POINT			m_lastMousePos;		// 鼠标当前坐标
	controlUC		*m_pEventClick;		// 当前事件处理者
	controlUC		*m_pEventHover;		// 当前事件处理者

	std::vector<controlUC *>	m_vecControl;	// 现有的引用，本处无需释放
	std::vector<INotifyUC *>	m_vecNotifys;	// 接收通知消息的窗口的引用，本处无需释放
	std::wstring	m_wsTitle;			// 窗口标题

	int				m_iOperateHandle;	// 当前操作区域点标记
	int				m_iTitleHeight;		// 窗口标题区高度
	RECT			m_rcSizeBox;		// 窗口尺寸可以改变，哪一边不能拖动就设置为0
	BYTE			m_nAlphaClient;		// 客户区透明度
	BYTE			m_nAlphaTitle;		// 标题区透明度

	RECT			m_rtPos;			// 窗口的屏幕坐标位置
	bool			m_bUpdateNeeded;	// 是否需要重新绘制
	bool			m_bActivate;		// 当前是否处于活动状态
	bool			m_bLockPos;			// 锁定的窗口不能移位也不能改变大小
	bool			m_bFocus;			// 得到光标
	BESelectMode	m_beSelectMode;		// 当前的操作模式

	bool			m_bInitial;			// 处于没有操作前的状态，当鼠标entry后就false
	bool			m_bVisible;
	bool			m_bEnabled;
	BSFontInfoUC	m_bsDefaultFont;	// 默认字体

	SIZE			m_szRoundCorner;	// 窗口圆角区域大小
	gtc::imageResource	*m_pimgBk;			// 背景图，无需释放内存
	gtc::imageResource	*m_pimgBkSelected;	// 选中时的背景图，无需释放内存
};

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
	void setCapture();
	void releaseCapture();
	bool isCaptured();
public:
	virtual LRESULT sendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	virtual LRESULT postMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
public:
	static void setInstance(HINSTANCE hInstance);
	static void messageLoop();
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
	static HINSTANCE	mc_hInstance;	// 
protected:
	bool			m_bMouseCapture;	// 
	bool			m_bSubclassed;
	HWND			m_hWnd;				// 窗口句柄
	WNDPROC			m_OldWndProc;

};

}