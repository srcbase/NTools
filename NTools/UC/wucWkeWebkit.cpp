#include "wucWkeWebkit.h"
#include "windowWnd.h"
#include "wucBase.h"
#pragma comment(lib,"D:/NTools/wke/wke.lib")
#pragma comment(lib,"Imm32.lib")

namespace wuc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	  类		2015-9-16
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////

const char	TM_TICKER			= 1;		// 时钟ID
const int	SCROLL_TICKS		= 10;		// 每10次tick检查一下内容尺寸
const int	SCROLL_CHECK_MAX	= 10;		// 超过10次检查后就认为内容尺寸不会变化了

class wkeWebkitWnd : public windowWnd ,protected wkeBufHandler 
{
	friend class wkeWebkitUC;
public:
	wkeWebkitWnd();
	~wkeWebkitWnd();

	virtual PCWSTR getWindowClassName() const 
	{
		return L"WkeWebkit";
	}
	virtual void onFinalMessage(HWND hWnd)
	{
		//m_pOwner->invalidate();
		
		if(m_pOwner)
		{
			//delete m_pOwner->m_pWindow;
			m_pOwner->m_pWindow = NULL;
		}
		delete this;
	}
	void init(wkeWebkitUC* pOwner);
	wkeWebView getWebView();

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled); 
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled); 
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled);
	LRESULT OnChar(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled);
	LRESULT OnImeStartComposition(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);	
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
	virtual void onBufUpdated (const HDC hdc,int x, int y, int cx, int cy);

	static void wke_onTitleChanged(const struct _wkeClientHandler* clientHandler, const wkeString title);
	static void wke_onUrlChanged(const struct _wkeClientHandler* clientHandler, const wkeString url);

protected:
	//drawManager		m_dm;
	wkeWebkitUC		*m_pOwner;
	wkeWebView		m_pWebView;
	std::wstring	m_wsUrl;
	bool			m_bInit;

	bool			m_bDocumentReady;
	bool			m_bLoaded;
	bool			m_bLoadComplete;

	bool				m_bTitleChanged;
	bool				m_bUrlChanged;
	std::wstring		m_wsCurrentTitle;
	std::wstring		m_wsCurrentUrl;
	wkeClientHandler	m_clientHandler;

	bool				m_bMouseEnter;
	// 检查加载完后文档区尺寸是否有变化，如图片后加载就会出现尺寸有变化
	bool			m_bCheckScroll;		// 是否检查
	SIZE			m_szScroll;			// 内容尺寸，用来设定滚动条
};
wkeWebkitWnd::wkeWebkitWnd()
	: m_pOwner(nullptr), m_pWebView(NULL), m_bInit(false)
	, m_bTitleChanged(false), m_bUrlChanged(false)
	, m_bDocumentReady(false), m_bLoaded(false), m_bLoadComplete(false)
	, m_bMouseEnter(false)
	, m_bCheckScroll(false)
{
	SEND_LOG(L"wkeWebkitWnd init ...");
	memset(&m_szScroll, 0, sizeof(m_szScroll));
	m_clientHandler.onTitleChanged = wke_onTitleChanged;
	m_clientHandler.onURLChanged = wke_onUrlChanged;
}
wkeWebkitWnd::~wkeWebkitWnd()
{
	SEND_LOG(L"wkeWebkitWnd free ...");
	if(m_pWebView) wkeDestroyWebView(m_pWebView);
}
void wkeWebkitWnd::init(wkeWebkitUC* pOwner)
{
	assert(pOwner);
	assert(pOwner->getManager());
	m_pOwner = pOwner;
	m_bInit = true;
	// 
	if (m_hWnd == NULL)
	{
		RECT rcPos = m_pOwner->getPosition();
		if (m_pOwner->getManager()->isLayerWindowTransparent())
		{
			bool bTopMost = (GetWindowLong(m_pOwner->getManager()->getPaintWindow(), GWL_EXSTYLE) & WS_EX_TOPMOST) == WS_EX_TOPMOST;
			LONG styleEx = WS_EX_TOOLWINDOW;
			if(bTopMost) styleEx |= WS_EX_TOPMOST;
			m_pOwner->getManager()->clientToScreen(rcPos);
			DWORD uStyle = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
			create(NULL, NULL, uStyle, styleEx, rcPos);
			//SetForegroundWindow(m_hWnd);
		}
		else
		{
			DWORD uStyle = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
			DWORD styleEx = 0;
			//styleEx |= WS_EX_TOOLWINDOW;	// 不能用这个属性，否则只能显示一个web，但能创建多个，要注意哦！！！
			//	HWND   hWnd = CreateWindow(_T("#32770"), _T("WndMediaDisplay"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, m_PaintManager.GetPaintWindow(), (HMENU)0, NULL, NULL);
			create(m_pOwner->getManager()->getPaintWindow(), NULL, uStyle, styleEx, rcPos);

		}
		//setParent(m_pOwner->getManager()->getPaintWindow());
	}

}
wkeWebView wkeWebkitWnd::getWebView()
{
	return m_pWebView;
}
void wkeWebkitWnd::onBufUpdated (const HDC hdc,int x, int y, int cx, int cy)
{
	RECT rcClient;
	GetClientRect(m_hWnd,&rcClient);
	RECT rcInvalid = {rcClient.left + x, rcClient.top + y, rcClient.left + x + cx, rcClient.top + y + cy};
	InvalidateRect(m_hWnd, &rcInvalid ,TRUE);

}

LRESULT wkeWebkitWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	bool bHandled = true;
	switch (uMsg)
	{
	case WM_PAINT:			lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEWHEEL:
		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); 
		break;
	case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_CHAR:			lRes = OnChar(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYUP:			lRes = OnKeyUp(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_TIMER:			lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
	case WM_IME_STARTCOMPOSITION: lRes = OnImeStartComposition(uMsg, wParam, lParam, bHandled); break;	
	case WM_CONTEXTMENU:	lRes= OnContextMenu(uMsg, wParam, lParam, bHandled); break;	
	case WM_LBUTTONDOWN:	
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEMOVE:
	case WM_MOUSELEAVE:
		{
			lRes = OnMouseEvent(uMsg, wParam, lParam, bHandled);
			bHandled = true;	// 响应鼠标点击跳转到新链接
		}
		break;
	default:
		bHandled = false; 
		break;
	}
	if(bHandled) return lRes;
	if(m_pOwner && m_pOwner->getManager()->isLayerWindowTransparent())
		m_pOwner->getManager()->messageHandler( uMsg, wParam, lParam, lRes);
	return __super::handleMessage(uMsg, wParam, lParam);
}

LRESULT wkeWebkitWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if(m_pOwner && m_pOwner->getManager() && m_pOwner->getManager()->isLayerWindowTransparent())
		m_pOwner->getManager()->removeLayerWindowControl(m_pOwner);
	KillTimer(m_hWnd, TM_TICKER);
	if(m_pWebView)
	{
		wkeDestroyWebView(m_pWebView);
		m_pWebView = NULL;
	}
	bHandled = true;
	return 0;
}

LRESULT wkeWebkitWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{

	PAINTSTRUCT ps = { 0 };
	HDC hDcPaint = ::BeginPaint(m_hWnd, &ps);
	//SEND_LOG(L"wkeWebkitWnd::OnPaint rcPaint:<%s> erase:%d", gtc::format::rect(ps.rcPaint, true).c_str(), ps.fErase);
	

	RECT rcClip;	
	::GetClipBox(hDcPaint,&rcClip);	

	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);

	RECT rcInvalid;
	::IntersectRect(&rcInvalid, &rcClip,&rcClient);

	//BitBlt(hDcPaint,rcInvalid.left,rcInvalid.top,
	//	rcInvalid.right - rcInvalid.left, rcInvalid.bottom - rcInvalid.top,
	//	m_pWebView->getViewDC(),rcInvalid.left-rcClient.left,rcInvalid.top-rcClient.top,SRCCOPY);            

	m_pWebView->paint(hDcPaint, rcInvalid.left,rcInvalid.top,
		rcInvalid.right - rcInvalid.left, rcInvalid.bottom - rcInvalid.top,
		rcInvalid.left-rcClient.left,
		rcInvalid.top-rcClient.top,true);



	::EndPaint(m_hWnd, &ps);
	return 0;
}

LRESULT wkeWebkitWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	//m_dm.init(m_hWnd);
	//verticalLayoutUC *pRoot = new wuc::verticalLayoutUC();
	//pRoot->setAutoDestroy(false);
	//pRoot->setManager(&m_dm, NULL, false);
	//pRoot->add(m_pOwner);
	//m_dm.attachDialog(pRoot);

	m_pWebView = wkeCreateWebView();
	if(!m_pWebView) return -1;
	// 
	m_pWebView->setBufHandler(this);
	// 
	m_pOwner->setClientHandler(&m_clientHandler);	// 在~wkeWebkitUC()中移出集合

	// 	hander.onTitleChanged = onTitleChanged;   //默认关闭了通知标题改变的回调函数
	// 	hander.onURLChanged = onURLChanged;
	// 	m_pWebView->setClientHandler(&hander);
	if (m_pOwner->getManager()->isLayerWindowTransparent())
	{
		m_pOwner->getManager()->addLayerWindowControl(m_pOwner, m_hWnd);
	}
	SetTimer(m_hWnd, TM_TICKER, 50, NULL);

	return 0;
}

LRESULT wkeWebkitWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	RECT rcWnd = {0};
	::GetWindowRect(*this, &rcWnd);

	m_pWebView->resize(rectWidth(rcWnd), rectHeight(rcWnd));
	m_pWebView->tick();

	::InvalidateRect(m_hWnd, NULL, FALSE);
	return 0;
}

LRESULT wkeWebkitWnd::OnMouseEvent(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled)
{
	if (uMsg == WM_LBUTTONDOWN || uMsg == WM_MBUTTONDOWN || uMsg == WM_RBUTTONDOWN)
	{
		SetFocus(m_hWnd);
		SetCapture(m_hWnd);
	}
	else if (uMsg == WM_LBUTTONUP || uMsg == WM_MBUTTONUP || uMsg == WM_RBUTTONUP)
	{
		ReleaseCapture();
	}

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	int x = GET_X_LPARAM(lParam)-rcClient.left;
	int y = GET_Y_LPARAM(lParam)-rcClient.top;

	unsigned int flags = 0;

	if (wParam & MK_CONTROL)
		flags |= WKE_CONTROL;
	if (wParam & MK_SHIFT)
		flags |= WKE_SHIFT;

	if (wParam & MK_LBUTTON)
		flags |= WKE_LBUTTON;
	if (wParam & MK_MBUTTON)
		flags |= WKE_MBUTTON;
	if (wParam & MK_RBUTTON)
		flags |= WKE_RBUTTON;

	bHandled = m_pWebView->mouseEvent(uMsg, x, y, flags);
	return 0;
}

LRESULT wkeWebkitWnd::OnMouseWheel(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ScreenToClient(m_hWnd, &pt);

	int delta = GET_WHEEL_DELTA_WPARAM(wParam);

	unsigned int flags = 0;

	if (wParam & MK_CONTROL)
		flags |= WKE_CONTROL;
	if (wParam & MK_SHIFT)
		flags |= WKE_SHIFT;

	if (wParam & MK_LBUTTON)
		flags |= WKE_LBUTTON;
	if (wParam & MK_MBUTTON)
		flags |= WKE_MBUTTON;
	if (wParam & MK_RBUTTON)
		flags |= WKE_RBUTTON;

	bHandled = m_pWebView->mouseWheel(pt.x, pt.y, delta, flags);
	return 0;
}

LRESULT wkeWebkitWnd::OnKeyDown(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled)
{
	unsigned int flags = 0;
	if (HIWORD(lParam) & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (HIWORD(lParam) & KF_EXTENDED)
		flags |= WKE_EXTENDED;

	bHandled = m_pWebView->keyDown(wParam, flags, false);
	return 0;
}
LRESULT wkeWebkitWnd::OnKeyUp(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled)
{
	unsigned int flags = 0;
	if (HIWORD(lParam) & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (HIWORD(lParam) & KF_EXTENDED)
		flags |= WKE_EXTENDED;

	bHandled = m_pWebView->keyUp(wParam, flags, false);
	return 0;
}

LRESULT wkeWebkitWnd::OnChar(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled)
{
	unsigned int charCode = wParam;
	unsigned int flags = 0;
	if (HIWORD(lParam) & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (HIWORD(lParam) & KF_EXTENDED)
		flags |= WKE_EXTENDED;

	//flags = HIWORD(lParam);

	bHandled = m_pWebView->keyPress(charCode, flags, false);
	return 0;
}

LRESULT wkeWebkitWnd::OnImeStartComposition(UINT uMsg, WPARAM wParam,LPARAM lParam, bool& bHandled)
{
	wkeRect caret = m_pWebView->getCaret();

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	CANDIDATEFORM form;
	form.dwIndex = 0;
	form.dwStyle = CFS_EXCLUDE;
	form.ptCurrentPos.x = caret.x + rcClient.left;
	form.ptCurrentPos.y = caret.y + caret.h + rcClient.top;
	form.rcArea.top = caret.y + rcClient.top;
	form.rcArea.bottom = caret.y + caret.h + rcClient.top;
	form.rcArea.left = caret.x + rcClient.left;
	form.rcArea.right = caret.x + caret.w + rcClient.left;
	COMPOSITIONFORM compForm;
	compForm.ptCurrentPos=form.ptCurrentPos;
	compForm.rcArea=form.rcArea;
	compForm.dwStyle=CFS_POINT;

	HWND hWnd=m_pOwner->getManager()->getPaintWindow();
	HIMC hIMC = ImmGetContext(hWnd);
	ImmSetCandidateWindow(hIMC, &form);
	ImmSetCompositionWindow(hIMC,&compForm);
	ImmReleaseContext(hWnd, hIMC);

	//bHandled = TRUE;
	return 0;
}

LRESULT wkeWebkitWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	m_pWebView->focus();
	//	bHandled = TRUE;
	return 0;
}

LRESULT wkeWebkitWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	m_pWebView->unfocus();
	//bHandled = TRUE;
	return 0;
}

LRESULT wkeWebkitWnd::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);

	if (pt.x != -1 && pt.y != -1)
		ScreenToClient(m_hWnd, &pt);

	unsigned int flags = 0;

	if (wParam & MK_CONTROL)
		flags |= WKE_CONTROL;
	if (wParam & MK_SHIFT)
		flags |= WKE_SHIFT;

	if (wParam & MK_LBUTTON)
		flags |= WKE_LBUTTON;
	if (wParam & MK_MBUTTON)
		flags |= WKE_MBUTTON;
	if (wParam & MK_RBUTTON)
		flags |= WKE_RBUTTON;

	bHandled = m_pWebView->contextMenuEvent(pt.x, pt.y, flags);
	return 0;
}
/*static*/ void wkeWebkitWnd::wke_onTitleChanged(const struct _wkeClientHandler* clientHandler, const wkeString title)
{
	wkeWebkitUC *pWebkit = wkeWebkitUC::findWebkitUC(clientHandler);
	if (pWebkit && pWebkit->getManager())
	{
		const wchar_t *pstr = wkeToStringW(title);
		if(pstr) pWebkit->m_pWindow->m_wsCurrentTitle = pstr;
		pWebkit->m_pWindow->m_bTitleChanged = true;
		
		//SEND_LOG(L"WARNING wkeWebkitWnd::wke_onTitleChanged 标题发生改变 title:<%s>", pWebkit->m_pWindow->m_wsCurrentTitle.c_str());
		pWebkit->getManager()->sendNotify(pWebkit, L"wke_titlechanged", (WPARAM)pWebkit->m_pWindow->m_wsCurrentTitle.c_str());

	}
}
//jsValue JS_CALL test(jsExecState es)
//{
//	std::wstring ws = L"aaa";
//	SEND_LOG(L"ERROR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.");
//	ws.clear();
//	return jsUndefined();
//}
//
//jsValue JS_CALL js_msgBox(jsExecState es)
//{
//	const wchar_t* text = jsToStringW(es, jsArg(es, 0));
//	//const wchar_t* title = jsToStringW(es, jsArg(es, 1));
//
//	//HWND hwnd = CFrameWnd::MainWnd()->GetHWND();
//	//if (::IsWindow(hwnd) )
//	//{
//	//	::PostMessage(hwnd, WM_USER_JS_NOTIFY, (WPARAM)text, (LPARAM)title);
//	//}
//	MessageBox(NULL, text, NULL, 0);
//	SEND_LOG(L"ERROR %s", text);
//	return jsUndefined();
//}
/*static*/ void wkeWebkitWnd::wke_onUrlChanged(const struct _wkeClientHandler* clientHandler, const wkeString url)
{
	wkeWebkitUC *pWebkit = wkeWebkitUC::findWebkitUC(clientHandler);
	if (pWebkit && pWebkit->getManager())
	{

		const wchar_t *pstr = wkeToStringW(url);
		if(pstr) pWebkit->m_pWindow->m_wsCurrentUrl = pstr;

		//std::wstring wsUrl = wkeToStringW(url);
		pWebkit->m_pWindow->m_bUrlChanged = true;
		pWebkit->m_pWindow->m_bLoadComplete = pWebkit->m_pWindow->m_bLoaded = pWebkit->m_pWindow->m_bDocumentReady = false;
		//SEND_LOG(L"WARNING wkeWebkitWnd::wke_onUrlChanged URL发生改变 title:<%s> newurl:<%s>", pWebkit->m_pWindow->m_wsCurrentTitle.c_str(), pWebkit->m_pWindow->m_wsCurrentUrl.c_str());
		pWebkit->getManager()->sendNotify(pWebkit, L"wke_urlchanged", (WPARAM)pWebkit->m_pWindow->m_wsCurrentUrl.c_str());
	}

}

LRESULT wkeWebkitWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	//if(m_pOwner && m_pOwner->getVerticalScrollBar())
	//{
	//	//m_pOwner->getVerticalScrollBar()->invalidate();
	//	InvalidateRect(m_hWnd, &m_pOwner->getVerticalScrollBar()->getPosition(), TRUE);
	//}
	static int c_tick_scroll = 0;
	static int c_check_scroll = 0;
	if(wParam == TM_TICKER)
	{
		m_pWebView->tick();

		assert(m_pOwner);
		if (!m_pOwner)
		{
			bHandled = TRUE;
			return 0;
		}
		if (m_bCheckScroll)
		{
			if(c_tick_scroll++ == SCROLL_TICKS)
			{
				++c_check_scroll;
				c_tick_scroll = 0;
				SIZE sz = {m_pWebView->contentsWidth(), m_pWebView->contentsHeight()};
				if (sz != m_szScroll)
				{
					c_check_scroll = 0;
					m_szScroll = sz;
					m_pOwner->initScrollBar(m_szScroll);
					SEND_LOG(L"wkeWebkitWnd::OnTimer scroll_ticks 检查");
					//m_bCheckScroll = false;
				}
			}
			if (c_check_scroll == SCROLL_CHECK_MAX)
			{
				c_check_scroll = 0;
				c_tick_scroll = 0;
				m_bCheckScroll = false;

				SIZE sz = {m_pWebView->contentsWidth(), m_pWebView->contentsHeight()};
				if (sz != m_szScroll)
				{
					m_bCheckScroll = true;	// 还得查
					m_szScroll = sz;
					m_pOwner->initScrollBar(m_szScroll);
					m_bCheckScroll = false;
				}
				SEND_LOG(L"wkeWebkitWnd::OnTimer scroll_ticks 检查 over：%d", m_bCheckScroll);
			}
		}
		if (m_bUrlChanged && !m_bDocumentReady && m_pWebView->isDocumentReady())
		{
			m_bDocumentReady = true;
			// 是否隐藏原始滚动条
			if(m_pOwner->m_pVerticalScrollBar || m_pOwner->m_pHorizontalScrollBar)
				m_pOwner->runJS(L"document.body.style.overflow='hidden'");

			m_pOwner->getManager()->sendNotify(m_pOwner, L"wke_documentready");
			//SEND_LOG(L"NOTE wkeWebkitWnd::OnTimer_isDocumentReady name:<%s> title:<%s>", gtc::stringUtil::toWChar(m_pWebView->name()).c_str(), m_pWebView->titleW());
		}
		if (m_bUrlChanged && !m_bLoaded && m_pWebView->isLoaded())
		{
			m_bLoaded = true;
			//SEND_LOG(L"NOTE wkeWebkitWnd::OnTimer_isLoaded name:<%s> title:<%s>", gtc::stringUtil::toWChar(m_pWebView->name()).c_str(), m_pWebView->titleW());
			// 是否隐藏原始滚动条
			if(m_pOwner->m_pVerticalScrollBar || m_pOwner->m_pHorizontalScrollBar)
				m_pOwner->runJS(L"document.body.style.overflow='hidden'");
			m_pOwner->getManager()->sendNotify(m_pOwner, L"wke_loaded");
		}
		if (m_bUrlChanged && !m_bLoadComplete && m_pWebView->isLoadComplete())
		{
			m_bLoadComplete = true;
			SEND_LOG(L"NOTE wkeWebkitWnd::OnTimer_isLoadComplete 执行 initScrollBar");
			m_szScroll.cx = m_pWebView->contentsWidth();
			m_szScroll.cy = m_pWebView->contentsHeight();
			m_pOwner->initScrollBar(m_szScroll);
			m_bCheckScroll = true;	// 开始检查内容尺寸变化
			c_tick_scroll = 0;
			c_check_scroll = 0;
			// 是否隐藏原始滚动条
			if(m_pOwner->m_pVerticalScrollBar || m_pOwner->m_pHorizontalScrollBar)
				m_pOwner->runJS(L"document.body.style.overflow='hidden'");

			//SEND_LOG(L"NOTE wkeWebkitWnd::OnTimer_isLoadComplete name:<%s> title:<%s>", gtc::stringUtil::toWChar(m_pWebView->name()).c_str(), m_pWebView->titleW());
			m_pOwner->getManager()->sendNotify(m_pOwner, L"wke_loadcomplete");

		}

		if(m_bUrlChanged && m_bDocumentReady && m_bLoaded && m_bLoadComplete)
		{
			m_bUrlChanged = false;
			//m_pOwner->setURL(L"www.ifeng.com");
		}


		// 自动隐藏滚动条
		if (m_bLoadComplete && m_pOwner && m_pOwner->isAutoHideScroll())
		{
			//RECT rcScroll = {0};
			RECT rcWnd = m_pOwner->getPosition();
			POINT pt = {0};
			GetCursorPos(&pt);
			//pWeb->getWindow()->getWindowRect(&rcWnd);
			m_pOwner->getManager()->clientToScreen(rcWnd);
			//SEND_LOG(L"dialogMain::notify wke_tick pt:<%d, %d> in:%d", pt.x, pt.y, ::PtInRect(&rcWnd, pt));
			if (::PtInRect(&rcWnd, pt))
			{
				if (!m_bMouseEnter)
				{
					m_bMouseEnter = true;
					m_pOwner->getVerticalScrollBar()->setVisible(true);
					//SEND_LOG(L"wkeWebkitWnd::OnTimer mouse_enter");
					m_pOwner->getManager()->sendNotify(m_pOwner, L"wke_enter");
				}
			}
			else
			{
				if (m_bMouseEnter)
				{
					m_bMouseEnter = false;
					if(!m_pOwner->getVerticalScrollBar()->isScrolling())
						m_pOwner->getVerticalScrollBar()->setVisible(false);
					m_pOwner->invalidate();
					//SEND_LOG(L"wkeWebkitWnd::OnTimer mouse_leave ****");
					m_pOwner->getManager()->sendNotify(m_pOwner, L"wke_leave");
				}
			}
		}

		m_pOwner->getManager()->sendNotify(m_pOwner, L"wke_tick");
	}
	bHandled = TRUE;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	wkeWebkitUC  类		2015-9-16
//	基于wke的封装库
////////////////////////////////////////////////////////////////////////////////////////////////////////

std::map<const wkeClientHandler *, wkeWebkitUC *>	wkeWebkitUC::mc_mapClientHandler;

wkeWebkitUC::wkeWebkitUC()
	: m_pWindow(NULL)
{
	//setBkColor(0xFE00FF00);
	SEND_LOG(L"wkeWebkitUC init ...");

}
wkeWebkitUC::~wkeWebkitUC()
{
	SEND_LOG(L"wkeWebkitUC free ...");
	if (m_pWindow)
	{
		m_pWindow->m_pOwner = NULL;
		//std::map<const wkeClientHandler *, wkeWebkitUC *>::const_iterator iter = mc_mapClientHandler.find(m_pWindow->m_pWebView->getClientHandler());
		//if(iter != mc_mapClientHandler.cend())
		//	mc_mapClientHandler.erase(iter);
		removeClientHandler(&m_pWindow->m_clientHandler);
		m_pWindow->postMessage(WM_CLOSE);
	}
}

/*static*/ void wkeWebkitUC::wkeWebkit_Init()
{
	wkeInit();
}
/*static*/ void wkeWebkitUC::wkeWebkit_Shutdown()
{
	wkeShutdown();
}
PCWSTR wkeWebkitUC::getClass() const
{
	return L"wkeWebkitUC";
}
LPVOID wkeWebkitUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"WkeWebkit") == 0 ) return static_cast<wkeWebkitUC*>(this);
	return __super::getInterface(pstrName);

}
void wkeWebkitUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else __super::Event(event);
		return;
	}

	if( event.Type == BEUCEventScrollWheel )
	{
		switch( LOWORD(event.wParam) ) {
		case SB_LINEUP:
			lineUp(50);
			return;
		case SB_LINEDOWN:
			//SEND_LOG(L"containerUC::Event BEUCEventScrollWheel_V SB_LINEDOWN %08X", getVerticalScrollBar()->isVisible());
			lineDown(50);
			return;
		}
	}
	if (event.Type == BEUCEventSetCursor)
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return;
	}
	if (event.Type == BEUCEventMouseLeave)
	{
		return;
	}
	__super::Event(event);
}
void wkeWebkitUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if (_tcsicmp(pstrName, L"URL") == 0)
	{
		setURL(pstrValue);
	}
	else __super::setAttribute(pstrName,pstrValue);
}

void wkeWebkitUC::setPosition(RECT pos)
{
	__super::setPosition(pos);
	//SEND_LOG(L"ERROR wkeWebkitUC::setPosition ");
	if (m_pWindow)
	{
		RECT rcPos = m_rcItem;
		rcPos.left += m_rcInset.left;
		rcPos.top += m_rcInset.top;
		rcPos.right -= m_rcInset.right;
		rcPos.bottom -= m_rcInset.bottom;
		if(m_pVerticalScrollBar) rcPos.right -= m_pVerticalScrollBar->getFixedWidth();
		if(m_pHorizontalScrollBar) rcPos.bottom -= m_pHorizontalScrollBar->getFixedHeight();

		if(m_pManager && m_pManager->isLayerWindowTransparent()) m_pManager->clientToScreen(rcPos);
		//SIZE sz = {rcPos.right - rcPos.left, rcPos.bottom - rcPos.top};
		//SEND_LOG(L"wkeWebkitUC::setPosition webpos:<%s>", gtc::format::rect(rcPos, true).c_str());
		::SetWindowPos(*m_pWindow, NULL, rcPos.left, rcPos.top, rectWidth(rcPos), rectHeight(rcPos), SWP_NOZORDER /*| SWP_NOSIZE*/| SWP_NOACTIVATE);   
		//::SetWindowPos(m_pManager->getPaintWindow(), *m_pWindow, 0, 0, 0, 0, SWP_NOSIZE);   
		//SetForegroundWindow(*m_pWindow);
		SEND_LOG(L"NOTE wkeWebkitUC::setPosition 执行 initScrollBar");
		SIZE szScroll = {m_pWindow->getWebView()->contentsWidth(), m_pWindow->getWebView()->contentsHeight()};
		initScrollBar(szScroll);
	}

}
void wkeWebkitUC::setScrollPos(SIZE szPos)
{
	int cx = 0;
	int cy = 0;
	SIZE szOldPos = getScrollPos();

	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) {
		int iLastScrollPos = m_pVerticalScrollBar->getScrollPos();
		m_pVerticalScrollBar->setScrollPos(szPos.cy);
		cy = m_pVerticalScrollBar->getScrollPos() - iLastScrollPos;
	}

	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) {
		int iLastScrollPos = m_pHorizontalScrollBar->getScrollPos();
		m_pHorizontalScrollBar->setScrollPos(szPos.cx);
		cx = m_pHorizontalScrollBar->getScrollPos() - iLastScrollPos;
	}

	if( cx == 0 && cy == 0 ) return;

	wchar_t pbuf[MAX_PATH] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"window.scrollTo(%d, %d)", szPos.cx, szPos.cy);
	runJS(pbuf);
}

void wkeWebkitUC::initScrollBar(SIZE szScroll)
{
	if(!m_pHorizontalScrollBar && !m_pVerticalScrollBar) return;
	assert(m_pWindow);
	if(!m_pWindow || !m_pWindow->getWebView()) return;

	
	//std::wstring wstr = runJS(L"document.body.scrollHeight");
	//szScroll.cy = std::wcstol(wstr.c_str(), NULL, 10);
	//wstr = runJS(L"document.body.scrollWidth");
	//szScroll.cx = std::wcstol(wstr.c_str(), NULL, 10);

	//SEND_LOG(L"ERROR wkeWebkitUC::initScrollBar 1 wnd:<%d,%d> scroll:<%d, %d>%d,%d", rectWidth(m_rcItem), rectHeight(m_rcItem), szScroll.cx, szScroll.cy, m_pWindow->getWebView()->contentsWidth(), m_pWindow->getWebView()->contentsHeight());
	
	RECT pos = m_rcItem;
	pos.left += m_rcInset.left;
	pos.top += m_rcInset.top;
	pos.right -= m_rcInset.right;
	pos.bottom -= m_rcInset.bottom;
	if(m_pVerticalScrollBar && szScroll.cy > rectHeight(pos)) pos.right -= m_pVerticalScrollBar->getFixedWidth();
	if(m_pHorizontalScrollBar && szScroll.cx > rectWidth(pos)) pos.bottom -= m_pHorizontalScrollBar->getFixedHeight();
	//if(m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible()) rc.right -= m_pVerticalScrollBar->getFixedWidth();
	//if(m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible()) rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();
	processScrollBar(pos, szScroll.cx, szScroll.cy);

	SEND_LOG(L"ERROR wkeWebkitUC::initScrollBar 2 wnd:<%d,%d> scroll:<%d, %d>", rectWidth(pos), rectHeight(pos), szScroll.cx, szScroll.cy);
	if(m_pManager && m_pManager->isLayerWindowTransparent()) m_pManager->clientToScreen(pos);
	::SetWindowPos(*m_pWindow, NULL, pos.left, pos.top, rectWidth(pos), rectHeight(pos), SWP_NOZORDER | SWP_NOACTIVATE);
}

void wkeWebkitUC::onInit()
{
	if(m_pWindow) return;
	SEND_LOG(L"wkeWebkitUC::onInit begin");
	try
	{
		
		//wuc::wkeWebkitUC::wkeWebkit_Init();
		m_pWindow = new wkeWebkitWnd();
		if(m_pWindow)
		{
			//jsBindFunction("newWindowOpen", js_msgBox, 1);
			//jsBindFunction("msgBox", js_msgBox, 2);//这里绑定js函数，让js主动调用c++函数

			m_pWindow->init(this);
			m_pWindow->showWindow();
		}
		
	}
	catch (gtc::baseException &e)
	{
		SEND_LOG(L"ERROR wkeWebkitUC::onInit %s", e.description().c_str());
	#if !defined(_DEBUG)
		WRITE_LOG(L"throw wkeWebkitUC::onInit %s", e.description().c_str());
	#endif
	}
	catch (...)
	{
		SEND_LASTERROR();
	}
	SEND_LOG(L"wkeWebkitUC::onInit end");
}
void wkeWebkitUC::setVisible(bool bVisible /*= true*/)
{
	__super::setVisible(bVisible);
	setInternVisible(bVisible);
}

void wkeWebkitUC::setInternVisible(bool bVisible /*= true*/)
{
	__super::setInternVisible(bVisible);
	if (m_pWindow)
	{
		m_pWindow->showWindow(bVisible);
		SEND_LOG(L"NOTE wkeWebkitUC::setInternVisible 显示");
	}
}
void wkeWebkitUC::setURL(std::wstring strValue)
{
	assert(m_pWindow);
	if(!m_pWindow) return;
	m_pWindow->m_wsUrl = strValue;

	m_pWindow->m_pWebView->loadURL(strValue.c_str());

}
void wkeWebkitUC::setFile(std::wstring strValue)
{
	assert(m_pWindow);
	if(!m_pWindow) return;
	m_pWindow->m_wsUrl = strValue;
	m_pWindow->m_pWebView->loadFile(strValue.c_str());

}
std::wstring wkeWebkitUC::runJS(std::wstring strValue)
{
	//assert(m_pWindow);
	if(!m_pWindow) return L"";
	jsValue jsRet = m_pWindow->m_pWebView->runJS(strValue.c_str());
	return jsToStringW(m_pWindow->m_pWebView->globalExec(), jsRet);

}
void wkeWebkitUC::setClientHandler(const wkeClientHandler* handler)
{
	assert(m_pWindow);
	if(!m_pWindow) return;
	m_pWindow->m_pWebView->setClientHandler(handler);

	mc_mapClientHandler[handler] = this;
}

void wkeWebkitUC::stopLoading()
{
	assert(m_pWindow);
	if(!m_pWindow) return;
	if(m_pWindow && m_pWindow->m_pWebView)
	{
		wkeStopLoading(m_pWindow->m_pWebView);
		//m_pWindow->m_pWebView->goBack();
	}
}
/*static*/ wkeWebkitUC* wkeWebkitUC::findWebkitUC(const wkeClientHandler *pHandler)
{
	std::map<const wkeClientHandler *, wkeWebkitUC *>::const_iterator iter = mc_mapClientHandler.find(pHandler);
	if(iter != mc_mapClientHandler.cend())
		return iter->second;
	return nullptr;
	
}
/*static*/ void wkeWebkitUC::removeClientHandler(const wkeClientHandler *pHandler)
{
	assert(pHandler);
	if(!pHandler) return;
	std::map<const wkeClientHandler *, wkeWebkitUC *>::const_iterator iter = mc_mapClientHandler.find(pHandler);
	if(iter != mc_mapClientHandler.cend())
		mc_mapClientHandler.erase(iter);
}
void wkeWebkitUC::goBack()
{
	assert(m_pWindow);
	if(!m_pWindow) return;
	if (m_pWindow->m_pWebView->canGoBack())
		m_pWindow->m_pWebView->goBack();

}
void wkeWebkitUC::goForward()
{
	assert(m_pWindow);
	if(!m_pWindow) return;
	if (m_pWindow->m_pWebView->canGoForward())
		m_pWindow->m_pWebView->goForward();

}
void wkeWebkitUC::buildJSFunction(const char *jsfuncName, jsNativeFunction exeFunc, int argCount)
{
	assert(m_pWindow);
	if(!m_pWindow) return;
	assert(jsfuncName);
	assert(exeFunc);
	if(!jsfuncName || !exeFunc || std::strlen(jsfuncName) == 0) return;

	jsBindFunction(jsfuncName, exeFunc, argCount);
}
windowWnd* wkeWebkitUC::getWindow()
{
	return dynamic_cast<windowWnd *>(m_pWindow);
}
std::wstring wkeWebkitUC::getURL() const
{
	assert(m_pWindow);
	if(!m_pWindow) return L"";
	return m_pWindow->m_wsUrl;
}
std::wstring wkeWebkitUC::getCurrentURL() const
{
	assert(m_pWindow);
	if(!m_pWindow) return L"";
	return m_pWindow->m_wsCurrentUrl;

}
}	// wuc