#include "baseUC.h"

namespace gtc{
labelUC::labelUC(void)
	: controlUC()
	, m_uiTextStyle(DT_VCENTER), m_dwTextColor(0)
	, m_dwDisabledTextColor(0), m_iFont(-1), m_bShowHtml(false)
{
}

labelUC::~labelUC(void)
{
}

PCWSTR labelUC::getClass() const
{
	return L"labelUC";
}
LPVOID labelUC::getInterface(PCWSTR pstrName)
{
	if(std::wcscmp(pstrName, L"Label") == 0) return static_cast<labelUC *>(this);
	return controlUC::getInterface(pstrName);
}

void labelUC::Event(BSEventUC &event)
{
	switch(event.Type)
	{
	case BEUCEventSetFocus:
		m_bFocused = true;
		break;
	case BEUCEventKillFocus:
		m_bFocused = false;
		break;
	case BEUCEventMouseEnter:
		return ;
		break;
	case BEUCEventMouseLeave:
		return ;
		break;
	}
	__super::Event(event);
}

void labelUC::paintText(HDC hdc)
{

}

void labelUC::setTextStyle(UINT uStyle)
{
	m_uiTextStyle = uStyle;
	invalidate();
}

UINT labelUC::getTextStyle() const
{
	return m_uiTextStyle;
}

void labelUC::setTextColor(DWORD dwTextColor)
{
	m_dwTextColor = dwTextColor;
}

DWORD labelUC::getTextColor() const
{
	return m_dwTextColor;
}

void labelUC::setDisabledTextColor(DWORD dwTextColor)
{
	m_dwDisabledTextColor = dwTextColor;
}

DWORD labelUC::getDisabledTextColor() const
{
	return m_dwDisabledTextColor;
}

void labelUC::setFont(int index)
{
	m_iFont = index;
}

int labelUC::getFont() const
{
	return m_iFont;
}

RECT labelUC::getTextPadding() const
{
	return m_rcTextPadding;
}

void labelUC::setTextPadding(RECT rc)
{
	m_rcTextPadding = rc;
	invalidate();
}

bool labelUC::isShowHtml()
{
	return m_bShowHtml;
}

void labelUC::setShowHtml(bool bShowHtml /*= true*/)
{
	if( m_bShowHtml == bShowHtml ) return;

	m_bShowHtml = bShowHtml;
	invalidate();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	buttonUC类	2014-06-23
//	自绘按钮
////////////////////////////////////////////////////////////////////////////////////////////////////////

buttonUC::buttonUC(void)
	: labelUC()
	, m_uiState(0), m_pImage(nullptr)
{

}

buttonUC::~buttonUC(void)
{
	//gtc::app::writeVSLogs(L"buttonUC free");
}

PCWSTR buttonUC::getClass() const
{
	return L"buttonUC";
}

LPVOID buttonUC::getInterface(PCWSTR pstrName)
{
	if(std::wcscmp(pstrName, L"Button") == 0) return static_cast<buttonUC *>(this);
	return labelUC::getInterface(pstrName);
}

void buttonUC::paintBkColor(HDC hdc)
{
	//if (m_uiState & UCSTATE_HOT)
	//{
	//	Gdiplus::Graphics grap(hdc);
	//	// 按钮位置（客户坐标）
	//	RECT rcItem = getPosition();	
	//	clientToScreen(&rcItem);
	//	Gdiplus::SolidBrush brush(0xC0FFFFFF);
	//	grap.FillRectangle(&brush, toGdiplus(rcItem));
	//}
}

void buttonUC::paintBorder(HDC hdc)
{
	//if (m_uiState & UCSTATE_HOT)
	//{
	//	Gdiplus::Graphics grap(hdc);
	//	// 按钮位置（客户坐标）
	//	RECT rcItem = getPosition();
	//	clientToScreen(&rcItem);
	//	Gdiplus::Pen pen(0xFFFFFFFF, 1);
	//	rcItem.right -= 1;
	//	rcItem.bottom -= 1;
	//	grap.DrawRectangle(&pen, toGdiplus(rcItem));
	//}
}

void buttonUC::paintStatusImage(HDC hdc)
{
	if(!m_pImage) return;
	Gdiplus::Graphics grap(hdc);
	// 单个图标尺寸
	SIZE szImage = m_pImage->getImageSize();
	szImage.cy /= 3;
	// 按钮位置（客户坐标）
	const RECT rcItem = getPosition();	
	// 按钮大小
	const SIZE szItem = {rcItem.right - rcItem.left,rcItem.bottom - rcItem.top};
	POINT pt = {rcItem.left, rcItem.top};
	pt.x += (szItem.cx - szImage.cx)/2;
	pt.y += (szItem.cy - szImage.cy)/2;
	clientToScreen(&pt);
	Gdiplus::Rect rcImageDest(pt.x, pt.y, szImage.cx, szImage.cy);

	if( isFocused() ) m_uiState |= UCSTATE_FOCUSED;
	else m_uiState &= ~ UCSTATE_FOCUSED;
	if( !isEnabled() ) m_uiState |= UCSTATE_DISABLED;
	else m_uiState &= ~ UCSTATE_DISABLED;

	if (m_uiState & UCSTATE_DISABLED)
	{
	}
	else if (m_uiState & UCSTATE_PUSHED)
	{
		Gdiplus::Rect rcImageSrc(0,szImage.cy*2,szImage.cx, szImage.cy);	// 第三个图标
		grap.DrawImage(*m_pImage, rcImageDest, rcImageSrc.X, rcImageSrc.Y, rcImageSrc.Width, rcImageSrc.Height, Gdiplus::UnitPixel);
		return;
	}
	else if (m_uiState & UCSTATE_HOT)
	{
		Gdiplus::Rect rcImageSrc(0,szImage.cy*1,szImage.cx, szImage.cy);	// 第二个图标
		grap.DrawImage(*m_pImage, rcImageDest, rcImageSrc.X, rcImageSrc.Y, rcImageSrc.Width, rcImageSrc.Height, Gdiplus::UnitPixel);
		return;
	}
	else if (m_uiState & UCSTATE_FOCUSED)
	{
	}
	if (m_pwndManager->activate() || m_pwndManager->isFocus())
	{
		// 常规
		Gdiplus::Rect rcImageSrc(0,0,szImage.cx, szImage.cy);	// 第一个图标
		grap.DrawImage(*m_pImage, rcImageDest, rcImageSrc.X, rcImageSrc.Y, rcImageSrc.Width, rcImageSrc.Height, Gdiplus::UnitPixel);
	}

	//SEND_LOG(L"buttonUC::paintStatusImage state=%d", m_uiState);
}

void buttonUC::Event(BSEventUC &event)
{
	//SEND_LOG(L"buttonUC::Event %d %d", isMouseEnabled(), event.Type);
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else __super::Event(event);
		return;
	}

	switch(event.Type)
	{
	case BEUCEventSetFocus:
	case BEUCEventKillFocus:
		invalidate();
		break;
	case BEUCEventButtonDown:
		{
			if (ptIn(event.ptMouse) && isEnabled())
			{
				m_uiState |= UCSTATE_PUSHED | UCSTATE_CAPTURED;
				invalidate();
				//SEND_LOG(L"BEUCEventButtonDown");
			}
			return;
		}
		break;
	case BEUCEventMouseMove:
		{
			if (m_uiState & UCSTATE_CAPTURED)
			{
				if (ptIn(event.ptMouse))
					m_uiState |= UCSTATE_PUSHED;
				else
					m_uiState &= ~UCSTATE_PUSHED;
				invalidate();
				//SEND_LOG(L"BEUCEventMouseMove");
			}
			return;
		}
		break;
	case BEUCEventButtonUp:
		{
			if (m_uiState & UCSTATE_CAPTURED)
			{
				if (ptIn(event.ptMouse)) activate();
				m_uiState &= ~(UCSTATE_PUSHED | UCSTATE_CAPTURED);
				invalidate();
				//SEND_LOG(L"BEUCEventButtonUp");
				m_pwndManager->sendNotify(this, L"click", event.wParam, event.lParam);
			}
			if (isContextMenuUsed())
			{
				//SEND_LOG(L"BEUCEventButtonUp menu");
				m_pwndManager->sendNotify(this, L"menu", event.wParam, event.lParam);
			}
			return;
		}
		break;
	case BEUCEventMouseEnter:
		{
			if (isEnabled())
			{
				m_uiState |= UCSTATE_HOT;
				invalidate();
				SEND_LOG(L"BEUCEventMouseEnter");
			}
			return;
		}
		break;
	case BEUCEventMouseLeave:
		{
			if (isEnabled())
			{
				m_uiState &= ~UCSTATE_HOT;
				invalidate();
				SEND_LOG(L"BEUCEventMouseLeave");
			}
			return;
		}
		break;
	case BEUCEventContextMenu:
		{
			if (isContextMenuUsed())
			{
				m_pwndManager->sendNotify(this, L"menu", event.wParam, event.lParam);
			}
		}
		break;
	}

	__super::Event(event);
}

void buttonUC::setImage(gtc::imageResource *pimage)
{
	m_pImage = pimage;
	invalidate();
	//SEND_LOG(L"buttonUC::setImage %08X", m_pImage);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	editUC类	2014-07-15
//	文本编辑控件
////////////////////////////////////////////////////////////////////////////////////////////////////////

editUC::editUC()
	: labelUC()
	, m_pWindow(NULL), m_uiMaxChar(255), m_bReadOnly(false), 
	m_bPasswordMode(false), m_chPasswordChar(L'*'), m_uiButtonState(0), m_dwEditbkColor(0xFFFFFFFF)
{
	RECT rc = {4, 3, 4, 3};
	setTextPadding(rc);
	setBkColor(0xFFFFFFFF);
}
PCWSTR editUC::getClass() const
{
	return L"editUC";
}
LPVOID editUC::getInterface(PCWSTR pstrName)
{
	if(std::wcscmp(pstrName, L"Edit") == 0) return static_cast<editUC *>(this);
	return labelUC::getInterface(pstrName);
}

void editUC::Event(BSEventUC &event)
{

}
void editUC::setEnabled(bool bEnable)
{
	controlUC::setEnabled(bEnable);
	if( !isEnabled() ) {
		m_uiButtonState = 0;
	}
}

void editUC::setText(PCWSTR pstrText)
{
	m_wsText = pstrText;
	if( m_pWindow != NULL ) Edit_SetText(*m_pWindow, m_wsText.c_str());
	invalidate();
}

void editUC::setMaxChar(UINT uMax)
{
	m_uiMaxChar = uMax;
	if( m_pWindow != NULL ) Edit_LimitText(*m_pWindow, m_uiMaxChar);
}

UINT editUC::getMaxChar()
{
	return m_uiMaxChar;
}

void editUC::setReadOnly(bool bReadOnly)
{
	if( m_bReadOnly == bReadOnly ) return;

	m_bReadOnly = bReadOnly;
	if( m_pWindow != NULL ) Edit_SetReadOnly(*m_pWindow, m_bReadOnly);
	invalidate();
}

bool editUC::isReadOnly() const
{
	return m_bReadOnly;
}

void editUC::setPasswordMode(bool bPasswordMode)
{
	if( m_bPasswordMode == bPasswordMode ) return;
	m_bPasswordMode = bPasswordMode;
	invalidate();
}

bool editUC::isPasswordMode() const
{
	return m_bPasswordMode;
}

void editUC::setPasswordChar(wchar_t cPasswordChar)
{
	if( m_chPasswordChar == cPasswordChar ) return;
	m_chPasswordChar = cPasswordChar;
	if( m_pWindow != NULL ) Edit_SetPasswordChar(*m_pWindow, m_chPasswordChar);
	invalidate();
}

wchar_t editUC::getPasswordChar() const
{
	return m_chPasswordChar;
}


void editUC::setPos(RECT rc)
{
	controlUC::setPosition(rc);
	if( m_pWindow != NULL ) {
		RECT rcPos = m_pWindow->calPos();
		::SetWindowPos(m_pWindow->getHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
			rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);        
	}
}

void editUC::setVisible(bool bVisible)
{
	controlUC::setVisible(bVisible);
	//if( !isVisible() && m_pWindow != NULL ) m_pwndManager->setFocus(NULL);
}


void editUC::paintStatusImage(HDC hDC)
{
	/*if( isFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !isEnabled() ) m_uButtonState |= UISTATE_DISABLED;
	else m_uButtonState &= ~ UISTATE_DISABLED;

	if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
		if( !m_sDisabledImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sDisabledImage.c_str()) ) m_sDisabledImage.clear();
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
		if( !m_sFocusedImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sFocusedImage.c_str()) ) m_sFocusedImage.clear();
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( !m_sHotImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sHotImage.c_str()) ) m_sHotImage.clear();
			else return;
		}
	}

	if( !m_sNormalImage.empty() ) {
		if( !drawImage(hDC, (PCWSTR)m_sNormalImage.c_str()) ) m_sNormalImage.clear();
		else return;
	}*/
}

void editUC::paintText(HDC hDC)
{
	/*if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->getDefaultFontColor();
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->getDefaultDisabledColor();

	if( m_sText.empty() ) return;

	std::wstring sText = m_sText;
	if( m_bPasswordMode ) {
	sText.clear();
	PCWSTR p = m_sText.data();
	while( *p != L'\0' ) {
	sText += m_cPasswordChar;
	p = ::CharNext(p);
	}
	}

	RECT rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;
	if( isEnabled() ) {
	renderEngine::drawText(hDC, m_pManager, rc, sText.c_str(), m_dwTextColor, \
	m_iFont, DT_SINGLELINE | m_uTextStyle);
	}
	else {
	renderEngine::drawText(hDC, m_pManager, rc, sText.c_str(), m_dwDisabledTextColor, \
	m_iFont, DT_SINGLELINE | m_uTextStyle);
	}*/
}



editWnd::editWnd() : m_pOwner(NULL), m_hBkBrush(NULL)
{
	//TRACE_CLASS_BEGIN(this)
}

void editWnd::init(editUC* pOwner)
{
	m_pOwner = pOwner;
	RECT rcPos = calPos();
	UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;
	if( m_pOwner->isPasswordMode() ) uStyle |= ES_PASSWORD;
	//create(m_pOwner->getManager()->getPaintWindow(), NULL, uStyle, 0, rcPos);
	//SetWindowFont(m_hWnd, m_pOwner->getManager()->getDefaultFontInfo()->hFont, TRUE);
	Edit_LimitText(m_hWnd, m_pOwner->getMaxChar());
	if( m_pOwner->isPasswordMode() ) Edit_SetPasswordChar(m_hWnd, m_pOwner->getPasswordChar());
	Edit_SetText(m_hWnd, m_pOwner->getText());
	Edit_SetModify(m_hWnd, FALSE);
	sendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
	Edit_Enable(m_hWnd, m_pOwner->isEnabled() == true);
	Edit_SetReadOnly(m_hWnd, m_pOwner->isReadOnly() == true);
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	::SetFocus(m_hWnd);
}

RECT editWnd::calPos()
{
	RECT rcPos = m_pOwner->getPosition();
	RECT rcInset = m_pOwner->getTextPadding();
	rcPos.left += rcInset.left;
	rcPos.top += rcInset.top;
	rcPos.right -= rcInset.right;
	rcPos.bottom -= rcInset.bottom;
	LONG lEditHeight = m_pOwner->getManager()->getDefaultFont()->tm.tmHeight;
	if( lEditHeight <rectHeight(rcPos) ) {
		rcPos.top += (rectHeight(rcPos) - lEditHeight) / 2;
		rcPos.bottom = rcPos.top + lEditHeight;
	}
	return rcPos;
}

PCWSTR editWnd::getWindowClassName() const
{
	return L"EditWnd";
}

PCWSTR editWnd::getSuperClassName() const
{
	return WC_EDIT;
}

void editWnd::onFinalMessage(HWND /*hWnd*/)
{
	// Clear reference and die
	if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
	m_pOwner->m_pWindow = NULL;
	delete this;
}

LRESULT editWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	if( uMsg == WM_KILLFOCUS ) lRes = onKillFocus(uMsg, wParam, lParam, bHandled);
	else if( uMsg == OCM_COMMAND ) {
		if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) lRes = onEditChanged(uMsg, wParam, lParam, bHandled);
		else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) {
			RECT rcClient;
			::GetClientRect(m_hWnd, &rcClient);
			::InvalidateRect(m_hWnd, &rcClient, FALSE);
		}
	}
	else if( uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_RETURN ) {
		m_pOwner->getManager()->sendNotify(m_pOwner, L"return");
	}
	//else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) {
	//	if( m_pOwner->getNativeEditBkColor() == 0xFFFFFFFF ) return NULL;
	//	::SetBkMode((HDC)wParam, TRANSPARENT);
	//	DWORD dwTextColor = m_pOwner->getTextColor();
	//	::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
	//	if( m_hBkBrush == NULL ) {
	//		DWORD clrColor = m_pOwner->getNativeEditBkColor();
	//		m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
	//	}
	//	return (LRESULT)m_hBkBrush;
	//}
	//else bHandled = FALSE;
	//if( !bHandled ) return windowWnd::handleMessage(uMsg, wParam, lParam);
	return lRes;
}

LRESULT editWnd::onKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	postMessage(WM_CLOSE);
	return lRes;
}

LRESULT editWnd::onEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if( m_pOwner == NULL ) return 0;
	// Copy text back
	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
	PWSTR pstr = static_cast<PWSTR>(_alloca(cchLen * sizeof(TCHAR)));
	assert(pstr);
	if( pstr == NULL ) return 0;
	::GetWindowText(m_hWnd, pstr, cchLen);
	m_pOwner->m_wsText = pstr;
	m_pOwner->getManager()->sendNotify(m_pOwner, L"textchanged");
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	scrollBarUC类	2014-07-17
//	滚动条栏
////////////////////////////////////////////////////////////////////////////////////////////////////////
scrollBarUC::scrollBarUC()
	: controlUC()
	, m_pContainer(nullptr), m_iBarSize(9)
	, m_btAlpha(200)
	, m_iRange(0), m_iScrollPos(0)
	, m_bHorizontal(false)
	, m_bMoveThumb(false)
	, m_iLastScrollPos(0), m_iLastScrollOffset(0)
	, m_uiState(0)
	, m_pThumbImage(nullptr)
{
	memset(&m_rtThumb, 0, sizeof(m_rtThumb));
	memset(&m_ptLastMouse, 0, sizeof(m_ptLastMouse));
}

scrollBarUC::~scrollBarUC()
{
	//gtc::app::writeVSLogs(L"scrollBarUC free");
}


PCWSTR scrollBarUC::getClass() const
{
	return L"scrollBarUC";
}
LPVOID scrollBarUC::getInterface(PCWSTR pstrName)
{
	if(std::wcscmp(pstrName, L"scrollbar") == 0) return static_cast<scrollBarUC *>(this);
	return controlUC::getInterface(pstrName);
}

void scrollBarUC::setPosition(RECT &pos)
{
	__super::setPosition(pos);
	refreshLayout();
}
void scrollBarUC::paint(HDC hdc)
{
	if(m_iRange == 0) return;
	if(!m_pwndManager->isInitial())
		if(!(m_pwndManager->activate() || m_pwndManager->isFocus())) return;
	RECT rcItem = getPosition();

	//SEND_LOG(L"scrollBarUC::paint title:<%s> ", m_pwndManager->getTitle());
	//::InflateRect(&rcItem, -1, -1);
	//if (std::wcscmp(m_pwndManager->getTitle(), L"文件夹") == 0)
	//{
	//	SEND_LOG(L"scrollBarUC::paint %d %d <%s> <%s>", m_iRange, m_iScrollPos, gtc::format::rect(rcItem).c_str(), gtc::format::rect(m_rtThumb).c_str());
	//	SEND_LOG(L"scrollBarUC::paint %08X  %d %d", m_uiState, bool(m_uiState & UCSTATE_HOT), m_pwndManager->activate());
	//}
	Gdiplus::Graphics grap(hdc);
	//if (m_uiState & UCSTATE_HOT)
	//{
	//	// bar背景
	//	clientToScreen(&rcItem);
	//	Gdiplus::SolidBrush brush(KPDESK_COLOR_SCROLL_BK_HOT);
	//	grap.FillRectangle(&brush, toGdiplus(rcItem));
	//}
	//else
	//{
	//	// bar背景
	//	clientToScreen(&rcItem);
	//	Gdiplus::SolidBrush brush(KPDESK_COLOR_SCROLL_BK);
	//	grap.FillRectangle(&brush, toGdiplus(rcItem));

	//}
	//if ((m_uiState & UCSTATE_HOT) || m_pwndManager->activate())
	{
		// 滑块
		rcItem = m_rtThumb;
		::InflateRect(&rcItem, -1, 0);
		clientToScreen(&rcItem);
		
		if (m_pThumbImage)
		{
			const int thumbW = rectWidth(rcItem);
			const int imageW = m_pThumbImage->getWidth();
			RECT rcSrc = {0,0,imageW,m_pThumbImage->getHeight()};
			//// go
			gtc::drawer::drawImage(grap, m_pThumbImage,rcItem, rcSrc);
			//gtc::drawer::drawImage3PatchV(grap, *m_pThumbImage, rcItem, rcSrc, imageW);


			////SEND_LOG(L"------------>%d %d", imageW, thumbW);
			//::OffsetRect(&rcItem, (thumbW-imageW)/2, 0);
			//rcItem.right = rcItem.left + imageW;
			//RECT rc = rcItem;
			//rc.bottom = rc.top + imageW;
			//// 上圆弧
			////bool bl = grap.DrawImage(*m_pThumbImage, rc, 0, 0, imageW, imageW, Gdiplus::UnitPixel) == Gdiplus::Ok;
			//RECT rcSrc = {0,0,imageW, imageW};
			//gtc::drawer::drawImage(grap, *m_pThumbImage, rc, rcSrc);
			//// 下圆弧
			//rc = rcItem;
			//rc.top = rc.bottom - imageW;
			//::OffsetRect(&rcSrc, 0, m_pThumbImage->getHeight()-imageW);
			////grap.DrawImage(*m_pThumbImage, rc, 0, m_pThumbImage->getHeight()-imageW, imageW, imageW, Gdiplus::UnitPixel);
			//gtc::drawer::drawImage(grap, *m_pThumbImage, rc, rcSrc);
			////SEND_LOG(L"绘制滚动条 %s <%s> <%d,%d>", bl?L"成功":L"失败", gtc::format::rect(rcItem).c_str(), imageW, m_pThumbImage->getHeight());
			//// 中间块
			//rc = rcItem;
			//rc.top += imageW;
			//rc.bottom -= imageW;
			//grap.DrawImage(*m_pThumbImage, toGdiplus(rc), 0, imageW, imageW, imageW, Gdiplus::UnitPixel);
		}
		else
		{
			//Gdiplus::SolidBrush brush1(0xC0FFFFFF);
			//grap.FillRectangle(&brush1, toGdiplus(rcItem));
			gtc::drawer::fillRectangle(grap, 0xC0FFFFFF, rcItem);
		}
	}
	//else
	//{

	//}
	//SEND_LOG(L"scrollBarUC::paint <%s>", gtc::format::rect(m_rcItem).c_str());
	//assert(hdc);
	//RECT rc = {0}, rcClient = m_rcItem;

	//rc = rcClient;
	//const int width = rcClient.right - rcClient.left;
	//const int height = rcClient.bottom - rcClient.top;
	////SEND_LOG(L"bar WM_ERASEBKGND %08X <%s> <%s> %d", m_hWnd, gtc::format::rect(rc).c_str(), 
	////	gtc::format::rect(m_rtThumb).c_str(), m_iBarSize);
	//HDC tmpDC = CreateCompatibleDC(hdc);
	//HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
	//HBRUSH hbrush = CreateSolidBrush(0x0);
	//// 背景
	//HBITMAP oldBitmap = (HBITMAP)SelectObject(tmpDC, hBitmap);
	//HBRUSH oldbrush = (HBRUSH) SelectObject(tmpDC, hbrush);
	//::RoundRect(tmpDC, rc.left, rc.top, rc.right, rc.bottom, m_iBarSize/2, m_iBarSize/2);
	//// 滑块
	//if(hbrush) DeleteObject(hbrush);
	//hbrush = CreateSolidBrush(0xFFFFFF);
	//SelectObject(tmpDC, hbrush);
	//::RoundRect(tmpDC, m_rtThumb.left, m_rtThumb.top, m_rtThumb.right, m_rtThumb.bottom, m_iBarSize, m_iBarSize);

	//BitBlt(hdc, 0, 0, width, height, tmpDC, 0, 0, SRCCOPY);
	//// release
	//SelectObject(tmpDC, oldbrush);
	//SelectObject(tmpDC, oldBitmap);
	//DeleteObject(hbrush);
	//DeleteObject(hBitmap);
	//DeleteDC(tmpDC);
}

void scrollBarUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd) {
		if( m_pContainer != NULL ) m_pContainer->Event(event);
		else controlUC::Event(event);
		return;
	}

	//SEND_LOG(L"scrollBarUC::Event %d", event.Type);
	switch(event.Type)
	{
	case BEUCEventSetFocus:
		m_bFocused = true;
		return;
		break;
	case BEUCEventKillFocus:
		m_bFocused = false;
		return;
		break;
	case BEUCEventMouseEnter:
		{
			if (isEnabled())
			{
				m_uiState |= UCSTATE_HOT; 
				invalidate();
			}
			return;
		}
		break;
	case BEUCEventMouseLeave:
		{
			if (isEnabled())
			{
				m_uiState &= ~UCSTATE_HOT; 
				invalidate();
			}
			return;
		}
		break;
	case BEUCEventButtonDown:
	case BEUCEventDBLClick:
		{
			if(!isEnabled()) return;

			m_iLastScrollOffset = 0;
			//SEND_LOG(L"BEUCEventButtonDown setTimer");
			m_pwndManager->setTimer(this, DEFAULT_TIMERID, 50U);

			if( ::PtInRect(&m_rtThumb, event.ptMouse) )
			{
				m_uiState |= UCSTATE_CAPTURED | UCSTATE_PUSHED;
				m_bMoveThumb = true;
				m_ptLastMouse = event.ptMouse;
				m_iLastScrollPos = m_iScrollPos;
			}
			else
			{
				if (m_bHorizontal)
				{
					if( event.ptMouse.x < m_rtThumb.left ) {
						if( m_pContainer != NULL ) m_pContainer->pageLeft(); 
						else setScrollPos(m_iScrollPos + m_rcItem.left - m_rcItem.right);
					}
					else if ( event.ptMouse.x > m_rtThumb.right ){
						if( m_pContainer != NULL ) m_pContainer->pageRight(); 
						else setScrollPos(m_iScrollPos - m_rcItem.left + m_rcItem.right);                    
					}
				}
				else
				{
					if(event.ptMouse.y < m_rtThumb.top)
					{	// SB_PAGEUP
						if(m_pContainer) m_pContainer->pageUp();
						else setScrollPos(m_iScrollPos + m_rcItem.top - m_rcItem.bottom);
					}
					else if ( event.ptMouse.y > m_rtThumb.bottom )
					{	// SB_PAGEDOWN
						if(m_pContainer) m_pContainer->pageDown();
						else setScrollPos(m_iScrollPos - m_rcItem.top + m_rcItem.bottom);    
					}
				}

			}
			if(!m_pContainer) m_pwndManager->sendNotify(this, L"scroll");
			return;
		}
		break;
	case BEUCEventMouseMove:
		{
			if (m_uiState & UCSTATE_CAPTURED)
			{
				if( !m_bHorizontal ) {
					m_iLastScrollOffset = (event.ptMouse.y - m_ptLastMouse.y) * m_iRange / \
						(m_rcItem.bottom - m_rcItem.top - m_rtThumb.bottom + m_rtThumb.top/* - 2 * m_cxyFixed.cx*/);
				}
				else {
					m_iLastScrollOffset = (event.ptMouse.x - m_ptLastMouse.x) * m_iRange / \
						(m_rcItem.right - m_rcItem.left - m_rtThumb.right + m_rtThumb.left/* - 2 * m_cxyFixed.cy*/);
				}
				//SEND_LOG(L"BEUCEventMouseMove %d <%s>-%d-<%s>", m_iLastScrollOffset,
				//	gtc::format::point(event.ptMouse).c_str(), event.ptMouse.y - m_ptLastMouse.y, gtc::format::point(m_ptLastMouse).c_str());
				
				
				//if (event.ptMouse.y > m_ptLastMouse.y)
				//{
				//	if(m_pContainer) m_pContainer->lineDown(5);
				//}
				//else
				//{
				//	if(m_pContainer) m_pContainer->lineUp(5);
				//}
				//m_ptLastMouse = event.ptMouse;
				return;
			}
		}
		break;
	case BEUCEventButtonUp:
		{
			m_bMoveThumb = false;
			m_iLastScrollOffset = 0;
			//SEND_LOG(L"BEUCEventButtonUp killTimer");
			m_pwndManager->killTimer(this, DEFAULT_TIMERID);
			if( (m_uiState & UCSTATE_CAPTURED) != 0 ) {
				m_uiState &= ~( UCSTATE_CAPTURED | UCSTATE_PUSHED );
				invalidate();
				return;
			}
		}
		break;
	case BEUCEventTimer:
		{
			//SEND_LOG(L"scrollBarUC::Event BEUCEventTimer");
			if (event.wParam == DEFAULT_TIMERID && (m_uiState & UCSTATE_CAPTURED))
			{
				if( !m_bHorizontal ) {
					SIZE sz = {m_pContainer->getScrollPos().cx,  m_iLastScrollPos + m_iLastScrollOffset};
					if( m_pContainer != NULL ) m_pContainer->setScrollPos(sz); 
					else setScrollPos(m_iLastScrollPos + m_iLastScrollOffset);
				}
				else {
					SIZE sz = {m_iLastScrollPos + m_iLastScrollOffset, m_pContainer->getScrollPos().cy};
					if( m_pContainer != NULL ) m_pContainer->setScrollPos(sz); 
					else setScrollPos(m_iLastScrollPos + m_iLastScrollOffset);
				}
				invalidate();
			}
			if( !m_pContainer ) m_pwndManager->sendNotify(this, L"scroll");
		}
		break;
	}

	if(m_pContainer)
		m_pContainer->Event(event);
	else
		__super::Event(event);
}


void scrollBarUC::setScrollRange(int range)
{
	if(m_iRange == range) return;
	m_iRange = range;
	if(m_iRange < 0) m_iRange = 0;
	if(m_iScrollPos > m_iRange) m_iScrollPos = m_iRange;

	refreshLayout();
}

void scrollBarUC::setScrollPos(int pos)
{
	if(m_iScrollPos == pos) return;
	m_iScrollPos = pos;
	if(m_iScrollPos < 0) m_iScrollPos = 0;
	if(m_iScrollPos > m_iRange) m_iScrollPos = m_iRange;

	refreshLayout();
}

void scrollBarUC::refreshLayout()
{
	//SEND_LOG(L"scrollbarWnd::refreshLayout...");
	const RECT rt = m_rcItem;
	int cx = m_iBarSize;
	int cy = rt.bottom - rt.top;
	//WRITE_LOG(L"scrollBarUC::refreshLayout AAA %d", m_iRange + rt.bottom - rt.top);
	int cyThumb = cy * (rt.bottom - rt.top) / (m_iRange + rt.bottom - rt.top);
	if(cyThumb < cx*3) cyThumb = cx*3;	// 滑块最小长度
	m_rtThumb.left = rt.left;
	m_rtThumb.right = rt.left + m_iBarSize;
	if (m_iRange > 0)
	{
		m_rtThumb.top = m_iScrollPos * (cy - cyThumb) / m_iRange + rt.top;
		m_rtThumb.bottom = m_rtThumb.top + cyThumb;
		if (m_rtThumb.bottom > rt.bottom)
		{
			m_rtThumb.bottom = rt.bottom;
			m_rtThumb.top = m_rtThumb.bottom - cyThumb;
		}
	}
	else
	{
		m_rtThumb.top = rt.top;
		m_rtThumb.bottom = rt.bottom;
	}
	// 
	m_rtThumb.top += 2;
	m_rtThumb.bottom -= 2;

}
void scrollBarUC::setThumbImage(gtc::imageResource *pImage)
{
	if(m_pThumbImage == pImage) return;
	m_pThumbImage = pImage;
	invalidate();
}

}