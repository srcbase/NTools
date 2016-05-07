#pragma once
#include "wucdef.h"
#include "AC/SyncObject.h"
namespace wuc{
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-10-14 WS_EX_LAYERED窗口控件的一些属性
struct BSLayerWndAttribute
{
	HWND	hWnd;				// 控件窗口句柄
	bool	isVisiblePrev;		// 记录控件先前的可见状态，

	BSLayerWndAttribute()
		: hWnd(NULL), isVisiblePrev(true)
	{}
	BSLayerWndAttribute(HWND _hWnd)
		: hWnd(_hWnd), isVisiblePrev(true)
	{}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
class controlUC;
class optionUC;
class containerUC;
class verticalLayoutUC;
class horizontalLayoutUC;
class INotifyUC;
typedef std::vector<controlUC *>					ControlArrayType;
typedef std::vector<optionUC *>						OptionArrayType;
typedef std::map<std::wstring, controlUC *>			ControlMapType;
typedef std::vector<INotifyUC *>					NotifyArrayType;
typedef std::map<std::wstring, ControlArrayType>	GroupMapType;
typedef std::map<controlUC*, BSLayerWndAttribute>	LayerWndMapType;
typedef std::vector<containerUC *>					ContainerArrayType;
typedef std::vector<verticalLayoutUC *>				VContainerArrayType;
typedef std::vector<horizontalLayoutUC *>			HContainerArrayType;
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

	//BSEventUC()
	//	: pSender(nullptr), Type(0)
	//	, dwTimestamp(0), chKey(0), wKeyState(0)
	//	, wParam(NULL), lParam(NULL)
	//{
	//	memset(&ptMouse, 0, sizeof(ptMouse));
	//}
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
	// 
	BSNotifyUC()
		: pSender(nullptr)
		, dwTimestamp(GetTickCount())
		, wParam(NULL), lParam(NULL)
	{
		memset(&ptMouse, 0, sizeof(ptMouse));
	}
	BSNotifyUC(PCWSTR pType, controlUC *pControl)
		: wsType(pType? pType: L""), pSender(pControl)
		, dwTimestamp(GetTickCount())
		, wParam(NULL), lParam(NULL)
	{
		assert(!wsType.empty());
		memset(&ptMouse, 0, sizeof(ptMouse));
	}
} BSNotifyUC;

// Structure for relative position to the parent
typedef struct BSRelativePosUC
{
	bool bRelative;
	SIZE szParent;
	int nMoveXPercent;
	int nMoveYPercent;
	int nZoomXPercent;
	int nZoomYPercent;
	//// 
	//BSRelativePosUC()
	//	: bRelative(false)
	//	, nMoveXPercent(0), nMoveYPercent(0)
	//	, nZoomXPercent(0), nZoomYPercent(0)
	//{
	//	memset(&szParent, 0, sizeof(szParent));
	//}
}BSRelativePosUC;

// Listener interface
class INotifyUC
{
public:
	virtual ~INotifyUC() {}
	virtual void notify(BSNotifyUC &msg) = 0;	// 接收来者控件的通知
};

// MessageFilter interface
class IMessageFilterUC
{
public:
	virtual ~IMessageFilterUC() {}
	virtual LRESULT messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};
typedef std::vector<IMessageFilterUC *> MessageFilterArrayType;

// 
class ITranslateAccelerator
{
public:
	virtual ~ITranslateAccelerator() {}
	virtual LRESULT translateAccelerator(MSG *pMsg) = 0;
};
typedef std::vector<ITranslateAccelerator *> TranslateAcceleratorArrayType;
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	IWindowUC  类		2015-8-10
//	共子窗口使用的接口
////////////////////////////////////////////////////////////////////////////////////////////////////////
class IWindowUC
{
public:
	virtual ~IWindowUC() {}
	virtual controlUC* getWindowOwner() = 0;
};

typedef struct BSTimerInfo
{
	controlUC*	pSender;
	UINT		nLocalID;
	HWND		hWnd;
	UINT		uWinTimer;
	bool		bKilled;
}BSTimerInfo;

typedef struct BSFindTabInfo
{
	controlUC* pFocus;
	controlUC* pLast;
	bool bForward;
	bool bNextIsIt;
}BSFindTabInfo;

typedef struct BSFindShortCut
{
	wchar_t ch;
	bool bPickNext;
}BSFindShortCut;
/////////////////////////////////////////////////////////////////////////////////////
//

class renderClip
{
public:
	~renderClip();
	RECT rcItem;
	HDC hDC;
	HRGN hRgn;
	HRGN hOldRgn;

	static void generateClip(HDC hDC, RECT rc, renderClip& clip);
	static void generateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, renderClip& clip);
	static void useOldClipBegin(HDC hDC, renderClip& clip);
	static void useOldClipEnd(HDC hDC, renderClip& clip);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	drawManager类	2014-07-30
//	窗口的绘图管理类
////////////////////////////////////////////////////////////////////////////////////////////////////////

class drawManager
{
public:
public:
	drawManager(void);
	~drawManager(void);

public:
	void init(HWND hWnd);
	void needUpdate();
	void invalidate(RECT& rcItem);

	HWND getPaintWindow() const;
	HDC  getPaintDC() const;
	HWND getTooltipWindow() const;

	bool attachDialog(controlUC* pControl);
	bool initControls(controlUC* pControl, controlUC* pParent = NULL);
	void reapObject(controlUC* pControl);

	void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);	// 设置窗口属性值
	
	void clientToScreen(RECT &rect);
	void clientToScreen(POINT &pt);
	void screenToClient(RECT &rect);
	void screenToClient(POINT &pt);

	POINT getMousePos() const;
	SIZE getClientSize() const;
	SIZE getInitSize();
	void setInitSize(int cx, int cy);
	RECT& getSizeBox();
	void setSizeBox(RECT& rcSizeBox);
	RECT& getCaptionRect();
	void setCaptionRect(LONG height);
	void setCaptionRect(RECT& rcCaption);
	SIZE getRoundCorner() const;
	void setRoundCorner(int cx, int cy);
	SIZE getMinInfo() const;
	void setMinInfo(int cx, int cy);
	SIZE getMaxInfo() const;
	void setMaxInfo(int cx, int cy);

	void setTransparent(int nOpacity);
	void setBackgroundTransparent(bool bTrans);
	bool isBackgroundTransparent() const;
	void setLayerWindowTransparent(bool bTrans);
	bool isLayerWindowTransparent() const;
	//void setBackgroupImage(PCWSTR imgKeyName);

	DWORD getDefaultDisabledColor() const;
	void setDefaultDisabledColor(DWORD dwColor);
	DWORD getDefaultFontColor() const;
	void setDefaultFontColor(DWORD dwColor);
	DWORD getDefaultLinkFontColor() const;
	void setDefaultLinkFontColor(DWORD dwColor);
	DWORD getDefaultLinkHoverFontColor() const;
	void setDefaultLinkHoverFontColor(DWORD dwColor);
	DWORD getDefaultSelectedBkColor() const;
	void setDefaultSelectedBkColor(DWORD dwColor);

	static const BSFontInfoUC* getDefaultFontInfo();
	static void setDefaultFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);

	static gtc::image* findImage(PCWSTR pKeyName);
	static const BSFontInfoUC* findFont(int fontIndex);
	static HFONT getFont(int fontIndex);
	static HMENU findMenu(PCWSTR pKeyName);
	static PCWSTR getDefaultAttributeList(PCWSTR pControlKey);

	void setCapture();
	void releaseCapture();
	bool isCaptured();

	controlUC* getRoot() const;
	controlUC* findControl(POINT pt) const;
	controlUC* findControl(PCWSTR pstrName) const;
	controlUC* findSubControlByPoint(controlUC* pParent, POINT pt) const;
	controlUC* findSubControlByName(controlUC* pParent, PCWSTR pstrName) const;
	controlUC* findSubControlByClass(controlUC* pParent, PCWSTR pstrClass, int iIndex = 0);
	ControlArrayType& findSubControlsByClass(controlUC* pParent, PCWSTR pstrClass);
	ControlArrayType& getSubControlsByClass();

	static bool addOptionGroup(PCWSTR pstrGroupName, controlUC* pControl);
	static ControlArrayType getOptionGroup(PCWSTR pstrGroupName);
	static void removeOptionGroup(PCWSTR pstrGroupName, controlUC* pControl);
	static void removeAllOptionGroups();

	controlUC* getFocus() const;
	void setFocus(controlUC* pControl);
	void setFocusNeeded(controlUC* pControl);

	bool setNextTabControl(bool bForward = true);

	void addDelayedCleanup(controlUC* pControl);
	void addDelayedFree(controlUC* pControl);
	bool existsDelayedFree(controlUC* pControl);	// 查看控件是否已经在延迟释放控件集会中
	void clearDelayedFree();	// 释放并清空延迟释放控件
	bool emptyDelayedFree();	// 是否存在需要延迟释放德控件
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-10-14 	添加WS_EX_LAYERED窗口包含的控件
	void addLayerWindowControl(controlUC *pOwner, HWND hWnd);
	void removeLayerWindowControl(controlUC *pOwner);
	void recordLayerWindowControlAttr();		// 记录当前控件属性
	void restoreLayerWindowControlAttr();		// 恢复先前保存的控件属性
	void showLayerWindowControl(bool bShow = true);		// 

	bool addNotifier(INotifyUC* pControl);
	bool removeNotifier(INotifyUC* pControl);   
	void sendNotify(BSNotifyUC& Msg, bool bAsync = false);
	void sendNotify(controlUC* pControl, PCWSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

	bool setTimer(controlUC* pControl, UINT nTimerID, UINT uElapse);
	bool killTimer(controlUC* pControl, UINT nTimerID);
	void killTimer(controlUC* pControl);
	void removeAllTimers();

	bool addMessageFilter(IMessageFilterUC *pFilter);
	bool removeMessageFilter(IMessageFilterUC *pFilter);

	bool addTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool removeTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool translateAccelerator(LPMSG pMsg);

	bool messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	bool preMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-4-27 设定排除刷新区域
	void setBesidesArea(RECT rcBesidesArea);
	void freeBesidesArea();
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-4-28 更新除“排除区域”外所有区域	
	void updateWindowOutBesidesArea();
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-6-8 由于拖放在WM_LBUTTONDOWN、WM_MOUSEMOVE中启动，没有对应的WM_LBUTTONUP，此处模拟一个
	void endDragDrop();
public:
	// static 
	static HINSTANCE getInstance();
	static void setInstance(HINSTANCE hInst);
	static gtc::resourceManager& getResource();

	static void messageLoop();
	static bool translateMessage(const LPMSG pMsg);
public:
	// wuc内部使用的消息
	static UINT	WUC_MSG_WINDOW_INIT;		// 发送windowinit通知时顺便往窗口发送的消息 WPARAM=NULL LPARAM=NULL
//protected:
	void updateLayeredWindow();
	// 计算气泡提示尺寸
	SIZE estimateToolTipSize();

protected:
	// 针对非释放非自动释放控件的延迟释放
	static unsigned int WINAPI threadDelayedFreeControl(void *pParam/*gtc::thread::_BSThreadInfo *pthr*/);
	void startDelayedFreeControl();
	void stopDelayedFreeControl();

protected:
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-10-8 定义对m_vecDelayedFree的保护锁	
#if defined(GTC_BOOST_THREAD)
	protected:
		mutable boost::shared_mutex m_rwMutexDelayedFree;
		mutable bool				m_bReadLockedDelayedFree;
		mutable bool				m_bWriteLockedDelayedFree;
	public:
#define READ_LOCK_DLYFREE	gtc::readLock	rdLockDelayedFree(m_rwMutexDelayedFree, m_bReadLockedDelayedFree, __FILEW__, __FUNCTIONW__);
#define WRITE_LOCK_DLYFREE	gtc::writeLock	wtLockDelayedFree(m_rwMutexDelayedFree, m_bWriteLockedDelayedFree, __FILEW__, __FUNCTIONW__);
#else
#define READ_LOCK_DLYFREE
#define WRITE_LOCK_DLYFREE
#endif

private:
	static controlUC* CALLBACK __findControlFromNameHash(controlUC* pThis, LPVOID pData);
	static controlUC* CALLBACK __findControlFromCount(controlUC* pThis, LPVOID pData);
	static controlUC* CALLBACK __findControlFromPoint(controlUC* pThis, LPVOID pData);
	static controlUC* CALLBACK __findControlFromTab(controlUC* pThis, LPVOID pData);
	static controlUC* CALLBACK __findControlFromShortcut(controlUC* pThis, LPVOID pData);
	static controlUC* CALLBACK __findControlFromUpdate(controlUC* pThis, LPVOID pData);
	static controlUC* CALLBACK __findControlFromName(controlUC* pThis, LPVOID pData);
	static controlUC* CALLBACK __findControlFromClass(controlUC* pThis, LPVOID pData);
	static controlUC* CALLBACK __findControlsFromClass(controlUC* pThis, LPVOID pData);
	//public:
	static HINSTANCE			mc_hInstance;	// 内存的资源句柄实例
	static gtc::resourceManager	mc_resManager;	// 图片，菜单等资源管启

	HWND			m_hwndDrawer;
	HDC				m_hdcDrawer;
	HDC				m_hdcOffscreen;
	HDC				m_hdcBackground;
	HBITMAP			m_hbmpOffscreen;
	HBITMAP			m_hbmpBackground;
	LPBYTE			m_pBmpBackgroundBits;

	HWND			m_hwndTooltip;
	TOOLINFO		m_tipToolTip;

	//
	controlUC		*m_pRoot;
	controlUC		*m_pFocus;
	controlUC		*m_pEventHover;
	controlUC		*m_pEventClick;
	controlUC		*m_pEventKey;
	controlUC		*m_pEventWheel;

	bool			m_bFirstLayout;
	bool			m_bUpdateNeeded;
	bool			m_bFocusNeeded;
	POINT			m_ptLastMousePos;
	SIZE			m_szInitWindowSize;			// 窗口初始大小
	RECT			m_rcSizeBox;				// 边框可拖拽，值为距左上右下边框的区域大小，在区域内鼠标可变，以提示可以拖动边框。如果想使某边框不可移动，使其为0即可。
	SIZE			m_szRoundCorner;			// 窗口是否有圆角及其大小
	RECT			m_rcCaption;				// 窗口可拖动的标题栏大小的边距,最后一个参数是指离上边框的距离,如(0,0,0,28)
	SIZE			m_szMinWindow;				// 窗口最小尺寸
	SIZE			m_szMaxWindow;				// 窗口最大尺寸
	bool			m_bAlphaBackground;			// 如:制作透明圆角窗体
	bool			m_bOffscreenPaint;
	bool			m_bAlphaLayerWindow;		// 通过png图片作为背景以实现透视窗口，需要给窗体增加WS_EX_LAYERED样式，与ActiveX控件冲突，使AX控件不能绘制
												// 注：如果软件运行后无法显示主窗口，则需要在WM_ACTIVATE消息中执行InvalidateRect()
	LayerWndMapType	m_mapLayerWnd;				// 针对WS_EX_LAYERED样式窗口，记录其所属控件key：控件uc，value：控件句柄
	//gtc::image		*m_pImageBk;				// 背景图
	DWORD			m_dwDefaultDisabledColor;	// 窗体缺省字体色
	DWORD			m_dwDefaultFontColor;
	DWORD			m_dwDefaultLinkFontColor;
	DWORD			m_dwDefaultLinkHoverFontColor;
	DWORD			m_dwDefaultSelectedBkColor;
	//BSFontInfoUC	m_bsDefaultFontInfo;

	bool			m_bMouseTracking;			// 窗口是否跟踪到鼠标
	bool			m_bMouseCapture;			// 窗口是否捕获鼠标

	UINT			m_uTimerID;					// 内部时钟m_vecTimers的起始ID
	NotifyArrayType	m_vecNotifys;				// 接收通知消息的窗口的引用，本处无需释放
	std::vector<BSTimerInfo *>	m_vecTimers;	// 时钟集合
	//public:
	ControlArrayType		m_vecDelayedFree;		// 延迟释放控件；比较自身事件中删除自己
	gtc::criticalSection	m_csDelayedFree;		// 对m_vecDelayedFree数据的保护
	volatile LONG			m_lDelayedFreeSwitch;	// threadDelayedFreeControl线程开关
	HANDLE					m_eventDelayedFree;		// threadDelayedFreeControl内部等待事件
	HANDLE					m_threadDelayedFree;	// 

	ControlArrayType		m_vecDelayedCleanup;	// 延迟删除控件；比如某容器控件内部一个子控件被mousemove捕获，如果这时直接删除就会异常
	//gtc::swmrg			m_smDelayedCleanup;
	ControlArrayType		m_vecFoundControls;		// 当前被找到控件的集合
	ControlMapType			m_mapNameControls;		// 控件名称及其的映射集合
	static GroupMapType		mc_mapGroupControls;	// 单选组按钮集合（当前组内只能有一个处于选中状态）； key=组名称 value=optionUC，菜单项等按钮集合
	
	MessageFilterArrayType			m_vecMessageFilter;				// 其他窗口处理的消息
	TranslateAcceleratorArrayType	m_vecTranslateAccelerator;		// 

	static std::vector<drawManager *> mc_vecPreMessages;

	bool			m_bBesidesArea;				// 是否启用排出区域，即指定区域不需要刷新
	RECT			m_rcBesidesArea;			// 无效刷新的区域
	
};

}