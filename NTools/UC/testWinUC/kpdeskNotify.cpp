#include "StdAfx.h"
#include "kpdeskNotify.h"


kpdeskNotify::kpdeskNotify(void)
	: wuc::windowWnd()
{
}


kpdeskNotify::~kpdeskNotify(void)
{
}

void kpdeskNotify::onFinalMessage(HWND hWnd)
{
	//m_paintManager.removePreMessageFilter(this);
	m_paintManager.removeNotifier(this);
	m_paintManager.reapObjects(m_paintManager.getRoot());

}
void kpdeskNotify::notify(wuc::BSNotifyUC& msg)
{
	if (msg.wsType == L"click")
	{

	}
	if (msg.wsType == L"killfocus")
	{

	}
}


LRESULT kpdeskNotify::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = S_OK;
	BOOL bHandled = TRUE;
	DEBUG_MSG(L"kpdeskNotify::handleMessage:  %04X %08X %08X", uMsg, wParam, lParam);
	switch(uMsg)
	{
	case WM_CREATE:
		{
			m_paintManager.init(m_hWnd);	// 准备绘图环境
			RECT rc = {0};
			GetClientRect(m_hWnd, &rc);
			wuc::controlUC *pRoot = new wuc::controlUC();
			pRoot->setName(L"controlRoot");
			pRoot->setPos(rc);
			assert(pRoot && L"布局控件失败！");
			pRoot->setBkImage(L"notify_main");

			m_btnKnow.setName(L"bottonKnow");
			rc.left = 345;
			rc.right = rc.left + 104;
			rc.top = 260;
			rc.bottom = rc.top + 39;
			m_btnKnow.setPos(rc);
			rc.left = rc.top = 0;
			rc.right = 104;
			rc.bottom = 39;
			m_btnKnow.setNormalImage(L"notify_know", &rc);
			::OffsetRect(&rc, 0, 39);
			m_btnKnow.setHotImage(L"notify_know", &rc);
			::OffsetRect(&rc, 0, 39);
			m_btnKnow.setPushedImage(L"notify_know", &rc);
			m_paintManager.initControls(&m_btnKnow, pRoot);
			//m_paintManager.setTransparent(100);

			rc.left = rc.right = rc.top = 0;
			rc.bottom = 40;
			m_paintManager.setCaptionRect(rc);
			m_paintManager.attachDialog(pRoot);
			m_paintManager.addNotifier(this);   // 添加控件等消息响应，这样消息就会传达到duilib的消息循环，我们可以在Notify函数里做消息处理

			return lRes;
		}
		break;
	case WM_DESTROY:
		{
			::PostQuitMessage(0L);
		}
		break;
	case WM_ERASEBKGND:
		return 1;
		break;
	case WM_NCACTIVATE: // 以下3个消息WM_NCACTIVATE、WM_NCCALCSIZE、WM_NCPAINT用于屏蔽系统标题栏
		{
			if( !::IsIconic(m_hWnd) ) 
			{
				return (wParam == 0) ? TRUE : FALSE;
			}
		}
		break;
	case WM_NCCALCSIZE:
		return 0;
		break;
	case WM_NCPAINT:
		return 0;
		break;
	case WM_NCHITTEST:
		lRes = onNcHitTest(uMsg, wParam, lParam, bHandled);
		//MessageBox(NULL, L"WM_NCHITTEST", L"msg", 0);
		break;
	case WM_GETMINMAXINFO:
		lRes = onGetMinMaxInfo(uMsg, wParam, lParam, bHandled);
		break;
	case WM_SIZE:
		lRes = onSize(uMsg, wParam, lParam, bHandled);
		break;
	case WM_SYSCOMMAND:
		//lRes = onSysCommand(uMsg, wParam, lParam, bHandled);
		break;
	default:
		bHandled = FALSE;
		break;
	}

	if (bHandled) return lRes;

	//lRes = handleCustomMessage(uMsg, wParam, lParam, bHandled);
	//if (bHandled) return lRes;

	if (m_paintManager.messageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return wuc::windowWnd::handleMessage(uMsg, wParam, lParam);


}



LRESULT kpdeskNotify::onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
			wuc::controlUC* pControl = static_cast<wuc::controlUC*>(m_paintManager.findControl(pt));
			if( pControl && _tcsicmp(pControl->getClass(), L"ButtonUI") != 0 && 
				_tcsicmp(pControl->getClass(), L"OptionUI") != 0 /*&&
				_tcsicmp(pControl->GetClass(), L"TextUI")) != 0 */)
				return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT kpdeskNotify::onGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	RECT rcWork = oMonitor.rcWork;
	//rcWork.offset(-rcWork.left, -rcWork.top);
	::OffsetRect(&rcWork, -rcWork.left, -rcWork.top);
	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return S_OK;

}

LRESULT kpdeskNotify::onSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_paintManager.getRoundCorner();
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		RECT rcWnd;
		::GetWindowRect(*this, &rcWnd);
		//rcWnd.offset(-rcWnd.left, -rcWnd.top);
		::OffsetRect(&rcWnd, -rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}

	bHandled = FALSE;
	return S_OK;

}