#include "StdAfx.h"
#include "kpdeskWnd.h"


kpdeskWnd::kpdeskWnd(void)
{
}


kpdeskWnd::~kpdeskWnd(void)
{
	TRACE_CLASS_BEGIN(this)
}

void kpdeskWnd::onFinalMessage(HWND hWnd)
{
	//m_paintManager.removePreMessageFilter(this);
	m_paintManager.removeNotifier(this);
	m_paintManager.reapObjects(m_paintManager.getRoot());

}
void kpdeskWnd::notify(TNotifyUI& msg)
{
	if (msg.sType == L"click")
	{
		if (msg.pSender->getName() == kpButtonLock)
		{
			MessageBox(NULL, L"lock", L"hello", 0);
		}
		else if (msg.pSender->getName() == kpButtonMenu)
		{
			MessageBox(NULL, L"menu", L"hello", 0);
		}
		else if(msg.pSender->getName() == kpButtonTitle/*signaturetip*/)
		{
			msg.pSender->setVisible(false);
			richEditUI* redit = static_cast<richEditUI*>(m_paintManager.findControl(L"reditTitle"/*signature*/)); 
			if (redit != NULL)
			{
				wui::rect rt = msg.pSender->getPos();
				rt.deflate(rt.width()/4, 0);
				rt.offset(0, 2);
				redit->setPos(rt);
				redit->setFixedWidth(rt.width());
				redit->setText(msg.pSender->getText().c_str());
				redit->setVisible(true);
				redit->setFocus();
				redit->setSelAll();
			}

		}
	}
	if (msg.sType == L"killfocus")
	{
		if (msg.pSender->getName() == kpREditTitle)
		{
			msg.pSender->setVisible(false);
			controlUI* btnTitle = m_paintManager.findControl(kpButtonTitle);
			if (btnTitle != NULL)
			{
				richEditUI* reditTitle = static_cast<richEditUI*>(msg.pSender);
				if (reditTitle != NULL)
					btnTitle->setText(reditTitle->getText().c_str());
				btnTitle->setVisible(true);
			}
		}
	}
}

LRESULT kpdeskWnd::onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT kpdeskWnd::onGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
	return S_OK;

}

LRESULT kpdeskWnd::onSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
	return S_OK;

}

LRESULT kpdeskWnd::onSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT kpdeskWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = S_OK;
	BOOL bHandled = TRUE;
	switch(uMsg)
	{
	case WM_CREATE:
		{
			m_paintManager.init(m_hWnd);	// 准备绘图环境

			dialogBuilder builder;			//duilib.xml
			controlUI* pRoot = builder.create(L"kpsk\\desk.xml", (UINT)0, NULL, &m_paintManager);   // duilib.xml需要放到exe目录下
			assert(pRoot && L"Failed to parse XML");

			m_paintManager.attachDialog(pRoot);
			m_paintManager.addNotifier(this);   // 添加控件等消息响应，这样消息就会传达到duilib的消息循环，我们可以在Notify函数里做消息处理

			//wui::controlUI *pWnd = new wui::buttonUI();
			//pWnd->setText(L"Hello World");   // 设置文字
			//pWnd->setBkColor(0xFF00FF00);       // 设置背景色
			//pWnd->setName(L"btnHello");

			//m_paintManager.attachDialog(pWnd);	// initControls pControl->setManager pControl->findControl * pManager->m_mNameHash
			//m_paintManager.addNotifier(this);
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
		lRes = onSysCommand(uMsg, wParam, lParam, bHandled);
		break;
	default:
		bHandled = FALSE;
		break;
	}

	if (bHandled) return lRes;

	//lRes = handleCustomMessage(uMsg, wParam, lParam, bHandled);
	//if (bHandled) return lRes;

	if (m_paintManager.messageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return windowWnd::handleMessage(uMsg, wParam, lParam);


}