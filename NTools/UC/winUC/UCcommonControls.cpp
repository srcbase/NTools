#include "UCcommonControls.h"


namespace wuc {


/////////////////////////////////////////////////////////////////////////////////////
//
//

labelUC::labelUC() : m_uTextStyle(DT_VCENTER), m_dwTextColor(0), 
	m_dwDisabledTextColor(0), m_iFont(-1), m_bShowHtml(false)
{
	::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
}

PCWSTR labelUC::getClass() const
{
	return L"LabelUI";
}

LPVOID labelUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Label") == 0 ) return static_cast<labelUC*>(this);
	return controlUC::getInterface(pstrName);
}

void labelUC::setTextStyle(UINT uStyle)
{
	m_uTextStyle = uStyle;
	invalidate();
}

UINT labelUC::getTextStyle() const
{
	return m_uTextStyle;
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

void labelUC::setShowHtml(bool bShowHtml)
{
	if( m_bShowHtml == bShowHtml ) return;

	m_bShowHtml = bShowHtml;
	invalidate();
}

SIZE labelUC::estimateSize(SIZE szAvailable)
{
	if( m_cxyFixed.cy == 0 ) return gtc::size(m_cxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 4);
	return controlUC::estimateSize(szAvailable);
}

void labelUC::Event(BSEventUC& event)
{
	if( event.Type == BEUCEventSetFocus ) 
	{
		m_bFocused = true;
		return;
	}
	if( event.Type == BEUCEventKillFocus ) 
	{
		m_bFocused = false;
		return;
	}
	if( event.Type == BEUCEventMouseEnter )
	{
		return;
	}
	if( event.Type == BEUCEventMouseLeave )
	{
		return;
	}
	controlUC::Event(event);
}

void labelUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"align") == 0 ) {
		if( _tcsstr(pstrValue, L"left") != NULL ) {
			m_uTextStyle &= ~(DT_CENTER | DT_RIGHT | DT_TOP | DT_BOTTOM);
			m_uTextStyle |= DT_LEFT;
		}
		if( _tcsstr(pstrValue, L"center") != NULL ) {
			m_uTextStyle &= ~(DT_LEFT | DT_RIGHT | DT_TOP | DT_BOTTOM);
			m_uTextStyle |= DT_CENTER;
		}
		if( _tcsstr(pstrValue, L"right") != NULL ) {
			m_uTextStyle &= ~(DT_LEFT | DT_CENTER | DT_TOP | DT_BOTTOM);
			m_uTextStyle |= DT_RIGHT;
		}
		if( _tcsstr(pstrValue, L"top") != NULL ) {
			m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER | DT_LEFT | DT_RIGHT);
			m_uTextStyle |= DT_TOP;
		}
		if( _tcsstr(pstrValue, L"bottom") != NULL ) {
			m_uTextStyle &= ~(DT_TOP | DT_VCENTER | DT_LEFT | DT_RIGHT);
			m_uTextStyle |= DT_BOTTOM;
		}
	}
	else if( _tcscmp(pstrName, L"endellipsis") == 0 ) {
		if( _tcscmp(pstrValue, L"true") == 0 ) m_uTextStyle |= DT_END_ELLIPSIS;
		else m_uTextStyle &= ~DT_END_ELLIPSIS;
	}    
	else if( _tcscmp(pstrName, L"font") == 0 ) setFont(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"textcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"disabledtextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setDisabledTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"textpadding") == 0 ) {
		RECT rcTextPadding = { 0 };
		PWSTR pstr = NULL;
		rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setTextPadding(rcTextPadding);
	}
	else if( _tcscmp(pstrName, L"showhtml") == 0 ) setShowHtml(_tcscmp(pstrValue, L"true") == 0);
	else controlUC::setAttribute(pstrName, pstrValue);
}

void labelUC::paintText(HDC hDC)
{
	//if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->getDefaultFontColor();
	//if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->getDefaultDisabledColor();

	//if( m_wsText.empty() ) return;
	//int nLinks = 0;
	//RECT rc = m_rcItem;
	//rc.left += m_rcTextPadding.left;
	//rc.right -= m_rcTextPadding.right;
	//rc.top += m_rcTextPadding.top;
	//rc.bottom -= m_rcTextPadding.bottom;
	//if( isEnabled() ) {
	//	if( m_bShowHtml )
	//		renderEngine::drawHtmlText(hDC, m_pManager, rc, m_wsText.c_str(), m_dwTextColor, \
	//		NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
	//	else
	//		renderEngine::drawText(hDC, m_pManager, rc, m_wsText.c_str(), m_dwTextColor, \
	//		m_iFont, DT_SINGLELINE | m_uTextStyle);
	//}
	//else {
	//	if( m_bShowHtml )
	//		renderEngine::drawHtmlText(hDC, m_pManager, rc, m_wsText.c_str(), m_dwDisabledTextColor, \
	//		NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
	//	else
	//		renderEngine::drawText(hDC, m_pManager, rc, m_wsText.c_str(), m_dwDisabledTextColor, \
	//		m_iFont, DT_SINGLELINE | m_uTextStyle);
	//}
}



/////////////////////////////////////////////////////////////////////////////////////
//
//

buttonUC::buttonUC()
	: labelUC()
	, m_uButtonState(0)
	, m_dwHotTextColor(0)
	, m_dwPushedTextColor(0)
	, m_dwFocusedTextColor(0)

{
	m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	//TRACE_CLASS_BEGIN(this)
}

PCWSTR buttonUC::getClass() const
{
	return L"ButtonUI";
}

LPVOID buttonUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Button") == 0 ) return static_cast<buttonUC*>(this);
	return labelUC::getInterface(pstrName);
}

UINT buttonUC::getControlFlags() const
{
	return UCFLAG_TABSTOP | (isEnabled() ? UCFLAG_SETCURSOR : 0);
}

void buttonUC::Event(BSEventUC& event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else labelUC::Event(event);
		return;
	}

	if( event.Type == BEUCEventSetFocus ) 
	{
		invalidate();
	}
	if( event.Type == BEUCEventKillFocus ) 
	{
		invalidate();
	}
	if( event.Type == BEUCEventKeyDown )
	{
		if( event.chKey == VK_SPACE || event.chKey == VK_RETURN ) {
			activate();
			return;
		}
	}
	if( event.Type == BEUCEventButtonDown || event.Type == BEUCEventDBLClick )
	{
		if( ::PtInRect(&m_rcItem, event.ptMouse) && isEnabled() ) {
			m_uButtonState |= UCSTATE_PUSHED | UCSTATE_CAPTURED;
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventMouseMove )
	{
		if( (m_uButtonState & UCSTATE_CAPTURED) != 0 ) {
			if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UCSTATE_PUSHED;
			else m_uButtonState &= ~UCSTATE_PUSHED;
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventButtonUp )
	{
		if( (m_uButtonState & UCSTATE_CAPTURED) != 0 ) {
			if( ::PtInRect(&m_rcItem, event.ptMouse) ) activate();
			m_uButtonState &= ~(UCSTATE_PUSHED | UCSTATE_CAPTURED);
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventContextMenu )
	{
		if( isContextMenuUsed() ) {
			m_pManager->sendNotify(this, L"menu", event.wParam, event.lParam);
		}
		return;
	}
	if( event.Type == BEUCEventMouseEnter )
	{
		if( isEnabled() ) {
			m_uButtonState |= UCSTATE_HOT;
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventMouseLeave )
	{
		if( isEnabled() ) {
			m_uButtonState &= ~UCSTATE_HOT;
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventSetFocus ) {
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		return;
	}
	labelUC::Event(event);
}

bool buttonUC::activate()
{
	if( !controlUC::activate() ) return false;
	if( m_pManager != NULL ) m_pManager->sendNotify(this, L"click");
	return true;
}

void buttonUC::setEnabled(bool bEnable)
{
	controlUC::setEnabled(bEnable);
	if( !isEnabled() ) {
		m_uButtonState = 0;
	}
}

void buttonUC::setHotTextColor(DWORD dwColor)
{
	m_dwHotTextColor = dwColor;
}

DWORD buttonUC::getHotTextColor() const
{
	return m_dwHotTextColor;
}

void buttonUC::setPushedTextColor(DWORD dwColor)
{
	m_dwPushedTextColor = dwColor;
}

DWORD buttonUC::getPushedTextColor() const
{
	return m_dwPushedTextColor;
}

void buttonUC::setFocusedTextColor(DWORD dwColor)
{
	m_dwFocusedTextColor = dwColor;
}

DWORD buttonUC::getFocusedTextColor() const
{
	return m_dwFocusedTextColor;
}

PCWSTR buttonUC::getNormalImage()
{
	return m_bsNormalImage.wsKeyImage.c_str();
}

void buttonUC::setNormalImage(PCWSTR pkeyName, const RECT *prcScr /*= nullptr*/)
{
	m_bsNormalImage.wsKeyImage = pkeyName;
	if(prcScr)
		m_bsNormalImage.rcScource = *prcScr;
	else if (gtc::image *pImage = m_pManager->findImage(m_bsNormalImage.wsKeyImage))
	{
		m_bsNormalImage.rcScource.left = m_bsNormalImage.rcScource.top = 0;
		m_bsNormalImage.rcScource.right = pImage->getWidth();
		m_bsNormalImage.rcScource.bottom = pImage->getHeight();
	}
	
	invalidate();
}

PCWSTR buttonUC::getHotImage()
{
	return m_bsHotImage.wsKeyImage.c_str();
}

void buttonUC::setHotImage(PCWSTR pkeyName, const RECT *prcScr /*= nullptr*/)
{
	m_bsHotImage.wsKeyImage = pkeyName;
	if(prcScr)
		m_bsHotImage.rcScource = *prcScr;
	else if (gtc::image *pImage = m_pManager->findImage(m_bsHotImage.wsKeyImage))
	{
		m_bsHotImage.rcScource.left = m_bsHotImage.rcScource.top = 0;
		m_bsHotImage.rcScource.right = pImage->getWidth();
		m_bsHotImage.rcScource.bottom = pImage->getHeight();
	}
	invalidate();
}

PCWSTR buttonUC::getPushedImage()
{
	return m_bsPushedImage.wsKeyImage.c_str();
}

void buttonUC::setPushedImage(PCWSTR pkeyName, const RECT *prcScr /*= nullptr*/)
{
	m_bsPushedImage.wsKeyImage = pkeyName;
	if(prcScr)
		m_bsPushedImage.rcScource = *prcScr;
	else if (gtc::image *pImage = m_pManager->findImage(m_bsPushedImage.wsKeyImage))
	{
		m_bsPushedImage.rcScource.left = m_bsPushedImage.rcScource.top = 0;
		m_bsPushedImage.rcScource.right = pImage->getWidth();
		m_bsPushedImage.rcScource.bottom = pImage->getHeight();
	}
	invalidate();
}

PCWSTR buttonUC::getFocusedImage()
{
	return m_bsFocusedImage.wsKeyImage.c_str();
}

void buttonUC::setFocusedImage(PCWSTR pkeyName, const RECT *prcScr /*= nullptr*/)
{
	m_bsFocusedImage.wsKeyImage = pkeyName;
	if(prcScr)
		m_bsFocusedImage.rcScource = *prcScr;
	else if (gtc::image *pImage = m_pManager->findImage(m_bsFocusedImage.wsKeyImage))
	{
		m_bsFocusedImage.rcScource.left = m_bsFocusedImage.rcScource.top = 0;
		m_bsFocusedImage.rcScource.right = pImage->getWidth();
		m_bsFocusedImage.rcScource.bottom = pImage->getHeight();
	}
	invalidate();
}

PCWSTR buttonUC::getDisabledImage()
{
	return m_bsDisabledImage.wsKeyImage.c_str();
}

void buttonUC::setDisabledImage(PCWSTR pkeyName, const RECT *prcScr /*= nullptr*/)
{
	m_bsDisabledImage.wsKeyImage = pkeyName;
	if(prcScr)
		m_bsDisabledImage.rcScource = *prcScr;
	else if (gtc::image *pImage = m_pManager->findImage(m_bsDisabledImage.wsKeyImage))
	{
		m_bsDisabledImage.rcScource.left = m_bsDisabledImage.rcScource.top = 0;
		m_bsDisabledImage.rcScource.right = pImage->getWidth();
		m_bsDisabledImage.rcScource.bottom = pImage->getHeight();
	}
	invalidate();
}

SIZE buttonUC::estimateSize(SIZE szAvailable)
{
	if( m_cxyFixed.cy == 0 ) return gtc::size(m_cxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 8);
	return controlUC::estimateSize(szAvailable);
}

void buttonUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"normalimage") == 0 ) setNormalImage(pstrValue);
	else if( _tcscmp(pstrName, L"hotimage") == 0 ) setHotImage(pstrValue);
	else if( _tcscmp(pstrName, L"pushedimage") == 0 ) setPushedImage(pstrValue);
	else if( _tcscmp(pstrName, L"focusedimage") == 0 ) setFocusedImage(pstrValue);
	else if( _tcscmp(pstrName, L"disabledimage") == 0 ) setDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, L"hottextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setHotTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"pushedtextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setPushedTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"focusedtextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setFocusedTextColor(clrColor);
	}
	else labelUC::setAttribute(pstrName, pstrValue);
}

void buttonUC::paintText(HDC hDC)
{
	/*if( isFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !isEnabled() ) m_uButtonState |= UISTATE_DISABLED;
	else m_uButtonState &= ~ UISTATE_DISABLED;

	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->getDefaultFontColor();
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->getDefaultDisabledColor();

	if( m_sText.empty() ) return;
	int nLinks = 0;
	RECT rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;

	DWORD clrColor = isEnabled()?m_dwTextColor:m_dwDisabledTextColor;

	if( ((m_uButtonState & UISTATE_PUSHED) != 0) && (getPushedTextColor() != 0) )
		clrColor = getPushedTextColor();
	else if( ((m_uButtonState & UISTATE_HOT) != 0) && (getHotTextColor() != 0) )
		clrColor = getHotTextColor();
	else if( ((m_uButtonState & UISTATE_FOCUSED) != 0) && (getFocusedTextColor() != 0) )
		clrColor = getFocusedTextColor();

	if( m_bShowHtml )
		renderEngine::drawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), clrColor, \
		NULL, NULL, nLinks, m_uTextStyle);
	else
		renderEngine::drawText(hDC, m_pManager, rc, m_sText.c_str(), clrColor, \
		m_iFont, m_uTextStyle);*/
}

void buttonUC::paintStatusImage(HDC hDC)
{
	if( isFocused() ) m_uButtonState |= UCSTATE_FOCUSED;
	else m_uButtonState &= ~ UCSTATE_FOCUSED;
	if( !isEnabled() ) m_uButtonState |= UCSTATE_DISABLED;
	else m_uButtonState &= ~ UCSTATE_DISABLED;

	if( (m_uButtonState & UCSTATE_DISABLED) != 0 ) {
		if(drawImage(hDC, m_bsDisabledImage)) return;
		//if( !m_sDisabledImage.empty() ) {
		//	if( !drawImage(hDC, (PCWSTR)m_sDisabledImage.c_str()) ) m_sDisabledImage.clear();
		//	else return;
		//}
	}
	else if( (m_uButtonState & UCSTATE_PUSHED) != 0 ) {
		if(drawImage(hDC, m_bsPushedImage)) return;
		//if( !m_sPushedImage.empty() ) {
		//	if( !drawImage(hDC, (PCWSTR)m_sPushedImage.c_str()) ) m_sPushedImage.clear();
		//	else return;
		//}
	}
	else if( (m_uButtonState & UCSTATE_HOT) != 0 ) {
		if(drawImage(hDC, m_bsHotImage)) return;
		//if( !m_sHotImage.empty() ) {
		//	if( !drawImage(hDC, (PCWSTR)m_sHotImage.c_str()) ) m_sHotImage.clear();
		//	else return;
		//}
	}
	else if( (m_uButtonState & UCSTATE_FOCUSED) != 0 ) {
		if(drawImage(hDC, m_bsFocusedImage)) return;
		//if( !m_sFocusedImage.empty() ) {
		//	if( !drawImage(hDC, (PCWSTR)m_sFocusedImage.c_str()) ) m_sFocusedImage.clear();
		//	else return;
		//}
	}

	drawImage(hDC, m_bsNormalImage);
	//if( !m_sNormalImage.empty() ) {
	//	if( !drawImage(hDC, (PCWSTR)m_sNormalImage.c_str()) ) m_sNormalImage.clear();
	//	else return;
	//}
}


}