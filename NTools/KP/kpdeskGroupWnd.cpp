#include "StdAfx.h"
#include "kpdeskGroupWnd.h"
//// 往用户发送调试信息
//#if defined(_DEBUG)
//#define SEND_LOG(log) gtc::app::sendtoUser(log, L"无标题 - testDeskHost");
//#else
//#define SEND_LOG(log)
//#endif
kpdeskGroupWnd::kpdeskGroupWnd(void)
	: m_bWinInit(false)
{
	m_wsGuid = gtc::app::createGUID();
}


kpdeskGroupWnd::~kpdeskGroupWnd(void)
{
	wui::TRACE_CLASS_BEGIN(this)
}

void kpdeskGroupWnd::onFinalMessage(HWND hWnd)
{
	//m_paintManager.removePreMessageFilter(this);
	m_paintManager.removeNotifier(this);
	m_paintManager.reapObjects(m_paintManager.getRoot());

}
void kpdeskGroupWnd::notify(wui::TNotifyUI& msg)
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
		else if(msg.pSender->getName() == kpTextTitle/*kpButtonTitle signaturetip*/)
		{
			msg.pSender->setVisible(false);
			wui::richEditUI* redit = static_cast<wui::richEditUI*>(m_paintManager.findControl(L"reditTitle"/*signature*/)); 
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
		else if (msg.pSender->getName() == kpButtonNode)
		{
			//gtc::deskListCtrl *plist = static_cast<gtc::deskListCtrl *>(m_paintManager.findControl(L"lstDesk"));
			//assert(plist && L"节点容器lstDesk不存在！");
			//wchar_t buf[80] = {0};
			//RECT rt = msg.pSender->getPos();
			//POINT pt;
			//pt.x = rt.left;
			//pt.y = rt.top;
			//::ClientToScreen(getHWND(), &pt);
			//std::swprintf(buf, _countof(buf), L"%d %d %d %d(%d,%d)", rt.left,rt.top,rt.right,rt.bottom,
			//	pt.x, pt.y);
			////msg.pSender->setText(buf);
			////gtc::app::writeLogs(buf, L"POS_C");
			//SEND_LOG(buf)
		}
	}
	if (msg.sType == L"killfocus")
	{
		if (msg.pSender->getName() == kpREditTitle)
		{
			msg.pSender->setVisible(false);
			wui::controlUI* txtTitle = m_paintManager.findControl(kpTextTitle/*kpButtonTitle*/);
			if (txtTitle != NULL)
			{
				wui::richEditUI* reditTitle = static_cast<wui::richEditUI*>(msg.pSender);
				if (reditTitle != NULL)
					txtTitle->setText(reditTitle->getText().c_str());
				txtTitle->setVisible(true);
			}
		}
	}
	if (msg.sType == L"windowinit")
	{
		m_bWinInit = true;
		wui::textUI *ptxt = static_cast<wui::textUI *>(m_paintManager.findControl(L"txtTitle"));
		if (ptxt)
		{
			ptxt->onNotify += makeDelegate(this, &kpdeskGroupWnd::textTitleNotify);
		}
		else
			SEND_LOG(L"没有找到txtTitle");
		redrawNodes();
	}
}
LRESULT kpdeskGroupWnd::onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
			wui::controlUI* pControl = static_cast<wui::controlUI*>(m_paintManager.findControl(pt));
			if( pControl && _tcsicmp(pControl->getClass(), L"ButtonUI") != 0 && 
				_tcsicmp(pControl->getClass(), L"OptionUI") != 0 /*&&
				_tcsicmp(pControl->GetClass(), L"TextUI")) != 0 */)
				return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT kpdeskGroupWnd::onGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	wui::rect rcWork = oMonitor.rcWork;
	rcWork.offset(-rcWork.left, -rcWork.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return S_OK;

}

LRESULT kpdeskGroupWnd::onSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_paintManager.getRoundCorner();
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		wui::rect rcWnd;
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

LRESULT kpdeskGroupWnd::onWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 获得桌面工作区大小,不包括任务栏
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rt,0);
	int maxWidth = rt.right - rt.left;// GetSystemMetrics(SM_CXSCREEN);
	int maxHeight = rt.bottom - rt.top;// GetSystemMetrics(SM_CYSCREEN);
	WINDOWPOS *ppos = ( WINDOWPOS *)lParam;
	if(ppos->x < 0)	ppos->x = 0;
	if(ppos->y < 0) ppos->y = 0;

	if(ppos->x + ppos->cx > maxWidth)	ppos->x = maxWidth - ppos->cx;
	if(ppos->y + ppos->cy > maxHeight)	ppos->y = maxHeight - ppos->cy;
	if(m_bWinInit)
		redrawNodes();
	bHandled = TRUE;
	return S_OK;
}
LRESULT kpdeskGroupWnd::onSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == SC_CLOSE){
		bHandled = TRUE;
		return 0;
	}
	if (wParam == SC_MINIMIZE)
	{
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = wui::windowWnd::handleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed ){
	}
	return lRes;

}

LRESULT kpdeskGroupWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = S_OK;
	BOOL bHandled = TRUE;
	switch(uMsg)
	{
	case WM_CREATE:
		{
			m_paintManager.init(m_hWnd);	// 准备绘图环境

			wui::dialogBuilder builder;			//duilib.xml
			dialogBuilderCallbackEx cb;
			wui::controlUI* pRoot = builder.create(L"kpsk\\desk.xml", (UINT)0, &cb, &m_paintManager);   // duilib.xml需要放到exe目录下
			assert(pRoot && L"Failed to parse XML");


			m_paintManager.attachDialog(pRoot);
			m_paintManager.addNotifier(this);   // 添加控件等消息响应，这样消息就会传达到duilib的消息循环，我们可以在Notify函数里做消息处理

			//SEND_LOG(L"共分配了%d个节点。",  m_mapDeskNodes.size());
			//// 分配布局区域
			//allocNodes();
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
			SEND_LOG(L"系统正在退出子窗口...");
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
	case WM_WINDOWPOSCHANGING:
		lRes = onWindowPosChanging(uMsg, wParam, lParam, bHandled);
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


void kpdeskGroupWnd::setWindowPos(int32_t idx)
{
	assert(::IsWindow(m_hWnd));
	assert((GetWindowStyle(m_hWnd)&WS_CHILD)==0);

	const int32_t rows = 2;	// 行数
	const int32_t cols = 4;	// 列数
	const int32_t spaceDeskX = 20;	// 距桌面边框空隙
	const int32_t spaceDeskY = 20;	// 距桌面边框空隙

	RECT rcDlg = { 0 };
	::GetWindowRect(m_hWnd, &rcDlg);

	int32_t row = idx/cols;
	int32_t col = idx%cols;
	RECT rcArea = { 0 };
	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
	int32_t cellw = (rcArea.right - rcArea.left - spaceDeskX*2)/cols;
	int32_t cellh = (rcArea.bottom - rcArea.top - spaceDeskY*2)/rows;
	int32_t left = rcArea.left + spaceDeskX + col * cellw;
	int32_t top = rcArea.top + spaceDeskY + row * cellh;
	int32_t width = rcDlg.right - rcDlg.left;
	int32_t height = rcDlg.bottom - rcDlg.top;
	// 修正
	if( left + width > rcArea.right - spaceDeskX ) left = rcArea.right - width - spaceDeskX;
	if( top + height > rcArea.bottom - spaceDeskY ) top = rcArea.bottom - height - spaceDeskY;

	::SetWindowPos(m_hWnd, NULL, left, top, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}


void kpdeskGroupWnd::setTitle(PCWSTR ptitle)
{
	wui::textUI *ptxt = static_cast<wui::textUI *>(m_paintManager.findControl(kpTextTitle));
	if (ptxt)
	{
		ptxt->setText(ptitle);
	}
}

/*****************************************************************************************
	<< ---  kpdeskGroupWnd::allocNodes		2014-06-09 --- >>
	说明：根据m_mapDeskNodes节点数量，在deskListCtrl中分配布局空间
	参数：
	返回值：
*****************************************************************************************/
void kpdeskGroupWnd::allocNodes()
{
	gtc::deskListCtrl *plist = static_cast<gtc::deskListCtrl *>(m_paintManager.findControl(L"lstDesk"));
	assert(plist && L"节点容器lstDesk不存在！");
	//wui::rect = GetWindowRect()
	plist->setHWND(getHWND());
	plist->setNodes(&m_mapDeskNodes);
	plist->removeAll();
	for (gtc::idlNode::IDINodeMap::const_iterator iter = m_mapDeskNodes.cbegin(); iter != m_mapDeskNodes.cend(); ++iter)
	{
		wui::dialogBuilder builder;
		wui::containerUI* pDesk = static_cast<wui::containerUI*>(builder.create(L"kpsk\\deskctrl.xml", (UINT)0));
		if (pDesk)
		{
			plist->add(pDesk);
		}
		else
		{
			plist->removeAll();
			return;
		}
	}
}

/*****************************************************************************************
	<< ---  kpdeskGroupWnd::redrawNodes		2014-06-09 --- >>
	说明：重新绘制节点信息
	参数：
	返回值：
*****************************************************************************************/
void kpdeskGroupWnd::redrawNodes()
{
	if(m_mapDeskNodes.size() <= 0) return;
	gtc::deskListCtrl *plist = static_cast<gtc::deskListCtrl *>(m_paintManager.findControl(L"lstDesk"));
	assert(plist && L"节点容器lstDesk不存在！");

	plist->redrawNodes();

	//// 
	//wui::textUI *ptxt = static_cast<wui::textUI *>(m_paintManager.findControl(kpTextTitle));
	//if (ptxt)
	//{
	//	SEND_LOG(L"redraw nodes:%s", ptxt->getText().c_str());
	//}
}

RECT kpdeskGroupWnd::getContainScreenPOS()
{
	gtc::deskListCtrl *plist = static_cast<gtc::deskListCtrl *>(m_paintManager.findControl(L"lstDesk"));
	assert(plist && L"节点容器lstDesk不存在！");
	RECT rt = plist->getPos();
	POINT pt;
	pt.x = rt.left;
	pt.y = rt.top;
	::ClientToScreen(getHWND(), &pt);
	rt.left = pt.x;
	rt.top = pt.y;
	pt.x = rt.right;
	pt.y = rt.bottom;
	::ClientToScreen(getHWND(), &pt);
	rt.right = pt.x;
	rt.bottom = pt.y;

	return rt;
}

bool kpdeskGroupWnd::getNodePos(const std::wstring &nodeName, RECT &rt)
{
	gtc::deskListCtrl *plist = static_cast<gtc::deskListCtrl *>(m_paintManager.findControl(L"lstDesk"));
	assert(plist && L"节点容器lstDesk不存在！");
	return plist->getNodePOS(nodeName, rt, true);
}