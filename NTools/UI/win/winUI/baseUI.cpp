#include "StdAfx.h"
#include "baseUI.h"

namespace wui {

/////////////////////////////////////////////////////////////////////////////////////
//
//

void WINUI_API __trace(PCWSTR pstrFormat, ...)
{
#ifdef _DEBUG
	wchar_t szBuffer[300] = { 0 };
	va_list args;
	va_start(args, pstrFormat);
	std::vswprintf(szBuffer, _countof(szBuffer) - 2, pstrFormat, args);
	std::wcscat(szBuffer, L"\n");
	va_end(args);
	::OutputDebugString(szBuffer);
#endif
}
void WINUI_API __traceA(PCSTR pstrFormat, ...)
{
#ifdef _DEBUG
	char szBuffer[300] = { 0 };
	va_list args;
	va_start(args, pstrFormat);
	std::vsprintf(szBuffer, pstrFormat, args);
	std::strcat(szBuffer, "\n");

	va_end(args);
	::OutputDebugStringA(szBuffer);
#endif
}
PCWSTR __traceMsg(UINT uMsg)
{
#define MSGDEF(x) if(uMsg==x) return _T(#x)
	MSGDEF(WM_SETCURSOR);
	MSGDEF(WM_NCHITTEST);
	MSGDEF(WM_NCPAINT);
	MSGDEF(WM_PAINT);
	MSGDEF(WM_ERASEBKGND);
	MSGDEF(WM_NCMOUSEMOVE);  
	MSGDEF(WM_MOUSEMOVE);
	MSGDEF(WM_MOUSELEAVE);
	MSGDEF(WM_MOUSEHOVER);   
	MSGDEF(WM_NOTIFY);
	MSGDEF(WM_COMMAND);
	MSGDEF(WM_MEASUREITEM);
	MSGDEF(WM_DRAWITEM);   
	MSGDEF(WM_LBUTTONDOWN);
	MSGDEF(WM_LBUTTONUP);
	MSGDEF(WM_LBUTTONDBLCLK);
	MSGDEF(WM_RBUTTONDOWN);
	MSGDEF(WM_RBUTTONUP);
	MSGDEF(WM_RBUTTONDBLCLK);
	MSGDEF(WM_SETFOCUS);
	MSGDEF(WM_KILLFOCUS);  
	MSGDEF(WM_MOVE);
	MSGDEF(WM_SIZE);
	MSGDEF(WM_SIZING);
	MSGDEF(WM_MOVING);
	MSGDEF(WM_GETMINMAXINFO);
	MSGDEF(WM_CAPTURECHANGED);
	MSGDEF(WM_WINDOWPOSCHANGED);
	MSGDEF(WM_WINDOWPOSCHANGING);   
	MSGDEF(WM_NCCALCSIZE);
	MSGDEF(WM_NCCREATE);
	MSGDEF(WM_NCDESTROY);
	MSGDEF(WM_TIMER);
	MSGDEF(WM_KEYDOWN);
	MSGDEF(WM_KEYUP);
	MSGDEF(WM_CHAR);
	MSGDEF(WM_SYSKEYDOWN);
	MSGDEF(WM_SYSKEYUP);
	MSGDEF(WM_SYSCOMMAND);
	MSGDEF(WM_SYSCHAR);
	MSGDEF(WM_VSCROLL);
	MSGDEF(WM_HSCROLL);
	MSGDEF(WM_CHAR);
	MSGDEF(WM_SHOWWINDOW);
	MSGDEF(WM_PARENTNOTIFY);
	MSGDEF(WM_CREATE);
	MSGDEF(WM_NCACTIVATE);
	MSGDEF(WM_ACTIVATE);
	MSGDEF(WM_ACTIVATEAPP);   
	MSGDEF(WM_CLOSE);
	MSGDEF(WM_DESTROY);
	MSGDEF(WM_GETICON);   
	MSGDEF(WM_GETTEXT);
	MSGDEF(WM_GETTEXTLENGTH);   
	static wchar_t szMsg[10];
	::wsprintf(szMsg, L"0x%04X", uMsg);
	return szMsg;
}

/////////////////////////////////////////////////////////////////////////////////////
// 字符串操作
/*****************************************************************************************
	<< --- static convert::toWChar	2014-04-25 --- >>
	说明：将单字符转换成宽字符串(UTF-8未测试)
	参数：
	pdata		= 被转换的字符
	isAnsi	= true:被转换的字符为ANSI码；false:否则为UTF-8
	返回值：转换后的字符串
*****************************************************************************************/
std::wstring  WINUI_API toWChar(const char *pdata, bool isAnsi /*= true*/)
{
	if(pdata == nullptr || std::strlen(pdata) <= 0)
		return L"";
	std::wstring wstr;
	wchar_t *buf = nullptr;

	int len = ::MultiByteToWideChar(isAnsi? CP_ACP: CP_UTF8, 0, pdata, -1, NULL, 0);
	if(len > 0)
	{
		buf = new wchar_t[len+1];
		memset(buf, 0, (len+1)*sizeof(wchar_t));
		len = ::MultiByteToWideChar(isAnsi? CP_ACP: CP_UTF8, 0, pdata, -1, buf, len);
	}

	if(buf)
	{
		wstr = buf;
		delete buf;
	}
	return wstr;
}
/*****************************************************************************************
	<< --- static convert::toWChar	2014-04-25 --- >>
	说明：将单字符转换成单字节字符串(UTF-8未测试)
	参数：
	pdata	= 被转换的字符
	toAnsi	= true:字符串转化为ANSI码；false:否则为UTF-8
	返回值：转换后的字符串
*****************************************************************************************/
std::string WINUI_API toChar(const wchar_t *pdata, bool toAnsi /*= true*/)
{
	assert(pdata != nullptr && std::wcslen(pdata) > 0);
	std::string cstr;
	char *buf = nullptr;
	int len = ::WideCharToMultiByte(toAnsi? CP_ACP: CP_UTF8, 0, pdata, -1, NULL,0, NULL, NULL);
	if(len > 0)
	{
		buf = new char[len+1];
		memset(buf, 0, (len+1)*sizeof(char));
		len = ::WideCharToMultiByte(toAnsi? CP_ACP: CP_UTF8, 0, pdata, -1, buf, len, NULL, NULL);
	}

	if(buf)
	{
		cstr = buf;
		delete buf;
	}
	return cstr;
}
// 
void WINUI_API makeUpper(std::wstring &ws) {
	std::string cs = toChar(ws.data());
	for (std::string::iterator iter = cs.begin(); iter != cs.end(); ++iter)
	{
		if(!(*iter & 0x80) && std::isalnum(*iter) && !std::isupper(*iter))
		{
			*iter = std::toupper(*iter);
		}
	}

	ws = toWChar(cs.c_str());
}
// 
void WINUI_API makeLower(std::wstring &ws)
{
	std::string cs = toChar(ws.data());
	for (std::string::iterator iter = cs.begin(); iter != cs.end(); ++iter)
	{
		if(!(*iter & 0x80) && std::isalnum(*iter) && std::isupper(*iter))
		{
			*iter = std::tolower(*iter);
		}
	}

	ws = toWChar(cs.c_str());
}

std::wstring WINUI_API smallFormat(PCWSTR pstrFormat, ...)
{
	//CStdString sFormat = pstrFormat;
	wchar_t szBuffer[64] = { 0 };
	va_list argList;
	va_start(argList, pstrFormat);
	//int iRet = ::wvsprintf(szBuffer, sFormat, argList);
	int iRet = std::vswprintf(szBuffer, _countof(szBuffer), pstrFormat, argList);
	va_end(argList);
	
	return szBuffer;
}
/////////////////////////////////////////////////////////////////////////////////////
//
//
rect& rect::operator=(const RECT &rt)
{
	left = rt.left;
	top = rt.top;
	right = rt.right;
	bottom = rt.bottom;
	return *this;
}

int rect::width() const
{
	return right - left;
}

int rect::height() const
{
	return bottom - top;
}

void rect::clear()
{
	left = top = right = bottom = 0;
}

bool rect::empty() const
{
	return (left == 0 && right == 0 && top == 0 && bottom == 0); 
}

void rect::join(const RECT& rc)
{
	if( rc.left < left ) left = rc.left;
	if( rc.top < top ) top = rc.top;
	if( rc.right > right ) right = rc.right;
	if( rc.bottom > bottom ) bottom = rc.bottom;
}

void rect::resetOffset()
{
	::OffsetRect(this, -left, -top);
}

void rect::normalize()
{
	if( left > right ) { int iTemp = left; left = right; right = iTemp; }
	if( top > bottom ) { int iTemp = top; top = bottom; bottom = iTemp; }
}

void rect::offset(int cx, int cy)
{
	::OffsetRect(this, cx, cy);
}

void rect::inflate(int cx, int cy)
{
	::InflateRect(this, cx, cy);
}

void rect::deflate(int cx, int cy)
{
	::InflateRect(this, -cx, -cy);
}

void rect::Union(rect& rc)
{
	::UnionRect(this, this, &rc);
}




/////////////////////////////////////////////////////////////////////////////////////
//
//

windowWnd::windowWnd() : m_hWnd(NULL), m_OldWndProc(::DefWindowProc), m_bSubclassed(false)
{
	////TRACE_CLASS_BEGIN(this)
}

HWND windowWnd::getHWND() const 
{ 
	return m_hWnd; 
}

UINT windowWnd::getClassStyle() const
{
	return 0;
}

PCWSTR windowWnd::getSuperClassName() const
{
	return NULL;
}

windowWnd::operator HWND() const
{
	return m_hWnd;
}

HWND windowWnd::create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
{
	return create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
}

HWND windowWnd::create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
{
	if( getSuperClassName() != NULL && !registerSuperclass() ) return NULL;
	if( getSuperClassName() == NULL && !registerWindowClass() ) return NULL;
	m_hWnd = ::CreateWindowEx(dwExStyle, getWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, paintManagerUI::getInstance(), this);
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

void windowWnd::showWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
	assert(::IsWindow(m_hWnd));
	if( !::IsWindow(m_hWnd) ) return;
	::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
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
		if( !paintManagerUI::translateMessage(&msg) ) {
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

void windowWnd::setIcon(UINT nRes)
{
	HICON hIcon = (HICON)::LoadImage(paintManagerUI::getInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	assert(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);
	hIcon = (HICON)::LoadImage(paintManagerUI::getInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	assert(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) FALSE, (LPARAM) hIcon);
}

bool windowWnd::registerWindowClass()
{
	WNDCLASS wc = { 0 };
	wc.style = getClassStyle();
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.lpfnWndProc = windowWnd::__wndProc;
	wc.hInstance = paintManagerUI::getInstance();
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = getWindowClassName();
	ATOM ret = ::RegisterClass(&wc);
	assert(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool windowWnd::registerSuperclass()
{
	// Get the class information from an existing
	// window so we can subclass it later on...
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	if( !::GetClassInfoEx(NULL, getSuperClassName(), &wc) ) {
		if( !::GetClassInfoEx(paintManagerUI::getInstance(), getSuperClassName(), &wc) ) {
			assert(!"Unable to locate window class");
			return NULL;
		}
	}
	m_OldWndProc = wc.lpfnWndProc;
	wc.lpfnWndProc = windowWnd::__controlProc;
	wc.hInstance = paintManagerUI::getInstance();
	wc.lpszClassName = getWindowClassName();
	ATOM ret = ::RegisterClassEx(&wc);
	assert(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
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

void windowWnd::resizeClient(int cx /*= -1*/, int cy /*= -1*/)
{
	assert(::IsWindow(m_hWnd));
	RECT rc = { 0 };
	if( !::GetClientRect(m_hWnd, &rc) ) return;
	if( cx != -1 ) rc.right = cx;
	if( cy != -1 ) rc.bottom = cy;
	if( !::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd)) ) return;
	::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

LRESULT windowWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
}

void windowWnd::onFinalMessage(HWND /*hWnd*/)
{
}
}