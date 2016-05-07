#include "drawManager.h"
#include "wucControl.h"
namespace wuc{

/////////////////////////////////////////////////////////////////////////////////////
//
//

renderClip::~renderClip()
{
	assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	assert(::GetObjectType(hRgn)==OBJ_REGION);
	assert(::GetObjectType(hOldRgn)==OBJ_REGION);
	::SelectClipRgn(hDC, hOldRgn);
	::DeleteObject(hOldRgn);
	::DeleteObject(hRgn);
}

void renderClip::generateClip(HDC hDC, RECT rc, renderClip& clip)
{
	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	clip.hRgn = ::CreateRectRgnIndirect(&rc);
	::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
	clip.hDC = hDC;
	clip.rcItem = rc;
	//if(rectWidth(rc) == 332)
	//{
	//	SEND_LOG(L"debug 1.......");
	//}
}

void renderClip::generateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, renderClip& clip)
{
	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	clip.hRgn = ::CreateRectRgnIndirect(&rc);
	HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right + 1, rcItem.bottom + 1, width, height);
	::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
	::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
	clip.hDC = hDC;
	clip.rcItem = rc;
	//if(rectWidth(rc) == 332)
	//{
	//	SEND_LOG(L"debug 2.......");
	//}
	::DeleteObject(hRgnItem);
}

void renderClip::useOldClipBegin(HDC hDC, renderClip& clip)
{
	::SelectClipRgn(hDC, clip.hOldRgn);
}

void renderClip::useOldClipEnd(HDC hDC,renderClip& clip)
{
	::SelectClipRgn(hDC, clip.hRgn);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	drawManager类	2014-07-30
//	窗口的绘图管理类
////////////////////////////////////////////////////////////////////////////////////////////////////////

HINSTANCE					drawManager::mc_hInstance = NULL;	// 内存的资源句柄实例
gtc::resourceManager		drawManager::mc_resManager;			// 图片，菜单等资源管启
std::vector<drawManager *>	drawManager::mc_vecPreMessages;
GroupMapType				drawManager::mc_mapGroupControls;
UINT						drawManager::WUC_MSG_WINDOW_INIT = ::RegisterWindowMessageW(L"WUC_MSG_WINDOW_INIT");		// 发送windowinit通知时顺便往窗口发送的消息 WPARAM=NULL LPARAM=NULL
drawManager::drawManager(void)
	: m_hwndDrawer(NULL)
	, m_hdcDrawer(NULL)
	, m_hdcOffscreen(NULL), m_hdcBackground(NULL), m_hbmpOffscreen(NULL), m_hbmpBackground(NULL)
	, m_pBmpBackgroundBits(NULL)
	, m_hwndTooltip(NULL)
	, m_pRoot(nullptr), m_pFocus(nullptr), m_pEventHover(nullptr), m_pEventClick(nullptr), m_pEventKey(nullptr)
	, m_pEventWheel(nullptr)
	, m_bFirstLayout(true)
	, m_bUpdateNeeded(false)
	, m_bFocusNeeded(false)
	, m_bAlphaBackground(false)
	, m_bAlphaLayerWindow(false)/*, m_pImageBk(nullptr)*/
	, m_bOffscreenPaint(true)
	, m_bMouseCapture(false), m_bMouseTracking(false)
	, m_uTimerID(0x1000)
	, m_bBesidesArea(false)
	, m_lDelayedFreeSwitch(0), m_eventDelayedFree(NULL), m_threadDelayedFree(NULL)
{
	::ZeroMemory(&m_rcBesidesArea, sizeof(m_rcBesidesArea));
	::ZeroMemory(&m_tipToolTip, sizeof(TOOLINFO));
	m_ptLastMousePos.x = m_ptLastMousePos.y	= -1;

	m_szInitWindowSize.cx = m_szInitWindowSize.cy = 0;
	::ZeroMemory(&m_rcSizeBox, sizeof(RECT));
	m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
	::ZeroMemory(&m_rcCaption, sizeof(RECT));
	m_szMinWindow.cx = m_szMinWindow.cy = 0;
	m_szMaxWindow.cx = m_szMaxWindow.cy = 0;

	m_dwDefaultDisabledColor = 0xFEA7A6AA;
	m_dwDefaultFontColor = 0xFE000000;
	m_dwDefaultLinkFontColor = 0xFE0000FF;
	m_dwDefaultLinkHoverFontColor = 0xFED3215F;
	m_dwDefaultSelectedBkColor = 0xFEBAE4FF;
	//LOGFONT lf = { 0 };
	//::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	//lf.lfCharSet = DEFAULT_CHARSET;
	//HFONT hDefaultFont = ::CreateFontIndirect(&lf);
	//m_bsDefaultFontInfo.hFont = hDefaultFont;
	//m_bsDefaultFontInfo.wsFontName = lf.lfFaceName;
	//m_bsDefaultFontInfo.iSize = -lf.lfHeight;
	//m_bsDefaultFontInfo.bBold = (lf.lfWeight >= FW_BOLD);
	//m_bsDefaultFontInfo.bUnderline = (lf.lfUnderline == TRUE);
	//m_bsDefaultFontInfo.bItalic = (lf.lfItalic == TRUE);
	//::ZeroMemory(&m_bsDefaultFontInfo.tm, sizeof(m_bsDefaultFontInfo.tm));

	startDelayedFreeControl();
}


drawManager::~drawManager(void)
{
	SEND_LOG(L"ERROR drawManager free ... begin");
	// 隐藏控件可见性，美化视觉效果
	for (LayerWndMapType::iterator iter = m_mapLayerWnd.begin(); iter != m_mapLayerWnd.end(); ++iter)
	{
		assert(iter->second.hWnd);
		::ShowWindow(iter->second.hWnd, SW_HIDE);
	}
	// 
	for (ControlArrayType::iterator iter = m_vecDelayedCleanup.begin(); iter != m_vecDelayedCleanup.end(); ++iter)
	{
		delete static_cast<controlUC*>(*iter);
	}
	m_vecDelayedCleanup.clear();

	//for (NotifyArrayType::iterator iter = m_vecNotifys.begin(); iter != m_vecNotifys.end(); ++iter)
	//{
	//	delete *iter;
	//}
	m_vecNotifys.clear();
	//removeAllOptionGroups();	// 内部是静态的
	removeAllTimers();
	delete m_pRoot;

	//::DeleteObject(m_bsDefaultFontInfo.hFont);
	if(m_hwndTooltip) ::DestroyWindow(m_hwndTooltip);
	if( m_hdcOffscreen != NULL ) ::DeleteDC(m_hdcOffscreen);
	if( m_hdcBackground != NULL ) ::DeleteDC(m_hdcBackground);
	if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
	if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
	if(m_hdcDrawer) ReleaseDC(m_hwndDrawer, m_hdcDrawer);

	stopDelayedFreeControl();

	SEND_LOG(L"ERROR drawManager free ... end");
}


HINSTANCE drawManager::getInstance()
{
	return mc_hInstance;
}

void drawManager::setInstance(HINSTANCE hInst)
{
	mc_hInstance = hInst;
	
}
gtc::resourceManager& drawManager::getResource()
{
	return mc_resManager;
}
void drawManager::init(HWND hWnd)
{
	assert(::IsWindow(hWnd));
	// Remember the window context we came from
	m_hwndDrawer = hWnd;
	m_hdcDrawer = ::GetDC(hWnd);
}

void drawManager::needUpdate()
{
	m_bUpdateNeeded = true;
}
void drawManager::invalidate(RECT& rcItem)
{
	if(m_bBesidesArea && !isEmpty(m_rcBesidesArea))
	{
		RECT rc;
		RECT rcSect = {0};
		if (::IntersectRect(&rcSect, &rcItem, &m_rcBesidesArea))
		{
			// left
			if(rcItem.left < m_rcBesidesArea.left) 
			{
				rc = rcItem;
				rc.right = rcSect.left;
				::InvalidateRect(m_hwndDrawer, &rc, FALSE);
			}
			// top
			if (rcItem.top < m_rcBesidesArea.top)
			{
				rc = rcItem;
				rc.bottom = rcSect.top;
				rc.left = rcSect.left;
				rc.right = rcSect.right;
				::InvalidateRect(m_hwndDrawer, &rc, FALSE);
			}
			// right
			if (rcItem.right > m_rcBesidesArea.right)
			{
				rc = rcItem;
				rc.left = rcSect.right;
				::InvalidateRect(m_hwndDrawer, &rc, FALSE);
			}
			// bottom
			if (rcItem.bottom > m_rcBesidesArea.bottom)
			{
				rc = rcItem;
				rc.top = rcSect.bottom;
				rc.left = rcSect.left;
				rc.right = rcSect.right;
				::InvalidateRect(m_hwndDrawer, &rc, FALSE);
			}
			//SEND_LOG(L"drawManager::invalidate 与排除区域有交集 交集:<%s> rc:<%s>", gtc::format::rect(rc, true).c_str(), gtc::format::rect(rcItem, true).c_str());
			return;
		}
	}
	//SEND_LOG(L"drawManager::invalidate area:<%s>", gtc::format::rect(rcItem, true).c_str());
	::InvalidateRect(m_hwndDrawer, &rcItem, /*m_bAlphaLayerWindow ? TRUE :*/ FALSE);
}

HWND drawManager::getPaintWindow() const
{
	return m_hwndDrawer;
}

HDC  drawManager::getPaintDC() const
{
	return m_hdcDrawer;
}
HWND drawManager::getTooltipWindow() const
{
	return m_hwndTooltip;
}
bool drawManager::attachDialog(controlUC* pControl)
{
	assert(::IsWindow(m_hwndDrawer));
	// Reset any previous attachment
	SetFocus(NULL);
	m_pEventKey = nullptr;
	m_pEventHover = nullptr;
	m_pEventClick = nullptr;
	m_pEventWheel = nullptr;
	// Remove the existing control-tree. We might have gotten inside this function as
	// a result of an event fired or similar, so we cannot just delete the objects and
	// pull the internal memory of the calling code. We'll delay the cleanup.
	if( m_pRoot ) {
		//m_aPostPaintControls.Empty();
		addDelayedCleanup(m_pRoot);
	}
	// Set the dialog root element
	m_pRoot = pControl;
	// Go ahead...
	m_bUpdateNeeded = true;
	m_bFirstLayout = true;
	m_bFocusNeeded = true;
	// Initiate all control
	return initControls(pControl);
}
bool drawManager::initControls(controlUC* pControl, controlUC* pParent /*= NULL*/)
{
	assert(pControl);
	if( pControl == NULL ) return false;
	pControl->setManager(this, pParent != NULL ? pParent : pControl->getParent(), true);
	pControl->findControl(__findControlFromNameHash, this, UCFIND_ALL);
	return true;
}

void drawManager::reapObject(controlUC* pControl)
{
	if(pControl == m_pEventClick) m_pEventClick = nullptr;
	if(pControl == m_pEventHover) m_pEventHover = nullptr;
	if(pControl == m_pEventKey) m_pEventKey = nullptr;
	if(pControl == m_pEventWheel) m_pEventWheel = nullptr;
	if(pControl == m_pFocus) m_pFocus = nullptr;
	killTimer(pControl);

	const std::wstring &wsName = pControl->getName();
	if (!wsName.empty())
	{
		ControlMapType::const_iterator itfind = m_mapNameControls.find(wsName);
		if(itfind != m_mapNameControls.cend()) m_mapNameControls.erase(itfind);
	}

	//for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) {
	//	TNotifyUI* pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
	//	if( pMsg->pSender == pControl ) pMsg->pSender = NULL;
	//}
}
void drawManager::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("size")) == 0 ) {
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);	assert(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);	assert(pstr); 
		setInitSize(cx, cy);
	} 
	else if( _tcscmp(pstrName, _T("sizebox")) == 0 ) {
		RECT rcSizeBox = { 0 };
		LPTSTR pstr = NULL;
		rcSizeBox.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setSizeBox(rcSizeBox);
	}
	else if( _tcscmp(pstrName, _T("caption")) == 0 ) {
		RECT rcCaption = { 0 };
		LPTSTR pstr = NULL;
		rcCaption.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setCaptionRect(rcCaption);
	}
	else if( _tcscmp(pstrName, _T("roundcorner")) == 0 ) {
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr); 
		setRoundCorner(cx, cy);
	} 
	else if( _tcscmp(pstrName, _T("mininfo")) == 0 ) {
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr); 
		setMinInfo(cx, cy);
	}
	else if( _tcscmp(pstrName, _T("maxinfo")) == 0 ) {
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr); 
		setMaxInfo(cx, cy);
	}
	//else if( _tcscmp(pstrName, _T("showdirty")) == 0 ) {
	//	setShowUpdateRect(_tcscmp(pstrValue, _T("true")) == 0);
	//} 
	else if( _tcscmp(pstrName, _T("alpha")) == 0 ) {
		setTransparent(_ttoi(pstrValue));
	} 
	else if( _tcscmp(pstrName, _T("bktrans")) == 0 ) {
		setBackgroundTransparent(_tcscmp(pstrValue, _T("true")) == 0);
	} 
	else if( _tcscmp(pstrName, _T("disabledfontcolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setDefaultDisabledColor(clrColor);
	} 
	else if( _tcscmp(pstrName, _T("defaultfontcolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setDefaultFontColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("linkfontcolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setDefaultLinkFontColor(clrColor);
	} 
	else if( _tcscmp(pstrName, _T("linkhoverfontcolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setDefaultLinkHoverFontColor(clrColor);
	} 
	else if( _tcscmp(pstrName, _T("selectedcolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setDefaultSelectedBkColor(clrColor);
	} 
}

void drawManager::clientToScreen(RECT &rect)
{
	POINT pt = {rect.left, rect.top};
	::ClientToScreen(m_hwndDrawer, &pt);
	rect.left = pt.x;
	rect.top = pt.y;

	pt.x = rect.right;
	pt.y = rect.bottom;
	::ClientToScreen(m_hwndDrawer, &pt);
	rect.right = pt.x;
	rect.bottom = pt.y;
}
void drawManager::clientToScreen(POINT &pt)
{
	::ClientToScreen(m_hwndDrawer, &pt);
}
void drawManager::screenToClient(RECT &rect)
{
	POINT pt = {rect.left, rect.top};
	::ScreenToClient(m_hwndDrawer, &pt);
	rect.left = pt.x;
	rect.top = pt.y;

	pt.x = rect.right;
	pt.y = rect.bottom;
	::ScreenToClient(m_hwndDrawer, &pt);
	rect.right = pt.x;
	rect.bottom = pt.y;

}
void drawManager::screenToClient(POINT &pt)
{
	::ScreenToClient(m_hwndDrawer, &pt);
}

POINT drawManager::getMousePos() const
{
	return m_ptLastMousePos;
}
SIZE drawManager::getClientSize() const
{
	RECT rcClient = { 0 };
	::GetClientRect(m_hwndDrawer, &rcClient);
	SIZE sz = {rcClient.right - rcClient.left, rcClient.bottom - rcClient.top};
	return sz;
}

SIZE drawManager::getInitSize()
{
	return m_szInitWindowSize;
}
void drawManager::setInitSize(int cx, int cy)
{
	m_szInitWindowSize.cx = cx;
	m_szInitWindowSize.cy = cy;
	if( m_pRoot == NULL && m_hwndDrawer != NULL ) {
		::SetWindowPos(m_hwndDrawer, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}
RECT& drawManager::getSizeBox()
{
	return m_rcSizeBox;
}
void drawManager::setSizeBox(RECT& rcSizeBox)
{
	m_rcSizeBox = rcSizeBox;
}
RECT& drawManager::getCaptionRect()
{
	return m_rcCaption;
}
void drawManager::setCaptionRect(LONG height)
{
	RECT rc = {0,0,0,height};
	setCaptionRect(rc);
}
void drawManager::setCaptionRect(RECT& rcCaption)
{
	m_rcCaption = rcCaption;
}
SIZE drawManager::getRoundCorner() const
{
	return m_szRoundCorner;
}
void drawManager::setRoundCorner(int cx, int cy)
{
	m_szRoundCorner.cx = cx;
	m_szRoundCorner.cy = cy;
}

SIZE drawManager::getMinInfo() const
{
	return m_szMinWindow;
}
void drawManager::setMinInfo(int cx, int cy)
{
	assert(cx>=0 && cy>=0);
	m_szMinWindow.cx = cx;
	m_szMinWindow.cy = cy;
}
SIZE drawManager::getMaxInfo() const
{
	return m_szMaxWindow;
}
void drawManager::setMaxInfo(int cx, int cy)
{
	assert(cx>=0 && cy>=0);
	m_szMaxWindow.cx = cx;
	m_szMaxWindow.cy = cy;
}

void drawManager::setTransparent(int nOpacity)
{
	if( m_hwndDrawer != NULL ) {
		typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
		PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes;

		HMODULE hUser32 = ::GetModuleHandle(L"User32.dll");
		if (hUser32)
		{
			fSetLayeredWindowAttributes = 
				(PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
			if( fSetLayeredWindowAttributes == NULL ) return;
		}

		DWORD dwStyle = ::GetWindowLong(m_hwndDrawer, GWL_EXSTYLE);
		DWORD dwNewStyle = dwStyle;
		if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
		else dwNewStyle &= ~WS_EX_LAYERED;
		if(dwStyle != dwNewStyle) ::SetWindowLong(m_hwndDrawer, GWL_EXSTYLE, dwNewStyle);
		fSetLayeredWindowAttributes(m_hwndDrawer, 0, nOpacity, LWA_ALPHA);
	}
}
void drawManager::setBackgroundTransparent(bool bTrans)
{
	m_bAlphaBackground = bTrans;
}
bool drawManager::isBackgroundTransparent() const
{
	return m_bAlphaBackground;
}
void drawManager::setLayerWindowTransparent(bool bTrans)
{
	m_bAlphaLayerWindow = bTrans;

	if (m_hwndDrawer)
	{
		DWORD dwStyle = ::GetWindowLong(m_hwndDrawer, GWL_EXSTYLE);
		DWORD dwNewStyle = dwStyle;
		if( m_bAlphaLayerWindow ) dwNewStyle |= WS_EX_LAYERED;
		else dwNewStyle &= ~WS_EX_LAYERED;
		if(dwStyle != dwNewStyle) ::SetWindowLong(m_hwndDrawer, GWL_EXSTYLE, dwNewStyle);
		
		//InvalidateRect(m_hwndDrawer, NULL, false);
		//if(m_pRoot) m_pRoot->needUpdate();
	}

}
bool drawManager::isLayerWindowTransparent() const
{
	return m_bAlphaLayerWindow;
}
//void drawManager::setBackgroupImage(PCWSTR imgKeyName)
//{
//	//m_wsBkImage = imgKeyName ? imgKeyName : L"";
//	m_pImageBk = findImage(imgKeyName);
//	assert(m_pImageBk);
//}
DWORD drawManager::getDefaultDisabledColor() const
{
	return m_dwDefaultDisabledColor;
}
void drawManager::setDefaultDisabledColor(DWORD dwColor)
{
	m_dwDefaultDisabledColor = dwColor;
}
DWORD drawManager::getDefaultFontColor() const
{
	return m_dwDefaultFontColor;
}
void drawManager::setDefaultFontColor(DWORD dwColor)
{
	m_dwDefaultFontColor = dwColor;
}
DWORD drawManager::getDefaultLinkFontColor() const
{
	return m_dwDefaultLinkFontColor;
}
void drawManager::setDefaultLinkFontColor(DWORD dwColor)
{
	m_dwDefaultLinkFontColor = dwColor;
}
DWORD drawManager::getDefaultLinkHoverFontColor() const
{
	return m_dwDefaultLinkHoverFontColor;
}
void drawManager::setDefaultLinkHoverFontColor(DWORD dwColor)
{
	m_dwDefaultLinkHoverFontColor = dwColor;
}
DWORD drawManager::getDefaultSelectedBkColor() const
{
	return m_dwDefaultSelectedBkColor;
}
void drawManager::setDefaultSelectedBkColor(DWORD dwColor)
{
	m_dwDefaultSelectedBkColor = dwColor;
}
const BSFontInfoUC* drawManager::getDefaultFontInfo()
{
	return mc_resManager.findFontInfo(-1);
	//if( m_bsDefaultFontInfo.tm.tmHeight == 0 ) {
	//	HFONT hOldFont = (HFONT) ::SelectObject(m_hdcDrawer, m_bsDefaultFontInfo.hFont);
	//	::GetTextMetrics(m_hdcDrawer, &m_bsDefaultFontInfo.tm);
	//	::SelectObject(m_hdcDrawer, hOldFont);
	//}
	//return &m_bsDefaultFontInfo;
}
void drawManager::setDefaultFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	mc_resManager.setDefaultFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
	/*LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	std::wcscpy(lf.lfFaceName, pStrFontName);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if( bBold ) lf.lfWeight += FW_BOLD;
	if( bUnderline ) lf.lfUnderline = TRUE;
	if( bItalic ) lf.lfItalic = TRUE;
	HFONT hFont = ::CreateFontIndirect(&lf);
	if( hFont == NULL ) return;

	::DeleteObject(m_bsDefaultFontInfo.hFont);
	m_bsDefaultFontInfo.hFont = hFont;
	m_bsDefaultFontInfo.wsFontName = pStrFontName;
	m_bsDefaultFontInfo.iSize = nSize;
	m_bsDefaultFontInfo.bBold = bBold;
	m_bsDefaultFontInfo.bUnderline = bUnderline;
	m_bsDefaultFontInfo.bItalic = bItalic;
	::ZeroMemory(&m_bsDefaultFontInfo.tm, sizeof(m_bsDefaultFontInfo.tm));
	if( m_hdcDrawer ) {
		HFONT hOldFont = (HFONT) ::SelectObject(m_hdcDrawer, hFont);
		::GetTextMetrics(m_hdcDrawer, &m_bsDefaultFontInfo.tm);
		::SelectObject(m_hdcDrawer, hOldFont);
	}*/
}

gtc::image* drawManager::findImage(PCWSTR pKeyName)
{
	assert(pKeyName && std::wcslen(pKeyName) > 0);
	return mc_resManager.findImage(pKeyName);
}

const BSFontInfoUC* drawManager::findFont(int fontIndex)
{
	return mc_resManager.findFontInfo(fontIndex);
}

HFONT drawManager::getFont(int fontIndex)
{
	return mc_resManager.getFont(fontIndex);
}

HMENU drawManager::findMenu(PCWSTR pKeyName)
{
	assert(pKeyName && std::wcslen(pKeyName) > 0);
	return mc_resManager.findMenu(pKeyName);
}
PCWSTR drawManager::getDefaultAttributeList(PCWSTR pControlKey)
{
	assert(pControlKey && std::wcslen(pControlKey) > 0);
	return mc_resManager.findAttributeList(pControlKey);
}
void drawManager::setCapture()
{
	::SetCapture(m_hwndDrawer);
	m_bMouseCapture = true;
}
void drawManager::releaseCapture()
{
	::ReleaseCapture();
	m_bMouseCapture = false;
}
bool drawManager::isCaptured()
{
	return m_bMouseCapture;
}

bool drawManager::addOptionGroup(PCWSTR pstrGroupName, controlUC* pControl)
{
	assert(pstrGroupName);
	assert(pControl);
	if(!pstrGroupName || std::wcslen(pstrGroupName) == 0 || !pControl) return false;
	GroupMapType::iterator iter = mc_mapGroupControls.find(pstrGroupName);
	if (iter != mc_mapGroupControls.end())
	{
		for (ControlArrayType::const_iterator itopt = iter->second.cbegin(); itopt != iter->second.cend(); ++itopt)
		{
			if(*itopt == pControl) return false;
		}
		iter->second.push_back(pControl);
	}
	else
	{
		ControlArrayType options;
		options.push_back(pControl);
		mc_mapGroupControls[pstrGroupName] = options;
	}
	return true;
	//LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
	//if( lp ) {
	//	CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
	//	for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
	//		if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
	//			return false;
	//		}
	//	}
	//	aOptionGroup->Add(pControl);
	//}
	//else {
	//	CStdPtrArray* aOptionGroup = new CStdPtrArray(6);
	//	aOptionGroup->Add(pControl);
	//	m_mOptionGroup.Insert(pStrGroupName, aOptionGroup);
	//}
	//return true;
}
ControlArrayType drawManager::getOptionGroup(PCWSTR pstrGroupName)
{
	ControlArrayType vecOption;
	assert(pstrGroupName);
	if(!pstrGroupName || std::wcslen(pstrGroupName) == 0) return vecOption;
	GroupMapType::iterator iter = mc_mapGroupControls.find(pstrGroupName);
	if (iter != mc_mapGroupControls.end())
	{
		vecOption = iter->second;
	}
	return vecOption;
}
void drawManager::removeOptionGroup(PCWSTR pstrGroupName, controlUC* pControl)
{
	assert(pstrGroupName);
	assert(pControl);
	if(!pstrGroupName || std::wcslen(pstrGroupName) == 0 || !pControl) return ;
	GroupMapType::iterator iter = mc_mapGroupControls.find(pstrGroupName);
	if (iter != mc_mapGroupControls.end())
	{
		for (ControlArrayType::const_iterator itopt = iter->second.cbegin(); itopt != iter->second.cend(); ++itopt)
		{
			if(*itopt == pControl)
			{
				iter->second.erase(itopt);
				break;
			}
		}
		if (iter->second.empty())
		{
			mc_mapGroupControls.erase(iter);
		}

	}
	
	//LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
	//if( lp ) {
	//	CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
	//	if( aOptionGroup == NULL ) return;
	//	for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
	//		if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
	//			aOptionGroup->Remove(i);
	//			break;
	//		}
	//	}
	//	if( aOptionGroup->IsEmpty() ) {
	//		delete aOptionGroup;
	//		m_mOptionGroup.Remove(pStrGroupName);
	//	}
	//}
}
void drawManager::removeAllOptionGroups()
{
	mc_mapGroupControls.clear();
}
controlUC* drawManager::getFocus() const
{
	return m_pFocus;
}
void drawManager::setFocus(controlUC* pControl)
{
	// Paint manager window has focus?
	HWND hFocusWnd = ::GetFocus();
	if( hFocusWnd != m_hwndDrawer && pControl != m_pFocus ) ::SetFocus(m_hwndDrawer);
	// Already has focus?
	if( pControl == m_pFocus ) return;
	// Remove focus from old control
	if( m_pFocus != NULL ) 
	{
		BSEventUC event = { 0 };
		event.Type = BEUCEventKillFocus;
		event.pSender = pControl;
		event.dwTimestamp = ::GetTickCount();
		m_pFocus->Event(event);
		sendNotify(m_pFocus, L"killfocus");
		m_pFocus = NULL;
	}
	if( pControl == NULL ) return;
	// Set focus to new control
	if( pControl != NULL 
		&& pControl->getManager() == this 
		&& pControl->isVisible() 
		&& pControl->isEnabled() ) 
	{
		m_pFocus = pControl;
		BSEventUC event = { 0 };
		event.Type = BEUCEventSetFocus;
		event.pSender = pControl;
		event.dwTimestamp = ::GetTickCount();
		m_pFocus->Event(event);
		sendNotify(m_pFocus, L"setfocus");
		//SEND_LOG(L"ERROR drawManager::setFocus xxyyxx addr:%08X class:<%s> name:<%s>", pControl, pControl->getClass(), pControl->getName().c_str());
	}
}
void drawManager::setFocusNeeded(controlUC* pControl)
{
	::SetFocus(m_hwndDrawer);
	if( pControl == NULL ) return;
	if( m_pFocus != NULL ) {
		BSEventUC event = { 0 };
		event.Type = BEUCEventKillFocus;
		event.pSender = pControl;
		event.dwTimestamp = ::GetTickCount();
		m_pFocus->Event(event);
		sendNotify(m_pFocus, _T("killfocus"));
		m_pFocus = NULL;
	}
	BSFindTabInfo info = { 0 };
	info.pFocus = pControl;
	info.bForward = false;
	m_pFocus = m_pRoot->findControl(__findControlFromTab, &info, UCFIND_VISIBLE | UCFIND_ENABLED | UCFIND_ME_FIRST);
	m_bFocusNeeded = true;
	if( m_pRoot != NULL ) m_pRoot->needUpdate();
}

bool drawManager::setNextTabControl(bool bForward /*= true*/)
{
	// If we're in the process of restructuring the layout we can delay the
	// focus calulation until the next repaint.
	if( m_bUpdateNeeded && bForward ) {
		m_bFocusNeeded = true;
		//SEND_LOG(L"NOTE drawManager::setNextTabControl 测试刷新区域 vvvvvvvvv");
		updateWindowOutBesidesArea();
		//::InvalidateRect(m_hwndDrawer, NULL, FALSE);
		return true;
	}
	// Find next/previous tabbable control
	BSFindTabInfo info1 = { 0 };
	info1.pFocus = m_pFocus;
	info1.bForward = bForward;
	controlUC* pControl = m_pRoot->findControl(__findControlFromTab, &info1, UCFIND_VISIBLE | UCFIND_ENABLED | UCFIND_ME_FIRST);
	if( pControl == NULL ) {  
		if( bForward ) {
			// Wrap around
			BSFindTabInfo info2 = { 0 };
			info2.pFocus = bForward ? NULL : info1.pLast;
			info2.bForward = bForward;
			pControl = m_pRoot->findControl(__findControlFromTab, &info2, UCFIND_VISIBLE | UCFIND_ENABLED | UCFIND_ME_FIRST);
		}
		else {
			pControl = info1.pLast;
		}
	}
	if( pControl != NULL ) setFocus(pControl);
	m_bFocusNeeded = false;
	return true;
}

void drawManager::addDelayedCleanup(controlUC* pControl)
{
	assert(pControl);
	if(!pControl) return;
	pControl->setManager(this, NULL, false);
	//SEND_LOG(L"ERROR drawManager::addDelayedCleanup 延迟清除 class:<%s>%08X  ", pControl->getClass(), pControl);

	//SEND_LOG(L"ERROR drawManager::addDelayedCleanup ddddddelayxxxxx addr:%08X class:<%s> name:<%s>", pControl, pControl->getClass(), pControl->getName().c_str());
	//SEND_LOG(L"drawManager::addDelayedCleanup focus:%08X  hover:%08X  click:%08X  key:%08X", m_pFocus, m_pEventHover, m_pEventClick, m_pEventKey);
	//m_smDelayedCleanup.waitToWrite(L"drawManager::addDelayedCleanup");
	m_vecDelayedCleanup.push_back(pControl);
	//m_smDelayedCleanup.done(L"drawManager::addDelayedCleanup");
	if (pControl->existsControl(m_pFocus) || pControl == m_pFocus) m_pFocus = nullptr;
	if (pControl->existsControl(m_pEventHover) || pControl == m_pEventHover) m_pEventHover = nullptr;
	if (pControl->existsControl(m_pEventClick) || pControl == m_pEventClick) m_pEventClick = nullptr;
	if (pControl->existsControl(m_pEventKey) || pControl == m_pEventKey) m_pEventKey = nullptr;
	if (pControl->existsControl(m_pEventWheel) || pControl == m_pEventWheel) m_pEventWheel = nullptr;
	
	//::SendMessage(m_hwndDrawer, WM_APP + 1, 0, 0L);
}

void drawManager::addDelayedFree(controlUC* pControl)
{
	assert(pControl);
	if(!pControl) return;
	pControl->setManager(this, NULL, false);
	SEND_LOG(L"ERROR drawManager::addDelayedFree 延迟释放 class:<%s>%08X  ", pControl->getClass(), pControl);

	if (pControl->existsControl(m_pFocus) || pControl == m_pFocus) m_pFocus = nullptr;
	if (pControl->existsControl(m_pEventHover) || pControl == m_pEventHover) m_pEventHover = nullptr;
	if (pControl->existsControl(m_pEventClick) || pControl == m_pEventClick) m_pEventClick = nullptr;
	if (pControl->existsControl(m_pEventKey) || pControl == m_pEventKey) m_pEventKey = nullptr;
	if (pControl->existsControl(m_pEventWheel) || pControl == m_pEventWheel) m_pEventWheel = nullptr;

	bool bexists = false;
	{
		READ_LOCK_DLYFREE
		bexists = existsDelayedFree(pControl);
	}
	if (!bexists)
	{
		WRITE_LOCK_DLYFREE
		m_vecDelayedFree.push_back(pControl);
	}
	//m_csDelayedFree.enter(__FUNCTIONW__);
	//if (std::find(m_vecDelayedFree.cbegin(), m_vecDelayedFree.cend(), pControl) == m_vecDelayedFree.cend())
	//{
	//	m_vecDelayedFree.push_back(pControl);
	//}
	//m_csDelayedFree.leave(__FUNCTIONW__);
	assert(m_eventDelayedFree);
	if(m_eventDelayedFree) SetEvent(m_eventDelayedFree);

}
bool drawManager::existsDelayedFree(controlUC* pControl)
{
	READ_LOCK_DLYFREE
	return std::find(m_vecDelayedFree.cbegin(), m_vecDelayedFree.cend(), pControl) != m_vecDelayedFree.cend();
}
void drawManager::clearDelayedFree()
{
	WRITE_LOCK_DLYFREE
	for (ControlArrayType::iterator iter = m_vecDelayedFree.begin(); iter != m_vecDelayedFree.end(); ++iter)
	{
		SEND_LOG(L"ERROR drawManager::threadDelayedFreeControl 正在延迟释放控件 class：<%s> name：<%s>", (*iter)->getClass(), (*iter)->getName().c_str());
		delete *iter;
	}
	m_vecDelayedFree.clear();

}
bool drawManager::emptyDelayedFree()
{
	READ_LOCK_DLYFREE
	return m_vecDelayedFree.size() > 0;
}
void drawManager::addLayerWindowControl(controlUC *pOwner, HWND hWnd)
{
	assert(pOwner && hWnd);
	if(!pOwner || !hWnd) return;
	//LayerWndMapType::iterator iter = m_mapLayerWnd.find(pOwner);
	//if(iter != m_mapLayerWnd.end()) return ;
	m_mapLayerWnd[pOwner] = BSLayerWndAttribute(hWnd);
}
void drawManager::removeLayerWindowControl(controlUC *pOwner)
{
	assert(pOwner);
	if(!pOwner) return;
	LayerWndMapType::iterator iter = m_mapLayerWnd.find(pOwner);
	if (iter != m_mapLayerWnd.end())
	{
		m_mapLayerWnd.erase(iter);
	}
}

void drawManager::recordLayerWindowControlAttr()
{
	// 用户虚拟子窗口的显示状态与主窗口同步
	for (LayerWndMapType::iterator iter = m_mapLayerWnd.begin(); iter != m_mapLayerWnd.end(); ++iter)
	{
		// 属性 BSLayerWndAttribute::isVisiblePrev
		iter->second.isVisiblePrev = ::IsWindowVisible(iter->second.hWnd) == TRUE;
	}
}
void drawManager::restoreLayerWindowControlAttr()
{
	// 用户虚拟子窗口的显示状态与主窗口同步
	for (LayerWndMapType::iterator iter = m_mapLayerWnd.begin(); iter != m_mapLayerWnd.end(); ++iter)
	{
		// 属性 BSLayerWndAttribute::isVisiblePrev
		ShowWindow(iter->second.hWnd, iter->second.isVisiblePrev ? SW_SHOW : SW_HIDE);
		//::SetWindowPos(iter->second.hWnd, HWND_TOPMOST, 0, 0, 0, 0
		//	, SWP_NOMOVE|SWP_NOSIZE|(iter->second.isVisiblePrev ? SWP_SHOWWINDOW: SWP_HIDEWINDOW));
	}
	SEND_LOG(L"NOTE drawManager::restoreLayerWindowControlAttr 显示");
}
void drawManager::showLayerWindowControl(bool bShow /*= true*/)
{
	for (LayerWndMapType::iterator iter = m_mapLayerWnd.begin(); iter != m_mapLayerWnd.end(); ++iter)
	{
		ShowWindow(iter->second.hWnd, bShow ? SW_SHOW : SW_HIDE);
		//::SetWindowPos(iter->second.hWnd, m_hwndDrawer, 0, 0, 0, 0
		//	, SWP_NOMOVE|SWP_NOSIZE|(bShow ? SWP_SHOWWINDOW: SWP_HIDEWINDOW));
	}
	SEND_LOG(L"NOTE drawManager::showLayerWindowControl 显示");
}
void drawManager::startDelayedFreeControl()
{
	assert(!m_threadDelayedFree);
	m_threadDelayedFree = (HANDLE)_beginthreadex(NULL, 0, threadDelayedFreeControl, this, CREATE_SUSPENDED, NULL);
	if (m_threadDelayedFree)
	{
		m_eventDelayedFree = CreateEvent(NULL, FALSE, FALSE, NULL);
		InterlockedCompareExchange(&m_lDelayedFreeSwitch, 1, 0);
		ResumeThread(m_threadDelayedFree);
	}
	//m_eventDelayedFree = CreateEvent(NULL, FALSE, FALSE, NULL);
	//m_threadDelayedFree = gtc::thread::start(threadDelayedFreeControl, this, false, false, &m_lDelayedFreeSwitch, m_eventDelayedFree);
}
void drawManager::stopDelayedFreeControl()
{
	InterlockedCompareExchange(&m_lDelayedFreeSwitch, 0, 1);
	if(m_eventDelayedFree) SetEvent(m_eventDelayedFree);
	WaitForSingleObject(m_threadDelayedFree, INFINITE);
	CloseHandle(m_eventDelayedFree);
	CloseHandle(m_threadDelayedFree);
	m_eventDelayedFree = NULL;
	m_threadDelayedFree = NULL;


	//gtc::thread::waitfor(m_threadDelayedFree/*, 3000*/);
	//CloseHandle(m_eventDelayedFree);
	//CloseHandle(m_threadDelayedFree);
	//m_eventDelayedFree = NULL;
	//m_threadDelayedFree = NULL;

}

/*static*/ unsigned int WINAPI drawManager::threadDelayedFreeControl(void *pParam/*gtc::thread::_BSThreadInfo *pthr*/)
{
	drawManager *pManager = static_cast<drawManager *>(pParam);
	assert(pManager);
	SEND_LOG(L"WARNING drawManager::threadDelayedFreeControl begin ...");
	while (pManager->m_lDelayedFreeSwitch == 1) 
	{
		WaitForSingleObject(pManager->m_eventDelayedFree, INFINITE);
		
		if(pManager->m_lDelayedFreeSwitch == 1)
			Sleep(1000);
		else break;
		pManager->clearDelayedFree();
		if (!pManager->emptyDelayedFree())
		{
			Sleep(100);
			SetEvent(pManager->m_eventDelayedFree);		// 再试一次
		}

	}

	//SEND_LOG(L"WARNING drawManager::threadDelayedFreeControl end ...");

	return 0;
	//////////////////////////////////////////////////////////////////////////////////////////
	// 2016-1-6		kp007	
	//assert(pthr && pthr->pParameter);
	//drawManager *pManager = static_cast<drawManager *>(pthr->pParameter);
	//assert(pManager);
	//SEND_LOG(L"WARNING drawManager::threadDelayedFreeControl begin ...");
	//do 
	//{
	//	if (pthr->hEvent)
	//	{
	//		WaitForSingleObject(pthr->hEvent, INFINITE);
	//	}
	//	if(pthr->plLooplock && *pthr->plLooplock == 1)
	//		Sleep(1000);
	//	pManager->clearDelayedFree();
	//	if (!pManager->emptyDelayedFree())
	//	{
	//		Sleep(100);
	//		SetEvent(pthr->hEvent);		// 再试一次
	//	}
	//	
	//} while (pthr->plLooplock && *pthr->plLooplock == 1);

	//SEND_LOG(L"WARNING drawManager::threadDelayedFreeControl end ...");

	//if(pthr->bAutofree) gtc::thread::free(pthr);
	//return 0;
}

controlUC* drawManager::getRoot() const
{
	assert(m_pRoot);
	return m_pRoot;
}
controlUC* drawManager::findControl(POINT pt) const
{
	assert(m_pRoot);
	return m_pRoot->findControl(__findControlFromPoint, &pt, UCFIND_VISIBLE | UCFIND_HITTEST | UCFIND_TOP_FIRST);
}
controlUC* drawManager::findControl(PCWSTR pstrName) const
{
	assert(m_pRoot);
	ControlMapType::const_iterator itfind = m_mapNameControls.find(pstrName);// std::find(m_mNameHash.begin(), m_mNameHash.end(), pstrName);
	if (itfind != m_mapNameControls.end())
	{
		return itfind->second;
	}
	return nullptr;
}
controlUC* drawManager::findSubControlByPoint(controlUC* pParent, POINT pt) const
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	return pParent->findControl(__findControlFromPoint, &pt, UCFIND_VISIBLE | UCFIND_HITTEST | UCFIND_TOP_FIRST);
}
controlUC* drawManager::findSubControlByName(controlUC* pParent, PCWSTR pstrName) const
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	return pParent->findControl(__findControlFromName, (LPVOID)pstrName, UCFIND_ALL);
}
controlUC* drawManager::findSubControlByClass(controlUC* pParent, PCWSTR pstrClass, int iIndex /*= 0*/)
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	//m_aFoundControls.resize(iIndex + 1);
	return pParent->findControl(__findControlFromClass, (LPVOID)pstrClass, UCFIND_ALL);
}
ControlArrayType& drawManager::findSubControlsByClass(controlUC* pParent, PCWSTR pstrClass)
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	m_vecFoundControls.clear();
	pParent->findControl(__findControlsFromClass, (LPVOID)pstrClass, UCFIND_ALL);
	return m_vecFoundControls;
}
ControlArrayType& drawManager::getSubControlsByClass()
{
	return m_vecFoundControls;
}

controlUC* CALLBACK drawManager::__findControlFromNameHash(controlUC* pThis, LPVOID pData)
{
	drawManager* pManager = static_cast<drawManager*>(pData);
	assert(pManager);
	const std::wstring& sName = pThis->getName();
	if( sName.empty() ) return NULL;
	// Add this control to the hash list
	pManager->m_mapNameControls[sName] = pThis;
	return NULL; // Attempt to add all controls
}
controlUC* CALLBACK drawManager::__findControlFromCount(controlUC* pThis, LPVOID pData)
{
	int* pnCount = static_cast<int*>(pData);
	(*pnCount)++;
	return NULL;  // Count all controls
}
controlUC* CALLBACK drawManager::__findControlFromPoint(controlUC* pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	return ::PtInRect(&pThis->getPosition(), *pPoint) ? pThis : NULL;
}
controlUC* CALLBACK drawManager::__findControlFromTab(controlUC* pThis, LPVOID pData)
{
	BSFindTabInfo* pInfo = static_cast<BSFindTabInfo*>(pData);
	if( pInfo->pFocus == pThis ) {
		if( pInfo->bForward ) pInfo->bNextIsIt = true;
		return pInfo->bForward ? NULL : pInfo->pLast;
	}
	if( (pThis->getControlFlags() & UCFLAG_TABSTOP) == 0 ) return NULL;
	pInfo->pLast = pThis;
	if( pInfo->bNextIsIt ) return pThis;
	if( pInfo->pFocus == NULL ) return pThis;
	return NULL;  // Examine all controls
}
controlUC* CALLBACK drawManager::__findControlFromShortcut(controlUC* pThis, LPVOID pData)
{
	if( !pThis->isVisible() ) return NULL; 
	BSFindShortCut* pFS = static_cast<BSFindShortCut*>(pData);
	if( pFS->ch == toupper(pThis->getShortcut()) ) pFS->bPickNext = true;
	if( _tcsstr(pThis->getClass(), L"LabelUC") != NULL ) return NULL;   // Labels never get focus!
	return pFS->bPickNext ? pThis : NULL;
}
controlUC* CALLBACK drawManager::__findControlFromUpdate(controlUC* pThis, LPVOID pData)
{
	return pThis->isUpdateNeeded() ? pThis : NULL;
}
controlUC* CALLBACK drawManager::__findControlFromName(controlUC* pThis, LPVOID pData)
{
	PCWSTR pstrName = static_cast<PCWSTR>(pData);
	const std::wstring& sName = pThis->getName();
	if( sName.empty() ) return NULL;
	return (_tcsicmp(sName.c_str(), pstrName) == 0) ? pThis : NULL;
}
controlUC* CALLBACK drawManager::__findControlFromClass(controlUC* pThis, LPVOID pData)
{
	PCWSTR pstrType = static_cast<PCWSTR>(pData);
	PCWSTR pType = pThis->getClass();
	ControlArrayType &refFoundControls = pThis->getManager()->getSubControlsByClass();
	if( _tcscmp(pstrType, L"*") == 0 || _tcscmp(pstrType, pType) == 0 ) {
		int iIndex = -1;
		while( refFoundControls.at(++iIndex) != NULL ) ;
		if( iIndex < (int)refFoundControls.size() ) refFoundControls[iIndex] = pThis;// pFoundControls->setAt(iIndex, pThis);
	}
	if( refFoundControls.at(refFoundControls.size() - 1) != NULL ) return pThis; 
	return NULL;
}
controlUC* CALLBACK drawManager::__findControlsFromClass(controlUC* pThis, LPVOID pData)
{
	PCWSTR pstrType = static_cast<PCWSTR>(pData);
	PCWSTR pType = pThis->getClass();
	if( _tcscmp(pstrType, L"*") == 0 || _tcscmp(pstrType, pType) == 0 ) 
		pThis->getManager()->getSubControlsByClass().push_back(pThis);//Add((LPVOID)pThis);
	return NULL;
}

bool drawManager::messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{
	static std::wstring c_wsOldToolTip;		// 消除气泡闪烁


	for( int i = 0; i < (int)m_vecMessageFilter.size(); i++ ) 
	{
		bool bHandled = false;
		LRESULT lResult = static_cast<IMessageFilterUC*>(m_vecMessageFilter[i])->messageHandler(uMsg, wParam, lParam, bHandled);
		if( bHandled ) {
			lRes = lResult;
			return true;
		}
	}

	switch(uMsg)
	{
	case WM_APP+1:
		{
			int idx = 0;
			//while(m_vecDelayedCleanup.size() > 0)
			//{
			//	++idx;
			//	delete static_cast<controlUC*>(m_vecDelayedCleanup.back());
			//	m_vecDelayedCleanup.erase(m_vecDelayedCleanup.end()-1);
			//}
			//m_smDelayedCleanup.waitToWrite(L"drawManager::messageHandler APP+1");
			for (ControlArrayType::iterator iter = m_vecDelayedCleanup.begin(); iter != m_vecDelayedCleanup.end(); ++iter)
			{
				++idx;
				delete static_cast<controlUC*>(*iter);
			}
			m_vecDelayedCleanup.clear();
			//m_smDelayedCleanup.done(L"drawManager::messageHandler APP+1");
		}
		break;
	//case WM_ACTIVATE:
	//	{
	//		// 首次刷新WS_EX_LAYERED窗口，否则无法显示
	//		//SEND_LOG(L"WARNING drawManager::messageHandler WM_ACTIVATE flag:%08X me:%08X hWnd:%08X", wParam, m_hwndDrawer, lParam);
	//		if(!m_bAlphaLayerWindow || m_mapLayerWnd.size() == 0) break;
	//		//bool bTopMost = (GetWindowLong(m_hwndDrawer, GWL_EXSTYLE) & WS_EX_TOPMOST) == WS_EX_TOPMOST;
	//		WORD flag = LOWORD(wParam);
	//		HWND hCtrlVisible = NULL;
	//		if(flag == WA_ACTIVE)
	//		{
	//			SEND_LOG(L"WARNING drawManager::messageHandler WA_ACTIVE hwnd:%08X", m_hwndDrawer);
	//			InvalidateRect(m_hwndDrawer, NULL, TRUE);
	//			for (LayerWndMapType::const_iterator iter = m_mapLayerWnd.cbegin(); iter != m_mapLayerWnd.cend(); ++iter)
	//			{
	//				hCtrlVisible = iter->second.hWnd;
	//				if (hCtrlVisible && ::IsWindowVisible(hCtrlVisible))
	//				{
	//					SetWindowPos(m_hwndDrawer, hCtrlVisible, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//					SetWindowPos(hCtrlVisible, HWND_TOP, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//					//PostMessage(m_hwndDrawer, WM_ACTIVATE, WA_ACTIVE, NULL);	// 必须，否则无法显示hCtrlVisible
	//					//break;
	//				}
	//			}
	//		}
	//		else if (flag == WA_CLICKACTIVE)
	//		{
	//			SEND_LOG(L"WARNING drawManager::messageHandler WA_CLICKACTIVE hwnd:%08X", m_hwndDrawer);
	//			for (LayerWndMapType::const_iterator iter = m_mapLayerWnd.cbegin(); iter != m_mapLayerWnd.cend(); ++iter)
	//			{
	//				hCtrlVisible = iter->second.hWnd;
	//				if (hCtrlVisible && ::IsWindowVisible(hCtrlVisible))
	//				{
	//					SetWindowPos(m_hwndDrawer, hCtrlVisible, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//					SetWindowPos(hCtrlVisible, HWND_TOP, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//					//break;

	//					//LONG styleEx = GetWindowLong(hCtrlVisible, GWL_EXSTYLE);
	//					//styleEx &= ~WS_EX_TOPMOST;
	//					//styleEx = SetWindowLong(hCtrlVisible, GWL_EXSTYLE, styleEx);
	//					//styleEx = 1;
	//				}
	//			}
	//			PostMessage(m_hwndDrawer, WM_ACTIVATE, WA_ACTIVE, NULL);	// 必须，否则无法显示hCtrlVisible
	//			//SetActiveWindow(m_hwndDrawer);
	//		}
	//		else if (flag == WA_INACTIVE)
	//		{
	//			SEND_LOG(L"WARNING drawManager::messageHandler WA_INACTIVE hwnd:%08X", m_hwndDrawer);
	//			if(!IsWindowVisible(m_hwndDrawer)) break;
	//			HWND hOtherActive = (HWND)lParam;
	//			if(!hOtherActive) hOtherActive = GetDesktopWindow();
	//			for (LayerWndMapType::const_iterator iter = m_mapLayerWnd.cbegin(); iter != m_mapLayerWnd.cend(); ++iter)
	//			{
	//				hCtrlVisible = iter->second.hWnd;
	//				if (hCtrlVisible && ::IsWindowVisible(hCtrlVisible))
	//				{
	//					SetWindowPos(hCtrlVisible, hOtherActive, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//					SetWindowPos(m_hwndDrawer, hCtrlVisible, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//					SetWindowPos(hCtrlVisible, HWND_TOP, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//					//PostMessage(m_hwndDrawer, WM_ACTIVATE, WA_ACTIVE, NULL);	// 必须，否则无法显示hCtrlVisible // 注释掉,否则无法显示edit、bubbleWnd等控件
	//					//break;
	//				}
	//			}
	//		}

	//	}
	//	break;
	case WM_MOVE:
		{
			// 对WS_EX_LAYERED窗口要一起移动子控件
			if (m_bAlphaLayerWindow)
			{
				RECT rcPos = {0};
				HWND hWnd = NULL;
				for (LayerWndMapType::const_iterator iter = m_mapLayerWnd.cbegin(); iter != m_mapLayerWnd.cend(); ++iter)
				{
					rcPos = iter->first->getPosition();
					hWnd = iter->second.hWnd;
					clientToScreen(rcPos);
					::SetWindowPos(hWnd, NULL, rcPos.left, rcPos.top, rectWidth(rcPos), rectHeight(rcPos), SWP_NOSIZE| SWP_NOZORDER | SWP_NOACTIVATE);
					//::SetWindowPos(m_hwndDrawer, hWnd, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE/*|SWP_NOACTIVATE*/);
				}
			}
		}
		break;
	case WM_WINDOWPOSCHANGING/*+10000*/:
		{
			// 对WS_EX_LAYERED窗口在可见子控件下
			if(!m_bAlphaLayerWindow) break;
			WINDOWPOS *pwinpos = static_cast<WINDOWPOS *>((void*)lParam);
			if(!pwinpos) break;

			//SEND_LOG(L"ERROR drawManager::messageHandler WM_WINDOWPOSCHANGING me:%08X(%08X) pos:%d,%d size:%d,%d flag:%04X after:%08X"
			//	, m_hwndDrawer, pwinpos->hwnd, pwinpos->x, pwinpos->y, pwinpos->cx, pwinpos->cy, pwinpos->flags, pwinpos->hwndInsertAfter);
			//
			//if (pwinpos->hwndInsertAfter && pwinpos->hwndInsertAfter != m_hwndDrawer)
			//{
			//	bool isctrl = false;
			//	for (LayerWndMapType::const_iterator iter = m_mapLayerWnd.cbegin(); iter != m_mapLayerWnd.cend(); ++iter)
			//	{
			//		if (iter->second.hWnd == pwinpos->hwndInsertAfter)
			//		{
			//			isctrl = true;
			//			break;
			//		}
			//	}
			//	if (!isctrl)
			//	{
			//		pwinpos->flags |= SWP_NOZORDER;
			//		SEND_LOG(L"ERROR drawManager::messageHandler WM_WINDOWPOSCHANGING  未知");
			//		return true;
			//	}
			//}
			if (pwinpos && pwinpos->hwnd == m_hwndDrawer)
			{
				for (LayerWndMapType::const_iterator iter = m_mapLayerWnd.cbegin(); iter != m_mapLayerWnd.cend(); ++iter)
				{
					if (::IsWindowVisible(iter->second.hWnd))
					{
						//SEND_LOG(L"NOTE drawManager::messageHandler WM_WINDOWPOSCHANGING");
						//pwinpos->flags &= ~SWP_NOACTIVATE;
						pwinpos->hwndInsertAfter = iter->second.hWnd;
						//break;
					}
				}


			}
			//else
			//	pwinpos->hwndInsertAfter = HWND_TOP;
		}
		break;
	case WM_CLOSE:
		{
			stopDelayedFreeControl();
			// Make sure all matching "closing" events are sent
			BSEventUC event = { 0 };
			event.ptMouse = m_ptLastMousePos;
			event.dwTimestamp = ::GetTickCount();
			if( m_pEventHover != NULL ) {
				event.Type = BEUCEventMouseLeave;
				event.pSender = m_pEventHover;
				m_pEventHover->Event(event);
			}
			if( m_pEventClick != NULL ) {
				event.Type = BEUCEventButtonUp;
				event.pSender = m_pEventClick;
				m_pEventClick->Event(event);
				m_pEventClick = nullptr;
			}

			SetFocus(NULL);

			// Hmmph, the usual Windows tricks to avoid
			// focus loss...
			HWND hwndParent = GetWindowOwner(m_hwndDrawer);
			if( hwndParent != NULL ) ::SetFocus(hwndParent);
		}
		break;
	case WM_ERASEBKGND:
		{
			//lRes = 1;
			//RECT rc = {0};
			//GetWindowRect(m_hwndDrawer, &rc);
			//RECT rcRoot = m_pRoot->getPosition();
			//SEND_LOG(L"NOTE drawManager::messageHandler WM_ERASEBKGND 当前绘图管理器 wp:%08X lp:%08X pos:<%s> root:<%s>", wParam, lParam, gtc::format::rect(rc, true).c_str(), gtc::format::rect(rcRoot, true).c_str());
			return true;
		}
		break;
	case WM_PAINT:
		{
			//SEND_LOG(L"drawManager->WM_PAINT 0");
			// Should we paint?
			RECT rcPaint = { 0 };
			if( !::GetUpdateRect(m_hwndDrawer, &rcPaint, FALSE) ) return true;
			if( m_pRoot == NULL ) {
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(m_hwndDrawer, &ps);
				::EndPaint(m_hwndDrawer, &ps);
				return true;
			}            
			// Do we need to resize anything?
			// This is the time where we layout the controls on the form.
			// We delay this even from the WM_SIZE messages since resizing can be
			// a very expensize operation.
			if( m_bUpdateNeeded ) {
				m_bUpdateNeeded = false;
				RECT rcClient = { 0 };
				::GetClientRect(m_hwndDrawer, &rcClient);
				if( !::IsRectEmpty(&rcClient) ) {
					if( m_pRoot->isUpdateNeeded() ) {
						m_pRoot->setPosition(rcClient);
						if( m_hdcOffscreen != NULL ) ::DeleteDC(m_hdcOffscreen);
						if( m_hdcBackground != NULL ) ::DeleteDC(m_hdcBackground);
						if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
						if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
						m_hdcOffscreen = NULL;
						m_hdcBackground = NULL;
						m_hbmpOffscreen = NULL;
						m_hbmpBackground = NULL;
					}
					else {
						controlUC* pControl = NULL;
						while( pControl = m_pRoot->findControl(__findControlFromUpdate, NULL, UCFIND_VISIBLE | UCFIND_ME_FIRST) ) {
							pControl->setPosition( pControl->getPosition() );
						}
					}
					// We'll want to notify the window when it is first initialized
					// with the correct layout. The window form would take the time
					// to submit swipes/animations.
					if( m_bFirstLayout ) {
						m_bFirstLayout = false;
						sendNotify(m_pRoot, L"windowinit",  0, 0, false);
						SendMessage(m_hwndDrawer, WUC_MSG_WINDOW_INIT, 0, 0);
					}
				}
			}
			// Set focus to first control?
			if( m_bFocusNeeded ) {
				setNextTabControl();
			}

			if(m_bAlphaLayerWindow) 
			{
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(m_hwndDrawer, &ps);
				::EndPaint(m_hwndDrawer, &ps);
				if( m_bUpdateNeeded ) {
					::InvalidateRect(m_hwndDrawer, NULL, FALSE);
				}
				updateLayeredWindow();
				//SEND_LOG(L"NOTE ----- >>>> updateLayeredWindow");
				return true;
			}

				//
			// Render screen
			//
			// Prepare offscreen bitmap?
			if( m_bOffscreenPaint && m_hbmpOffscreen == NULL )
			{
				RECT rcClient = { 0 };
				::GetClientRect(m_hwndDrawer, &rcClient);
				m_hdcOffscreen = ::CreateCompatibleDC(m_hdcDrawer);
				m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hdcDrawer, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
				assert(m_hdcOffscreen);
				assert(m_hbmpOffscreen);
			}
			// Begin Windows paint
			PAINTSTRUCT ps = { 0 };
			::BeginPaint(m_hwndDrawer, &ps);
			if( m_bOffscreenPaint )
			{
				HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hdcOffscreen, m_hbmpOffscreen);
				int iSaveDC = ::SaveDC(m_hdcOffscreen);
				if( m_bAlphaBackground ) {
					
					if( m_hbmpBackground == NULL ) {
						RECT rcClient = { 0 };
						::GetClientRect(m_hwndDrawer, &rcClient);
						m_hdcBackground = ::CreateCompatibleDC(m_hdcDrawer);;
						m_hbmpBackground = ::CreateCompatibleBitmap(m_hdcDrawer, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
						assert(m_hdcBackground);
						assert(m_hbmpBackground);
						::SelectObject(m_hdcBackground, m_hbmpBackground);
						::BitBlt(m_hdcBackground, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
							ps.rcPaint.bottom - ps.rcPaint.top, ps.hdc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
					}
					else
						::SelectObject(m_hdcBackground, m_hbmpBackground);
					::BitBlt(m_hdcOffscreen, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
						ps.rcPaint.bottom - ps.rcPaint.top, m_hdcBackground, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
				}
				m_pRoot->paint(m_hdcOffscreen, ps.rcPaint);
				//for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
				//	CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
				//	pPostPaintControl->DoPostPaint(m_hDcOffscreen, ps.rcPaint);
				//}
				::RestoreDC(m_hdcOffscreen, iSaveDC);
				::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
					ps.rcPaint.bottom - ps.rcPaint.top, m_hdcOffscreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
				::SelectObject(m_hdcOffscreen, hOldBitmap);

				//if( m_bShowUpdateRect ) {
				//	HPEN hOldPen = (HPEN)::SelectObject(ps.hdc, m_hUpdateRectPen);
				//	::SelectObject(ps.hdc, ::GetStockObject(HOLLOW_BRUSH));
				//	::Rectangle(ps.hdc, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
				//	::SelectObject(ps.hdc, hOldPen);
				//}
			}
			else
			{
				// A standard paint job
				int iSaveDC = ::SaveDC(ps.hdc);
				m_pRoot->paint(ps.hdc, ps.rcPaint);
				::RestoreDC(ps.hdc, iSaveDC);
			}
			// All Done!
			::EndPaint(m_hwndDrawer, &ps);
		}
		// If any of the painting requested a resize again, we'll need
		// to invalidate the entire window once more.
		if( m_bUpdateNeeded ) {
			::InvalidateRect(m_hwndDrawer, NULL, FALSE);
		}


		//	//////////////////////////////////////////////////////////////////////////////////////////
		//	//	2015-8-11 	
		//	SEND_LOG(L"drawManager->WM_PAINT 0");
		//	
		//	// Should we paint?
		//	RECT rcPaint = { 0 };
		//	if( !::GetUpdateRect(m_hwndDrawer, &rcPaint, FALSE) ) 
		//	{
		//		SEND_LOG(L"drawManager->WM_PAINT 01");
		//		return true;
		//	}
		//	if( m_pRoot == NULL ) {
		//		PAINTSTRUCT ps = { 0 };
		//		::BeginPaint(m_hwndDrawer, &ps);
		//		::EndPaint(m_hwndDrawer, &ps);
		//		SEND_LOG(L"drawManager->WM_PAINT 02");
		//		return true;
		//	}
		//	if (m_bAlphaBackground /*|| m_bAlphaLayerWindow*/)
		//	{
		//		DWORD dwExStyle=GetWindowLong(m_hwndDrawer,GWL_EXSTYLE);
		//		if((dwExStyle&WS_EX_LAYERED)!=WS_EX_LAYERED)
		//			SetWindowLong(m_hwndDrawer,GWL_EXSTYLE,dwExStyle^WS_EX_LAYERED);

		//		RECT rcClient = { 0 };
		//		::GetClientRect(m_hwndDrawer, &rcClient);

		//		PAINTSTRUCT ps = { 0 };
		//		::BeginPaint(m_hwndDrawer, &ps);
		//		// Do we need to resize anything?
		//		// This is the time where we layout the controls on the form.
		//		// We delay this even from the WM_SIZE messages since resizing can be
		//		// a very expensize operation.
		//		if( m_bUpdateNeeded ) {
		//			m_bUpdateNeeded = false;
		//			RECT rcClient = { 0 };
		//			::GetClientRect(m_hwndDrawer, &rcClient);
		//			if( !::IsRectEmpty(&rcClient) ) {
		//				if( m_pRoot->isUpdateNeeded() ) {
		//					m_pRoot->setPosition(rcClient);
		//					if( m_hdcOffscreen != NULL ) ::DeleteDC(m_hdcOffscreen);
		//					if( m_hdcBackground != NULL ) ::DeleteDC(m_hdcBackground);
		//					if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
		//					if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
		//					m_hdcOffscreen = NULL;
		//					m_hdcBackground = NULL;
		//					m_hbmpOffscreen = NULL;
		//					m_hbmpBackground = NULL;
		//				}
		//				else {
		//					controlUC* pControl = NULL;
		//					while( pControl = m_pRoot->findControl(__findControlFromUpdate, NULL, UCFIND_VISIBLE | UCFIND_ME_FIRST) ) {
		//						pControl->setPosition( pControl->getPosition() );
		//					}
		//				}
		//				// We'll want to notify the window when it is first initialized
		//				// with the correct layout. The window form would take the time
		//				// to submit swipes/animations.
		//				if( m_bFirstLayout ) {
		//					m_bFirstLayout = false;
		//					sendNotify(m_pRoot, L"windowinit",  0, 0, false);
		//					SendMessage(m_hwndDrawer, WUC_MSG_WINDOW_INIT, 0, 0);
		//				}
		//			}
		//		}
		//		// Set focus to first control?
		//		if( m_bFocusNeeded ) {
		//			setNextTabControl();
		//		}

		//		//updateLayeredWindow();

		//		int width = rcClient.right - rcClient.left;
		//		int height = rcClient.bottom - rcClient.top;
		//		if(/*m_bOffscreenPaint &&*/ m_hbmpBackground == NULL)
		//		{
		//			m_hdcBackground = ::CreateCompatibleDC(m_hdcDrawer);
		//			//m_hbmpBackground = ::CreateCompatibleBitmap(m_hdcDrawer, width, height); 

		//			BITMAPINFO bmi;
		//			::ZeroMemory(&bmi, sizeof(BITMAPINFO));
		//			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		//			bmi.bmiHeader.biWidth = width;
		//			bmi.bmiHeader.biHeight = height;
		//			bmi.bmiHeader.biPlanes = 1;
		//			bmi.bmiHeader.biBitCount = 32;
		//			bmi.bmiHeader.biCompression = BI_RGB;
		//			bmi.bmiHeader.biSizeImage = width * height * 4;
		//			bmi.bmiHeader.biClrUsed = 0;   
		//			m_hbmpBackground = ::CreateDIBSection(m_hdcDrawer, &bmi, DIB_RGB_COLORS, 
		//				(void**)&m_pBmpBackgroundBits, NULL, 0);

		//			assert(m_hdcBackground);
		//			assert(m_hbmpBackground);
		//		}

		//		int mirrorTop = height - rcPaint.bottom;
		//		int mirrorBottom = height - rcPaint.top;
		//		int morrorLeft = rcPaint.left;
		//		int morrorRight = rcPaint.right;
		//		for(int i = mirrorTop; i < mirrorBottom; ++i)
		//		{
		//			for(int j = morrorLeft; j < morrorRight; ++j)
		//			{
		//				int x = (i*width + j)*4;
		//				m_pBmpBackgroundBits[x + 0] = 0;
		//				m_pBmpBackgroundBits[x + 1] = 0;
		//				m_pBmpBackgroundBits[x + 2] = 0;
		//				m_pBmpBackgroundBits[x + 3] = 0;
		//			}

		//		}
		//		::SelectObject(m_hdcBackground,m_hbmpBackground);
		//		//m_pRoot->DoPaint(m_hDcBackground, rcClient);
		//		m_pRoot->paint(m_hdcBackground, rcPaint);

		//		//由于GDI的绘制函数比如DrawText会导致透明层变为0，所以这里专门手动设置成255；
		//		//为了区分原本正常的透明部分和绘制后的文字部分，这里根据颜色值判断，
		//		//颜色值为纯黑色，认为是原本正常的透明层，否则认为是绘制后的文字部分
		//		//所以这里要求文字不能为纯黑色，在绘制文字等地方会做手脚，
		//		//如果进来的颜色值为纯黑色，就自动修改为1,1,1，接近纯黑色。
		//		for(int i = mirrorTop; i < mirrorBottom; ++i)
		//		{
		//			for(int j = morrorLeft; j < morrorRight; ++j)
		//			{
		//				int x = (i*width + j)*4;
		//				if (m_pBmpBackgroundBits[x + 3] == 0
		//					&& (m_pBmpBackgroundBits[x + 0] != 0 
		//					|| m_pBmpBackgroundBits[x + 1] !=0 
		//					|| m_pBmpBackgroundBits[x + 2] != 0))
		//				{
		//					m_pBmpBackgroundBits[x + 3] = 255;
		//				}
		//			}
		//		}

		//		RECT rcWnd={0};
		//		::GetWindowRect(m_hwndDrawer, &rcWnd);
		//		POINT pt = {rcWnd.left,rcWnd.top};
		//		SIZE szWindow = {rcClient.right - rcClient.left,rcClient.bottom - rcClient.top};
		//		POINT ptSrc = {0, 0};
		//		BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		//		::UpdateLayeredWindow(m_hwndDrawer,NULL,&pt,&szWindow,m_hdcBackground,&ptSrc,0,&blendPixelFunction,ULW_ALPHA);
		//		::EndPaint(m_hwndDrawer, &ps);
		//		return true;
		//	}
		//	// Do we need to resize anything?
		//	// This is the time where we layout the controls on the form.
		//	// We delay this even from the WM_SIZE messages since resizing can be
		//	// a very expensize operation.
		//	if( m_bUpdateNeeded ) {
		//		m_bUpdateNeeded = false;
		//		RECT rcClient = { 0 };
		//		::GetClientRect(m_hwndDrawer, &rcClient);
		//		if( !::IsRectEmpty(&rcClient) ) {
		//			if( m_pRoot->isUpdateNeeded() ) {
		//				m_pRoot->setPosition(rcClient);
		//				if( m_hdcOffscreen != NULL ) ::DeleteDC(m_hdcOffscreen);
		//				if( m_hdcBackground != NULL ) ::DeleteDC(m_hdcBackground);
		//				if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
		//				if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
		//				m_hdcOffscreen = NULL;
		//				m_hdcBackground = NULL;
		//				m_hbmpOffscreen = NULL;
		//				m_hbmpBackground = NULL;
		//			}
		//			else {
		//				controlUC* pControl = NULL;
		//				while( pControl = m_pRoot->findControl(__findControlFromUpdate, NULL, UCFIND_VISIBLE | UCFIND_ME_FIRST) ) {
		//					pControl->setPosition( pControl->getPosition() );
		//				}
		//			}
		//			// We'll want to notify the window when it is first initialized
		//			// with the correct layout. The window form would take the time
		//			// to submit swipes/animations.
		//			if( m_bFirstLayout ) {
		//				m_bFirstLayout = false;
		//				sendNotify(m_pRoot, L"windowinit",  0, 0, false);
		//				SendMessage(m_hwndDrawer, WUC_MSG_WINDOW_INIT, 0, 0);
		//			}
		//		}
		//	}
		//	// Set focus to first control?
		//	if( m_bFocusNeeded ) {
		//		setNextTabControl();
		//	}

		//				
		//	//
		//	// Render screen
		//	//
		//	// Prepare offscreen bitmap?
		//	if( m_hbmpOffscreen == NULL )
		//	{
		//		RECT rcClient = { 0 };
		//		::GetClientRect(m_hwndDrawer, &rcClient);
		//		m_hdcOffscreen = ::CreateCompatibleDC(m_hdcDrawer);
		//		m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hdcDrawer, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
		//		assert(m_hdcOffscreen);
		//		assert(m_hbmpOffscreen);
		//	}
		//	// Begin Windows paint
		//	PAINTSTRUCT ps = { 0 };
		//	::BeginPaint(m_hwndDrawer, &ps);
		//	//SEND_LOG(L"ERROR ----- rcUpdate:<%s> ps.rcPaint:<%s>", gtc::format::rect(rcPaint, true).c_str(), gtc::format::rect(ps.rcPaint, true).c_str());
		//	//if( m_bOffscreenPaint )
		//	{
		//		HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hdcOffscreen, m_hbmpOffscreen);
		//		int iSaveDC = ::SaveDC(m_hdcOffscreen);
		//		//if( m_bAlphaBackground ) {
		//		//	if( m_hbmpBackground == NULL ) {
		//		//		RECT rcClient = { 0 };
		//		//		::GetClientRect(m_hwndDrawer, &rcClient);
		//		//		m_hdcBackground = ::CreateCompatibleDC(m_hdcDrawer);;
		//		//		m_hbmpBackground = ::CreateCompatibleBitmap(m_hdcDrawer, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
		//		//		assert(m_hdcBackground);
		//		//		assert(m_hbmpBackground);
		//		//		::SelectObject(m_hdcBackground, m_hbmpBackground);
		//		//		::BitBlt(m_hdcBackground, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
		//		//			ps.rcPaint.bottom - ps.rcPaint.top, ps.hdc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
		//		//	}
		//		//	else
		//		//		::SelectObject(m_hdcBackground, m_hbmpBackground);
		//		//	::BitBlt(m_hdcOffscreen, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
		//		//		ps.rcPaint.bottom - ps.rcPaint.top, m_hdcBackground, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
		//		//}
		//		//
		//		//if (m_bAlphaLayerWindow)
		//		//{
		//		//	updateLayeredWindow();
		//		//}
		//		//else
		//		{
		//			m_pRoot->paint(m_hdcOffscreen, ps.rcPaint);
		//			//SEND_LOG(L"drawManager->WM_PAINT 1 hwnd:%08X", m_hwndDrawer);
		//			//for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
		//			//	controlUC* pPostPaintControl = m_aPostPaintControls[i];
		//			//	pPostPaintControl->DoPostPaint(m_hDcOffscreen, ps.rcPaint);
		//			//}
		//			::RestoreDC(m_hdcOffscreen, iSaveDC);
		//			::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
		//				ps.rcPaint.bottom - ps.rcPaint.top, m_hdcOffscreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
		//			::SelectObject(m_hdcOffscreen, hOldBitmap);


		//		}

		//		//if( m_bShowUpdateRect ) {
		//		//	HPEN hOldPen = (HPEN)::SelectObject(ps.hdc, m_hUpdateRectPen);
		//		//	::SelectObject(ps.hdc, ::GetStockObject(HOLLOW_BRUSH));
		//		//	::Rectangle(ps.hdc, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
		//		//	::SelectObject(ps.hdc, hOldPen);
		//		//}
		//	}
		//	//else
		//	//{
		//	//	// A standard paint job
		//	//	int iSaveDC = ::SaveDC(ps.hdc);
		//	//	m_pRoot->DoPaint(ps.hdc, ps.rcPaint);
		//	//	::RestoreDC(ps.hdc, iSaveDC);
		//	//}
		//	// All Done!
		//	::EndPaint(m_hwndDrawer, &ps);


		//	// If any of the painting requested a resize again, we'll need
		//	// to invalidate the entire window once more.
		//	if( m_bUpdateNeeded ) {
		//		//SEND_LOG(L"NOTE drawManager::messageHandler 测试刷新区域 vvvvvvvvv WM_PAINT");
		//		updateWindowOutBesidesArea();
		//		//::InvalidateRect(m_hwndDrawer, NULL, FALSE);
		//	}

		//}

		return true;
		break;
	case WM_PRINTCLIENT:
		{
			RECT rcClient;
			::GetClientRect(m_hwndDrawer, &rcClient);
			HDC hDC = (HDC) wParam;
			int save = ::SaveDC(hDC);
			m_pRoot->paint(hDC, rcClient);
			// Check for traversing children. The crux is that WM_PRINT will assume
			// that the DC is positioned at frame coordinates and will paint the child
			// control at the wrong position. We'll simulate the entire thing instead.
			if( (lParam & PRF_CHILDREN) != 0 ) {
				HWND hWndChild = ::GetWindow(m_hwndDrawer, GW_CHILD);
				while( hWndChild != NULL ) {
					RECT rcPos = { 0 };
					::GetWindowRect(hWndChild, &rcPos);
					::MapWindowPoints(HWND_DESKTOP, m_hwndDrawer, reinterpret_cast<LPPOINT>(&rcPos), 2);
					::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
					// NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
					//       since the latter will not print the nonclient correctly for
					//       EDIT controls.
					::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
					hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
				}
			}
			::RestoreDC(hDC, save);
		}
		break;
	case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
			if( m_szMinWindow.cx > 0 ) lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
			if( m_szMinWindow.cy > 0 ) lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
			if( m_szMaxWindow.cx > 0 ) lpMMI->ptMaxTrackSize.x = m_szMaxWindow.cx;
			if( m_szMaxWindow.cy > 0 ) lpMMI->ptMaxTrackSize.y = m_szMaxWindow.cy;
		}
		break;
	case WM_SIZE:
		{
			if( m_pFocus != NULL ) {
				BSEventUC event = { 0 };
				event.Type = BEUCEventWindowSize;
				event.pSender = m_pFocus;
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
			}
			if( m_pRoot != NULL ) m_pRoot->needUpdate();
		}
		return true;
		break;
	case WM_TIMER:
		{
			//SEND_LOG(L"ERROR WM_TIMER 1");
			for (std::vector<BSTimerInfo *>::iterator iter = m_vecTimers.begin(); iter != m_vecTimers.end(); ++iter)
			{
				if ((*iter)->hWnd == m_hwndDrawer && (*iter)->uWinTimer == LOWORD(wParam) && (*iter)->bKilled == false)
				{
					BSEventUC event = { 0 };
					event.Type = BEUCEventTimer;
					event.pSender = (*iter)->pSender;
					event.wParam = (*iter)->nLocalID;
					event.dwTimestamp = ::GetTickCount();
					(*iter)->pSender->Event(event);
					//SEND_LOG(L"ERROR WM_TIMER 2");
					break;
				}
			}
		}
		break;
	case WM_MOUSEHOVER:
		{
			m_bMouseTracking = false;
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			controlUC* pHover = findControl(pt);
			if( pHover == NULL ) break;
			// Generate mouse hover event
			if( m_pEventHover != NULL ) {
				BSEventUC event = { 0 };
				event.ptMouse = pt;
				event.Type = BEUCEventMouseHover;
				event.pSender = m_pEventHover;
				event.dwTimestamp = ::GetTickCount();
				m_pEventHover->Event(event);
			}
			// Create tooltip information
			std::wstring sToolTip = pHover->getToolTip();
			if( sToolTip.empty() )
			{
				c_wsOldToolTip.clear();
				if( m_hwndTooltip != NULL )
				{
					SEND_LOG(L"ERROR WM_MOUSEHOVER TTM_TRACKACTIVATE tip 消失");
					::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_tipToolTip);
				}
				return true;
			}
			::ZeroMemory(&m_tipToolTip, sizeof(TOOLINFO));
			m_tipToolTip.cbSize = sizeof(TOOLINFO);	// 
//#if _WIN32_WINNT>0x0500
//			m_tipToolTip.cbSize = sizeof(TOOLINFO)-sizeof(void*);
//#else
//			m_tipToolTip.cbSize = sizeof(TOOLINFO);
//#endif

			m_tipToolTip.uFlags = TTF_IDISHWND/*|TTF_SUBCLASS*/;
			m_tipToolTip.hwnd = m_hwndDrawer;
			m_tipToolTip.uId = (UINT_PTR) m_hwndDrawer;
			m_tipToolTip.hinst = mc_hInstance;
			m_tipToolTip.lpszText = const_cast<PWSTR>(sToolTip.c_str());
			m_tipToolTip.rect = pHover->getPosition();
			if( m_hwndTooltip == NULL ) {
				m_hwndTooltip = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hwndDrawer, NULL, mc_hInstance, NULL);
				if(!::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_tipToolTip))
				{
					assert(!L"创建气泡失败");
				}
			}
			SEND_LOG(L"NOTE WM_MOUSEHOVER ----> control:<%s> tip:<%s>", pHover->getName().c_str(), sToolTip.c_str());
			if(c_wsOldToolTip != sToolTip)
			{
				c_wsOldToolTip = sToolTip;
				::SendMessage(m_hwndTooltip, TTM_SETMAXTIPWIDTH, 0, estimateToolTipSize().cx);	// 显示最大宽度（像素），客户自动换行哦
				::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_tipToolTip);
				::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_tipToolTip);
				//SetWindowPos(m_hwndTooltip, HWND_TOPMOST, 0, 0, 100, 100, SWP_NOMOVE|/*SWP_NOSIZE|*/SWP_SHOWWINDOW/*|SWP_DRAWFRAME*/);
				
				SEND_LOG(L"WARNING WM_MOUSEHOVER tip 开启");
				//RECT rc = {0};
				//GetWindowRect(m_hwndTooltip, &rc);
				//SEND_LOG(L"WARNING WM_MOUSEHOVER tip 开启 pos:<%s> visible:%d", gtc::format::rect(rc, true).c_str(), IsWindowVisible(m_hwndTooltip));
			}
		}
		return true;
		break;
	case WM_MOUSELEAVE:
		{
			if( m_pEventHover != NULL ) {
				BSEventUC event = { 0 };
				GetCursorPos(&event.ptMouse);
				event.wParam = wParam;
				event.lParam = lParam;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				event.Type = BEUCEventMouseLeave;
				event.pSender = m_pEventHover;
				m_pEventHover->Event(event);
				m_pEventHover = NULL;
			}


			if( m_hwndTooltip != NULL )
			{
				SEND_LOG(L"ERROR WM_MOUSELEAVE TTM_TRACKACTIVATE tip 消失");
				c_wsOldToolTip.clear();
				::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_tipToolTip);
			}
			if( m_bMouseTracking ) ::SendMessage(m_hwndDrawer, WM_MOUSEMOVE, 0, (LPARAM) -1);
			m_bMouseTracking = false;
		}
		break;
	case WM_MOUSEMOVE:
		{
			// Start tracking this entire window again...
			if( !m_bMouseTracking ) {
				TRACKMOUSEEVENT tme = { 0 };
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_HOVER | TME_LEAVE;
				tme.hwndTrack = m_hwndDrawer;
				tme.dwHoverTime = m_hwndTooltip == NULL ? 1000UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
				_TrackMouseEvent(&tme);
				m_bMouseTracking = true;
			}
			// Generate the appropriate mouse messages
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			controlUC* pNewHover = findControl(pt);
			if( pNewHover != NULL && pNewHover->getManager() != this ) break;
			//if(pNewHover)
			//	SEND_LOG(L"ERROR WM_MOUSEMOVE addr:%08X pos:<%s> name:<%s> class:<%s>", pNewHover, gtc::format::rect(pNewHover->getPosition(), true).c_str(), pNewHover->getName().c_str(), pNewHover->getClass());
			BSEventUC event = { 0 };
			event.ptMouse = pt;
			event.wParam = wParam;
			event.lParam = lParam;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
				event.Type = BEUCEventMouseLeave;
				event.pSender = m_pEventHover;
				m_pEventHover->Event(event);
				m_pEventHover = NULL;
				if( m_hwndTooltip != NULL )
				{
					//SEND_LOG(L"ERROR WM_MOUSEMOVE TTM_TRACKACTIVATE tip 消失");
					c_wsOldToolTip.clear();
					::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_tipToolTip);
				}
			}
			if( pNewHover != m_pEventHover && pNewHover != NULL ) {
				//SEND_LOG(L"BEUCEventMouseEnter class:<%s>", pNewHover->getClass());
				event.Type = BEUCEventMouseEnter;
				event.pSender = pNewHover;
				pNewHover->Event(event);
				m_pEventHover = pNewHover;
			}
			if( m_pEventClick != NULL ) {
				event.Type = BEUCEventMouseMove;
				event.pSender = m_pEventClick;
				m_pEventClick->Event(event);
			}
			else if( pNewHover != NULL ) {
				event.Type = BEUCEventMouseMove;
				event.pSender = pNewHover;
				pNewHover->Event(event);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			// We alway set focus back to our app (this helps
			// when Win32 child windows are placed on the dialog
			// and we need to remove them on focus change).
			RECT rcWnd = {0}, rcClient = {0};
			GetWindowRect(m_hwndDrawer, &rcWnd);
			GetClientRect(m_hwndDrawer, &rcClient);
			clientToScreen(rcClient);
			::SetFocus(m_hwndDrawer);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			controlUC* pControl = findControl(pt);
			if( pControl == NULL ) break;
			if( pControl->getManager() != this ) break;
			m_pEventClick = pControl;
			pControl->setFocus();
			setCapture();
			BSEventUC event = { 0 };
			event.Type = BEUCEventButtonDown;
			event.pSender = pControl;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			pControl->Event(event);
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			::SetFocus(m_hwndDrawer);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			controlUC* pControl = findControl(pt);
			if( pControl == NULL ) break;
			if( pControl->getManager() != this ) break;
			setCapture();
			BSEventUC event = { 0 };
			event.Type = BEUCEventDBLClick;
			event.pSender = pControl;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			pControl->Event(event);
			m_pEventClick = pControl;
		}
		break;
	case WM_LBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			if( m_pEventClick == nullptr ) break;
			releaseCapture();
			BSEventUC event = { 0 };
			event.Type = BEUCEventButtonUp;
			event.pSender = m_pEventClick;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			if(m_pEventClick) m_pEventClick->Event(event);
			m_pEventClick = nullptr;
		}
		break;
	case WM_RBUTTONDOWN:
		{
			::SetFocus(m_hwndDrawer);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			controlUC* pControl = findControl(pt);
			if( pControl == NULL ) break;
			if( pControl->getManager() != this ) break;
			pControl->setFocus();
			setCapture();
			BSEventUC event = { 0 };
			event.Type = BEUCEventRButtonDown;
			event.pSender = pControl;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			pControl->Event(event);
			m_pEventClick = pControl;
		}
		break;
	case WM_CONTEXTMENU:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			screenToClient(pt);
			m_ptLastMousePos = pt;
			if( m_pEventClick == nullptr ) break;
			releaseCapture();
			BSEventUC event = { 0 };
			event.Type = BEUCEventContextMenu;
			event.pSender = m_pEventClick;
			event.ptMouse = pt;
			event.wKeyState = keyState();// wParam为接受消息的窗口句柄  (WORD)wParam;
			event.lParam = (LPARAM)m_pEventClick;
			event.dwTimestamp = ::GetTickCount();
			m_pEventClick->Event(event);
			m_pEventClick = nullptr;
		}
		break;
	case WM_MOUSEWHEEL:
		{
			//static controlUC *c_pControl = nullptr;
			static POINT c_ptOld = {0};
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			screenToClient(pt);
			//SEND_LOG(L"WM_MOUSEWHEEL last:<%d, %d> pt:<%d, %d>", c_ptOld.x, c_ptOld.y, pt.x, pt.y);
			controlUC* pControl = nullptr;
			if (c_ptOld == pt)
			{
				if(!m_pEventWheel)
					m_pEventWheel = pControl = findControl(pt);
				else pControl = m_pEventWheel;
			}
			else 
				m_pEventWheel = pControl = findControl(pt);
			c_ptOld = pt;
			m_ptLastMousePos = pt;
			//controlUC* pControl = findControl(pt);
			if( pControl == NULL ) break;
			if( pControl->getManager() != this ) break;
			int zDelta = (int) (short) HIWORD(wParam);
			BSEventUC event = { 0 };
			event.Type = BEUCEventScrollWheel;
			event.pSender = pControl;
			event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
			event.lParam = lParam;
			event.wKeyState = keyState();
			event.dwTimestamp = ::GetTickCount();
			event.ptMouse = pt;
			pControl->Event(event);

			// Let's make sure that the scroll item below the cursor is the same as before...
			::SendMessage(m_hwndDrawer, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
		}
		break;
	case WM_CHAR:
		{
			if( m_pFocus == NULL ) break;
			BSEventUC event = { 0 };
			event.Type = BEUCEventChar;
			event.chKey = (wchar_t)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = keyState();
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
		}
		break;
	case WM_KEYDOWN:
		{
			if( m_pFocus == NULL ) break;
			BSEventUC event = { 0 };
			event.Type = BEUCEventKeyDown;
			event.chKey = (wchar_t)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = keyState();
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			m_pEventKey = m_pFocus;
		}
		break;
	case WM_KEYUP:
		{
			if( m_pEventKey == NULL ) break;
			BSEventUC event = { 0 };
			event.Type = BEUCEventKeyUp;
			event.chKey = (wchar_t)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = keyState();
			event.dwTimestamp = ::GetTickCount();
			m_pEventKey->Event(event);
			m_pEventKey = NULL;
		}
		break;
	case WM_SETCURSOR:
		{
			if( LOWORD(lParam) != HTCLIENT )
			{	// 当点击到CAPTION区返回HTCAPTION，这时就会影响到其内嵌按钮的光标形状
				//SEND_LOG(L"WM_SETCURSOR break");
				break;
			}
			//SEND_LOG(L"WM_SETCURSOR begin");
			if( m_bMouseCapture ) return true;
			POINT pt = { 0 };
			::GetCursorPos(&pt);
			screenToClient(pt);
			controlUC* pControl = findControl(pt);
			if( pControl == NULL ) break;
			if( (pControl->getControlFlags() & UCFLAG_SETCURSOR) == 0 ) break;
			BSEventUC event = { 0 };
			event.Type = BEUCEventSetCursor;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = keyState();
			event.dwTimestamp = ::GetTickCount();
			lRes = 1;
			//SEND_LOG(L"WM_SETCURSOR class:<%s> name:<%s>", pControl->getClass(), pControl->getName().c_str());
			pControl->Event(event);
		}
		return true;
	case WM_NOTIFY:
		{
			LPNMHDR lpNMHDR = (LPNMHDR) lParam;
			if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
			return true;
		}
		break;
	case WM_COMMAND:
		{
			if( lParam == 0 ) break;
			HWND hWndChild = (HWND) lParam;
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			return true;
		}
		break;
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORSTATIC:
		{
			// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
			// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
			if( lParam == 0 ) break;
			HWND hWndChild = (HWND) lParam;
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}
void drawManager::updateLayeredWindow()
{
	//gtc::image *pimgBk = getResource().findImage(m_wsBkImage);
	//assert(m_pImageBk);
	//if(!m_pImageBk) return ;
	//SEND_LOG(L"drawManager->updateLayeredWindow %d,%d", pimgBk->getWidth(), pimgBk->getHeight());
	DWORD dwStyle = ::GetWindowLong(m_hwndDrawer, GWL_EXSTYLE);
	DWORD dwNewStyle = dwStyle;
	if( m_bAlphaLayerWindow ) dwNewStyle |= WS_EX_LAYERED;
	else dwNewStyle &= ~WS_EX_LAYERED;
	if(dwStyle != dwNewStyle) ::SetWindowLong(m_hwndDrawer, GWL_EXSTYLE, dwNewStyle);

	BLENDFUNCTION blend = {AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA};
	//m_Blend.BlendOp = AC_SRC_OVER; //theonlyBlendOpdefinedinWindows2000
	//m_Blend.BlendFlags = 0; //nothingelseisspecial...
	//m_Blend.AlphaFormat = AC_SRC_ALPHA; //...
	//m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA



	//RECT rcClient = { 0 };
	//::GetClientRect(m_hwndDrawer, &rcClient);
	RECT rcWindow = {0};
	::GetWindowRect(m_hwndDrawer, &rcWindow);
	POINT ptWindow = {rcWindow.left, rcWindow.top};
	SIZE szWindow = {rectWidth(rcWindow), rectHeight(rcWindow)};

	HDC tmpDC = ::CreateCompatibleDC(m_hdcDrawer);
	HBITMAP hbitmap = ::CreateCompatibleBitmap(m_hdcDrawer, szWindow.cx, szWindow.cy);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(tmpDC, hbitmap);
	//Gdiplus::Graphics grap(tmpDC);
	//grap.DrawImage(*m_pImageBk, 0, 0, m_pImageBk->getWidth(), m_pImageBk->getHeight());
	m_pRoot->paint(tmpDC, m_pRoot->getPosition());
	POINT ptSrc = {0,0};
	//SEND_LOG(L"drawManager->WM_ERASEBKGND 1");
	BOOL bl = ::UpdateLayeredWindow(m_hwndDrawer, m_hdcDrawer, &ptWindow, &szWindow, tmpDC, &ptSrc, 0, &blend, ULW_ALPHA);
	//SEND_LOG(L"drawManager->WM_ERASEBKGND 2");
	if(!bl) SEND_LOG(L"ERROR drawManager::updateLayeredWindow lastError:%d", GetLastError());
	SelectObject(tmpDC, oldBitmap);
	DeleteObject(hbitmap);
	DeleteDC(tmpDC);
}

SIZE drawManager::estimateToolTipSize()
{
	SIZE szTip = {245, 36};
	if (m_hwndTooltip && std::wcslen(m_tipToolTip.lpszText) > 0)
	{
		HDC hdc = GetDC(m_hwndTooltip);
		if (hdc)
		{
			Gdiplus::Graphics grap(hdc);
			RECT rc = {0,0,0,0};
			HFONT hFont = GetWindowFont(m_hwndTooltip);
			if(!hFont) hFont = getFont(0);
			gtc::rect rcm = gtc::drawer::measureString(grap, m_tipToolTip.lpszText, rc, hFont, DT_CENTER|DT_NOPREFIX|DT_EDITCONTROL|DT_WORDBREAK);
			//if(::DrawText(hdc, m_tipToolTip.lpszText, -1, &rc, DT_CENTER|DT_NOPREFIX|DT_EDITCONTROL|DT_WORDBREAK|DT_CALCRECT))
			//{
			//	szTip.cx = rectWidth(rc);
			//	szTip.cy = rectHeight(rc);
			//}
			
			szTip.cx = rcm.width;
			szTip.cy = rcm.height;
			ReleaseDC(m_hwndTooltip, hdc);
		}
	}
	return szTip;
}
bool drawManager::preMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{
	return false;
}

void drawManager::messageLoop()
{
	MSG msg = { 0 };
	while( ::GetMessage(&msg, NULL, 0, 0) ) {
		if( !drawManager::translateMessage(&msg) ) {
			::TranslateMessage(&msg);
			try
			{
				::DispatchMessage(&msg);
			}
			catch(...)
			{
#if defined(_DEBUG)
				SEND_LOG(L"ERROR drawManager::messageLoop_DispatchMessage hwnd:%08X msg:04X wp:%08X lp:%08X", msg.hwnd, msg.message, msg.wParam, msg.lParam);
#endif
			}
		}
	}
}
bool drawManager::translateMessage(const LPMSG pMsg)
{
	UINT uStyle = GetWindowStyle(pMsg->hwnd);
	LRESULT lRes = 0;

	if (uStyle & WS_CHILD)
	{
		HWND hWndParent = ::GetParent(pMsg->hwnd);

		for(std::vector<drawManager *>::iterator iter = mc_vecPreMessages.begin(); iter != mc_vecPreMessages.end(); ++iter) 
		{
			//CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);        
			HWND hTempParent = hWndParent;
			while(hTempParent)
			{
				if(pMsg->hwnd == (*iter)->getPaintWindow() || hTempParent == (*iter)->getPaintWindow())
				{
					if ((*iter)->translateAccelerator(pMsg))
						return true;

					if( (*iter)->preMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
						return true;

					return false;
				}
				hTempParent = GetParent(hTempParent);
			}
		}
	}
	else
	{
		for (std::vector<drawManager *>::iterator iter = mc_vecPreMessages.begin(); iter != mc_vecPreMessages.end(); ++iter)
		{
			if (pMsg->hwnd == (*iter)->getPaintWindow()/*
													   || (hwndParent == (*iter)->getPaintWindow() && ((uStyle & WS_CHILD) != 0))*/)
			{
				if((*iter)->translateAccelerator(pMsg)) return true;

				if((*iter)->preMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes)) return true;

				return false;
			}
		}

	}
	return false;
}


bool drawManager::addNotifier(INotifyUC* pControl)
{
	assert(pControl);
	bool bRet = false;
	//SEND_LOG(L"drawManager::addNotifier oldsize:%d threadID:%u", m_vecNotifys.size(), GetCurrentThreadId());
	if (std::find(m_vecNotifys.cbegin(), m_vecNotifys.cend(), pControl) == m_vecNotifys.cend())
	{
		m_vecNotifys.push_back(pControl);
		bRet = true;
	}
	return bRet;
}
bool drawManager::removeNotifier(INotifyUC* pControl)
{
	bool bRet = false;
	//SEND_LOG(L"drawManager::removeNotifier oldsize:%d threadID:%u", m_vecNotifys.size(), GetCurrentThreadId());
	NotifyArrayType::const_iterator iter = std::find(m_vecNotifys.cbegin(), m_vecNotifys.cend(), pControl);
	if (iter != m_vecNotifys.cend())
	{
		m_vecNotifys.erase(iter);
		bRet = true;
	}
	return bRet;
}
void drawManager::sendNotify(BSNotifyUC& Msg, bool bAsync /*= false*/)
{
	assert(Msg.pSender);
	if(Msg.ptMouse.x == 0 && Msg.ptMouse.y == 0) Msg.ptMouse = m_ptLastMousePos;
	Msg.dwTimestamp = GetTickCount();
	if (!bAsync)
	{
		//int idx = 0;
		//for (NotifyArrayType::iterator iter = m_vecNotifys.begin(); iter != m_vecNotifys.end(); ++iter)
		//{
		//	(*iter)->notify(Msg);
		//}
		int count = m_vecNotifys.size();
		for (int idx = count-1; idx >= 0; --idx)
		{
			m_vecNotifys[idx]->notify(Msg);
		}
	}
	else
	{

	}
}

void drawManager::sendNotify(controlUC* pControl, PCWSTR pstrMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
{
	assert(pControl);
	BSNotifyUC Msg;
	Msg.pSender = pControl;
	Msg.wsType = pstrMessage;
	Msg.wParam = wParam;
	Msg.lParam = lParam;
	sendNotify(Msg, bAsync);
}



bool drawManager::setTimer(controlUC* pControl, UINT nTimerID, UINT uElapse)
{
	assert(pControl!=NULL && uElapse > 0);
	for(std::vector<BSTimerInfo *>::iterator iter = m_vecTimers.begin(); iter != m_vecTimers.end(); ++iter) {
		if( (*iter)->pSender == pControl
			&& (*iter)->hWnd == m_hwndDrawer
			&& (*iter)->nLocalID == nTimerID )
		{
			if( (*iter)->bKilled ) {
				if( ::SetTimer(m_hwndDrawer, (*iter)->uWinTimer, uElapse, NULL) ) {
					(*iter)->bKilled = false;
					return true;
				}

				return false;
			}

			return false;
		}
	}

	m_uTimerID = (++m_uTimerID) % 0xFF;
	if( !::SetTimer(m_hwndDrawer, m_uTimerID, uElapse, NULL) ) return FALSE;
	BSTimerInfo* pTimer = new BSTimerInfo;
	if( pTimer == NULL ) return FALSE;
	pTimer->hWnd = m_hwndDrawer;
	pTimer->pSender = pControl;
	pTimer->nLocalID = nTimerID;
	pTimer->uWinTimer = m_uTimerID;
	pTimer->bKilled = false;
	m_vecTimers.push_back(pTimer);//.Add(pTimer);
	return true;
}
bool drawManager::killTimer(controlUC* pControl, UINT nTimerID)
{
	assert(pControl!=NULL);
	for(std::vector<BSTimerInfo *>::iterator iter = m_vecTimers.begin(); iter != m_vecTimers.end(); ++iter ) {
		if( (*iter)->pSender == pControl
			&& (*iter)->hWnd == m_hwndDrawer
			&& (*iter)->nLocalID == nTimerID )
		{
			if( (*iter)->bKilled == false ) {
				if( ::IsWindow(m_hwndDrawer) ) ::KillTimer((*iter)->hWnd, (*iter)->uWinTimer);
				(*iter)->bKilled = true;
				return true;
			}
		}
	}
	return false;
}
void drawManager::killTimer(controlUC* pControl)
{
	assert(pControl!=NULL);

	for (std::vector<BSTimerInfo *>::iterator iter = m_vecTimers.begin(); iter != m_vecTimers.end();)
	{
		if ((*iter)->pSender == pControl && (*iter)->hWnd == m_hwndDrawer)
		{
			if(!(*iter)->bKilled) ::KillTimer((*iter)->hWnd, (*iter)->uWinTimer);
			delete *iter;
			iter = m_vecTimers.erase(iter);
		}
		else
			++iter;
	}

}
void drawManager::removeAllTimers()
{
	for (std::vector<BSTimerInfo *>::iterator iter = m_vecTimers.begin(); iter != m_vecTimers.end(); ++iter)
	{
		if(!(*iter)->bKilled)
		{
			if (::IsWindow(m_hwndDrawer))
			{
				::KillTimer(m_hwndDrawer, (*iter)->uWinTimer);
			}
		}
		delete *iter;
	}
	m_vecTimers.clear();
}

bool drawManager::addMessageFilter(IMessageFilterUC *pFilter)
{
	assert(pFilter);
	if(!pFilter) return false;
	MessageFilterArrayType::const_iterator iter = std::find(m_vecMessageFilter.cbegin(), m_vecMessageFilter.cend(), pFilter);
	if(iter == m_vecMessageFilter.cend())
	{
		m_vecMessageFilter.push_back(pFilter);
		return true;
	}
	return false;
}
bool drawManager::removeMessageFilter(IMessageFilterUC *pFilter)
{
	assert(pFilter);
	if(!pFilter) return false;
	MessageFilterArrayType::const_iterator iter = std::find(m_vecMessageFilter.cbegin(), m_vecMessageFilter.cend(), pFilter);
	if (iter != m_vecMessageFilter.cend())
	{
		m_vecMessageFilter.erase(iter);
		return true;
	}
	return false;
}

bool drawManager::addTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
{
	TranslateAcceleratorArrayType::const_iterator iter = std::find(m_vecTranslateAccelerator.cbegin(), m_vecTranslateAccelerator.cend(), pTranslateAccelerator);
	if(iter == m_vecTranslateAccelerator.cend())
	{
		m_vecTranslateAccelerator.push_back(pTranslateAccelerator);
		return true;
	}
	return false;
}
bool drawManager::removeTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
{
	TranslateAcceleratorArrayType::const_iterator iter = std::find(m_vecTranslateAccelerator.cbegin(), m_vecTranslateAccelerator.cend(), pTranslateAccelerator);
	if(iter != m_vecTranslateAccelerator.cend())
	{
		m_vecTranslateAccelerator.erase(iter);
		return true;
	}
	return false;
}
bool drawManager::translateAccelerator(LPMSG pMsg)
{
	for (int i = 0; i < (int)m_vecTranslateAccelerator.size(); i++)
	{
		LRESULT lResult = static_cast<ITranslateAccelerator *>(m_vecTranslateAccelerator[i])->translateAccelerator(pMsg);
		if( lResult == S_OK ) return true;
	}
	return false;
}
void drawManager::setBesidesArea(RECT rcBesidesArea)
{
	if(m_bBesidesArea)
	{	// 与现有区域合并
		::UnionRect(&m_rcBesidesArea, &m_rcBesidesArea, &rcBesidesArea);
	}
	else
	{
		m_bBesidesArea = true;
		m_rcBesidesArea = rcBesidesArea;

	}
	SEND_LOG(L"WARNING drawManager::setBesidesArea 开启 被排除刷新区域 <%s>", gtc::format::rect(m_rcBesidesArea, true).c_str());
}
void drawManager::freeBesidesArea()
{
	m_bBesidesArea = false;
	invalidate(m_rcBesidesArea);
	memset(&m_rcBesidesArea, 0, sizeof(m_rcBesidesArea));
	SEND_LOG(L"WARNING drawManager::setBesidesArea 停止 被排除刷新区域");
}

void drawManager::updateWindowOutBesidesArea()
{
	//SEND_LOG(L"WARNING drawManager::updateWindowOutBesidesArea 注意:影响");
	if(m_bBesidesArea)
	{
		RECT rcWnd = {0};
		RECT rc = {0};
		GetClientRect(m_hwndDrawer, &rcWnd);
		/*
			|   top    |
			|----------|
		left| besides  |right
			|----------|
			|  bottom  |
		*/
		// left
		rc = rcWnd;
		rc.right = m_rcBesidesArea.left;
		::InvalidateRect(m_hwndDrawer, &rc, FALSE);
		// top
		rc = rcWnd;
		rc.left = m_rcBesidesArea.left;
		rc.right = m_rcBesidesArea.right;
		rc.bottom = m_rcBesidesArea.top;
		::InvalidateRect(m_hwndDrawer, &rc, FALSE);
		// right
		rc = rcWnd;
		rc.left = m_rcBesidesArea.right;
		::InvalidateRect(m_hwndDrawer, &rc, FALSE);
		// bottom
		rc = rcWnd;
		rc.left = m_rcBesidesArea.left;
		rc.right = m_rcBesidesArea.right;
		rc.top = m_rcBesidesArea.bottom;
		::InvalidateRect(m_hwndDrawer, &rc, FALSE);

	}
	else
	{
		InvalidateRect(m_hwndDrawer, NULL, FALSE);
	}
}

void drawManager::endDragDrop()
{
	releaseCapture();
	m_pEventClick = nullptr;
}
}