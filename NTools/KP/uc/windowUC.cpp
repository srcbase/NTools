#include "windowUC.h"
#include "controlUC.h"
#include "kpdeskListView.h"
namespace gtc{


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	windowUC类	2014-06-21
//	自绘窗口的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
HWND	windowUC::mc_hWnd = NULL;	// 依赖的绘图窗口，如：桌面窗口
std::vector<BSTimerInfo *>	windowUC::mc_vecTimers;	// 时钟集合
UINT		windowUC::mc_uTimerID = 0x1000;			// 实际时钟ID

windowUC::windowUC()
	: m_bActivate(false)
	, m_pEventClick(nullptr), m_pEventHover(nullptr)
	, m_bUpdateNeeded(true)
	, m_bInitial(true)
	, m_bVisible(true), m_bEnabled(true)
	, m_wsTitle(L""), m_iTitleHeight(0)
	, m_nAlphaClient(20), m_nAlphaTitle(20)
	, m_beSelectMode(BESelectModeNone)
	, m_bLockPos(false)
	, m_beActive(BEUCActiveFlagNone)
	, m_bFocus(false)
	, m_iOperateHandle(HTNOWHERE)
	, m_pimgBk(nullptr), m_pimgBkSelected(nullptr)
{
	//WRITE_LOG(L"windowUC::windowUC AAA");
	m_lastMousePos.x = m_lastMousePos.y	= 0;

	m_rtPos.left = m_rtPos.right = 0;
	m_rtPos.top = m_rtPos.bottom = 0;

	m_rcSizeBox.left = m_rcSizeBox.right = m_rcSizeBox.bottom = 8;
	m_rcSizeBox.top = 0; // 上边不可拖动

	m_szRoundCorner.cx = m_szRoundCorner.cy = 6;
	// 
	//WRITE_LOG(L"windowUC::windowUC BBB");
	memset(&m_bsDefaultFont, 0, sizeof(m_bsDefaultFont));
	LOGFONT lf = { 0 };
	//WRITE_LOG(L"windowUC::windowUC CCC");
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	lf.lfCharSet = DEFAULT_CHARSET;
	HFONT hDefaultFont = ::CreateFontIndirect(&lf);
	//WRITE_LOG(L"windowUC::windowUC DDD");
	m_bsDefaultFont.hFont = hDefaultFont;
	m_bsDefaultFont.sFontName = lf.lfFaceName;
	m_bsDefaultFont.iSize = -lf.lfHeight;
	m_bsDefaultFont.bBold = (lf.lfWeight >= FW_BOLD);
	m_bsDefaultFont.bUnderline = (lf.lfUnderline == TRUE);
	m_bsDefaultFont.bItalic = (lf.lfItalic == TRUE);
	//WRITE_LOG(L"windowUC::windowUC EEE");
	::ZeroMemory(&m_bsDefaultFont.tm, sizeof(m_bsDefaultFont.tm));
	//WRITE_LOG(L"windowUC::windowUC FFF");

}

windowUC::~windowUC()
{
	//gtc::app::writeVSLogs(L"windowUC free %s", m_wsTitle.c_str());
	if(m_bsDefaultFont.hFont) ::DeleteObject(m_bsDefaultFont.hFont);
	removeAllTimers();
}
RECT windowUC::getWindowPos() const
{
	return m_rtPos;
}
void windowUC::setWindowPos(RECT rt)
{
	if(m_rtPos == rt) return;
	invalidate();
	m_rtPos = rt;
	invalidate();
}
RECT windowUC::getWindowNcPos() const
{
	RECT rcClient = getWindowPos();
	rcClient.left -= m_rcSizeBox.left/2;
	rcClient.top -= m_rcSizeBox.top/2;
	rcClient.right += m_rcSizeBox.right/2;
	rcClient.bottom += m_rcSizeBox.bottom/2;
	return rcClient;
}
RECT windowUC::getClientRect() const
{
	RECT rc = getClientTitle();
	rc.top = rc.bottom;
	rc.bottom = m_rtPos.bottom-m_rtPos.top;
	//rc.top = rc.bottom;
	//rc.bottom = m_rtPos.bottom-m_rtPos.top-m_rcSizeBox.bottom/2;
	return rc;
}
RECT windowUC::getClientTitle() const
{
	RECT rc = {0};
	rc.right = m_rtPos.right-m_rtPos.left;
	rc.bottom = rc.top + m_iTitleHeight;

	return rc;
}
bool windowUC::ptIn(const POINT &pt, bool inclusionNc /*= false*/)
{
	RECT rc = inclusionNc ? getWindowNcPos() : getWindowPos();
	//rc.left -= m_rcSizeBox.left/2;
	//rc.top -= m_rcSizeBox.top/2;
	//rc.right += m_rcSizeBox.right/2;
	//rc.bottom += m_rcSizeBox.bottom/2;
	
	return ::PtInRect(&rc, pt) == TRUE;
}

void windowUC::clientToScreen(RECT *prect)
{
	assert(prect);
	::OffsetRect(prect, m_rtPos.left, m_rtPos.top);
}

void windowUC::clientToScreen(POINT *ppt)
{
	assert(ppt);
	//POINT pt = *ppt;
	ppt->x += m_rtPos.left;
	ppt->y += m_rtPos.top;
	//SEND_LOG(L"windowUC::clientToScreen old<%s> <%s> <%s>", gtc::format::point(pt).c_str(), gtc::format::point(*ppt).c_str(), gtc::format::rect(m_rtPos).c_str());
}

void windowUC::screenToClient(POINT *ppt)
{
	assert(ppt);
	ppt->x -= m_rtPos.left;
	ppt->y -= m_rtPos.top;
}

void windowUC::screenToClient(RECT *prect)
{
	assert(prect);
	::OffsetRect(prect, -m_rtPos.left, -m_rtPos.top);
}

void windowUC::invalidate(RECT& rcItem, bool isClient /*= true*/)
{
	RECT rc = rcItem;
	if(isClient)
		::OffsetRect(&rc, m_rtPos.left, m_rtPos.top);
	::InflateRect(&rc, 2, 2);
	::InvalidateRect(mc_hWnd, &rc, TRUE);
	//UpdateWindow(mc_hWnd);
}

void windowUC::invalidate(controlUC *pUC)
{
	if(!pUC) return;
	invalidate(pUC->getPosition());
	//UpdateWindow(mc_hWnd);
}

void windowUC::invalidate()
{
	::InvalidateRect(mc_hWnd, &m_rtPos, TRUE);
}

bool windowUC::isVisible() const
{
	return m_bVisible;
}
bool windowUC::isEnabled() const
{
	return m_bEnabled;
}
bool windowUC::isFocus() const
{
	return m_bFocus;
}
void windowUC::setFocus(bool bFocus /*= true*/)
{
	if(m_bFocus == bFocus) return;
	m_bFocus = bFocus;
	invalidate();
}
bool windowUC::isInitial() const
{
	return m_bInitial;
}
int windowUC::getOperateHandle() const
{
	return m_iOperateHandle;
}

gtc::imageResource* windowUC::getBkImage() const
{
	return m_pimgBk;
}
void windowUC::setBkImage(gtc::imageResource *pImage)
{
	m_pimgBk = pImage;
}
gtc::imageResource* windowUC::getBkImageSelected() const
{
	return m_pimgBkSelected;
}

void windowUC::setBkImageSelected(gtc::imageResource *pImage)
{
	m_pimgBkSelected = pImage;
}

SIZE windowUC::getRoundCorner() const
{
	return m_szRoundCorner;
}
void windowUC::setTitle(PCWSTR ptitle) {
	std::wstring title = ptitle ? ptitle :L"";
	setTitle(title);
}
void windowUC::setTitle(std::wstring wstitle)
{
	gtc::convert::replace(wstitle, L':', L' ');
	gtc::convert::replace(wstitle, L'*', L' ');
	gtc::convert::replace(wstitle, L'|', L' ');
	if(m_wsTitle == wstitle) return;
	m_wsTitle = wstitle;
	invalidate();
	//invalidate(getClientTitle());
}
void windowUC::showWindow(bool show /*= true*/)
{
	if(m_bVisible == show) return;
	m_bVisible = show;
	invalidate();
}

void windowUC::setEnabled(bool enabled /*= true*/)
{
	if(m_bEnabled == enabled) return;
	m_bEnabled = enabled;
	invalidate();
}

BSFontInfoUC* windowUC::getDefaultFont()
{
	if (m_bsDefaultFont.tm.tmHeight == 0)
	{
		HDC hdc = GetDC(mc_hWnd);
		HFONT hOldFont = (HFONT) ::SelectObject(hdc, m_bsDefaultFont.hFont);
		::GetTextMetrics(hdc, &m_bsDefaultFont.tm);
		::SelectObject(hdc, hOldFont);
		ReleaseDC(mc_hWnd, hdc);
	}
	return &m_bsDefaultFont;
}
void windowUC::setDefaultFont(PCWSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	std::wcscpy(lf.lfFaceName, pStrFontName);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if( bBold ) lf.lfWeight += FW_BOLD;
	if( bUnderline ) lf.lfUnderline = TRUE;
	if( bItalic ) lf.lfItalic = TRUE;
	HFONT hFont = ::CreateFontIndirect(&lf);
	if( hFont == NULL ) return;

	::DeleteObject(m_bsDefaultFont.hFont);
	m_bsDefaultFont.hFont = hFont;
	m_bsDefaultFont.sFontName = pStrFontName;
	m_bsDefaultFont.iSize = nSize;
	m_bsDefaultFont.bBold = bBold;
	m_bsDefaultFont.bUnderline = bUnderline;
	m_bsDefaultFont.bItalic = bItalic;

	::ZeroMemory(&m_bsDefaultFont.tm, sizeof(m_bsDefaultFont.tm));
	HDC hdc = GetDC(mc_hWnd);
	HFONT hOldFont = (HFONT) ::SelectObject(hdc, m_bsDefaultFont.hFont);
	::GetTextMetrics(hdc, &m_bsDefaultFont.tm);
	::SelectObject(hdc, hOldFont);
	ReleaseDC(mc_hWnd, hdc);

}
void windowUC::updateWindow()
{
	RECT rc = {0};
	if (GetClientRect(mc_hWnd, &rc))
	{
		//SEND_LOG(L"update <%d %d %d %d>", rc.left, rc.top, rc.right-rc.left,rc.bottom-rc.top);
		RedrawWindow(mc_hWnd, &rc, nullptr, RDW_ERASENOW);
	}
}

void windowUC::needUpdate()
{
	m_bUpdateNeeded = true;
}
bool windowUC::activate() const
{
	return m_bActivate;
}
void windowUC::setActivate(bool bActive)
{
	if(m_bActivate == bActive) return;
	//SEND_LOG(L"active %d  %d", m_bActivate, bActive);
	m_bActivate = bActive;
	//invalidate(m_rtPos, false);
	invalidate(getClientTitle());	// 减少刷新
}

void windowUC::addControl(controlUC *pControl)
{
	assert(pControl);
	m_vecControl.push_back(pControl);
}
void windowUC::removeControl(controlUC *pControl)
{
	assert(pControl);
	std::vector<controlUC *>::iterator iter = std::find(m_vecControl.begin(), m_vecControl.end(), pControl);
	if(iter != m_vecControl.end())
	{
		m_vecControl.erase(iter);
	}
	if(m_pEventClick == pControl) m_pEventClick = nullptr;
	if(m_pEventHover == pControl) m_pEventHover =nullptr;

}

bool windowUC::initControl(controlUC *pControl, controlUC *pParent /*= nullptr*/)
{
	assert(pControl);
	if(!pControl) return false;
	pControl->setManager(this, pParent ?pParent :pControl->getParent());
	m_vecControl.push_back(pControl);
	return true;
}

controlUC* windowUC::findControl(const POINT &pt, bool bLast /*= true*/)
{
	POINT point = pt;
	screenToClient(&point);
	if (bLast)
	{
		for (std::vector<controlUC *>::reverse_iterator iter = m_vecControl.rbegin(); iter != m_vecControl.rend(); ++iter)
		{
			if ((*iter)->ptIn(point))
			{
				return *iter;
			}
		}
	}
	else
	{
		for (std::vector<controlUC *>::iterator iter = m_vecControl.begin(); iter != m_vecControl.end(); ++iter)
		{
			if ((*iter)->ptIn(point))
			{
				return *iter;
			}
		}

	}
	return nullptr;
}

controlUC* windowUC::findControl(const std::wstring &wsName)
{
	for (std::vector<controlUC *>::iterator iter = m_vecControl.begin(); iter != m_vecControl.end(); ++iter)
	{
		if ((*iter)->getName() == wsName)
		{
			return *iter;
		}
	}
	return nullptr;
}

void windowUC::init(HWND hMain)
{
	assert(hMain);
	mc_hWnd = hMain;
}

VOID CALLBACK windowUC::doTimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	for (std::vector<BSTimerInfo *>::iterator iter = mc_vecTimers.begin(); iter != mc_vecTimers.end(); ++iter)
	{
		if ((*iter)->hWnd == mc_hWnd && (*iter)->uWinTimer == idEvent && (*iter)->bKilled == false)
		{
			//SEND_LOG(L"windowUC::doTimerProc %u", dwTime);
			BSEventUC event = { 0 };
			event.Type = BEUCEventTimer;
			event.pSender = (*iter)->pSender;
			event.wParam = (*iter)->nLocalID;
			event.dwTimestamp = ::GetTickCount();
			(*iter)->pSender->Event(event);
			break;
		}
	}
}

void windowUC::draw(HDC hdc)
{
	if(!isVisible() || !hdc) return;
	const RECT rcWnd = getWindowPos();
	const RECT rcTitle = getClientTitle();
	const int titleHeight = rectHeight(rcTitle);
	RECT rc = {0}, rcSrc = {0};
	Gdiplus::Graphics grap(hdc);
	gtc::imageResource *pImage = m_bFocus ? m_pimgBkSelected : m_pimgBk;
	//SEND_LOG(L"windowUC::draw %s<%s> image:%08X", getTitle(), gtc::format::rect(rcWnd,true).c_str(), pImage);
	if (pImage)
	{
		const SIZE szImage = pImage->getImageSize();
		// 窗口圆角
		if (m_szRoundCorner.cy > 0)
		{
			// 左上
			rc = rcWnd;
			rc.bottom = rc.top + m_szRoundCorner.cy;
			rc.right = rc.left + m_szRoundCorner.cx;
			rcSrc.left = rcSrc.top = 0;
			rcSrc.right = m_szRoundCorner.cx;
			rcSrc.bottom = m_szRoundCorner.cy;
			gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);
			//SEND_LOG(L"windowUC::draw 左上 image:<%d,%d> dest:<%s> src:<%s>", szImage.cx, szImage.cy, gtc::format::rect(rc, true).c_str(), gtc::format::rect(rcSrc, true).c_str());
			// 右上
			::OffsetRect(&rc, rectWidth(rcWnd)-m_szRoundCorner.cx, 0);
			::OffsetRect(&rcSrc, szImage.cx-m_szRoundCorner.cx, 0);
			gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);
			// 右下角
			::OffsetRect(&rc, 0, rectHeight(rcWnd)-m_szRoundCorner.cy);
			::OffsetRect(&rcSrc, 0, szImage.cy-m_szRoundCorner.cy);
			gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);
			// 左下角
			::OffsetRect(&rc, -(rectWidth(rcWnd)-m_szRoundCorner.cx), 0);
			::OffsetRect(&rcSrc, -(szImage.cx-m_szRoundCorner.cx), 0);
			gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);
			// 上边框
			rc = rcWnd;
			rc.left += m_szRoundCorner.cx;
			rc.right -= m_szRoundCorner.cx;
			rc.bottom = rc.top + m_szRoundCorner.cy;
			rcSrc.left = m_szRoundCorner.cx;
			rcSrc.right = szImage.cx - m_szRoundCorner.cx;
			rcSrc.top = 0;
			rcSrc.bottom = m_szRoundCorner.cy;
			gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);
			// 底边框
			::OffsetRect(&rc, 0, rectHeight(rcWnd)-m_szRoundCorner.cy);
			::OffsetRect(&rcSrc, 0, szImage.cy-m_szRoundCorner.cy);
			gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);

		}
		// 标题区
		// 1、左边框
		rc = rcWnd;
		rc.right = rc.left + m_szRoundCorner.cx;
		rc.bottom = rc.top + titleHeight;
		rc.top += m_szRoundCorner.cy;	// 排除上边框
		rcSrc.left = 0;//m_szRoundCorner.cx;
		rcSrc.right = m_szRoundCorner.cx;// - m_szRoundCorner.cx;
		rcSrc.top = m_szRoundCorner.cy;
		rcSrc.bottom = titleHeight;
		gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);
		// 2、右边框
		::OffsetRect(&rc, rectWidth(rcWnd)-m_szRoundCorner.cx, 0);
		::OffsetRect(&rcSrc, szImage.cx-m_szRoundCorner.cx, 0);
		gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);
		// 3、填充区
		rc.left = rcWnd.left + m_szRoundCorner.cx;
		rc.right = rcWnd.right - m_szRoundCorner.cx;
		rcSrc.left = m_szRoundCorner.cx;
		rcSrc.right = szImage.cx - m_szRoundCorner.cx;
		gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);
		// 4、标题文本
		if (m_beActive != BEUCActiveFlagEditTitle)
		{
			rc = rcWnd;
			//rc.right = rc.left + m_szRoundCorner.cx;
			rc.bottom = rc.top + titleHeight;
			rc.left += titleHeight;		// 文字区排除左右两个按钮
			rc.right -= titleHeight;

			bool bTheme = false;
			acm::theme themeShadow;
			if (themeShadow.open(mc_hWnd, L"TextStyle"))
			{
				bTheme = themeShadow.drawShadowText(hdc, m_wsTitle.c_str(), &rc);
				themeShadow.close();
			}
			if (!bTheme)
			{
				gtc::drawer::drawString(grap, m_wsTitle.c_str(), rc, getDefaultFont()->hFont, BEAlignModeAllCenter, m_bFocus ? KPDESK_COLOR_GROUP_TITLE_TEXT_HOT : KPDESK_COLOR_GROUP_TITLE_TEXT, false, true);
			}
			//HMODULE hMdl = NULL;
			//typedef int (WINAPI *dll_DrawShadowText)(HDC , LPCWSTR , UINT , const RECT *, DWORD , COLORREF , COLORREF , int , int );
			//hMdl = GetModuleHandle(L"ComCtl32.dll");
			//dll_DrawShadowText drawShadowText = NULL;
			//if (hMdl)
			//{
			//	drawShadowText = (dll_DrawShadowText)::GetProcAddress(hMdl, "DrawShadowText");
			//}
			//if (drawShadowText)
			//{
			//	//SEND_LOG(L"%08X %08X %d", hMdl, drawShadowText, GetLastError());
			//	HFONT oldFont = (HFONT)::SelectObject(hdc, getDefaultFont()->hFont);
			//	int ret = drawShadowText(hdc, m_wsTitle.c_str(), -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE, 0x00FFFFFF, 0x7f000000, 2, 2);
			//	::SelectObject(hdc, oldFont);
			//	//SEND_LOG(L"drawShadowText ret:%d", ret);
			//}
			//else
			//{
			//	gtc::drawer::drawString(grap, m_wsTitle.c_str(), rc, getDefaultFont()->hFont, BEAlignModeAllCenter, m_bFocus ? KPDESK_COLOR_GROUP_TITLE_TEXT_HOT : KPDESK_COLOR_GROUP_TITLE_TEXT, false, true);
			//	//gtc::drawer::drawStringEffectBk(grap, m_wsTitle.c_str(), rc, getDefaultFont()->hFont,
			//	//	BEAlignModeAllCenter, m_bFocus ? KPDESK_COLOR_GROUP_TITLE_TEXT_HOT : KPDESK_COLOR_GROUP_TITLE_TEXT, 0x8F404040);
			//}
		}
		// 客户区
		// 1、左边框
		rc = getClientRect();
		clientToScreen(&rc);
		rc.right = rc.left + m_szRoundCorner.cx;
		rc.bottom -= m_szRoundCorner.cy;		// 排出底边框
		rcSrc.left = 0;
		rcSrc.right = m_szRoundCorner.cx;
		rcSrc.top = titleHeight;
		rcSrc.bottom = szImage.cy - m_szRoundCorner.cy;
		gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);
		// 2、右边框
		::OffsetRect(&rc, rectWidth(rcWnd)-m_szRoundCorner.cx, 0);
		::OffsetRect(&rcSrc, szImage.cx-m_szRoundCorner.cx, 0);
		gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);
		// 3、填充区
		rc.left = rcWnd.left + m_szRoundCorner.cx;
		rc.right = rcWnd.right - m_szRoundCorner.cx;
		rcSrc.left = m_szRoundCorner.cx;
		rcSrc.right = szImage.cx - m_szRoundCorner.cx;
		gtc::drawer::drawImage(grap, *pImage, rc, rcSrc);
	}
	else
	{
		// 标题区
		rc = rcWnd;
		rc.bottom = rc.top + rectHeight(rcTitle);
		gtc::drawer::fillRectangle(grap, m_bFocus ? KPDESK_COLOR_GROUP_TITLE_HOT : KPDESK_COLOR_GROUP_TITLE, rc);
		if (m_beActive != BEUCActiveFlagEditTitle)
		{
			rc.left += rectHeight(rcTitle);		// 文字区排除左右两个按钮
			rc.right -= rectHeight(rcTitle);
			gtc::drawer::drawString(grap, m_wsTitle.c_str(), rc, getDefaultFont()->hFont, BEAlignModeAllCenter, m_bFocus ? KPDESK_COLOR_GROUP_TITLE_TEXT_HOT : KPDESK_COLOR_GROUP_TITLE_TEXT, false, true);
		}
		// 客户区
		rc = getClientRect();
		clientToScreen(&rc);
		gtc::drawer::fillRectangle(grap, m_bFocus ? KPDESK_COLOR_GROUP_CLIENT_HOT : KPDESK_COLOR_GROUP_CLIENT, rc);
		// 窗口边框线
		rc = rcWnd;
		rc.right -= 1;
		rc.bottom -= 1;
		gtc::drawer::drawBorder(grap, gtc::BSPen(BEPenStyleSolid, 1, m_bFocus ? KPDESK_COLOR_GROUP_BORDER_HOT: KPDESK_COLOR_GROUP_BORDER), rc, BEBorderStyleRectangle);
		// 标题与客户间的分割线
		gtc::drawer::drawLine(grap, gtc::BSPen(BEPenStyleSolid, 1, m_bFocus ? KPDESK_COLOR_GROUP_CUTLINE_HOT : KPDESK_COLOR_GROUP_CUTLINE), rc.left, rc.top+m_iTitleHeight, rc.right, rc.top+m_iTitleHeight);

	}

}


void windowUC::onKeyDown(UINT nChar, UINT nFlag, UINT nRepCnt)
{

}
void windowUC::onKeyUp(UINT nChar, UINT nFlag, UINT nRepCnt)
{

}
void windowUC::onChar(UINT nChar, UINT nFlag, UINT nRepCnt)
{

}

//windowUC::BESelectMode gc_beSelectMode = windowUC::BESelectModeNone;
//int gc_iHandle = HTNOWHERE;
void windowUC::onLButtonDblClk(UINT nFlags, POINT pt, bool& bHandled)
{
	m_lastMousePos = pt;
	setFocus(true);
	m_beSelectMode = BESelectModeNone;
	m_iOperateHandle = hitTest(pt);

}
void windowUC::onLButtonDown(UINT nFlags, POINT pt, bool& bHandled)
{
	m_lastMousePos = pt;
	setFocus(true);
	m_beSelectMode = BESelectModeNone;
	m_iOperateHandle = hitTest(pt);
	//if(!isLockPos()) ::SetCursor(getCursor(m_iOperateHandle));
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
			controlUC *pControl = findControl(pt, false);
			if(!pControl) return;
			if(pControl->getManager() != this) return;
			pControl->setFocus();
			BSEventUC event = { 0 };
			event.Type = BEUCEventButtonDown;
			event.pSender = pControl;
			event.ptMouse = pt;
			screenToClient(&event.ptMouse);
			event.wParam = nFlags;
			event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
			event.wKeyState = (WORD)nFlags;
			event.dwTimestamp = ::GetTickCount();
			//SEND_LOG(L"windowUC::onLButtonDown %s <%s> <%s>", pControl->getClass(), gtc::format::point(pt).c_str(), gtc::format::point(event.ptMouse).c_str());
			pControl->Event(event);
			m_pEventClick = pControl;
			bHandled = true;
			m_beActive = BEUCActiveFlagSysMenu;
		}
		break;

	default:
		break;
	}

	
}


void windowUC::onMouseMove(UINT nFlags, POINT pt, bool& bHandled)
{
	//SEND_LOG(L"windowUC::onMouseMove %d", m_iOperateHandle);
	//static int nc_iHandle = -1;
	//SEND_LOG(L"move handle %d %d", handle, gc_iHandle);
	//if(m_iOperateHandle > HTCLIENT) ::SetCursor(getCursor(m_iOperateHandle));
	//else if(!isLockPos())
	//{
	//	int handle = hitTest(pt);
	//	::SetCursor(getCursor(handle));
	//	//SEND_LOG(L"windowUC::onMouseMovewindowUC::onMouseMove %d", handle);
	//	//if (handle != nc_iHandle)
	//	//{
	//	//	nc_iHandle = handle;
	//	//	::SetCursor(getCursor(nc_iHandle));
	//	//}
	//}
	//if(!isLockPos()) ::SetCursor(getCursor(handle));
	//if(m_iOperateHandle > 0)
	//{
		if(!isLockPos())
		{
			//::SetCursor(getCursor(gc_iHandle));
			if (m_beSelectMode == BESelectModeMove)
			{
				//SEND_LOG(L"windowUC::onMouseMove  BESelectModeMove %04X", keyState());
				RECT pos = getWindowPos();
				::OffsetRect(&pos, pt.x-m_lastMousePos.x, pt.y-m_lastMousePos.y);
				moveTo(pos, false);
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
	//}
	// 是否在子控件
	m_lastMousePos = pt;

	controlUC* pNewHover = findControl(pt, false);
	//SEND_LOG(L"windowUC::onMouseMove  %08X", pNewHover);
	BSEventUC event = { 0 };
	event.ptMouse = pt;
	screenToClient(&event.ptMouse);
	event.wParam = nFlags;
	event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
	event.dwTimestamp = ::GetTickCount();
	//SEND_LOG(L"windowUC::onMouseMove title:<%s> pt:<%s> mousePt:<%s> oldHover:<%s> newHover:<%s>", getTitle(), gtc::format::point(pt).c_str(), gtc::format::point(event.ptMouse).c_str(), m_pEventHover?m_pEventHover->getName().c_str():L"", pNewHover?pNewHover->getName().c_str():L"");
	if( pNewHover != m_pEventHover && m_pEventHover != NULL )
	{
		event.Type = BEUCEventMouseLeave;
		event.pSender = m_pEventHover;
		m_pEventHover->Event(event);
		m_pEventHover = nullptr;
	}
	if( pNewHover != m_pEventHover && pNewHover != NULL ) {
		event.Type = BEUCEventMouseEnter;
		event.pSender = pNewHover;
		pNewHover->Event(event);
		m_pEventHover = pNewHover;
	}
	if( m_pEventClick != NULL ) {
		//SEND_LOG(L"windowUC::onMouseMove click %s", m_pEventClick->getClass());
		event.Type = BEUCEventMouseMove;
		event.pSender = m_pEventClick;
		m_pEventClick->Event(event);
	}
	else if( pNewHover != NULL ) {
		event.Type = BEUCEventMouseMove;
		event.pSender = pNewHover;
		pNewHover->Event(event);
	}
	bHandled = m_pEventClick || m_pEventHover;

	//if(!isLockPos()) ::SetCursor(getCursor(handle));
	// 
	//m_lastMousePos = pt;
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
		//updateWindow();
		bHandled = true;
		return;
	}
	if (m_pEventClick)
	{
		//SEND_LOG(L"windowUC::onLButtonUp %s", m_pEventClick->getClass());
		BSEventUC event = { 0 };
		event.Type = BEUCEventButtonUp;
		event.pSender = m_pEventClick;
		event.ptMouse = pt;
		screenToClient(&event.ptMouse);
		event.wParam = nFlags;
		event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
		event.wKeyState = (WORD)nFlags;
		event.dwTimestamp = ::GetTickCount();
		m_pEventClick->Event(event);
		if (std::wcscmp(m_pEventClick->getClass(), L"scrollBarUC") == 0)
			m_beActive = BEUCActiveFlagNone;
		m_pEventClick = nullptr;
		bHandled = true;
	}
}
void windowUC::onRButtonDown(UINT nFlags, POINT pt, bool& bHandled)
{
	m_lastMousePos = pt;
	setFocus(true);
	int handle = hitTest(pt);
	if (handle == HTCAPTION)
	{
		m_beActive = BEUCActiveFlagSysMenu;
		bHandled = true;
		return;
	}
	controlUC *pControl = findControl(pt, false);
	if(!pControl) return;
	if(pControl->getManager() != this) return;
	pControl->setFocus();
	BSEventUC event = { 0 };
	event.Type = BEUCEventRButtonDown;
	event.pSender = pControl;
	event.ptMouse = pt;
	screenToClient(&event.ptMouse);
	event.wParam = nFlags;
	event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
	event.wKeyState = (WORD)nFlags;
	event.dwTimestamp = ::GetTickCount();
	//SEND_LOG(L"windowUC::onRButtonDown %s <%s> <%s>", pControl->getClass(), gtc::format::point(pt).c_str(), gtc::format::point(event.ptMouse).c_str());
	pControl->Event(event);
	m_pEventClick = pControl;
	//bHandled = true;
}

void windowUC::onRButtonUp(UINT nFlags, POINT pt, bool& bHandled)
{
	m_lastMousePos = pt;
	int handle = hitTest(pt);
	if (handle == HTCAPTION)
	{
		bHandled = true;
		return;
	}
	//if (handle == HTSYSMENU)
	//{
	//	controlUC *pControl = findControl(pt);
	//	if(!pControl) return;
	//	if(pControl->getManager() != this) return;
	//	pControl->setFocus();
	//}
	if (m_pEventClick)
	{
		BSEventUC event = { 0 };
		event.Type = BEUCEventContextMenu;
		event.pSender = m_pEventClick;
		event.ptMouse = pt;
		screenToClient(&event.ptMouse);
		event.wParam = nFlags;
		event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
		event.wKeyState = (WORD)nFlags;
		event.dwTimestamp = ::GetTickCount();
		m_pEventClick->Event(event);
		m_pEventClick = nullptr;
		//bHandled = true;
	}
}

void windowUC::onMouseWheel(UINT nFlags, short zDelta, POINT pt, bool& bHandled)
{
	m_lastMousePos = pt;

	controlUC *pControl = findControl(pt);
	if(!pControl) return;
	if(pControl->getManager() != this) return;
	pControl->setFocus();
	BSEventUC event = { 0 };
	event.Type = BEUCEventScrollWheel;
	event.pSender = pControl;
	event.ptMouse = pt;
	screenToClient(&event.ptMouse);
	event.wParam = MAKEWPARAM((zDelta < 0 ? SB_LINEDOWN : SB_LINEUP), kpdeskListView::getNodeSizeMax().cy);
	event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
	//SEND_LOG(L"windowUC::onMouseWheel %u %d %08X class:%s", nFlags, zDelta, event.wParam, pControl->getClass());
	event.wKeyState = (WORD)nFlags;
	event.dwTimestamp = ::GetTickCount();
	pControl->Event(event);
	bHandled = true;
}

void windowUC::onMouseEntry(bool& bHandled)
{
	//SEND_LOG(L"windowUC::onMouseEntry 0 %s", getTitle());
	m_bInitial = false;
	setActivate(true);
	bHandled = true;
}

void windowUC::onMouseLeave(bool& bHandled)
{
	//SEND_LOG(L"windowUC::onMouseLeave 0 %s", getTitle());
	setActivate(false);
	if(!m_bFocus) m_beActive = BEUCActiveFlagNone;
	if(!m_bFocus) m_iOperateHandle = HTNOWHERE;
	//
	bHandled = true;
	if (m_pEventHover)
	{
		//SEND_LOG(L"windowUC::onMouseLeave 1 %08X %s", m_pEventHover, getTitle());
		//SEND_LOG(L"windowUC::onMouseLeave %s = %s", getTitle(), m_pEventHover->getClass());
		BSEventUC event = { 0 };
		GetCursorPos(&event.ptMouse);
		screenToClient(&event.ptMouse);
		event.wParam = 0;
		event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
		event.dwTimestamp = ::GetTickCount();
		event.Type = BEUCEventMouseLeave;
		event.pSender = m_pEventHover;
		m_pEventHover->Event(event);
		m_pEventHover = nullptr;
		//SEND_LOG(L"windowUC::onMouseLeave 1 %s", getTitle());
	}
}

void windowUC::onMouseHover(UINT nFlags, POINT pt, bool& bHandled)
{
	controlUC* pNewHover = findControl(pt, false);
	//SEND_LOG(L"windowUC::onMouseMove  %08X", pNewHover);
	BSEventUC event = { 0 };
	event.ptMouse = pt;
	screenToClient(&event.ptMouse);
	event.wParam = nFlags;
	event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
	event.dwTimestamp = ::GetTickCount();
	//SEND_LOG(L"windowUC::onMouseHover title:<%s> pt:<%s> mousePt:<%s> oldHover:<%s> newHover:<%s>", getTitle(), gtc::format::point(pt).c_str(), gtc::format::point(event.ptMouse).c_str(), m_pEventHover?m_pEventHover->getName().c_str():L"", pNewHover?pNewHover->getName().c_str():L"");
	if( pNewHover != m_pEventHover && m_pEventHover != NULL )
	{
		event.Type = BEUCEventMouseLeave;
		event.pSender = m_pEventHover;
		m_pEventHover->Event(event);
		m_pEventHover = nullptr;
	}
	if( pNewHover != m_pEventHover && pNewHover != NULL ) {
		event.Type = BEUCEventMouseEnter;
		event.pSender = pNewHover;
		pNewHover->Event(event);
		m_pEventHover = pNewHover;
	}
	if( pNewHover != NULL ) {
		event.Type = BEUCEventMouseMove;
		event.pSender = pNewHover;
		pNewHover->Event(event);
	}
	bHandled = m_pEventHover != nullptr;
}


LRESULT windowUC::onNcHitTest(POINT pt, bool& bHandled)
{
	//SEND_LOG(L"windowUC::onNcHitTest <%d,%d> %s", pt.x, pt.y, getTitle());
	//::SetCursor(LoadCursor(NULL, IDC_HAND));
	//return 0;
	int handle = hitTest(pt);
	//if(!isLockPos())
	//{
	//	::SetCursor(getCursor(handle));
	//}
	return handle;
}
//
//void windowUC::onDragEnter(HGLOBAL hgdata, DWORD keyState, POINT pt, bool &bHandled)
//{
//
//}
//void windowUC::onDragOver(DWORD keyState, POINT pt, bool &bHandled)
//{
//
//}
//void windowUC::onDragLeave(bool &bHandled)
//{
//
//}
//void windowUC::onDrop(HGLOBAL hgdata, DWORD keyState, POINT pt, bool &bHandled)
//{
//
//}

bool windowUC::addNotifier(INotifyUC* pControl)
{
	assert(pControl);
	if (std::find(m_vecNotifys.cbegin(), m_vecNotifys.cend(), pControl) == m_vecNotifys.cend())
	{
		m_vecNotifys.push_back(pControl);
		return true;
	}
	return false;
}
bool windowUC::removeNotifier(INotifyUC* pControl)
{
	std::vector<INotifyUC *>::const_iterator iter = std::find(m_vecNotifys.cbegin(), m_vecNotifys.cend(), pControl);
	if (iter != m_vecNotifys.cend())
	{
		m_vecNotifys.erase(iter);
		return true;
	}
	return false;
}
void windowUC::sendNotify(BSNotifyUC& Msg, bool bAsync /*= false*/)
{
	assert(Msg.pSender);
	Msg.ptMouse = m_lastMousePos;
	Msg.dwTimestamp = GetTickCount();
	if (!bAsync)
	{
		for (std::vector<INotifyUC *>::iterator iter = m_vecNotifys.begin(); iter != m_vecNotifys.end(); ++iter)
		{
			(*iter)->notify(Msg);
		}
	}
	else
	{

	}
}

void windowUC::sendNotify(controlUC* pControl, PCWSTR pstrMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
{
	BSNotifyUC Msg;
	Msg.pSender = pControl;
	Msg.wsType = pstrMessage;
	Msg.wParam = wParam;
	Msg.lParam = lParam;
	sendNotify(Msg, bAsync);
}

bool windowUC::setTimer(controlUC* pControl, UINT nTimerID, UINT uElapse)
{
	assert(pControl!=NULL && uElapse > 0);
	for(std::vector<BSTimerInfo *>::iterator iter = mc_vecTimers.begin(); iter != mc_vecTimers.end(); ++iter) {
		if( (*iter)->pSender == pControl
			&& (*iter)->hWnd == mc_hWnd
			&& (*iter)->nLocalID == nTimerID )
		{
			if( (*iter)->bKilled ) {
				if( ::SetTimer(mc_hWnd, (*iter)->uWinTimer, uElapse, doTimerProc) ) {
					(*iter)->bKilled = false;
					return true;
				}

				return false;
			}

			return false;
		}
	}

	mc_uTimerID = (++mc_uTimerID) % 0xFF;
	if( !::SetTimer(mc_hWnd, mc_uTimerID, uElapse, doTimerProc) ) return FALSE;
	BSTimerInfo* pTimer = new BSTimerInfo;
	if( pTimer == NULL ) return FALSE;
	pTimer->hWnd = mc_hWnd;
	pTimer->pSender = pControl;
	pTimer->nLocalID = nTimerID;
	pTimer->uWinTimer = mc_uTimerID;
	pTimer->bKilled = false;
	mc_vecTimers.push_back(pTimer);//.Add(pTimer);
	return true;
}
bool windowUC::killTimer(controlUC* pControl, UINT nTimerID)
{
	assert(pControl!=NULL);
	for(std::vector<BSTimerInfo *>::iterator iter = mc_vecTimers.begin(); iter != mc_vecTimers.end(); ++iter ) {
		if( (*iter)->pSender == pControl
			&& (*iter)->hWnd == mc_hWnd
			&& (*iter)->nLocalID == nTimerID )
		{
			if( (*iter)->bKilled == false ) {
				if( ::IsWindow(mc_hWnd) ) ::KillTimer((*iter)->hWnd, (*iter)->uWinTimer);
				(*iter)->bKilled = true;
				return true;
			}
		}
	}
	return false;
}
void windowUC::killTimer(controlUC* pControl)
{
	assert(pControl!=NULL);

	for (std::vector<BSTimerInfo *>::iterator iter = mc_vecTimers.begin(); iter != mc_vecTimers.end();)
	{
		if ((*iter)->pSender == pControl && (*iter)->hWnd == mc_hWnd)
		{
			if(!(*iter)->bKilled) ::KillTimer((*iter)->hWnd, (*iter)->uWinTimer);
			delete *iter;
			iter = mc_vecTimers.erase(iter);
		}
		else
			++iter;
	}

}
void windowUC::removeAllTimers()
{
	for (std::vector<BSTimerInfo *>::iterator iter = mc_vecTimers.begin(); iter != mc_vecTimers.end(); ++iter)
	{
		if(!(*iter)->bKilled)
		{
			if (::IsWindow(mc_hWnd))
			{
				::KillTimer(mc_hWnd, (*iter)->uWinTimer);
			}
			delete *iter;
		}
	}
	mc_vecTimers.clear();
}

int windowUC::hitTest(const POINT &pt)
{
	if(!ptIn(pt, true)) return HTNOWHERE;
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
	RECT rcCaption = getClientTitle();
	clientToScreen(&rcCaption);

	if( ::PtInRect(&rcCaption, pt) ) {

		controlUC *pControl = findControl(pt, false);
		if (pControl)
		{
			if(std::wcscmp(pControl->getClass(), L"buttonUC") == 0)
			{
				//SEND_LOG(L"HTSYSMENU %d,%d %s", pt.x, pt.y, pControl->getName().c_str());
				return HTSYSMENU;
			}
			//SEND_LOG(L"HTCAPTION %s<%s>", pControl->getClass(), pControl->getName().c_str());
			return HTCAPTION;
		}
		return HTCAPTION;
	}

	return HTCLIENT;
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

void windowUC::moveTo(const RECT &position, bool mustMove /*= false*/)
{
	//SEND_LOG(L"moveto %s", getTitle());
	if(m_rtPos == position) return;

	RECT rc = {0};
	IntersectRect(&rc, &m_rtPos, &position);

	//SEND_LOG(L"windowUC::moveTo handle:%d 交集：<%s> 原：<%s> 新：<%s>", m_iOperateHandle, gtc::format::rect(rc, true).c_str(), 
	//	gtc::format::rect(m_rtPos, true).c_str(), gtc::format::rect(position, true).c_str());
	//SEND_LOG(L"moveTo <%d %d %d %d> old<%d %d %d %d>", position.left, position.top, position.right, position.bottom,
		//m_rtPos.left, m_rtPos.top, m_rtPos.right, m_rtPos.bottom);
	invalidate(m_rtPos, false);
	setWindowPos(position);
	invalidate(m_rtPos, false);

}

void windowUC::moveHandleTo(int handle, const POINT &pt)
{
	RECT pos = getWindowPos();
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
	moveTo(pos, true);
}

void windowUC::close()
{
	BSEventUC event = { 0 };
	event.ptMouse = m_lastMousePos;
	screenToClient(&event.ptMouse);
	//event.wParam = nFlags;
	//event.lParam = MAKELPARAM(event.ptMouse.x, event.ptMouse.y);
	//event.wKeyState = (WORD)nFlags;
	event.dwTimestamp = ::GetTickCount();
	if (m_pEventHover)
	{
		event.Type = BEUCEventMouseLeave;
		event.pSender = m_pEventHover;
		m_pEventHover->Event(event);
		m_pEventHover = nullptr;
	}
	if (m_pEventClick)
	{
		event.Type = BEUCEventButtonUp;
		event.pSender = m_pEventClick;
		m_pEventClick->Event(event);
		m_pEventClick = nullptr;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	windowWnd类	2014-06-21
//	消息窗口的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////

HINSTANCE windowWnd::mc_hInstance = nullptr;	// 

windowWnd::windowWnd(void)
	: m_hWnd(NULL)
	, m_bMouseCapture(false)
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
	assert(mc_hInstance);

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = getClassStyle();
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = LoadIcon( mc_hInstance, IDI_APPLICATION );
	wc.lpfnWndProc = windowWnd::__wndProc;
	wc.hInstance = mc_hInstance;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = getWindowClassName();
	//bool bl = UnregisterClass(getWindowClassName(), mc_hInstance);
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
		if( !::GetClassInfoEx(mc_hInstance, getSuperClassName(), &wc) ) {
			assert(!"Unable to locate window class");
			return NULL;
		}
	}
	m_OldWndProc = wc.lpfnWndProc;
	wc.lpfnWndProc = windowWnd::__controlProc;
	wc.hInstance = mc_hInstance;
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

//static int seh_create_filter(LPEXCEPTION_POINTERS pExcept)
//{
//	if (pExcept)
//	{
//		if (pExcept->ExceptionRecord)
//		{
//			gtc::app::writeVSLogs(L"EXCEPTION_RECORD code:%08X flags:%08X exceptionRecord:%08X exceptionAddress:%08X NumberParameters:%d", 
//				pExcept->ExceptionRecord->ExceptionCode, pExcept->ExceptionRecord->ExceptionFlags, pExcept->ExceptionRecord->ExceptionRecord,
//				pExcept->ExceptionRecord->ExceptionAddress, pExcept->ExceptionRecord->NumberParameters);
//			return EXCEPTION_EXECUTE_HANDLER;
//		}
//	}
//	return EXCEPTION_CONTINUE_SEARCH;
//}
HWND windowWnd::create(HWND hwndParent, PCWSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
{
	WRITE_LOG(L"windowWnd::create AAA %08X", mc_hInstance);
	assert(mc_hInstance);
	HMODULE hMdl = NULL;
	typedef HWND (WINAPI *dll_CreateWindowEx)(DWORD ,LPCTSTR ,LPCTSTR ,DWORD ,int ,int ,int ,int ,HWND ,HMENU ,HINSTANCE ,LPVOID );
	__try
	{
		if( getSuperClassName() != NULL && !registerSuperclass() ) __leave;
		if( getSuperClassName() == NULL && !registerWindowClass() ) __leave;
		hMdl = LoadLibrary(L"User32.dll");
		if(!hMdl) __leave;
		dll_CreateWindowEx createWindowEx = (dll_CreateWindowEx)GetProcAddress(hMdl, "CreateWindowExW");
		WRITE_LOG(L"windowWnd::create BBB %08X %d", createWindowEx, GetLastError());
		if(!createWindowEx) __leave;
		//int *pp = nullptr;
		//*pp	= 10;
		m_hWnd = createWindowEx(dwExStyle, getWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, mc_hInstance, this);
	}
	__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"windowWnd::create"))
	{
		m_hWnd = NULL;
		WRITE_LOG(L"windowWnd::create SEH异常 name:<%s> code:%08X", pstrName?pstrName:L"", GetExceptionCode());
		//gtc::app::writeVSLogs(L"windowWnd::create 异常失败！name:<%s> code:%08X", pstrName?pstrName:L"", GetExceptionCode());
	}
	//WRITE_LOG(L"windowWnd::create EEE %08X", m_hWnd);
	//////////////////////////////////////////////////////////////////////////
	//__finally
	//{
	//	gtc::app::writeVSLogs(L"windowWnd::create 失败！%08X", GetExceptionCode());
	//}
	//////////////////////////////////////////////////////////////////////////
	//try
	//{
	//	m_hWnd = ::CreateWindowEx(dwExStyle, getWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, mc_hInstance, this);
	//}
	//catch (...)
	//{
	//	gtc::app::writeVSLogs(L"windowWnd::create 失败！%d", GetLastError());
	//}
	//assert(m_hWnd!=NULL);
	if(hMdl) FreeLibrary(hMdl);	
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
//	m_hWnd = ::CreateWindowEx(dwExStyle, getWindowClassName(), pstrName, dwStyle, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hwndParent, hMenu, mc_hInstance, this);
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
	HICON hIcon = (HICON)::LoadImage(mc_hInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	assert(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);
	hIcon = (HICON)::LoadImage(mc_hInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
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
		//if( !paintManagerUI::translateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		//}
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
	RECT rtpos = getWindowPos();
	if(rtpos == rt) return;
	::SetWindowPos(m_hWnd, nullptr, rt.left, rt.top, rt.right-rt.left, rt.bottom-rt.top,  SWP_NOZORDER | SWP_NOACTIVATE);
}

void windowWnd::setCapture()
{
	::SetCapture(m_hWnd);
	m_bMouseCapture = true;
}

void windowWnd::releaseCapture()
{
	::ReleaseCapture();
	m_bMouseCapture = false;
}

bool windowWnd::isCaptured()
{
	return m_bMouseCapture;
}

LRESULT windowWnd::sendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	assert(::IsWindow(m_hWnd));
	if(!m_hWnd) return 0;
	return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
} 

LRESULT windowWnd::postMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	assert(::IsWindow(m_hWnd));
	if(!m_hWnd) return 0;
	return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

void windowWnd::setInstance(HINSTANCE hInstance)
{
	assert(hInstance);
	mc_hInstance = hInstance;
}

void windowWnd::messageLoop()
{
	MSG msg = { 0 };
	while( ::GetMessage(&msg, NULL, 0, 0) ) {
		//if (!msg.hwnd)
		//{
		//	WRITE_LOG(L"windowWnd::messageLoop 句柄为空 hWnd:%08X msg:%04X", msg.hwnd, msg.message);
		//}
		//if( !paintManagerUI::translateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		//}
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
		WRITE_LOG(L"windowWnd::__wndProc WM_NCCREATE wp:%08X lp:%08X", wParam, lParam);
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