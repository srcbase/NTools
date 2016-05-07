#include "StdAfx.h"
#include "managerUI.h"
#include <zmouse.h>

namespace wui {


static void free(std::vector<controlUI *> &vecControl)
{
	for (std::vector<controlUI *>::iterator iter = vecControl.begin(); iter != vecControl.end(); ++iter)
	{
		delete *iter;
	}
	vecControl.clear();
}
/////////////////////////////////////////////////////////////////////////////////////
//
//

static UINT mapKeyState()
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
HINSTANCE paintManagerUI::m_hInstance = NULL;
HINSTANCE paintManagerUI::m_hResourceInstance = NULL;
stringex paintManagerUI::m_pStrResourcePath;
stringex paintManagerUI::m_pStrResourceZip;
short paintManagerUI::m_H = 180;
short paintManagerUI::m_S = 100;
short paintManagerUI::m_L = 100;
std::vector<paintManagerUI *> paintManagerUI::m_aPreMessages;


paintManagerUI::paintManagerUI() :
m_hWndPaint(NULL),
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
	m_bAlphaBackground(false),
	m_pParentResourcePM(NULL)
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

paintManagerUI::~paintManagerUI()
{
	//TRACE_CLASS_END(this)
	// Delete the control-tree structures
	//for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
	//for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) delete static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
	//m_mNameHash.Resize(0);
	for (std::vector<controlUI *>::iterator iter = m_aDelayedCleanup.begin(); iter != m_aDelayedCleanup.end(); ++iter)
	{
		delete *iter;
	}
	for (std::vector<TNotifyUI *>::iterator iter = m_aAsyncNotify.begin(); iter != m_aAsyncNotify.end(); ++iter)
	{
		delete *iter;
	}
	m_mNameHash.clear();
	delete m_pRoot;

	::DeleteObject(m_DefaultFontInfo.hFont);
	removeAllFonts();
	removeAllImages();
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
	//m_aPreMessages.Remove(m_aPreMessages.Find(this));
	std::vector<paintManagerUI *>::iterator iter = std::find(m_aPreMessages.begin(), m_aPreMessages.end(), this);
	if (iter != m_aPreMessages.end())
	{
		m_aPreMessages.erase(iter);
	}
}

void paintManagerUI::init(HWND hWnd)
{
	assert(::IsWindow(hWnd));
	// Remember the window context we came from
	m_hWndPaint = hWnd;
	m_hDcPaint = ::GetDC(hWnd);
	// We'll want to filter messages globally too
	m_aPreMessages.push_back(this);
}

HINSTANCE paintManagerUI::getInstance()
{
	return m_hInstance;
}

stringex paintManagerUI::getInstancePath()
{
	if( m_hInstance == NULL ) return stringex();

	TCHAR tszModule[MAX_PATH + 1] = { 0 };
	::GetModuleFileName(m_hInstance, tszModule, MAX_PATH);
	stringex sInstancePath = tszModule;
	//int pos = sInstancePath.ReverseFind(L'\\');
	//if( pos >= 0 ) sInstancePath = sInstancePath.Left(pos + 1);
	int pos = sInstancePath.find_last_of(L"\\");
	if( pos >= 0 ) sInstancePath = sInstancePath.left(pos + 1);
	return sInstancePath;
}

stringex paintManagerUI::getCurrentPath()
{
	TCHAR tszModule[MAX_PATH + 1] = { 0 };
	::GetCurrentDirectory(MAX_PATH, tszModule);
	return tszModule;
}

HINSTANCE paintManagerUI::getResourceDll()
{
	if( m_hResourceInstance == NULL ) return m_hInstance;
	return m_hResourceInstance;
}

const stringex& paintManagerUI::getResourcePath()
{
	return m_pStrResourcePath;
}

const stringex& paintManagerUI::getResourceZip()
{
	return m_pStrResourceZip;
}

void paintManagerUI::setInstance(HINSTANCE hInst)
{
	m_hInstance = hInst;
}

void paintManagerUI::setCurrentPath(PCWSTR pStrPath)
{
	::SetCurrentDirectory(pStrPath);
}

void paintManagerUI::setResourceDll(HINSTANCE hInst)
{
	m_hResourceInstance = hInst;
}

void paintManagerUI::setResourcePath(PCWSTR pStrPath)
{
	m_pStrResourcePath = pStrPath;
	if( m_pStrResourcePath.empty() ) return;
	wchar_t cEnd = m_pStrResourcePath.at(m_pStrResourcePath.length() - 1);
	if( cEnd != L'\\' && cEnd != L'/' ) m_pStrResourcePath += L'\\';
}

void paintManagerUI::setResourceZip(PCWSTR pStrPath)
{
	m_pStrResourceZip = pStrPath;
}

void paintManagerUI::getHSL(short* H, short* S, short* L)
{
	*H = m_H;
	*S = m_S;
	*L = m_L;
}

void paintManagerUI::setHSL(bool bUseHSL, short H, short S, short L)
{
	if( H == m_H && S == m_S && L == m_L ) return;
	m_H = CLAMP(H, 0, 360);
	m_S = CLAMP(S, 0, 200);
	m_L = CLAMP(L, 0, 200);
	//for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
	//	CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
	//	if( pManager != NULL && pManager->GetRoot() != NULL )
	//		pManager->GetRoot()->Invalidate();
	//}
	for (std::vector<paintManagerUI *>::iterator iter = m_aPreMessages.begin(); iter != m_aPreMessages.end(); ++iter)
	{
		if((*iter)->getRoot() != nullptr)
			(*iter)->getRoot()->invalidate();
	}
}

void paintManagerUI::reloadSkin()
{
	//for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
	//	CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
	//	pManager->ReloadAllImages();
	//}
	for (std::vector<paintManagerUI *>::iterator iter = m_aPreMessages.begin(); iter != m_aPreMessages.end(); ++iter)
	{
		(*iter)->reloadAllImages();
	}
}

HWND paintManagerUI::getPaintWindow() const
{
	return m_hWndPaint;
}

HWND paintManagerUI::getTooltipWindow() const
{
	return m_hwndTooltip;
}

HDC paintManagerUI::getPaintDC() const
{
	return m_hDcPaint;
}

POINT paintManagerUI::getMousePos() const
{
	return m_ptLastMousePos;
}

SIZE paintManagerUI::getClientSize() const
{
	RECT rcClient = { 0 };
	::GetClientRect(m_hWndPaint, &rcClient);
	return size(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
}

SIZE paintManagerUI::getInitSize()
{
	return m_szInitWindowSize;
}

void paintManagerUI::setInitSize(int cx, int cy)
{
	m_szInitWindowSize.cx = cx;
	m_szInitWindowSize.cy = cy;
	if( m_pRoot == NULL && m_hWndPaint != NULL ) {
		::SetWindowPos(m_hWndPaint, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

RECT& paintManagerUI::getSizeBox()
{
	return m_rcSizeBox;
}

void paintManagerUI::setSizeBox(RECT& rcSizeBox)
{
	m_rcSizeBox = rcSizeBox;
}

RECT& paintManagerUI::getCaptionRect()
{
	return m_rcCaption;
}

void paintManagerUI::setCaptionRect(RECT& rcCaption)
{
	m_rcCaption = rcCaption;
}

SIZE paintManagerUI::getRoundCorner() const
{
	return m_szRoundCorner;
}

void paintManagerUI::setRoundCorner(int cx, int cy)
{
	m_szRoundCorner.cx = cx;
	m_szRoundCorner.cy = cy;
}

SIZE paintManagerUI::getMinInfo() const
{
	return m_szMinWindow;
}

void paintManagerUI::setMinInfo(int cx, int cy)
{
	assert(cx>=0 && cy>=0);
	m_szMinWindow.cx = cx;
	m_szMinWindow.cy = cy;
}

SIZE paintManagerUI::getMaxInfo() const
{
	return m_szMaxWindow;
}

void paintManagerUI::setMaxInfo(int cx, int cy)
{
	assert(cx>=0 && cy>=0);
	m_szMaxWindow.cx = cx;
	m_szMaxWindow.cy = cy;
}

void paintManagerUI::setTransparent(int nOpacity)
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

void paintManagerUI::setBackgroundTransparent(bool bTrans)
{
	m_bAlphaBackground = bTrans;
}

bool paintManagerUI::isShowUpdateRect() const
{
	return m_bShowUpdateRect;
}

void paintManagerUI::setShowUpdateRect(bool show)
{
	m_bShowUpdateRect = show;
}

bool paintManagerUI::preMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& /*lRes*/)
{
	//for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) 
	//{
	//	bool bHandled = false;
	//	LRESULT lResult = static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
	//	if( bHandled ) {
	//		return true;
	//	}
	//}
	for (std::vector<IMessageFilterUI *>::iterator iter = m_aPreMessageFilters.begin(); iter != m_aPreMessageFilters.end(); ++iter)
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
			FINDSHORTCUT fs = { 0 };
			fs.ch = toupper((int)wParam);
			controlUI* pControl = m_pRoot->findControl(__findControlFromShortcut, &fs, UIFIND_ENABLED | UIFIND_ME_FIRST | UIFIND_TOP_FIRST);
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
				TEventUI event = { 0 };
				event.Type = UIEVENT_SYSKEY;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = mapKeyState();
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
			}
		}
		break;
	}
	return false;
}

bool paintManagerUI::messageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
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

	//TNotifyUI* pMsg = NULL;
	//while( pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0)) ) {
	//	m_aAsyncNotify.Remove(0);
	//	if( pMsg->pSender != NULL ) {
	//		if( pMsg->pSender->OnNotify ) pMsg->pSender->OnNotify(pMsg);
	//	}
	//	for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
	//		static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
	//	}
	//	delete pMsg;
	//}
	TNotifyUI* pMsg = NULL;
	while(m_aAsyncNotify.size() > 0)
	{
		pMsg = m_aAsyncNotify.front();
		if(pMsg->pSender != nullptr && pMsg->pSender->onNotify) pMsg->pSender->onNotify(pMsg);
		for (std::vector<INotifyUI *>::iterator iter = m_aNotifiers.begin(); iter != m_aNotifiers.end(); ++iter)
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
	//	LRESULT lResult = static_cast<IMessageFilterUI*>(m_aMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
	//	if( bHandled ) {
	//		lRes = lResult;
	//		return true;
	//	}
	//}
	for (std::vector<IMessageFilterUI *>::iterator iter = m_aMessageFilters.begin(); iter != m_aMessageFilters.end(); ++iter)
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
	case WM_APP + 1:
		{
			//for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) 
			//	delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
			//m_aDelayedCleanup.Empty();
			free(m_aDelayedCleanup);
		}
		break;
	case WM_CLOSE:
		{
			// Make sure all matching "closing" events are sent
			TEventUI event = { 0 };
			event.ptMouse = m_ptLastMousePos;
			event.dwTimestamp = ::GetTickCount();
			if( m_pEventHover != NULL ) {
				event.Type = UIEVENT_MOUSELEAVE;
				event.pSender = m_pEventHover;
				m_pEventHover->Event(event);
			}
			if( m_pEventClick != NULL ) {
				event.Type = UIEVENT_BUTTONUP;
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
						controlUI* pControl = NULL;
						while( pControl = m_pRoot->findControl(__findControlFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST) ) {
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
				m_pRoot->doPaint(m_hDcOffscreen, ps.rcPaint);
				//for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
				//	CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
				//	pPostPaintControl->DoPostPaint(m_hDcOffscreen, ps.rcPaint);
				//}
				for (std::vector<controlUI *>::iterator iter = m_aPostPaintControls.begin(); iter != m_aPostPaintControls.end(); ++iter)
				{
					(*iter)->doPostPaint(m_hDcOffscreen, ps.rcPaint);
				}
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
				m_pRoot->doPaint(ps.hdc, ps.rcPaint);
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
			m_pRoot->doPaint(hDC, rcClient);
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
				TEventUI event = { 0 };
				event.Type = UIEVENT_WINDOWSIZE;
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
			//	const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
			//	if( pTimer->hWnd == m_hWndPaint && pTimer->uWinTimer == LOWORD(wParam) && pTimer->bKilled == false) {
			//		TEventUI event = { 0 };
			//		event.Type = UIEVENT_TIMER;
			//		event.pSender = pTimer->pSender;
			//		event.wParam = pTimer->nLocalID;
			//		event.dwTimestamp = ::GetTickCount();
			//		pTimer->pSender->Event(event);
			//		break;
			//	}
			//}
			for (std::vector<TIMERINFO *>::iterator iter = m_aTimers.begin(); iter != m_aTimers.end(); ++iter)
			{
				if ((*iter)->hWnd == m_hWndPaint && (*iter)->uWinTimer == LOWORD(wParam) && (*iter)->bKilled == false)
				{
					TEventUI event = { 0 };
					event.Type = UIEVENT_TIMER;
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
			controlUI* pHover = findControl(pt);
			if( pHover == NULL ) break;
			// Generate mouse hover event
			if( m_pEventHover != NULL ) {
				TEventUI event = { 0 };
				event.ptMouse = pt;
				event.Type = UIEVENT_MOUSEHOVER;
				event.pSender = m_pEventHover;
				event.dwTimestamp = ::GetTickCount();
				m_pEventHover->Event(event);
			}
			// Create tooltip information
			stringex sToolTip = pHover->getToolTip();
			if( sToolTip.empty() ) return true;
			::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
			m_ToolTip.cbSize = sizeof(TOOLINFO);
			m_ToolTip.uFlags = TTF_IDISHWND;
			m_ToolTip.hwnd = m_hWndPaint;
			m_ToolTip.uId = (UINT_PTR) m_hWndPaint;
			m_ToolTip.hinst = m_hInstance;
			m_ToolTip.lpszText = const_cast<PWSTR>(sToolTip.c_str());
			m_ToolTip.rect = pHover->getPos();
			if( m_hwndTooltip == NULL ) {
				m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
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
			controlUI* pNewHover = findControl(pt);
			if( pNewHover != NULL && pNewHover->getManager() != this ) break;
			TEventUI event = { 0 };
			event.ptMouse = pt;
			event.dwTimestamp = ::GetTickCount();
			if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
				event.Type = UIEVENT_MOUSELEAVE;
				event.pSender = m_pEventHover;
				m_pEventHover->Event(event);
				m_pEventHover = NULL;
				if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
			}
			if( pNewHover != m_pEventHover && pNewHover != NULL ) {
				event.Type = UIEVENT_MOUSEENTER;
				event.pSender = pNewHover;
				pNewHover->Event(event);
				m_pEventHover = pNewHover;
			}
			if( m_pEventClick != NULL ) {
				event.Type = UIEVENT_MOUSEMOVE;
				event.pSender = m_pEventClick;
				m_pEventClick->Event(event);
			}
			else if( pNewHover != NULL ) {
				event.Type = UIEVENT_MOUSEMOVE;
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
			controlUI* pControl = findControl(pt);
			if( pControl == NULL ) break;
			if( pControl->getManager() != this ) break;
			m_pEventClick = pControl;
			pControl->setFocus();
			setCapture();
			TEventUI event = { 0 };
			event.Type = UIEVENT_BUTTONDOWN;
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
			controlUI* pControl = findControl(pt);
			if( pControl == NULL ) break;
			if( pControl->getManager() != this ) break;
			setCapture();
			TEventUI event = { 0 };
			event.Type = UIEVENT_DBLCLICK;
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
			TEventUI event = { 0 };
			event.Type = UIEVENT_BUTTONUP;
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
			controlUI* pControl = findControl(pt);
			if( pControl == NULL ) break;
			if( pControl->getManager() != this ) break;
			pControl->setFocus();
			setCapture();
			TEventUI event = { 0 };
			event.Type = UIEVENT_RBUTTONDOWN;
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
			TEventUI event = { 0 };
			event.Type = UIEVENT_CONTEXTMENU;
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
			controlUI* pControl = findControl(pt);
			if( pControl == NULL ) break;
			if( pControl->getManager() != this ) break;
			int zDelta = (int) (short) HIWORD(wParam);
			TEventUI event = { 0 };
			event.Type = UIEVENT_SCROLLWHEEL;
			event.pSender = pControl;
			event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
			event.lParam = lParam;
			event.wKeyState = mapKeyState();
			event.dwTimestamp = ::GetTickCount();
			pControl->Event(event);

			// Let's make sure that the scroll item below the cursor is the same as before...
			::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
		}
		break;
	case WM_CHAR:
		{
			if( m_pFocus == NULL ) break;
			TEventUI event = { 0 };
			event.Type = UIEVENT_CHAR;
			event.chKey = (TCHAR)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = mapKeyState();
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
		}
		break;
	case WM_KEYDOWN:
		{
			if( m_pFocus == NULL ) break;
			TEventUI event = { 0 };
			event.Type = UIEVENT_KEYDOWN;
			event.chKey = (TCHAR)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = mapKeyState();
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->Event(event);
			m_pEventKey = m_pFocus;
		}
		break;
	case WM_KEYUP:
		{
			if( m_pEventKey == NULL ) break;
			TEventUI event = { 0 };
			event.Type = UIEVENT_KEYUP;
			event.chKey = (TCHAR)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = mapKeyState();
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
			controlUI* pControl = findControl(pt);
			if( pControl == NULL ) break;
			if( (pControl->getControlFlags() & UIFLAG_SETCURSOR) == 0 ) break;
			TEventUI event = { 0 };
			event.Type = UIEVENT_SETCURSOR;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = mapKeyState();
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
	//while( pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0)) ) {
	//	m_aAsyncNotify.Remove(0);
	//	if( pMsg->pSender != NULL ) {
	//		if( pMsg->pSender->OnNotify ) pMsg->pSender->OnNotify(pMsg);
	//	}
	//	for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
	//		static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
	//	}
	//	delete pMsg;
	//}
	while(m_aAsyncNotify.size() > 0)
	{
		pMsg = m_aAsyncNotify.front();
		if(pMsg->pSender != nullptr && pMsg->pSender->onNotify) pMsg->pSender->onNotify(pMsg);
		for (std::vector<INotifyUI *>::iterator iter = m_aNotifiers.begin(); iter != m_aNotifiers.end(); ++iter)
		{
			(*iter)->notify(*pMsg);
		}
		delete pMsg;
		m_aAsyncNotify.erase(m_aAsyncNotify.begin());
	}
	return false;
}

void paintManagerUI::needUpdate()
{
	m_bUpdateNeeded = true;
}

void paintManagerUI::invalidate(RECT& rcItem)
{
	::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
}

bool paintManagerUI::attachDialog(controlUI* pControl)
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
	if( m_pRoot != NULL ) {
		m_aPostPaintControls.clear();
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

bool paintManagerUI::initControls(controlUI* pControl, controlUI* pParent /*= NULL*/)
{
	assert(pControl);
	if( pControl == NULL ) return false;
	pControl->setManager(this, pParent != NULL ? pParent : pControl->getParent(), true);
	pControl->findControl(__findControlFromNameHash, this, UIFIND_ALL);
	return true;
}

void paintManagerUI::reapObjects(controlUI* pControl)
{
	if( pControl == m_pEventKey ) m_pEventKey = NULL;
	if( pControl == m_pEventHover ) m_pEventHover = NULL;
	if( pControl == m_pEventClick ) m_pEventClick = NULL;
	if( pControl == m_pFocus ) m_pFocus = NULL;
	killTimer(pControl);
	const stringex& sName = pControl->getName();
	if( !sName.empty() ) {
		if( pControl == findControl(sName.c_str()) ) m_mNameHash.erase(sName);// m_mNameHash.Remove(sName);
	}
	//for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) {
	//	TNotifyUI* pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
	//	if( pMsg->pSender == pControl ) pMsg->pSender = NULL;
	//}  

	for (std::vector<TNotifyUI *>::iterator iter = m_aAsyncNotify.begin(); iter != m_aAsyncNotify.end(); ++iter)
	{
		if((*iter)->pSender == pControl) (*iter)->pSender = nullptr;
	}
}

bool paintManagerUI::addOptionGroup(PCWSTR pStrGroupName, controlUI* pControl)
{
	//LPVOID lp = (LPVOID)m_mOptionGroup.find(stringex(pStrGroupName));
	std::map<stringex, std::vector<controlUI *>*>::iterator itfind = m_mOptionGroup.find(pStrGroupName);
	if( itfind != m_mOptionGroup.end() ) {
		std::vector<controlUI *>* aOptionGroup = itfind->second;// static_cast<std::vector<controlUI *>*>(lp);
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
		std::vector<controlUI *>* aOptionGroup = new std::vector<controlUI *>();
		aOptionGroup->push_back(pControl);
		m_mOptionGroup[pStrGroupName] = aOptionGroup;
	}
	return true;
}

std::vector<controlUI*>* paintManagerUI::getOptionGroup(PCWSTR pStrGroupName)
{
	//LPVOID lp = m_mOptionGroup.find(pStrGroupName);
	//if( lp ) return static_cast<CStdPtrArray*>(lp);

	std::map<stringex, std::vector<controlUI *>*>::iterator itfind = m_mOptionGroup.find(pStrGroupName);
	if (itfind != m_mOptionGroup.end())
	{
		return itfind->second;
	}
	return NULL;
}

void paintManagerUI::removeOptionGroup(PCWSTR pStrGroupName, controlUI* pControl)
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
	std::map<stringex, std::vector<controlUI *>*>::iterator itfind = m_mOptionGroup.find(pStrGroupName);
	if (itfind != m_mOptionGroup.end())
	{
		std::vector<controlUI *>* aOptionGroup = itfind->second;
		for (std::vector<controlUI *>::iterator iter = aOptionGroup->begin(); iter != aOptionGroup->end(); ++iter)
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

void paintManagerUI::removeAllOptionGroups()
{
	//std::vector<controlUI *>* aOptionGroup;
	//for( int i = 0; i< m_mOptionGroup.GetSize(); i++ ) {
	//	if(LPCTSTR key = m_mOptionGroup.GetAt(i)) {
	//		aOptionGroup = static_cast<CStdPtrArray*>(m_mOptionGroup.Find(key));
	//		delete aOptionGroup;
	//	}
	//}
	std::map<stringex, std::vector<controlUI *>*>::iterator iter ;
	for (iter = m_mOptionGroup.begin(); iter != m_mOptionGroup.end(); ++iter)
	{
		delete iter->second;
	}
	m_mOptionGroup.clear();
}

void paintManagerUI::messageLoop()
{
	MSG msg = { 0 };
	while( ::GetMessage(&msg, NULL, 0, 0) ) {
		if( !paintManagerUI::translateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

bool paintManagerUI::translateMessage(const LPMSG pMsg)
{
	// Pretranslate Message takes care of system-wide messages, such as
	// tabbing and shortcut key-combos. We'll look for all messages for
	// each window and any child control attached.
	HWND hwndParent = ::GetParent(pMsg->hwnd);
	UINT uStyle = GetWindowStyle(pMsg->hwnd);
	LRESULT lRes = 0;
	//for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
	//	CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
	//	if( pMsg->hwnd == pT->GetPaintWindow()
	//		|| (hwndParent == pT->GetPaintWindow() && ((uStyle & WS_CHILD) != 0)) )
	//	{
	//		if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) return true;
	//	}
	//}
	std::vector<paintManagerUI *>::iterator iter;
	for (iter = m_aPreMessages.begin(); iter != m_aPreMessages.end(); ++iter)
	{
		if (pMsg->hwnd == (*iter)->getPaintWindow()
			|| (hwndParent == (*iter)->getPaintWindow() && ((uStyle & WS_CHILD) != 0)))
		{
			if((*iter)->preMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes)) return true;
		}
	}
	return false;
}

controlUI* paintManagerUI::getFocus() const
{
	return m_pFocus;
}

void paintManagerUI::setFocus(controlUI* pControl)
{
	// Paint manager window has focus?
	HWND hFocusWnd = ::GetFocus();
	if( hFocusWnd != m_hWndPaint && pControl != m_pFocus ) ::SetFocus(m_hWndPaint);
	// Already has focus?
	if( pControl == m_pFocus ) return;
	// Remove focus from old control
	if( m_pFocus != NULL ) 
	{
		TEventUI event = { 0 };
		event.Type = UIEVENT_KILLFOCUS;
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
		TEventUI event = { 0 };
		event.Type = UIEVENT_SETFOCUS;
		event.pSender = pControl;
		event.dwTimestamp = ::GetTickCount();
		m_pFocus->Event(event);
		sendNotify(m_pFocus, L"setfocus");
	}
}

void paintManagerUI::setFocusNeeded(controlUI* pControl)
{
	::SetFocus(m_hWndPaint);
	if( pControl == NULL ) return;
	if( m_pFocus != NULL ) {
		TEventUI event = { 0 };
		event.Type = UIEVENT_KILLFOCUS;
		event.pSender = pControl;
		event.dwTimestamp = ::GetTickCount();
		m_pFocus->Event(event);
		sendNotify(m_pFocus, L"killfocus");
		m_pFocus = NULL;
	}
	FINDTABINFO info = { 0 };
	info.pFocus = pControl;
	info.bForward = false;
	m_pFocus = m_pRoot->findControl(__findControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
	m_bFocusNeeded = true;
	if( m_pRoot != NULL ) m_pRoot->needUpdate();
}

bool paintManagerUI::setTimer(controlUI* pControl, UINT nTimerID, UINT uElapse)
{
	assert(pControl!=NULL);
	assert(uElapse>0);
	for(std::vector<TIMERINFO *>::iterator iter = m_aTimers.begin(); iter != m_aTimers.end(); ++iter) {
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
	TIMERINFO* pTimer = new TIMERINFO;
	if( pTimer == NULL ) return FALSE;
	pTimer->hWnd = m_hWndPaint;
	pTimer->pSender = pControl;
	pTimer->nLocalID = nTimerID;
	pTimer->uWinTimer = m_uTimerID;
	pTimer->bKilled = false;
	m_aTimers.push_back(pTimer);//.Add(pTimer);
	return true;
}

bool paintManagerUI::killTimer(controlUI* pControl, UINT nTimerID)
{
	assert(pControl!=NULL);
	for(std::vector<TIMERINFO *>::iterator iter = m_aTimers.begin(); iter != m_aTimers.end(); ++iter ) {
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

void paintManagerUI::killTimer(controlUI* pControl)
{
	assert(pControl!=NULL);
	int count = m_aTimers.size();
	for( int i = 0, j = 0; i < count; i++ ) {
		TIMERINFO* pTimer = m_aTimers[i - j];
		if( pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint ) {
			if( pTimer->bKilled == false ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
			//m_aTimers.Remove(i);
			m_aTimers.erase(m_aTimers.begin()+i);
			j++;
		}
	}
}

void paintManagerUI::removeAllTimers()
{
	//for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
	//	TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
	//	if( pTimer->hWnd == m_hWndPaint )
	//	{
	//		if( pTimer->bKilled == false ) {
	//			if( ::IsWindow(m_hWndPaint) ) ::KillTimer(m_hWndPaint, pTimer->uWinTimer);
	//		}
	//		delete pTimer;
	//	}
	//}

	//m_aTimers.Empty();
	for (std::vector<TIMERINFO *>::iterator iter = m_aTimers.begin(); iter != m_aTimers.end(); ++iter)
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

void paintManagerUI::setCapture()
{
	::SetCapture(m_hWndPaint);
	m_bMouseCapture = true;
}

void paintManagerUI::releaseCapture()
{
	::ReleaseCapture();
	m_bMouseCapture = false;
}

bool paintManagerUI::isCaptured()
{
	return m_bMouseCapture;
}

bool paintManagerUI::setNextTabControl(bool bForward)
{
	// If we're in the process of restructuring the layout we can delay the
	// focus calulation until the next repaint.
	if( m_bUpdateNeeded && bForward ) {
		m_bFocusNeeded = true;
		::InvalidateRect(m_hWndPaint, NULL, FALSE);
		return true;
	}
	// Find next/previous tabbable control
	FINDTABINFO info1 = { 0 };
	info1.pFocus = m_pFocus;
	info1.bForward = bForward;
	controlUI* pControl = m_pRoot->findControl(__findControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
	if( pControl == NULL ) {  
		if( bForward ) {
			// Wrap around
			FINDTABINFO info2 = { 0 };
			info2.pFocus = bForward ? NULL : info1.pLast;
			info2.bForward = bForward;
			pControl = m_pRoot->findControl(__findControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		}
		else {
			pControl = info1.pLast;
		}
	}
	if( pControl != NULL ) setFocus(pControl);
	m_bFocusNeeded = false;
	return true;
}

bool paintManagerUI::addNotifier(INotifyUI* pNotifier)
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

bool paintManagerUI::removeNotifier(INotifyUI* pNotifier)
{
	//for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
	//	if( static_cast<INotifyUI*>(m_aNotifiers[i]) == pNotifier ) {
	//		return m_aNotifiers.Remove(i);
	//	}
	//}
	std::vector<INotifyUI *>::iterator iter = std::find(m_aNotifiers.begin(), m_aNotifiers.end(), pNotifier);
	if (iter != m_aNotifiers.end())
	{
		m_aNotifiers.erase(iter);
	}
	return false;
}

bool paintManagerUI::addPreMessageFilter(IMessageFilterUI* pFilter)
{
	//assert(m_aPreMessageFilters.Find(pFilter)<0);
	//return m_aPreMessageFilters.Add(pFilter);
	std::vector<IMessageFilterUI *>::iterator iter = std::find(m_aPreMessageFilters.begin(), m_aPreMessageFilters.end(), pFilter);
	if (iter == m_aPreMessageFilters.end())
	{
		m_aPreMessageFilters.push_back(pFilter);
		return true;
	}
	return false;
}

bool paintManagerUI::removePreMessageFilter(IMessageFilterUI* pFilter)
{
	//for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) {
	//	if( static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i]) == pFilter ) {
	//		return m_aPreMessageFilters.Remove(i);
	//	}
	//}
	for (std::vector<IMessageFilterUI *>::iterator iter = m_aPreMessageFilters.begin(); iter != m_aPreMessageFilters.end(); ++iter)
	{
		if ((*iter ) == pFilter)
		{
			m_aPreMessageFilters.erase(iter);
			return true;
		}
	}
	return false;
}

bool paintManagerUI::addMessageFilter(IMessageFilterUI* pFilter)
{
	//assert(m_aMessageFilters.Find(pFilter)<0);
	//return m_aMessageFilters.Add(pFilter);
	std::vector<IMessageFilterUI *>::iterator iter = std::find(m_aMessageFilters.begin(), m_aMessageFilters.end(), pFilter);
	if (iter == m_aMessageFilters.end())
	{
		m_aMessageFilters.push_back(pFilter);
		return true;
	}
	return false;
}

bool paintManagerUI::removeMessageFilter(IMessageFilterUI* pFilter)
{
	//for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
	//	if( static_cast<IMessageFilterUI*>(m_aMessageFilters[i]) == pFilter ) {
	//		return m_aMessageFilters.Remove(i);
	//	}
	//}
	std::vector<IMessageFilterUI *>::iterator iter = std::find(m_aMessageFilters.begin(), m_aMessageFilters.end(), pFilter);
	if (iter != m_aMessageFilters.end())
	{
		m_aMessageFilters.erase(iter);
		return true;
	}
	return false;
}

int paintManagerUI::getPostPaintCount() const
{
	return m_aPostPaintControls.size();
}

bool paintManagerUI::addPostPaint(controlUI* pControl)
{
	//assert(m_aPostPaintControls.Find(pControl) < 0);
	//return m_aPostPaintControls.Add(pControl);
	std::vector<controlUI *>::iterator iter = std::find(m_aPostPaintControls.begin(), m_aPostPaintControls.end(), pControl);
	if (iter == m_aPostPaintControls.end())
	{
		m_aPostPaintControls.push_back(pControl);
		return true;
	}
	return false;
}

bool paintManagerUI::removePostPaint(controlUI* pControl)
{
	//for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
	//	if( static_cast<CControlUI*>(m_aPostPaintControls[i]) == pControl ) {
	//		return m_aPostPaintControls.Remove(i);
	//	}
	//}
	//return false;
	std::vector<controlUI *>::iterator iter = std::find(m_aPostPaintControls.begin(), m_aPostPaintControls.end(), pControl);
	if (iter != m_aPostPaintControls.end())
	{
		m_aPostPaintControls.erase(iter);
		return true;
	}
	return false;

}

bool paintManagerUI::setPostPaintIndex(controlUI* pControl, int iIndex)
{
	removePostPaint(pControl);
	m_aPostPaintControls.insert(m_aPostPaintControls.begin()+ iIndex, pControl);
	return true;
}

void paintManagerUI::addDelayedCleanup(controlUI* pControl)
{
	pControl->setManager(this, NULL, false);
	m_aDelayedCleanup.push_back(pControl);//.Add(pControl);
	::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
}

void paintManagerUI::sendNotify(controlUI* pControl, PCWSTR pstrMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
{
	TNotifyUI Msg;
	Msg.pSender = pControl;
	Msg.sType = pstrMessage;
	Msg.wParam = wParam;
	Msg.lParam = lParam;
	sendNotify(Msg, bAsync);
}

void paintManagerUI::sendNotify(TNotifyUI& Msg, bool bAsync /*= false*/)
{
	Msg.ptMouse = m_ptLastMousePos;
	Msg.dwTimestamp = ::GetTickCount();
	if( !bAsync ) {
		// Send to all listeners
		if( Msg.pSender != NULL ) {
			if( Msg.pSender->onNotify ) Msg.pSender->onNotify(&Msg);
		}
		//for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
		//	static_cast<INotifyUI*>(m_aNotifiers[i])->Notify(Msg);
		//}
		for (std::vector<INotifyUI *>::iterator iter = m_aNotifiers.begin(); iter != m_aNotifiers.end(); ++iter)
		{
			(*iter)->notify(Msg);
		}
	}
	else {
		TNotifyUI *pMsg = new TNotifyUI;
		pMsg->pSender = Msg.pSender;
		pMsg->sType = Msg.sType;
		pMsg->wParam = Msg.wParam;
		pMsg->lParam = Msg.lParam;
		pMsg->ptMouse = Msg.ptMouse;
		pMsg->dwTimestamp = Msg.dwTimestamp;
		m_aAsyncNotify.push_back(pMsg);//.Add(pMsg);
	}
}

bool paintManagerUI::useParentResource(paintManagerUI* pm)
{
	if( pm == NULL ) {
		m_pParentResourcePM = NULL;
		return true;
	}
	if( pm == this ) return false;

	paintManagerUI* pParentPM = pm->getParentResource();
	while( pParentPM ) {
		if( pParentPM == this ) return false;
		pParentPM = pParentPM->getParentResource();
	}
	m_pParentResourcePM = pm;
	return true;
}

paintManagerUI* paintManagerUI::getParentResource() const
{
	return m_pParentResourcePM;
}

DWORD paintManagerUI::getDefaultDisabledColor() const
{
	if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultDisabledColor();
	return m_dwDefaultDisabledColor;
}

void paintManagerUI::setDefaultDisabledColor(DWORD dwColor)
{
	m_dwDefaultDisabledColor = dwColor;
}

DWORD paintManagerUI::getDefaultFontColor() const
{
	if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultFontColor();
	return m_dwDefaultFontColor;
}

void paintManagerUI::setDefaultFontColor(DWORD dwColor)
{
	m_dwDefaultFontColor = dwColor;
}

DWORD paintManagerUI::getDefaultLinkFontColor() const
{
	if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultLinkFontColor();
	return m_dwDefaultLinkFontColor;
}

void paintManagerUI::setDefaultLinkFontColor(DWORD dwColor)
{
	m_dwDefaultLinkFontColor = dwColor;
}

DWORD paintManagerUI::getDefaultLinkHoverFontColor() const
{
	if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultLinkHoverFontColor();
	return m_dwDefaultLinkHoverFontColor;
}

void paintManagerUI::setDefaultLinkHoverFontColor(DWORD dwColor)
{
	m_dwDefaultLinkHoverFontColor = dwColor;
}

DWORD paintManagerUI::getDefaultSelectedBkColor() const
{
	if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultSelectedBkColor();
	return m_dwDefaultSelectedBkColor;
}

void paintManagerUI::setDefaultSelectedBkColor(DWORD dwColor)
{
	m_dwDefaultSelectedBkColor = dwColor;
}

TFontInfo* paintManagerUI::getDefaultFontInfo()
{
	if( m_pParentResourcePM ) return m_pParentResourcePM->getDefaultFontInfo();

	if( m_DefaultFontInfo.tm.tmHeight == 0 ) {
		HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, m_DefaultFontInfo.hFont);
		::GetTextMetrics(m_hDcPaint, &m_DefaultFontInfo.tm);
		::SelectObject(m_hDcPaint, hOldFont);
	}
	return &m_DefaultFontInfo;
}

void paintManagerUI::setDefaultFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
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

DWORD paintManagerUI::getCustomFontCount() const
{
	return m_aCustomFonts.size();
}

HFONT paintManagerUI::addFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
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

	TFontInfo* pFontInfo = new TFontInfo;
	if( !pFontInfo ) return false;
	::ZeroMemory(pFontInfo, sizeof(TFontInfo));
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

HFONT paintManagerUI::addFontAt(int index, PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
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

	TFontInfo* pFontInfo = new TFontInfo;
	if( !pFontInfo ) return false;
	::ZeroMemory(pFontInfo, sizeof(TFontInfo));
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

HFONT paintManagerUI::getFont(int index)
{
	if( index < 0 || index >= m_aCustomFonts.size() ) return getDefaultFontInfo()->hFont;
	//TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
	//return pFontInfo->hFont;
	return m_aCustomFonts[index]->hFont;
}

HFONT paintManagerUI::getFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	TFontInfo* pFontInfo = NULL;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
	//	if( pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
	//		pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
	//		return pFontInfo->hFont;
	//}
	for (std::vector<TFontInfo *>::iterator iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		if( (*iter)->sFontName == pStrFontName && (*iter)->iSize == nSize && 
			(*iter)->bBold == bBold && (*iter)->bUnderline == bUnderline && (*iter)->bItalic == bItalic) 
			return (*iter)->hFont;
	}
	if( m_pParentResourcePM ) return m_pParentResourcePM->getFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
	return NULL;
}

bool paintManagerUI::findFont(HFONT hFont)
{
	//TFontInfo* pFontInfo = NULL;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
	//	if( pFontInfo->hFont == hFont ) return true;
	//}
	for (std::vector<TFontInfo *>::iterator iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		if((*iter)->hFont == hFont) return true;
	}
	if( m_pParentResourcePM ) return m_pParentResourcePM->findFont(hFont);
	return false;
}

bool paintManagerUI::findFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	//TFontInfo* pFontInfo = NULL;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
	//	if( pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
	//		pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
	//		return true;
	//}
	for (std::vector<TFontInfo *>::iterator iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		if( (*iter)->sFontName == pStrFontName && (*iter)->iSize == nSize && 
			(*iter)->bBold == bBold && (*iter)->bUnderline == bUnderline && (*iter)->bItalic == bItalic) 
			return true;

	}
	if( m_pParentResourcePM ) return m_pParentResourcePM->findFont(pStrFontName, nSize, bBold, bUnderline, bItalic);
	return false;
}

int paintManagerUI::getFontIndex(HFONT hFont)
{
	TFontInfo* pFontInfo = NULL;
	for( int it = 0; it < m_aCustomFonts.size(); it++ ) {
		if( m_aCustomFonts[it]->hFont == hFont ) return it;
	}
	return -1;
}

int paintManagerUI::getFontIndex(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	TFontInfo* pFontInfo = NULL;
	for( int it = 0; it < m_aCustomFonts.size(); it++ ) {
		if( m_aCustomFonts[it]->sFontName == pStrFontName && m_aCustomFonts[it]->iSize == nSize && 
			m_aCustomFonts[it]->bBold == bBold && m_aCustomFonts[it]->bUnderline == bUnderline && m_aCustomFonts[it]->bItalic == bItalic) 
			return it;
	}
	return -1;
}

bool paintManagerUI::removeFont(HFONT hFont)
{
	//TFontInfo* pFontInfo = NULL;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
	//	if( pFontInfo->hFont == hFont ) {
	//		::DeleteObject(pFontInfo->hFont);
	//		delete pFontInfo;
	//		return m_aCustomFonts.Remove(it);
	//	}
	//}
	for (std::vector<TFontInfo *>::iterator iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
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

bool paintManagerUI::removeFontAt(int index)
{
	if( index < 0 || index >= m_aCustomFonts.size() ) return false;
	//TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
	::DeleteObject(m_aCustomFonts[index]->hFont);
	delete m_aCustomFonts[index];
	m_aCustomFonts.erase(m_aCustomFonts.begin()+index);
	return true;
}

void paintManagerUI::removeAllFonts()
{
	//TFontInfo* pFontInfo;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
	//	::DeleteObject(pFontInfo->hFont);
	//	delete pFontInfo;
	//}
	//m_aCustomFonts.Empty();
	for (std::vector<TFontInfo *>::iterator iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		::DeleteObject((*iter)->hFont);
		delete *iter;
	}
	m_aCustomFonts.clear();
}

TFontInfo* paintManagerUI::getFontInfo(int index)
{
	if( index < 0 || index >= m_aCustomFonts.size() ) return getDefaultFontInfo();
	//TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[index]);
	//if( pFontInfo->tm.tmHeight == 0 ) {
	//	HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
	//	::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
	//	::SelectObject(m_hDcPaint, hOldFont);
	//}
	std::vector<TFontInfo *>::iterator iter;
	for (iter = m_aCustomFonts.begin(); iter != m_aCustomFonts.end(); ++iter)
	{
		HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, (*iter)->hFont);
		::GetTextMetrics(m_hDcPaint, &(*iter)->tm);
		::SelectObject(m_hDcPaint, hOldFont);

	}
	return (*iter);
}

TFontInfo* paintManagerUI::getFontInfo(HFONT hFont)
{
	//TFontInfo* pFontInfo = NULL;
	//for( int it = 0; it < m_aCustomFonts.GetSize(); it++ ) {
	//	pFontInfo = static_cast<TFontInfo*>(m_aCustomFonts[it]);
	//	if( pFontInfo->hFont == hFont ) {
	//		if( pFontInfo->tm.tmHeight == 0 ) {
	//			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
	//			::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
	//			::SelectObject(m_hDcPaint, hOldFont);
	//		}
	//		return pFontInfo;
	//	}
	//}
	std::vector<TFontInfo *>::iterator iter;
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
	if( m_pParentResourcePM ) return m_pParentResourcePM->getFontInfo(hFont);
	return getDefaultFontInfo();
}

const TImageInfo* paintManagerUI::getImage(PCWSTR bitmap)
{
	//TImageInfo* data = static_cast<TImageInfo*>(m_mImageHash.find(bitmap));
	//if( !data && m_pParentResourcePM ) return m_pParentResourcePM->GetImage(bitmap);
	//else return data;
	std::map<stringex, TImageInfo *>::iterator itfind = m_mImageHash.find(bitmap);// std::find(m_mImageHash.begin(), m_mImageHash.end(), bitmap);
	if(itfind == m_mImageHash.end() && m_pParentResourcePM)
		return m_pParentResourcePM->getImage(bitmap);
	if(itfind != m_mImageHash.end())
		return itfind->second;
	return NULL;
}

const TImageInfo* paintManagerUI::getImageEx(PCWSTR bitmap, PCWSTR type, DWORD mask)
{
	//TImageInfo* data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
	//if( !data ) {
	//	if( AddImage(bitmap, type, mask) ) {
	//		data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
	//	}
	//}

	//return data;
	std::map<stringex, TImageInfo *>::iterator itfind = m_mImageHash.find(bitmap);//std::find(m_mImageHash.begin(), m_mImageHash.end(), bitmap);
	if (itfind == m_mImageHash.end())
	{
		if (addImage(bitmap, type, mask))
		{
			itfind = m_mImageHash.find(bitmap);//std::find(m_mImageHash.begin(), m_mImageHash.end(), bitmap);
		}
	}
	if(itfind != m_mImageHash.end())
		return itfind->second;
	return NULL;
}

const TImageInfo* paintManagerUI::addImage(PCWSTR bitmap, PCWSTR type, DWORD mask)
{
	TImageInfo* data = NULL;
	if( type != NULL ) {
		if( isdigit(*bitmap) ) {
			PWSTR pstr = NULL;
			int iIndex = _tcstol(bitmap, &pstr, 10);
			data = renderEngine::loadImage(iIndex, type, mask);
		}
	}
	else {
		data = renderEngine::loadImage(bitmap, NULL, mask);
	}

	if( !data ) return NULL;
	if( type != NULL ) data->sResType = type;
	data->dwMask = mask;
	//if( !m_mImageHash.Insert(bitmap, data) ) {
	//	::DeleteObject(data->hBitmap);
	//	delete data;
	//}
	std::map<stringex, TImageInfo *>::iterator itfind = m_mImageHash.find(bitmap);//std::find(m_mImageHash.begin(), m_mImageHash.end(), bitmap);
	//m_mImageHash.insert(itfind, std::pair<stringex, TImageInfo *>(, data));
	m_mImageHash[bitmap] = data;
	return data;
}

const TImageInfo* paintManagerUI::addImage(PCWSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha)
{
	if( hBitmap == NULL || iWidth <= 0 || iHeight <= 0 ) return NULL;

	TImageInfo* data = new TImageInfo;
	data->hBitmap = hBitmap;
	data->nX = iWidth;
	data->nY = iHeight;
	data->alphaChannel = bAlpha;
	//data->sResType = _T("");
	data->dwMask = 0;
	//if( !m_mImageHash.Insert(bitmap, data) ) {
	//	::DeleteObject(data->hBitmap);
	//	delete data;
	//}
	m_mImageHash[bitmap] = data;

	return data;
}

bool paintManagerUI::removeImage(PCWSTR bitmap)
{
	const TImageInfo* data = getImage(bitmap);
	if( !data ) return false;

	::DeleteObject(data->hBitmap);
	delete data;

	//return m_mImageHash.Remove(bitmap);
	m_mImageHash.erase(bitmap);
	return true;
}

void paintManagerUI::removeAllImages()
{
	//TImageInfo* data;
	//for( int i = 0; i< m_mImageHash.GetSize(); i++ ) {
	//	if(LPCTSTR key = m_mImageHash.GetAt(i)) {
	//		data = static_cast<TImageInfo*>(m_mImageHash.Find(key));
	//		::DeleteObject(data->hBitmap);
	//		delete data;
	//	}
	//}
	for (std::map<stringex, TImageInfo *>::iterator iter = m_mImageHash.begin(); iter != m_mImageHash.end(); ++iter)
	{
		::DeleteObject(iter->second->hBitmap);
		delete iter->second;
	}
	m_mImageHash.clear();
}

void paintManagerUI::reloadAllImages()
{
	bool bRedraw = false;
	//TImageInfo* data;
	TImageInfo* pNewData;
	/*for( int i = 0; i< m_mImageHash.GetSize(); i++ ) {
		if(LPCTSTR bitmap = m_mImageHash.GetAt(i)) {
			data = static_cast<TImageInfo*>(m_mImageHash.Find(bitmap));
			if( data != NULL ) {
				if( !data->sResType.IsEmpty() ) {
					if( isdigit(*bitmap) ) {
						LPTSTR pstr = NULL;
						int iIndex = _tcstol(bitmap, &pstr, 10);
						pNewData = CRenderEngine::LoadImage(iIndex, data->sResType.GetData(), data->dwMask);
					}
				}
				else {
					pNewData = CRenderEngine::LoadImage(bitmap, NULL, data->dwMask);
				}
				if( pNewData == NULL ) continue;

				if( data->hBitmap != NULL ) ::DeleteObject(data->hBitmap);
				data->hBitmap = pNewData->hBitmap;
				data->nX = pNewData->nX;
				data->nY = pNewData->nY;
				data->alphaChannel = pNewData->alphaChannel;

				delete pNewData;
				bRedraw = true;
			}
		}
	}*/
	for (std::map<stringex, TImageInfo *>::iterator iter = m_mImageHash.begin(); iter != m_mImageHash.end(); ++iter)
	{
		if (!iter->first.empty())
		{
			if (isdigit(*iter->first.c_str()))
			{
				PWSTR pstr = NULL;
				int iIndex = _tcstol(iter->first.c_str(), &pstr, 10);
				pNewData = renderEngine::loadImage(iIndex, iter->second->sResType.data(), iter->second->dwMask);
			}
			else {
				pNewData = renderEngine::loadImage(iter->first.c_str(), NULL, iter->second->dwMask);
			}
			if( pNewData == NULL ) continue;

			if( iter->second->hBitmap != NULL ) ::DeleteObject(iter->second->hBitmap);
			iter->second->hBitmap = pNewData->hBitmap;
			iter->second->nX = pNewData->nX;
			iter->second->nY = pNewData->nY;
			iter->second->alphaChannel = pNewData->alphaChannel;

			delete pNewData;
			bRedraw = true;
		}
	}
	if( bRedraw && m_pRoot ) m_pRoot->invalidate();
}

void paintManagerUI::addDefaultAttributeList(PCWSTR pStrControlName, PCWSTR pStrControlAttrList)
{
	//stringex* pDefaultAttr = new stringex(pStrControlAttrList);
	//if (pDefaultAttr != NULL)
	//{
	//	if (m_DefaultAttrHash.Find(pStrControlName) == NULL)
	//		m_DefaultAttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr);
	//	else
	//		delete pDefaultAttr;
	//}
	std::map<stringex, stringex *>::iterator itfind = m_DefaultAttrHash.find(pStrControlName);
	if(itfind == m_DefaultAttrHash.end())
		m_DefaultAttrHash[pStrControlName] = new stringex(pStrControlAttrList);
}

PCWSTR paintManagerUI::getDefaultAttributeList(PCWSTR pStrControlName) const
{
	//CStdString* pDefaultAttr = static_cast<CStdString*>(m_DefaultAttrHash.Find(pStrControlName));
	//if( !pDefaultAttr && m_pParentResourcePM ) return m_pParentResourcePM->GetDefaultAttributeList(pStrControlName);

	//if( pDefaultAttr ) return pDefaultAttr->GetData();
	//else return NULL;
	std::map<stringex, stringex *>::const_iterator itfind = m_DefaultAttrHash.find(pStrControlName); 
	if (itfind == m_DefaultAttrHash.end() && m_pParentResourcePM)
	{
		return m_pParentResourcePM->getDefaultAttributeList(pStrControlName);
	}
	if (itfind != m_DefaultAttrHash.end())
	{
		return itfind->second->c_str();
	}
	return nullptr;
}

bool paintManagerUI::removeDefaultAttributeList(PCWSTR pStrControlName)
{
	//CStdString* pDefaultAttr = static_cast<CStdString*>(m_DefaultAttrHash.Find(pStrControlName));
	//if( !pDefaultAttr ) return false;

	//delete pDefaultAttr;
	//return m_DefaultAttrHash.Remove(pStrControlName);
	std::map<stringex, stringex *>::iterator itfind = m_DefaultAttrHash.find(pStrControlName);// std::find(m_DefaultAttrHash.begin(), m_DefaultAttrHash.end(), pStrControlName);
	if(itfind == m_DefaultAttrHash.end()) return false;

	delete itfind->second;
	m_DefaultAttrHash.erase(itfind);
	return true;
}

const std::map<stringex, stringex *>& paintManagerUI::getDefaultAttribultes() const
{
	return m_DefaultAttrHash;
}

void paintManagerUI::removeAllDefaultAttributeList()
{
	//CStdString* pDefaultAttr;
	//for( int i = 0; i< m_DefaultAttrHash.GetSize(); i++ ) {
	//	if(LPCTSTR key = m_DefaultAttrHash.GetAt(i)) {
	//		pDefaultAttr = static_cast<CStdString*>(m_DefaultAttrHash.Find(key));
	//		delete pDefaultAttr;
	//	}
	//}
	for (std::map<stringex, stringex *>::iterator iter = m_DefaultAttrHash.begin(); iter != m_DefaultAttrHash.end(); ++iter)
	{
		delete iter->second;
	}
	m_DefaultAttrHash.clear();
}

controlUI* paintManagerUI::getRoot() const
{
	assert(m_pRoot);
	return m_pRoot;
}

controlUI* paintManagerUI::findControl(POINT pt) const
{
	assert(m_pRoot);
	return m_pRoot->findControl(__findControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

controlUI* paintManagerUI::findControl(PCWSTR pstrName) const
{
	assert(m_pRoot);
	//return static_cast<controlUI*>(m_mNameHash.Find(pstrName));
	std::map<stringex, controlUI *>::const_iterator itfind = m_mNameHash.find(pstrName);// std::find(m_mNameHash.begin(), m_mNameHash.end(), pstrName);
	if (itfind != m_mNameHash.end())
	{
		return itfind->second;
	}
	return nullptr;
}

controlUI* paintManagerUI::findSubControlByPoint(controlUI* pParent, POINT pt) const
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	return pParent->findControl(__findControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

controlUI* paintManagerUI::findSubControlByName(controlUI* pParent, PCWSTR pstrName) const
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	return pParent->findControl(__findControlFromName, (LPVOID)pstrName, UIFIND_ALL);
}

controlUI* paintManagerUI::findSubControlByClass(controlUI* pParent, PCWSTR pstrClass, int iIndex)
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	m_aFoundControls.resize(iIndex + 1);
	return pParent->findControl(__findControlFromClass, (LPVOID)pstrClass, UIFIND_ALL);
}

std::vector<void*>* paintManagerUI::findSubControlsByClass(controlUI* pParent, PCWSTR pstrClass)
{
	if( pParent == NULL ) pParent = getRoot();
	assert(pParent);
	m_aFoundControls.clear();
	pParent->findControl(__findControlsFromClass, (LPVOID)pstrClass, UIFIND_ALL);
	return &m_aFoundControls;
}

std::vector<void*>* paintManagerUI::getSubControlsByClass()
{
	return &m_aFoundControls;
}

controlUI* CALLBACK paintManagerUI::__findControlFromNameHash(controlUI* pThis, LPVOID pData)
{
	paintManagerUI* pManager = static_cast<paintManagerUI*>(pData);
	const stringex& sName = pThis->getName();
	if( sName.empty() ) return NULL;
	// Add this control to the hash list
	pManager->m_mNameHash[sName] = pThis;
	return NULL; // Attempt to add all controls
}

controlUI* CALLBACK paintManagerUI::__findControlFromCount(controlUI* /*pThis*/, LPVOID pData)
{
	int* pnCount = static_cast<int*>(pData);
	(*pnCount)++;
	return NULL;  // Count all controls
}

controlUI* CALLBACK paintManagerUI::__findControlFromPoint(controlUI* pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	return ::PtInRect(&pThis->getPos(), *pPoint) ? pThis : NULL;
}

controlUI* CALLBACK paintManagerUI::__findControlFromTab(controlUI* pThis, LPVOID pData)
{
	FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
	if( pInfo->pFocus == pThis ) {
		if( pInfo->bForward ) pInfo->bNextIsIt = true;
		return pInfo->bForward ? NULL : pInfo->pLast;
	}
	if( (pThis->getControlFlags() & UIFLAG_TABSTOP) == 0 ) return NULL;
	pInfo->pLast = pThis;
	if( pInfo->bNextIsIt ) return pThis;
	if( pInfo->pFocus == NULL ) return pThis;
	return NULL;  // Examine all controls
}

controlUI* CALLBACK paintManagerUI::__findControlFromShortcut(controlUI* pThis, LPVOID pData)
{
	if( !pThis->isVisible() ) return NULL; 
	FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
	if( pFS->ch == toupper(pThis->getShortcut()) ) pFS->bPickNext = true;
	if( _tcsstr(pThis->getClass(), L"LabelUI") != NULL ) return NULL;   // Labels never get focus!
	return pFS->bPickNext ? pThis : NULL;
}

controlUI* CALLBACK paintManagerUI::__findControlFromUpdate(controlUI* pThis, LPVOID pData)
{
	return pThis->isUpdateNeeded() ? pThis : NULL;
}

controlUI* CALLBACK paintManagerUI::__findControlFromName(controlUI* pThis, LPVOID pData)
{
	PCWSTR pstrName = static_cast<PCWSTR>(pData);
	const stringex& sName = pThis->getName();
	if( sName.empty() ) return NULL;
	return (_tcsicmp(sName.c_str(), pstrName) == 0) ? pThis : NULL;
}

controlUI* CALLBACK paintManagerUI::__findControlFromClass(controlUI* pThis, LPVOID pData)
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

controlUI* CALLBACK paintManagerUI::__findControlsFromClass(controlUI* pThis, LPVOID pData)
{
	PCWSTR pstrType = static_cast<PCWSTR>(pData);
	PCWSTR pType = pThis->getClass();
	if( _tcscmp(pstrType, L"*") == 0 || _tcscmp(pstrType, pType) == 0 ) 
		pThis->getManager()->getSubControlsByClass()->push_back((LPVOID)pThis);//Add((LPVOID)pThis);
	return NULL;
}


}