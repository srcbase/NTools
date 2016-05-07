#pragma once

namespace wuc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
class controlUC;

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

// Flags for CControlUI::GetControlFlags()
#define UCFLAG_TABSTOP       0x00000001
#define UCFLAG_SETCURSOR     0x00000002
#define UCFLAG_WANTRETURN    0x00000004

// Flags for FindControl()
#define UCFIND_ALL           0x00000000
#define UCFIND_VISIBLE       0x00000001
#define UCFIND_ENABLED       0x00000002
#define UCFIND_HITTEST       0x00000004
#define UCFIND_TOP_FIRST     0x00000008
#define UCFIND_ME_FIRST      0x80000000

// Flags for the CDialogLayout stretching
#define UCSTRETCH_NEWGROUP   0x00000001
#define UCSTRETCH_NEWLINE    0x00000002
#define UCSTRETCH_MOVE_X     0x00000004
#define UCSTRETCH_MOVE_Y     0x00000008
#define UCSTRETCH_SIZE_X     0x00000010
#define UCSTRETCH_SIZE_Y     0x00000020

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

// Structure for relative position to the parent
typedef struct BSRelativePosUC
{
	bool bRelative;
	SIZE szParent;
	int nMoveXPercent;
	int nMoveYPercent;
	int nZoomXPercent;
	int nZoomYPercent;
}BSRelativePosUC;

typedef struct BSFindTabInfo
{
	controlUC* pFocus;
	controlUC* pLast;
	bool bForward;
	bool bNextIsIt;
} BSFindTabInfo;

typedef struct BSFindShortCut
{
	wchar_t ch;
	bool bPickNext;
} BSFindShortCut;

typedef struct BSTimerInfo
{
	controlUC* pSender;
	UINT nLocalID;
	HWND hWnd;
	UINT uWinTimer;
	bool bKilled;
} BSTimerInfo;

// Listener interface
class INotifyUC
{
public:
	virtual void notify(BSNotifyUC& msg) = 0;	// 接收来者控件的通知
};
// MessageFilter interface
class IMessageFilterUC
{
public:
	virtual LRESULT messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
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

class WINUC_API paintManagerUC
{
public:
	paintManagerUC(void);
	~paintManagerUC(void);

public:
	// 初始化
	void init(HWND hWnd);
	void needUpdate();
	void invalidate(RECT& rcItem);

	HDC getPaintDC() const;
	void setPaintDC(HDC hdc) {
		m_hDcPaint = hdc;
	}
	HWND getPaintWindow() const;
	HWND getTooltipWindow() const;

	POINT getMousePos() const;
	SIZE getClientSize() const;
	SIZE getInitSize();
	void setInitSize(int cx, int cy);
	RECT& getSizeBox();
	void setSizeBox(RECT& rcSizeBox);
	RECT& getCaptionRect();
	void setCaptionRect(RECT& rcCaption);
	SIZE getRoundCorner() const;
	void setRoundCorner(int cx, int cy);
	SIZE getMinInfo() const;
	void setMinInfo(int cx, int cy);
	SIZE getMaxInfo() const;
	void setMaxInfo(int cx, int cy);
	void setTransparent(int nOpacity);
	void setBackgroundTransparent(bool bTrans);
	bool isShowUpdateRect() const;
	void setShowUpdateRect(bool show);

	static HINSTANCE getInstance();
	static void setInstance(HINSTANCE hInst);
	static gtc::resourceManager& getResourceManager();
	static void setResourceManager(HMODULE hResource);

	// 图片 png
	static bool addImage(std::wstring imgKey, UINT id, LPCTSTR pType = RT_RCDATA);
	static gtc::image* findImage(std::wstring imgKey);
	// 菜单
	static bool addMenu(std::wstring menuKey, UINT id);
	static HMENU findMenu(std::wstring menuKey);
 
	//static std::wstring getInstancePath();
	//static std::wstring getCurrentPath();
	//static HINSTANCE getResourceDll();
	//static const std::wstring& getResourcePath();
	//static const std::wstring& getResourceZip();
	//static void setCurrentPath(PCWSTR pStrPath);
	//static void setResourceDll(HINSTANCE hInst);
	//static void setResourcePath(PCWSTR pStrPath);
	//static void setResourceZip(PCWSTR pStrZip);
	//static void getHSL(short* H, short* S, short* L);
	//static void setHSL(bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 
	//static void reloadSkin();

	//bool useParentResource(paintManagerUC* pm);
	//paintManagerUC* getParentResource() const;

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
	BSFontInfoUC* getDefaultFontInfo();
	void setDefaultFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	DWORD getCustomFontCount() const;
	HFONT addFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	HFONT addFontAt(int index, PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	HFONT getFont(int index);
	HFONT getFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	bool findFont(HFONT hFont);
	bool findFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	int getFontIndex(HFONT hFont);
	int getFontIndex(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	bool removeFont(HFONT hFont);
	bool removeFontAt(int index);
	void removeAllFonts();
	BSFontInfoUC* getFontInfo(int index);
	BSFontInfoUC* getFontInfo(HFONT hFont);

	void addDefaultAttributeList(PCWSTR pStrControlName, PCWSTR pStrControlAttrList);
	PCWSTR getDefaultAttributeList(PCWSTR pStrControlName) const;
	bool removeDefaultAttributeList(PCWSTR pStrControlName);
	const std::map<std::wstring, std::wstring *>& getDefaultAttribultes() const;
	void removeAllDefaultAttributeList();

	bool attachDialog(controlUC* pControl);
	bool initControls(controlUC* pControl, controlUC* pParent = NULL);
	void reapObjects(controlUC* pControl);

	bool addOptionGroup(PCWSTR pStrGroupName, controlUC* pControl);
	std::vector<controlUC*>* getOptionGroup(PCWSTR pStrGroupName);
	void removeOptionGroup(PCWSTR pStrGroupName, controlUC* pControl);
	void removeAllOptionGroups();

	controlUC* getFocus() const;
	void setFocus(controlUC* pControl);
	void setFocusNeeded(controlUC* pControl);

	bool setNextTabControl(bool bForward = true);

	bool setTimer(controlUC* pControl, UINT nTimerID, UINT uElapse);
	bool killTimer(controlUC* pControl, UINT nTimerID);
	void killTimer(controlUC* pControl);
	void removeAllTimers();

	void setCapture();
	void releaseCapture();
	bool isCaptured();

	bool addNotifier(INotifyUC* pControl);
	bool removeNotifier(INotifyUC* pControl);   
	void sendNotify(BSNotifyUC& Msg, bool bAsync = false);
	void sendNotify(controlUC* pControl, PCWSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

	bool addPreMessageFilter(IMessageFilterUC* pFilter);
	bool removePreMessageFilter(IMessageFilterUC* pFilter);

	bool addMessageFilter(IMessageFilterUC* pFilter);
	bool removeMessageFilter(IMessageFilterUC* pFilter);

	//int getPostPaintCount() const;
	//bool addPostPaint(controlUC* pControl);
	//bool removePostPaint(controlUC* pControl);
	//bool setPostPaintIndex(controlUC* pControl, int iIndex);

	//void addDelayedCleanup(controlUC* pControl);

	controlUC* getRoot() const;
	controlUC* findControl(POINT pt) const;
	controlUC* findControl(PCWSTR pstrName) const;
	controlUC* findSubControlByPoint(controlUC* pParent, POINT pt) const;
	controlUC* findSubControlByName(controlUC* pParent, PCWSTR pstrName) const;
	controlUC* findSubControlByClass(controlUC* pParent, PCWSTR pstrClass, int iIndex = 0);
	std::vector<void*>* findSubControlsByClass(controlUC* pParent, PCWSTR pstrClass);
	std::vector<void*>* getSubControlsByClass();

	static void messageLoop();
	static bool translateMessage(const LPMSG pMsg);

	bool messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	bool preMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

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

private:
	HWND		m_hWndPaint;
	HDC			m_hDcPaint;
	HDC			m_hDcOffscreen;
	HDC			m_hDcBackground;
	HBITMAP		m_hbmpOffscreen;
	HBITMAP		m_hbmpBackground;
	HWND		m_hwndTooltip;
	TOOLINFO	m_ToolTip;
	bool		m_bShowUpdateRect;		// 
	//
	controlUC* m_pRoot;
	controlUC* m_pFocus;
	controlUC* m_pEventHover;
	controlUC* m_pEventClick;
	controlUC* m_pEventKey;
	//
	POINT m_ptLastMousePos;
	SIZE m_szMinWindow;			// 窗口最小尺寸
	SIZE m_szMaxWindow;			// 窗口最大尺寸
	SIZE m_szInitWindowSize;	// 窗口启动时的大小 size
	RECT m_rcSizeBox;			// 边框可拖拽，值为距左上右下边框的区域大小，在区域内鼠标可变，以提示可以拖动边框。如果想使某边框不可移动，使其为0即可。
	SIZE m_szRoundCorner;		// 窗口是否有圆角及其大小
	RECT m_rcCaption;			// 窗口标题区域
	UINT m_uTimerID;
	bool m_bFirstLayout;
	bool m_bUpdateNeeded;
	bool m_bFocusNeeded;
	bool m_bOffscreenPaint;
	bool m_bAlphaBackground;	// 
	bool m_bMouseTracking;
	bool m_bMouseCapture;
	//
	std::vector<INotifyUC *> m_aNotifiers;	// 同步通知
	std::vector<BSTimerInfo *> m_aTimers;
	std::vector<IMessageFilterUC *> m_aPreMessageFilters;
	std::vector<IMessageFilterUC *> m_aMessageFilters;
	//std::vector<controlUC *> m_aPostPaintControls;
	//std::vector<controlUC *> m_aDelayedCleanup;
	std::vector<BSNotifyUC *> m_aAsyncNotify;	// 被m_aNotifiers通知的数据
	std::vector<void*> m_aFoundControls;
	std::map<std::wstring, controlUC *> m_mNameHash;
	std::map<std::wstring, std::vector<controlUC *>*> m_mOptionGroup;
	//
	//paintManagerUC* m_pParentResourcePM;
	DWORD m_dwDefaultDisabledColor;			// 窗体缺省字体色
	DWORD m_dwDefaultFontColor;
	DWORD m_dwDefaultLinkFontColor;
	DWORD m_dwDefaultLinkHoverFontColor;
	DWORD m_dwDefaultSelectedBkColor;
	BSFontInfoUC m_DefaultFontInfo;
	std::vector<BSFontInfoUC *> m_aCustomFonts;

	//std::map<std::wstring, BSImageInfo *> m_mImageHash;
	std::map<std::wstring, std::wstring *> m_DefaultAttrHash;
	//
	static HINSTANCE mc_hInstance;
	static gtc::resourceManager mc_resourceManager;
	//static HINSTANCE m_hResourceInstance;
	//static std::wstring m_pStrResourcePath;
	//static std::wstring m_pStrResourceZip;
	//static short m_H;
	//static short m_S;
	//static short m_L;
	static std::vector<paintManagerUC *> mc_vecPreMessages;
};





}