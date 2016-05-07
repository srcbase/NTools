#include "wucMenu.h"



namespace wuc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	menuWnd  类		2015-8-24
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
menuWnd::PtrArrayType	menuWnd::mc_vecWnd;
menuWnd::menuWnd()
	: m_pLayout(nullptr)
	, m_pOwner(nullptr)
{
}
menuWnd::~menuWnd()
{
	SEND_LOG(L"menuWnd free ...");
}
menuUC* menuWnd::getLayout() const
{
	assert(m_pLayout);
	return m_pLayout;
}
//void menuWnd::AddWnd(menuWnd *pWnd)
//{
//	assert(pWnd);
//	mc_vecWnd.push_back(pWnd);
//}
/*static*/ void menuWnd::closeAll()
{
	SEND_LOG(L"menuWnd::closeAll begin");
	for (PtrArrayType::reverse_iterator iter = mc_vecWnd.rbegin(); iter != mc_vecWnd.rend(); ++iter)
	{
		(*iter)->close();

	}
	mc_vecWnd.clear();
	SEND_LOG(L"menuWnd::closeAll end");
}
/*static*/ void menuWnd::closeWnd(HWND hWnd)
{
	menuWnd *ptmpWnd = NULL;
	for (PtrArrayType::iterator iter = mc_vecWnd.begin(); iter != mc_vecWnd.end(); )
	{
		ptmpWnd = *iter;
		HWND hParent = GetParent(ptmpWnd->m_hWnd);
		while (hParent != NULL)
		{
			if (hParent == hWnd)
			{
				iter = mc_vecWnd.erase(iter);
				ptmpWnd->close();
				ptmpWnd = NULL;
				break;
			}
			hParent = GetParent(hParent);
		}
		// 
		if(ptmpWnd)
			++iter;

	}
	//HWND hParent = GetParent(m_hWnd);
	//while (hParent != NULL)
	//{
	//	if (hParent == param.hWnd)
	//	{
	//		Close();
	//		break;
	//	}
	//	hParent = GetParent(hParent);
	//}
}

void menuWnd::init(HWND hParent, menuUC *pLayout)
{
	assert(hParent);
	assert(pLayout);
	assert(!m_pOwner);
	m_pLayout = pLayout;
	RECT rc = {0};

	mc_vecWnd.push_back(this);

	create(hParent, rc);
	HWND hWndParent = m_hWnd;
	while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
	::ShowWindow(m_hWnd, SW_SHOW);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

void menuWnd::init(menuElementUC *pMenuElem)
{
	assert(!m_pLayout);
	assert(!m_pOwner);
	m_pOwner = pMenuElem;

	RECT rc = {0};

	mc_vecWnd.push_back(this);

	create(m_pOwner->getManager()->getPaintWindow(), rc);
	HWND hWndParent = m_hWnd;
	while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
	::ShowWindow(m_hWnd, SW_SHOW);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

LRESULT menuWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
	{
		if (!m_pOwner)
		{	// 第一个菜单
			assert(m_pLayout);
			//m_pLayout->setBkColor(0xFFFF99FF);
			m_drawManager.init(m_hWnd);
			m_drawManager.setBackgroundTransparent(true);
			m_drawManager.attachDialog(m_pLayout);
			// 窗口尺寸
			//MONITORINFO monitor = {}; 
			//monitor.cbSize = sizeof(monitor);
			//::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &monitor);
			//const RECT rcWork = monitor.rcWork;
			const RECT rcWork = {0, 0, GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN)};
			SIZE szWnd = {rectWidth(rcWork), rectHeight(rcWork)};
			szWnd = m_pLayout->estimateSize(szWnd);
			m_drawManager.setInitSize(szWnd.cx, szWnd.cy);

			POINT pt = m_pLayout->getBasePoint();
			RECT rcWnd = {pt.x, pt.y, pt.x + szWnd.cx, pt.y + szWnd.cy};
			DWORD align = m_pLayout->getPopAlign();

			if (align & BEMenuAlignRight)
			{
				rcWnd.right = pt.x;
				rcWnd.left = rcWnd.right - szWnd.cx;
			}
			if (align & BEMenuAlignBottom)
			{
				rcWnd.bottom = pt.y;
				rcWnd.top = rcWnd.bottom - szWnd.cy;
			}
			// 保证在桌面内
			if(rcWnd.left < rcWork.left) ::OffsetRect(&rcWnd, rcWork.left - rcWnd.left, 0);
			if(rcWnd.top < rcWork.top) ::OffsetRect(&rcWnd, 0, rcWork.top - rcWnd.top);
			if(rcWnd.right > rcWork.right) ::OffsetRect(&rcWnd, rcWork.right - rcWnd.right, 0);
			if(rcWnd.bottom > rcWork.bottom) ::OffsetRect(&rcWnd, 0, rcWork.bottom - rcWnd.bottom);
			SetForegroundWindow(m_hWnd);
			MoveWindow(m_hWnd, rcWnd.left, rcWnd.top, rectWidth(rcWnd), rectHeight(rcWnd), FALSE);
			SetWindowPos(m_hWnd, HWND_TOPMOST, rcWnd.left, rcWnd.top, rectWidth(rcWnd), rectHeight(rcWnd), SWP_SHOWWINDOW);
			//m_pLayout->needUpdate();
		}
		else
		{
			assert(!m_pLayout);
			m_drawManager.init(m_hWnd);
			m_drawManager.setBackgroundTransparent(true);
			m_pLayout = new menuUC();
			assert(m_pLayout);
			m_drawManager.attachDialog(m_pLayout);
			m_pLayout->setBkColor(0xFEFFCC66);
			m_pLayout->applyAttributeList(L"Menu");
			//m_pLayout->setBorderSize(0);
			m_pLayout->setAutoDestroy(false);
			m_pLayout->enableScrollBar();
			for( int i = 0; i < m_pOwner->getCount(); i++ ) {
				menuElementUC *pelem = static_cast<menuElementUC *>(m_pOwner->getItemAt(i)->getInterface(L"MenuElement"));
				if (pelem)
				{
					//pelem->setOwner(m_pLayout);
					m_pLayout->add(pelem);
				}
			}

			// 窗口尺寸
			//MONITORINFO monitor = {}; 
			//monitor.cbSize = sizeof(monitor);
			//::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &monitor);
			//RECT rcWork = monitor.rcWork;
			const RECT rcWork = {0, 0, GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN)};
			SIZE szWnd = {rectWidth(rcWork), rectHeight(rcWork)};
			szWnd = m_pLayout->estimateSize(szWnd);
			//m_drawManager.setInitSize(szWnd.cx, szWnd.cy);

			RECT rcOwner = m_pOwner->getPosition();
			RECT rc = rcOwner;
			RECT rcWindow;
			GetWindowRect(m_pOwner->getManager()->getPaintWindow(), &rcWindow);

			rc.top = rcOwner.top;
			rc.bottom = rc.top + szWnd.cy;
			::MapWindowRect(m_pOwner->getManager()->getPaintWindow(), HWND_DESKTOP, &rc);
			rc.left = rcWindow.right;
			rc.right = rc.left + szWnd.cx;
			rc.right += 2;

			bool bReachBottom = false;
			bool bReachRight = false;
			LONG chRightAlgin = 0;
			LONG chBottomAlgin = 0;
			RECT rcPreWindow = {0};
			for (PtrArrayType::iterator iter = mc_vecWnd.begin(); iter != mc_vecWnd.end(); ++iter)
			{
				GetWindowRect((*iter)->getHWND(), &rcPreWindow);

				bReachRight = rcPreWindow.left >= rcWindow.right;
				bReachBottom = rcPreWindow.top >= rcWindow.bottom;
				if( (*iter)->getHWND() == m_pOwner->getManager()->getPaintWindow() 
					||  bReachBottom || bReachRight )
					break;

			}
			if (bReachBottom)
			{
				rc.bottom = rcWindow.top;
				rc.top = rc.bottom - szWnd.cy;
			}

			if (bReachRight)
			{
				rc.right = rcWindow.left;
				rc.left = rc.right - szWnd.cx;
			}

			if( rc.bottom > rcWork.bottom )
			{
				rc.bottom = rc.top;
				rc.top = rc.bottom - szWnd.cy;
			}

			if (rc.right > rcWork.right)
			{
				rc.right = rcWindow.left;
				rc.left = rc.right - szWnd.cx;

				rc.top = rcWindow.bottom;
				rc.bottom = rc.top + szWnd.cy;
			}

			if( rc.top < rcWork.top )
			{
				rc.top = rcOwner.top;
				rc.bottom = rc.top + szWnd.cy;
			}

			if (rc.left < rcWork.left)
			{
				rc.left = rcWindow.right;
				rc.right = rc.left + szWnd.cx;
			}

			MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
			//::MapWindowRect(m_pOwner->getManager()->getPaintWindow(), HWND_DESKTOP, &rcWnd);
			//MoveWindow(m_hWnd, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, FALSE);
		}
		return 0;
	}
	else if( uMsg == WM_CLOSE ) {
		if( m_pOwner != NULL )
		{
			m_pOwner->setManager(m_pOwner->getManager(), m_pOwner->getParent(), false);
			m_pOwner->setPosition(m_pOwner->getPosition());
			m_pOwner->setFocus();
		}
	}
	else if( uMsg == WM_RBUTTONDOWN || uMsg == WM_CONTEXTMENU || uMsg == WM_RBUTTONUP || uMsg == WM_RBUTTONDBLCLK )
	{
		return 0L;
	}
	else if (uMsg == WM_SETFOCUS)
	{
		HWND hFocusWnd = (HWND)wParam;
		hFocusWnd = NULL;
	}
	else if( uMsg == WM_KILLFOCUS )
	{
		HWND hFocusWnd = (HWND)wParam;
		//if(hFocusWnd) 
		{
			bool bInMenuWindowList = false;

			for (PtrArrayType::iterator iter = mc_vecWnd.begin(); iter != mc_vecWnd.end(); ++iter)
			{
				if (*(*iter) == hFocusWnd)
				{
					bInMenuWindowList = true;
					break;
				}
			}
														//ContextMenuParam param;
		//param.hWnd = GetHWND();

		//ContextMenuObserver::Iterator<BOOL, ContextMenuParam> iterator(s_context_menu_observer);
		//ReceiverImplBase<BOOL, ContextMenuParam>* pReceiver = iterator.next();
		//while( pReceiver != NULL ) {
		//	CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
		//	if( pContextMenu != NULL && pContextMenu->GetHWND() ==  hFocusWnd ) {
		//		bInMenuWindowList = TRUE;
		//		break;
		//	}
		//	pReceiver = iterator.next();
		//}

			if( !bInMenuWindowList ) {
				closeAll();
				return 0;
			}
		}
	}
	else if( uMsg == WM_KEYDOWN)
	{
		if( wParam == VK_ESCAPE)
		{
			close();
		}
	}
	if(!wuc::windowWnd::isOutMsg(uMsg))
		SEND_LOG(L"menuWnd::handleMessage msg:%s", wuc::windowWnd::stringMsg(uMsg).c_str() );

	LRESULT lres = 0;
	if(m_drawManager.messageHandler(uMsg, wParam, lParam, lres)) return lres;
	return windowWnd::handleMessage(uMsg, wParam, lParam);
}
void menuWnd::onFinalMessage(HWND hWnd)
{
	PtrArrayType::iterator iter = std::find(mc_vecWnd.begin(), mc_vecWnd.end(), this);
	if(iter != mc_vecWnd.end()) mc_vecWnd.erase(iter);
	if( m_pOwner != NULL ) {
		for( int i = 0; i < m_pOwner->getCount(); i++ ) {
			menuElementUC *pelem = static_cast<menuElementUC *>(m_pOwner->getItemAt(i)->getInterface(L"MenuElement"));
			if (pelem)
			{
				pelem->setOwner(m_pOwner->getParent());
				pelem->setVisible(false);
				pelem->setInternVisible(false);
			}
		}
		m_pOwner->m_pWindow = NULL;
		m_pOwner->m_uiButtonState &= ~ UCSTATE_PUSHED;
		m_pOwner->invalidate();
	}
	delete this;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	menuElementUC  类		2015-8-24
//	菜单项
////////////////////////////////////////////////////////////////////////////////////////////////////////
menuElementUC::menuElementUC()
	: m_pWindow(nullptr)
	, m_uiCmdID(0)
	, m_buttonIcon(nullptr)
{
	m_szxyFixed.cy = 35;
	m_bMouseChildEnabled = true;

	setMouseChildEnabled(false);

}
menuElementUC::~menuElementUC()
{
	if(!m_wsGroupName.empty() && m_pManager) m_pManager->removeOptionGroup(m_wsGroupName.c_str(), this);
}

PCWSTR menuElementUC::getClass() const
{
	return L"menuElementUC";
}
LPVOID menuElementUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"MenuElement") == 0 ) return static_cast<menuElementUC*>(this);
	return __super::getInterface(pstrName);
}

void menuElementUC::paint(HDC hdc, const RECT &rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	drawItemBk(hdc, m_rcItem);
	//__super::drawItemBk(hdc, m_rcItem);
	drawItem(hdc, m_rcItem);
	READ_LOCK
	for (ControlArrayType::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		// 刷新非菜单项控件
		if(!(*iter)->getInterface(L"MenuElement"))
			(*iter)->paint(hdc, rcPaint);
	}
}

void menuElementUC::drawItem(HDC hdc, const RECT& rcItem)
{
	assert(m_pOwner);
	if(m_wsText.empty() || m_wsText == L"-") return;
	if(!m_pOwner) return;
	BSListItemInfo *pInfo = m_pOwner->getListInfo();
	DWORD clrText = pInfo->clrText;
	if(m_uiButtonState & UCSTATE_HOT)
		clrText = pInfo->clrHotText;
	else if(isSelected()) 
		clrText = pInfo->clrSelectedText;
	else if(!isEnabled()) 
		clrText = pInfo->clrDisabledText;

	RECT rcText = rcItem;
	rcText.left += pInfo->rcTextPadding.left;
	rcText.right -= pInfo->rcTextPadding.right;
	rcText.top += pInfo->rcTextPadding.top;
	rcText.bottom -= pInfo->rcTextPadding.bottom;
	if(m_pParent && m_pParent->getParent())
	{
		menuUC *pMenu = static_cast<menuUC *>(m_pParent->getParent());
		assert(pMenu);
		if (pMenu)
		{
			RECT rcPadding = m_pParent->getParent()->getPadding();
			RECT rcInset = pMenu->getInset();
			rcText.left += rcPadding.left + rcInset.left;
			rcText.right -= rcPadding.right + rcInset.right;
			rcText.top += rcPadding.top;
			rcText.bottom -= rcPadding.bottom;
		}
	}
	Gdiplus::Graphics grap(hdc);
	gtc::drawer::drawString(grap, m_wsText.c_str(), rcText, m_pManager->getFont(pInfo->iFont), DT_SINGLELINE | pInfo->uTextStyle, clrText);
	//if( m_sText.IsEmpty() ) return;

	//if( m_pOwner == NULL ) return;
	//TListInfoUI* pInfo = m_pOwner->GetListInfo();
	//DWORD iTextColor = pInfo->dwTextColor;
	//if( (m_uButtonState & UISTATE_HOT) != 0 ) {
	//	iTextColor = pInfo->dwHotTextColor;
	//}
	//if( IsSelected() ) {
	//	iTextColor = pInfo->dwSelectedTextColor;
	//}
	//if( !IsEnabled() ) {
	//	iTextColor = pInfo->dwDisabledTextColor;
	//}
	//int nLinks = 0;
	//RECT rcText = rcItem;
	//rcText.left += pInfo->rcTextPadding.left;
	//rcText.right -= pInfo->rcTextPadding.right;
	//rcText.top += pInfo->rcTextPadding.top;
	//rcText.bottom -= pInfo->rcTextPadding.bottom;

	//if( pInfo->bShowHtml )
	//	CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, iTextColor, \
	//	NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle);
	//else
	//	CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, iTextColor, \
	//	pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
}
void menuElementUC::drawItemBk(HDC hdc, const RECT& rcItem)
{
	//__super::drawItemBk(hdc, rcItem);
	//return ;
	if(!hdc) return;
	if( !isEnabled() ) m_uiButtonState |= UCSTATE_DISABLED;
	else m_uiButtonState &= ~ UCSTATE_DISABLED;

	if(!m_pOwner) return;
	menuUC *pMenu = nullptr;
	if(m_pParent && m_pParent->getParent()) 
		pMenu = static_cast<menuUC *>(m_pParent->getParent());
	assert(pMenu);
	if(!pMenu) return;
	BSListItemInfo *pInfo = m_pOwner->getListInfo();
	assert(pInfo);
	if(!pInfo) return;
	Gdiplus::Graphics grap(hdc);
	RECT rcSrc = {0}, rc = {0};
	const RECT rcPadding = m_pParent->getParent()->getPadding();
	const RECT rcInset = pMenu->getInset();
	bool bl  = false;
	RECT rcIcon = m_rcItem;		
	// 绘制背景
	DWORD clrBk = pInfo->clrBk;
	SEND_LOG(L"ERROR menuElementUC::drawItemBk state:%08X name:<%s>", m_uiButtonState, m_wsText.c_str());
	if( m_uiButtonState & UCSTATE_DISABLED ) {
		clrBk = pInfo->clrDisabledBk;
	}
	else if( m_uiButtonState & UCSTATE_PUSHED ) {
		clrBk = pInfo->clrSelectedBk;
	}
	else if( m_uiButtonState & UCSTATE_HOT ) {
		clrBk = pInfo->clrHotBk;
	}
	if ( clrBk != 0 ) {
		gtc::drawer::fillRectangle(grap, clrBk, m_rcItem);
		//CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
	}
	// 有子菜单时对应的右图标
	bl = false;
	READ_LOCK
	for (ControlArrayType::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		if((*iter)->getInterface(L"MenuElement"))
		{
			bl = true;
			break;
		}
	}
	if (bl)
	{
		SIZE szArrow = {4, 9};
		rcIcon = m_rcItem;
		rcIcon.left = rcIcon.right - rcPadding.right;
		::OffsetRect(&rcIcon, -rcInset.right, 0);
		::InflateRect(&rcIcon, (szArrow.cx - rectWidth(rcIcon))/2, (szArrow.cy - rectHeight(rcIcon))/2);
		Gdiplus::Point pts[3] = {Gdiplus::Point(rcIcon.left, rcIcon.top), Gdiplus::Point(rcIcon.left, rcIcon.bottom), Gdiplus::Point(rcIcon.right, (rcIcon.top+rcIcon.bottom)/2)};
		grap.FillPolygon(&Gdiplus::SolidBrush(0xFE000000), pts, 3);
		//Gdiplus::GraphicsPath path;
		//path.AddLine(rcIcon.left, rc.top, rcIcon.left, rc.bottom);
		//path.AddLine(rcIcon.left, rc.top, rcIcon.right, rc.top);
		//path.AddLine(rcIcon.right, rc.bottom, rcIcon.left, rc.bottom);
		//path.AddLine(rcIcon.right, rc.bottom, rcIcon.right, rc.top);
		////path.AddLine(rcIcon.left, rc.bottom, rcIcon.right, (rcIcon.top + rcIcon.bottom)/2);
		////path.AddLine(rcIcon.right, (rcIcon.top + rcIcon.bottom)/2, rcIcon.left, rc.top);
		//grap.FillPath(&Gdiplus::SolidBrush(0xFFFF0000), &path);
		////gtc::drawer::fillRectangle(grap, 0xFFFFCC33, rcIcon);
	}
	if(!m_pImage) return;
	// 菜单项左图标
	rcIcon = m_rcItem;
	rcIcon.right = rcIcon.left + rcPadding.left + rcInset.left;
	if( (m_uiButtonState & UCSTATE_DISABLED) != 0 ) {
		if (isSelected())
		{
			bl = getDisabledImageSelected(rcSrc);
		}
		
		if(!bl) bl = getDisabledImage(rcSrc);
		
		if(bl)
		{
			rc = rcIcon;
			::InflateRect(&rc, -(rectWidth(rcIcon) - rectWidth(rcSrc))/2, -(rectHeight(rcIcon) - rectHeight(rcSrc))/2);
			if(rectHeight(rc) != rectHeight(rcSrc)) rc.bottom = rc.top + rectHeight(rcSrc);
			if(rectWidth(rc) != rectWidth(rcSrc)) rc.right = rc.left + rectWidth(rcSrc);
			bl = gtc::drawer::drawImage(grap, m_pImage, rc, rcSrc);
		}
		if(bl) return;
	}
	else if( (m_uiButtonState & UCSTATE_PUSHED) != 0 ) {
		if (isSelected())
		{
			bl = getPushedImageSelected(rcSrc);
		}
		if(!bl)	bl = getPushedImage(rcSrc);
		
		if(bl)
		{
			rc = rcIcon;
			::InflateRect(&rc, -(rectWidth(rcIcon) - rectWidth(rcSrc))/2, -(rectHeight(rcIcon) - rectHeight(rcSrc))/2);
			if(rectHeight(rc) != rectHeight(rcSrc)) rc.bottom = rc.top + rectHeight(rcSrc);
			if(rectWidth(rc) != rectWidth(rcSrc)) rc.right = rc.left + rectWidth(rcSrc);
			bl = gtc::drawer::drawImage(grap, m_pImage, rc, rcSrc);
		}
		if(bl) return;
	}
	else if( (m_uiButtonState & UCSTATE_HOT) != 0 ) {
		if (isSelected())
		{
			bl = getHotImageSelected(rcSrc);
		}
		if(!bl) bl = getHotImage(rcSrc);
		
		if(bl)
		{
			rc = rcIcon;
			::InflateRect(&rc, -(rectWidth(rcIcon) - rectWidth(rcSrc))/2, -(rectHeight(rcIcon) - rectHeight(rcSrc))/2);
			if(rectHeight(rc) != rectHeight(rcSrc)) rc.bottom = rc.top + rectHeight(rcSrc);
			if(rectWidth(rc) != rectWidth(rcSrc)) rc.right = rc.left + rectWidth(rcSrc);
			bl = gtc::drawer::drawImage(grap, m_pImage, rc, rcSrc);
		}
		if(bl) return;
	}

	if (isSelected())
	{
		bl = getNormalImageSelected(rcSrc);
	}
	if(!bl) bl = getNormalImage(rcSrc);

	if(bl)
	{
		rc = rcIcon;
		::InflateRect(&rc, -(rectWidth(rcIcon) - rectWidth(rcSrc))/2, -(rectHeight(rcIcon) - rectHeight(rcSrc))/2);
		if(rectHeight(rc) != rectHeight(rcSrc)) rc.bottom = rc.top + rectHeight(rcSrc);
		if(rectWidth(rc) != rectWidth(rcSrc)) rc.right = rc.left + rectWidth(rcSrc);
		bl = gtc::drawer::drawImage(grap, m_pImage, rc, rcSrc);
	}

	//SEND_LOG(L"ERROR menuElementUC::drawItemBk end");
}

bool menuElementUC::select(bool bSelect /*= true*/)
{
	__super::select(bSelect);
	
	assert(m_pManager);
	if (m_pManager)
	{
		if (!m_wsGroupName.empty())
		{
			if (m_bSelected)
			{
				ControlArrayType vecOption = m_pManager->getOptionGroup(m_wsGroupName.c_str());
				for (ControlArrayType::iterator iter = vecOption.begin(); iter != vecOption.end(); ++iter)
				{
					menuElementUC *pControl = static_cast<menuElementUC *>(*iter);
					assert(pControl);
					if(pControl && pControl != this) pControl->select(false);
				}
			}
		}
		//else
		//{
		//	m_pManager->sendNotify(this, L"selectchanged");
		//}
	}
	invalidate();

	return true;
}
void menuElementUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pOwner != NULL ) m_pOwner->Event(event);
		else __super::Event(event);
		return;
	}
	if (event.Type == BEUCEventMouseEnter)
	{
		__super::Event(event);
		if( m_pWindow ) return;
		bool hasSubMenu = false;
		for( int i = 0; i < getCount(); ++i )
		{
			menuElementUC *pelem = static_cast<menuElementUC *>(getItemAt(i)->getInterface(L"MenuElement"));
			if (pelem)
			{
				pelem->setVisible(true);
				pelem->setInternVisible(true);
				hasSubMenu = true;
			}

		}
		if( hasSubMenu )
		{
			m_pOwner->selectItem(getIndex());
			createMenuWnd();
		}
		else
		{
			menuWnd::closeWnd(m_pManager->getPaintWindow());
			//ContextMenuParam param;
			//param.hWnd = m_pManager->GetPaintWindow();
			//param.wParam = 2;
			//s_context_menu_observer.RBroadcast(param);
			m_pOwner->selectItem(getIndex());
		}
		return;
	}
	else if (event.Type == BEUCEventButtonDown)
	{
		if( ::PtInRect(&m_rcItem, event.ptMouse) && isEnabled() ) {
			m_uiButtonState |= UCSTATE_PUSHED | UCSTATE_CAPTURED;
			invalidate();
		}
		return;
	}
	else if (event.Type == BEUCEventButtonUp)
	{
		if( isEnabled() ){
			__super::Event(event);
			if( m_pWindow ) return;

			bool hasSubMenu = false;
			for( int i = 0; i < getCount(); ++i ) {
				menuElementUC *pelem = static_cast<menuElementUC *>(getItemAt(i)->getInterface(L"MenuElement"));
				if (pelem)
				{
					pelem->setVisible(true);
					pelem->setInternVisible(true);
					hasSubMenu = true;
				}
			}
			if( hasSubMenu )
			{
				createMenuWnd();
			}
			else
			{
				if (m_uiCmdID > 0 && m_hwndMsg)
				{
					::PostMessage(m_hwndMsg, menuUC::getSelfRegMsg(), m_uiCmdID, NULL);
				}
				//if(OnMenuCommand && m_uiCmdID > 0)
				//{
				//	m_pManager->reapObject(this);	// 在drawManager内部WM_CLOSE消息时会执行BEUCEventButtonUp消息
				//									// 所以先置drawManager::m_pEventClick = NULL
				//	OnMenuCommand(m_uiCmdID, m_uiButtonState);
				//}
				menuWnd::closeAll();
			}
		}
		return;
	}
	return __super::Event(event);
}
bool menuElementUC::activate()
{
	if (__super::activate() && m_bSelected)
	{
		if( m_pWindow ) return true;
		bool hasSubMenu = false;
		for (int i = 0; i < getCount(); ++i)
		{
			menuElementUC *pelem = static_cast<menuElementUC *>(getItemAt(i)->getInterface(L"MenuElement"));
			if (pelem)
			{
				pelem->setVisible(true);
				pelem->setInternVisible(true);
				hasSubMenu = true;
			}
		}
		if (hasSubMenu)
		{
			createMenuWnd();
		}
		else
		{
			menuWnd::closeAll();
		}

		return true;
	}
	return false;
}
SIZE menuElementUC::estimateSize(SIZE szAvailable)
{
	SIZE szFixed = {0};
	READ_LOCK
	for (ControlArrayType::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		if(!(*iter)->isVisible()) continue;
		SIZE sz = (*iter)->estimateSize(szAvailable);
		szFixed.cy += sz.cy;
		if(szFixed.cx < sz.cx) szFixed.cx = sz.cx;
	}
	if (szFixed.cy == 0 || szFixed.cx == 0)
	{	// 计算文本区
		BSListItemInfo *pInfo = m_pOwner->getListInfo();
		HDC hdc = GetDC(NULL);
		Gdiplus::Graphics grap(hdc);
		RECT rc = {0,0,0,0};
		std::wstring wstr = m_wsText + L"汉";
		gtc::rect rcms = gtc::drawer::measureString(grap, wstr.c_str(), rc, m_pManager->getFont(pInfo->iFont), pInfo->uTextStyle|DT_SINGLELINE);
		::ReleaseDC(NULL, hdc);
		if (szFixed.cx == 0)
		{
			szFixed.cx = rcms.width + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;
		}
		if (szFixed.cy == 0)
		{
			szFixed.cy = rcms.height + pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}
	}
	if(!m_wsText.empty() && m_wsText != L"-")
		if(m_szxyFixed.cy != 0) szFixed.cy = m_szxyFixed.cy;
	return szFixed;
}

void menuElementUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = NULL;
	if( _tcscmp(pstrName, L"selected") == 0 ) select(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"group") == 0 ) setGroup(pstrValue);
	else if( _tcscmp(pstrName, L"normalimage") == 0 ) setNormalImage(gtc::format::rect(pstrValue));
	else if( _tcscmp(pstrName, L"hotimage") == 0 ) setHotImage(gtc::format::rect(pstrValue));
	else if( _tcscmp(pstrName, L"pushedimage") == 0 ) setPushedImage(gtc::format::rect(pstrValue));
	else if( _tcscmp(pstrName, L"disabledimage") == 0 ) setDisabledImage(gtc::format::rect(pstrValue));
	else if( _tcscmp(pstrName, L"normalimageselected") == 0 ) setNormalImageSelected(gtc::format::rect(pstrValue));
	else if( _tcscmp(pstrName, L"hotimageselected") == 0 ) setHotImageSelected(gtc::format::rect(pstrValue));
	else if( _tcscmp(pstrName, L"pushedimageselected") == 0 ) setPushedImageSelected(gtc::format::rect(pstrValue));
	else if( _tcscmp(pstrName, L"disabledimageselected") == 0 ) setDisabledImageSelected(gtc::format::rect(pstrValue));

	else __super::setAttribute(pstrName, pstrValue);


}
buttonUC* menuElementUC::createIconButton(PCWSTR pAttr /*= NULL*/)
{
	assert(!m_buttonIcon);
	if(m_buttonIcon) return m_buttonIcon;
	m_buttonIcon = new buttonUC();
	if(pAttr) m_buttonIcon->applyAttributeList(pAttr);

	return m_buttonIcon;
}
void menuElementUC::setCmdID(UINT cmdID)
{
	m_uiCmdID = cmdID;
}
UINT menuElementUC::getCmdID() const
{
	return m_uiCmdID;
}
void menuElementUC::setMsgWnd(HWND hWnd)
{
	assert(hWnd);
	m_hwndMsg = hWnd;
}
HWND menuElementUC::getMsgWnd() const
{
	return m_hwndMsg;
}
std::wstring menuElementUC::getGroup() const
{
	return m_wsGroupName;
}
void menuElementUC::setGroup(PCWSTR pstrGroupName /*= NULL*/)
{
	if (!pstrGroupName)
	{
		if(m_wsGroupName.empty()) return;
		m_wsGroupName.clear();
	}
	else
	{
		if(m_wsGroupName == pstrGroupName) return;
		if(!m_wsGroupName.empty() && m_pManager) m_pManager->removeOptionGroup(m_wsGroupName.c_str(), this);
		m_wsGroupName = pstrGroupName;
	}
	if (!m_wsGroupName.empty())
	{
		if(m_pManager) m_pManager->addOptionGroup(m_wsGroupName.c_str(), this);
	}
	else
	{
		if(m_pManager) m_pManager->removeOptionGroup(m_wsGroupName.c_str(), this);
	}
	select(m_bSelected);
}

void menuElementUC::createMenuWnd()
{
	if( m_pWindow ) return;

	m_pWindow = new menuWnd();
	assert(m_pWindow);

	menuWnd::closeWnd(m_pManager->getPaintWindow());

	m_pWindow->init(this);
}


bool menuElementUC::getNormalImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_NORMAL, &rc);
}

void menuElementUC::setNormalImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_NORMAL, rcImgCell);
	invalidate();
}
void menuElementUC::setNormalImage(LONG left, LONG top, LONG right, LONG bottom)
{
	assert(m_pImage);
	RECT rc = {left, top, right, bottom};
	setNormalImage(rc);
}
bool menuElementUC::getHotImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_HOT, &rc);
}

void menuElementUC::setHotImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_HOT, rcImgCell);
	invalidate();
}

void menuElementUC::setHotImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setHotImage(rc);
}
bool menuElementUC::getPushedImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_PUSHED, &rc);
}

void menuElementUC::setPushedImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_PUSHED, rcImgCell);
	invalidate();
}

void menuElementUC::setPushedImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setPushedImage(rc);
}

bool menuElementUC::getDisabledImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_DISABLED, &rc);
}

void menuElementUC::setDisabledImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_DISABLED, rcImgCell);
	invalidate();
}
void menuElementUC::setDisabledImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setDisabledImage(rc);
}

bool menuElementUC::getNormalImageSelected(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_NORMAL|UCIMAGE_STATE_SELECTED, &rc);
}

void menuElementUC::setNormalImageSelected(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_NORMAL|UCIMAGE_STATE_SELECTED, rcImgCell);
	invalidate();
}
void menuElementUC::setNormalImageSelected(LONG left, LONG top, LONG right, LONG bottom)
{
	assert(m_pImage);
	RECT rc = {left, top, right, bottom};
	setNormalImageSelected(rc);
}
bool menuElementUC::getHotImageSelected(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_HOT|UCIMAGE_STATE_SELECTED, &rc);
}

void menuElementUC::setHotImageSelected(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_HOT|UCIMAGE_STATE_SELECTED, rcImgCell);
	invalidate();
}

void menuElementUC::setHotImageSelected(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setHotImageSelected(rc);
}
bool menuElementUC::getPushedImageSelected(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_PUSHED|UCIMAGE_STATE_SELECTED, &rc);
}

void menuElementUC::setPushedImageSelected(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_PUSHED|UCIMAGE_STATE_SELECTED, rcImgCell);
	invalidate();
}

void menuElementUC::setPushedImageSelected(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setPushedImageSelected(rc);
}

bool menuElementUC::getDisabledImageSelected(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_DISABLED|UCIMAGE_STATE_SELECTED, &rc);
}

void menuElementUC::setDisabledImageSelected(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_DISABLED|UCIMAGE_STATE_SELECTED, rcImgCell);
	invalidate();
}
void menuElementUC::setDisabledImageSelected(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setDisabledImageSelected(rc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	menuUC  类		2015-8-24
//	菜单封装类
////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT menuUC::mc_ucMsgMenu	= RegisterWindowMessageW(L"wuc_menu");	// 注册消息，wuc菜单发送(post)的命令消息

menuUC::menuUC()
	: m_iDefaultItemHeight(24)
	, m_dwAlign(BEMenuAlignLeft|BEMenuAlignTop)
	, m_clrSeparator(0xFEF7F7F7)
	, m_hwndMsg(NULL)
{
	::ZeroMemory(&m_ptBase, sizeof(m_ptBase));

	setInset(2,2,2,2);			// 菜单对话框留边
	setPadding(32, 0, 16, 0);	// 菜单项左右图标显示区域
	if(getHeader()) getHeader()->setVisible(false);
	setItemTextStyle(DT_LEFT|DT_SINGLELINE|DT_VCENTER);
}
menuUC::~menuUC()
{
	SEND_LOG(L"menuUC free ...");
}

PCWSTR menuUC::getClass() const
{
	return L"menuUC";
}
LPVOID menuUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Menu") == 0 ) return static_cast<menuUC*>(this);
	return __super::getInterface(pstrName);
}
int menuUC::getItemIndex(controlUC* pControl) const
{
	menuElementUC *pmenuitem = static_cast<menuElementUC *>(pControl);
	assert(pmenuitem);
	if(!pmenuitem) return -1;
	return __super::getItemIndex(pControl);
}
bool menuUC::setItemIndex(controlUC* pControl, int iIndex)
{
	menuElementUC *pmenuitem = static_cast<menuElementUC *>(pControl);
	assert(pmenuitem);
	if(!pmenuitem) return false;
	return __super::setItemIndex(pControl, iIndex);
}
bool menuUC::add(controlUC* pControl)
{
	menuElementUC *pmenuitem = static_cast<menuElementUC *>(pControl);
	assert(pmenuitem);
	if(!pmenuitem) return false;

	for (int idx = 0; idx < pmenuitem->getCount(); ++idx)
	{
		menuElementUC *pMe = static_cast<menuElementUC *>(pmenuitem->getItemAt(idx)->getInterface(L"MenuElement"));
		if (pMe) pMe->setInternVisible(false);
	}
	pmenuitem->setInternVisible(true);
	return __super::add(pControl);
}
bool menuUC::addAt(controlUC* pControl, int iIndex)
{
	menuElementUC *pmenuitem = static_cast<menuElementUC *>(pControl);
	assert(pmenuitem);
	if(!pmenuitem) return false;

	for (int idx = 0; idx < pmenuitem->getCount(); ++idx)
	{
		menuElementUC *pMe = static_cast<menuElementUC *>(pmenuitem->getItemAt(idx)->getInterface(L"MenuElement"));
		if (pMe) pMe->setInternVisible(false);
	}
	pmenuitem->setInternVisible(true);
	return __super::addAt(pControl, iIndex);
}
bool menuUC::remove(controlUC* pControl)
{
	menuElementUC *pmenuitem = static_cast<menuElementUC *>(pControl);
	assert(pmenuitem);
	if(!pmenuitem) return false;

	return __super::remove(pControl);
}
void menuUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"defaultitemheight") == 0 ) setDefaultItemHeight(std::wcstol(pstrValue, NULL, 10));
	else if (_tcscmp(pstrName, L"separatorcolor") == 0) setSeparatorColor(gtc::format::color(pstrValue));
	else __super::setAttribute(pstrName, pstrValue);
}
SIZE menuUC::estimateSize(SIZE szAvailable)
{
	//int cxFixed = 0;
	//int cyFixed = 0;
	//for( int it = 0; it < GetCount(); it++ ) {
	//	CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
	//	if( !pControl->IsVisible() ) continue;
	//	SIZE sz = pControl->EstimateSize(szAvailable);
	//	cyFixed += sz.cy;
	//	if( cxFixed < sz.cx )
	//		cxFixed = sz.cx;
	//}
	SIZE szFixed = {0};
	for (int idx = 0; idx < getCount(); ++idx)
	{
		controlUC *pControl = getItemAt(idx);
		assert(pControl);
		if(!pControl->isVisible()) continue;
		SIZE sz = pControl->estimateSize(szAvailable);
		szFixed.cy += sz.cy;
		if(szFixed.cx < sz.cx) szFixed.cx = sz.cx;

	}
	szFixed.cy += m_rcInset.top + m_rcInset.bottom;
	szFixed.cx += m_rcInset.left + m_rcInset.right;
	szFixed.cx += m_rcPadding.left + m_rcPadding.right;
	return szFixed;
}
void menuUC::setDefaultItemHeight(int height)
{
	m_iDefaultItemHeight = height;
}
int menuUC::getDefaultItemHeight() const
{
	return m_iDefaultItemHeight;
}
void menuUC::setSeparatorColor(DWORD dwClr)
{
	m_clrSeparator = dwClr;
}
DWORD menuUC::getSeparatorColor() const
{
	return m_clrSeparator;
}
POINT menuUC::getBasePoint() const
{
	return m_ptBase;
}
DWORD menuUC::getPopAlign() const
{
	return m_dwAlign;
}
bool menuUC::trackPopupMenu(HWND hParent, LONG x, LONG y, DWORD dwFlag /*= BEMenuAlignLeft|BEMenuAlignTop*/)
{
	assert(hParent);
	assert(getCount() > 0);
	m_ptBase.x = x;
	m_ptBase.y = y;
	m_dwAlign = dwFlag;

	menuWnd *pWnd = new menuWnd();
	pWnd->init(hParent, this);
	return pWnd != NULL;
}
menuElementUC* menuUC::addSeparator()
{
	menuElementUC *pMenu = createSeparator();
	if(pMenu) add(pMenu);

	return pMenu;
}
// 普通菜单，其可能带有三态图标的普通按钮
menuElementUC* menuUC::addMenu(UINT cmdID, PCWSTR pText, PCWSTR pbtnAttr /*= NULL*/)
{
	menuElementUC *pMenu = createMenu(cmdID, pText, pbtnAttr);
	if(pMenu) add(pMenu);

	return pMenu;
}
menuElementUC* menuUC::addMenu(PCWSTR pText, menuElementUC::PtrArrayType &vecSubMenu)
{
	menuElementUC *pMenu = createMenu(pText, vecSubMenu);
	if(pMenu) add(pMenu);

	return pMenu;
}
menuElementUC* menuUC::createSeparator()
{
	const int lineH = 1;
	menuElementUC *pSeparator = new menuElementUC();
	horizontalLayoutUC *phor = new horizontalLayoutUC();
	phor->setFixedHeight(lineH);
	// 左留白
	verticalLayoutUC *pver = new verticalLayoutUC();
	pver->setFixedWidth(m_rcInset.left + m_rcPadding.left);
	phor->add(pver);
	// 分隔线
	pver = new verticalLayoutUC();
	pver->setBkColor(m_clrSeparator);
	phor->add(pver);
	// 右留白
	pver = new verticalLayoutUC();
	pver->setFixedWidth(m_rcInset.right);
	phor->add(pver);
	pSeparator->add(phor);

	pSeparator->setInternVisible(false);
	return pSeparator;
}
menuElementUC* menuUC::createMenu(UINT cmdID, PCWSTR pText, PCWSTR pAttrList /*= NULL*/)
{
	assert(cmdID > 0);
	assert(pText);
	if(cmdID == 0 || !pText) return nullptr;

	menuElementUC *pMenu = new menuElementUC();
	pMenu->setText(pText);
	pMenu->setCmdID(cmdID);
	pMenu->setMsgWnd(m_hwndMsg);
	if(pAttrList) pMenu->applyAttributeList(pAttrList);
	pMenu->setFixedHeight(m_iDefaultItemHeight);
	//if(OnMenuCommand) pMenu->OnMenuCommand = OnMenuCommand;


	pMenu->setInternVisible(false);
	return pMenu;
}

menuElementUC* menuUC::createMenu(PCWSTR pText, menuElementUC::PtrArrayType &vecSubMenu)
{
	assert(vecSubMenu.size() > 0);
	assert(pText);
	if(!pText || vecSubMenu.size() == 0) return nullptr;

	menuElementUC *pMenu = new menuElementUC();
	pMenu->setText(pText);
	horizontalLayoutUC *phor = new horizontalLayoutUC();
	phor->setFixedHeight(m_iDefaultItemHeight);
	// 左留白
	verticalLayoutUC *pver = new verticalLayoutUC();
	pver->setFixedWidth(m_rcInset.left + m_rcPadding.left);
	phor->add(pver);
	// 文本
	pver = new verticalLayoutUC();
	phor->add(pver);
	//labelUC *pLabel = new labelUC();
	//pLabel->setTextPadding(m_bsInfo.rcTextPadding);
	//pLabel->setText(pText);
	//pLabel->setFont(m_bsInfo.iFont);
	//pver->add(pLabel);
	// 右留白
	pver = new verticalLayoutUC();
	pver->setFixedWidth(m_rcInset.right);
	phor->add(pver);
	pMenu->add(phor);
	// 子菜单
	for (menuElementUC::PtrArrayType::iterator iter = vecSubMenu.begin(); iter != vecSubMenu.end(); ++iter)
	{
		(*iter)->setOwner(this);
		pMenu->add(*iter);
	}
	// 
	pMenu->setInternVisible(false);
	return pMenu;
}


std::wstring menuUC::getNormalAttributeList(PCWSTR pImage, RECT *pNormal
	, RECT *pHot /*= NULL*/, RECT *pPushed /*= NULL*/, RECT *pDisabled /*= NULL*/)
{
	std::wstring wsAttr;
	wchar_t pbuf[MAX_PATH] = {0};
	assert(pImage && std::wcslen(pImage) > 0);
	assert(pNormal);
	std::swprintf(pbuf, _countof(pbuf), L"image=\"%s\" normalimage=\"%s\"", pImage, gtc::format::rect(*pNormal).c_str());
	wsAttr += pbuf;

	if(pHot)
	{
		std::swprintf(pbuf, _countof(pbuf), L" hotimage=\"%s\"", gtc::format::rect(*pHot).c_str());
		wsAttr += pbuf;
	}
	if(pPushed)
	{
		std::swprintf(pbuf, _countof(pbuf), L" pushedimage=\"%s\"", gtc::format::rect(*pPushed).c_str());
		wsAttr += pbuf;
	}
	if(pDisabled)
	{
		std::swprintf(pbuf, _countof(pbuf), L" disabledimage=\"%s\"", gtc::format::rect(*pDisabled).c_str());
		wsAttr += pbuf;
	}

	return wsAttr;
}
void menuUC::registerMessageWnd(HWND hWnd)
{
	assert(hWnd);
	m_hwndMsg = hWnd;
}
HWND menuUC::getMessageWnd() const
{
	assert(m_hwndMsg);
	return m_hwndMsg;
}
/*static*/ UINT menuUC::getSelfRegMsg()
{
	return mc_ucMsgMenu;
}

}	// wuc