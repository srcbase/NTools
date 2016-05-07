#include "StdAfx.h"
#include "UCmanager.h"

namespace wuc{


static UINT keyState()
{
	UINT uState = 0;
	if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
	if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_LBUTTON;
	if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_RBUTTON;
	if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
	if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
	return uState;
}


/////////////////////////////////////////////////////////////////////////////////////

HPEN m_hUpdateRectPen = NULL;
HINSTANCE paintManagerUC::mc_hInstance = NULL;
std::vector<paintManagerUC *> paintManagerUC::mc_vecPreMessages;
gtc::resourceManager paintManagerUC::mc_resourceManager;

paintManagerUC::paintManagerUC()
	: m_hWndPaint(NULL),
	m_hDcPaint(NULL),
	m_hDcOffscreen(NULL),
	m_hDcBackground(NULL),
	m_hbmpOffscreen(NULL),
	m_hbmpBackground(NULL),
	m_hwndTooltip(NULL),
	m_bShowUpdateRect(false),
	m_uTimerID(0x1000),
	m_pRoot(NULL),
	m_pFocus(NULL),
	m_pEventHover(NULL),
	m_pEventClick(NULL),
	m_pEventKey(NULL),
	m_bFirstLayout(true),
	m_bFocusNeeded(false),
	m_bUpdateNeeded(false),
	m_bMouseTracking(false),
	m_bMouseCapture(false),
	m_bOffscreenPaint(true),
	m_bAlphaBackground(false)
{
	//TRACE_CLASS_BEGIN(this)
	m_dwDefaultDisabledColor = 0xFFA7A6AA;
	m_dwDefaultFontColor = 0xFF000000;
	m_dwDefaultLinkFontColor = 0xFF0000FF;
	m_dwDefaultLinkHoverFontColor = 0xFFD3215F;
	m_dwDefaultSelectedBkColor = 0xFFBAE4FF;
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	lf.lfCharSet = DEFAULT_CHARSET;
	HFONT hDefaultFont = ::CreateFontIndirect(&lf);
	m_DefaultFontInfo.hFont = hDefaultFont;
	m_DefaultFontInfo.sFontName = lf.lfFaceName;
	m_DefaultFontInfo.iSize = -lf.lfHeight;
	m_DefaultFontInfo.bBold = (lf.lfWeight >= FW_BOLD);
	m_DefaultFontInfo.bUnderline = (lf.lfUnderline == TRUE);
	m_DefaultFontInfo.bItalic = (lf.lfItalic == TRUE);
	::ZeroMemory(&m_DefaultFontInfo.tm, sizeof(m_DefaultFontInfo.tm));

	if( m_hUpdateRectPen == NULL ) {
		m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
		// Boot Windows Common Controls (for the ToolTip control)
		::InitCommonControls();
		::LoadLibrary(L"msimg32.dll");
	}

	m_szMinWindow.cx = 0;
	m_szMinWindow.cy = 0;
	m_szMaxWindow.cx = 0;
	m_szMaxWindow.cy = 0;
	m_szInitWindowSize.cx = 0;
	m_szInitWindowSize.cy = 0;
	m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
	::ZeroMemory(&m_rcSizeBox, sizeof(m_rcSizeBox));
	::ZeroMemory(&m_rcCaption, sizeof(m_rcCaption));
	m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
}

paintManagerUC::~paintManagerUC()
{

	for (std::vector<BSNotifyUC *>::iterator iter = m_aAsyncNotify.begin(); iter != m_aAsyncNotify.end(); ++iter)
	{
		delete *iter;
	}
	m_mNameHash.clear();
	delete m_pRoot;

	::DeleteObject(m_DefaultFontInfo.hFont);
	removeAllFonts();
	removeAllDefaultAttributeList();
	removeAllOptionGroups();
	removeAllTimers();

	// Reset other parts...
	if( m_hwndTooltip != NULL ) ::DestroyWindow(m_hwndTooltip);
	if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
	if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
	if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
	if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
	if( m_hDcPaint != NULL ) ::ReleaseDC(m_hWndPaint, m_hDcPaint);
	//mc_vecPreMessages.Remove(mc_vecPreMessages.Find(this));
	std::vector<paintManagerUC *>::iterator iter = std::find(mc_vecPreMessages.begin(), mc_vecPreMessages.end(), this);
	if (iter != mc_vecPreMessages.end())
	{
		mc_vecPreMessages.erase(iter);
	}
}

void paintManagerUC::init(HWND hWnd)
{
	assert(::IsWindow(hWnd));
	// Remember the window context we came from
	m_hWndPaint = hWnd;
	m_hDcPaint = ::GetDC(hWnd);
	// We'll want to filter messages globally too
	mc_vecPreMessages.push_back(this);
}

HINSTANCE paintManagerUC::getInstance()
{
	return mc_hInstance;
}

void paintManagerUC::setInstance(HINSTANCE hInst)
{
	mc_hInstance = hInst;
}

gtc::resourceManager& paintManagerUC::getResourceManager()
{
	return mc_resourceManager;
}
void paintManagerUC::setResourceManager(HMODULE hResource)
{
	mc_resourceManager.setModuble(hResource);
}

// Í¼Æ¬ png
bool paintManagerUC::addImage(std::wstring imgKey, UINT id, LPCTSTR pType /*= RT_RCDATA*/)
{
	return mc_resourceManager.addImage(imgKey, id, pType);
}

gtc::image* paintManagerUC::findImage(std::wstring imgKey)
{
	return mc_resourceManager.findImage(imgKey);
}
// ²Ëµ¥
bool paintManagerUC::addMenu(std::wstring menuKey, UINT id)
{
	return mc_resourceManager.addMenu(menuKey, id);
}

HMENU paintManagerUC::findMenu(std::wstring menuKey)
{
	return mc_resourceManager.findMenu(menuKey);
}

HWND paintManagerUC::getPaintWindow() const
{
	return m_hWndPaint;
}

HWND paintManagerUC::getTooltipWindow() const
{
	return m_hwndTooltip;
}

HDC paintManagerUC::getPaintDC() const
{
	return m_hDcPaint;
}

POINT paintManagerUC::getMousePos() const
{
	return m_ptLastMousePos;
}

SIZE paintManagerUC::getClientSize() const
{
	RECT rcClient = { 0 };
	::GetClientRect(m_hWndPaint, &rcClient);
	return gtc::size(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
}

SIZE paintManagerUC::getInitSize()
{
	return m_szInitWindowSize;
}

void paintManagerUC::setInitSize(int cx, int cy)
{
	m_szInitWindowSize.cx = cx;
	m_szInitWindowSize.cy = cy;
	if( m_pRoot == NULL && m_hWndPaint != NULL ) {
		::SetWindowPos(m_hWndPaint, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

RECT& paintManagerUC::getSizeBox()
{
	return m_rcSizeBox;
}

void paintManagerUC::setSizeBox(RECT& rcSizeBox)
{
	m_rcSizeBox = rcSizeBox;
}

RECT& paintManagerUC::getCaptionRect()
{
	return m_rcCaption;
}

void paintManagerUC::setCaptionRect(RECT& rcCaption)
{
	m_rcCaption = rcCaption;
}

SIZE paintManagerUC::getRoundCorner() const
{
	return m_szRoundCorner;
}

void paintManagerUC::setRoundCorner(int cx, int cy)
{
	m_szRoundCorner.cx = cx;
	m_szRoundCorner.cy = cy;
}

SIZE paintManagerUC::getMinInfo() const
{
	return m_szMinWindow;
}

void paintManagerUC::setMinInfo(int cx, int cy)
{
	assert(cx>=0 && cy>=0);
	m_szMinWindow.cx = cx;
	m_szMinWindow.cy = cy;
}

SIZE paintManagerUC::getMaxInfo() const
{
	return m_szMaxWindow;
}

void paintManagerUC::setMaxInfo(int cx, int cy)
{
	assert(cx>=0 && cy>=0);
	m_szMaxWindow.cx = cx;
	m_szMaxWindow.cy = cy;
}

void paintManagerUC::setTransparent(int nOpacity)
{
	if( m_hWndPaint != NULL ) {
		typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
		PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes;

		HMODULE hUser32 = ::GetModuleHandle(L"User32.dll");
		if (hUser32)
		{
			fSetLayeredWindowAttributes = 
				(PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
			if( fSetLayeredWindowAttributes == NULL ) return;
		}

		DWORD dwStyle = ::GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
		DWORD dwNewStyle = dwStyle;
		if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
		else dwNewStyle &= ~WS_EX_LAYERED;
		if(dwStyle != dwNewStyle) ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewStyle);
		fSetLayeredWindowAttributes(m_hWndPaint, 0, nOpacity, LWA_ALPHA);
	}
}

void paintManagerUC::setBackgroundTransparent(bool bTrans)
{
	m_bAlphaBackground = bTrans;
}

bool paintManagerUC::isShowUpdateRect() const
{
	return m_bShowUpdateRect;
}

void paintManagerUC::setShowUpdateRect(bool show)
{
	m_bShowUpdateRect = show;
}

bool paintManagerUC::preMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& /*lRes*/)
{
	//for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) 
	//{
	//	bool bHandled = false;
	//	LRESULT lResult = static_cast<IMessageFilterUC*>(m_aPreMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
	//	if( bHandled ) {
	//		return true;
	//	}
	//}
	for (std::vector<IMessageFilterUC *>::iterator iter = m_aPreMessageFilters.begin(); iter != m_aPreMessageFilters.end(); ++iter)
	{
		bool bHandled = false;
		LRESULT lResult = (*iter)->messageHandler(uMsg, wParam, lParam, bHandled);
		if(bHandled) return true;
	}
	switch( uMsg ) {
	case WM_KEYDOWN:
		{
			// DAI
			//// Tabbing between controls
			//if( wParam == VK_TAB ) {
			//	if( m_pFocus && m_pFocus->isVisible() && m_pFocus->isEnabled() && _tcsstr(m_pFocus->getClass(), L"RichEditUI")) != NULL ) {
			//		if( static_cast<CRichEditUI*>(m_pFocus)->IsWantTab() ) return false;
			//	}
			//	SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
			//	return true;
			//}
		}
		break;
	case WM_SYSCHAR:
		{
			// Handle ALT-shortcut key-combinations
			BSFindShortCut fs = { 0 };
			fs.ch = toupper((int)wParam);
			controlUC* pControl = m_pRoot->findControl(__findControlFromShortcut, &fs, UCFIND_ENABLED | UCFIND_ME_FIRST | UCFIND_TOP_FIRST);
			if( pControl != NULL ) {
				pControl->setFocus();
				pControl->activate();
				return true;
			}
		}
		break;
	case WM_SYSKEYDOWN:
		{
			if( m_pFocus != NULL ) {
				BSEventUC event = { 0 };
				event.Type = BEUCEventSysKey;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = keyState();
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
			}
		}
		break;
	}
	return false;
}

bool paintManagerUC::messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{
	//#ifdef _DEBUG
	//    switch( uMsg ) {
	//    case WM_NCPAINT:
	//    case WM_NCHITTEST:
	//    case WM_SETCURSOR:
	//       break;
	//    default:
	//       TRACE(_T("MSG: %-20s (%08ld)"), TRACEMSG(uMsg), ::GetTickCount());
	//    }
	//#endif
	// Not ready yet?
	if( m_hWndPaint == NULL ) return false;

	//BSNotifyUC* pMsg = NULL;
	//while( pMsg = static_cast<BSNotifyUC*>(m_aAsyncNotify.GetAt(0)) ) {
	//	m_aAsyncNotify.Remove(0);
	//	if( pMsg->pSender != NULL ) {
	//		if( pMsg->pSender->OnNotify ) pMsg->pSender->OnNotify(pMsg);
	//	}
	//	for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
	//		static_cast<INotifyUC*>(m_aNotifiers[j])->Notify(*pMsg);
	//	}
	//	delete pMsg;
	//}
	BSNotifyUC* pMsg = NULL;
	while(m_aAsyncNotify.size() > 0)
	{
		pMsg = m_aAsyncNotify.front();
		//if(pMsg->pSender != nullptr && pMsg->pSender->onNotify) pMsg->pSender->onNotify(pMsg);
		for (std::vector<INotifyUC *>::iterator iter = m_aNotifiers.begin(); iter != m_aNotifiers.end(); ++iter)
		{
			(*iter)->notify(*pMsg);
		}
		delete pMsg;
		m_aAsyncNotify.erase(m_aAsyncNotify.begin());

	}
	// Cycle through listeners
	//for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) 
	//{
	//	bool bHandled = false;
	//	LRESULT lResult = static_cast<IMessageFilterUC*>(m_aMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
	//	if( bHandled ) {
	//		lRes = lResult;
	//		return true;
	//	}
	//}
	for (std::vector<IMessageFilterUC *>::iterator iter = m_aMessageFilters.begin(); iter != m_aMessageFilters.end(); ++iter)
	{
		bool bHandled = false;
		LRESULT lResult = (*iter)->messageHandler(uMsg, wParam, lParam, bHandled);
		if( bHandled ) {
			lRes = lResult;
			return true;
		}
	}
	// Custom handling of events
	switch( uMsg ) {
	case WM_CLOSE:
		{
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
			}

			SetFocus(NULL);

			// Hmmph, the usual Windows tricks to avoid
			// focus loss...
			HWND hwndParent = GetWindowOwner(m_hWndPaint);
			if( hwndParent != NULL ) ::SetFocus(hwndParent);
		}
		break;
	case WM_ERASEBKGND:
		{
			// We'll do the painting here...
			lRes = 1;
		}
		return true;
	case WM_PAINT:
		{
			// Should we paint?
			RECT rcPaint = { 0 };
			if( !::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE) ) return true;
			if( m_pRoot == NULL ) {
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(m_hWndPaint, &ps);
				::EndPaint(m_hWndPaint, &ps);
				return true;
			}            
			// Do we need to resize anything?
			// This is the time where we layout the controls on the form.
			// We delay this even from the WM_SIZE messages since resizing can be
			// a very expensize operation.
			if( m_bUpdateNeeded ) {
				m_bUpdateNeeded = false;
				RECT rcClient = { 0 };
				::GetClientRect(m_hWndPaint, &rcClient);
				if( !::IsRectEmpty(&rcClient) ) {
					if( m_pRoot->isUpdateNeeded() ) {
						m_pRoot->setPos(rcClient);
						if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
						if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
						if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
						if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
						m_hDcOffscreen = NULL;
						m_hDcBackground = NULL;
						m_hbmpOffscreen = NULL;
						m_hbmpBackground = NULL;
					}
					else {
						controlUC* pControl = NULL;
						while( pControl = m_pRoot->findControl(__findControlFromUpdate, NULL, UCFIND_VISIBLE | UCFIND_ME_FIRST) ) {
							pControl->setPos( pControl->getPos() );
						}
					}
					// We'll want to notify the window when it is first initialized
					// with the correct layout. The window form would take the time
					// to submit swipes/animations.
					if( m_bFirstLayout ) {
						m_bFirstLayout = false;
						sendNotify(m_pRoot, L"windowinit",  0, 0, false);
					}
				}
			}
			// Set focus to first control?
			if( m_bFocusNeeded ) {
				setNextTabControl();
			}
			//
			// Render screen
			//
			// Prepare offscreen bitmap?
			if( m_bOffscreenPaint && m_hbmpOffscreen == NULL )
			{
				RECT rcClient = { 0 };
				::GetClientRect(m_hWndPaint, &rcClient);
				m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
				m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
				assert(m_hDcOffscreen);
				assert(m_hbmpOffscreen);
			}
			// Begin Windows paint
			PAINTSTRUCT ps = { 0 };
			::BeginPaint(m_hWndPaint, &ps);
			if( m_bOffscreenPaint )
			{
				HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
				int iSaveDC = ::SaveDC(m_hDcOffscreen);
				if( m_bAlphaBackground ) {
					if( m_hbmpBackground == NULL ) {
						RECT rcClient = { 0 };
						::GetClientRect(m_hWndPaint, &rcClient);
						m_hDcBackground = ::CreateCompatibleDC(m_hDcPaint);;
						m_hbmpBackground = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
						assert(m_hDcBackground);
						assert(m_hbmpBackground);
						::SelectObject(m_hDcBackground, m_hbmpBackground);
						::BitBlt(m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
							ps.rcPaint.bottom - ps.rcPaint.top, ps.hdc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
					}
					else
						::SelectObject(m_hDcBackground, m_hbmpBackground);
					::BitBlt(m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
						ps.rcPaint.bottom - ps.rcPaint.top, m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
				}
				m_pRoot->paint(m_hDcOffscreen, ps.rcPaint);
				//for (std::vector<controlUC *>::iterator iter = m_aPostPaintControls.begin(); iter != m_aPostPaintControls.end(); ++iter)
				//{
				//	(*iter)->doPostPaint(m_hDcOffscreen, ps.rcPaint);
				//}
				::RestoreDC(m_hDcOffscreen, iSaveDC);
				::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
					ps.rcPaint.bottom - ps.rcPaint.top, m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
				::SelectObject(m_hDcOffscreen, hOldBitmap);

				if( m_bShowUpdateRect ) {
					HPEN hOldPen = (HPEN)::SelectObject(ps.hdc, m_hUpdateRectPen);
					::SelectObject(ps.hdc, ::GetStockObject(HOLLOW_BRUSH));
					::Rectangle(ps.hdc, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
					::SelectObject(ps.hdc, hOldPen);
				}
			}
			else
			{
				// A standard paint job
				int iSaveDC = ::SaveDC(ps.hdc);
				m_pRoot->paint(ps.hdc, ps.rcPaint);
				::RestoreDC(ps.hdc, iSaveDC);
			}
			// All Done!
			::EndPaint(m_hWndPaint, &ps);
		}
		// If any of the painting requested a resize again, we'll need
		// to invalidate the entire window once more.
		if( m_bUpdateNeeded ) {
			::InvalidateRect(m_hWndPaint, NULL, FALSE);
		}
		return true;
	case WM_PRINTCLIENT:
		{
			RECT rcClient;
			::GetClientRect(m_hWndPaint, &rcClient);
			HDC hDC = (HDC) wParam;
			int save = ::SaveDC(hDC);
			m_pRoot->paint(hDC, rcClient);
			// Check for traversing children. The crux is that WM_PRINT will assume
			// that the DC is positioned at frame coordinates and will paint the child
			// control at the wrong position. We'll simulate the entire thing instead.
			if( (lParam & PRF_CHILDREN) != 0 ) {
				HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
				while( hWndChild != NULL ) {
					RECT rcPos = { 0 };
					::GetWindowRect(hWndChild, &rcPos);
					::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
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
				event.Type = BEUCEventWindowZize;
				event.pSender = m_pFocus;
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
			}
			if( m_pRoot != NULL ) m_pRoot->needUpdate();
		}
		return true;
	case WM_TIMER:
		{
			//for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
			//	const BSTimerInfo* pTimer = static_cast<BSTimerInfo*>(m_aTimers[i]);
			//	if( pTimer->hWnd == m_hWndPaint && pTimer->uWinTimer == LOWORD(wParam) && pTimer->bKilled == false) {
			//		BSEventUC event = { 0 };
			//		event.Type = UIEVENT_TIMER;
			//		event.pSender = pTimer->pSender;
			//		event.wParam = pTimer->nLocalID;
			//		event.dwTimestamp = ::GetTickCount();
			//		pTimer->pSender->Event(event);
			//		break;
			//	}
			//}
			for (std::vector<BSTimerInfo *>::iterator iter = m_aTimers.begin(); iter != m_aTimers.end(); ++iter)
			{
				if ((*iter)->hWnd == m_hWndPaint && (*iter)->uWinTimer == LOWORD(wParam) && (*iter)->bKilled == false)
				{
					BSEventUC event = { 0 };
					event.Type = BEUCEventTimer;
					event.pSender = (*iter)->pSender;
					event.wParam = (*iter)->nLocalID;
					event.dwTimestamp = ::GetTickCount();
					(*iter)->pSender->Event(event);
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
			if( sToolTip.empty() ) return true;
			::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
			m_ToolTip.cbSize = sizeof(TOOLINFO);
			m_ToolTip.uFlags = TTF_IDISHWND;
			m_ToolTip.hwnd = m_hWndPaint;
			m_ToolTip.uId = (UINT_PTR) m_hWndPaint;
			m_ToolTip.hinst = mc_hInstance;
			m_ToolTip.lpszText = const_cast<PWSTR>(sToolTip.c_str());
			m_ToolTip.rect = pHover->getPos();
			if( m_hwndTooltip == NULL ) {
				m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, mc_hInstance, NULL);
				::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
			}
			::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
			::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
		}
		return true;
	case WM_MOUSELEAVE:
		{
			if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
			if( m_bMouseTracking ) ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) -1);
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
				tme.hwndTrack = m_hWndPaint;
				tme.dwHoverTime = m_hwndTooltip == NULL ? 1000UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
				_TrackMouseEvent(&tme);
				m_bMouseTracking = true;
			}
			// Generate the appropriate mouse messages
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			controlUC* pNewHover = findControl(pt);
			if( pNewHover != NULL && pNewHover->getManager() != this ) break;
			BSEventUC event = { 0 };
			event.ptMouse = pt;
			event.dwTimestamp = ::GetTickCount();
			if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
				event.Type = BEUCEventMouseLeave;
				event.pSender = m_pEventHover;
				m_pEventHover->Event(event);
				m_pEventHover = NULL;
				if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
			}
			if( pNewHover != m_pEventHover && pNewHover != NULL ) {
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
			::SetFocus(m_hWndPaint);
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
			::SetFocus(m_hWndPaint);
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
			if( m_pEventClick == NULL ) break;
			releaseCapture();
			BSEventUC event = { 0 };
			event.Type = BEUCEventButtonUp;
			event.pSender = m_pEventClick;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			m_pEventClick->Event(event);
			m_pEventClick = NULL;
		}
		break;
	case WM_RBUTTONDOWN:
		{
			::SetFocus(m_hWndPaint);
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
			::ScreenToClient(m_hWndPaint, &pt);
			m_ptLastMousePos = pt;
			if( m_pEventClick == NULL ) break;
			releaseCapture();
			BSEventUC event = { 0 };
			event.Type = BEUCEventContextMenu;
			event.pSender = m_pEventClick;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.lParam = (LPARAM)m_pEventClick;
			event.dwTimestamp = ::GetTickCount();
			m_pEventClick->Event(event);
			m_pEventClick = NULL;
		}
		break;
	case WM_MOUSEWHEEL:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			::ScreenToClient(m_hWndPaint, &pt);
			m_ptLastMousePos = pt;
			controlUC* pControl = findControl(pt);
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
			pControl->Event(event);

			// Let's make sure that the scroll item below the cursor is the same as before...
			::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
		}
		break;
	case WM_CHAR:
		{
			if( m_pFocus == NULL ) break;
			BSEventUC event = { 0 };
			event.Type = BEUCEventChar;
			event.chKey = (TCHAR)wParam;
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
			event.chKey = (TCHAR)wParam;
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
			event.chKey = (TCHAR)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = keyState();
			event.dwTimestamp = ::GetTickCount();
			m_pEventKey->Event(event);
			m_pEventKey = NULL;
		}
		break;
	case WM_SETCURSOR:
		{
			if( LOWORD(lParam) != HTCLIENT ) break;
			if( m_bMouseCapture ) return true;

			POINT pt = { 0 };
			::GetCursorPos(&pt);
			::ScreenToClient(m_hWndPaint, &pt);
			controlUC* pControl = findControl(pt);
			if( pControl == NULL ) break;
			if( (pControl->getControlFlags() & UCFLAG_SETCURSOR) == 0 ) break;
			BSEventUC event = { 0 };
			event.Type = BEUCEventSetFocus;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = keyState();
			event.dwTimestamp = ::GetTickCount();
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

	pMsg = NULL;
	//while( pMsg = static_cast<BSNotifyUC*>(m_aAsyncNotify.GetAt(0)) ) {
	//	m_aAsyncNotify.Remove(0);
	//	if( pMsg->pSender != NULL ) {
	//		if( pMsg->pSender->OnNotify ) pMsg->pSender->OnNotify(pMsg);
	//	}
	//	for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
	//		static_cast<INotifyUC*>(m_aNotifiers[j])->Notify(*pMsg);
	//	}
	//	delete pMsg;
	//}
	while(m_aAsyncNotify.size() > 0)
	{
		pMsg = m_aAsyncNotify.front();
		//if(pMsg->pSender != nullptr && pMsg->pSender->onNotify) pMsg->pSender->onNotify(pMsg);
		for (std::vector<INotifyUC *>::iterator iter = m_aNotifiers.begin(); iter != m_aNotifiers.end(); ++iter)
		{
			(*iter)->notify(*pMsg);
		}
		delete pMsg;
		m_aAsyncNotify.erase(m_aAsyncNotify.begin());
	}
	return false;
}

void paintManagerUC::needUpdate()
{
	m_bUpdateNeeded = true;
}

void paintManagerUC::invalidate(RECT& rcItem)
{
	::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
}

bool paintManagerUC::attachDialog(controlUC* pControl)
{
	assert(::IsWindow(m_hWndPaint));
	// Reset any previous attachment
	SetFocus(NULL);
	m_pEventKey = NULL;
	m_pEventHover = NULL;
	m_pEventClick = NULL;
	// Remove the existing control-tree. We might have gotten inside this function as
	// a result of an event fired or similar, so we cannot just delete the objects and
	// pull the internal memory of the calling code. We'll delay the cleanup.
	// Set the dialog root element
	m_pRoot = pControl;
	// Go ahead...
	m_bUpdateNeeded = true;
	m_bFirstLayout = true;
	m_bFocusNeeded = true;
	// Initiate all control
	return initControls(pControl);
}

bool paintManagerUC::initControls(controlUC* pControl, controlUC* pParent /*= NULL*/)
{
	assert(pControl);
	if( pControl == NULL ) return false;
	pControl->setManager(this, pParent != NULL ? pParent : pControl->getParent(), true);
	pControl->findControl(__findControlFromNameHash, this, UCFIND_ALL);
	return true;
}

void paintManagerUC::reapObjects(controlUC* pControl)
{
	if( pControl == m_pEventKey ) m_pEventKey = NULL;
	if( pControl == m_pEventHover ) m_pEventHover = NULL;
	if( pControl == m_pEventClick ) m_pEventClick = NULL;
	if( pControl == m_pFocus ) m_pFocus = NULL;
	killTimer(pControl);
	const std::wstring& sName = pControl->getName();
	if( !sName.empty() ) {
		if( pControl == findControl(sName.c_str()) ) m_mNameHash.erase(sName);// m_mNameHash.Remove(sName);
	}
	//for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) {
	//	BSNotifyUC* pMsg = static_cast<BSNotifyUC*>(m_aAsyncNotify[i]);
	//	if( pMsg->pSender == pControl ) pMsg->pSender = NULL;
	//}  

	for (std::vector<BSNotifyUC *>::iterator iter = m_aAsyncNotify.begin(); iter != m_aAsyncNotify.end(); ++iter)
	{
		if((*iter)->pSender == pControl) (*iter)->pSender = nullptr;
	}
}

bool paintManagerUC::addOptionGroup(PCWSTR pStrGroupName, controlUC* pControl)
{
	//LPVOID lp = (LPVOID)m_mOptionGroup.find(std::wstring(pStrGroupName));
	std::map<std::wstring, std::vector<controlUC *>*>::iterator itfind = m_mOptionGroup.find(pStrGroupName);
	if( itfind != m_mOptionGroup.end() ) {
		std::vector<controlUC *>* aOptionGroup = itfind->second;// static_cast<std::vector<controlUC *>*>(lp);
		for( int i = 0; i < aOptionGroup->size(); i++ ) {
			if( (*aOptionGroup)[i] == pControl ) {
				return false;
			}
		}
		aOptionGroup->push_back(pControl);//Add(pControl);
	}
	else {
		//CStdPtrArray* aOptionGroup = new CStdPtrArray(6);
		//aOptionGroup->Add(pControl);
		//m_mOptionGroup.Insert(pStrGroupName, aOptionGroup);
		std::vector<controlUC *>* aOptionGroup = new std::vector<controlUC *>();
		aOptionGroup->push_back(pControl);
		m_mOptionGroup[pStrGroupName] = aOptionGroup;
	}
	return true;
}

std::vector<controlUC*>* paintManagerUC::getOptionGroup(PCWSTR pStrGroupName)
{
	//LPVOID lp = m_mOptionGroup.find(pStrGroupName);
	//if( lp ) return static_cast<CStdPtrArray*>(lp);

	std::map<std::wstring, std::vector<controlUC *>*>::iterator itfind = m_mOptionGroup.find(pStrGroupName);
	if (itfind != m_mOptionGroup.end())
	{
		return itfind->second;
	}
	return NULL;
}

void paintManagerUC::removeOptionGroup(PCWSTR pStrGroupName, controlUC* pControl)
{
	//LPVOID lp = m_mOptionGroup.find(pStrGroupName);
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
	std::map<std::wstring, std::vector<controlUC *>*>::iterator itfind = m_mOptionGroup.find(pStrGroupName);
	if (itfind != m_mOptionGroup.end())
	{
		std::vector<controlUC *>* aOptionGroup = itfind->second;
		for (std::vector<controlUC *>::iterator iter = aOptionGroup->begin(); iter != aOptionGroup->end(); ++iter)
		{
			if ((*iter) == pControl)
			{
				aOptionGroup->erase(iter);
				break;
			}
		}
		if( aOptionGroup->empty() ) {
			delete aOptionGroup;
			m_mOptionGroup.erase(itfind);
		}
	}
}

void paintManagerUC::removeAllOptionGroups()
{
	//std::vector<controlUC *>* aOptionGroup;
	//for( int i = 0; i< m_mOptionGroup.GetSize(); i++ ) {
	//	if(LPCTSTR key = m_mOptionGroup.GetAt(i)) {
	//		aOptionGroup = static_cast<CStdPtrArray*>(m_mOptionGroup.Find(key));
	//		delete aOptionGroup;
	//	}
	//}
	std::map<std::wstring, std::vector<controlUC *>*>::iterator iter ;
	for (iter = m_mOptionGroup.begin(); iter != m_mOptionGroup.end(); ++iter)
	{
		delete iter->second;
	}
	m_mOptionGroup.clear();
}

void paintManagerUC::messageLoop()
{
	MSG msg = { 0 };
	while( ::GetMessage(&msg, NULL, 0, 0) ) {
		if( !paintManagerUC::translateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

bool paintManagerUC::translateMessage(const LPMSG pMsg)
{
	// Pretranslate Message takes care of system-wide messages, such as
	// tabbing and shortcut key-combos. We'll look for all messages for
	// each window and any child control attached.
	HWND hwndParent = ::GetParent(pMsg->hwnd);
	UINT uStyle = GetWindowStyle(pMsg->hwnd);
	LRESULT lRes = 0;
	//for( int i = 0; i < mc_vecPreMessages.GetSize(); i++ ) {
	//	CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(mc_vecPreMessages[i]);
	//	if( pMsg->hwnd == pT->GetPaintWindow()
	//		|| (hwndParent == pT->GetPaintWindow() && ((uStyle & WS_CHILD) != 0)) )
	//	{
	//		if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) return true;
	//	}
	//}
	std::vector<paintManagerUC *>::iterator iter;
	for (iter = mc_vecPreMessages.begin(); iter != mc_vecPreMessages.end(); ++iter)
	{
		if (pMsg->hwnd == (*iter)->getPaintWindow()
			|| (hwndParent == (*iter)->getPaintWindow() && ((uStyle & WS_CHILD) != 0)))
		{
			if((*iter)->preMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes)) return true;
		}
	}
	return false;
}

controlUC* paintManagerUC::getFocus() const
{
	return m_pFocus;
}

void paintManagerUC::setFocus(controlUC* pControl)
{
	// Paint manager window has focus?
	HWND hFocusWnd = ::GetFocus();
	if( hFocusWnd != m_hWndPaint && pControl != m_pFocus ) ::SetFocus(m_hWndPaint);
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
	}
}

void paintManagerUC::setFocusNeeded(controlUC* pControl)
{
	::SetFocus(m_hWndPaint);
	if( pControl == NULL ) return;
	if( m_pFocus != NULL ) {
		BSEventUC event = { 0 };
		event.Type = BEUCEventKillFocus;
		event.pSender = pControl;
		event.dwTimestamp = ::GetTickCount();
		m_pFocus->Event(event);
		sendNotify(m_pFocus, L"killfocus");
		m_pFocus = NULL;
	}
	BSFindTabInfo info = { 0 };
	info.pFocus = pControl;
	info.bForward = false;
	m_pFocus = m_pRoot->findControl(__findControlFromTab, &info, UCFIND_VISIBLE | UCFIND_ENABLED | UCFIND_ME_FIRST);
	m_bFocusNeeded = true;
	if( m_pRoot != NULL ) m_pRoot->needUpdate();
}

bool paintManagerUC::setTimer(controlUC* pControl, UINT nTimerID, UINT uElapse)
{
	assert(pControl!=NULL);
	assert(uElapse>0);
	for(std::vector<BSTimerInfo *>::iterator iter = m_aTimers.begin(); iter != m_aTimers.end(); ++iter) {
		if( (*iter)->pSender == pControl
			&& (*iter)->hWnd == m_hWndPaint
			&& (*iter)->nLocalID == nTimerID )
		{
			if( (*iter)->bKilled == true ) {
				if( ::SetTimer(m_hWndPaint, (*iter)->uWinTimer, uElapse, NULL) ) {
					(*iter)->bKilled = false;
					return true;
				}

				return false;
			}

			return false;
		}
	}

	m_uTimerID = (++m_uTimerID) % 0xFF;
	if( !::SetTimer(m_hWndPaint, m_uTimerID, uElapse, NULL) ) return FALSE;
	BSTimerInfo* pTimer = new BSTimerInfo;
	if( pTimer == NULL ) return FALSE;
	pTimer->hWnd = m_hWndPaint;
	pTimer->pSender = pControl;
	pTimer->nLocalID = nTimerID;
	pTimer->uWinTimer = m_uTimerID;
	pTimer->bKilled = false;
	m_aTimers.push_back(pTimer);//.Add(pTimer);
	return true;
}

bool paintManagerUC::killTimer(controlUC* pControl, UINT nTimerID)
{
	assert(pControl!=NULL);
	for(std::vector<BSTimerInfo *>::iterator iter = m_aTimers.begin(); iter != m_aTimers.end(); ++iter ) {
		if( (*iter)->pSender == pControl
			&& (*iter)->hWnd == m_hWndPaint
			&& (*iter)->nLocalID == nTimerID )
		{
			if( (*iter)->bKilled == false ) {
				if( ::IsWindow(m_hWndPaint) ) ::KillTimer((*iter)->hWnd, (*iter)->uWinTimer);
				(*iter)->bKilled = true;
				return true;
			}
		}
	}
	return false;
}

void paintManagerUC::killTimer(controlUC* pControl)
{
	assert(pControl!=NULL);
	int count = m_aTimers.size();
	for( int i = 0, j = 0; i < count; i++ ) {
		BSTimerInfo* pTimer = m_aTimers[i - j];
		if( pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint ) {
			if( pTimer->bKilled == false ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
			//m_aTimers.Remove(i);
			m_aTimers.erase(m_aTimers.begin()+i);
			j++;
		}
	}
}

void paintManagerUC::removeAllTimers()
{
	//for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
	//	BSTimerInfo* pTimer = static_cast<BSTimerInfo*>(m_aTimers[i]);
	//	if( pTimer->hWnd == m_hWndPaint )
	//	{
	//		if( pTimer->bKilled == false ) {
	//			if( ::IsWindow(m_hWndPaint) ) ::KillTimer(m_hWndPaint, pTimer->uWinTimer);
	//		}
	//		delete pTimer;
	//	}
	//}

	//m_aTimers.Empty();
	for (std::vector<BSTimerInfo *>::iterator iter = m_aTimers.begin(); iter != m_aTimers.end(); ++iter)
	{
		if((*iter)->bKilled == false)
		{
			if (::IsWindow(m_hWndPaint))
			{
				::KillTimer(m_hWndPaint, (*iter)->uWinTimer);
			}
			delete *iter;
		}
	}
	m_aTimers.clear();
}

void paintManagerUC::setCapture()
{
	::SetCapture(m_hWndPaint);
	m_bMouseCapture = true;
}

void paintManagerUC::releaseCapture()
{
	::ReleaseCapture();
	m_bMouseCapture = false;
}

bool paintManagerUC::isCaptured()
{
	return m_bMouseCapture;
}

bool paintManagerUC::setNextTabControl(bool bForward)
{
	// If we're in the process of restructuring the layout we can delay the
	// focus calulation until the next repaint.
	if( m_bUpdateNeeded && bForward ) {
		m_bFocusNeeded = true;
		::InvalidateRect(m_hWndPaint, NULL, FALSE);
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

bool paintManagerUC::addNotifier(INotifyUC* pNotifier)
{
	//assert(m_aNotifiers.Find(pNotifier)<0);
	//return m_aNotifiers.Add(pNotifier);
	if (std::find(m_aNotifiers.begin(), m_aNotifiers.end(), pNotifier) == m_aNotifiers.end())
	{
		m_aNotifiers.push_back(pNotifier);
		return true;
	}
	return false;
}

bool paintManagerUC::removeNotifier(INotifyUC* pNotifier)
{
	//for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
	//	if( static_cast<INotifyUC*>(m_aNotifiers[i]) == pNotifier ) {
	//		return m_aNotifiers.Remove(i);
	//	}
	//}
	std::vector<INotifyUC *>::iterator iter = std::find(m_aNotifiers.begin(), m_aNotifiers.end(), pNotifier);
	if (iter != m_aNotifiers.end())
	{
		m_aNotifiers.erase(iter);
	}
	return false;
}

bool paintManagerUC::addPreMessageFilter(IMessageFilterUC* pFilter)
{
	//assert(m_aPreMessageFilters.Find(pFilter)<0);
	//return m_aPreMessageFilters.Add(pFilter);
	std::vector<IMessageFilterUC *>::iterator iter = std::find(m_aPreMessageFilters.begin(), m_aPreMessageFilters.end(), pFilter);
	if (iter == m_aPreMessageFilters.end())
	{
		m_aPreMessageFilters.push_back(pFilter);
		return true;
	}
	return false;
}

bool paintManagerUC::removePreMessageFilter(IMessageFilterUC* pFilter)
{
	//for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) {
	//	if( static_cast<IMessageFilterUC*>(m_aPreMessageFilters[i]) == pFilter ) {
	//		return m_aPreMessageFilters.Remove(i);
	//	}
	//}
	for (std::vector<IMessageFilterUC *>::iterator iter = m_aPreMessageFilters.begin(); iter != m_aPreMessageFilters.end(); ++iter)
	{
		if ((*iter ) == pFilter)
		{
			m_aPreMessageFilters.erase(iter);
			return true;
		}
	}
	return false;
}

bool paintManagerUC::addMessageFilter(IMessageFilterUC* pFilter)
{
	//assert(m_aMessageFilters.Find(pFilter)<0);
	//return m_aMessageFilters.Add(pFilter);
	std::vector<IMessageFilterUC *>::iterator iter = std::find(m_aMessageFilters.begin(), m_aMessageFilters.end(), pFilter);
	if (iter == m_aMessageFilters.end())
	{
		m_aMessageFilters.push_back(pFilter);
		return true;
	}
	return false;
}

bool paintManagerUC::removeMessageFilter(IMessageFilterUC* pFilter)
{
	//for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
	//	if( static_cast<IMessageFilterUC*>(m_aMessageFilters[i]) == pFilter ) {
	//		return m_aMessageFilters.Remove(i);
	//	}
	//}
	std::vector<IMessageFilterUC *>::iterator iter = std::find(m_aMessageFilters.begin(), m_aMessageFilters.end(), pFilter);
	if (iter != m_aMessageFilters.end())
	{
		m_aMessageFilters.erase(iter);
		return true;
	}
	return false;
}


void paintManagerUC::sendNotify(controlUC* pControl, PCWSTR pstrMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
{
	BSNotifyUC Msg;
	Msg.pSender = pControl;
	Msg.wsType = pstrMessage;
	Msg.wParam = wParam;
	Msg.lParam = lParam;
	sendNotify(Msg, bAsync);
}

void paintManagerUC::sendNotify(BSNotifyUC& Msg, bool bAsync /*= false*/)
{
	Msg.ptMouse = m_ptLastMousePos;
	Msg.dwTimestamp = ::GetTickCount();
	if( !bAsync ) {
		// Send to all listeners
		//if( Msg.pSender != NULL ) {
		//	Msg.pSender->notify(&Msg);
		//}
		//for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
		//	static_cast<INotifyUC*>(m_aNotifiers[i])->Notify(Msg);
		//}
		for (std::vector<INotifyUC *>::iterator iter = m_aNotifiers.begin(); iter != m_aNotifiers.end(); ++iter)
		{
			(*iter)->notify(Msg);
		}
	}
	else {
		BSNotifyUC *pMsg = new BSNotifyUC;
		pMsg->pSender = Msg.pSender;
		pMsg->wsType = Msg.wsType;
		pMsg->wParam = Msg.wParam;
		pMsg->lParam = Msg.lParam;
		pMsg->ptMouse = Msg.ptMouse;
		pMsg->dwTimestamp = Msg.dwTimestamp;
		m_aAsyncNotify.push_back(pMsg);//.Add(pMsg);
	}
}


DWORD paintManagerUC::getDefaultDisabledColor() const
{
	//if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultDisabledColor();
	return m_dwDefaultDisabledColor;
}

void paintManagerUC::setDefaultDisabledColor(DWORD dwColor)
{
	m_dwDefaultDisabledColor = dwColor;
}

DWORD paintManagerUC::getDefaultFontColor() const
{
	//if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultFontColor();
	return m_dwDefaultFontColor;
}

void paintManagerUC::setDefaultFontColor(DWORD dwColor)
{
	m_dwDefaultFontColor = dwColor;
}

DWORD paintManagerUC::getDefaultLinkFontColor() const
{
	//if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultLinkFontColor();
	return m_dwDefaultLinkFontColor;
}

void paintManagerUC::setDefaultLinkFontColor(DWORD dwColor)
{
	m_dwDefaultLinkFontColor = dwColor;
}

DWORD paintManagerUC::getDefaultLinkHoverFontColor() const
{
	//if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultLinkHoverFontColor();
	return m_dwDefaultLinkHoverFontColor;
}

void paintManagerUC::setDefaultLinkHoverFontColor(DWORD dwColor)
{
	m_dwDefaultLinkHoverFontColor = dwColor;
}

DWORD paintManagerUC::getDefaultSelectedBkColor() const
{
	//if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultSelectedBkColor();
	return m_dwDefaultSelectedBkColor;
}

void paintManagerUC::setDefaultSelectedBkColor(DWORD dwColor)
{
	m_dwDefaultSelectedBkColor = dwColor;
}

BSFontInfoUC* paintManagerUC::getDefaultFontInfo()
{
	//if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultFontInfo();

	if( m_DefaultFontInfo.tm.tmHeight == 0 ) {
		HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, m_DefaultFontInfo.hFont);
		::GetTextMetrics(m_hDcPaint, &m_DefaultFontInfo.tm);
		::SelectObject(m_hDcPaint, hOldFont);
	}
	return &m_DefaultFontInfo;
}

void paintManagerUC::setDefaultFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcscpy(lf.lfFaceName, pStrFontName);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if( bBold ) lf.lfWeight += FW_BOLD;
	if( bUnderline ) lf.lfUnderline = TRUE;
	if( bItalic ) lf.lfItalic = TRUE;
	HFONT hFont = ::CreateFontIndirect(&lf);
	if( hFont == NULL ) return;

	::DeleteObject(m_DefaultFontInfo.hFont);
	m_DefaultFontInfo.hFont = hFont;
	m_DefaultFontInfo.sFontName = pStrFontName;
	m_DefaultFontInfo.iSize = nSize;
	m_DefaultFontInfo.bBold = bBold;
	m_DefaultFontInfo.bUnderline = bUnderline;
	m_DefaultFontInfo.bItalic = bItalic;
	::ZeroMemory(&m_DefaultFontInfo.tm, sizeof(m_DefaultFontInfo.tm));
	if( m_hDcPaint ) {
		HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
		::GetTextMetrics(m_hDcPaint, &m_DefaultFontInfo.tm);
		::SelectObject(m_hDcPaint, hOldFont);
	}
}

DWORD paintManagerUC::getCustomFontCount() const
{
	return m_aCustomFonts.size();
}

HFONT paintManagerUC::addFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcscpy(lf.lfFaceName, pStrFontName);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if( bBold ) lf.lfWeight += FW_BOLD;
	if( bUnderline ) lf.lfUnderline = TRUE;
	if( bItalic ) lf.lfItalic = TRUE;
	HFONT hFont = ::CreateFontIndirect(&lf);
	if( hFont == NULL ) return NULL;

	BSFontInfoUC* pFontInfo = new BSFontInfoUC;
	if( !pFontInfo ) return false;
	::ZeroMemory(pFontInfo, sizeof(BSFontInfoUC));
	pFontInfo->hFont = hFont;
	pFontInfo->sFontName = pStrFontName;
	pFontInfo->iSize = nSize;
	pFontInfo->bBold = bBold;
	pFontInfo->bUnderline = bUnderline;
	pFontInfo->bItalic = bItalic;
	if( m_hDcPaint ) {
		HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
		::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
		::SelectObject(m_hDcPaint, hOldFont);
	}
	//if( !m_aCustomFonts.Add(pFontInfo) ) {
	//	::DeleteObject(hFont);
	//	delete pFontInfo;
	//	return NULL;
	//}
	m_aCustomFonts.push_back(pFontInfo);
	return hFont;
}

HFONT paintManagerUC::addFontAt(int index, PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcscpy(lf.lfFaceName, pStrFontName);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if( bBold ) lf.lfWeight += FW_BOLD;
	if( bUnderline ) lf.lfUnderline = TRUE;
	if( bItalic ) lf.lfItalic = TRUE;
	HFONT hFont = ::CreateFontIndirect(&lf);
	if( hFont == NULL ) return NULL;

	BSFontInfoUC* pFontInfo = new BSFontInfoUC;
	if( !pFontInfo ) return false;
	::ZeroMemory(pFontInfo, sizeof(BSFontInfoUC));
	pFontInfo->hFont = hFont;
	pFontInfo->sFontName = pStrFontName;
	pFontInfo->iSize = nSize;
	pFontInfo->bBold = bBold;
	pFontInfo->bUnderline = bUnderline;
	pFontInfo->bItalic = bItalic;
	if( m_hDcPaint ) {
		HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
		::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
		::SelectObject(m_hDcPaint, hOldFont);
	}
	//if( !m_aCustomFonts.InsertAt(index, pFontInfo) ) {
	//	::DeleteObject(hFont);
	//	delete pFontInfo;
	//	return NULL;
	//}
	m_aCustomFonts.insert(m_aCustomFonts.begin()+index, pFontInfo);
	return hFont;
}

HFONT paintManagerUC::getFont(int index)
{
	if( index < 0 || index >= m_aCustomFonts.size() ) return getDefaultFontInfo()->hFont;
	//BSFontInfoUC* pFontInfo = static_cast<BSFontInfoUC*>(m_aCustomFonts[index]);
	//return pFontInfo->hFont;
	return m_aCustomFonts[index]->hFont;
}

HFONT paintManagerUC::getFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	BSFontInfoUC* pFontInfo = NULL;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<BSFontInfoUC*>(m_aCustomFonts[it]);
	//	if( pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
	//		pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
	//		return pFontInfo->hFont;
	//}
	for (std::vector<BSFontInfoUC *>::iterator iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		if( (*iter)->sFontName == pStrFontName && (*iter)->iSize == nSize && 
			(*iter)->bBold == bBold && (*iter)->bUnderline == bUnderline && (*iter)->bItalic == bItalic) 
			return (*iter)->hFont;
	}
	//if( m_pParentResourcePM ) return m_pParentResourcePM->getFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
	return NULL;
}

bool paintManagerUC::findFont(HFONT hFont)
{
	//BSFontInfoUC* pFontInfo = NULL;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<BSFontInfoUC*>(m_aCustomFonts[it]);
	//	if( pFontInfo->hFont == hFont ) return true;
	//}
	for (std::vector<BSFontInfoUC *>::iterator iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		if((*iter)->hFont == hFont) return true;
	}
	//if( m_pParentResourcePM ) return m_pParentResourcePM->findFont(hFont);
	return false;
}

bool paintManagerUC::findFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	//BSFontInfoUC* pFontInfo = NULL;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<BSFontInfoUC*>(m_aCustomFonts[it]);
	//	if( pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
	//		pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
	//		return true;
	//}
	for (std::vector<BSFontInfoUC *>::iterator iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		if( (*iter)->sFontName == pStrFontName && (*iter)->iSize == nSize && 
			(*iter)->bBold == bBold && (*iter)->bUnderline == bUnderline && (*iter)->bItalic == bItalic) 
			return true;

	}
	//if( m_pParentResourcePM ) return m_pParentResourcePM->findFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
	return false;
}

int paintManagerUC::getFontIndex(HFONT hFont)
{
	BSFontInfoUC* pFontInfo = NULL;
	for( int it = 0; it < m_aCustomFonts.size(); it++ ) {
		if( m_aCustomFonts[it]->hFont == hFont ) return it;
	}
	return -1;
}

int paintManagerUC::getFontIndex(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	BSFontInfoUC* pFontInfo = NULL;
	for( int it = 0; it < m_aCustomFonts.size(); it++ ) {
		if( m_aCustomFonts[it]->sFontName == pStrFontName && m_aCustomFonts[it]->iSize == nSize && 
			m_aCustomFonts[it]->bBold == bBold && m_aCustomFonts[it]->bUnderline == bUnderline && m_aCustomFonts[it]->bItalic == bItalic) 
			return it;
	}
	return -1;
}

bool paintManagerUC::removeFont(HFONT hFont)
{
	//BSFontInfoUC* pFontInfo = NULL;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<BSFontInfoUC*>(m_aCustomFonts[it]);
	//	if( pFontInfo->hFont == hFont ) {
	//		::DeleteObject(pFontInfo->hFont);
	//		delete pFontInfo;
	//		return m_aCustomFonts.Remove(it);
	//	}
	//}
	for (std::vector<BSFontInfoUC *>::iterator iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		if ((*iter)->hFont == hFont)
		{
			::DeleteObject((*iter)->hFont);
			delete *iter;
			m_aCustomFonts.erase(iter);
			return true;
		}
	}
	return false;
}

bool paintManagerUC::removeFontAt(int index)
{
	if( index < 0 || index >= m_aCustomFonts.size() ) return false;
	//BSFontInfoUC* pFontInfo = static_cast<BSFontInfoUC*>(m_aCustomFonts[index]);
	::DeleteObject(m_aCustomFonts[index]->hFont);
	delete m_aCustomFonts[index];
	m_aCustomFonts.erase(m_aCustomFonts.begin()+index);
	return true;
}

void paintManagerUC::removeAllFonts()
{
	//BSFontInfoUC* pFontInfo;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<BSFontInfoUC*>(m_aCustomFonts[it]);
	//	::DeleteObject(pFontInfo->hFont);
	//	delete pFontInfo;
	//}
	//m_aCustomFonts.Empty();
	for (std::vector<BSFontInfoUC *>::iterator iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		::DeleteObject((*iter)->hFont);
		delete *iter;
	}
	m_aCustomFonts.clear();
}

BSFontInfoUC* paintManagerUC::getFontInfo(int index)
{
	if( index < 0 || index >= m_aCustomFonts.size() ) return getDefaultFontInfo();
	//BSFontInfoUC* pFontInfo = static_cast<BSFontInfoUC*>(m_aCustomFonts[index]);
	//if( pFontInfo->tm.tmHeight == 0 ) {
	//	HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
	//	::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
	//	::SelectObject(m_hDcPaint, hOldFont);
	//}
	std::vector<BSFontInfoUC *>::iterator iter;
	for (iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, (*iter)->hFont);
		::GetTextMetrics(m_hDcPaint, &(*iter)->tm);
		::SelectObject(m_hDcPaint, hOldFont);

	}
	return (*iter);
}

BSFontInfoUC* paintManagerUC::getFontInfo(HFONT hFont)
{
	//BSFontInfoUC* pFontInfo = NULL;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<BSFontInfoUC*>(m_aCustomFonts[it]);
	//	if( pFontInfo->hFont == hFont ) {
	//		if( pFontInfo->tm.tmHeight == 0 ) {
	//			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
	//			::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
	//			::SelectObject(m_hDcPaint, hOldFont);
	//		}
	//		return pFontInfo;
	//	}
	//}
	std::vector<BSFontInfoUC *>::iterator iter;
	for (iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		if( (*iter)->hFont == hFont ) {
			if( (*iter)->tm.tmHeight == 0 ) {
				HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, (*iter)->hFont);
				::GetTextMetrics(m_hDcPaint, &(*iter)->tm);
				::SelectObject(m_hDcPaint, hOldFont);
			}
			return (*iter);
		}

	}
	//if( m_pParentResourcePM ) return m_pParentResourcePM->getFontInfo(hFont);
	return getDefaultFontInfo();
}

void paintManagerUC::addDefaultAttributeList(PCWSTR pStrControlName, PCWSTR pStrControlAttrList)
{
	//std::wstring* pDefaultAttr = new std::wstring(pStrControlAttrList);
	//if (pDefaultAttr != NULL)
	//{
	//	if (m_DefaultAttrHash.Find(pStrControlName) == NULL)
	//		m_DefaultAttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr);
	//	else
	//		delete pDefaultAttr;
	//}
	std::map<std::wstring, std::wstring *>::iterator itfind = m_DefaultAttrHash.find(pStrControlName);
	if(itfind == m_DefaultAttrHash.end())
		m_DefaultAttrHash[pStrControlName] = new std::wstring(pStrControlAttrList);
}

PCWSTR paintManagerUC::getDefaultAttributeList(PCWSTR pStrControlName) const
{
	//CStdString* pDefaultAttr = static_cast<CStdString*>(m_DefaultAttrHash.Find(pStrControlName));
	//if( !pDefaultAttr && m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultAttributeList(pStrControlName);

	//if( pDefaultAttr ) return pDefaultAttr->GetData();
	//else return NULL;
	std::map<std::wstring, std::wstring *>::const_iterator itfind = m_DefaultAttrHash.find(pStrControlName); 
	//if (itfind == m_DefaultAttrHash.end() && m_pParentResourcePM)
	//{
	//	return m_pParentResourcePM->getDefaultAttributeList(pStrControlName);
	//}
	if (itfind != m_DefaultAttrHash.end())
	{
		return itfind->second->c_str();
	}
	return nullptr;
}

bool paintManagerUC::removeDefaultAttributeList(PCWSTR pStrControlName)
{
	//CStdString* pDefaultAttr = static_cast<CStdString*>(m_DefaultAttrHash.Find(pStrControlName));
	//if( !pDefaultAttr ) return false;

	//delete pDefaultAttr;
	//return m_DefaultAttrHash.Remove(pStrControlName);
	std::map<std::wstring, std::wstring *>::iterator itfind = m_DefaultAttrHash.find(pStrControlName);// std::find(m_DefaultAttrHash.begin(), m_DefaultAttrHash.end(), pStrControlName);
	if(itfind == m_DefaultAttrHash.end()) return false;

	delete itfind->second;
	m_DefaultAttrHash.erase(itfind);
	return true;
}

const std::map<std::wstring, std::wstring *>& paintManagerUC::getDefaultAttribultes() const
{
	return m_DefaultAttrHash;
}

void paintManagerUC::removeAllDefaultAttributeList()
{
	//CStdString* pDefaultAttr;
	//for( int i = 0; i< m_DefaultAttrHash.GetSize(); i++ ) {
	//	if(LPCTSTR key = m_DefaultAttrHash.GetAt(i)) {
	//		pDefaultAttr = static_cast<CStdString*>(m_DefaultAttrHash.Find(key));
	//		delete pDefaultAttr;
	//	}
	//}
	for (std::map<std::wstring, std::wstring *>::iterator iter = m_DefaultAttrHash.begin(); iter != m_DefaultAttrHash.end(); ++iter)
	{
		delete iter->second;
	}
	m_DefaultAttrHash.clear();
}

controlUC* paintManagerUC::getRoot() const
{
	assert(m_pRoot);
	return m_pRoot;
}

controlUC* paintManagerUC::findControl(POINT pt) const
{
	assert(m_pRoot);
	return m_pRoot->findControl(__findControlFromPoint, &pt, UCFIND_VISIBLE | UCFIND_HITTEST | UCFIND_TOP_FIRST);
}

controlUC* paintManagerUC::findControl(PCWSTR pstrName) const
{
	assert(m_pRoot);
	//return static_cast<controlUC*>(m_mNameHash.Find(pstrName));
	std::map<std::wstring, controlUC *>::const_iterator itfind = m_mNameHash.find(pstrName);// std::find(m_mNameHash.begin(), m_mNameHash.end(), pstrName);
	if (itfind != m_mNameHash.end())
	{
		return itfind->second;
	}
	return nullptr;
}

controlUC* paintManagerUC::findSubControlByPoint(controlUC* pParent, POINT pt) const
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	return pParent->findControl(__findControlFromPoint, &pt, UCFIND_VISIBLE | UCFIND_HITTEST | UCFIND_TOP_FIRST);
}

controlUC* paintManagerUC::findSubControlByName(controlUC* pParent, PCWSTR pstrName) const
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	return pParent->findControl(__findControlFromName, (LPVOID)pstrName, UCFIND_ALL);
}

controlUC* paintManagerUC::findSubControlByClass(controlUC* pParent, PCWSTR pstrClass, int iIndex)
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	m_aFoundControls.resize(iIndex + 1);
	return pParent->findControl(__findControlFromClass, (LPVOID)pstrClass, UCFIND_ALL);
}

std::vector<void*>* paintManagerUC::findSubControlsByClass(controlUC* pParent, PCWSTR pstrClass)
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	m_aFoundControls.clear();
	pParent->findControl(__findControlsFromClass, (LPVOID)pstrClass, UCFIND_ALL);
	return &m_aFoundControls;
}

std::vector<void*>* paintManagerUC::getSubControlsByClass()
{
	return &m_aFoundControls;
}

controlUC* CALLBACK paintManagerUC::__findControlFromNameHash(controlUC* pThis, LPVOID pData)
{
	paintManagerUC* pManager = static_cast<paintManagerUC*>(pData);
	const std::wstring& sName = pThis->getName();
	if( sName.empty() ) return NULL;
	// Add this control to the hash list
	pManager->m_mNameHash[sName] = pThis;
	return NULL; // Attempt to add all controls
}

controlUC* CALLBACK paintManagerUC::__findControlFromCount(controlUC* /*pThis*/, LPVOID pData)
{
	int* pnCount = static_cast<int*>(pData);
	(*pnCount)++;
	return NULL;  // Count all controls
}

controlUC* CALLBACK paintManagerUC::__findControlFromPoint(controlUC* pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	return ::PtInRect(&pThis->getPos(), *pPoint) ? pThis : NULL;
}

controlUC* CALLBACK paintManagerUC::__findControlFromTab(controlUC* pThis, LPVOID pData)
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

controlUC* CALLBACK paintManagerUC::__findControlFromShortcut(controlUC* pThis, LPVOID pData)
{
	if( !pThis->isVisible() ) return NULL; 
	BSFindShortCut* pFS = static_cast<BSFindShortCut*>(pData);
	if( pFS->ch == toupper(pThis->getShortcut()) ) pFS->bPickNext = true;
	if( _tcsstr(pThis->getClass(), L"LabelUI") != NULL ) return NULL;   // Labels never get focus!
	return pFS->bPickNext ? pThis : NULL;
}

controlUC* CALLBACK paintManagerUC::__findControlFromUpdate(controlUC* pThis, LPVOID pData)
{
	return pThis->isUpdateNeeded() ? pThis : NULL;
}

controlUC* CALLBACK paintManagerUC::__findControlFromName(controlUC* pThis, LPVOID pData)
{
	PCWSTR pstrName = static_cast<PCWSTR>(pData);
	const std::wstring& sName = pThis->getName();
	if( sName.empty() ) return NULL;
	return (_tcsicmp(sName.c_str(), pstrName) == 0) ? pThis : NULL;
}

controlUC* CALLBACK paintManagerUC::__findControlFromClass(controlUC* pThis, LPVOID pData)
{
	PCWSTR pstrType = static_cast<PCWSTR>(pData);
	PCWSTR pType = pThis->getClass();
	std::vector<void *>* pFoundControls = pThis->getManager()->getSubControlsByClass();
	if( _tcscmp(pstrType, L"*") == 0 || _tcscmp(pstrType, pType) == 0 ) {
		int iIndex = -1;
		while( pFoundControls->at(++iIndex) != NULL ) ;
		if( iIndex < pFoundControls->size() ) (*pFoundControls)[iIndex] = pThis;// pFoundControls->setAt(iIndex, pThis);
	}
	if( pFoundControls->at(pFoundControls->size() - 1) != NULL ) return pThis; 
	return NULL;
}

controlUC* CALLBACK paintManagerUC::__findControlsFromClass(controlUC* pThis, LPVOID pData)
{
	PCWSTR pstrType = static_cast<PCWSTR>(pData);
	PCWSTR pType = pThis->getClass();
	if( _tcscmp(pstrType, L"*") == 0 || _tcscmp(pstrType, pType) == 0 ) 
		pThis->getManager()->getSubControlsByClass()->push_back((LPVOID)pThis);//Add((LPVOID)pThis);
	return NULL;
}



}