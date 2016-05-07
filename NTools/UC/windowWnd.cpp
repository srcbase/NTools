#include "windowWnd.h"
#include "drawManager.h"
namespace wuc{


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	windowWnd类	2014-06-21
//	消息窗口的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////

windowWnd::windowWnd(void)
	: m_hWnd(NULL)
	, m_OldWndProc(::DefWindowProc), m_bSubclassed(false)
	//, m_beAnswerState(BEAnswerStateNone)
{

}

windowWnd::~windowWnd(void)
{
	//gtc::app::writeVSLogs(L"free windowWnd ");
}

HWND windowWnd::getHWND() const
{
	return m_hWnd;
}
windowWnd::operator HWND() const
{
	return m_hWnd;
}

bool windowWnd::registerWindowClass()
{

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = getClassStyle();
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL; // LoadIcon( drawManager::getInstance(), IDI_APPLICATION );
	wc.lpfnWndProc = windowWnd::__wndProc;
	wc.hInstance = drawManager::getInstance();
	wc.hCursor = ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	wc.hbrBackground = (HBRUSH)GetStockObject(/*NULL_BRUSH*/WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = getWindowClassName();
	//bool bl = UnregisterClass(getWindowClassName(), drawManager::getInstance());
	ATOM ret = ::RegisterClassEx(&wc);
	//DWORD dw = ::GetLastError();
	assert(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool windowWnd::registerSuperclass()
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	if( !::GetClassInfoEx(NULL, getSuperClassName(), &wc) ) {
		if( !::GetClassInfoEx(drawManager::getInstance(), getSuperClassName(), &wc) ) {
			assert(!"Unable to locate window class");
			return NULL;
		}
	}
	m_OldWndProc = wc.lpfnWndProc;
	wc.lpfnWndProc = windowWnd::__controlProc;
	wc.hInstance = drawManager::getInstance();
	wc.lpszClassName = getWindowClassName();
	ATOM ret = ::RegisterClassEx(&wc);
	assert(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}
HWND windowWnd::create(HWND hwndParent, const RECT &rc, PCWSTR pstrName /*= NULL*/)
{
	//RECT rc = {0};
	return create(hwndParent, pstrName, getWindowStyle(), getWindowExStyle(), rc, nullptr);
}
//HWND windowWnd::create(HWND hwndParent, PCWSTR pstrName, const RECT &rc)
//{
//	return create(hwndParent, pstrName, getWindowStyle(), getWindowExStyle(), rc, nullptr);
//}

HWND windowWnd::create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT &rc, HMENU hMenu)
{
	return create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
}

HWND windowWnd::create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
{
	assert(drawManager::getInstance());
	if( getSuperClassName() != NULL && !registerSuperclass() ) return NULL;
	if( getSuperClassName() == NULL && !registerWindowClass() ) return NULL;
	m_hWnd = ::CreateWindowEx(dwExStyle, getWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, drawManager::getInstance(), this);
	DWORD dw = GetLastError();
	assert(m_hWnd!=NULL);
	
	return m_hWnd;
}
//
//HWND windowWnd::create(HWND hwndParent, WNDPROC fnWndProc, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu /*= NULL*/)
//{
//	//if(!registerWindowClass(fnWndProc) ) return NULL;
//
//	if( getSuperClassName() != NULL && !registerSuperclass() ) return NULL;
//	if( getSuperClassName() == NULL && !registerWindowClass() ) return NULL;
//
//	m_hWnd = ::CreateWindowEx(dwExStyle, getWindowClassName(), pstrName, dwStyle, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hwndParent, hMenu, drawManager::getInstance(), this);
//	assert(m_hWnd!=NULL);
//
//	return m_hWnd;
//
//}

HWND windowWnd::subclass(HWND hWnd)
{
	assert(::IsWindow(hWnd));
	assert(m_hWnd==NULL);
	m_OldWndProc = SubclassWindow(hWnd, __wndProc);
	if( m_OldWndProc == NULL ) return NULL;
	m_bSubclassed = true;
	m_hWnd = hWnd;
	return m_hWnd;
}

void windowWnd::unsubclass()
{
	assert(::IsWindow(m_hWnd));
	if( !::IsWindow(m_hWnd) ) return;
	if( !m_bSubclassed ) return;
	SubclassWindow(m_hWnd, m_OldWndProc);
	m_OldWndProc = ::DefWindowProc;
	m_bSubclassed = false;
}

void windowWnd::setIcon(UINT nRes)
{
	HICON hIcon = (HICON)::LoadImage(drawManager::getInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	assert(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);
	hIcon = (HICON)::LoadImage(drawManager::getInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	assert(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) FALSE, (LPARAM) hIcon);
}

bool windowWnd::isVisible() const
{
	assert(m_hWnd && IsWindow(m_hWnd));
	return ::IsWindowVisible(m_hWnd) == TRUE;
}
bool windowWnd::isEnabled() const
{
	assert(m_hWnd && IsWindow(m_hWnd));
	return ::IsWindowEnabled(m_hWnd) == TRUE;
}
void windowWnd::setEnabled(bool enabled /*= true*/)
{
	assert(m_hWnd && IsWindow(m_hWnd));
	::EnableWindow(m_hWnd, enabled);
}

void windowWnd::showWindow(bool bShow /*= true*/, bool bTakeFocus /*= true*/)
{
	assert(::IsWindow(m_hWnd));
	if( !::IsWindow(m_hWnd) ) return;
	::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
	//::UpdateWindow(m_hWnd);
}
BEAnswerState windowWnd::showModal(const RECT *prcParent /*= NULL*/)
{
	assert(::IsWindow(m_hWnd));
	BEAnswerState beAnswerState = BEAnswerStateNone;
	HWND hWndParent = GetWindowOwner(m_hWnd);
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	centerWindow(prcParent);
	::EnableWindow(hWndParent, FALSE);
	//::SetForegroundWindow(m_hWnd);
	//::SetWindowPos(hWndParent, m_hWnd/*HWND_TOP*/, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	//::SetWindowPos(m_hWnd, HWND_TOPMOST, 0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	MSG msg = { 0 };
	while( ::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) {
		if( msg.message == WM_CLOSE && msg.hwnd == m_hWnd ) {
			beAnswerState = (BEAnswerState)msg.wParam;
			::EnableWindow(hWndParent, TRUE);
			::SetFocus(hWndParent);
		}
		if( !drawManager::translateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if( msg.message == WM_QUIT ) break;
	}
	::EnableWindow(hWndParent, TRUE);
	::SetFocus(hWndParent);
	if( msg.message == WM_QUIT ) ::PostQuitMessage(msg.wParam);
	return beAnswerState;
}
void windowWnd::close(BEAnswerState answerState /*= BEAnswerStateOK*/)
{
	assert(::IsWindow(m_hWnd));
	if( !::IsWindow(m_hWnd) ) return;
	postMessage(WM_CLOSE, (WPARAM)answerState, 0L);
}
void windowWnd::centerWindow(const RECT *prcParent /*= NULL*/)
{
	assert(::IsWindow(m_hWnd));
	assert((GetWindowStyle(m_hWnd)&WS_CHILD)==0);
	RECT rcDlg = { 0 };
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea = { 0 };
	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
	RECT rcCenter = { 0 };
	if (prcParent && !isEmpty(*prcParent))
	{
		rcCenter = *prcParent;
	}
	else
	{
		HWND hWndParent = ::GetParent(m_hWnd);
		HWND hWndCenter = ::GetWindowOwner(m_hWnd);
		if( hWndCenter == NULL ) rcCenter = rcArea; else ::GetWindowRect(hWndCenter, &rcCenter);

	}

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// The dialog is outside the screen, move it inside
	if( xLeft < rcArea.left ) xLeft = rcArea.left;
	else if( xLeft + DlgWidth > rcArea.right ) xLeft = rcArea.right - DlgWidth;
	if( yTop < rcArea.top ) yTop = rcArea.top;
	else if( yTop + DlgHeight > rcArea.bottom ) yTop = rcArea.bottom - DlgHeight;
	::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

RECT windowWnd::getWindowPos() const
{
	assert(::IsWindow(m_hWnd));
	RECT rc = {0};
	GetWindowRect(m_hWnd, &rc);
	return rc;
}
void windowWnd::setWindowPos(RECT rt)
{
	RECT rtpos = getWindowPos();
	if(rtpos == rt) return;
	::SetWindowPos(m_hWnd, nullptr, rt.left, rt.top, rt.right-rt.left, rt.bottom-rt.top,  SWP_NOZORDER | SWP_NOACTIVATE);
}

void windowWnd::setCapture()
{
	::SetCapture(m_hWnd);
}

void windowWnd::releaseCapture()
{
	::ReleaseCapture();
}

bool windowWnd::modifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags /*= 0*/)
{
	assert(::IsWindow(m_hWnd));

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if(dwStyle == dwNewStyle)
		return false;

	::SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);
	if(nFlags != 0)
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}

	return true;
}
bool windowWnd::modifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags /*= 0*/)
{
	assert(::IsWindow(m_hWnd));

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if(dwStyle == dwNewStyle)
		return false;

	::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewStyle);
	if(nFlags != 0)
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}

	return true;
}
//BEAnswerState windowWnd::getAnswerState() const
//{
//	return m_beAnswerState;
//}
//void windowWnd::setAnswerState(BEAnswerState state)
//{
//	m_beAnswerState = state;
//}

LRESULT windowWnd::sendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	assert(::IsWindow(m_hWnd));
	return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
} 

LRESULT windowWnd::postMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	assert(::IsWindow(m_hWnd));
	return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

void windowWnd::messageLoop()
{
	MSG msg = { 0 };
	while( ::GetMessage(&msg, NULL, 0, 0) ) {
		if( !drawManager::translateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

LRESULT windowWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
}

void windowWnd::onFinalMessage(HWND /*hWnd*/)
{
}


LRESULT CALLBACK windowWnd::__wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	windowWnd* pThis = NULL;
	if( uMsg == WM_NCCREATE ) {
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<windowWnd*>(lpcs->lpCreateParams);
		pThis->m_hWnd = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
	} 
	else {
		pThis = reinterpret_cast<windowWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if( uMsg == WM_NCDESTROY && pThis != NULL ) {
			LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
			::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
			if( pThis->m_bSubclassed ) pThis->unsubclass();
			pThis->m_hWnd = NULL;
			pThis->onFinalMessage(hWnd);
			return lRes;
		}
	}
	if( pThis != NULL ) {
		return pThis->handleMessage(uMsg, wParam, lParam);
	} 
	else {
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK windowWnd::__controlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	windowWnd* pThis = NULL;
	if( uMsg == WM_NCCREATE ) {
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<windowWnd*>(lpcs->lpCreateParams);
		::SetProp(hWnd, L"WndX", (HANDLE) pThis);
		pThis->m_hWnd = hWnd;
	} 
	else {
		pThis = reinterpret_cast<windowWnd*>(::GetProp(hWnd, L"WndX"));
		if( uMsg == WM_NCDESTROY && pThis != NULL ) {
			LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
			if( pThis->m_bSubclassed ) pThis->unsubclass();
			::SetProp(hWnd, L"WndX", NULL);
			pThis->m_hWnd = NULL;
			pThis->onFinalMessage(hWnd);
			return lRes;
		}
	}
	if( pThis != NULL ) {
		return pThis->handleMessage(uMsg, wParam, lParam);
	} 
	else {
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}
bool windowWnd::isOutMsg(UINT uMsg)
{
	static UINT outMsg[] = {WM_NCHITTEST,WM_MOUSEMOVE,WM_SETCURSOR,WM_PAINT,WM_NCPAINT,WM_ERASEBKGND,0};
	return std::find(std::begin(outMsg), std::end(outMsg), uMsg) != std::end(outMsg);
}
std::wstring windowWnd::stringMsg(UINT uMsg)
{
	wchar_t pstr[80] = {0};
	std::swprintf(pstr, _countof(pstr), L"%04X ", uMsg);
	switch(uMsg)
	{
	case WM_NULL	: std::wcsncat(pstr, L"WM_NULL", _countof(pstr)); break;
	case WM_CREATE	: std::wcsncat(pstr, L"WM_CREATE", _countof(pstr)); break;
	case WM_DESTROY	: std::wcsncat(pstr, L"WM_DESTROY", _countof(pstr)); break;
	case WM_MOVE	: std::wcsncat(pstr, L"WM_MOVE", _countof(pstr)); break;
	case WM_SIZE	: std::wcsncat(pstr, L"WM_SIZE", _countof(pstr)); break;
	case WM_ACTIVATE: std::wcsncat(pstr, L"WM_ACTIVATE", _countof(pstr)); break;
	case WM_SETFOCUS: std::wcsncat(pstr, L"WM_SETFOCUS", _countof(pstr)); break;
	case WM_KILLFOCUS	: std::wcsncat(pstr, L"WM_KILLFOCUS", _countof(pstr)); break;
	case WM_ENABLE		: std::wcsncat(pstr, L"WM_ENABLE", _countof(pstr)); break;
	case WM_SETREDRAW	: std::wcsncat(pstr, L"WM_SETREDRAW", _countof(pstr)); break;
	case WM_SETTEXT		: std::wcsncat(pstr, L"WM_SETTEXT", _countof(pstr)); break;
	case WM_GETTEXT		: std::wcsncat(pstr, L"WM_GETTEXT", _countof(pstr)); break;
	case WM_GETTEXTLENGTH	: std::wcsncat(pstr, L"WM_GETTEXTLENGTH", _countof(pstr)); break;
	case WM_PAINT	: std::wcsncat(pstr, L"WM_PAINT", _countof(pstr)); break;
	case WM_CLOSE	: std::wcsncat(pstr, L"WM_CLOSE", _countof(pstr)); break;
	case WM_QUERYENDSESSION	: std::wcsncat(pstr, L"WM_QUERYENDSESSION", _countof(pstr)); break;
	case WM_QUERYOPEN	: std::wcsncat(pstr, L"WM_QUERYOPEN", _countof(pstr)); break;
	case WM_ENDSESSION	: std::wcsncat(pstr, L"WM_ENDSESSION", _countof(pstr)); break;
	case WM_QUIT	: std::wcsncat(pstr, L"WM_QUIT", _countof(pstr)); break;
	case WM_ERASEBKGND	: std::wcsncat(pstr, L"WM_ERASEBKGND", _countof(pstr)); break;
	case WM_SYSCOLORCHANGE	: std::wcsncat(pstr, L"WM_SYSCOLORCHANGE", _countof(pstr)); break;
	case WM_SHOWWINDOW	: std::wcsncat(pstr, L"WM_SHOWWINDOW", _countof(pstr)); break;
	case WM_WININICHANGE: std::wcsncat(pstr, L"WM_WININICHANGE or WM_SETTINGCHANGE", _countof(pstr)); break;
	case WM_DEVMODECHANGE	: std::wcsncat(pstr, L"WM_DEVMODECHANGE", _countof(pstr)); break;
	case WM_ACTIVATEAPP	: std::wcsncat(pstr, L"WM_ACTIVATEAPP", _countof(pstr)); break;
	case WM_FONTCHANGE	: std::wcsncat(pstr, L"WM_FONTCHANGE", _countof(pstr)); break;
	case WM_TIMECHANGE	: std::wcsncat(pstr, L"WM_TIMECHANGE", _countof(pstr)); break;
	case WM_CANCELMODE	: std::wcsncat(pstr, L"WM_CANCELMODE", _countof(pstr)); break;
	case WM_SETCURSOR	: std::wcsncat(pstr, L"WM_SETCURSOR", _countof(pstr)); break;
	case WM_MOUSEACTIVATE	: std::wcsncat(pstr, L"WM_MOUSEACTIVATE", _countof(pstr)); break;
	case WM_CHILDACTIVATE	: std::wcsncat(pstr, L"WM_CHILDACTIVATE", _countof(pstr)); break;
	case WM_QUEUESYNC	: std::wcsncat(pstr, L"WM_QUEUESYNC", _countof(pstr)); break;
	case WM_GETMINMAXINFO	: std::wcsncat(pstr, L"WM_GETMINMAXINFO", _countof(pstr)); break;
	case WM_PAINTICON: std::wcsncat(pstr, L"WM_PAINTICON", _countof(pstr)); break;
	case WM_ICONERASEBKGND: std::wcsncat(pstr, L"WM_ICONERASEBKGND", _countof(pstr)); break;
	case WM_NEXTDLGCTL: std::wcsncat(pstr, L"WM_NEXTDLGCTL", _countof(pstr)); break;
	case WM_SPOOLERSTATUS: std::wcsncat(pstr, L"WM_SPOOLERSTATUS", _countof(pstr)); break;
	case WM_DRAWITEM: std::wcsncat(pstr, L"WM_DRAWITEM", _countof(pstr)); break;
	case WM_MEASUREITEM: std::wcsncat(pstr, L"WM_MEASUREITEM", _countof(pstr)); break;
	case WM_DELETEITEM: std::wcsncat(pstr, L"WM_DELETEITEM", _countof(pstr)); break;
	case WM_VKEYTOITEM: std::wcsncat(pstr, L"WM_VKEYTOITEM", _countof(pstr)); break;
	case WM_CHARTOITEM: std::wcsncat(pstr, L"WM_CHARTOITEM", _countof(pstr)); break;
	case WM_SETFONT: std::wcsncat(pstr, L"WM_SETFONT", _countof(pstr)); break;
	case WM_GETFONT: std::wcsncat(pstr, L"WM_GETFONT", _countof(pstr)); break;
	case WM_SETHOTKEY: std::wcsncat(pstr, L"WM_SETHOTKEY", _countof(pstr)); break;
	case WM_GETHOTKEY: std::wcsncat(pstr, L"WM_GETHOTKEY", _countof(pstr)); break;
	case WM_QUERYDRAGICON: std::wcsncat(pstr, L"WM_QUERYDRAGICON", _countof(pstr)); break;
	case WM_COMPAREITEM: std::wcsncat(pstr, L"WM_COMPAREITEM", _countof(pstr)); break;
	case WM_GETOBJECT: std::wcsncat(pstr, L"WM_GETOBJECT", _countof(pstr)); break;
	case WM_COMPACTING: std::wcsncat(pstr, L"WM_COMPACTING", _countof(pstr)); break;
	case WM_COMMNOTIFY: std::wcsncat(pstr, L"WM_COMMNOTIFY", _countof(pstr)); break;
	case WM_WINDOWPOSCHANGING: std::wcsncat(pstr, L"WM_WINDOWPOSCHANGING", _countof(pstr)); break;
	case WM_WINDOWPOSCHANGED: std::wcsncat(pstr, L"WM_WINDOWPOSCHANGED", _countof(pstr)); break;
	case WM_POWER: std::wcsncat(pstr, L"WM_POWER", _countof(pstr)); break;
	case WM_COPYDATA: std::wcsncat(pstr, L"WM_COPYDATA", _countof(pstr)); break;
	case WM_CANCELJOURNAL: std::wcsncat(pstr, L"WM_CANCELJOURNAL", _countof(pstr)); break;
	case WM_NOTIFY: std::wcsncat(pstr, L"WM_NOTIFY", _countof(pstr)); break;
	case WM_INPUTLANGCHANGEREQUEST: std::wcsncat(pstr, L"WM_INPUTLANGCHANGEREQUEST", _countof(pstr)); break;
	case WM_INPUTLANGCHANGE: std::wcsncat(pstr, L"WM_INPUTLANGCHANGE", _countof(pstr)); break;
	case WM_TCARD: std::wcsncat(pstr, L"WM_TCARD", _countof(pstr)); break;
	case WM_HELP: std::wcsncat(pstr, L"WM_HELP", _countof(pstr)); break;
	case WM_USERCHANGED: std::wcsncat(pstr, L"WM_USERCHANGED", _countof(pstr)); break;
	case WM_NOTIFYFORMAT: std::wcsncat(pstr, L"WM_NOTIFYFORMAT", _countof(pstr)); break;
	case WM_CONTEXTMENU: std::wcsncat(pstr, L"WM_CONTEXTMENU", _countof(pstr)); break;
	case WM_STYLECHANGING: std::wcsncat(pstr, L"WM_STYLECHANGING", _countof(pstr)); break;
	case WM_STYLECHANGED: std::wcsncat(pstr, L"WM_STYLECHANGED", _countof(pstr)); break;
	case WM_DISPLAYCHANGE: std::wcsncat(pstr, L"WM_DISPLAYCHANGE", _countof(pstr)); break;
	case WM_GETICON: std::wcsncat(pstr, L"WM_GETICON", _countof(pstr)); break;
	case WM_SETICON: std::wcsncat(pstr, L"WM_SETICON", _countof(pstr)); break;
	case WM_NCCREATE: std::wcsncat(pstr, L"WM_NCCREATE", _countof(pstr)); break;
	case WM_NCDESTROY: std::wcsncat(pstr, L"WM_NCDESTROY", _countof(pstr)); break;
	case WM_NCCALCSIZE: std::wcsncat(pstr, L"WM_NCCALCSIZE", _countof(pstr)); break;
	case WM_NCHITTEST: std::wcsncat(pstr, L"WM_NCHITTEST", _countof(pstr)); break;
	case WM_NCPAINT: std::wcsncat(pstr, L"WM_NCPAINT", _countof(pstr)); break;
	case WM_NCACTIVATE: std::wcsncat(pstr, L"WM_NCACTIVATE", _countof(pstr)); break;
	case WM_GETDLGCODE: std::wcsncat(pstr, L"WM_GETDLGCODE", _countof(pstr)); break;
	case WM_SYNCPAINT: std::wcsncat(pstr, L"WM_SYNCPAINT", _countof(pstr)); break;
	case WM_NCMOUSEMOVE: std::wcsncat(pstr, L"WM_NCMOUSEMOVE", _countof(pstr)); break;
	case WM_NCLBUTTONDOWN: std::wcsncat(pstr, L"WM_NCLBUTTONDOWN", _countof(pstr)); break;
	case WM_NCLBUTTONUP: std::wcsncat(pstr, L"WM_NCLBUTTONUP", _countof(pstr)); break;
	case WM_NCLBUTTONDBLCLK: std::wcsncat(pstr, L"WM_NCLBUTTONDBLCLK", _countof(pstr)); break;
	case WM_NCRBUTTONDOWN: std::wcsncat(pstr, L"WM_NCRBUTTONDOWN", _countof(pstr)); break;
	case WM_NCRBUTTONUP: std::wcsncat(pstr, L"WM_NCRBUTTONUP", _countof(pstr)); break;
	case WM_NCRBUTTONDBLCLK: std::wcsncat(pstr, L"WM_NCRBUTTONDBLCLK", _countof(pstr)); break;
	case WM_NCMBUTTONDOWN: std::wcsncat(pstr, L"WM_NCMBUTTONDOWN", _countof(pstr)); break;
	case WM_NCMBUTTONUP: std::wcsncat(pstr, L"WM_NCMBUTTONUP", _countof(pstr)); break;
	case WM_NCMBUTTONDBLCLK: std::wcsncat(pstr, L"WM_NCMBUTTONDBLCLK", _countof(pstr)); break;
	case WM_NCXBUTTONDOWN: std::wcsncat(pstr, L"WM_NCXBUTTONDOWN", _countof(pstr)); break;
	case WM_NCXBUTTONUP: std::wcsncat(pstr, L"WM_NCXBUTTONUP", _countof(pstr)); break;
	case WM_NCXBUTTONDBLCLK: std::wcsncat(pstr, L"WM_NCXBUTTONDBLCLK", _countof(pstr)); break;
	case WM_INPUT_DEVICE_CHANGE: std::wcsncat(pstr, L"WM_INPUT_DEVICE_CHANGE", _countof(pstr)); break;
	case WM_INPUT: std::wcsncat(pstr, L"WM_INPUT", _countof(pstr)); break;
	case WM_KEYFIRST: std::wcsncat(pstr, L"WM_KEYFIRST or WM_KEYDOWN", _countof(pstr)); break;
	case WM_KEYUP: std::wcsncat(pstr, L"WM_KEYUP", _countof(pstr)); break;
	case WM_CHAR: std::wcsncat(pstr, L"WM_CHAR", _countof(pstr)); break;
	case WM_DEADCHAR: std::wcsncat(pstr, L"WM_DEADCHAR", _countof(pstr)); break;
	case WM_SYSKEYDOWN: std::wcsncat(pstr, L"WM_SYSKEYDOWN", _countof(pstr)); break;
	case WM_SYSKEYUP: std::wcsncat(pstr, L"WM_SYSKEYUP", _countof(pstr)); break;
	case WM_SYSCHAR: std::wcsncat(pstr, L"WM_SYSCHAR", _countof(pstr)); break;
	case WM_SYSDEADCHAR: std::wcsncat(pstr, L"WM_SYSDEADCHAR", _countof(pstr)); break;
	case WM_UNICHAR: std::wcsncat(pstr, L"WM_UNICHAR or WM_KEYLAST", _countof(pstr)); break;
	case WM_IME_STARTCOMPOSITION: std::wcsncat(pstr, L"WM_IME_STARTCOMPOSITION", _countof(pstr)); break;
	case WM_IME_ENDCOMPOSITION: std::wcsncat(pstr, L"WM_IME_ENDCOMPOSITION", _countof(pstr)); break;
	case WM_IME_COMPOSITION: std::wcsncat(pstr, L"WM_IME_COMPOSITION or WM_IME_KEYLAST", _countof(pstr)); break;
	case WM_INITDIALOG: std::wcsncat(pstr, L"WM_INITDIALOG", _countof(pstr)); break;
	case WM_COMMAND: std::wcsncat(pstr, L"WM_COMMAND", _countof(pstr)); break;
	case WM_SYSCOMMAND: std::wcsncat(pstr, L"WM_SYSCOMMAND", _countof(pstr)); break;
	case WM_TIMER: std::wcsncat(pstr, L"WM_TIMER", _countof(pstr)); break;
	case WM_HSCROLL: std::wcsncat(pstr, L"WM_HSCROLL", _countof(pstr)); break;
	case WM_VSCROLL: std::wcsncat(pstr, L"WM_VSCROLL", _countof(pstr)); break;
	case WM_INITMENU: std::wcsncat(pstr, L"WM_INITMENU", _countof(pstr)); break;
	case WM_INITMENUPOPUP: std::wcsncat(pstr, L"WM_INITMENUPOPUP", _countof(pstr)); break;
	case WM_GESTURE: std::wcsncat(pstr, L"WM_GESTURE", _countof(pstr)); break;
	case WM_GESTURENOTIFY: std::wcsncat(pstr, L"WM_GESTURENOTIFY", _countof(pstr)); break;
	case WM_MENUSELECT: std::wcsncat(pstr, L"WM_MENUSELECT", _countof(pstr)); break;
	case WM_MENUCHAR: std::wcsncat(pstr, L"WM_MENUCHAR", _countof(pstr)); break;
	case WM_ENTERIDLE: std::wcsncat(pstr, L"WM_ENTERIDLE", _countof(pstr)); break;
	case WM_MENURBUTTONUP: std::wcsncat(pstr, L"WM_MENURBUTTONUP", _countof(pstr)); break;
	case WM_MENUDRAG: std::wcsncat(pstr, L"WM_MENUDRAG", _countof(pstr)); break;
	case WM_MENUGETOBJECT: std::wcsncat(pstr, L"WM_MENUGETOBJECT", _countof(pstr)); break;
	case WM_UNINITMENUPOPUP: std::wcsncat(pstr, L"WM_UNINITMENUPOPUP", _countof(pstr)); break;
	case WM_MENUCOMMAND: std::wcsncat(pstr, L"WM_MENUCOMMAND", _countof(pstr)); break;
	case WM_CHANGEUISTATE: std::wcsncat(pstr, L"WM_CHANGEUISTATE", _countof(pstr)); break;
	case WM_UPDATEUISTATE: std::wcsncat(pstr, L"WM_UPDATEUISTATE", _countof(pstr)); break;
	case WM_QUERYUISTATE: std::wcsncat(pstr, L"WM_QUERYUISTATE", _countof(pstr)); break;
	case WM_CTLCOLORMSGBOX: std::wcsncat(pstr, L"WM_CTLCOLORMSGBOX", _countof(pstr)); break;
	case WM_CTLCOLOREDIT: std::wcsncat(pstr, L"WM_CTLCOLOREDIT", _countof(pstr)); break;
	case WM_CTLCOLORLISTBOX: std::wcsncat(pstr, L"WM_CTLCOLORLISTBOX", _countof(pstr)); break;
	case WM_CTLCOLORBTN: std::wcsncat(pstr, L"WM_CTLCOLORBTN", _countof(pstr)); break;
	case WM_CTLCOLORDLG: std::wcsncat(pstr, L"WM_CTLCOLORDLG", _countof(pstr)); break;
	case WM_CTLCOLORSCROLLBAR: std::wcsncat(pstr, L"WM_CTLCOLORSCROLLBAR", _countof(pstr)); break;
	case WM_CTLCOLORSTATIC: std::wcsncat(pstr, L"WM_CTLCOLORSTATIC", _countof(pstr)); break;
	case MN_GETHMENU: std::wcsncat(pstr, L"MN_GETHMENU", _countof(pstr)); break;
	case WM_MOUSEFIRST: std::wcsncat(pstr, L"WM_MOUSEFIRST or WM_MOUSEMOVE", _countof(pstr)); break;
	case WM_LBUTTONDOWN: std::wcsncat(pstr, L"WM_LBUTTONDOWN", _countof(pstr)); break;
	case WM_LBUTTONUP: std::wcsncat(pstr, L"WM_LBUTTONUP", _countof(pstr)); break;
	case WM_LBUTTONDBLCLK: std::wcsncat(pstr, L"WM_LBUTTONDBLCLK", _countof(pstr)); break;
	case WM_RBUTTONDOWN: std::wcsncat(pstr, L"WM_RBUTTONDOWN", _countof(pstr)); break;
	case WM_RBUTTONUP: std::wcsncat(pstr, L"WM_RBUTTONUP", _countof(pstr)); break;
	case WM_RBUTTONDBLCLK: std::wcsncat(pstr, L"WM_RBUTTONDBLCLK", _countof(pstr)); break;
	case WM_MBUTTONDOWN: std::wcsncat(pstr, L"WM_MBUTTONDOWN", _countof(pstr)); break;
	case WM_MBUTTONUP: std::wcsncat(pstr, L"WM_MBUTTONUP", _countof(pstr)); break;
	case WM_MBUTTONDBLCLK: std::wcsncat(pstr, L"WM_MBUTTONDBLCLK", _countof(pstr)); break;
	case WM_MOUSEWHEEL: std::wcsncat(pstr, L"WM_MOUSEWHEEL", _countof(pstr)); break;
	case WM_XBUTTONDOWN: std::wcsncat(pstr, L"WM_XBUTTONDOWN", _countof(pstr)); break;
	case WM_XBUTTONUP: std::wcsncat(pstr, L"WM_XBUTTONUP", _countof(pstr)); break;
	case WM_XBUTTONDBLCLK: std::wcsncat(pstr, L"WM_XBUTTONDBLCLK", _countof(pstr)); break;
	case WM_MOUSEHWHEEL: std::wcsncat(pstr, L"WM_MOUSEHWHEEL or WM_MOUSELAST", _countof(pstr)); break;
	case WM_PARENTNOTIFY: std::wcsncat(pstr, L"WM_PARENTNOTIFY", _countof(pstr)); break;
	case WM_ENTERMENULOOP: std::wcsncat(pstr, L"WM_ENTERMENULOOP", _countof(pstr)); break;
	case WM_EXITMENULOOP: std::wcsncat(pstr, L"WM_EXITMENULOOP", _countof(pstr)); break;
	case WM_NEXTMENU: std::wcsncat(pstr, L"WM_NEXTMENU", _countof(pstr)); break;
	case WM_SIZING: std::wcsncat(pstr, L"WM_SIZING", _countof(pstr)); break;
	case WM_CAPTURECHANGED: std::wcsncat(pstr, L"WM_CAPTURECHANGED", _countof(pstr)); break;
	case WM_MOVING: std::wcsncat(pstr, L"WM_MOVING", _countof(pstr)); break;
	case WM_POWERBROADCAST: std::wcsncat(pstr, L"WM_POWERBROADCAST", _countof(pstr)); break;
	case WM_DEVICECHANGE: std::wcsncat(pstr, L"WM_DEVICECHANGE", _countof(pstr)); break;
	case WM_MDICREATE: std::wcsncat(pstr, L"WM_MDICREATE", _countof(pstr)); break;
	case WM_MDIDESTROY: std::wcsncat(pstr, L"WM_MDIDESTROY", _countof(pstr)); break;
	case WM_MDIACTIVATE: std::wcsncat(pstr, L"WM_MDIACTIVATE", _countof(pstr)); break;
	case WM_MDIRESTORE: std::wcsncat(pstr, L"WM_MDIRESTORE", _countof(pstr)); break;
	case WM_MDINEXT: std::wcsncat(pstr, L"WM_MDINEXT", _countof(pstr)); break;
	case WM_MDIMAXIMIZE: std::wcsncat(pstr, L"WM_MDIMAXIMIZE", _countof(pstr)); break;
	case WM_MDITILE: std::wcsncat(pstr, L"WM_MDITILE", _countof(pstr)); break;
	case WM_MDICASCADE: std::wcsncat(pstr, L"WM_MDICASCADE", _countof(pstr)); break;
	case WM_MDIICONARRANGE: std::wcsncat(pstr, L"WM_MDIICONARRANGE", _countof(pstr)); break;
	case WM_MDIGETACTIVE: std::wcsncat(pstr, L"WM_MDIGETACTIVE", _countof(pstr)); break;
	case WM_MDISETMENU: std::wcsncat(pstr, L"WM_MDISETMENU", _countof(pstr)); break;
	case WM_ENTERSIZEMOVE: std::wcsncat(pstr, L"WM_ENTERSIZEMOVE", _countof(pstr)); break;
	case WM_EXITSIZEMOVE: std::wcsncat(pstr, L"WM_EXITSIZEMOVE", _countof(pstr)); break;
	case WM_DROPFILES: std::wcsncat(pstr, L"WM_DROPFILES", _countof(pstr)); break;
	case WM_MDIREFRESHMENU: std::wcsncat(pstr, L"WM_MDIREFRESHMENU", _countof(pstr)); break;
	case WM_TOUCH: std::wcsncat(pstr, L"WM_TOUCH", _countof(pstr)); break;
	case WM_IME_SETCONTEXT: std::wcsncat(pstr, L"WM_IME_SETCONTEXT", _countof(pstr)); break;
	case WM_IME_NOTIFY: std::wcsncat(pstr, L"WM_IME_NOTIFY", _countof(pstr)); break;
	case WM_IME_CONTROL: std::wcsncat(pstr, L"WM_IME_CONTROL", _countof(pstr)); break;
	case WM_IME_COMPOSITIONFULL: std::wcsncat(pstr, L"WM_IME_COMPOSITIONFULL", _countof(pstr)); break;
	case WM_IME_SELECT: std::wcsncat(pstr, L"WM_IME_SELECT", _countof(pstr)); break;
	case WM_IME_CHAR: std::wcsncat(pstr, L"WM_IME_CHAR", _countof(pstr)); break;
	case WM_IME_REQUEST: std::wcsncat(pstr, L"WM_IME_REQUEST", _countof(pstr)); break;
	case WM_IME_KEYDOWN: std::wcsncat(pstr, L"WM_IME_KEYDOWN", _countof(pstr)); break;
	case WM_IME_KEYUP: std::wcsncat(pstr, L"WM_IME_KEYUP", _countof(pstr)); break;
	case WM_MOUSEHOVER: std::wcsncat(pstr, L"WM_MOUSEHOVER", _countof(pstr)); break;
	case WM_MOUSELEAVE: std::wcsncat(pstr, L"WM_MOUSELEAVE", _countof(pstr)); break;
	case WM_NCMOUSEHOVER: std::wcsncat(pstr, L"WM_NCMOUSEHOVER", _countof(pstr)); break;
	case WM_NCMOUSELEAVE: std::wcsncat(pstr, L"WM_NCMOUSELEAVE", _countof(pstr)); break;
	case WM_WTSSESSION_CHANGE: std::wcsncat(pstr, L"WM_WTSSESSION_CHANGE", _countof(pstr)); break;
	case WM_TABLET_FIRST: std::wcsncat(pstr, L"WM_TABLET_FIRST", _countof(pstr)); break;
	case WM_TABLET_LAST: std::wcsncat(pstr, L"WM_TABLET_LAST", _countof(pstr)); break;
	case WM_CUT: std::wcsncat(pstr, L"WM_CUT", _countof(pstr)); break;
	case WM_COPY: std::wcsncat(pstr, L"WM_COPY", _countof(pstr)); break;
	case WM_PASTE: std::wcsncat(pstr, L"WM_PASTE", _countof(pstr)); break;
	case WM_CLEAR: std::wcsncat(pstr, L"WM_CLEAR", _countof(pstr)); break;
	case WM_UNDO: std::wcsncat(pstr, L"WM_UNDO", _countof(pstr)); break;
	case WM_RENDERFORMAT: std::wcsncat(pstr, L"WM_RENDERFORMAT", _countof(pstr)); break;
	case WM_RENDERALLFORMATS: std::wcsncat(pstr, L"WM_RENDERALLFORMATS", _countof(pstr)); break;
	case WM_DESTROYCLIPBOARD: std::wcsncat(pstr, L"WM_DESTROYCLIPBOARD", _countof(pstr)); break;
	case WM_DRAWCLIPBOARD: std::wcsncat(pstr, L"WM_DRAWCLIPBOARD", _countof(pstr)); break;
	case WM_PAINTCLIPBOARD: std::wcsncat(pstr, L"WM_PAINTCLIPBOARD", _countof(pstr)); break;
	case WM_VSCROLLCLIPBOARD: std::wcsncat(pstr, L"WM_VSCROLLCLIPBOARD", _countof(pstr)); break;
	case WM_SIZECLIPBOARD: std::wcsncat(pstr, L"WM_SIZECLIPBOARD", _countof(pstr)); break;
	case WM_ASKCBFORMATNAME: std::wcsncat(pstr, L"WM_ASKCBFORMATNAME", _countof(pstr)); break;
	case WM_CHANGECBCHAIN: std::wcsncat(pstr, L"WM_CHANGECBCHAIN", _countof(pstr)); break;
	case WM_HSCROLLCLIPBOARD: std::wcsncat(pstr, L"WM_HSCROLLCLIPBOARD", _countof(pstr)); break;
	case WM_QUERYNEWPALETTE: std::wcsncat(pstr, L"WM_QUERYNEWPALETTE", _countof(pstr)); break;
	case WM_PALETTEISCHANGING: std::wcsncat(pstr, L"WM_PALETTEISCHANGING", _countof(pstr)); break;
	case WM_PALETTECHANGED: std::wcsncat(pstr, L"WM_PALETTECHANGED", _countof(pstr)); break;
	case WM_HOTKEY: std::wcsncat(pstr, L"WM_HOTKEY", _countof(pstr)); break;
	case WM_PRINT: std::wcsncat(pstr, L"WM_PRINT", _countof(pstr)); break;
	case WM_PRINTCLIENT: std::wcsncat(pstr, L"WM_PRINTCLIENT", _countof(pstr)); break;
	case WM_APPCOMMAND: std::wcsncat(pstr, L"WM_APPCOMMAND", _countof(pstr)); break;
	case WM_THEMECHANGED: std::wcsncat(pstr, L"WM_THEMECHANGED", _countof(pstr)); break;
	case WM_CLIPBOARDUPDATE: std::wcsncat(pstr, L"WM_CLIPBOARDUPDATE", _countof(pstr)); break;
	case WM_DWMCOMPOSITIONCHANGED: std::wcsncat(pstr, L"WM_DWMCOMPOSITIONCHANGED", _countof(pstr)); break;
	case WM_DWMNCRENDERINGCHANGED: std::wcsncat(pstr, L"WM_DWMNCRENDERINGCHANGED", _countof(pstr)); break;
	case WM_DWMCOLORIZATIONCOLORCHANGED: std::wcsncat(pstr, L"WM_DWMCOLORIZATIONCOLORCHANGED", _countof(pstr)); break;
	case WM_DWMWINDOWMAXIMIZEDCHANGE: std::wcsncat(pstr, L"WM_DWMWINDOWMAXIMIZEDCHANGE", _countof(pstr)); break;
	case WM_DWMSENDICONICTHUMBNAIL: std::wcsncat(pstr, L"WM_DWMSENDICONICTHUMBNAIL", _countof(pstr)); break;
	case WM_DWMSENDICONICLIVEPREVIEWBITMAP: std::wcsncat(pstr, L"WM_DWMSENDICONICLIVEPREVIEWBITMAP", _countof(pstr)); break;
	case WM_GETTITLEBARINFOEX: std::wcsncat(pstr, L"WM_GETTITLEBARINFOEX", _countof(pstr)); break;
	case WM_HANDHELDFIRST: std::wcsncat(pstr, L"WM_HANDHELDFIRST", _countof(pstr)); break;
	case WM_HANDHELDLAST: std::wcsncat(pstr, L"WM_HANDHELDLAST", _countof(pstr)); break;
	case WM_AFXFIRST: std::wcsncat(pstr, L"WM_AFXFIRST", _countof(pstr)); break;
	case WM_AFXLAST: std::wcsncat(pstr, L"WM_AFXLAST", _countof(pstr)); break;
	case WM_PENWINFIRST: std::wcsncat(pstr, L"WM_PENWINFIRST", _countof(pstr)); break;
	case WM_PENWINLAST: std::wcsncat(pstr, L"WM_PENWINLAST", _countof(pstr)); break;
	case WM_APP: std::wcsncat(pstr, L"WM_APP", _countof(pstr)); break;
	case WM_USER: std::wcsncat(pstr, L"WM_USER", _countof(pstr)); break;
	default:
		{
			if(uMsg & WM_APP)
			{
				std::swprintf(pstr, _countof(pstr), L"WM_APP+%d", uMsg & ~WM_APP);
			}
			else if (uMsg & WM_USER)
			{
				std::swprintf(pstr, _countof(pstr), L"WM_USER+d", uMsg & ~WM_USER);
			}
		}
		break;
	}
	return pstr;
}

}