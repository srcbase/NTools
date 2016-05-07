#include "stdafx.h"
#include "UCwindow.h"
//#include "controlUC.h"
namespace wuc{


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	windowWnd类	2014-06-21
//	消息窗口的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////

windowWnd::windowWnd(void)
	: m_hWnd(NULL)
	, m_OldWndProc(::DefWindowProc), m_bSubclassed(false)
{

}

windowWnd::~windowWnd(void)
{
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
	wc.hIcon = LoadIcon( paintManagerUC::getInstance(), IDI_APPLICATION );
	wc.lpfnWndProc = windowWnd::__wndProc;
	wc.hInstance = paintManagerUC::getInstance();
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = getWindowClassName();
	//bool bl = UnregisterClass(getWindowClassName(), paintManagerUC::getInstance());
	//DWORD dw = ::GetLastError();
	ATOM ret = ::RegisterClassEx(&wc);
	assert(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool windowWnd::registerSuperclass()
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	if( !::GetClassInfoEx(NULL, getSuperClassName(), &wc) ) {
		if( !::GetClassInfoEx(paintManagerUC::getInstance(), getSuperClassName(), &wc) ) {
			assert(!"Unable to locate window class");
			return NULL;
		}
	}
	m_OldWndProc = wc.lpfnWndProc;
	wc.lpfnWndProc = windowWnd::__controlProc;
	wc.hInstance = paintManagerUC::getInstance();
	wc.lpszClassName = getWindowClassName();
	ATOM ret = ::RegisterClassEx(&wc);
	assert(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

HWND windowWnd::create(HWND hwndParent, PCWSTR pstrName, const RECT rc)
{
	return create(hwndParent, pstrName, getWindowStyle(), getWindowExStyle(), rc, nullptr);
}

HWND windowWnd::create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
{
	return create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
}

HWND windowWnd::create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
{
	if( getSuperClassName() != NULL && !registerSuperclass() ) return NULL;
	if( getSuperClassName() == NULL && !registerWindowClass() ) return NULL;
	m_hWnd = ::CreateWindowEx(dwExStyle, getWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, paintManagerUC::getInstance(), this);
	assert(m_hWnd!=NULL);
	return m_hWnd;
}

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
	HICON hIcon = (HICON)::LoadImage(paintManagerUC::getInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	assert(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);
	hIcon = (HICON)::LoadImage(paintManagerUC::getInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
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
	::UpdateWindow(m_hWnd);
}
bool windowWnd::showModal()
{
	assert(::IsWindow(m_hWnd));
	HWND hWndParent = GetWindowOwner(m_hWnd);
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::EnableWindow(hWndParent, FALSE);
	MSG msg = { 0 };
	while( ::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) {
		if( msg.message == WM_CLOSE && msg.hwnd == m_hWnd ) {
			::EnableWindow(hWndParent, TRUE);
			::SetFocus(hWndParent);
		}
		if( !paintManagerUC::translateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if( msg.message == WM_QUIT ) break;
	}
	::EnableWindow(hWndParent, TRUE);
	::SetFocus(hWndParent);
	if( msg.message == WM_QUIT ) ::PostQuitMessage(msg.wParam);
	return true;
}
void windowWnd::close()
{
	assert(::IsWindow(m_hWnd));
	if( !::IsWindow(m_hWnd) ) return;
	postMessage(WM_CLOSE);
}
void windowWnd::centerWindow()
{
	assert(::IsWindow(m_hWnd));
	assert((GetWindowStyle(m_hWnd)&WS_CHILD)==0);
	RECT rcDlg = { 0 };
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea = { 0 };
	RECT rcCenter = { 0 };
	HWND hWndParent = ::GetParent(m_hWnd);
	HWND hWndCenter = ::GetWindowOwner(m_hWnd);
	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
	if( hWndCenter == NULL ) rcCenter = rcArea; else ::GetWindowRect(hWndCenter, &rcCenter);

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
	//RECT rtpos = getWindowPos();
	gtc::rect rc = getWindowPos();
	//if(rtpos == rt) return;
	::SetWindowPos(m_hWnd, nullptr, rt.left, rt.top, rt.right-rt.left, rt.bottom-rt.top,  SWP_NOZORDER | SWP_NOACTIVATE);
}

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

}