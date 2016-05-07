#include "StdAfx.h"
#include "windowImplBase.h"


namespace wui {



//#if !defined(UNDER_CE) && defined(_DEBUG)
//#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#endif
//
//#if USE(ZIP_SKIN)
//static const TCHAR* const kResourceSkinZipFileName = L"QQRes.zip");
//#endif

windowImplBase::windowImplBase()
{

}

windowImplBase::~windowImplBase()
{

}

UINT windowImplBase::getClassStyle() const
{
	return CS_DBLCLKS;
}

controlUI* windowImplBase::createControl(PCWSTR pstrClass)
{
	return NULL;
}

void windowImplBase::onFinalMessage(HWND /*hWnd*/)
{
	m_paintManager.removePreMessageFilter(this);
	m_paintManager.removeNotifier(this);
	m_paintManager.reapObjects(m_paintManager.getRoot());
}

void windowImplBase::init()
{}

LRESULT windowImplBase::onClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT windowImplBase::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}


LRESULT windowImplBase::onNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT windowImplBase::onNcCalcSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT windowImplBase::onNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT windowImplBase::onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	if( !::IsZoomed(*this) ) {
		RECT rcSizeBox = m_paintManager.getSizeBox();
		if( pt.y < rcClient.top + rcSizeBox.top ) {
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
			return HTTOP;
		}
		else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}
		if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
		if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
	}

	RECT rcCaption = m_paintManager.getCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			controlUI* pControl = static_cast<controlUI*>(m_paintManager.findControl(pt));
			if( pControl && _tcsicmp(pControl->getClass(), L"ButtonUI") != 0 && 
				_tcsicmp(pControl->getClass(), L"OptionUI") != 0 /*&&
				_tcsicmp(pControl->GetClass(), L"TextUI")) != 0 */)
				return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT windowImplBase::onGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	rect rcWork = oMonitor.rcWork;
	rcWork.offset(-rcWork.left, -rcWork.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return 0;
}

LRESULT windowImplBase::onMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}



LRESULT windowImplBase::onSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT windowImplBase::onLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT windowImplBase::onMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT windowImplBase::onLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT windowImplBase::onKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT windowImplBase::onKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT windowImplBase::onSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_paintManager.getRoundCorner();

	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		rect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}

	bHandled = FALSE;
	return 0;
}


LRESULT windowImplBase::onSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == SC_CLOSE){
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = windowWnd::handleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed ){
	}
	return lRes;
}


std::wstring windowImplBase::getSkinFolder()
{
	return (paintManagerUI::getInstancePath() + L"skin\\").c_str();
}

LRESULT windowImplBase::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	m_paintManager.init(m_hWnd);
	m_paintManager.addPreMessageFilter(this);

	dialogBuilder builder;

	//m_paintManager.setResourcePath(getSkinFolder().c_str());


	//stringex tstrSkin = m_paintManager.getResourcePath();
	//tstrSkin += getSkinFile();
	stringex tstrSkin = getSkinFile();
	controlUI* pRoot = builder.create(tstrSkin.c_str(), (UINT)0, this, &m_paintManager);
	m_paintManager.attachDialog(pRoot);
	m_paintManager.addNotifier(this);

	init();
	return 0;
}

LRESULT windowImplBase::handleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT windowImplBase::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg)
	{
	case WM_CREATE:			lRes = onCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:			lRes = onClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:		lRes = onDestroy(uMsg, wParam, lParam, bHandled); break;
	case WM_NCACTIVATE:		lRes = onNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:		lRes = onNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	case WM_NCPAINT:		lRes = onNcPaint(uMsg, wParam, lParam, bHandled); break;
	case WM_NCHITTEST:		lRes = onNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_GETMINMAXINFO:	lRes = onGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEWHEEL:		lRes = onMouseWheel(uMsg, wParam, lParam, bHandled); break;

	case WM_SIZE:			lRes = onSize(uMsg, wParam, lParam, bHandled); break;	
	case WM_SYSCOMMAND:		lRes = onSysCommand(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYDOWN:		lRes = onKeyDown(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:		lRes = onKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_SETFOCUS:		lRes = onSetFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONUP:		lRes = onLButtonUp(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONDOWN:	lRes = onLButtonDown(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEMOVE:		lRes = onMouseMove(uMsg, wParam, lParam, bHandled); break;
	default:				bHandled = FALSE; break;
	}
	if (bHandled) return lRes;

	lRes = handleCustomMessage(uMsg, wParam, lParam, bHandled);
	if (bHandled) return lRes;

	if (m_paintManager.messageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return windowWnd::handleMessage(uMsg, wParam, lParam);
}

LRESULT windowImplBase::messageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
			return responseDefaultKeyEvent(wParam);
		default:
			break;
		}
	}
	return FALSE;
}

LRESULT windowImplBase::responseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		close();
		return TRUE;
	}

	return FALSE;
}

}