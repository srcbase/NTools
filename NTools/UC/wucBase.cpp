#include "wucBase.h"
#include "windowWnd.h"
#if defined(WUC_SUPPORT_THEME)
	#include "AC/theme.h"

#endif

namespace wuc
{
labelUC::labelUC(void)
	: controlUC()
	, m_uiTextStyle(DT_VCENTER), m_dwTextColor(0)
	, m_dwDisabledTextColor(0), m_iFont(-1)
	, m_bShowHtml(false), m_bShadowText(false)
	, m_pimgText(nullptr)
{
	::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
	::ZeroMemory(&m_szOffsetXY, sizeof(m_szOffsetXY));
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
		return;
		break;
	case BEUCEventKillFocus:
		m_bFocused = false;
		return;
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
SIZE labelUC::estimateSize(SIZE szAvailable)
{
	if( m_szxyFixed.cy == 0 )
	{
		SIZE sz = {m_szxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 4};
		return sz;
	}
	return controlUC::estimateSize(szAvailable);
}
SIZE labelUC::autoCalculateSize()
{
	gtc::rect rc;
	rc = gtc::drawer::measureString(m_wsText.c_str(), rc, m_pManager->getFont(getFont()), m_uiTextStyle);
	return rc.getSize();
}
void labelUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = NULL;
	if( _tcscmp(pstrName, L"align") == 0 ) {
		if( _tcsstr(pstrValue, L"left") != NULL ) {
			m_uiTextStyle &= ~(DT_CENTER | DT_RIGHT | DT_TOP | DT_BOTTOM);
			m_uiTextStyle |= DT_LEFT;
		}
		if( _tcsstr(pstrValue, L"center") != NULL ) {
			m_uiTextStyle &= ~(DT_LEFT | DT_RIGHT | DT_TOP | DT_BOTTOM);
			m_uiTextStyle |= DT_CENTER;
		}
		if( _tcsstr(pstrValue, L"right") != NULL ) {
			m_uiTextStyle &= ~(DT_LEFT | DT_CENTER | DT_TOP | DT_BOTTOM);
			m_uiTextStyle |= DT_RIGHT;
		}
		if( _tcsstr(pstrValue, L"top") != NULL ) {
			m_uiTextStyle &= ~(DT_BOTTOM | DT_VCENTER | DT_LEFT | DT_RIGHT);
			m_uiTextStyle |= DT_TOP;
		}
		if( _tcsstr(pstrValue, L"bottom") != NULL ) {
			m_uiTextStyle &= ~(DT_TOP | DT_VCENTER | DT_LEFT | DT_RIGHT);
			m_uiTextStyle |= DT_BOTTOM;
		}
	}
	else if( _tcscmp(pstrName, L"endellipsis") == 0 ) {
		if( _tcscmp(pstrValue, L"true") == 0 ) m_uiTextStyle |= DT_END_ELLIPSIS;
		else m_uiTextStyle &= ~DT_END_ELLIPSIS;
	}    
	else if( _tcscmp(pstrName, L"font") == 0 ) setFont(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"textcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"disabledtextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setDisabledTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"textpadding") == 0 ) {
		RECT rcTextPadding = { 0 };
		rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setTextPadding(rcTextPadding);
	}
	else if( _tcscmp(pstrName, L"textimage") == 0 ) {
		setTextImage(pstrValue);
	}
	else if( _tcscmp(pstrName, L"showhtml") == 0 ) setShowHtml(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"shadowtext") == 0 ) setShadowText(_tcscmp(pstrValue, L"true") == 0);
	else controlUC::setAttribute(pstrName, pstrValue);
}

gtc::image* labelUC::getTextImage() const
{
	return m_pimgText;
}
void labelUC::setTextImage(PCWSTR pImage)
{
	gtc::image *pimg = analyzeImage(pImage);// drawManager::getResource().findImage(pImage);
	setTextImage(pimg);
}
void labelUC::setTextImage(gtc::image *pImage)
{
	if(m_pimgText == pImage) return;
	m_pimgText = pImage;
	invalidate();
}
SIZE labelUC::getTextImageOffsetXY() const
{
	return m_szOffsetXY;
}
void labelUC::setTextImageOffsetXY(SIZE szXY)
{
	if(m_szOffsetXY == szXY) return;
	m_szOffsetXY = szXY;
	invalidate();
}
void labelUC::setTextImageOffsetXY(LONG x, LONG y)
{
	SIZE sz = {x, y};
	setTextImageOffsetXY(sz);
}

void labelUC::drawTextImage(HDC hdc, RECT rcPaint)
{
	assert(hdc);
	if(!hdc || !m_pimgText) return;
	SIZE szImage = m_pimgText->getImageSize();
	RECT rcSrc = {0, 0, szImage.cx, szImage.cy};
	RECT rc = rcPaint;
	Gdiplus::Graphics grap(hdc);

	//assert(rcInRect(rc, rcSrc));
	assert(rectWidth(rc) > szImage.cx);
	assert(rectHeight(rc) > szImage.cy);
	SIZE szSpace = {(rectWidth(rc)-szImage.cx)/2, (rectHeight(rc)-szImage.cy)/2};
	::InflateRect(&rc, -szSpace.cx, -szSpace.cy);
	if(m_uiTextStyle & DT_LEFT) ::OffsetRect(&rc, -szSpace.cx, 0);
	else if(m_uiTextStyle & DT_RIGHT) ::OffsetRect(&rc, szSpace.cx, 0);

	if(m_uiTextStyle & DT_TOP) ::OffsetRect(&rc, 0, -szSpace.cy);
	else if(m_uiTextStyle & DT_BOTTOM) ::OffsetRect(&rc, 0, szSpace.cy);

	::OffsetRect(&rc, -m_szOffsetXY.cx, -m_szOffsetXY.cy);
	::IntersectRect(&rc, &rc, &rcPaint);
	rc.right--/*, rc.bottom--*/;
	gtc::drawer::drawImage(grap, m_pimgText, rc, rcSrc);
	
}
void labelUC::paintText(HDC hdc)
{
	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->getDefaultFontColor();
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->getDefaultDisabledColor();

	int nLinks = 0;
	RECT rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;

	::OffsetRect(&rc, m_szOffsetXY.cx, m_szOffsetXY.cy);
	//if(m_wsText == L"正在载入...")
	//{
	//	int s;
	//	s = 0;
	//}
	if( m_wsText.empty() ) 
	{
		drawTextImage(hdc, rc);
		return;
	}
	Gdiplus::Graphics grap(hdc);
	if( isEnabled() ) {
		if( m_bShowHtml )
		{
			//renderEngine::drawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), m_dwTextColor, \
			//NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
		}
		else if (m_bShadowText)
		{
#if defined(WUC_SUPPORT_THEME)
			acm::theme themeShadow;
			if (themeShadow.open(m_pManager->getPaintWindow(), L"TextStyle"))
			{
				bool bTheme = themeShadow.drawShadowText(hdc, m_wsText.c_str(), &rc, m_dwTextColor);
				themeShadow.close();
			}

#endif
		}
		else
		{
			gtc::drawer::drawString(grap, m_wsText.c_str(), gtc::rect(rc), m_pManager->getFont(m_iFont), (BEAlignMode)m_uiTextStyle, m_dwTextColor);
			//renderEngine::drawText(hDC, m_pManager, rc, m_sText.c_str(), m_dwTextColor, \
			//m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
	}
	else {
		if( m_bShowHtml )
		{
			//renderEngine::drawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), m_dwDisabledTextColor, \
			//NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
		}
		else if (m_bShadowText)
		{
#if defined(WUC_SUPPORT_THEME)
			acm::theme themeShadow;
			if (themeShadow.open(m_pManager->getPaintWindow(), L"TextStyle"))
			{
				bool bTheme = themeShadow.drawShadowText(hdc, m_wsText.c_str(), &rc, m_dwDisabledTextColor);
				themeShadow.close();
			}

#endif
		}
		else
		{
			gtc::drawer::drawString(grap, m_wsText.c_str(), gtc::rect(rc), m_pManager->getDefaultFontInfo()->hFont, (BEAlignMode)m_uiTextStyle, m_dwDisabledTextColor);
			//renderEngine::drawText(hDC, m_pManager, rc, m_sText.c_str(), m_dwDisabledTextColor, \
			//m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
	}
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
void labelUC::setTextPadding(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setTextPadding(rc);
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
bool labelUC::isShadowText() const
{
	return m_bShadowText;
}
void labelUC::setShadowText(bool bShadow /*= true*/)
{
	if(m_bShadowText == bShadow) return;
	m_bShadowText = bShadow;
	invalidate();
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

buttonUC::buttonUC() 
	: labelUC()
	, m_uButtonState(0), m_dwHotTextColor(0), m_dwPushedTextColor(0), m_dwFocusedTextColor(0)
	, m_dwTextColorCheck(0), m_dwHotTextColorCheck(0), m_dwPushedTextColorCheck(0), m_dwFocusedTextColorCheck(0), m_dwDisabledTextColorCheck(0)
	, m_bSelected(false), m_bCheckBox(false)
	, m_beIconPosition(iconBeLeft), m_iIconSpace(-1)
	, m_pimgIcon(NULL)/*, m_bFrontIcon(true), m_iIconFixedWidth(0)*/
	//, m_pImage(NULL)
	//, m_iNormalImage(0), m_iHotImage(0), m_iPushedImage(0), m_iFocusedImage(0), m_iDisabledImage(0)
{
	m_uiTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	m_szIconFixed.cx = m_szIconFixed.cy = 0;
	//setImagePatch(4,4);
	//TRACE_CLASS_BEGIN(this)
}
buttonUC::~buttonUC()
{
	SEND_LOG(L"buttonUC <%s>%08X free ...", m_wsName.c_str(), this);
}
PCWSTR buttonUC::getClass() const
{
	return L"buttonUC";
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
	//if(event.pSender) SEND_LOG(L"buttonUC::Event %s %s", event.pSender->getName().c_str(), toString((BEUCEvent)event.Type).c_str());
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
	if( event.Type == BEUCEventSetCursor ) {
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		return;
	}
	__super::Event(event);
}

bool buttonUC::activate()
{
	if( !controlUC::activate() ) return false;
	if (m_bCheckBox) switchSelected();
	else selected();
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

void buttonUC::setTextColorCheck(DWORD dwColor)
{
	m_dwTextColorCheck = dwColor;
}
DWORD buttonUC::getTextColorCheck() const
{
	return m_dwTextColorCheck;
}
void buttonUC::setHotTextColorCheck(DWORD dwColor)
{
	m_dwHotTextColorCheck = dwColor;
}
DWORD buttonUC::getHotTextColorCheck() const
{
	return m_dwHotTextColorCheck;
}
void buttonUC::setPushedTextColorCheck(DWORD dwColor)
{
	m_dwPushedTextColorCheck = dwColor;
}
DWORD buttonUC::getPushedTextColorCheck() const
{
	return m_dwPushedTextColorCheck;
}
void buttonUC::setFocusedTextColorCheck(DWORD dwColor)
{
	m_dwFocusedTextColorCheck = dwColor;
}
DWORD buttonUC::getFocusedTextColorCheck() const
{
	return m_dwFocusedTextColorCheck;
}
void buttonUC::setDisabledTextColorCheck(DWORD dwColor)
{
	m_dwDisabledTextColorCheck = dwColor;
}
DWORD buttonUC::getDisabledTextColorCheck() const
{
	return m_dwDisabledTextColorCheck;
}

inline bool buttonUC::isCheckBox() const
{
	return m_bCheckBox;
}
void buttonUC::setCheckBox(bool bCheckbox /*= true*/)
{
	if(m_bCheckBox == bCheckbox) return;
	m_bCheckBox = bCheckbox;
	invalidate();
}

bool buttonUC::isSelected() const
{
	return m_bSelected;
}
void buttonUC::selected(bool bSelected /*= true*/)
{
	if(m_bSelected == bSelected) return;
	m_bSelected = bSelected;
	invalidate();
}

bool buttonUC::switchSelected()
{
	m_bSelected = !m_bSelected;
	invalidate();
	return m_bSelected;
}

bool buttonUC::getNormalImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_NORMAL, &rc);
}

void buttonUC::setNormalImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_NORMAL, rcImgCell);
	invalidate();
}
void buttonUC::setNormalImage(LONG left, LONG top, LONG right, LONG bottom)
{
	assert(m_pImage);
	RECT rc = {left, top, right, bottom};
	setNormalImage(rc);
}
bool buttonUC::getHotImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_HOT, &rc);
}

void buttonUC::setHotImage(RECT rcImgCell)
{
	if (rcImgCell.right == 20 && rcImgCell.bottom == 12)
	{
		int s = 0;
		s = 1;
	}
	setCellImage(UCIMAGE_STATE_HOT, rcImgCell);
	invalidate();
}

void buttonUC::setHotImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setHotImage(rc);
}
bool buttonUC::getPushedImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_PUSHED, &rc);
}

void buttonUC::setPushedImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_PUSHED, rcImgCell);
	invalidate();
}

void buttonUC::setPushedImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setPushedImage(rc);
}
bool buttonUC::getFocusedImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_FOCUSED, &rc);
}

void buttonUC::setFocusedImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_FOCUSED, rcImgCell);
	invalidate();
}

void buttonUC::setFocusedImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setFocusedImage(rc);
}
bool buttonUC::getDisabledImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_DISABLED, &rc);
}

void buttonUC::setDisabledImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_DISABLED, rcImgCell);
	invalidate();
}
void buttonUC::setDisabledImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setDisabledImage(rc);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//	2015-1-23	

bool buttonUC::getNormalImageCheck(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_NORMAL|UCIMAGE_STATE_CHECKED, &rc);
}

void buttonUC::setNormalImageCheck(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_NORMAL|UCIMAGE_STATE_CHECKED, rcImgCell);
	invalidate();
}

void buttonUC::setNormalImageCheck(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setNormalImageCheck(rc);
}
bool buttonUC::getHotImageCheck(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_HOT|UCIMAGE_STATE_CHECKED, &rc);
}

void buttonUC::setHotImageCheck(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_HOT|UCIMAGE_STATE_CHECKED, rcImgCell);
	invalidate();
}
void buttonUC::setHotImageCheck(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setHotImageCheck(rc);
}

bool buttonUC::getPushedImageCheck(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_PUSHED|UCIMAGE_STATE_CHECKED, &rc);
}

void buttonUC::setPushedImageCheck(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_PUSHED|UCIMAGE_STATE_CHECKED, rcImgCell);
	invalidate();
}
void buttonUC::setPushedImageCheck(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setPushedImageCheck(rc);
}

bool buttonUC::getFocusedImageCheck(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_FOCUSED|UCIMAGE_STATE_CHECKED, &rc);
}

void buttonUC::setFocusedImageCheck(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_FOCUSED|UCIMAGE_STATE_CHECKED, rcImgCell);
	invalidate();
}
void buttonUC::setFocusedImageCheck(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setFocusedImageCheck(rc);
}
bool buttonUC::getDisabledImageCheck(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_DISABLED|UCIMAGE_STATE_CHECKED, &rc);
}

void buttonUC::setDisabledImageCheck(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_DISABLED|UCIMAGE_STATE_CHECKED, rcImgCell);
	invalidate();
}
void buttonUC::setDisabledImageCheck(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setDisabledImageCheck(rc);
}

void buttonUC::setIconFixedSize(int cxcy)
{
	setIconFixedSize(cxcy, cxcy);
}
void buttonUC::setIconFixedSize(SIZE sz)
{
	setIconFixedSize(sz.cx, sz.cy);
}
void buttonUC::setIconFixedSize(int cx, int cy)
{
	if(m_szIconFixed.cx == cx && m_szIconFixed.cy == cy) return;
	m_szIconFixed.cx = cx;
	m_szIconFixed.cy = cy;
	invalidate();
}
SIZE buttonUC::getIconFixedSize() const
{
	return m_szIconFixed;
}
//void buttonUC::setIconFixedWidth(int width)
//{
//	if(m_iIconFixedWidth == width) return ;
//	m_iIconFixedWidth = width;
//	invalidate();
//}
//int buttonUC::getIconFixedWidth() const
//{
//	return m_iIconFixedWidth > 0 ? m_iIconFixedWidth : rectHeight(m_rcItem);
//}
void buttonUC::setIconSpace(int space)
{
	if(m_iIconSpace == space) return;
	m_iIconSpace = space;
	invalidate();
}
int buttonUC::getIconSpace() const
{
	return m_iIconSpace < 0 ? 3 : m_iIconSpace;
}
RECT buttonUC::getIconArea(RECT *prcText /*= NULL*/) const
{
	int iconSpace = getIconSpace();
	RECT rcIcon = m_rcItem;
	switch(m_beIconPosition)
	{
	case iconBeLeft:
		rcIcon.right = rcIcon.left + iconSpace * 2 + m_szIconFixed.cx;
		if (prcText)
		{
			*prcText = m_rcItem;
			prcText->left = rcIcon.right;
		}
		::InflateRect(&rcIcon, -iconSpace, -(rectHeight(m_rcItem) - m_szIconFixed.cy)/2);		// 垂直居中
		break;
	case iconBeRight:
		rcIcon.left = rcIcon.right - iconSpace * 2 - m_szIconFixed.cx;
		if (prcText)
		{
			*prcText = m_rcItem;
			prcText->right = rcIcon.left;
		}
		::InflateRect(&rcIcon, -iconSpace, -(rectHeight(m_rcItem) - m_szIconFixed.cy)/2);		// 垂直居中
		break;
	case iconBeTop:
		rcIcon.bottom = rcIcon.top + iconSpace * 2 + m_szIconFixed.cy;
		if (prcText)
		{
			*prcText = m_rcItem;
			prcText->top = rcIcon.bottom;
		}
		::InflateRect(&rcIcon, -(rectWidth(m_rcItem) - m_szIconFixed.cx)/2, -iconSpace);		// 水平居中
		break;
	case iconBeBottom:
		rcIcon.top = rcIcon.bottom - iconSpace * 2 - m_szIconFixed.cy;
		if (prcText)
		{
			*prcText = m_rcItem;
			prcText->bottom = rcIcon.top;
		}
		::InflateRect(&rcIcon, -(rectWidth(m_rcItem) - m_szIconFixed.cx)/2, -iconSpace);		// 水平居中
		break;
	}
	rcIcon.right = rcIcon.left + m_szIconFixed.cx;
	rcIcon.bottom = rcIcon.top + m_szIconFixed.cy;
	return rcIcon;
}
//RECT buttonUC::getIconArea(const RECT *prcSrc /*= NULL*/) const
//{
//	RECT rcIcon = m_rcItem;
//	assert(m_pimgIcon);
//	if(m_bFrontIcon) rcIcon.right = rcIcon.left + getIconFixedWidth();
//	else rcIcon.left = rcIcon.right - getIconFixedWidth();
//	if (prcSrc)
//	{
//		RECT rcSrc = *prcSrc;
//		if(rectHeight(rcIcon) > rectHeight(rcSrc))
//		{
//			::OffsetRect(&rcIcon, 0, (rectHeight(rcIcon) - rectHeight(rcSrc))/2);
//			rcIcon.bottom = rcIcon.top + rectHeight(rcSrc);
//		}
//		if (rectWidth(rcIcon) > rectWidth(rcSrc))
//		{
//			::OffsetRect(&rcIcon, (rectWidth(rcIcon) - rectWidth(rcSrc))/2, 0);
//			rcIcon.right = rcIcon.left + rectWidth(rcSrc);
//		}
//	}
//	return rcIcon;
//}

void buttonUC::setIconPosition(_BEIconPosition pos)
{
	if(m_beIconPosition == pos) return;
	m_beIconPosition = pos;
	invalidate();
}
buttonUC::_BEIconPosition buttonUC::getIconPosition() const
{
	return m_beIconPosition;
}

//void buttonUC::setIconFront(bool bBefore)
//{
//	if(m_bFrontIcon == bBefore) return;
//	m_bFrontIcon = bBefore;
//	invalidate();
//}
//bool buttonUC::isFrontIcon() const
//{
//	return m_bFrontIcon;
//}
void buttonUC::setIcon(PCWSTR pImage)
{
	gtc::image *pimg = analyzeImage(pImage);
	setIcon(pimg);
}
void buttonUC::setIcon(gtc::image *pImage)
{
	if(m_pimgIcon == pImage) return;
	m_pimgIcon = pImage;
	invalidate();
}
void buttonUC::clearIcon()
{
	m_pimgIcon = NULL;
	invalidate();
}
gtc::image* buttonUC::getIcon() const
{
	return m_pimgIcon;
}
bool buttonUC::getNormalIcon(RECT &rc)
{
	return existsImage(m_mapIcon, UCIMAGE_STATE_NORMAL, &rc);
}
void buttonUC::setNormalIcon(RECT rcImgCell)
{
	setCellImage(m_mapIcon, UCIMAGE_STATE_NORMAL, rcImgCell);
	invalidate();
}
void buttonUC::setNormalIcon(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setNormalIcon(rc);
}
bool buttonUC::getHotIcon(RECT &rc)
{
	return existsImage(m_mapIcon, UCIMAGE_STATE_HOT, &rc);
}
void buttonUC::setHotIcon(RECT rcImgCell)
{
	setCellImage(m_mapIcon, UCIMAGE_STATE_HOT, rcImgCell);
	invalidate();
}
void buttonUC::setHotIcon(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setHotIcon(rc);
}
bool buttonUC::getPushedIcon(RECT &rc)
{
	return existsImage(m_mapIcon, UCIMAGE_STATE_PUSHED, &rc);
}
void buttonUC::setPushedIcon(RECT rcImgCell)
{
	setCellImage(m_mapIcon, UCIMAGE_STATE_PUSHED, rcImgCell);
	invalidate();
}
void buttonUC::setPushedIcon(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setPushedIcon(rc);
}
bool buttonUC::getFocusedIcon(RECT &rc)
{
	return existsImage(m_mapIcon, UCIMAGE_STATE_FOCUSED, &rc);
}
void buttonUC::setFocusedIcon(RECT rcImgCell)
{
	setCellImage(m_mapIcon, UCIMAGE_STATE_FOCUSED, rcImgCell);
	invalidate();
}
void buttonUC::setFocusedIcon(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setFocusedIcon(rc);
}
bool buttonUC::getDisabledIcon(RECT &rc)
{
	return existsImage(m_mapIcon, UCIMAGE_STATE_DISABLED, &rc);
}
void buttonUC::setDisabledIcon(RECT rcImgCell)
{
	setCellImage(m_mapIcon, UCIMAGE_STATE_DISABLED, rcImgCell);
	invalidate();
}
void buttonUC::setDisabledIcon(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setDisabledIcon(rc);
}

bool buttonUC::getNormalIconCheck(RECT &rc)
{
	return existsImage(m_mapIcon, UCIMAGE_STATE_NORMAL|UCIMAGE_STATE_CHECKED, &rc);
}
void buttonUC::setNormalIconCheck(RECT rcImgCell)
{
	setCellImage(m_mapIcon, UCIMAGE_STATE_NORMAL|UCIMAGE_STATE_CHECKED, rcImgCell);
	invalidate();
}
void buttonUC::setNormalIconCheck(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setNormalIconCheck(rc);
}
bool buttonUC::getHotIconCheck(RECT &rc)
{
	return existsImage(m_mapIcon, UCIMAGE_STATE_HOT|UCIMAGE_STATE_CHECKED, &rc);
}
void buttonUC::setHotIconCheck(RECT rcImgCell)
{
	setCellImage(m_mapIcon, UCIMAGE_STATE_HOT|UCIMAGE_STATE_CHECKED, rcImgCell);
	invalidate();
}
void buttonUC::setHotIconCheck(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setHotIconCheck(rc);
}
bool buttonUC::getPushedIconCheck(RECT &rc)
{
	return existsImage(m_mapIcon, UCIMAGE_STATE_PUSHED|UCIMAGE_STATE_CHECKED, &rc);
}
void buttonUC::setPushedIconCheck(RECT rcImgCell)
{
	setCellImage(m_mapIcon, UCIMAGE_STATE_PUSHED|UCIMAGE_STATE_CHECKED, rcImgCell);
	invalidate();
}
void buttonUC::setPushedIconCheck(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setPushedIconCheck(rc);
}
bool buttonUC::getFocusedIconCheck(RECT &rc)
{
	return existsImage(m_mapIcon, UCIMAGE_STATE_FOCUSED|UCIMAGE_STATE_CHECKED, &rc);
}
void buttonUC::setFocusedIconCheck(RECT rcImgCell)
{
	setCellImage(m_mapIcon, UCIMAGE_STATE_FOCUSED|UCIMAGE_STATE_CHECKED, rcImgCell);
	invalidate();
}
void buttonUC::setFocusedIconCheck(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setFocusedIconCheck(rc);
}
bool buttonUC::getDisabledIconCheck(RECT &rc)
{
	return existsImage(m_mapIcon, UCIMAGE_STATE_DISABLED|UCIMAGE_STATE_CHECKED, &rc);
}
void buttonUC::setDisabledIconCheck(RECT rcImgCell)
{
	setCellImage(m_mapIcon, UCIMAGE_STATE_DISABLED|UCIMAGE_STATE_CHECKED, rcImgCell);
	invalidate();
}
void buttonUC::setDisabledIconCheck(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setDisabledIconCheck(rc);
}

SIZE buttonUC::estimateSize(SIZE szAvailable)
{
	if( m_szxyFixed.cy == 0 )
	{
		SIZE sz = {m_szxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 8};
		return sz;
	}
	return controlUC::estimateSize(szAvailable);
}
SIZE buttonUC::autoCalculateSize()
{
	SIZE sz = __super::autoCalculateSize();
	if (m_pimgIcon)
	{
		int iconSpace = getIconSpace();
		if (m_beIconPosition == iconBeLeft || m_beIconPosition == iconBeRight)
		{
			sz.cx += iconSpace * 2 + m_szIconFixed.cx;
		}
		else
		{
			sz.cy += iconSpace * 2 + m_szIconFixed.cy;
		}
	}

	return sz;
}
void buttonUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = NULL;
	/*if( _tcscmp(pstrName, L"image") == 0 ) setImage(pstrValue);
	else */if( _tcscmp(pstrName, L"selected") == 0 ) selected(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"ascheckbox") == 0 ) setCheckBox(_tcscmp(pstrValue, L"true") == 0);	// ZDY
	else if( _tcscmp(pstrName, L"icon") == 0 ) setIcon(pstrValue);
	else if( _tcscmp(pstrName, L"iconfixedsize") == 0 ) {
		SIZE sz = { 0 };
		sz.cx = _tcstol(pstrValue, &pstr, 10);	assert(pstr);    
		sz.cy = _tcstol(pstr + 1, &pstr, 10);	assert(pstr);
		setIconFixedSize(sz);

	}
	else if( _tcscmp(pstrName, L"normalimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setNormalImage(rcCell);
		//setNormalImage(_tcstol(pstrValue, &pstr, 10));
	}
	else if( _tcscmp(pstrName, L"hotimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setHotImage(rcCell);
		//setHotImage(_tcstol(pstrValue, &pstr, 10));
	}
	else if( _tcscmp(pstrName, L"pushedimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setPushedImage(rcCell);
		//setPushedImage(_tcstol(pstrValue, &pstr, 10));
	}
	else if( _tcscmp(pstrName, L"focusedimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setFocusedImage(rcCell);
		//setFocusedImage(_tcstol(pstrValue, &pstr, 10));
	}
	else if( _tcscmp(pstrName, L"disabledimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setDisabledImage(rcCell);
		//setDisabledImage(_tcstol(pstrValue, &pstr, 10));
	}
	else if( _tcscmp(pstrName, L"normalimagecheck") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setNormalImageCheck(rcCell);
		//setNormalImage(_tcstol(pstrValue, &pstr, 10));
	}
	else if( _tcscmp(pstrName, L"hotimagecheck") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setHotImageCheck(rcCell);
		//setHotImage(_tcstol(pstrValue, &pstr, 10));
	}
	else if( _tcscmp(pstrName, L"pushedimagecheck") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setPushedImageCheck(rcCell);
		//setPushedImage(_tcstol(pstrValue, &pstr, 10));
	}
	else if( _tcscmp(pstrName, L"focusedimagecheck") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setFocusedImageCheck(rcCell);
		//setFocusedImage(_tcstol(pstrValue, &pstr, 10));
	}
	else if( _tcscmp(pstrName, L"disabledimagecheck") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setDisabledImageCheck(rcCell);
		//setDisabledImage(_tcstol(pstrValue, &pstr, 10));
	}



	else if( _tcscmp(pstrName, L"normalicon") == 0 ) {
		setNormalIcon(gtc::format::rect(pstrValue));
	}
	else if( _tcscmp(pstrName, L"hoticon") == 0 ) {
		setHotIcon(gtc::format::rect(pstrValue));
	}
	else if( _tcscmp(pstrName, L"pushedicon") == 0 ) {
		setPushedIcon(gtc::format::rect(pstrValue));
	}
	else if( _tcscmp(pstrName, L"focusedicon") == 0 ) {
		setFocusedIcon(gtc::format::rect(pstrValue));
	}
	else if( _tcscmp(pstrName, L"disabledicon") == 0 ) {
		setDisabledIcon(gtc::format::rect(pstrValue));
	}
	else if( _tcscmp(pstrName, L"normaliconcheck") == 0 ) {
		setNormalIconCheck(gtc::format::rect(pstrValue));
	}
	else if( _tcscmp(pstrName, L"hoticoncheck") == 0 ) {
		setHotIconCheck(gtc::format::rect(pstrValue));
	}
	else if( _tcscmp(pstrName, L"pushediconcheck") == 0 ) {
		setPushedIconCheck(gtc::format::rect(pstrValue));
	}
	else if( _tcscmp(pstrName, L"focusediconcheck") == 0 ) {
		setFocusedIconCheck(gtc::format::rect(pstrValue));
	}
	else if( _tcscmp(pstrName, L"disablediconcheck") == 0 ) {
		setDisabledIconCheck(gtc::format::rect(pstrValue));
	}

	else if( _tcscmp(pstrName, L"hottextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setHotTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"pushedtextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setPushedTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"focusedtextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setFocusedTextColor(clrColor);
	}

	else if( _tcscmp(pstrName, L"textcolorcheck") == 0 ) {
		setTextColorCheck(gtc::format::color(pstrValue));
	}
	else if( _tcscmp(pstrName, L"hottextcolorcheck") == 0 ) {
		setHotTextColorCheck(gtc::format::color(pstrValue));
	}
	else if( _tcscmp(pstrName, L"pushedtextcolorcheck") == 0 ) {
		setPushedTextColorCheck(gtc::format::color(pstrValue));
	}
	else if( _tcscmp(pstrName, L"focusedtextcolorcheck") == 0 ) {
		setFocusedTextColorCheck(gtc::format::color(pstrValue));
	}
	else if( _tcscmp(pstrName, L"disabledtextcolorcheck") == 0 ) {
		setDisabledTextColorCheck(gtc::format::color(pstrValue));
	}

	else __super::setAttribute(pstrName, pstrValue);
}

void buttonUC::paintText(HDC hdc)
{
	if( isFocused() ) m_uButtonState |= UCSTATE_FOCUSED;
	else m_uButtonState &= ~ UCSTATE_FOCUSED;
	if( !isEnabled() ) m_uButtonState |= UCSTATE_DISABLED;
	else m_uButtonState &= ~ UCSTATE_DISABLED;

	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->getDefaultFontColor();
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->getDefaultDisabledColor();

	RECT rcText = m_rcItem;
	if(m_pimgIcon) getIconArea(&rcText);
	rcText.left += m_rcTextPadding.left;
	rcText.right -= m_rcTextPadding.right;
	rcText.top += m_rcTextPadding.top;
	rcText.bottom -= m_rcTextPadding.bottom;

	//RECT rc = m_rcItem;
	//rc.left += m_rcTextPadding.left;
	//rc.right -= m_rcTextPadding.right;
	//rc.top += m_rcTextPadding.top;
	//rc.bottom -= m_rcTextPadding.bottom;
	//if (m_pimgIcon)
	//{
	//	int iconW = 0;
	//	if(m_iIconFixedWidth > 0) iconW = m_iIconFixedWidth;
	//	else iconW = rectHeight(m_rcItem);
	//	if(m_bFrontIcon) rc.left += iconW;
	//	else rc.right -= iconW;
	//}
	if( m_wsText.empty() )
	{
		drawTextImage(hdc, rcText);
		return;
	}
	Gdiplus::Graphics grap(hdc);
	int nLinks = 0;

	DWORD clrColor = isEnabled()?m_dwTextColor:m_dwDisabledTextColor;
	if(m_bSelected)
	{
		if (isEnabled())
		{
			if(m_dwTextColorCheck != 0) clrColor = m_dwTextColorCheck;
		}
		else if(m_dwDisabledTextColorCheck != 0) clrColor = m_dwDisabledTextColorCheck;
	}
	if( ((m_uButtonState & UCSTATE_PUSHED) != 0) )
	{
		if(m_bCheckBox && m_bSelected)
		{
			if(m_dwPushedTextColorCheck != 0) clrColor = m_dwPushedTextColorCheck;
		}
		else if(getPushedTextColor() != 0) clrColor = getPushedTextColor();
	}
	else if( ((m_uButtonState & UCSTATE_HOT) != 0) )
	{
		if(m_bCheckBox && m_bSelected)
		{
			if(m_dwHotTextColorCheck != 0) clrColor = m_dwHotTextColorCheck;
		}
		else if(getHotTextColor() != 0) clrColor = getHotTextColor();
	}
	else if( ((m_uButtonState & UCSTATE_FOCUSED) != 0) )
	{
		if (m_bCheckBox && m_bSelected)
		{
			if(m_dwFocusedTextColorCheck != 0) clrColor = m_dwFocusedTextColorCheck;
		}
		else if(getFocusedTextColor() != 0) clrColor = getFocusedTextColor();
	}
	if( m_bShowHtml )
	{
		//renderEngine::drawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), clrColor, \
		//NULL, NULL, nLinks, m_uTextStyle);
	}
	else if (m_bShadowText)
	{
#if defined(WUC_SUPPORT_THEME)
		acm::theme themeShadow;
		if (themeShadow.open(m_pManager->getPaintWindow(), L"TextStyle"))
		{
			bool bTheme = themeShadow.drawShadowText(hdc, m_wsText.c_str(), &rc, clrColor);
			themeShadow.close();
		}

#endif
	}
	else
	{
		::OffsetRect(&rcText, m_szOffsetXY.cx, m_szOffsetXY.cy);
		gtc::drawer::drawString(grap, m_wsText.c_str(), rcText, m_pManager->getFont(m_iFont), /*(BEAlignMode)*/m_uiTextStyle/*|DT_SINGLELINE|DT_VCENTER*/, clrColor);
		
		//renderEngine::drawText(hDC, m_pManager, rc, m_sText.c_str(), clrColor, \
		//m_iFont, m_uTextStyle);
	}
}

void buttonUC::paintStatusImage(HDC hdc)
{
	if(!m_pImage && !m_pimgIcon) return;
	if( isFocused() ) m_uButtonState |= UCSTATE_FOCUSED;
	else m_uButtonState &= ~ UCSTATE_FOCUSED;
	if( !isEnabled() ) m_uButtonState |= UCSTATE_DISABLED;
	else m_uButtonState &= ~ UCSTATE_DISABLED;
	Gdiplus::Graphics grap(hdc);
	RECT rcSrcImage = {0};
	RECT rcSrcIcon = {0};
	//RECT rcIcon = {0};
	bool bIcon = false;
	bool bImage = false;

	if( (m_uButtonState & UCSTATE_DISABLED) != 0 ) {
		if (m_bCheckBox && m_bSelected)
		{
			bImage = getDisabledImageCheck(rcSrcImage);
		}
		else
		{
			bImage = getDisabledImage(rcSrcImage);
		}
		//if(bl) bl = gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
		if (m_pimgIcon)
		{
			if (m_bCheckBox && m_bSelected)
			{
				bIcon = getDisabledIconCheck(rcSrcIcon);
			}
			else
			{
				bIcon = getDisabledIcon(rcSrcIcon);
			}
			//if(!getDisabledIcon(rcSrc)) getNormalIcon(rcSrc);
			//if (bl)
			//{
			//	RECT rcIcon = getIconArea(&rcSrc);
			//	bl = gtc::drawer::drawImage(grap, m_pimgIcon, rcIcon, rcSrc);
			//}
		}
		//if(bl) return;
	}
	else if( (m_uButtonState & UCSTATE_PUSHED) != 0 ) {
		if (m_bCheckBox && m_bSelected)
		{
			bImage = getPushedImageCheck(rcSrcImage);
		}
		else
		{
			bImage = getPushedImage(rcSrcImage);
		}
		//if(bl) bl = gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
		if (m_pimgIcon)
		{
			if (m_bCheckBox && m_bSelected)
			{
				bIcon = getPushedIconCheck(rcSrcIcon);
			}
			else
			{
				bIcon = getPushedIcon(rcSrcIcon);
			}
			//if(!getPushedIcon(rcSrc)) getNormalIcon(rcSrc);
			//if (bl)
			//{
			//	RECT rcIcon = getIconArea(&rcSrc);
			//	bl = gtc::drawer::drawImage(grap, m_pimgIcon, rcIcon, rcSrc);
			//}
		}
		//if(bl) return;
	}
	else if( (m_uButtonState & UCSTATE_HOT) != 0 ) {
		if (m_bCheckBox && m_bSelected)
		{
			bImage = getHotImageCheck(rcSrcImage);
		}
		else
		{
			bImage = getHotImage(rcSrcImage);
		}
		//if(bl) bl = gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
		if (m_pimgIcon)
		{
			//if(!getHotIcon(rcSrc)) getNormalIcon(rcSrc);
			if (m_bCheckBox && m_bSelected)
			{
				bIcon = getHotIconCheck(rcSrcIcon);
			}
			else
			{
				bIcon = getHotIcon(rcSrcIcon);
			}
			//if (bl)
			//{
			//	RECT rcIcon = getIconArea();
			//	bl = gtc::drawer::drawImage(grap, m_pimgIcon, rcIcon, rcSrc);
			//}
		}
		//if(bl) return;
	}
	else if( (m_uButtonState & UCSTATE_FOCUSED) != 0 ) {
		if (m_bCheckBox && m_bSelected)
		{
			bImage = getFocusedImageCheck(rcSrcImage);
		}
		else
		{
			bImage = getFocusedImage(rcSrcImage);
		}
		//if(bl) bl = gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
		if (m_pimgIcon)
		{
			if (m_bCheckBox && m_bSelected)
			{
				bIcon = getFocusedIconCheck(rcSrcIcon);
			}
			else
			{
				bIcon = getFocusedIcon(rcSrcIcon);
			}
			//if (bl)
			//{
			//	RECT rcIcon = getIconArea(&rcSrc);
			//	bl = gtc::drawer::drawImage(grap, m_pimgIcon, rcIcon, rcSrc);
			//}
		}
		//if(bl) return;
	}
	if (!bImage)
	{
		if (m_bCheckBox && m_bSelected)
		{
			bImage = getNormalImageCheck(rcSrcImage);
		}
		if (!bImage)	// 最后补救，所以不用else
		{
			bImage = getNormalImage(rcSrcImage);
		}
	}

	if(bImage) bImage = gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrcImage);
	if(m_pimgIcon)
	{
		if (!bIcon)
		{
			if (m_bCheckBox && m_bSelected)
			{
				bIcon = getNormalIconCheck(rcSrcIcon);
			}
			if (!bIcon)
			{
				bIcon = getNormalIcon(rcSrcIcon);
			}
		}
		if (bIcon)
		{
			RECT rcIcon = getIconArea();
			gtc::drawer::drawImage(grap, m_pimgIcon, rcIcon, rcSrcIcon);
		}
	}

	//if( (m_uButtonState & UCSTATE_DISABLED) != 0 ) {
	//	if (m_bCheckBox && m_bSelected)
	//	{
	//		bl = getDisabledImageCheck(rcSrc);
	//	}
	//	else
	//	{
	//		bl = getDisabledImage(rcSrc);
	//	}
	//	if(bl) bl = gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
	//	if (m_pimgIcon)
	//	{
	//		if (m_bCheckBox && m_bSelected)
	//		{
	//			bl = getDisabledIconCheck(rcSrc);
	//		}
	//		else
	//		{
	//			bl = getDisabledIcon(rcSrc);
	//		}
	//		//if(!getDisabledIcon(rcSrc)) getNormalIcon(rcSrc);
	//		if (bl)
	//		{
	//			RECT rcIcon = getIconArea(&rcSrc);
	//			bl = gtc::drawer::drawImage(grap, m_pimgIcon, rcIcon, rcSrc);
	//		}
	//	}
	//	if(bl) return;
	//}
	//else if( (m_uButtonState & UCSTATE_PUSHED) != 0 ) {
	//	if (m_bCheckBox && m_bSelected)
	//	{
	//		bl = getPushedImageCheck(rcSrc);
	//	}
	//	else
	//	{
	//		bl = getPushedImage(rcSrc);
	//	}
	//	if(bl) bl = gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
	//	if (m_pimgIcon)
	//	{
	//		if (m_bCheckBox && m_bSelected)
	//		{
	//			bl = getPushedIconCheck(rcSrc);
	//		}
	//		else
	//		{
	//			bl = getPushedIcon(rcSrc);
	//		}
	//		//if(!getPushedIcon(rcSrc)) getNormalIcon(rcSrc);
	//		if (bl)
	//		{
	//			RECT rcIcon = getIconArea(&rcSrc);
	//			bl = gtc::drawer::drawImage(grap, m_pimgIcon, rcIcon, rcSrc);
	//		}
	//	}
	//	if(bl) return;
	//}
	//else if( (m_uButtonState & UCSTATE_HOT) != 0 ) {
	//	if (m_bCheckBox && m_bSelected)
	//	{
	//		bl = getHotImageCheck(rcSrc);
	//	}
	//	else
	//	{
	//		bl = getHotImage(rcSrc);
	//	}
	//	if(bl) bl = gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
	//	if (m_pimgIcon)
	//	{
	//		//if(!getHotIcon(rcSrc)) getNormalIcon(rcSrc);
	//		if (m_bCheckBox && m_bSelected)
	//		{
	//			bl = getHotIconCheck(rcSrc);
	//		}
	//		else
	//		{
	//			bl = getHotIcon(rcSrc);
	//		}
	//		if (bl)
	//		{
	//			RECT rcIcon = getIconArea();
	//			bl = gtc::drawer::drawImage(grap, m_pimgIcon, rcIcon, rcSrc);
	//		}
	//	}
	//	if(bl) return;
	//}
	//else if( (m_uButtonState & UCSTATE_FOCUSED) != 0 ) {
	//	if (m_bCheckBox && m_bSelected)
	//	{
	//		bl = getFocusedImageCheck(rcSrc);
	//	}
	//	else
	//	{
	//		bl = getFocusedImage(rcSrc);
	//	}
	//	if(bl) bl = gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
	//	if (m_pimgIcon)
	//	{
	//		if (m_bCheckBox && m_bSelected)
	//		{
	//			bl = getFocusedIconCheck(rcSrc);
	//		}
	//		else
	//		{
	//			bl = getFocusedIcon(rcSrc);
	//		}
	//		if (bl)
	//		{
	//			RECT rcIcon = getIconArea(&rcSrc);
	//			bl = gtc::drawer::drawImage(grap, m_pimgIcon, rcIcon, rcSrc);
	//		}
	//	}
	//	if(bl) return;
	//}
	//
	//if (m_bCheckBox && m_bSelected)
	//{
	//	bl = getNormalImageCheck(rcSrc);
	//}
	//else
	//{
	//	bl = getNormalImage(rcSrc);
	//}
	//if(bl) bl = gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
	//if(m_pimgIcon)
	//{
	//	if (m_bCheckBox && m_bSelected)
	//	{
	//		bl = getNormalIconCheck(rcSrc);
	//	}
	//	else
	//	{
	//		bl = getNormalIcon(rcSrc);
	//	}
	//	if (bl)
	//	{
	//		RECT rcIcon = getIconArea();
	//		gtc::drawer::drawImage(grap, m_pimgIcon, rcIcon, rcSrc);
	//	}
	//}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	optionUC  类		2015-7-27
//	按钮组内只能单选的按钮
////////////////////////////////////////////////////////////////////////////////////////////////////////
optionUC::optionUC()
	: m_dwSelectedTextColor(0)
{

}
optionUC::~optionUC()
{
	if(!m_wsGroupName.empty() && m_pManager) m_pManager->removeOptionGroup(m_wsGroupName.c_str(), this);
}

PCWSTR optionUC::getClass() const
{
	return L"optionUC";
}
LPVOID optionUC::getInterface(PCWSTR pstrName)
{
	if( std::wcscmp(pstrName, L"Option") == 0 ) return static_cast<optionUC *>(this);
	return __super::getInterface(pstrName);
}

void optionUC::setManager(drawManager *pManager, controlUC *pParent, bool bInit /*= true*/)
{
	controlUC::setManager(pManager, pParent, bInit);
	if( !m_wsGroupName.empty() && bInit ) {
		if (m_pManager) m_pManager->addOptionGroup(m_wsGroupName.c_str(), this);
	}
}

std::wstring optionUC::getGroup() const
{
	return m_wsGroupName;
}
void optionUC::setGroup(PCWSTR pstrGroupName /*= NULL*/)
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
	selected(m_bSelected);
	/*if( pStrGroupName == NULL ) {
	if( m_sGroupName.IsEmpty() ) return;
	m_sGroupName.Empty();
	}
	else {
	if( m_sGroupName == pStrGroupName ) return;
	if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
	m_sGroupName = pStrGroupName;
	}

	if( !m_sGroupName.IsEmpty() ) {
	if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
	}
	else {
	if (m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
	}

	Selected(m_bSelected);*/
}
//bool optionUC::isSelected() const
//{
//	return m_bSelected;
//}
void optionUC::selected(bool bSelected /*= true*/)
{
	if(m_bSelected == bSelected) return;
	m_bSelected = bSelected;
	if(m_bSelected) m_uButtonState |= UCSTATE_SELECTED;
	else m_uButtonState &= ~UCSTATE_SELECTED;

	if (m_pManager)
	{
		if (!m_wsGroupName.empty())
		{
			if (m_bSelected)
			{
				ControlArrayType vecOption = m_pManager->getOptionGroup(m_wsGroupName.c_str());
				for (ControlArrayType::iterator iter = vecOption.begin(); iter != vecOption.end(); ++iter)
				{
					optionUC *pControl = static_cast<optionUC *>(*iter);
					assert(pControl);
					if(pControl && pControl != this) pControl->selected(false);
				}
			}
		}
		else
		{
			m_pManager->sendNotify(this, L"selectchanged");
		}
	}
	invalidate();
	/*if( m_bSelected == bSelected ) return;
	m_bSelected = bSelected;
	if( m_bSelected ) m_uButtonState |= UISTATE_SELECTED;
	else m_uButtonState &= ~UISTATE_SELECTED;

	if( m_pManager != NULL ) {
		if( !m_sGroupName.IsEmpty() ) {
			if( m_bSelected ) {
				CStdPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
				for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
					COptionUI* pControl = static_cast<COptionUI*>(aOptionGroup->GetAt(i));
					if( pControl != this ) {
						pControl->Selected(false);
					}
				}
				m_pManager->SendNotify(this, _T("selectchanged"));
			}
		}
		else {
			m_pManager->SendNotify(this, _T("selectchanged"));
		}
	}

	Invalidate();*/
}
bool optionUC::activate()
{
	if( !buttonUC::activate() ) return false;
	if( !m_wsGroupName.empty() ) selected(true);
	else selected(!m_bSelected);

	return true;
}
void optionUC::setEnabled(bool bEnable /*= true*/)
{
	controlUC::setEnabled(bEnable);
	if( !isEnabled() ) {
		if( m_bSelected ) m_uButtonState = UCSTATE_SELECTED;
		else m_uButtonState = 0;
	}
}
bool optionUC::getSelectedImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_SELECTED, &rc);
}
void optionUC::setSelectedImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_SELECTED, rcImgCell);
	invalidate();
}
void optionUC::setSelectedImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setSelectedImage(rc);
}

bool optionUC::getSelectedIcon(RECT &rc)
{
	return existsImage(m_mapIcon, UCIMAGE_STATE_SELECTED, &rc);
}
void optionUC::setSelectedIcon(RECT rcImgCell)
{
	setCellImage(m_mapIcon, UCIMAGE_STATE_SELECTED, rcImgCell);
}
void optionUC::setSelectedIcon(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setSelectedIcon(rc);
}

void optionUC::setSelectedTextColor(DWORD dwTextColor)
{
	m_dwSelectedTextColor = dwTextColor;
}
DWORD optionUC::getSelectedTextColor()
{
	return m_dwSelectedTextColor;
}
SIZE optionUC::estimateSize(SIZE szAvailable)
{
	if( m_szxyFixed.cy == 0 )
	{
		SIZE sz = {m_szxyFixed.cx, m_pManager->findFont(m_iFont)->tm.tmHeight + 8};
		return sz;
	}
	return controlUC::estimateSize(szAvailable);
}
void optionUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = NULL;
	if( _tcscmp(pstrName, L"group") == 0 ) setGroup(pstrValue);
	else if( _tcscmp(pstrName, L"selected") == 0 ) selected(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, _T("selectedimage")) == 0 )
	{
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setSelectedImage(rcCell);
		//SetSelectedImage(pstrValue);
	}
	//else if( _tcscmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
	else if( _tcscmp(pstrName, _T("selectedtextcolor")) == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setSelectedTextColor(clrColor);
	}
	else __super::setAttribute(pstrName, pstrValue);
}

void optionUC::paintStatusImage(HDC hdc)
{
	Gdiplus::Graphics grap(hdc);
	RECT rcSrc = {0};
	bool bl;
	m_uButtonState &= ~UCSTATE_PUSHED;

	if( (m_uButtonState & UCSTATE_SELECTED) != 0 ) {
		if (m_pImage)
		{
			bl = getSelectedImage(rcSrc);
			if(bl) bl = gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
			if(bl) return ;
		}
		if (m_pimgIcon)
		{
			bl = getSelectedIcon(rcSrc);
			if(bl)
			{
				RECT rcIcon = getIconArea();
				bl = gtc::drawer::drawImage(grap, m_pimgIcon, rcIcon, rcSrc);
			}
			if(bl) return;
		}
	}

	__super::paintStatusImage(hdc);

}
//void optionUC::paintText(HDC hdc)
//{
//
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	scrollBarUC类	2014-07-17
//	滚动条栏
////////////////////////////////////////////////////////////////////////////////////////////////////////


scrollBarUC::scrollBarUC() 
	: m_pOwner(NULL)
	, m_bHorizontal(false), m_iRange(100), m_iScrollPos(0), m_iLineSize(8)
	, m_iLastScrollPos(0), m_iLastScrollOffset(0), m_iScrollRepeatDelay(0), m_uiButton1State(0)
	, m_uiButton2State(0), m_uiThumbState(0), m_bShowButton1(true), m_bShowButton2(true)
{
	//setImagePatch(DEFAULT_SCROLLBAR_SIZE/4, DEFAULT_SCROLLBAR_SIZE/4);
	m_szxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
	ptLastMouse.x = ptLastMouse.y = 0;
	::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
	::ZeroMemory(&m_rcButton1, sizeof(m_rcButton1));
	::ZeroMemory(&m_rcButton2, sizeof(m_rcButton2));
}

scrollBarUC::~scrollBarUC()
{

}
PCWSTR scrollBarUC::getClass() const
{
	return L"scrollBarUC";
}

LPVOID scrollBarUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"ScrollBar") == 0 ) return static_cast<scrollBarUC*>(this);
	return controlUC::getInterface(pstrName);
}

containerUC* scrollBarUC::getOwner() const
{
	return m_pOwner;
}

void scrollBarUC::setOwner(containerUC* pOwner)
{
	m_pOwner = pOwner;
}

void scrollBarUC::setEnabled(bool bEnable)
{
	controlUC::setEnabled(bEnable);
	if( !isEnabled() ) {
		m_uiButton1State = 0;
		m_uiButton2State = 0;
		m_uiThumbState = 0;
	}
}

void scrollBarUC::setFocus()
{
	if( m_pOwner != NULL ) m_pOwner->setFocus();
	else controlUC::setFocus();
}

bool scrollBarUC::isHorizontal()
{
	return m_bHorizontal;
}

void scrollBarUC::setHorizontal(bool bHorizontal)
{
	if( m_bHorizontal == bHorizontal ) return;

	m_bHorizontal = bHorizontal;
	if( m_bHorizontal ) {
		if( m_szxyFixed.cy == 0 ) {
			m_szxyFixed.cx = 0;
			m_szxyFixed.cy = DEFAULT_SCROLLBAR_SIZE;
		}
	}
	else {
		if( m_szxyFixed.cx == 0 ) {
			m_szxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
			m_szxyFixed.cy = 0;
		}
	}

	if( m_pOwner != NULL ) m_pOwner->needUpdate(); else needParentUpdate();
}

int scrollBarUC::getScrollRange() const
{
	return m_iRange;
}

void scrollBarUC::setScrollRange(int nRange)
{
	if( m_iRange == nRange ) return;

	m_iRange = nRange;
	if( m_iRange < 0 ) m_iRange = 0;
	if( m_iScrollPos > m_iRange ) m_iScrollPos = m_iRange;
	setPosition(m_rcItem);
}

int scrollBarUC::getScrollPos() const
{
	return m_iScrollPos;
}

void scrollBarUC::setScrollPos(int nPos)
{
	if( m_iScrollPos == nPos ) return;

	m_iScrollPos = nPos;
	if( m_iScrollPos < 0 ) m_iScrollPos = 0;
	if( m_iScrollPos > m_iRange ) m_iScrollPos = m_iRange;
	setPosition(m_rcItem);
}

int scrollBarUC::getLineSize() const
{
	return m_iLineSize;
}

void scrollBarUC::setLineSize(int nSize)
{
	m_iLineSize = nSize;
}

bool scrollBarUC::getShowButton1()
{
	return m_bShowButton1;
}

void scrollBarUC::setShowButton1(bool bShow)
{
	m_bShowButton1 = bShow;
	setPosition(m_rcItem);
}

bool scrollBarUC::getButton1NormalImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_BUTTON1|UCIMAGE_STATE_NORMAL, &rc);
}

void scrollBarUC::setButton1NormalImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_BUTTON1|UCIMAGE_STATE_NORMAL, rcImgCell))
		invalidate();
}
void scrollBarUC::setButton1NormalImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setButton1NormalImage(rc);
}
bool scrollBarUC::getButton1HotImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_BUTTON1|UCIMAGE_STATE_HOT, &rc);
}

void scrollBarUC::setButton1HotImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_BUTTON1|UCIMAGE_STATE_HOT, rcImgCell))
		invalidate();
}
void scrollBarUC::setButton1HotImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setButton1HotImage(rc);
}
bool scrollBarUC::getButton1PushedImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_BUTTON1|UCIMAGE_STATE_PUSHED, &rc);
}

void scrollBarUC::setButton1PushedImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_BUTTON1|UCIMAGE_STATE_PUSHED, rcImgCell))
		invalidate();
}
void scrollBarUC::setButton1PushedImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setButton1PushedImage(rc);
}
bool scrollBarUC::getButton1DisabledImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_BUTTON1|UCIMAGE_STATE_DISABLED, &rc);
}

void scrollBarUC::setButton1DisabledImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_BUTTON1|UCIMAGE_STATE_DISABLED, rcImgCell))
		invalidate();
}
void scrollBarUC::setButton1DisabledImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setButton1DisabledImage(rc);
}
bool scrollBarUC::getShowButton2()
{
	return m_bShowButton2;
}

void scrollBarUC::setShowButton2(bool bShow)
{
	m_bShowButton2 = bShow;
	setPosition(m_rcItem);
}

bool scrollBarUC::getButton2NormalImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_BUTTON2|UCIMAGE_STATE_NORMAL, &rc);
}

void scrollBarUC::setButton2NormalImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_BUTTON2|UCIMAGE_STATE_NORMAL, rcImgCell))
		invalidate();
}
void scrollBarUC::setButton2NormalImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setButton2NormalImage(rc);
}
bool scrollBarUC::getButton2HotImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_BUTTON2|UCIMAGE_STATE_HOT, &rc);
}

void scrollBarUC::setButton2HotImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_BUTTON2|UCIMAGE_STATE_HOT, rcImgCell))
		invalidate();
}
void scrollBarUC::setButton2HotImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setButton2HotImage(rc);
}
bool scrollBarUC::getButton2PushedImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_BUTTON2|UCIMAGE_STATE_PUSHED, &rc);
}

void scrollBarUC::setButton2PushedImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_BUTTON2|UCIMAGE_STATE_PUSHED, rcImgCell))
		invalidate();
}
void scrollBarUC::setButton2PushedImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setButton2PushedImage(rc);
}
bool scrollBarUC::getButton2DisabledImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_BUTTON2|UCIMAGE_STATE_DISABLED, &rc);
}

void scrollBarUC::setButton2DisabledImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_BUTTON2|UCIMAGE_STATE_DISABLED, rcImgCell))
		invalidate();
}
void scrollBarUC::setButton2DisabledImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setButton2DisabledImage(rc);
}
bool scrollBarUC::getThumbNormalImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_THUMB|UCIMAGE_STATE_NORMAL, &rc);
}

void scrollBarUC::setThumbNormalImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_THUMB|UCIMAGE_STATE_NORMAL, rcImgCell))
		invalidate();
}
void scrollBarUC::setThumbNormalImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setThumbNormalImage(rc);
}
bool scrollBarUC::getThumbHotImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_THUMB|UCIMAGE_STATE_HOT, &rc);
}

void scrollBarUC::setThumbHotImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_THUMB|UCIMAGE_STATE_HOT, rcImgCell))
		invalidate();
}
void scrollBarUC::setThumbHotImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setThumbHotImage(rc);
}
bool scrollBarUC::getThumbPushedImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_THUMB|UCIMAGE_STATE_PUSHED, &rc);
}

void scrollBarUC::setThumbPushedImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_THUMB|UCIMAGE_STATE_PUSHED, rcImgCell))
		invalidate();
}
void scrollBarUC::setThumbPushedImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setThumbPushedImage(rc);
}
bool scrollBarUC::getThumbDisabledImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_THUMB|UCIMAGE_STATE_DISABLED, &rc);
}

void scrollBarUC::setThumbDisabledImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_THUMB|UCIMAGE_STATE_DISABLED, rcImgCell))
		invalidate();
}
void scrollBarUC::setThumbDisabledImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setThumbDisabledImage(rc);
}
bool scrollBarUC::getRailNormalImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_RAIL|UCIMAGE_STATE_NORMAL, &rc);
}

void scrollBarUC::setRailNormalImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_RAIL|UCIMAGE_STATE_NORMAL, rcImgCell))
		invalidate();
}
void scrollBarUC::setRailNormalImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setRailNormalImage(rc);
}
bool scrollBarUC::getRailHotImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_RAIL|UCIMAGE_STATE_HOT, &rc);
}

void scrollBarUC::setRailHotImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_RAIL|UCIMAGE_STATE_HOT, rcImgCell))
		invalidate();
}
void scrollBarUC::setRailHotImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setRailHotImage(rc);
}
bool scrollBarUC::getRailPushedImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_RAIL|UCIMAGE_STATE_PUSHED, &rc);
}

void scrollBarUC::setRailPushedImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_RAIL|UCIMAGE_STATE_PUSHED, rcImgCell))
		invalidate();
}
void scrollBarUC::setRailPushedImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setRailPushedImage(rc);
}
bool scrollBarUC::getRailDisabledImage(RECT &rc)
{
	return existsImage(UCIMAGE_SCROLL_RAIL|UCIMAGE_STATE_DISABLED, &rc);
}

void scrollBarUC::setRailDisabledImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_SCROLL_RAIL|UCIMAGE_STATE_DISABLED, rcImgCell))
		invalidate();
}
void scrollBarUC::setRailDisabledImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setRailDisabledImage(rc);
}
bool scrollBarUC::getBkNormalImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_NORMAL, &rc);
}

void scrollBarUC::setBkNormalImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_STATE_NORMAL, rcImgCell))
		invalidate();
}
void scrollBarUC::setBkNormalImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setBkNormalImage(rc);
}
bool scrollBarUC::getBkHotImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_HOT, &rc);
}

void scrollBarUC::setBkHotImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_STATE_HOT, rcImgCell))
		invalidate();
}
void scrollBarUC::setBkHotImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setBkHotImage(rc);
}
bool scrollBarUC::getBkPushedImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_PUSHED, &rc);
}

void scrollBarUC::setBkPushedImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_STATE_PUSHED, rcImgCell))
		invalidate();
}
void scrollBarUC::setBkPushedImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setBkPushedImage(rc);
}
bool scrollBarUC::getBkDisabledImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_DISABLED, &rc);
}

void scrollBarUC::setBkDisabledImage(RECT rcImgCell)
{
	if(setCellImage(UCIMAGE_STATE_DISABLED, rcImgCell))
		invalidate();
}
void scrollBarUC::setBkDisabledImage(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rc = {left, top, right, bottom};
	setBkDisabledImage(rc);
}
void scrollBarUC::setPosition(RECT rc)
{
	controlUC::setPosition(rc);
	rc = m_rcItem;

	if( m_bHorizontal ) {
		int cx = rc.right - rc.left;
		if( m_bShowButton1 ) cx -= m_szxyFixed.cy;
		if( m_bShowButton2 ) cx -= m_szxyFixed.cy;
		if( cx > m_szxyFixed.cy ) {
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + m_szxyFixed.cy;
				m_rcButton1.bottom = rc.top + m_szxyFixed.cy;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.top = rc.top;
			m_rcButton2.right = rc.right;
			if( m_bShowButton2 ) {
				m_rcButton2.left = rc.right - m_szxyFixed.cy;
				m_rcButton2.bottom = rc.top + m_szxyFixed.cy;
			}
			else {
				m_rcButton2.left = m_rcButton2.right;
				m_rcButton2.bottom = m_rcButton2.top;
			}

			m_rcThumb.top = rc.top;
			m_rcThumb.bottom = rc.top + m_szxyFixed.cy;
			if( m_iRange > 0 ) {
				int cxThumb = cx * (rc.right - rc.left) / (m_iRange + rc.right - rc.left);
				if( cxThumb < m_szxyFixed.cy ) cxThumb = m_szxyFixed.cy;

				m_rcThumb.left = m_iScrollPos * (cx - cxThumb) / m_iRange + m_rcButton1.right;
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
		else {	// 当滚动条不足以容纳滑块时，只显示2按钮
			int cxButton = (rc.right - rc.left) / 2;
			if( cxButton > m_szxyFixed.cy ) cxButton = m_szxyFixed.cy;
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + cxButton;
				m_rcButton1.bottom = rc.top + m_szxyFixed.cy;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.top = rc.top;
			m_rcButton2.right = rc.right;
			if( m_bShowButton2 ) {
				m_rcButton2.left = rc.right - cxButton;
				m_rcButton2.bottom = rc.top + m_szxyFixed.cy;
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
		if( m_bShowButton1 ) cy -= m_szxyFixed.cx;
		if( m_bShowButton2 ) cy -= m_szxyFixed.cx;
		if( cy > m_szxyFixed.cx ) {
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + m_szxyFixed.cx;
				m_rcButton1.bottom = rc.top + m_szxyFixed.cx;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.left = rc.left;
			m_rcButton2.bottom = rc.bottom;
			if( m_bShowButton2 ) {
				m_rcButton2.top = rc.bottom - m_szxyFixed.cx;
				m_rcButton2.right = rc.left + m_szxyFixed.cx;
			}
			else {
				m_rcButton2.top = m_rcButton2.bottom;
				m_rcButton2.right = m_rcButton2.left;
			}

			m_rcThumb.left = rc.left;
			m_rcThumb.right = rc.left + m_szxyFixed.cx;
			if( m_iRange > 0 ) {
				int cyThumb = cy * (rc.bottom - rc.top) / (m_iRange + rc.bottom - rc.top);
				if( cyThumb < m_szxyFixed.cx ) cyThumb = m_szxyFixed.cx;

				m_rcThumb.top = m_iScrollPos * (cy - cyThumb) / m_iRange + m_rcButton1.bottom;
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
			if( cyButton > m_szxyFixed.cx ) cyButton = m_szxyFixed.cx;
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + m_szxyFixed.cx;
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
				m_rcButton2.right = rc.left + m_szxyFixed.cx;
			}
			else {
				m_rcButton2.top = m_rcButton2.bottom;
				m_rcButton2.right = m_rcButton2.left;
			}

			::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		}
	}
}

void scrollBarUC::Event(BSEventUC& event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pOwner != NULL ) m_pOwner->Event(event);
		else controlUC::Event(event);
		return;
	}

	if( event.Type == BEUCEventSetFocus ) 
	{
		return;
	}
	if( event.Type == BEUCEventKillFocus ) 
	{
		return;
	}
	if( event.Type == BEUCEventButtonDown || event.Type == BEUCEventDBLClick )
	{
		if( !isEnabled() ) return;

		m_iLastScrollOffset = 0;
		m_iScrollRepeatDelay = 0;
		m_pManager->setTimer(this, DEFAULT_TIMERID, 50U);

		if( ::PtInRect(&m_rcButton1, event.ptMouse) ) {
			m_uiButton1State |= UCSTATE_PUSHED;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->lineUp(); 
				else setScrollPos(m_iScrollPos - m_iLineSize);
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->lineLeft(); 
				else setScrollPos(m_iScrollPos - m_iLineSize);
			}
		}
		else if( ::PtInRect(&m_rcButton2, event.ptMouse) ) {
			m_uiButton2State |= UCSTATE_PUSHED;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->lineDown(); 
				else setScrollPos(m_iScrollPos + m_iLineSize);
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->lineRight(); 
				else setScrollPos(m_iScrollPos + m_iLineSize);
			}
		}
		else if( ::PtInRect(&m_rcThumb, event.ptMouse) ) {
			m_uiThumbState |= UCSTATE_CAPTURED | UCSTATE_PUSHED;
			ptLastMouse = event.ptMouse;
			m_iLastScrollPos = m_iScrollPos;
		}
		else {
			if( !m_bHorizontal ) {
				if( event.ptMouse.y < m_rcThumb.top ) {
					if( m_pOwner != NULL ) m_pOwner->pageUp(); 
					else setScrollPos(m_iScrollPos + m_rcItem.top - m_rcItem.bottom);
				}
				else if ( event.ptMouse.y > m_rcThumb.bottom ){
					if( m_pOwner != NULL ) m_pOwner->pageDown(); 
					else setScrollPos(m_iScrollPos - m_rcItem.top + m_rcItem.bottom);                    
				}
			}
			else {
				if( event.ptMouse.x < m_rcThumb.left ) {
					if( m_pOwner != NULL ) m_pOwner->pageLeft(); 
					else setScrollPos(m_iScrollPos + m_rcItem.left - m_rcItem.right);
				}
				else if ( event.ptMouse.x > m_rcThumb.right ){
					if( m_pOwner != NULL ) m_pOwner->pageRight(); 
					else setScrollPos(m_iScrollPos - m_rcItem.left + m_rcItem.right);                    
				}
			}
		}
		//if(m_pManager) m_pManager->sendNotify(this, L"click");
		if( m_pManager != NULL && m_pOwner == NULL ) m_pManager->sendNotify(this, L"scroll");
		return;
	}
	if( event.Type == BEUCEventButtonUp )
	{
		m_iScrollRepeatDelay = 0;
		m_iLastScrollOffset = 0;
		m_pManager->killTimer(this, DEFAULT_TIMERID);

		if( (m_uiThumbState & UCSTATE_CAPTURED) != 0 ) {
			m_uiThumbState &= ~( UCSTATE_CAPTURED | UCSTATE_PUSHED );
			invalidate();
		}
		else if( (m_uiButton1State & UCSTATE_PUSHED) != 0 ) {
			m_uiButton1State &= ~UCSTATE_PUSHED;
			invalidate();
		}
		else if( (m_uiButton2State & UCSTATE_PUSHED) != 0 ) {
			m_uiButton2State &= ~UCSTATE_PUSHED;
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventMouseMove )
	{
		if( (m_uiThumbState & UCSTATE_CAPTURED) != 0 ) {
			if( !m_bHorizontal ) {
				m_iLastScrollOffset = (event.ptMouse.y - ptLastMouse.y) * m_iRange / \
					(m_rcItem.bottom - m_rcItem.top - m_rcThumb.bottom + m_rcThumb.top - 2 * m_szxyFixed.cx);
			}
			else {
				m_iLastScrollOffset = (event.ptMouse.x - ptLastMouse.x) * m_iRange / \
					(m_rcItem.right - m_rcItem.left - m_rcThumb.right + m_rcThumb.left - 2 * m_szxyFixed.cy);
			}
		}
		else {
			if( (m_uiThumbState & UCSTATE_HOT) != 0 ) {
				if( !::PtInRect(&m_rcThumb, event.ptMouse) ) {
					m_uiThumbState &= ~UCSTATE_HOT;
					invalidate();
				}
			}
			else {
				if( !isEnabled() ) return;
				if( ::PtInRect(&m_rcThumb, event.ptMouse) ) {
					m_uiThumbState |= UCSTATE_HOT;
					invalidate();
				}
			}
		}
		return;
	}
	if( event.Type == BEUCEventContextMenu )
	{
		return;
	}
	if( event.Type == BEUCEventTimer && event.wParam == DEFAULT_TIMERID )
	{
		//SEND_LOG(L"scrollBarUC::Event BEUCEventTimer");
		++m_iScrollRepeatDelay;
		if( (m_uiThumbState & UCSTATE_CAPTURED) != 0 ) {
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) {
					SIZE sz = {m_pOwner->getScrollPos().cx, m_iLastScrollPos + m_iLastScrollOffset};
					m_pOwner->setScrollPos(sz); 
				}
				else setScrollPos(m_iLastScrollPos + m_iLastScrollOffset);
			}
			else {
				if( m_pOwner != NULL ) {
					SIZE sz = {m_iLastScrollPos + m_iLastScrollOffset,m_pOwner->getScrollPos().cy};
					m_pOwner->setScrollPos(sz); 
				}
				else setScrollPos(m_iLastScrollPos + m_iLastScrollOffset);
			}
			invalidate();
		}
		else if( (m_uiButton1State & UCSTATE_PUSHED) != 0 ) {
			if( m_iScrollRepeatDelay <= 5 ) return;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->lineUp(); 
				else setScrollPos(m_iScrollPos - m_iLineSize);
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->lineLeft(); 
				else setScrollPos(m_iScrollPos - m_iLineSize);
			}
		}
		else if( (m_uiButton2State & UCSTATE_PUSHED) != 0 ) {
			if( m_iScrollRepeatDelay <= 5 ) return;
			if( !m_bHorizontal ) {
				if( m_pOwner != NULL ) m_pOwner->lineDown(); 
				else setScrollPos(m_iScrollPos + m_iLineSize);
			}
			else {
				if( m_pOwner != NULL ) m_pOwner->lineRight(); 
				else setScrollPos(m_iScrollPos + m_iLineSize);
			}
		}
		else {
			if( m_iScrollRepeatDelay <= 5 ) return;
			POINT pt = { 0 };
			::GetCursorPos(&pt);
			::ScreenToClient(m_pManager->getPaintWindow(), &pt);
			if( !m_bHorizontal ) {
				if( pt.y < m_rcThumb.top ) {
					if( m_pOwner != NULL ) m_pOwner->pageUp(); 
					else setScrollPos(m_iScrollPos + m_rcItem.top - m_rcItem.bottom);
				}
				else if ( pt.y > m_rcThumb.bottom ){
					if( m_pOwner != NULL ) m_pOwner->pageDown(); 
					else setScrollPos(m_iScrollPos - m_rcItem.top + m_rcItem.bottom);                    
				}
			}
			else {
				if( pt.x < m_rcThumb.left ) {
					if( m_pOwner != NULL ) m_pOwner->pageLeft(); 
					else setScrollPos(m_iScrollPos + m_rcItem.left - m_rcItem.right);
				}
				else if ( pt.x > m_rcThumb.right ){
					if( m_pOwner != NULL ) m_pOwner->pageRight(); 
					else setScrollPos(m_iScrollPos - m_rcItem.left + m_rcItem.right);                    
				}
			}
		}
		if( m_pManager != NULL && m_pOwner == NULL ) m_pManager->sendNotify(this, L"scroll");
		return;
	}
	if( event.Type == BEUCEventMouseEnter )
	{
		if( isEnabled() ) {
			m_uiButton1State |= UCSTATE_HOT;
			m_uiButton2State |= UCSTATE_HOT;
			if( ::PtInRect(&m_rcThumb, event.ptMouse) ) m_uiThumbState |= UCSTATE_HOT;
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventMouseLeave )
	{
		if( isEnabled() ) {
			m_uiButton1State &= ~UCSTATE_HOT;
			m_uiButton2State &= ~UCSTATE_HOT;
			m_uiThumbState &= ~UCSTATE_HOT;
			invalidate();
		}
		return;
	}

	if( m_pOwner != NULL ) m_pOwner->Event(event); else controlUC::Event(event);
}

void scrollBarUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = NULL;
	if( _tcscmp(pstrName, L"button1normalimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setButton1NormalImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"button1hotimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setButton1HotImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"button1pushedimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setButton1PushedImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"button1disabledimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setButton1DisabledImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"button2normalimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setButton2NormalImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"button2hotimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setButton2HotImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"button2pushedimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setButton2PushedImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"button2disabledimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setButton2DisabledImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"thumbnormalimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setThumbNormalImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"thumbhotimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setThumbHotImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"thumbpushedimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setThumbPushedImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"thumbdisabledimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setThumbDisabledImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"railnormalimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setRailNormalImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"railhotimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setRailHotImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"railpushedimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setRailPushedImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"raildisabledimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setRailDisabledImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"bknormalimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setBkNormalImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"bkhotimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setBkHotImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"bkpushedimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setBkPushedImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"bkdisabledimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setBkDisabledImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"hor") == 0 ) setHorizontal(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"linesize") == 0 ) setLineSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"range") == 0 ) setScrollRange(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"value") == 0 ) setScrollPos(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"showbutton1") == 0 ) setShowButton1(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"showbutton2") == 0 ) setShowButton2(_tcscmp(pstrValue, L"true") == 0);
	else controlUC::setAttribute(pstrName, pstrValue);
}

void scrollBarUC::paint(HDC hDC, const RECT& rcPaint)
{
	if(!isVisible()) return;
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	paintBk(hDC);
	paintButton1(hDC);
	paintButton2(hDC);
	paintThumb(hDC);
	paintRail(hDC);
}

void scrollBarUC::paintBk(HDC hDC)
{
	if( !isEnabled() ) m_uiThumbState |= UCSTATE_DISABLED;
	else m_uiThumbState &= ~ UCSTATE_DISABLED;
	RECT rcScr = {0};
	Gdiplus::Graphics grap(hDC);

	//gtc::drawer::fillRectangle(grap, 0x1FFF0000, m_rcItem);
	//return;
	if( (m_uiThumbState & UCSTATE_DISABLED) != 0 ) {
		if (getBkDisabledImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcItem, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcItem, rcScr);
			//return;
		}
	}
	else if( (m_uiThumbState & UCSTATE_PUSHED) != 0 ) {
		if (getBkPushedImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcItem, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcItem, rcScr);
			//return;
		}
	}
	else if( (m_uiThumbState & UCSTATE_HOT) != 0 ) {
		if (getBkHotImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcItem, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcItem, rcScr);
			//return;
		}
	}

	if (getBkNormalImage(rcScr))
	{
		if(gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcScr)) return;
		//if(m_bHorizontal)
		//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcItem, rcScr);
		//else
		//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcItem, rcScr);
		//return;
	}
}

void scrollBarUC::paintButton1(HDC hDC)
{
	if( !m_bShowButton1 ) return;

	if( !isEnabled() ) m_uiButton1State |= UCSTATE_DISABLED;
	else m_uiButton1State &= ~ UCSTATE_DISABLED;

	RECT rcScr = {0};
	Gdiplus::Graphics grap(hDC);
	if( (m_uiButton1State & UCSTATE_DISABLED) != 0 ) {
		if (getButton1DisabledImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcButton1, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton1, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton1, rcScr);
			//return;
		}
	}
	else if( (m_uiButton1State & UCSTATE_PUSHED) != 0 ) {
		if (getButton1PushedImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcButton1, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton1, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton1, rcScr);
			//return;
		}
	}
	else if( (m_uiButton1State & UCSTATE_HOT) != 0 ) {
		if (getButton1HotImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcButton1, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton1, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton1, rcScr);
			//return;
		}
	}
	if (getButton1NormalImage(rcScr))
	{
		if(gtc::drawer::drawImage(grap, m_pImage, m_rcButton1, rcScr)) return;
		//if(m_bHorizontal)
		//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton1, rcScr);
		//else
		//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton1, rcScr);
		//return;
	}


	DWORD dwBorderColor = 0xFE85E4FF;
	int nBorderSize = 2;
	gtc::drawer::drawBorder(grap, gtc::BSPen(gtc::BEPenStyleSolid, nBorderSize, dwBorderColor), m_rcButton1);
}

void scrollBarUC::paintButton2(HDC hDC)
{
	if( !m_bShowButton2 ) return;

	if( !isEnabled() ) m_uiButton2State |= UCSTATE_DISABLED;
	else m_uiButton2State &= ~ UCSTATE_DISABLED;

	RECT rcScr = {0};
	Gdiplus::Graphics grap(hDC);

	if( (m_uiButton2State & UCSTATE_DISABLED) != 0 ) {
		if (getButton2DisabledImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcButton2, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton2, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton2, rcScr);
			//return;
		}
	}
	else if( (m_uiButton2State & UCSTATE_PUSHED) != 0 ) {
		if (getButton2PushedImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcButton2, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton2, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton2, rcScr);
			//return;
		}
	}
	else if( (m_uiButton2State & UCSTATE_HOT) != 0 ) {
		if (getButton2HotImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcButton2, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton2, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton2, rcScr);
			//return;
		}
	}

	if (getButton2NormalImage(rcScr))
	{
		if(gtc::drawer::drawImage(grap, m_pImage, m_rcButton2, rcScr)) return;
		//if(m_bHorizontal)
		//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton2, rcScr);
		//else
		//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcButton2, rcScr);
		//return;
	}

	DWORD dwBorderColor = 0xFE85E4FF;
	int nBorderSize = 2;
	gtc::drawer::drawBorder(grap, gtc::BSPen(gtc::BEPenStyleSolid, nBorderSize, dwBorderColor), m_rcButton2);
	//CRenderEngine::DrawRect(hDC, m_rcButton2, nBorderSize, dwBorderColor);
}

void scrollBarUC::paintThumb(HDC hDC)
{
	if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
	if( !isEnabled() ) m_uiThumbState |= UCSTATE_DISABLED;
	else m_uiThumbState &= ~ UCSTATE_DISABLED;

	RECT rcScr = {0};
	Gdiplus::Graphics grap(hDC);
	if( (m_uiThumbState & UCSTATE_DISABLED) != 0 ) {
		if (getThumbDisabledImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcThumb, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcThumb, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcThumb, rcScr);
			//return;
		}
	}
	else if( (m_uiThumbState & UCSTATE_PUSHED) != 0 ) {
		if (getThumbPushedImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcThumb, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcThumb, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcThumb, rcScr);
			//return;
		}
	}
	else if( (m_uiThumbState & UCSTATE_HOT) != 0 ) {
		if (getThumbHotImage(rcScr))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcThumb, rcScr)) return;
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcThumb, rcScr);
			//else
			//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcThumb, rcScr);
			//return;
		}
	}

	if (getThumbNormalImage(rcScr))
	{
		if(gtc::drawer::drawImage(grap, m_pImage, m_rcThumb, rcScr)) return;
		//if(m_bHorizontal)
		//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcThumb, rcScr);
		//else
		//	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcThumb, rcScr);
		//return;
	}

	DWORD dwBorderColor = 0xFE85E4FF;
	int nBorderSize = 2;
	gtc::drawer::drawBorder(grap, gtc::BSPen(gtc::BEPenStyleSolid, nBorderSize, dwBorderColor), m_rcThumb);
	//CRenderEngine::DrawRect(hDC, m_rcThumb, nBorderSize, dwBorderColor);
}

void scrollBarUC::paintRail(HDC hDC)
{
	if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
	if( !isEnabled() ) m_uiThumbState |= UCSTATE_DISABLED;
	else m_uiThumbState &= ~ UCSTATE_DISABLED;

	RECT rcScr = {0};
	Gdiplus::Graphics grap(hDC);
	if( (m_uiThumbState & UCSTATE_DISABLED) != 0 ) {
		if (getRailDisabledImage(rcScr))
		{
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage3PatchH(grap, *m_pImage, m_rcThumb, rcScr, m_szPatch.cx);
			//else
			//	gtc::drawer::drawImage3PatchV(grap, *m_pImage, m_rcThumb, rcScr, m_szPatch.cy);
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcThumb, rcScr)) return;
		}
	}
	else if( (m_uiThumbState & UCSTATE_PUSHED) != 0 ) {
		if (getRailPushedImage(rcScr))
		{
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage3PatchH(grap, *m_pImage, m_rcThumb, rcScr, m_szPatch.cx);
			//else
			//	gtc::drawer::drawImage3PatchV(grap, *m_pImage, m_rcThumb, rcScr, m_szPatch.cy);
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcThumb, rcScr)) return;
		}
	}
	else if( (m_uiThumbState & UCSTATE_HOT) != 0 ) {
		if (getRailHotImage(rcScr))
		{
			//if(m_bHorizontal)
			//	gtc::drawer::drawImage3PatchH(grap, *m_pImage, m_rcThumb, rcScr, m_szPatch.cx);
			//else
			//	gtc::drawer::drawImage3PatchV(grap, *m_pImage, m_rcThumb, rcScr, m_szPatch.cy);
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcThumb, rcScr)) return;
		}
	}
	if (getRailNormalImage(rcScr))
	{
		//if(m_bHorizontal)
		//	gtc::drawer::drawImage3PatchH(grap, *m_pImage, m_rcThumb, rcScr, m_szPatch.cx);
		//else
		//	gtc::drawer::drawImage3PatchV(grap, *m_pImage, m_rcThumb, rcScr, m_szPatch.cy);
		if(gtc::drawer::drawImage(grap, m_pImage, m_rcThumb, rcScr)) return;
	}

}

bool scrollBarUC::isScrolling()
{
	return (m_uiThumbState & UCSTATE_PUSHED) != 0
		|| (m_uiButton1State & UCSTATE_PUSHED) != 0
		|| (m_uiButton2State & UCSTATE_PUSHED) != 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	trackerUC  类		2015-3-5
//	普通的跟踪框，如：左键选中一个区域
////////////////////////////////////////////////////////////////////////////////////////////////////////
trackerUC::trackerUC()
{
	setFocus(true);
	memset(&m_ptStart, 0, sizeof(m_ptStart));
	memset(&m_ptEnd, 0, sizeof(m_ptEnd));
}
trackerUC::~trackerUC()
{
	SEND_LOG(L"trackerUC free ... <%s>", gtc::format::rect(m_rcItem, true).c_str());
}

PCWSTR trackerUC::getClass() const
{
	return L"trackerUC";
}

LPVOID trackerUC::getInterface(PCWSTR pstrName)
{
	if(std::wcscmp(pstrName, L"Tracker") == 0) return static_cast<trackerUC *>(this);
	return __super::getInterface(pstrName);
}
void trackerUC::paintBkImage(HDC hdc)
{
	if(!hdc || !m_pImage) return;

	Gdiplus::Graphics grap(hdc);
	//gtc::drawer::fillRectangle(grap, 0xff00ff00, m_rcItem);
	RECT rcSrc = {0,0,m_pImage->getWidth(), m_pImage->getHeight()};
	gtc::drawer::drawImage9Patch(grap, m_pImage, m_rcItem, rcSrc);
	//gtc::drawer::drawImage9Patch(grap, *m_pImage, m_rcItem, rcSrc, m_szPatch);
}
//void trackerUC::setPosition(RECT pos)
//{
//	if(pos.left == pos.right && pos.left == 0)
//	{
//		if (pos.top == pos.bottom  && pos.top == 0)
//		{
//			int s;
//			s = 0;
//		}
//	}
//	__super::setPosition(pos);
//}
POINT trackerUC::getStartPoint() const
{
	return m_ptStart;
}
void trackerUC::setStartPoint(POINT pt)
{
	m_ptStart = pt;
	setEndPoint(pt);
}
POINT trackerUC::getEndPoint() const
{
	return m_ptEnd;
}
void trackerUC::setEndPoint(POINT pt)
{
	m_ptEnd = pt;
	RECT rc = {min(m_ptStart.x, m_ptEnd.x), min(m_ptStart.y, m_ptEnd.y),
		max(m_ptStart.x, m_ptEnd.x), max(m_ptStart.y, m_ptEnd.y)};
	//setFixedHeight(rectHeight(rc));
	//setFixedWidth(rectWidth(rc));
	setPosition(rc);
	setAttribute(L"pos", gtc::format::rect(rc).c_str());
	invalidate();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	progressUC	类 2016-3-3	kp007
//	进度条
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
progressUC::progressUC()
	: m_bHorizontal(true)
	, m_iMax(100), m_iMin(0), m_iValue(0)
{
	setTextStyle(DT_SINGLELINE|DT_VCENTER|DT_CENTER);
}
progressUC::~progressUC()
{

}

PCWSTR progressUC::getClass() const
{
	return L"progressUC";
}
LPVOID progressUC::getInterface(PCWSTR pstrName)
{
	if(std::wcscmp(pstrName, L"Progress") == 0) return static_cast<progressUC *>(this);
	return __super::getInterface(pstrName);
}

void progressUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = nullptr;
	if( _tcscmp(pstrName, L"max") == 0 ) setMaxValue(_tcstol(pstrValue, NULL, 10));
	else if( _tcscmp(pstrName, L"min") == 0 ) setMinValue(_tcstol(pstrValue, NULL, 10));
	else if( _tcscmp(pstrName, L"value") == 0 ) setValue(_tcstol(pstrValue, NULL, 10));
	else if( _tcscmp(pstrName, L"horizontal") == 0 ) setHorizontal(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, L"normalimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setNormalImage(rcCell);
		//setNormalImage(_tcstol(pstrValue, &pstr, 10));
	}
	else if( _tcscmp(pstrName, L"progressimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setProgressImage(rcCell);
		//setNormalImage(_tcstol(pstrValue, &pstr, 10));
	}
	else __super::setAttribute(pstrName, pstrValue);
}
void progressUC::paintStatusImage(HDC hdc)
{
	if(!m_pImage) return;

	if( m_iMax <= m_iMin ) m_iMax = m_iMin + 1;
	if( m_iValue > m_iMax ) m_iValue = m_iMax;
	if( m_iValue < m_iMin ) m_iValue = m_iMin;

	RECT rc = m_rcItem;
	if( m_bHorizontal ) {
		rc.right = rc.left + (m_iValue - m_iMin) * (m_rcItem.right - m_rcItem.left) / (m_iMax - m_iMin);
		//rc.bottom = m_rcItem.bottom - m_rcItem.top;
	}
	else {
		rc.top = rc.bottom - (m_rcItem.bottom - m_rcItem.top) * (m_iMax - m_iValue) / (m_iMax - m_iMin);
		//rc.right = m_rcItem.right - m_rcItem.left;
		//rc.bottom = m_rcItem.bottom - m_rcItem.top;
	}

	Gdiplus::Graphics grap(hdc);
	RECT rcSrc = {0};

	if (getNormalImage(rcSrc))
	{
		gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
	}
	if (getProgressImage(rcSrc))
	{
		gtc::drawer::drawImage(grap, m_pImage, rc, rcSrc);
	}


}

bool progressUC::getNormalImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_NORMAL, &rc);
}

void progressUC::setNormalImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_NORMAL, rcImgCell);
	invalidate();
}
void progressUC::setNormalImage(LONG left, LONG top, LONG right, LONG bottom)
{
	assert(m_pImage);
	RECT rc = {left, top, right, bottom};
	setNormalImage(rc);
}

bool progressUC::getProgressImage(RECT &rc)
{
	return existsImage(UCIMAGE_PROGRESS_FORE, &rc);
}

void progressUC::setProgressImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_PROGRESS_FORE, rcImgCell);
	invalidate();
}
void progressUC::setProgressImage(LONG left, LONG top, LONG right, LONG bottom)
{
	assert(m_pImage);
	RECT rc = {left, top, right, bottom};
	setNormalImage(rc);
}

void progressUC::setHorizontal(bool bHorizontal /*= true*/)
{
	if(m_bHorizontal == bHorizontal) return;
	m_bHorizontal = bHorizontal;
	invalidate();
}
bool progressUC::isHorizontal() const
{
	return m_bHorizontal;
}
void progressUC::setMaxValue(int _max)
{
	m_iMax = _max;
	invalidate();
}
int progressUC::getMaxValue() const
{
	return m_iMax;
}
void progressUC::setMinValue(int _min)
{
	m_iMin = _min;
	invalidate();
}
int progressUC::getMinValue() const
{
	return m_iMin;
}
void progressUC::setValue(int val)
{
	m_iValue = val;
	invalidate();
}
int progressUC::getValue() const
{
	return m_iValue;
}
}	// end wuc