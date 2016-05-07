#include "StdAfx.h"
#include "commonControls.h"


namespace wui {

/////////////////////////////////////////////////////////////////////////////////////
//
//

labelUI::labelUI() : m_uTextStyle(DT_VCENTER), m_dwTextColor(0), 
	m_dwDisabledTextColor(0), m_iFont(-1), m_bShowHtml(false)
{
	::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
}

PCWSTR labelUI::getClass() const
{
	return L"LabelUI";
}

LPVOID labelUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Label") == 0 ) return static_cast<labelUI*>(this);
	return controlUI::getInterface(pstrName);
}

void labelUI::setTextStyle(UINT uStyle)
{
	m_uTextStyle = uStyle;
	invalidate();
}

UINT labelUI::getTextStyle() const
{
	return m_uTextStyle;
}

void labelUI::setTextColor(DWORD dwTextColor)
{
	m_dwTextColor = dwTextColor;
}

DWORD labelUI::getTextColor() const
{
	return m_dwTextColor;
}

void labelUI::setDisabledTextColor(DWORD dwTextColor)
{
	m_dwDisabledTextColor = dwTextColor;
}

DWORD labelUI::getDisabledTextColor() const
{
	return m_dwDisabledTextColor;
}

void labelUI::setFont(int index)
{
	m_iFont = index;
}

int labelUI::getFont() const
{
	return m_iFont;
}

RECT labelUI::getTextPadding() const
{
	return m_rcTextPadding;
}

void labelUI::setTextPadding(RECT rc)
{
	m_rcTextPadding = rc;
	invalidate();
}

bool labelUI::isShowHtml()
{
	return m_bShowHtml;
}

void labelUI::setShowHtml(bool bShowHtml)
{
	if( m_bShowHtml == bShowHtml ) return;

	m_bShowHtml = bShowHtml;
	invalidate();
}

SIZE labelUI::estimateSize(SIZE szAvailable)
{
	if( m_cxyFixed.cy == 0 ) return size(m_cxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 4);
	return controlUI::estimateSize(szAvailable);
}

void labelUI::doEvent(TEventUI& event)
{
	if( event.Type == UIEVENT_SETFOCUS ) 
	{
		m_bFocused = true;
		return;
	}
	if( event.Type == UIEVENT_KILLFOCUS ) 
	{
		m_bFocused = false;
		return;
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		return;
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		return;
	}
	controlUI::doEvent(event);
}

void labelUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
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
	else controlUI::setAttribute(pstrName, pstrValue);
}

void labelUI::paintText(HDC hDC)
{
	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->getDefaultFontColor();
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->getDefaultDisabledColor();

	if( m_sText.empty() ) return;
	int nLinks = 0;
	RECT rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;
	if( isEnabled() ) {
		if( m_bShowHtml )
			renderEngine::drawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), m_dwTextColor, \
			NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
		else
			renderEngine::drawText(hDC, m_pManager, rc, m_sText.c_str(), m_dwTextColor, \
			m_iFont, DT_SINGLELINE | m_uTextStyle);
	}
	else {
		if( m_bShowHtml )
			renderEngine::drawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), m_dwDisabledTextColor, \
			NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
		else
			renderEngine::drawText(hDC, m_pManager, rc, m_sText.c_str(), m_dwDisabledTextColor, \
			m_iFont, DT_SINGLELINE | m_uTextStyle);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

buttonUI::buttonUI() : m_uButtonState(0), m_dwHotTextColor(0), m_dwPushedTextColor(0), m_dwFocusedTextColor(0)

{
	m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	//TRACE_CLASS_BEGIN(this)
}

PCWSTR buttonUI::getClass() const
{
	return L"ButtonUI";
}

LPVOID buttonUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Button") == 0 ) return static_cast<buttonUI*>(this);
	return labelUI::getInterface(pstrName);
}

UINT buttonUI::getControlFlags() const
{
	return UIFLAG_TABSTOP | (isEnabled() ? UIFLAG_SETCURSOR : 0);
}

void buttonUI::doEvent(TEventUI& event)
{
	if( !isMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->doEvent(event);
		else labelUI::doEvent(event);
		return;
	}

	if( event.Type == UIEVENT_SETFOCUS ) 
	{
		invalidate();
	}
	if( event.Type == UIEVENT_KILLFOCUS ) 
	{
		invalidate();
	}
	if( event.Type == UIEVENT_KEYDOWN )
	{
		if( event.chKey == VK_SPACE || event.chKey == VK_RETURN ) {
			activate();
			return;
		}
	}
	if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
	{
		if( ::PtInRect(&m_rcItem, event.ptMouse) && isEnabled() ) {
			m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
			invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSEMOVE )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
			else m_uButtonState &= ~UISTATE_PUSHED;
			invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			if( ::PtInRect(&m_rcItem, event.ptMouse) ) activate();
			m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
			invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_CONTEXTMENU )
	{
		if( isContextMenuUsed() ) {
			m_pManager->sendNotify(this, L"menu", event.wParam, event.lParam);
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		if( isEnabled() ) {
			m_uButtonState |= UISTATE_HOT;
			invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		if( isEnabled() ) {
			m_uButtonState &= ~UISTATE_HOT;
			invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_SETCURSOR ) {
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		return;
	}
	labelUI::doEvent(event);
}

bool buttonUI::activate()
{
	if( !controlUI::activate() ) return false;
	if( m_pManager != NULL ) m_pManager->sendNotify(this, L"click");
	return true;
}

void buttonUI::setEnabled(bool bEnable)
{
	controlUI::setEnabled(bEnable);
	if( !isEnabled() ) {
		m_uButtonState = 0;
	}
}

void buttonUI::setHotTextColor(DWORD dwColor)
{
	m_dwHotTextColor = dwColor;
}

DWORD buttonUI::getHotTextColor() const
{
	return m_dwHotTextColor;
}

void buttonUI::setPushedTextColor(DWORD dwColor)
{
	m_dwPushedTextColor = dwColor;
}

DWORD buttonUI::getPushedTextColor() const
{
	return m_dwPushedTextColor;
}

void buttonUI::setFocusedTextColor(DWORD dwColor)
{
	m_dwFocusedTextColor = dwColor;
}

DWORD buttonUI::getFocusedTextColor() const
{
	return m_dwFocusedTextColor;
}

PCWSTR buttonUI::getNormalImage()
{
	return m_sNormalImage.c_str();
}

void buttonUI::setNormalImage(PCWSTR pStrImage)
{
	m_sNormalImage = pStrImage;
	invalidate();
}

PCWSTR buttonUI::getHotImage()
{
	return m_sHotImage.c_str();
}

void buttonUI::setHotImage(PCWSTR pStrImage)
{
	m_sHotImage = pStrImage;
	invalidate();
}

PCWSTR buttonUI::getPushedImage()
{
	return m_sPushedImage.c_str();
}

void buttonUI::setPushedImage(PCWSTR pStrImage)
{
	m_sPushedImage = pStrImage;
	invalidate();
}

PCWSTR buttonUI::getFocusedImage()
{
	return m_sFocusedImage.c_str();
}

void buttonUI::setFocusedImage(PCWSTR pStrImage)
{
	m_sFocusedImage = pStrImage;
	invalidate();
}

PCWSTR buttonUI::getDisabledImage()
{
	return m_sDisabledImage.c_str();
}

void buttonUI::setDisabledImage(PCWSTR pStrImage)
{
	m_sDisabledImage = pStrImage;
	invalidate();
}

SIZE buttonUI::estimateSize(SIZE szAvailable)
{
	if( m_cxyFixed.cy == 0 ) return size(m_cxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 8);
	return controlUI::estimateSize(szAvailable);
}

void buttonUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
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
	else labelUI::setAttribute(pstrName, pstrValue);
}

void buttonUI::paintText(HDC hDC)
{
	if( isFocused() ) m_uButtonState |= UISTATE_FOCUSED;
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
		m_iFont, m_uTextStyle);
}

void buttonUI::paintStatusImage(HDC hDC)
{
	if( isFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;
	if( !isEnabled() ) m_uButtonState |= UISTATE_DISABLED;
	else m_uButtonState &= ~ UISTATE_DISABLED;

	if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
		if( !m_sDisabledImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sDisabledImage.c_str()) ) m_sDisabledImage.clear();
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
		if( !m_sPushedImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sPushedImage.c_str()) ) m_sPushedImage.clear();
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( !m_sHotImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sHotImage.c_str()) ) m_sHotImage.clear();
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
		if( !m_sFocusedImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sFocusedImage.c_str()) ) m_sFocusedImage.clear();
			else return;
		}
	}

	if( !m_sNormalImage.empty() ) {
		if( !drawImage(hDC, (PCWSTR)m_sNormalImage.c_str()) ) m_sNormalImage.clear();
		else return;
	}
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

optionUI::optionUI() : m_bSelected(false), m_dwSelectedTextColor(0)
{
	//TRACE_CLASS_BEGIN(this)
}

optionUI::~optionUI()
{
	//TRACE_CLASS_END(this)
	if( !m_sGroupName.empty() && m_pManager ) m_pManager->removeOptionGroup(m_sGroupName.c_str(), this);
}

PCWSTR optionUI::getClass() const
{
	return L"OptionUI";
}

LPVOID optionUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Option") == 0 ) return static_cast<optionUI*>(this);
	return buttonUI::getInterface(pstrName);
}

void optionUI::setManager(paintManagerUI* pManager, controlUI* pParent, bool bInit)
{
	controlUI::setManager(pManager, pParent, bInit);
	if( bInit && !m_sGroupName.empty() ) {
		if (m_pManager) m_pManager->addOptionGroup(m_sGroupName.c_str(), this);
	}
}

PCWSTR optionUI::getGroup() const
{
	return m_sGroupName.c_str();
}

void optionUI::setGroup(PCWSTR pStrGroupName)
{
	if( pStrGroupName == NULL ) {
		if( m_sGroupName.empty() ) return;
		m_sGroupName.clear();
	}
	else {
		if( m_sGroupName == pStrGroupName ) return;
		if (!m_sGroupName.empty() && m_pManager) m_pManager->removeOptionGroup(m_sGroupName.c_str(), this);
		m_sGroupName = pStrGroupName;
	}

	if( !m_sGroupName.empty() ) {
		if (m_pManager) m_pManager->addOptionGroup(m_sGroupName.c_str(), this);
	}
	else {
		if (m_pManager) m_pManager->removeOptionGroup(m_sGroupName.c_str(), this);
	}

	selected(m_bSelected);
}

bool optionUI::isSelected() const
{
	return m_bSelected;
}

void optionUI::selected(bool bSelected)
{
	if( m_bSelected == bSelected ) return;
	m_bSelected = bSelected;
	if( m_bSelected ) m_uButtonState |= UISTATE_SELECTED;
	else m_uButtonState &= ~UISTATE_SELECTED;

	if( m_pManager != NULL ) {
		if( !m_sGroupName.empty() ) {
			if( m_bSelected ) {
				std::vector<controlUI*>* aOptionGroup = m_pManager->getOptionGroup(m_sGroupName.c_str());
				for( int i = 0; i < aOptionGroup->size(); i++ ) {
					optionUI* pControl = static_cast<optionUI*>(aOptionGroup->at(i));
					if( pControl != this ) {
						pControl->selected(false);
					}
				}
				m_pManager->sendNotify(this, L"selectchanged");
			}
		}
		else {
			m_pManager->sendNotify(this, L"selectchanged");
		}
	}

	invalidate();
}

bool optionUI::activate()
{
	if( !buttonUI::activate() ) return false;
	if( !m_sGroupName.empty() ) selected(true);
	else selected(!m_bSelected);

	return true;
}

void optionUI::setEnabled(bool bEnable)
{
	controlUI::setEnabled(bEnable);
	if( !isEnabled() ) {
		if( m_bSelected ) m_uButtonState = UISTATE_SELECTED;
		else m_uButtonState = 0;
	}
}

PCWSTR optionUI::getSelectedImage()
{
	return m_sSelectedImage.c_str();
}

void optionUI::setSelectedImage(PCWSTR pStrImage)
{
	m_sSelectedImage = pStrImage;
	invalidate();
}

void optionUI::setSelectedTextColor(DWORD dwTextColor)
{
	m_dwSelectedTextColor = dwTextColor;
}

DWORD optionUI::getSelectedTextColor()
{
	if (m_dwSelectedTextColor == 0) m_dwSelectedTextColor = m_pManager->getDefaultFontColor();
	return m_dwSelectedTextColor;
}

PCWSTR optionUI::getForeImage()
{
	return m_sForeImage.c_str();
}

void optionUI::setForeImage(PCWSTR pStrImage)
{
	m_sForeImage = pStrImage;
	invalidate();
}

SIZE optionUI::estimateSize(SIZE szAvailable)
{
	if( m_cxyFixed.cy == 0 ) return size(m_cxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 8);
	return controlUI::estimateSize(szAvailable);
}

void optionUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"group") == 0 ) setGroup(pstrValue);
	else if( _tcscmp(pstrName, L"selected") == 0 ) selected(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"selectedimage") == 0 ) setSelectedImage(pstrValue);
	else if( _tcscmp(pstrName, L"foreimage") == 0 ) setForeImage(pstrValue);
	else if( _tcscmp(pstrName, L"selectedtextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setSelectedTextColor(clrColor);
	}
	else buttonUI::setAttribute(pstrName, pstrValue);
}

void optionUI::paintStatusImage(HDC hDC)
{
	m_uButtonState &= ~UISTATE_PUSHED;

	if( (m_uButtonState & UISTATE_SELECTED) != 0 ) {
		if( !m_sSelectedImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sSelectedImage.c_str()) ) m_sSelectedImage.clear();
			else goto Label_ForeImage;
		}
	}

	buttonUI::paintStatusImage(hDC);

Label_ForeImage:
	if( !m_sForeImage.empty() ) {
		if( !drawImage(hDC, (PCWSTR)m_sForeImage.c_str()) ) m_sForeImage.clear();
	}
}

void optionUI::paintText(HDC hDC)
{
	if( (m_uButtonState & UISTATE_SELECTED) != 0 )
	{
		DWORD oldTextColor = m_dwTextColor;
		if( m_dwSelectedTextColor != 0 ) m_dwTextColor = m_dwSelectedTextColor;

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->getDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->getDefaultDisabledColor();

		if( m_sText.empty() ) return;
		int nLinks = 0;
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		if( m_bShowHtml )
			renderEngine::drawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), isEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
			NULL, NULL, nLinks, m_uTextStyle);
		else
			renderEngine::drawText(hDC, m_pManager, rc, m_sText.c_str(), isEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
			m_iFont, m_uTextStyle);

		m_dwTextColor = oldTextColor;
	}
	else
		buttonUI::paintText(hDC);
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

textUI::textUI() : m_nLinks(0), m_nHoverLink(-1)
{
	//TRACE_CLASS_BEGIN(this)
	m_uTextStyle = DT_WORDBREAK;
	m_rcTextPadding.left = 2;
	m_rcTextPadding.right = 2;
	::ZeroMemory(m_rcLinks, sizeof(m_rcLinks));
}

textUI::~textUI()
{
	//TRACE_CLASS_END(this)
}

PCWSTR textUI::getClass() const
{
	return L"TextUI";
}

LPVOID textUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Text") == 0 ) return static_cast<textUI*>(this);
	return labelUI::getInterface(pstrName);
}

UINT textUI::getControlFlags() const
{
	if( isEnabled() && m_nLinks > 0 ) return UIFLAG_SETCURSOR;
	else return 0;
}

std::wstring* textUI::getLinkContent(int iIndex)
{
	if( iIndex >= 0 && iIndex < m_nLinks ) return &m_sLinks[iIndex];
	return NULL;
}

void textUI::doEvent(TEventUI& event)
{
	if( !isMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->doEvent(event);
		else labelUI::doEvent(event);
		return;
	}

	if( event.Type == UIEVENT_SETCURSOR ) {
		for( int i = 0; i < m_nLinks; i++ ) {
			if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
				return;
			}
		}
	}
	if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK && isEnabled() ) {
		for( int i = 0; i < m_nLinks; i++ ) {
			if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
				invalidate();
				return;
			}
		}
	}
	if( event.Type == UIEVENT_BUTTONUP && isEnabled() ) {
		for( int i = 0; i < m_nLinks; i++ ) {
			if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
				m_pManager->sendNotify(this, L"link", i);
				return;
			}
		}
	}
	if( event.Type == UIEVENT_CONTEXTMENU )
	{
		return;
	}
	// When you move over a link
	if( m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE && isEnabled() ) {
		int nHoverLink = -1;
		for( int i = 0; i < m_nLinks; i++ ) {
			if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
				nHoverLink = i;
				break;
			}
		}

		if(m_nHoverLink != nHoverLink) {
			m_nHoverLink = nHoverLink;
			invalidate();
			return;
		}      
	}
	if( event.Type == UIEVENT_MOUSELEAVE ) {
		if( m_nLinks > 0 && isEnabled() ) {
			if(m_nHoverLink != -1) {
				m_nHoverLink = -1;
				invalidate();
				return;
			}
		}
	}

	labelUI::doEvent(event);
}

SIZE textUI::estimateSize(SIZE szAvailable)
{
	RECT rcText = { 0, 0, max(szAvailable.cx, m_cxyFixed.cx), 9999 };
	rcText.left += m_rcTextPadding.left;
	rcText.right -= m_rcTextPadding.right;
	if( m_bShowHtml ) {   
		int nLinks = 0;
		renderEngine::drawHtmlText(m_pManager->getPaintDC(), m_pManager, rcText, m_sText.c_str(), m_dwTextColor, NULL, NULL, nLinks, DT_CALCRECT | m_uTextStyle);
	}
	else {
		renderEngine::drawText(m_pManager->getPaintDC(), m_pManager, rcText, m_sText.c_str(), m_dwTextColor, m_iFont, DT_CALCRECT | m_uTextStyle);
	}
	SIZE cXY = {rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right,
		rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom};

	if( m_cxyFixed.cy != 0 ) cXY.cy = m_cxyFixed.cy;
	return cXY;
}

void textUI::paintText(HDC hDC)
{
	if( m_sText.empty() ) {
		m_nLinks = 0;
		return;
	}

	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->getDefaultFontColor();
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->getDefaultDisabledColor();

	if( m_sText.empty() ) return;

	m_nLinks = _countof(m_rcLinks);
	RECT rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;
	if( isEnabled() ) {
		if( m_bShowHtml )
			renderEngine::drawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), m_dwTextColor, \
			m_rcLinks, m_sLinks, m_nLinks, m_uTextStyle);
		else
			renderEngine::drawText(hDC, m_pManager, rc, m_sText.c_str(), m_dwTextColor, \
			m_iFont, m_uTextStyle);
	}
	else {
		if( m_bShowHtml )
			renderEngine::drawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), m_dwDisabledTextColor, \
			m_rcLinks, m_sLinks, m_nLinks, m_uTextStyle);
		else
			renderEngine::drawText(hDC, m_pManager, rc, m_sText.c_str(), m_dwDisabledTextColor, \
			m_iFont, m_uTextStyle);
	}
}

void textUI::setText(PCWSTR pstrText)
{
	__super::setText(pstrText);
	needParentUpdate();
}
/////////////////////////////////////////////////////////////////////////////////////
//
//

progressUI::progressUI() : m_bHorizontal(true), m_nMin(0), m_nMax(100), m_nValue(0)
{
	//TRACE_CLASS_BEGIN(this)
	m_uTextStyle = DT_SINGLELINE | DT_CENTER;
	setFixedHeight(12);
}

PCWSTR progressUI::getClass() const
{
	return L"ProgressUI";
}

LPVOID progressUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Progress") == 0 ) return static_cast<progressUI*>(this);
	return labelUI::getInterface(pstrName);
}

bool progressUI::isHorizontal()
{
	return m_bHorizontal;
}

void progressUI::setHorizontal(bool bHorizontal)
{
	if( m_bHorizontal == bHorizontal ) return;

	m_bHorizontal = bHorizontal;
	invalidate();
}

int progressUI::getMinValue() const
{
	return m_nMin;
}

void progressUI::setMinValue(int nMin)
{
	m_nMin = nMin;
	invalidate();
}

int progressUI::getMaxValue() const
{
	return m_nMax;
}

void progressUI::setMaxValue(int nMax)
{
	m_nMax = nMax;
	invalidate();
}

int progressUI::getValue() const
{
	return m_nValue;
}

void progressUI::setValue(int nValue)
{
	m_nValue = nValue;
	invalidate();
}

PCWSTR progressUI::getFgImage() const
{
	return m_sFgImage.c_str();
}

void progressUI::setFgImage(PCWSTR pStrImage)
{
	if( m_sFgImage == pStrImage ) return;

	m_sFgImage = pStrImage;
	invalidate();
}

void progressUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"fgimage") == 0 ) setFgImage(pstrValue);
	else if( _tcscmp(pstrName, L"hor") == 0 ) setHorizontal(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"min") == 0 ) setMinValue(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"max") == 0 ) setMaxValue(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"value") == 0 ) setValue(_ttoi(pstrValue));
	else labelUI::setAttribute(pstrName, pstrValue);
}

void progressUI::paintStatusImage(HDC hDC)
{
	if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
	if( m_nValue > m_nMax ) m_nValue = m_nMax;
	if( m_nValue < m_nMin ) m_nValue = m_nMin;

	RECT rc = {0};
	if( m_bHorizontal ) {
		rc.right = (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
		rc.bottom = m_rcItem.bottom - m_rcItem.top;
	}
	else {
		rc.top = (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
		rc.right = m_rcItem.right - m_rcItem.left;
		rc.bottom = m_rcItem.bottom - m_rcItem.top;
	}

	if( !m_sFgImage.empty() ) {
		m_sFgImageModify.clear();
		m_sFgImageModify = smallFormat(L"dest='%d,%d,%d,%d'", rc.left, rc.top, rc.right, rc.bottom);

		if( !drawImage(hDC, (PCWSTR)m_sFgImage.c_str(), (PCWSTR)m_sFgImageModify.c_str()) ) m_sFgImage.clear();
		else return;
	}
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

sliderUI::sliderUI() : m_uButtonState(0), m_nStep(1)
{
	//TRACE_CLASS_BEGIN(this)
	m_uTextStyle = DT_SINGLELINE | DT_CENTER;
	m_szThumb.cx = m_szThumb.cy = 10;
}

PCWSTR sliderUI::getClass() const
{
	return L"SliderUI";
}

UINT sliderUI::getControlFlags() const
{
	if( isEnabled() ) return UIFLAG_SETCURSOR;
	else return 0;
}

LPVOID sliderUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Slider") == 0 ) return static_cast<sliderUI*>(this);
	return progressUI::getInterface(pstrName);
}

void sliderUI::setEnabled(bool bEnable)
{
	controlUI::setEnabled(bEnable);
	if( !isEnabled() ) {
		m_uButtonState = 0;
	}
}

int sliderUI::getChangeStep()
{
	return m_nStep;
}

void sliderUI::setChangeStep(int step)
{
	m_nStep = step;
}

void sliderUI::setThumbSize(SIZE szXY)
{
	m_szThumb = szXY;
}

RECT sliderUI::getThumbRect() const
{
	if( m_bHorizontal ) {
		int left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_szThumb.cx) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
		int top = (m_rcItem.bottom + m_rcItem.top - m_szThumb.cy) / 2;
		return rect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
	}
	else {
		int left = (m_rcItem.right + m_rcItem.left - m_szThumb.cx) / 2;
		int top = m_rcItem.bottom - m_szThumb.cy - (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
		return rect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
	}
}

PCWSTR sliderUI::getThumbImage() const
{
	return m_sThumbImage.c_str();
}

void sliderUI::setThumbImage(PCWSTR pStrImage)
{
	m_sThumbImage = pStrImage;
	invalidate();
}

PCWSTR sliderUI::getThumbHotImage() const
{
	return m_sThumbHotImage.c_str();
}

void sliderUI::setThumbHotImage(PCWSTR pStrImage)
{
	m_sThumbHotImage = pStrImage;
	invalidate();
}

PCWSTR sliderUI::getThumbPushedImage() const
{
	return m_sThumbPushedImage.c_str();
}

void sliderUI::setThumbPushedImage(PCWSTR pStrImage)
{
	m_sThumbPushedImage = pStrImage;
	invalidate();
}

void sliderUI::doEvent(TEventUI& event)
{
	if( !isMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->doEvent(event);
		else progressUI::doEvent(event);
		return;
	}

	if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
	{
		if( isEnabled() ) {
			RECT rcThumb = getThumbRect();
			if( ::PtInRect(&rcThumb, event.ptMouse) ) {
				m_uButtonState |= UISTATE_CAPTURED;
			}
		}
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			m_uButtonState &= ~UISTATE_CAPTURED;
		}
		if( m_bHorizontal ) {
			if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) m_nValue = m_nMax;
			else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) m_nValue = m_nMin;
			else m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
		}
		else {
			if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) m_nValue = m_nMin;
			else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) m_nValue = m_nMax;
			else m_nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
		}
		m_pManager->sendNotify(this, L"valuechanged");
		invalidate();
		return;
	}
	if( event.Type == UIEVENT_CONTEXTMENU )
	{
		return;
	}
	if( event.Type == UIEVENT_SCROLLWHEEL ) 
	{
		switch( LOWORD(event.wParam) ) {
		case SB_LINEUP:
			setValue(getValue() + getChangeStep());
			m_pManager->sendNotify(this, L"valuechanged");
			return;
		case SB_LINEDOWN:
			setValue(getValue() - getChangeStep());
			m_pManager->sendNotify(this, L"valuechanged");
			return;
		}
	}
	if( event.Type == UIEVENT_MOUSEMOVE )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			if( m_bHorizontal ) {
				if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) m_nValue = m_nMax;
				else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) m_nValue = m_nMin;
				else m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
			}
			else {
				if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) m_nValue = m_nMin;
				else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) m_nValue = m_nMax;
				else m_nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
			}
			invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_SETCURSOR )
	{
		RECT rcThumb = getThumbRect();
		if( isEnabled() && ::PtInRect(&rcThumb, event.ptMouse) ) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		if( isEnabled() ) {
			m_uButtonState |= UISTATE_HOT;
			invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		if( isEnabled() ) {
			m_uButtonState &= ~UISTATE_HOT;
			invalidate();
		}
		return;
	}
	controlUI::doEvent(event);
}


void sliderUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"thumbimage") == 0 ) setThumbImage(pstrValue);
	else if( _tcscmp(pstrName, L"thumbhotimage") == 0 ) setThumbHotImage(pstrValue);
	else if( _tcscmp(pstrName, L"thumbpushedimage") == 0 ) setThumbPushedImage(pstrValue);
	else if( _tcscmp(pstrName, L"thumbsize") == 0 ) {
		SIZE szXY = {0};
		PWSTR pstr = NULL;
		szXY.cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		szXY.cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr); 
		setThumbSize(szXY);
	}
	else if( _tcscmp(pstrName, L"step") == 0 ) {
		setChangeStep(_ttoi(pstrValue));
	}
	else progressUI::setAttribute(pstrName, pstrValue);
}

void sliderUI::paintStatusImage(HDC hDC)
{
	progressUI::paintStatusImage(hDC);

	RECT rcThumb = getThumbRect();
	rcThumb.left -= m_rcItem.left;
	rcThumb.top -= m_rcItem.top;
	rcThumb.right -= m_rcItem.left;
	rcThumb.bottom -= m_rcItem.top;
	if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
		if( !m_sThumbPushedImage.empty() ) {
			m_sImageModify.clear();
			m_sImageModify = smallFormat(L"dest='%d,%d,%d,%d'", rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
			if( !drawImage(hDC, (PCWSTR)m_sThumbPushedImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sThumbPushedImage.clear();
			else return;
		}
	}
	else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( !m_sThumbHotImage.empty() ) {
			m_sImageModify.clear();
			m_sImageModify = smallFormat(L"dest='%d,%d,%d,%d'", rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
			if( !drawImage(hDC, (PCWSTR)m_sThumbHotImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sThumbHotImage.clear();
			else return;
		}
	}

	if( !m_sThumbImage.empty() ) {
		m_sImageModify.clear();
		m_sImageModify = smallFormat(L"dest='%d,%d,%d,%d'", rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
		if( !drawImage(hDC, (PCWSTR)m_sThumbImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sThumbImage.clear();
		else return;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//d
//


editWnd::editWnd() : m_pOwner(NULL), m_hBkBrush(NULL)
{
	//TRACE_CLASS_BEGIN(this)
}

void editWnd::init(editUI* pOwner)
{
	m_pOwner = pOwner;
	RECT rcPos = calPos();
	UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;
	if( m_pOwner->isPasswordMode() ) uStyle |= ES_PASSWORD;
	create(m_pOwner->getManager()->getPaintWindow(), NULL, uStyle, 0, rcPos);
	SetWindowFont(m_hWnd, m_pOwner->getManager()->getDefaultFontInfo()->hFont, TRUE);
	Edit_LimitText(m_hWnd, m_pOwner->getMaxChar());
	if( m_pOwner->isPasswordMode() ) Edit_SetPasswordChar(m_hWnd, m_pOwner->getPasswordChar());
	Edit_SetText(m_hWnd, m_pOwner->getText().c_str());
	Edit_SetModify(m_hWnd, FALSE);
	sendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
	Edit_Enable(m_hWnd, m_pOwner->isEnabled() == true);
	Edit_SetReadOnly(m_hWnd, m_pOwner->isReadOnly() == true);
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	::SetFocus(m_hWnd);
}

RECT editWnd::calPos()
{
	rect rcPos = m_pOwner->getPos();
	RECT rcInset = m_pOwner->getTextPadding();
	rcPos.left += rcInset.left;
	rcPos.top += rcInset.top;
	rcPos.right -= rcInset.right;
	rcPos.bottom -= rcInset.bottom;
	LONG lEditHeight = m_pOwner->getManager()->getDefaultFontInfo()->tm.tmHeight;
	if( lEditHeight < rcPos.height() ) {
		rcPos.top += (rcPos.height() - lEditHeight) / 2;
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
	else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) {
		if( m_pOwner->getNativeEditBkColor() == 0xFFFFFFFF ) return NULL;
		::SetBkMode((HDC)wParam, TRANSPARENT);
		DWORD dwTextColor = m_pOwner->getTextColor();
		::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
		if( m_hBkBrush == NULL ) {
			DWORD clrColor = m_pOwner->getNativeEditBkColor();
			m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
		}
		return (LRESULT)m_hBkBrush;
	}
	else bHandled = FALSE;
	if( !bHandled ) return windowWnd::handleMessage(uMsg, wParam, lParam);
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
	m_pOwner->m_sText = pstr;
	m_pOwner->getManager()->sendNotify(m_pOwner, L"textchanged");
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

editUI::editUI() : m_pWindow(NULL), m_uMaxChar(255), m_bReadOnly(false), 
	m_bPasswordMode(false), m_cPasswordChar(L'*'), m_uButtonState(0), m_dwEditbkColor(0xFFFFFFFF)
{
	//TRACE_CLASS_BEGIN(this)
	setTextPadding(rect(4, 3, 4, 3));
	setBkColor(0xFFFFFFFF);
}

PCWSTR editUI::getClass() const
{
	return L"EditUI";
}

LPVOID editUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Edit") == 0 ) return static_cast<editUI*>(this);
	return labelUI::getInterface(pstrName);
}

UINT editUI::getControlFlags() const
{
	if( !isEnabled() ) return controlUI::getControlFlags();

	return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

void editUI::doEvent(TEventUI& event)
{
	if( !isMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->doEvent(event);
		else labelUI::doEvent(event);
		return;
	}

	if( event.Type == UIEVENT_SETCURSOR && isEnabled() )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
		return;
	}
	if( event.Type == UIEVENT_WINDOWSIZE )
	{
		if( m_pWindow != NULL ) m_pManager->setFocusNeeded(this);
	}
	if( event.Type == UIEVENT_SCROLLWHEEL )
	{
		if( m_pWindow != NULL ) return;
	}
	if( event.Type == UIEVENT_SETFOCUS && isEnabled() ) 
	{
		if( m_pWindow ) return;
		m_pWindow = new editWnd();
		assert(m_pWindow);
		m_pWindow->init(this);
		invalidate();
	}
	if( event.Type == UIEVENT_KILLFOCUS && isEnabled() ) 
	{
		invalidate();
	}
	if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
	{
		if( isEnabled() ) {
			getManager()->releaseCapture();
			if( isFocused() && m_pWindow == NULL )
			{
				m_pWindow = new editWnd();
				assert(m_pWindow);
				m_pWindow->init(this);

				if( PtInRect(&m_rcItem, event.ptMouse) )
				{
					int nSize = GetWindowTextLength(*m_pWindow);
					if( nSize == 0 )
						nSize = 1;

					Edit_SetSel(*m_pWindow, 0, nSize);
				}
			}
			else if( m_pWindow != NULL )
			{
#if 1
				int nSize = GetWindowTextLength(*m_pWindow);
				if( nSize == 0 )
					nSize = 1;

				Edit_SetSel(*m_pWindow, 0, nSize);
#else
				POINT pt = event.ptMouse;
				pt.x -= m_rcItem.left + m_rcTextPadding.left;
				pt.y -= m_rcItem.top + m_rcTextPadding.top;
				::SendMessage(*m_pWindow, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
#endif
			}
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSEMOVE ) 
	{
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP ) 
	{
		return;
	}
	if( event.Type == UIEVENT_CONTEXTMENU )
	{
		return;
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		if( isEnabled() ) {
			m_uButtonState |= UISTATE_HOT;
			invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		if( isEnabled() ) {
			m_uButtonState &= ~UISTATE_HOT;
			invalidate();
		}
		return;
	}
	labelUI::doEvent(event);
}

void editUI::setEnabled(bool bEnable)
{
	controlUI::setEnabled(bEnable);
	if( !isEnabled() ) {
		m_uButtonState = 0;
	}
}

void editUI::setText(PCWSTR pstrText)
{
	m_sText = pstrText;
	if( m_pWindow != NULL ) Edit_SetText(*m_pWindow, m_sText.c_str());
	invalidate();
}

void editUI::setMaxChar(UINT uMax)
{
	m_uMaxChar = uMax;
	if( m_pWindow != NULL ) Edit_LimitText(*m_pWindow, m_uMaxChar);
}

UINT editUI::getMaxChar()
{
	return m_uMaxChar;
}

void editUI::setReadOnly(bool bReadOnly)
{
	if( m_bReadOnly == bReadOnly ) return;

	m_bReadOnly = bReadOnly;
	if( m_pWindow != NULL ) Edit_SetReadOnly(*m_pWindow, m_bReadOnly);
	invalidate();
}

bool editUI::isReadOnly() const
{
	return m_bReadOnly;
}

void editUI::setPasswordMode(bool bPasswordMode)
{
	if( m_bPasswordMode == bPasswordMode ) return;
	m_bPasswordMode = bPasswordMode;
	invalidate();
}

bool editUI::isPasswordMode() const
{
	return m_bPasswordMode;
}

void editUI::setPasswordChar(wchar_t cPasswordChar)
{
	if( m_cPasswordChar == cPasswordChar ) return;
	m_cPasswordChar = cPasswordChar;
	if( m_pWindow != NULL ) Edit_SetPasswordChar(*m_pWindow, m_cPasswordChar);
	invalidate();
}

wchar_t editUI::getPasswordChar() const
{
	return m_cPasswordChar;
}

PCWSTR editUI::getNormalImage()
{
	return m_sNormalImage.c_str();
}

void editUI::setNormalImage(PCWSTR pStrImage)
{
	m_sNormalImage = pStrImage;
	invalidate();
}

PCWSTR editUI::getHotImage()
{
	return m_sHotImage.c_str();
}

void editUI::setHotImage(PCWSTR pStrImage)
{
	m_sHotImage = pStrImage;
	invalidate();
}

PCWSTR editUI::getFocusedImage()
{
	return m_sFocusedImage.c_str();
}

void editUI::setFocusedImage(PCWSTR pStrImage)
{
	m_sFocusedImage = pStrImage;
	invalidate();
}

PCWSTR editUI::getDisabledImage()
{
	return m_sDisabledImage.c_str();
}

void editUI::setDisabledImage(PCWSTR pStrImage)
{
	m_sDisabledImage = pStrImage;
	invalidate();
}

void editUI::setNativeEditBkColor(DWORD dwBkColor)
{
	m_dwEditbkColor = dwBkColor;
}

DWORD editUI::getNativeEditBkColor() const
{
	return m_dwEditbkColor;
}

void editUI::setPos(RECT rc)
{
	controlUI::setPos(rc);
	if( m_pWindow != NULL ) {
		RECT rcPos = m_pWindow->calPos();
		::SetWindowPos(m_pWindow->getHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
			rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);        
	}
}

void editUI::setVisible(bool bVisible)
{
	controlUI::setVisible(bVisible);
	if( !isVisible() && m_pWindow != NULL ) m_pManager->setFocus(NULL);
}

void editUI::setInternVisible(bool bVisible)
{
	if( !isVisible() && m_pWindow != NULL ) m_pManager->setFocus(NULL);
}

SIZE editUI::estimateSize(SIZE szAvailable)
{
	if( m_cxyFixed.cy == 0 ) return size(m_cxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 6);
	return controlUI::estimateSize(szAvailable);
}

void editUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"readonly") == 0 ) setReadOnly(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"password") == 0 ) setPasswordMode(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"maxchar") == 0 ) setMaxChar(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"normalimage") == 0 ) setNormalImage(pstrValue);
	else if( _tcscmp(pstrName, L"hotimage") == 0 ) setHotImage(pstrValue);
	else if( _tcscmp(pstrName, L"focusedimage") == 0 ) setFocusedImage(pstrValue);
	else if( _tcscmp(pstrName, L"disabledimage") == 0 ) setDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, L"nativebkcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setNativeEditBkColor(clrColor);
	}
	else labelUI::setAttribute(pstrName, pstrValue);
}

void editUI::paintStatusImage(HDC hDC)
{
	if( isFocused() ) m_uButtonState |= UISTATE_FOCUSED;
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
	}
}

void editUI::paintText(HDC hDC)
{
	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->getDefaultFontColor();
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
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

scrollBarUI::scrollBarUI() : m_bHorizontal(false), m_nRange(100), m_nScrollPos(0), m_nLineSize(8), 
	m_pOwner(NULL), m_nLastScrollPos(0), m_nLastScrollOffset(0), m_nScrollRepeatDelay(0), m_uButton1State(0), \
	m_uButton2State(0), m_uThumbState(0), m_bShowButton1(true), m_bShowButton2(true)
{
	//TRACE_CLASS_BEGIN(this)
	m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
	ptLastMouse.x = ptLastMouse.y = 0;
	::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
	::ZeroMemory(&m_rcButton1, sizeof(m_rcButton1));
	::ZeroMemory(&m_rcButton2, sizeof(m_rcButton2));
}

PCWSTR scrollBarUI::getClass() const
{
	return L"ScrollBarUI";
}

LPVOID scrollBarUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"ScrollBar") == 0 ) return static_cast<scrollBarUI*>(this);
	return controlUI::getInterface(pstrName);
}

containerUI* scrollBarUI::getOwner() const
{
	return m_pOwner;
}

void scrollBarUI::setOwner(containerUI* pOwner)
{
	m_pOwner = pOwner;
}

void scrollBarUI::setEnabled(bool bEnable)
{
	controlUI::setEnabled(bEnable);
	if( !isEnabled() ) {
		m_uButton1State = 0;
		m_uButton2State = 0;
		m_uThumbState = 0;
	}
}

void scrollBarUI::setFocus()
{
	if( m_pOwner != NULL ) m_pOwner->setFocus();
	else controlUI::setFocus();
}

bool scrollBarUI::isHorizontal()
{
	return m_bHorizontal;
}

void scrollBarUI::setHorizontal(bool bHorizontal)
{
	if( m_bHorizontal == bHorizontal ) return;

	m_bHorizontal = bHorizontal;
	if( m_bHorizontal ) {
		if( m_cxyFixed.cy == 0 ) {
			m_cxyFixed.cx = 0;
			m_cxyFixed.cy = DEFAULT_SCROLLBAR_SIZE;
		}
	}
	else {
		if( m_cxyFixed.cx == 0 ) {
			m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
			m_cxyFixed.cy = 0;
		}
	}

	if( m_pOwner != NULL ) 
		m_pOwner->needUpdate(); 
	else 
		needParentUpdate();
}

int scrollBarUI::getScrollRange() const
{
	return m_nRange;
}

void scrollBarUI::setScrollRange(int nRange)
{
	if( m_nRange == nRange ) return;

	m_nRange = nRange;
	if( m_nRange < 0 ) m_nRange = 0;
	if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;
	setPos(m_rcItem);
}

int scrollBarUI::getScrollPos() const
{
	return m_nScrollPos;
}

void scrollBarUI::setScrollPos(int nPos)
{
	if( m_nScrollPos == nPos ) return;

	m_nScrollPos = nPos;
	if( m_nScrollPos < 0 ) m_nScrollPos = 0;
	if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;
	setPos(m_rcItem);
}

int scrollBarUI::getLineSize() const
{
	return m_nLineSize;
}

void scrollBarUI::setLineSize(int nSize)
{
	m_nLineSize = nSize;
}

bool scrollBarUI::getShowButton1()
{
	return m_bShowButton1;
}

void scrollBarUI::setShowButton1(bool bShow)
{
	m_bShowButton1 = bShow;
	setPos(m_rcItem);
}

PCWSTR scrollBarUI::getButton1NormalImage()
{
	return m_sButton1NormalImage.c_str();
}

void scrollBarUI::setButton1NormalImage(PCWSTR pStrImage)
{
	m_sButton1NormalImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getButton1HotImage()
{
	return m_sButton1HotImage.c_str();
}

void scrollBarUI::setButton1HotImage(PCWSTR pStrImage)
{
	m_sButton1HotImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getButton1PushedImage()
{
	return m_sButton1PushedImage.c_str();
}

void scrollBarUI::setButton1PushedImage(PCWSTR pStrImage)
{
	m_sButton1PushedImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getButton1DisabledImage()
{
	return m_sButton1DisabledImage.c_str();
}

void scrollBarUI::setButton1DisabledImage(PCWSTR pStrImage)
{
	m_sButton1DisabledImage = pStrImage;
	invalidate();
}

bool scrollBarUI::getShowButton2()
{
	return m_bShowButton2;
}

void scrollBarUI::setShowButton2(bool bShow)
{
	m_bShowButton2 = bShow;
	setPos(m_rcItem);
}

PCWSTR scrollBarUI::getButton2NormalImage()
{
	return m_sButton2NormalImage.c_str();
}

void scrollBarUI::setButton2NormalImage(PCWSTR pStrImage)
{
	m_sButton2NormalImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getButton2HotImage()
{
	return m_sButton2HotImage.c_str();
}

void scrollBarUI::setButton2HotImage(PCWSTR pStrImage)
{
	m_sButton2HotImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getButton2PushedImage()
{
	return m_sButton2PushedImage.c_str();
}

void scrollBarUI::setButton2PushedImage(PCWSTR pStrImage)
{
	m_sButton2PushedImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getButton2DisabledImage()
{
	return m_sButton2DisabledImage.c_str();
}

void scrollBarUI::setButton2DisabledImage(PCWSTR pStrImage)
{
	m_sButton2DisabledImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getThumbNormalImage()
{
	return m_sThumbNormalImage.c_str();
}

void scrollBarUI::setThumbNormalImage(PCWSTR pStrImage)
{
	m_sThumbNormalImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getThumbHotImage()
{
	return m_sThumbHotImage.c_str();
}

void scrollBarUI::setThumbHotImage(PCWSTR pStrImage)
{
	m_sThumbHotImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getThumbPushedImage()
{
	return m_sThumbPushedImage.c_str();
}

void scrollBarUI::setThumbPushedImage(PCWSTR pStrImage)
{
	m_sThumbPushedImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getThumbDisabledImage()
{
	return m_sThumbDisabledImage.c_str();
}

void scrollBarUI::setThumbDisabledImage(PCWSTR pStrImage)
{
	m_sThumbDisabledImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getRailNormalImage()
{
	return m_sRailNormalImage.c_str();
}

void scrollBarUI::setRailNormalImage(PCWSTR pStrImage)
{
	m_sRailNormalImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getRailHotImage()
{
	return m_sRailHotImage.c_str();
}

void scrollBarUI::setRailHotImage(PCWSTR pStrImage)
{
	m_sRailHotImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getRailPushedImage()
{
	return m_sRailPushedImage.c_str();
}

void scrollBarUI::setRailPushedImage(PCWSTR pStrImage)
{
	m_sRailPushedImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getRailDisabledImage()
{
	return m_sRailDisabledImage.c_str();
}

void scrollBarUI::setRailDisabledImage(PCWSTR pStrImage)
{
	m_sRailDisabledImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getBkNormalImage()
{
	return m_sBkNormalImage.c_str();
}

void scrollBarUI::setBkNormalImage(PCWSTR pStrImage)
{
	m_sBkNormalImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getBkHotImage()
{
	return m_sBkHotImage.c_str();
}

void scrollBarUI::setBkHotImage(PCWSTR pStrImage)
{
	m_sBkHotImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getBkPushedImage()
{
	return m_sBkPushedImage.c_str();
}

void scrollBarUI::setBkPushedImage(PCWSTR pStrImage)
{
	m_sBkPushedImage = pStrImage;
	invalidate();
}

PCWSTR scrollBarUI::getBkDisabledImage()
{
	return m_sBkDisabledImage.c_str();
}

void scrollBarUI::setBkDisabledImage(PCWSTR pStrImage)
{
	m_sBkDisabledImage = pStrImage;
	invalidate();
}

void scrollBarUI::setPos(RECT rc)
{
	controlUI::setPos(rc);
	rc = m_rcItem;

	if( m_bHorizontal ) {
		int cx = rc.right - rc.left;
		if( m_bShowButton1 ) cx -= m_cxyFixed.cy;
		if( m_bShowButton2 ) cx -= m_cxyFixed.cy;
		if( cx > m_cxyFixed.cy ) {
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + m_cxyFixed.cy;
				m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.top = rc.top;
			m_rcButton2.right = rc.right;
			if( m_bShowButton2 ) {
				m_rcButton2.left = rc.right - m_cxyFixed.cy;
				m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
			}
			else {
				m_rcButton2.left = m_rcButton2.right;
				m_rcButton2.bottom = m_rcButton2.top;
			}

			m_rcThumb.top = rc.top;
			m_rcThumb.bottom = rc.top + m_cxyFixed.cy;
			if( m_nRange > 0 ) {
				int cxThumb = cx * (rc.right - rc.left) / (m_nRange + rc.right - rc.left);
				if( cxThumb < m_cxyFixed.cy ) cxThumb = m_cxyFixed.cy;

				m_rcThumb.left = m_nScrollPos * (cx - cxThumb) / m_nRange + m_rcButton1.right;
				m_rcThumb.right = m_rcThumb.left + cxThumb;
				if( m_rcThumb.right > m_rcButton2.left ) {
					m_rcThumb.left = m_rcButton2.left - cxThumb;
					m_rcThumb.right = m_rcButton2.left;
				}
			}
			else {
				m_rcThumb.left = m_rcButton1.right;
				m_rcThumb.right = m_rcButton2.left;
			}
		}
		else {
			int cxButton = (rc.right - rc.left) / 2;
			if( cxButton > m_cxyFixed.cy ) cxButton = m_cxyFixed.cy;
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + cxButton;
				m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.top = rc.top;
			m_rcButton2.right = rc.right;
			if( m_bShowButton2 ) {
				m_rcButton2.left = rc.right - cxButton;
				m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
			}
			else {
				m_rcButton2.left = m_rcButton2.right;
				m_rcButton2.bottom = m_rcButton2.top;
			}

			::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		}
	}
	else {
		int cy = rc.bottom - rc.top;
		if( m_bShowButton1 ) cy -= m_cxyFixed.cx;
		if( m_bShowButton2 ) cy -= m_cxyFixed.cx;
		if( cy > m_cxyFixed.cx ) {
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + m_cxyFixed.cx;
				m_rcButton1.bottom = rc.top + m_cxyFixed.cx;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.left = rc.left;
			m_rcButton2.bottom = rc.bottom;
			if( m_bShowButton2 ) {
				m_rcButton2.top = rc.bottom - m_cxyFixed.cx;
				m_rcButton2.right = rc.left + m_cxyFixed.cx;
			}
			else {
				m_rcButton2.top = m_rcButton2.bottom;
				m_rcButton2.right = m_rcButton2.left;
			}

			m_rcThumb.left = rc.left;
			m_rcThumb.right = rc.left + m_cxyFixed.cx;
			if( m_nRange > 0 ) {
				int cyThumb = cy * (rc.bottom - rc.top) / (m_nRange + rc.bottom - rc.top);
				if( cyThumb < m_cxyFixed.cx ) cyThumb = m_cxyFixed.cx;

				m_rcThumb.top = m_nScrollPos * (cy - cyThumb) / m_nRange + m_rcButton1.bottom;
				m_rcThumb.bottom = m_rcThumb.top + cyThumb;
				if( m_rcThumb.bottom > m_rcButton2.top ) {
					m_rcThumb.top = m_rcButton2.top - cyThumb;
					m_rcThumb.bottom = m_rcButton2.top;
				}
			}
			else {
				m_rcThumb.top = m_rcButton1.bottom;
				m_rcThumb.bottom = m_rcButton2.top;
			}
		}
		else {
			int cyButton = (rc.bottom - rc.top) / 2;
			if( cyButton > m_cxyFixed.cx ) cyButton = m_cxyFixed.cx;
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + m_cxyFixed.cx;
				m_rcButton1.bottom = rc.top + cyButton;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.left = rc.left;
			m_rcButton2.bottom = rc.bottom;
			if( m_bShowButton2 ) {
				m_rcButton2.top = rc.bottom - cyButton;
				m_rcButton2.right = rc.left + m_cxyFixed.cx;
			}
			else {
				m_rcButton2.top = m_rcButton2.bottom;
				m_rcButton2.right = m_rcButton2.left;
			}

			::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		}
	}
}

void scrollBarUI::doEvent(TEventUI& event)
{
	if( !isMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pOwner != NULL ) m_pOwner->doEvent(event);
		else controlUI::doEvent(event);
		return;
	}

	if( event.Type == UIEVENT_SETFOCUS ) 
	{
		return;
	}
	if( event.Type == UIEVENT_KILLFOCUS ) 
	{
		return;
	}
	if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
	{
		if( !isEnabled() ) return;

		m_nLastScrollOffset = 0;
		m_nScrollRepeatDelay = 0;
		m_pManager->setTimer(this, DEFAULT_TIMERID, 50U);

		if( ::PtInRect(&m_rcButton1, event.ptMouse) ) {
			m_uButton1State |= UISTATE_PUSHED;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->lineUp(); 
				else setScrollPos(m_nScrollPos - m_nLineSize);
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->lineLeft(); 
				else setScrollPos(m_nScrollPos - m_nLineSize);
			}
		}
		else if( ::PtInRect(&m_rcButton2, event.ptMouse) ) {
			m_uButton2State |= UISTATE_PUSHED;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->lineDown(); 
				else setScrollPos(m_nScrollPos + m_nLineSize);
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->lineRight(); 
				else setScrollPos(m_nScrollPos + m_nLineSize);
			}
		}
		else if( ::PtInRect(&m_rcThumb, event.ptMouse) ) {
			m_uThumbState |= UISTATE_CAPTURED | UISTATE_PUSHED;
			ptLastMouse = event.ptMouse;
			m_nLastScrollPos = m_nScrollPos;
		}
		else {
			if( !m_bHorizontal ) {
				if( event.ptMouse.y < m_rcThumb.top ) {
					if( m_pOwner != NULL ) m_pOwner->pageUp(); 
					else setScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
				}
				else if ( event.ptMouse.y > m_rcThumb.bottom ){
					if( m_pOwner != NULL ) m_pOwner->pageDown(); 
					else setScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);                    
				}
			}
			else {
				if( event.ptMouse.x < m_rcThumb.left ) {
					if( m_pOwner != NULL ) m_pOwner->pageLeft(); 
					else setScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
				}
				else if ( event.ptMouse.x > m_rcThumb.right ){
					if( m_pOwner != NULL ) m_pOwner->pageRight(); 
					else setScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);                    
				}
			}
		}
		if( m_pManager != NULL && m_pOwner == NULL ) m_pManager->sendNotify(this, L"scroll");
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		m_nScrollRepeatDelay = 0;
		m_nLastScrollOffset = 0;
		m_pManager->killTimer(this, DEFAULT_TIMERID);

		if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
			m_uThumbState &= ~( UISTATE_CAPTURED | UISTATE_PUSHED );
			invalidate();
		}
		else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
			m_uButton1State &= ~UISTATE_PUSHED;
			invalidate();
		}
		else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
			m_uButton2State &= ~UISTATE_PUSHED;
			invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSEMOVE )
	{
		if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
			if( !m_bHorizontal ) {
				m_nLastScrollOffset = (event.ptMouse.y - ptLastMouse.y) * m_nRange / \
					(m_rcItem.bottom - m_rcItem.top - m_rcThumb.bottom + m_rcThumb.top - 2 * m_cxyFixed.cx);
			}
			else {
				m_nLastScrollOffset = (event.ptMouse.x - ptLastMouse.x) * m_nRange / \
					(m_rcItem.right - m_rcItem.left - m_rcThumb.right + m_rcThumb.left - 2 * m_cxyFixed.cy);
			}
		}
		else {
			if( (m_uThumbState & UISTATE_HOT) != 0 ) {
				if( !::PtInRect(&m_rcThumb, event.ptMouse) ) {
					m_uThumbState &= ~UISTATE_HOT;
					invalidate();
				}
			}
			else {
				if( !isEnabled() ) return;
				if( ::PtInRect(&m_rcThumb, event.ptMouse) ) {
					m_uThumbState |= UISTATE_HOT;
					invalidate();
				}
			}
		}
		return;
	}
	if( event.Type == UIEVENT_CONTEXTMENU )
	{
		return;
	}
	if( event.Type == UIEVENT_TIMER && event.wParam == DEFAULT_TIMERID )
	{
		++m_nScrollRepeatDelay;
		if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->setScrollPos(size(m_pOwner->getScrollPos().cx, \
					m_nLastScrollPos + m_nLastScrollOffset)); 
				else setScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->setScrollPos(size(m_nLastScrollPos + m_nLastScrollOffset, \
					m_pOwner->getScrollPos().cy)); 
				else setScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
			}
			invalidate();
		}
		else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
			if( m_nScrollRepeatDelay <= 5 ) return;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->lineUp(); 
				else setScrollPos(m_nScrollPos - m_nLineSize);
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->lineLeft(); 
				else setScrollPos(m_nScrollPos - m_nLineSize);
			}
		}
		else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
			if( m_nScrollRepeatDelay <= 5 ) return;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->lineDown(); 
				else setScrollPos(m_nScrollPos + m_nLineSize);
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->lineRight(); 
				else setScrollPos(m_nScrollPos + m_nLineSize);
			}
		}
		else {
			if( m_nScrollRepeatDelay <= 5 ) return;
			POINT pt = { 0 };
			::GetCursorPos(&pt);
			::ScreenToClient(m_pManager->getPaintWindow(), &pt);
			if( !m_bHorizontal ) {
				if( pt.y < m_rcThumb.top ) {
					if( m_pOwner != NULL ) m_pOwner->pageUp(); 
					else setScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
				}
				else if ( pt.y > m_rcThumb.bottom ){
					if( m_pOwner != NULL ) m_pOwner->pageDown(); 
					else setScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);                    
				}
			}
			else {
				if( pt.x < m_rcThumb.left ) {
					if( m_pOwner != NULL ) m_pOwner->pageLeft(); 
					else setScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
				}
				else if ( pt.x > m_rcThumb.right ){
					if( m_pOwner != NULL ) m_pOwner->pageRight(); 
					else setScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);                    
				}
			}
		}
		if( m_pManager != NULL && m_pOwner == NULL ) m_pManager->sendNotify(this, L"scroll");
		return;
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		if( isEnabled() ) {
			m_uButton1State |= UISTATE_HOT;
			m_uButton2State |= UISTATE_HOT;
			if( ::PtInRect(&m_rcThumb, event.ptMouse) ) m_uThumbState |= UISTATE_HOT;
			invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		if( isEnabled() ) {
			m_uButton1State &= ~UISTATE_HOT;
			m_uButton2State &= ~UISTATE_HOT;
			m_uThumbState &= ~UISTATE_HOT;
			invalidate();
		}
		return;
	}

	if( m_pOwner != NULL ) m_pOwner->doEvent(event); else controlUI::doEvent(event);
}

void scrollBarUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"button1normalimage") == 0 ) setButton1NormalImage(pstrValue);
	else if( _tcscmp(pstrName, L"button1hotimage") == 0 ) setButton1HotImage(pstrValue);
	else if( _tcscmp(pstrName, L"button1pushedimage") == 0 ) setButton1PushedImage(pstrValue);
	else if( _tcscmp(pstrName, L"button1disabledimage") == 0 ) setButton1DisabledImage(pstrValue);
	else if( _tcscmp(pstrName, L"button2normalimage") == 0 ) setButton2NormalImage(pstrValue);
	else if( _tcscmp(pstrName, L"button2hotimage") == 0 ) setButton2HotImage(pstrValue);
	else if( _tcscmp(pstrName, L"button2pushedimage") == 0 ) setButton2PushedImage(pstrValue);
	else if( _tcscmp(pstrName, L"button2disabledimage") == 0 ) setButton2DisabledImage(pstrValue);
	else if( _tcscmp(pstrName, L"thumbnormalimage") == 0 ) setThumbNormalImage(pstrValue);
	else if( _tcscmp(pstrName, L"thumbhotimage") == 0 ) setThumbHotImage(pstrValue);
	else if( _tcscmp(pstrName, L"thumbpushedimage") == 0 ) setThumbPushedImage(pstrValue);
	else if( _tcscmp(pstrName, L"thumbdisabledimage") == 0 ) setThumbDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, L"railnormalimage") == 0 ) setRailNormalImage(pstrValue);
	else if( _tcscmp(pstrName, L"railhotimage") == 0 ) setRailHotImage(pstrValue);
	else if( _tcscmp(pstrName, L"railpushedimage") == 0 ) setRailPushedImage(pstrValue);
	else if( _tcscmp(pstrName, L"raildisabledimage") == 0 ) setRailDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, L"bknormalimage") == 0 ) setBkNormalImage(pstrValue);
	else if( _tcscmp(pstrName, L"bkhotimage") == 0 ) setBkHotImage(pstrValue);
	else if( _tcscmp(pstrName, L"bkpushedimage") == 0 ) setBkPushedImage(pstrValue);
	else if( _tcscmp(pstrName, L"bkdisabledimage") == 0 ) setBkDisabledImage(pstrValue);
	else if( _tcscmp(pstrName, L"hor") == 0 ) setHorizontal(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"linesize") == 0 ) setLineSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"range") == 0 ) setScrollRange(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"value") == 0 ) setScrollPos(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"showbutton1") == 0 ) setShowButton1(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"showbutton2") == 0 ) setShowButton2(_tcscmp(pstrValue, L"true") == 0);
	else controlUI::setAttribute(pstrName, pstrValue);
}

void scrollBarUI::doPaint(HDC hDC, const RECT& rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	paintBk(hDC);
	paintButton1(hDC);
	paintButton2(hDC);
	paintThumb(hDC);
	paintRail(hDC);
}

void scrollBarUI::paintBk(HDC hDC)
{
	if( !isEnabled() ) m_uThumbState |= UISTATE_DISABLED;
	else m_uThumbState &= ~ UISTATE_DISABLED;

	if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
		if( !m_sBkDisabledImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sBkDisabledImage.c_str()) ) m_sBkDisabledImage.clear();
			else return;
		}
	}
	else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
		if( !m_sBkPushedImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sBkPushedImage.c_str()) ) m_sBkPushedImage.clear();
			else return;
		}
	}
	else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
		if( !m_sBkHotImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sBkHotImage.c_str()) ) m_sBkHotImage.clear();
			else return;
		}
	}

	if( !m_sBkNormalImage.empty() ) {
		if( !drawImage(hDC, (PCWSTR)m_sBkNormalImage.c_str()) ) m_sBkNormalImage.clear();
		else return;
	}
}

void scrollBarUI::paintButton1(HDC hDC)
{
	if( !m_bShowButton1 ) return;

	if( !isEnabled() ) m_uButton1State |= UISTATE_DISABLED;
	else m_uButton1State &= ~ UISTATE_DISABLED;

	m_sImageModify.clear();
	m_sImageModify = smallFormat(L"dest='%d,%d,%d,%d'", m_rcButton1.left - m_rcItem.left, \
		m_rcButton1.top - m_rcItem.top, m_rcButton1.right - m_rcItem.left, m_rcButton1.bottom - m_rcItem.top);

	if( (m_uButton1State & UISTATE_DISABLED) != 0 ) {
		if( !m_sButton1DisabledImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sButton1DisabledImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sButton1DisabledImage.clear();
			else return;
		}
	}
	else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
		if( !m_sButton1PushedImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sButton1PushedImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sButton1PushedImage.clear();
			else return;
		}
	}
	else if( (m_uButton1State & UISTATE_HOT) != 0 ) {
		if( !m_sButton1HotImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sButton1HotImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sButton1HotImage.clear();
			else return;
		}
	}

	if( !m_sButton1NormalImage.empty() ) {
		if( !drawImage(hDC, (PCWSTR)m_sButton1NormalImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sButton1NormalImage.clear();
		else return;
	}

	DWORD dwBorderColor = 0xFF85E4FF;
	int nBorderSize = 2;
	renderEngine::drawRect(hDC, m_rcButton1, nBorderSize, dwBorderColor);
}

void scrollBarUI::paintButton2(HDC hDC)
{
	if( !m_bShowButton2 ) return;

	if( !isEnabled() ) m_uButton2State |= UISTATE_DISABLED;
	else m_uButton2State &= ~ UISTATE_DISABLED;

	m_sImageModify.clear();
	m_sImageModify = smallFormat(L"dest='%d,%d,%d,%d'", m_rcButton2.left - m_rcItem.left, \
		m_rcButton2.top - m_rcItem.top, m_rcButton2.right - m_rcItem.left, m_rcButton2.bottom - m_rcItem.top);

	if( (m_uButton2State & UISTATE_DISABLED) != 0 ) {
		if( !m_sButton2DisabledImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sButton2DisabledImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sButton2DisabledImage.clear();
			else return;
		}
	}
	else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
		if( !m_sButton2PushedImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sButton2PushedImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sButton2PushedImage.clear();
			else return;
		}
	}
	else if( (m_uButton2State & UISTATE_HOT) != 0 ) {
		if( !m_sButton2HotImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sButton2HotImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sButton2HotImage.clear();
			else return;
		}
	}

	if( !m_sButton2NormalImage.empty() ) {
		if( !drawImage(hDC, (PCWSTR)m_sButton2NormalImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sButton2NormalImage.clear();
		else return;
	}

	DWORD dwBorderColor = 0xFF85E4FF;
	int nBorderSize = 2;
	renderEngine::drawRect(hDC, m_rcButton2, nBorderSize, dwBorderColor);
}

void scrollBarUI::paintThumb(HDC hDC)
{
	if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
	if( !isEnabled() ) m_uThumbState |= UISTATE_DISABLED;
	else m_uThumbState &= ~ UISTATE_DISABLED;

	m_sImageModify.clear();
	m_sImageModify = smallFormat(L"dest='%d,%d,%d,%d'", m_rcThumb.left - m_rcItem.left, \
		m_rcThumb.top - m_rcItem.top, m_rcThumb.right - m_rcItem.left, m_rcThumb.bottom - m_rcItem.top);

	if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
		if( !m_sThumbDisabledImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sThumbDisabledImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sThumbDisabledImage.clear();
			else return;
		}
	}
	else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
		if( !m_sThumbPushedImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sThumbPushedImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sThumbPushedImage.clear();
			else return;
		}
	}
	else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
		if( !m_sThumbHotImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sThumbHotImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sThumbHotImage.clear();
			else return;
		}
	}

	if( !m_sThumbNormalImage.empty() ) {
		if( !drawImage(hDC, (PCWSTR)m_sThumbNormalImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sThumbNormalImage.clear();
		else return;
	}

	DWORD dwBorderColor = 0xFF85E4FF;
	int nBorderSize = 2;
	renderEngine::drawRect(hDC, m_rcThumb, nBorderSize, dwBorderColor);
}

void scrollBarUI::paintRail(HDC hDC)
{
	if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
	if( !isEnabled() ) m_uThumbState |= UISTATE_DISABLED;
	else m_uThumbState &= ~ UISTATE_DISABLED;

	m_sImageModify.clear();
	if( !m_bHorizontal ) {
		m_sImageModify = smallFormat(L"dest='%d,%d,%d,%d'", m_rcThumb.left - m_rcItem.left, \
			(m_rcThumb.top + m_rcThumb.bottom) / 2 - m_rcItem.top - m_cxyFixed.cx / 2, \
			m_rcThumb.right - m_rcItem.left, \
			(m_rcThumb.top + m_rcThumb.bottom) / 2 - m_rcItem.top + m_cxyFixed.cx - m_cxyFixed.cx / 2);
	}
	else {
		m_sImageModify = smallFormat(L"dest='%d,%d,%d,%d'", \
			(m_rcThumb.left + m_rcThumb.right) / 2 - m_rcItem.left - m_cxyFixed.cy / 2, \
			m_rcThumb.top - m_rcItem.top, \
			(m_rcThumb.left + m_rcThumb.right) / 2 - m_rcItem.left + m_cxyFixed.cy - m_cxyFixed.cy / 2, \
			m_rcThumb.bottom - m_rcItem.top);
	}

	if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
		if( !m_sRailDisabledImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sRailDisabledImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sRailDisabledImage.clear();
			else return;
		}
	}
	else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
		if( !m_sRailPushedImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sRailPushedImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sRailPushedImage.clear();
			else return;
		}
	}
	else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
		if( !m_sRailHotImage.empty() ) {
			if( !drawImage(hDC, (PCWSTR)m_sRailHotImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sRailHotImage.clear();
			else return;
		}
	}

	if( !m_sRailNormalImage.empty() ) {
		if( !drawImage(hDC, (PCWSTR)m_sRailNormalImage.c_str(), (PCWSTR)m_sImageModify.c_str()) ) m_sRailNormalImage.clear();
		else return;
	}
}

}