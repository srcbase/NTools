#pragma once
namespace wui {

/////////////////////////////////////////////////////////////////////////////////////
//

class controlUI;


/////////////////////////////////////////////////////////////////////////////////////
//

typedef enum EVENTTYPE_UI
{
	UIEVENT__FIRST = 1,
	UIEVENT__KEYBEGIN,
	UIEVENT_KEYDOWN,
	UIEVENT_KEYUP,
	UIEVENT_CHAR,
	UIEVENT_SYSKEY,
	UIEVENT__KEYEND,
	UIEVENT__MOUSEBEGIN,
	UIEVENT_MOUSEMOVE,
	UIEVENT_MOUSELEAVE,
	UIEVENT_MOUSEENTER,
	UIEVENT_MOUSEHOVER,
	UIEVENT_BUTTONDOWN,
	UIEVENT_BUTTONUP,
	UIEVENT_RBUTTONDOWN,
	UIEVENT_DBLCLICK,
	UIEVENT_CONTEXTMENU,
	UIEVENT_SCROLLWHEEL,
	UIEVENT__MOUSEEND,
	UIEVENT_KILLFOCUS,
	UIEVENT_SETFOCUS,
	UIEVENT_WINDOWSIZE,
	UIEVENT_SETCURSOR,
	UIEVENT_TIMER,
	UIEVENT_NOTIFY,
	UIEVENT_COMMAND,
	UIEVENT__LAST,
};

/////////////////////////////////////////////////////////////////////////////////////
//

// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_ME_FIRST      0x80000000

// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040



/////////////////////////////////////////////////////////////////////////////////////
//

typedef struct tagTFontInfo
{
	HFONT hFont;
	stringex sFontName;
	int iSize;
	bool bBold;
	bool bUnderline;
	bool bItalic;
	TEXTMETRIC tm;
} TFontInfo;



// Structure for notifications from the system
// to the control implementation.
typedef struct tagTEventUI
{
	int Type;
	controlUI* pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	wchar_t chKey;
	WORD wKeyState;
	WPARAM wParam;
	LPARAM lParam;
} TEventUI;

// Structure for notifications to the outside world
typedef struct tagTNotifyUI 
{
	stringex sType;
	controlUI* pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	WPARAM wParam;
	LPARAM lParam;
} TNotifyUI;

// Structure for relative position to the parent
typedef struct tagTRelativePosUI
{
	bool bRelative;
	SIZE szParent;
	int nMoveXPercent;
	int nMoveYPercent;
	int nZoomXPercent;
	int nZoomYPercent;
}TRelativePosUI;


typedef struct tagFINDTABINFO
{
	controlUI* pFocus;
	controlUI* pLast;
	bool bForward;
	bool bNextIsIt;
} FINDTABINFO;

typedef struct tagFINDSHORTCUT
{
	wchar_t ch;
	bool bPickNext;
} FINDSHORTCUT;

typedef struct tagTIMERINFO
{
	controlUI* pSender;
	UINT nLocalID;
	HWND hWnd;
	UINT uWinTimer;
	bool bKilled;
} TIMERINFO;


// Listener interface
class INotifyUI
{
public:
	virtual void notify(TNotifyUI& msg) = 0;
};

// MessageFilter interface
class IMessageFilterUI
{
public:
	virtual LRESULT messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class WINUI_API paintManagerUI
{
public:
	paintManagerUI();
	~paintManagerUI();

public:
	// 初始化
	void init(HWND hWnd);
	void needUpdate();
	void invalidate(RECT& rcItem);

	HDC getPaintDC() const;
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
	static stringex getInstancePath();
	static stringex getCurrentPath();
	static HINSTANCE getResourceDll();
	static const stringex& getResourcePath();
	static const stringex& getResourceZip();
	static void setInstance(HINSTANCE hInst);
	static void setCurrentPath(PCWSTR pStrPath);
	static void setResourceDll(HINSTANCE hInst);
	static void setResourcePath(PCWSTR pStrPath);
	static void setResourceZip(PCWSTR pStrZip);
	static void getHSL(short* H, short* S, short* L);
	static void setHSL(bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 
	static void reloadSkin();

	bool useParentResource(paintManagerUI* pm);
	paintManagerUI* getParentResource() const;

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
	TFontInfo* getDefaultFontInfo();
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
	TFontInfo* getFontInfo(int index);
	TFontInfo* getFontInfo(HFONT hFont);

	const TImageInfo* getImage(PCWSTR bitmap);
	const TImageInfo* getImageEx(PCWSTR bitmap, PCWSTR type = NULL, DWORD mask = 0);
	const TImageInfo* addImage(PCWSTR bitmap, PCWSTR type = NULL, DWORD mask = 0);
	const TImageInfo* addImage(PCWSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha);
	bool removeImage(PCWSTR bitmap);
	void removeAllImages();
	void reloadAllImages();

	void addDefaultAttributeList(PCWSTR pStrControlName, PCWSTR pStrControlAttrList);
	PCWSTR getDefaultAttributeList(PCWSTR pStrControlName) const;
	bool removeDefaultAttributeList(PCWSTR pStrControlName);
	const std::map<stringex, stringex *>& getDefaultAttribultes() const;
	void removeAllDefaultAttributeList();

	bool attachDialog(controlUI* pControl);
	bool initControls(controlUI* pControl, controlUI* pParent = NULL);
	void reapObjects(controlUI* pControl);

	bool addOptionGroup(PCWSTR pStrGroupName, controlUI* pControl);
	std::vector<controlUI*>* getOptionGroup(PCWSTR pStrGroupName);
	void removeOptionGroup(PCWSTR pStrGroupName, controlUI* pControl);
	void removeAllOptionGroups();

	controlUI* getFocus() const;
	void setFocus(controlUI* pControl);
	void setFocusNeeded(controlUI* pControl);

	bool setNextTabControl(bool bForward = true);

	bool setTimer(controlUI* pControl, UINT nTimerID, UINT uElapse);
	bool killTimer(controlUI* pControl, UINT nTimerID);
	void killTimer(controlUI* pControl);
	void removeAllTimers();

	void setCapture();
	void releaseCapture();
	bool isCaptured();

	bool addNotifier(INotifyUI* pControl);
	bool removeNotifier(INotifyUI* pControl);   
	void sendNotify(TNotifyUI& Msg, bool bAsync = false);
	void sendNotify(controlUI* pControl, PCWSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

	bool addPreMessageFilter(IMessageFilterUI* pFilter);
	bool removePreMessageFilter(IMessageFilterUI* pFilter);

	bool addMessageFilter(IMessageFilterUI* pFilter);
	bool removeMessageFilter(IMessageFilterUI* pFilter);

	int getPostPaintCount() const;
	bool addPostPaint(controlUI* pControl);
	bool removePostPaint(controlUI* pControl);
	bool setPostPaintIndex(controlUI* pControl, int iIndex);

	void addDelayedCleanup(controlUI* pControl);

	controlUI* getRoot() const;
	controlUI* findControl(POINT pt) const;
	controlUI* findControl(PCWSTR pstrName) const;
	controlUI* findSubControlByPoint(controlUI* pParent, POINT pt) const;
	controlUI* findSubControlByName(controlUI* pParent, PCWSTR pstrName) const;
	controlUI* findSubControlByClass(controlUI* pParent, PCWSTR pstrClass, int iIndex = 0);
	std::vector<void*>* findSubControlsByClass(controlUI* pParent, PCWSTR pstrClass);
	std::vector<void*>* getSubControlsByClass();

	static void messageLoop();
	static bool translateMessage(const LPMSG pMsg);

	bool messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	bool preMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

private:
	static controlUI* CALLBACK __findControlFromNameHash(controlUI* pThis, LPVOID pData);
	static controlUI* CALLBACK __findControlFromCount(controlUI* pThis, LPVOID pData);
	static controlUI* CALLBACK __findControlFromPoint(controlUI* pThis, LPVOID pData);
	static controlUI* CALLBACK __findControlFromTab(controlUI* pThis, LPVOID pData);
	static controlUI* CALLBACK __findControlFromShortcut(controlUI* pThis, LPVOID pData);
	static controlUI* CALLBACK __findControlFromUpdate(controlUI* pThis, LPVOID pData);
	static controlUI* CALLBACK __findControlFromName(controlUI* pThis, LPVOID pData);
	static controlUI* CALLBACK __findControlFromClass(controlUI* pThis, LPVOID pData);
	static controlUI* CALLBACK __findControlsFromClass(controlUI* pThis, LPVOID pData);

private:
	HWND m_hWndPaint;
	HDC m_hDcPaint;
	HDC m_hDcOffscreen;
	HDC m_hDcBackground;
	HBITMAP m_hbmpOffscreen;
	HBITMAP m_hbmpBackground;
	HWND m_hwndTooltip;
	TOOLINFO m_ToolTip;
	bool m_bShowUpdateRect;		// 
	//
	controlUI* m_pRoot;
	controlUI* m_pFocus;
	controlUI* m_pEventHover;
	controlUI* m_pEventClick;
	controlUI* m_pEventKey;
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
	std::vector<INotifyUI *> m_aNotifiers;	// 同步通知
	std::vector<TIMERINFO *> m_aTimers;
	std::vector<IMessageFilterUI *> m_aPreMessageFilters;
	std::vector<IMessageFilterUI *> m_aMessageFilters;
	std::vector<controlUI *> m_aPostPaintControls;
	std::vector<controlUI *> m_aDelayedCleanup;
	std::vector<TNotifyUI *> m_aAsyncNotify;	// 被m_aNotifiers通知的数据
	std::vector<void*> m_aFoundControls;
	std::map<stringex, controlUI *> m_mNameHash;
	std::map<stringex, std::vector<controlUI *>*> m_mOptionGroup;
	//
	paintManagerUI* m_pParentResourcePM;
	DWORD m_dwDefaultDisabledColor;			// 窗体缺省字体色
	DWORD m_dwDefaultFontColor;
	DWORD m_dwDefaultLinkFontColor;
	DWORD m_dwDefaultLinkHoverFontColor;
	DWORD m_dwDefaultSelectedBkColor;
	TFontInfo m_DefaultFontInfo;
	std::vector<TFontInfo *> m_aCustomFonts;

	std::map<stringex, TImageInfo *> m_mImageHash;
	std::map<stringex, stringex *> m_DefaultAttrHash;
	//
	static HINSTANCE m_hInstance;
	static HINSTANCE m_hResourceInstance;
	static stringex m_pStrResourcePath;
	static stringex m_pStrResourceZip;
	static short m_H;
	static short m_S;
	static short m_L;
	static std::vector<paintManagerUI *> m_aPreMessages;
};


}