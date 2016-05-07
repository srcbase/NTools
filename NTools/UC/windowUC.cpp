#include "windowUC.h"
#include "AC/theme.h"
//#include "KPDeskDef.h"
namespace wuc {

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////	windowTitleUC类	2015-1-22
////	windowUC窗口的标题区，内可放置最大、最小、关闭等按钮
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//windowTitleUC::windowTitleUC()
//{
//
//}
//windowTitleUC::~windowTitleUC()
//{
//
//}
//PCWSTR windowTitleUC::getClass() const
//{
//	return L"windowTitleUC";
//}
//LPVOID windowTitleUC::getInterface(PCWSTR pstrName)
//{
//	if( _tcscmp(pstrName, L"WindowTitle") == 0 ) return static_cast<windowTitleUC*>(this);
//	return __super::getInterface(pstrName);
//}
//
//int windowTitleUC::getHeight() const
//{
//
//}
//void windowTitleUC::setHeight(int height)
//{
//	
//}
//PCWSTR windowTitleUC::getTitle() const
//{
//
//}
//void windowTitleUC::setTitle(std::wstring wsTitle)
//{
//
//}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	windowUC类	2015-1-22
//	模拟的窗口基类
//	窗口功能区域共有2部分：标题（可以隐藏），客户区
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
static bool nc_bMouseEntry = false;		// 每个控件都能收到MouseEntry消息，我们只需执行其中一个
static bool nc_bMouseLeave	= true;

windowUC::windowUC(void)
	: m_bInitial(true)
	, m_bActivated(false)
	, m_bLockPos(false)
	, m_iOperateHandle(HTNOWHERE)
	, m_beActive(BEUCActiveFlagNone)
	, m_beSelectMode(BESelectModeNone)
	, m_bSelected(false)
{
	memset(&m_lastMousePos, 0, sizeof(m_lastMousePos));
	m_szxyBorderRound.cx = m_szxyBorderRound.cy = 6;
	m_rcSizeBox.left = m_rcSizeBox.right = m_rcSizeBox.bottom = m_rcSizeBox.top = 8;
	// 标题栏
	m_pTitleBar = new horizontalLayoutUC();
	m_pTitleBar->setName(L"windowTitle");
	//m_pTitleBar->setBkColor(0xff00ff00);
	m_pTitleBar->setAttribute(L"inset", L"2,1,2,1");		// m_szxyBorderRound.cx/2, m_szxyBorderRound.cy/2
	// 客户区
	//m_pTitleBar->setAttribute(L"padding", L"6,6,6,6");
	//m_pTitleBar->setAttribute(L"height", L"50");
	//m_pTitleBar->setFixedHeight(50);
	m_pClientUC = new horizontalLayoutUC();
	m_pClientUC->setName(L"windowClient");
	m_pClientUC->setAttribute(L"inset", L"3,3,2,3");		// m_szxyBorderRound.cx/2, m_szxyBorderRound.cy/2
	//m_pClientUC->setBkColor(0xffff0000);
	//m_pClientArea->setAttribute(L"padding", L"6,6,6,6");
	// 任务栏
	m_pTaskBar = new horizontalLayoutUC();
	m_pTaskBar->setName(L"windowTask");
	m_pTaskBar->setAttribute(L"inset", L"2,1,2,1");

	add(m_pTitleBar);
	add(m_pClientUC);
	add(m_pTaskBar);
}

windowUC::~windowUC(void)
{
	SEND_LOG(L"windowUC<%s> free ...", getTitle());
}

inline PCWSTR windowUC::getClass() const
{
	return L"windowUC";
}
LPVOID windowUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Window") == 0 ) return static_cast<windowUC*>(this);
	return __super::getInterface(pstrName);
}

void windowUC::setPosition(RECT pos)
{
	RECT rc = m_rcItem;
	::UnionRect(&rc, &m_rcItem, &pos);

	__super::setPosition(pos);
	if (m_pManager)
	{
		m_pManager->invalidate(rc);
	}
}

horizontalLayoutUC* windowUC::getTitleBar() const
{
	return m_pTitleBar;
}
horizontalLayoutUC* windowUC::getClientUC() const
{
	return m_pClientUC;
}
horizontalLayoutUC* windowUC::getTaskBar() const
{
	return m_pTaskBar;
}
void windowUC::setLockPos(bool lock)
{
	m_bLockPos = lock;
}
bool windowUC::isLockPos() const
{
	return m_bLockPos;
}
BESelectMode windowUC::getSelectMode() const
{
	return m_beSelectMode;
}
bool windowUC::isActivated() const
{
	return m_bActivated;
}

void windowUC::setActivated(bool activate)
{
	if(m_bActivated == activate) return;
	m_bActivated = activate;
	invalidate();
}

bool windowUC::isSelected() const
{
	return m_bSelected;
}
void windowUC::selected(bool bSelected /*= true*/)
{
	if(m_bSelected == bSelected) return;
	m_bSelected = bSelected;
	invalidate();
}
BEUCActiveFlag windowUC::getActiveFlag() const
{
	return m_beActive;
}
void windowUC::clearActiveFlag()
{
	m_beActive = BEUCActiveFlagNone;
}
bool windowUC::isInitial() const
{
	return m_bInitial;
}
int windowUC::getOperateHandle() const
{
	return m_iOperateHandle;
}
PCWSTR windowUC::getTitle() const
{
	return m_wsTitle.c_str();
}
void windowUC::setTitle(const std::wstring &wsTitle)
{
	m_wsTitle = wsTitle;
}
int windowUC::getTitlebarHeight() const
{
	assert(m_pTitleBar);
	if(!m_pTitleBar) return 0;
	if (m_pTitleBar->isVisible())
	{
		return m_pTitleBar->getFixedHeight();
	}
	return 0;
}
void windowUC::setTitlebarHeight(int height)
{
	assert(m_pTitleBar);
	if(!m_pTitleBar) return ;
	if (m_pTitleBar->getFixedHeight() == height) return;
	m_pTitleBar->setFixedHeight(height);
}
RECT windowUC::getTitlebarPos() const
{
	assert(m_pTitleBar);
	RECT rc = {0};
	if(!m_pTitleBar) return rc;
	//RECT rc = {0,0,getFixedWidth(), getTitlebarHeight()};
	//rc.left += m_pTitleBar->getInset().left;
	//rc.top += m_pTitleBar->getInset().top;
	//rc.right -= m_pTitleBar->getInset().right;
	//rc.bottom -= m_pTitleBar->getInset().bottom;
	//return rc;
	if(m_pTitleBar) rc = m_pTitleBar->getPosition();

	return rc;
}

int windowUC::getTaskbarHeight() const
{
	assert(m_pTaskBar);
	if(!m_pTaskBar) return 0;
	if (m_pTaskBar->isVisible())
	{
		return m_pTaskBar->getFixedHeight();
	}
	return 0;
}
void windowUC::setTaskbarHeight(int height)
{
	assert(m_pTaskBar);
	if(!m_pTaskBar) return;
	if (m_pTaskBar->getFixedHeight() == height) return;
	m_pTaskBar->setFixedHeight(height);
}
RECT windowUC::getTaskbarPos() const
{
	assert(m_pTaskBar);
	RECT rc = {0};
	if(!m_pTaskBar) return rc;
	if(m_pTaskBar) rc = m_pTaskBar->getPosition();

	return rc;
}

void windowUC::paintStatusImage(HDC hdc)
{
	if(!hdc || !isVisible() || !m_pImage) return;
	const RECT rcWnd = m_rcItem;
	const RECT rcTitle = m_pTitleBar->getPosition();
	const int titleHeight = rectHeight(rcTitle);
	RECT rc = {0}, rcSrc = {0}, rcImage = {0};
	Gdiplus::Graphics grap(hdc);
	//gtc::imageResource *m_pImage = m_bFocus ? m_pimgBkSelected : m_pimgBk;
	//SEND_LOG(L"windowUC::draw %s<%s> image:%08X", getTitle(), gtc::format::rect(rcWnd,true).c_str(), m_pImage);
	if (m_pImage)
	{
		const SIZE szImage = {m_pImage->getWidth(), m_pImage->getHeight()/2};
		rcImage.right = szImage.cx;
		rcImage.bottom = szImage.cy;
		if(m_bSelected) ::OffsetRect(&rcImage, 0, szImage.cy);
		gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcItem, rcImage);
		//// 窗口圆角
		//if (m_szxyBorderRound.cy > 0)
		//{
		//	// 左上
		//	rc = rcWnd;
		//	rc.bottom = rc.top + m_szxyBorderRound.cy;
		//	rc.right = rc.left + m_szxyBorderRound.cx;
		//	//rcSrc.left = rcSrc.top = 0;
		//	rcSrc = rcImage;
		//	rcSrc.right = m_szxyBorderRound.cx;
		//	rcSrc.bottom = rcSrc.top + m_szxyBorderRound.cy;
		//	gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);
		//	//SEND_LOG(L"windowUC::draw 左上 image:<%d,%d> dest:<%s> src:<%s>", szImage.cx, szImage.cy, gtc::format::rect(rc, true).c_str(), gtc::format::rect(rcSrc, true).c_str());
		//	// 右上
		//	::OffsetRect(&rc, rectWidth(rcWnd)-m_szxyBorderRound.cx, 0);
		//	::OffsetRect(&rcSrc, szImage.cx-m_szxyBorderRound.cx, 0);
		//	gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);
		//	// 右下角
		//	::OffsetRect(&rc, 0, rectHeight(rcWnd)-m_szxyBorderRound.cy);
		//	::OffsetRect(&rcSrc, 0, szImage.cy-m_szxyBorderRound.cy);
		//	gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);
		//	// 左下角
		//	::OffsetRect(&rc, -(rectWidth(rcWnd)-m_szxyBorderRound.cx), 0);
		//	::OffsetRect(&rcSrc, -(szImage.cx-m_szxyBorderRound.cx), 0);
		//	gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);
		//	// 上边框
		//	rc = rcWnd;
		//	rc.left += m_szxyBorderRound.cx;
		//	rc.right -= m_szxyBorderRound.cx;
		//	rc.bottom = rc.top + m_szxyBorderRound.cy;
		//	rcSrc = rcImage;
		//	rcSrc.left = m_szxyBorderRound.cx;
		//	rcSrc.right = szImage.cx - m_szxyBorderRound.cx;
		//	//rcSrc.top = 0;
		//	rcSrc.bottom = rcSrc.top + m_szxyBorderRound.cy;
		//	gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);
		//	// 底边框
		//	::OffsetRect(&rc, 0, rectHeight(rcWnd)-m_szxyBorderRound.cy);
		//	::OffsetRect(&rcSrc, 0, szImage.cy-m_szxyBorderRound.cy);
		//	gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);

		//}
		//// 标题区
		//// 1、左边框
		//rc = rcWnd;
		//rc.right = rc.left + m_szxyBorderRound.cx;
		//rc.bottom = rc.top + titleHeight;
		//rc.top += m_szxyBorderRound.cy;	// 排除上边框
		//rcSrc = rcImage;
		//rcSrc.right = m_szxyBorderRound.cx;
		//rcSrc.bottom = rcSrc.top + titleHeight;
		//rcSrc.top += m_szxyBorderRound.cy;
		////rcSrc.left = 0;
		////rcSrc.right = m_szxyBorderRound.cx;
		////rcSrc.top = m_szxyBorderRound.cy;
		////rcSrc.bottom = titleHeight;
		//gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);
		//// 2、右边框
		//::OffsetRect(&rc, rectWidth(rcWnd)-m_szxyBorderRound.cx, 0);
		//::OffsetRect(&rcSrc, szImage.cx-m_szxyBorderRound.cx, 0);
		//gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);
		//// 3、填充区
		//rc.left = rcWnd.left + m_szxyBorderRound.cx;
		//rc.right = rcWnd.right - m_szxyBorderRound.cx;
		//rcSrc.left = m_szxyBorderRound.cx;
		//rcSrc.right = szImage.cx - m_szxyBorderRound.cx;
		//gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);
		////// 4、标题文本
		////if (m_beActive != BEUCActiveFlagEditTitle)
		////{
		////	rc = rcWnd;
		////	//rc.right = rc.left + m_szxyBorderRound.cx;
		////	rc.bottom = rc.top + titleHeight;
		////	rc.left += titleHeight;		// 文字区排除左右两个按钮
		////	rc.right -= titleHeight;

		////	bool bTheme = false;
		////	acm::theme themeShadow;
		////	if (themeShadow.open(m_pManager->getPaintWindow(), L"TextStyle"))
		////	{
		////		HFONT holdFont = (HFONT)SelectObject(hdc, m_pManager->getFont(-1));
		////		bTheme = themeShadow.drawShadowText(hdc, getTitle(), &rc);
		////		themeShadow.close();
		////		if(holdFont) SelectObject(hdc, holdFont);
		////	}
		////	if (!bTheme)
		////	{
		////		//gtc::drawer::drawString(grap, m_wsText.c_str(), rc, m_pManager->getDefaultFontInfo()->hFont, BEAlignModeAllCenter, m_bFocused ? KPDESK_COLOR_GROUP_TITLE_TEXT_HOT : KPDESK_COLOR_GROUP_TITLE_TEXT, false, true);
		////	}
		////}
		//// 客户区
		//// 1、左边框
		//rc = m_pClientUC->getPosition();
		//m_pManager->clientToScreen(rc);
		//rc.right = rc.left + m_szxyBorderRound.cx;
		//rc.bottom -= m_szxyBorderRound.cy;		// 排出底边框
		//rcSrc = rcImage;
		////rcSrc.left = 0;
		//rcSrc.right = m_szxyBorderRound.cx;
		//rcSrc.top += titleHeight;
		//rcSrc.bottom -= m_szxyBorderRound.cy;
		//gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);
		//// 2、右边框
		//::OffsetRect(&rc, rectWidth(rcWnd)-m_szxyBorderRound.cx, 0);
		//::OffsetRect(&rcSrc, szImage.cx-m_szxyBorderRound.cx, 0);
		//gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);
		//// 3、填充区
		//rc.left = rcWnd.left + m_szxyBorderRound.cx;
		//rc.right = rcWnd.right - m_szxyBorderRound.cx;
		//rcSrc.left = m_szxyBorderRound.cx;
		//rcSrc.right = szImage.cx - m_szxyBorderRound.cx;
		//gtc::drawer::drawImage(grap, *m_pImage, rc, rcSrc);
	}
}

void windowUC::onLButtonDown(UINT nFlags, POINT pt, bool& bHandled)
{
	m_lastMousePos = pt;
	//m_pManager->setFocus(this);	// 可以接收键盘消息哦
	//setFocus(true);

	m_beSelectMode = BESelectModeNone;
	m_iOperateHandle = onNcHitTest(pt);
	switch(m_iOperateHandle)
	{
	case HTCAPTION:
		{
			if (!isLockPos())
			{
				m_beSelectMode = BESelectModeMove;
				m_beActive = BEUCActiveFlagSize;
				bHandled = true;
				//SEND_LOG(L"HTCAPTION %d,%d", pt.x, pt.y);
				return;
			}
		}
		break;
	case HTTOPLEFT:
	case HTTOPRIGHT:
	case HTTOP:
	case HTBOTTOMLEFT:
	case HTBOTTOMRIGHT:
	case HTBOTTOM:
	case HTLEFT:
	case HTRIGHT:
		{
			if (!isLockPos())
			{
				m_beSelectMode = BESelectModeSize;
				m_beActive = BEUCActiveFlagSize;
				bHandled = true;
				//SEND_LOG(L"windowUC::onLButtonDown  BESelectModeSize");
				return;
			}
		}
		break;
	case HTSYSMENU:
		{
			//controlUC *pControl = findControl(pt, false);
			//if(!pControl) return;
			//if(pControl->getManager() != this) return;
			//pControl->setFocus();
			//BSEventUC event = { 0 };
			//event.Type = BEUCEventButtonDown;
			//event.pSender = pControl;
			//event.ptMouse = pt;
			//screenToClient(&event.ptMouse);
			//event.wParam = nFlags;
			//event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
			//event.wKeyState = (WORD)nFlags;
			//event.dwTimestamp = ::GetTickCount();
			////SEND_LOG(L"windowUC::onLButtonDown %s <%s> <%s>", pControl->getClass(), gtc::format::point(pt).c_str(), gtc::format::point(event.ptMouse).c_str());
			//pControl->Event(event);
			//m_pEventClick = pControl;
			//bHandled = true;
			//m_beActive = BEUCActiveFlagSysMenu;
		}
		break;

	default:
		break;
	}
	SEND_LOG(L"NOTE windowUC::onLButtonDown 真正执行");
}
void windowUC::onLButtonUp(UINT nFlags, POINT pt, bool& bHandled)
{
	m_lastMousePos = pt;
	//SEND_LOG(L"windowUC::onLButtonUp 0");
	if (m_beSelectMode != BESelectModeNone)
	{
		m_beSelectMode = BESelectModeNone;
		m_beActive = BEUCActiveFlagNone;
		m_iOperateHandle = HTNOWHERE;
		//bHandled = true;	// 不能取消注释，传递给光标设定消息
		return;
	}
	SEND_LOG(L"NOTE windowUC::onLButtonUp 真正执行");
}
void windowUC::onRButtonDown(UINT nFlags, POINT pt, bool& bHandled)
{
	m_lastMousePos = pt;
	//setFocus(true);
	int handle = onNcHitTest(pt);
	if (handle == HTCAPTION)
	{
		//m_beActive = BEUCActiveFlagSysMenu;
		bHandled = true;
		return;
	}
	//controlUC *pControl = findControl(pt, false);
	//if(!pControl) return;
	//if(pControl->getManager() != this) return;
	//pControl->setFocus();
	//BSEventUC event = { 0 };
	//event.Type = BEUCEventRButtonDown;
	//event.pSender = pControl;
	//event.ptMouse = pt;
	//screenToClient(&event.ptMouse);
	//event.wParam = nFlags;
	//event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
	//event.wKeyState = (WORD)nFlags;
	//event.dwTimestamp = ::GetTickCount();
	////SEND_LOG(L"windowUC::onRButtonDown %s <%s> <%s>", pControl->getClass(), gtc::format::point(pt).c_str(), gtc::format::point(event.ptMouse).c_str());
	//pControl->Event(event);
	//m_pEventClick = pControl;
}
void windowUC::onRButtonUp(UINT nFlags, POINT pt, bool& bHandled)
{
	m_lastMousePos = pt;
	int handle = onNcHitTest(pt);
	if (handle == HTCAPTION)
	{
		bHandled = true;
		return;
	}
	//if (m_pEventClick)
	//{
	//	BSEventUC event = { 0 };
	//	event.Type = BEUCEventContextMenu;
	//	event.pSender = m_pEventClick;
	//	event.ptMouse = pt;
	//	screenToClient(&event.ptMouse);
	//	event.wParam = nFlags;
	//	event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
	//	event.wKeyState = (WORD)nFlags;
	//	event.dwTimestamp = ::GetTickCount();
	//	m_pEventClick->Event(event);
	//	m_pEventClick = nullptr;
	//	//bHandled = true;
	//}
}
void windowUC::onMouseMove(UINT nFlags, POINT pt, bool& bHandled)
{
	if(!isLockPos())
	{
		//::SetCursor(getCursor(gc_iHandle));
		if (m_beSelectMode == BESelectModeMove)
		{
			//SEND_LOG(L"windowUC::onMouseMove  BESelectModeMove %04X", keyState());
			RECT pos = getPosition();
			::OffsetRect(&pos, pt.x-m_lastMousePos.x, pt.y-m_lastMousePos.y);
			moveTo(pos);
			bHandled = true;
			m_lastMousePos = pt;	// 相关
			return;
		}
		else if (m_beSelectMode == BESelectModeSize)
		{
			//SEND_LOG(L"windowUC::onMouseMove  BESelectModeSize");
			moveHandleTo(m_iOperateHandle, pt);
			bHandled = true;
			m_lastMousePos = pt;	// 相关
			//RECT pos = getWindowPos();
			//SEND_LOG(L"windowUC::onMouseMove  left:%d  right:%d", pos.left, pos.right);
			return;
		}
	}

	//// 是否在子控件
	//m_lastMousePos = pt;

	//controlUC* pNewHover = findControl(pt, false);
	////SEND_LOG(L"windowUC::onMouseMove  %08X", pNewHover);
	//BSEventUC event = { 0 };
	//event.ptMouse = pt;
	//screenToClient(&event.ptMouse);
	//event.wParam = nFlags;
	//event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
	//event.dwTimestamp = ::GetTickCount();
	////SEND_LOG(L"windowUC::onMouseMove title:<%s> pt:<%s> mousePt:<%s> oldHover:<%s> newHover:<%s>", getTitle(), gtc::format::point(pt).c_str(), gtc::format::point(event.ptMouse).c_str(), m_pEventHover?m_pEventHover->getName().c_str():L"", pNewHover?pNewHover->getName().c_str():L"");
	//if( pNewHover != m_pEventHover && m_pEventHover != NULL )
	//{
	//	event.Type = BEUCEventMouseLeave;
	//	event.pSender = m_pEventHover;
	//	m_pEventHover->Event(event);
	//	m_pEventHover = nullptr;
	//}
	//if( pNewHover != m_pEventHover && pNewHover != NULL ) {
	//	event.Type = BEUCEventMouseEnter;
	//	event.pSender = pNewHover;
	//	pNewHover->Event(event);
	//	m_pEventHover = pNewHover;
	//}
	//if( m_pEventClick != NULL ) {
	//	//SEND_LOG(L"windowUC::onMouseMove click %s", m_pEventClick->getClass());
	//	event.Type = BEUCEventMouseMove;
	//	event.pSender = m_pEventClick;
	//	m_pEventClick->Event(event);
	//}
	//else if( pNewHover != NULL ) {
	//	event.Type = BEUCEventMouseMove;
	//	event.pSender = pNewHover;
	//	pNewHover->Event(event);
	//}
	//bHandled = m_pEventClick || m_pEventHover;
}

void windowUC::onLButtonDBClick(UINT nFlags, POINT pt, bool& bHandled)
{

}
void windowUC::onMouseWheel(UINT nFlags, short zDelta, POINT pt, bool& bHandled)
{
	m_lastMousePos = pt;

	//controlUC *pControl = findControl(pt);
	//if(!pControl) return;
	//if(pControl->getManager() != this) return;
	//pControl->setFocus();
	//BSEventUC event = { 0 };
	//event.Type = BEUCEventScrollWheel;
	//event.pSender = pControl;
	//event.ptMouse = pt;
	//screenToClient(&event.ptMouse);
	//event.wParam = MAKEWPARAM((zDelta < 0 ? SB_LINEDOWN : SB_LINEUP), kpdeskListView::getNodeSizeMax().cy);
	//event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
	////SEND_LOG(L"windowUC::onMouseWheel %u %d %08X class:%s", nFlags, zDelta, event.wParam, pControl->getClass());
	//event.wKeyState = (WORD)nFlags;
	//event.dwTimestamp = ::GetTickCount();
	//pControl->Event(event);
	//bHandled = true;
}
void windowUC::onMouseEntry(bool& bHandled)
{
	m_bInitial = false;
	setActivated(true);
	bHandled = true;
}
void windowUC::onMouseLeave(bool& bHandled)
{
	setActivated(false);
	if(!m_bFocused) m_beActive = BEUCActiveFlagNone;
	if(!m_bFocused) m_iOperateHandle = HTNOWHERE;
	//
	bHandled = true;
}
void windowUC::onMouseHover(UINT nFlags, POINT pt, bool& bHandled)
{
}
LRESULT windowUC::onNcHitTest(POINT pt)
{
	if(!m_pTitleBar || !ptIn(pt, true)) return HTNOWHERE;
	RECT rcClient = getWindowNcPos();
	//RECT rcClient = getWindowPos();
	//rcClient.left -= m_rcSizeBox.left/2;
	//rcClient.top -= m_rcSizeBox.top/2;
	//rcClient.right += m_rcSizeBox.right/2;
	//rcClient.bottom += m_rcSizeBox.bottom/2;
	// 窗口周边
	if( pt.y < rcClient.top + m_rcSizeBox.top ) {
		if( pt.x < rcClient.left + m_rcSizeBox.left ) return HTTOPLEFT;
		if( pt.x > rcClient.right - m_rcSizeBox.right ) return HTTOPRIGHT;
		return HTTOP;
	}
	else if( pt.y > rcClient.bottom - m_rcSizeBox.bottom ) {
		if( pt.x < rcClient.left + m_rcSizeBox.left ) return HTBOTTOMLEFT;
		if( pt.x > rcClient.right - m_rcSizeBox.right ) return HTBOTTOMRIGHT;
		return HTBOTTOM;
	}
	if( pt.x < rcClient.left + m_rcSizeBox.left ) return HTLEFT;
	if( pt.x > rcClient.right - m_rcSizeBox.right ) return HTRIGHT;
	// 标题
	RECT rcCaption = m_pTitleBar->getPosition();

	if(::PtInRect(&rcCaption, pt)) {
		assert(m_pManager);
		//controlUC *pControl = m_pManager->findControl(pt, false);
		//if (pControl)
		//{
		//	if(std::wcscmp(pControl->getClass(), L"buttonUC") == 0)
		//	{
		//		//SEND_LOG(L"HTSYSMENU %d,%d %s", pt.x, pt.y, pControl->getName().c_str());
		//		return HTSYSMENU;
		//	}
		//	//SEND_LOG(L"HTCAPTION %s<%s>", pControl->getClass(), pControl->getName().c_str());
		//	return HTCAPTION;
		//}
		return HTCAPTION;
	}

	return HTCLIENT;
}
RECT windowUC::getWindowNcPos() const
{
	RECT rc = getPosition();
	rc.left -= m_rcSizeBox.left/2;
	rc.top -= m_rcSizeBox.top/2;
	rc.right += m_rcSizeBox.right/2;
	rc.bottom += m_rcSizeBox.bottom/2;
	return rc;
}
//int windowUC::hitTest(const POINT &pt)
//{
//	if(!m_pTitleBar || !ptIn(pt, true)) return HTNOWHERE;
//	RECT rcClient = getWindowNcPos();
//	//RECT rcClient = getWindowPos();
//	//rcClient.left -= m_rcSizeBox.left/2;
//	//rcClient.top -= m_rcSizeBox.top/2;
//	//rcClient.right += m_rcSizeBox.right/2;
//	//rcClient.bottom += m_rcSizeBox.bottom/2;
//	// 窗口周边
//	if( pt.y < rcClient.top + m_rcSizeBox.top ) {
//		if( pt.x < rcClient.left + m_rcSizeBox.left ) return HTTOPLEFT;
//		if( pt.x > rcClient.right - m_rcSizeBox.right ) return HTTOPRIGHT;
//		return HTTOP;
//	}
//	else if( pt.y > rcClient.bottom - m_rcSizeBox.bottom ) {
//		if( pt.x < rcClient.left + m_rcSizeBox.left ) return HTBOTTOMLEFT;
//		if( pt.x > rcClient.right - m_rcSizeBox.right ) return HTBOTTOMRIGHT;
//		return HTBOTTOM;
//	}
//	if( pt.x < rcClient.left + m_rcSizeBox.left ) return HTLEFT;
//	if( pt.x > rcClient.right - m_rcSizeBox.right ) return HTRIGHT;
//	// 标题
//	RECT rcCaption = m_pTitleBar->getPosition();
//
//	if(::PtInRect(&rcCaption, pt)) {
//		assert(m_pManager);
//		//controlUC *pControl = m_pManager->findControl(pt, false);
//		//if (pControl)
//		//{
//		//	if(std::wcscmp(pControl->getClass(), L"buttonUC") == 0)
//		//	{
//		//		//SEND_LOG(L"HTSYSMENU %d,%d %s", pt.x, pt.y, pControl->getName().c_str());
//		//		return HTSYSMENU;
//		//	}
//		//	//SEND_LOG(L"HTCAPTION %s<%s>", pControl->getClass(), pControl->getName().c_str());
//		//	return HTCAPTION;
//		//}
//		return HTCAPTION;
//	}
//
//	return HTCLIENT;
//}
void windowUC::moveTo(const RECT &position, bool mustMove /*= false*/)
{
	setPosition(position);
	SIZE sz = {m_rcItem.left, m_rcItem.top};
	setFixedXY(sz);
	setFixedWidth(m_rcItem.right - m_rcItem.left);
	setFixedHeight(m_rcItem.bottom - m_rcItem.top);
}
void windowUC::moveHandleTo(int handle, const POINT &pt)
{
	RECT pos = getPosition();
	switch(handle)
	{
	case HTTOPLEFT:
		pos.left = pt.x;
		pos.top = pt.y;
		break;
	case HTTOPRIGHT:
		pos.right = pt.x;
		pos.top = pt.y;
		break;
	case HTTOP:
		pos.top = pt.y;
		break;
	case HTBOTTOMLEFT:
		pos.left = pt.x;
		pos.bottom = pt.y;
		break;
	case HTBOTTOMRIGHT:
		pos.right = pt.x;
		pos.bottom = pt.y;
		break;
	case HTBOTTOM:
		//SEND_LOG(L"moveHandleTo %d old<%d %d %d %d>", pt.y,
		//	m_rtPos.left, m_rtPos.top, m_rtPos.right, m_rtPos.bottom);
		pos.bottom = pt.y;
		break;
	case HTLEFT:
		pos.left = pt.x;
		break;
	case HTRIGHT:
		pos.right = pt.x;
		break;

	}
	moveTo(pos);
}

HCURSOR windowUC::getCursor(int handle)
{
	//SEND_LOG(L"getCursor %d", handle);
	HCURSOR hcursor = LoadCursor(NULL, IDC_ARROW);
	switch(handle)
	{
	case HTTOPLEFT:
		hcursor = LoadCursor(NULL, IDC_SIZENWSE);
		break;
	case HTTOPRIGHT:
		hcursor = LoadCursor(NULL, IDC_SIZENESW);
		break;
	case HTTOP:
		hcursor = LoadCursor(NULL, IDC_SIZENS);
		break;
	case HTBOTTOMLEFT:
		hcursor = LoadCursor(NULL, IDC_SIZENESW);
		break;
	case HTBOTTOMRIGHT:
		hcursor = LoadCursor(NULL, IDC_SIZENWSE);
		break;
	case HTBOTTOM:
		hcursor = LoadCursor(NULL, IDC_SIZENS);
		break;
	case HTLEFT:
		hcursor = LoadCursor(NULL, IDC_SIZEWE);
		break;
	case HTRIGHT:
		hcursor = LoadCursor(NULL, IDC_SIZEWE);
		break;
	case HTCAPTION:
		hcursor = LoadCursor(NULL, IDC_SIZEALL);
		break;
	case HTSYSMENU:
		hcursor = LoadCursor(NULL, IDC_HAND);
		break;
	default:
		//SEND_LOG(L"XXXXXXXXXXXXXXXXXXXXXXXXXX %d", handle);
		break;
	}
	return hcursor;
}

bool windowUC::ptIn(const POINT &pt, bool inclusionNc)
{
	if(inclusionNc)
	{
		RECT rc = getWindowNcPos();
		return ::PtInRect(&rc, pt) == TRUE;
	}
	return __super::ptIn(pt);
}

bool windowUC::ptInClient(POINT pt)
{
	bool bIn = false;
	if (m_pClientUC)
	{
		bIn = m_pClientUC->ptIn(pt);
	}
	return bIn;
}

bool windowUC::ptInCaption(POINT pt)
{
	bool bIn = false;
	if (m_pTitleBar)
	{
		bIn = m_pTitleBar->ptIn(pt);
	}
	return bIn;

}
void windowUC::clientToScreen(RECT &rc)
{
	::OffsetRect(&rc, m_rcItem.left, m_rcItem.top);
}
void windowUC::clientToScreen(POINT &pt)
{
	pt.x += m_rcItem.left;
	pt.y += m_rcItem.top;
}
void windowUC::screenToClient(RECT &rc)
{
	::OffsetRect(&rc, -m_rcItem.left, -m_rcItem.top);
}
void windowUC::screenToClient(POINT &pt)
{
	pt.x -= m_rcItem.left;
	pt.y -= m_rcItem.top;
}
void windowUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else __super::Event(event);
		return;
	}
	bool bHandled = false;
	//switch(event.Type)
	//{
	////case BEUCEventMouseEnter:
	////	{
	////		SEND_LOG(L"windowUC::Event %s title:<%s> sender:<%s> hit:%d", toString(BEUCEvent(event.Type)).c_str(), getTitle(), event.pSender?event.pSender->getName().c_str():L"", onNcHitTest(event.ptMouse));
	////		m_bInitial = false;
	////		setActivated(true);
	////		bHandled = true;
	////	}
	////	break;
	////case BEUCEventMouseLeave:
	////	{
	////		SEND_LOG(L"windowUC::Event %s title:<%s> sender:<%s> hit:%d", toString(BEUCEvent(event.Type)).c_str(), getTitle(), event.pSender?event.pSender->getName().c_str():L"", onNcHitTest(event.ptMouse));
	////		setActivated(false);
	////		if(!m_bFocused) m_beActive = BEUCActiveFlagNone;
	////		if(!m_bFocused) m_iOperateHandle = HTNOWHERE;
	////		//
	////		bHandled = true;
	////	}
	////	break;
	////case BEUCEventRButtonDown:
	////case BEUCEventButtonDown:
	////	{
	////		//SEND_LOG(L"ERROR windowUC::Event %s title:<%s> sender:<%s> ", toString(BEUCEvent(event.Type)).c_str(), getTitle(), event.pSender?event.pSender->getName().c_str():L"");
	////	}
	////	break;
	//default:
	//	bHandled = false;
	//	break;
	//}
	// 
	if (!bHandled)
	{
		__super::Event(event);
	}
}

}
