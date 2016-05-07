#include "StdAfx.h"
#include "controlUI.h"


namespace wui {


controlUI::controlUI() 
	: m_pManager(NULL), 
	m_pParent(NULL), 
	m_bUpdateNeeded(true),
	m_bMenuUsed(false),
	m_bVisible(true), 
	m_bInternVisible(true),
	m_bFocused(false),
	m_bEnabled(true),
	m_bMouseEnabled(true),
	m_bFloat(false),
	m_bSetPos(false),
	m_chShortcut('\0'),
	m_pTag(NULL),
	m_dwBackColor(0),
	m_dwBackColor2(0),
	m_dwBackColor3(0),
	m_dwBorderColor(0),
	m_dwFocusBorderColor(0),
	m_bColorHSL(false),
	m_nBorderSize(1)
{
	//TRACE_CLASS_BEGIN(this)
	m_cXY.cx = m_cXY.cy = 0;
	m_cxyFixed.cx = m_cxyFixed.cy = 0;
	m_cxyMin.cx = m_cxyMin.cy = 0;
	m_cxyMax.cx = m_cxyMax.cy = 9999;
	m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

	::ZeroMemory(&m_rcPadding, sizeof(m_rcPadding));
	::ZeroMemory(&m_rcItem, sizeof(RECT));
	::ZeroMemory(&m_rcPaint, sizeof(RECT));
	::ZeroMemory(&m_tRelativePos, sizeof(TRelativePosUI));
}

controlUI::~controlUI()
{
	//TRACE_CLASS_END(this)
	if( onDestroy ) onDestroy(this);
	if( m_pManager != NULL ) m_pManager->reapObjects(this);
}

std::wstring controlUI::getName() const
{
	return m_sName;
}

void controlUI::setName(PCWSTR pstrName)
{
	m_sName = pstrName;
}

LPVOID controlUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Control") == 0 ) return this;
	return NULL;
}

PCWSTR controlUI::getClass() const
{
	return L"ControlUI";
}

UINT controlUI::getControlFlags() const
{
	return 0;
}

bool controlUI::activate()
{
	if( !isVisible() ) return false;
	if( !isEnabled() ) return false;
	return true;
}

paintManagerUI* controlUI::getManager() const
{
	return m_pManager;
}

void controlUI::setManager(paintManagerUI* pManager, controlUI* pParent, bool bInit)
{
	m_pManager = pManager;
	m_pParent = pParent;
	if( bInit && m_pParent ) init();
}

controlUI* controlUI::getParent() const
{
	return m_pParent;
}

std::wstring controlUI::getText() const
{
	return m_sText;
}

void controlUI::setText(PCWSTR pstrText)
{
	if( m_sText == pstrText ) return;

	m_sText = pstrText;
	invalidate();
}

DWORD controlUI::getBkColor() const
{
	return m_dwBackColor;
}

void controlUI::setBkColor(DWORD dwBackColor)
{
	if( m_dwBackColor == dwBackColor ) return;

	m_dwBackColor = dwBackColor;
	invalidate();
}

DWORD controlUI::getBkColor2() const
{
	return m_dwBackColor2;
}

void controlUI::setBkColor2(DWORD dwBackColor)
{
	if( m_dwBackColor2 == dwBackColor ) return;

	m_dwBackColor2 = dwBackColor;
	invalidate();
}

DWORD controlUI::getBkColor3() const
{
	return m_dwBackColor3;
}

void controlUI::setBkColor3(DWORD dwBackColor)
{
	if( m_dwBackColor3 == dwBackColor ) return;

	m_dwBackColor3 = dwBackColor;
	invalidate();
}

PCWSTR controlUI::getBkImage()
{
	return m_sBkImage.c_str();
}

void controlUI::setBkImage(PCWSTR pStrImage)
{
	if( m_sBkImage == pStrImage ) return;

	m_sBkImage = pStrImage;
	invalidate();
}

DWORD controlUI::getBorderColor() const
{
	return m_dwBorderColor;
}

void controlUI::setBorderColor(DWORD dwBorderColor)
{
	if( m_dwBorderColor == dwBorderColor ) return;

	m_dwBorderColor = dwBorderColor;
	invalidate();
}

DWORD controlUI::getFocusBorderColor() const
{
	return m_dwFocusBorderColor;
}

void controlUI::setFocusBorderColor(DWORD dwBorderColor)
{
	if( m_dwFocusBorderColor == dwBorderColor ) return;

	m_dwFocusBorderColor = dwBorderColor;
	invalidate();
}

bool controlUI::isColorHSL() const
{
	return m_bColorHSL;
}

void controlUI::setColorHSL(bool bColorHSL)
{
	if( m_bColorHSL == bColorHSL ) return;

	m_bColorHSL = bColorHSL;
	invalidate();
}

int controlUI::getBorderSize() const
{
	return m_nBorderSize;
}

void controlUI::setBorderSize(int nSize)
{
	if( m_nBorderSize == nSize ) return;

	m_nBorderSize = nSize;
	invalidate();
}

SIZE controlUI::getBorderRound() const
{
	return m_cxyBorderRound;
}

void controlUI::setBorderRound(SIZE cxyRound)
{
	m_cxyBorderRound = cxyRound;
	invalidate();
}

bool controlUI::drawImage(HDC hDC, PCWSTR pStrImage, PCWSTR pStrModify)
{
	return renderEngine::drawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, pStrImage, pStrModify);
}

const RECT& controlUI::getPos() const
{
	return m_rcItem;
}

void controlUI::setPos(RECT rc)
{
	if( rc.right < rc.left ) rc.right = rc.left;
	if( rc.bottom < rc.top ) rc.bottom = rc.top;

	rect invalidateRc = m_rcItem;
	if( ::IsRectEmpty(&invalidateRc) ) invalidateRc = rc;

	m_rcItem = rc;
	if( m_pManager == NULL ) return;

	if( m_bFloat ) {
		controlUI* pParent = getParent();
		if( pParent != NULL ) {
			RECT rcParentPos = pParent->getPos();
			if( m_cXY.cx >= 0 ) m_cXY.cx = m_rcItem.left - rcParentPos.left;
			else m_cXY.cx = m_rcItem.right - rcParentPos.right;

			if( m_cXY.cy >= 0 ) m_cXY.cy = m_rcItem.top - rcParentPos.top;
			else m_cXY.cy = m_rcItem.bottom - rcParentPos.bottom;
		}
	}

	if( !m_bSetPos ) {
		m_bSetPos = true;
		if( onSize ) onSize(this);
		m_bSetPos = false;
	}

	m_bUpdateNeeded = false;

	// NOTE: SetPos() is usually called during the WM_PAINT cycle where all controls are
	//       being laid out. Calling UpdateLayout() again would be wrong. Refreshing the
	//       window won't hurt (if we're already inside WM_PAINT we'll just validate it out).
	invalidateRc.join(m_rcItem);

	controlUI* pParent = this;
	RECT rcTemp;
	RECT rcParent;
	while( pParent = pParent->getParent() )
	{
		rcTemp = invalidateRc;
		rcParent = pParent->getPos();
		if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
		{
			return;
		}
	}
	m_pManager->invalidate(invalidateRc);
}

int controlUI::getWidth() const
{
	return m_rcItem.right - m_rcItem.left;
}

int controlUI::getHeight() const
{
	return m_rcItem.bottom - m_rcItem.top;
}

int controlUI::getX() const
{
	return m_rcItem.left;
}

int controlUI::getY() const
{
	return m_rcItem.top;
}

RECT controlUI::getPadding() const
{
	return m_rcPadding;
}

void controlUI::setPadding(RECT rcPadding)
{
	m_rcPadding = rcPadding;
	needParentUpdate();
}

SIZE controlUI::getFixedXY() const
{
	return m_cXY;
}

void controlUI::setFixedXY(SIZE szXY)
{
	m_cXY.cx = szXY.cx;
	m_cXY.cy = szXY.cy;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUI::getFixedWidth() const
{
	return m_cxyFixed.cx;
}

void controlUI::setFixedWidth(int cx)
{
	if( cx < 0 ) return; 
	m_cxyFixed.cx = cx;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUI::getFixedHeight() const
{
	return m_cxyFixed.cy;
}

void controlUI::setFixedHeight(int cy)
{
	if( cy < 0 ) return; 
	m_cxyFixed.cy = cy;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUI::getMinWidth() const
{
	return m_cxyMin.cx;
}

void controlUI::setMinWidth(int cx)
{
	if( m_cxyMin.cx == cx ) return;

	if( cx < 0 ) return; 
	m_cxyMin.cx = cx;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUI::getMaxWidth() const
{
	return m_cxyMax.cx;
}

void controlUI::setMaxWidth(int cx)
{
	if( m_cxyMax.cx == cx ) return;

	if( cx < 0 ) return; 
	m_cxyMax.cx = cx;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUI::getMinHeight() const
{
	return m_cxyMin.cy;
}

void controlUI::setMinHeight(int cy)
{
	if( m_cxyMin.cy == cy ) return;

	if( cy < 0 ) return; 
	m_cxyMin.cy = cy;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUI::getMaxHeight() const
{
	return m_cxyMax.cy;
}

void controlUI::setMaxHeight(int cy)
{
	if( m_cxyMax.cy == cy ) return;

	if( cy < 0 ) return; 
	m_cxyMax.cy = cy;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

void controlUI::setRelativePos(SIZE szMove,SIZE szZoom)
{
	m_tRelativePos.bRelative = TRUE;
	m_tRelativePos.nMoveXPercent = szMove.cx;
	m_tRelativePos.nMoveYPercent = szMove.cy;
	m_tRelativePos.nZoomXPercent = szZoom.cx;
	m_tRelativePos.nZoomYPercent = szZoom.cy;
}

void controlUI::setRelativeParentSize(SIZE sz)
{
	m_tRelativePos.szParent = sz;
}

TRelativePosUI controlUI::getRelativePos() const
{
	return m_tRelativePos;
}

bool controlUI::isRelativePos() const
{
	return m_tRelativePos.bRelative;
}

std::wstring controlUI::getToolTip() const
{
	return m_sToolTip;
}

void controlUI::setToolTip(PCWSTR pstrText)
{
	m_sToolTip = pstrText;
}


wchar_t controlUI::getShortcut() const
{
	return m_chShortcut;
}

void controlUI::setShortcut(TCHAR ch)
{
	m_chShortcut = ch;
}

bool controlUI::isContextMenuUsed() const
{
	return m_bMenuUsed;
}

void controlUI::setContextMenuUsed(bool bMenuUsed)
{
	m_bMenuUsed = bMenuUsed;
}

const std::wstring& controlUI::getUserData()
{
	return m_sUserData;
}

void controlUI::setUserData(PCWSTR pstrText)
{
	m_sUserData = pstrText;
}

UINT_PTR controlUI::getTag() const
{
	return m_pTag;
}

void controlUI::setTag(UINT_PTR pTag)
{
	m_pTag = pTag;
}

bool controlUI::isVisible() const
{
	return m_bVisible && m_bInternVisible;
}

void controlUI::setVisible(bool bVisible)
{
	if( m_bVisible == bVisible ) return;

	bool v = isVisible();
	m_bVisible = bVisible;
	if( m_bFocused ) m_bFocused = false;
	if( isVisible() != v ) {
		needParentUpdate();
	}
}

void controlUI::setInternVisible(bool bVisible)
{
	m_bInternVisible = bVisible;
}

bool controlUI::isEnabled() const
{
	return m_bEnabled;
}

void controlUI::setEnabled(bool bEnabled)
{
	if( m_bEnabled == bEnabled ) return;

	m_bEnabled = bEnabled;
	invalidate();
}

bool controlUI::isMouseEnabled() const
{
	return m_bMouseEnabled;
}

void controlUI::setMouseEnabled(bool bEnabled)
{
	m_bMouseEnabled = bEnabled;
}

bool controlUI::isFocused() const
{
	return m_bFocused;
}

void controlUI::setFocus()
{
	if( m_pManager != NULL ) m_pManager->setFocus(this);
}

bool controlUI::isFloat() const
{
	return m_bFloat;
}

void controlUI::setFloat(bool bFloat)
{
	if( m_bFloat == bFloat ) return;

	m_bFloat = bFloat;
	needParentUpdate();
}

controlUI* controlUI::findControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if( (uFlags & UIFIND_VISIBLE) != 0 && !isVisible() ) return NULL;
	if( (uFlags & UIFIND_ENABLED) != 0 && !isEnabled() ) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!m_bMouseEnabled || !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) ) return NULL;
	return Proc(this, pData);
}

void controlUI::invalidate()
{
	if( !isVisible() ) return;

	RECT invalidateRc = m_rcItem;

	controlUI* pParent = this;
	RECT rcTemp;
	RECT rcParent;
	while( pParent = pParent->getParent() )
	{
		rcTemp = invalidateRc;
		rcParent = pParent->getPos();
		if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
		{
			return;
		}
	}

	if( m_pManager != NULL ) m_pManager->invalidate(invalidateRc);
}

bool controlUI::isUpdateNeeded() const
{
	return m_bUpdateNeeded;
}

void controlUI::needUpdate()
{
	if( !isVisible() ) return;
	m_bUpdateNeeded = true;
	invalidate();

	if( m_pManager != NULL ) m_pManager->needUpdate();
}

void controlUI::needParentUpdate()
{
	if( getParent() ) {
		getParent()->needUpdate();
		getParent()->invalidate();
	}
	else {
		needUpdate();
	}

	if( m_pManager != NULL ) m_pManager->needUpdate();
}

DWORD controlUI::getAdjustColor(DWORD dwColor)
{
	if( !m_bColorHSL ) return dwColor;
	short H, S, L;
	paintManagerUI::getHSL(&H, &S, &L);
	return renderEngine::adjustColor(dwColor, H, S, L);
}

void controlUI::init()
{
	doInit();
	if( onInit ) onInit(this);
}

void controlUI::doInit()
{

}

void controlUI::Event(TEventUI& event)
{
	if( onEvent(&event) ) doEvent(event);
}

void controlUI::doEvent(TEventUI& event)
{
	if( event.Type == UIEVENT_SETCURSOR )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return;
	}
	if( event.Type == UIEVENT_SETFOCUS ) 
	{
		m_bFocused = true;
		invalidate();
		return;
	}
	if( event.Type == UIEVENT_KILLFOCUS ) 
	{
		m_bFocused = false;
		invalidate();
		return;
	}
	if( event.Type == UIEVENT_TIMER )
	{
		m_pManager->sendNotify(this, L"timer", event.wParam, event.lParam);
		return;
	}
	if( event.Type == UIEVENT_CONTEXTMENU )
	{
		if( isContextMenuUsed() ) {
			m_pManager->sendNotify(this, L"menu", event.wParam, event.lParam);
			return;
		}
	}
	if( m_pParent != NULL ) m_pParent->doEvent(event);
}

void controlUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"pos") == 0 ) {
		RECT rcPos = { 0 };
		PWSTR pstr = NULL;
		rcPos.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcPos.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcPos.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};
		setFixedXY(szXY);
		setFixedWidth(rcPos.right - rcPos.left);
		setFixedHeight(rcPos.bottom - rcPos.top);
	}
	else if( _tcscmp(pstrName, L"relativepos") == 0 ) {
		SIZE szMove,szZoom;
		PWSTR pstr = NULL;
		szMove.cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		szMove.cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		szZoom.cx = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		szZoom.cy = _tcstol(pstr + 1, &pstr, 10); assert(pstr); 
		setRelativePos(szMove,szZoom);
	}
	else if( _tcscmp(pstrName, L"padding") == 0 ) {
		RECT rcPadding = { 0 };
		PWSTR pstr = NULL;
		rcPadding.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setPadding(rcPadding);
	}
	else if( _tcscmp(pstrName, L"bkcolor") == 0 || _tcscmp(pstrName, L"bkcolor1") == 0 ) {
		while( *pstrValue > L'\0' && *pstrValue <= L' ')  pstrValue = ::CharNext(pstrValue);
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"bkcolor2") == 0 ) {
		while( *pstrValue > L'\0' && *pstrValue <= L' ' ) pstrValue = ::CharNext(pstrValue);
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setBkColor2(clrColor);
	}
	else if( _tcscmp(pstrName, L"bkcolor3") == 0 ) {
		while( *pstrValue > L'\0' && *pstrValue <= L' ' ) pstrValue = ::CharNext(pstrValue);
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setBkColor3(clrColor);
	}
	else if( _tcscmp(pstrName, L"bordercolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setBorderColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"focusbordercolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		PWSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setFocusBorderColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"colorhsl") == 0 ) setColorHSL(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"bordersize") == 0 ) setBorderSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"borderround") == 0 ) {
		SIZE cxyRound = { 0 };
		PWSTR pstr = NULL;
		cxyRound.cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);     
		setBorderRound(cxyRound);
	}
	else if( _tcscmp(pstrName, L"bkimage") == 0 ) setBkImage(pstrValue);
	else if( _tcscmp(pstrName, L"width") == 0 ) setFixedWidth(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"height") == 0 ) setFixedHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"minwidth") == 0 ) setMinWidth(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"minheight") == 0 ) setMinHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"maxwidth") == 0 ) setMaxWidth(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"maxheight") == 0 ) setMaxHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"name") == 0 ) setName(pstrValue);
	else if( _tcscmp(pstrName, L"text") == 0 ) setText(pstrValue);
	else if( _tcscmp(pstrName, L"tooltip") == 0 ) setToolTip(pstrValue);
	else if( _tcscmp(pstrName, L"userdata") == 0 ) setUserData(pstrValue);
	else if( _tcscmp(pstrName, L"enabled") == 0 ) setEnabled(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"mouse") == 0 ) setMouseEnabled(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"visible") == 0 ) setVisible(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"float") == 0 ) setFloat(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"shortcut") == 0 ) setShortcut(pstrValue[0]);
	else if( _tcscmp(pstrName, L"menu") == 0 ) setContextMenuUsed(_tcscmp(pstrValue, L"true") == 0);
}

controlUI* controlUI::applyAttributeList(PCWSTR pstrList)
{
	std::wstring sItem;
	std::wstring sValue;
	while( *pstrList != L'\0' ) {
		sItem.clear();
		sValue.clear();
		while( *pstrList != L'\0' && *pstrList != L'=' ) {
			PWSTR pstrTemp = ::CharNext(pstrList);
			while( pstrList < pstrTemp) {
				sItem += *pstrList++;
			}
		}
		assert( *pstrList == L'=' );
		if( *pstrList++ != L'=' ) return this;
		assert( *pstrList == L'\"' );
		if( *pstrList++ != L'\"' ) return this;
		while( *pstrList != L'\0' && *pstrList != L'\"' ) {
			PWSTR pstrTemp = ::CharNext(pstrList);
			while( pstrList < pstrTemp) {
				sValue += *pstrList++;
			}
		}
		assert( *pstrList == L'\"' );
		if( *pstrList++ != L'\"' ) return this;
		setAttribute(sItem.c_str(), sValue.c_str());
		if( *pstrList++ != L' ' ) return this;
	}
	return this;
}

SIZE controlUI::estimateSize(SIZE szAvailable)
{
	return m_cxyFixed;
}

void controlUI::doPaint(HDC hDC, const RECT& rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

	// »æÖÆÑ­Ðò£º±³¾°ÑÕÉ«->±³¾°Í¼->×´Ì¬Í¼->ÎÄ±¾->±ß¿ò
	if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ) {
		renderClip roundClip;
		renderClip::generateRoundClip(hDC, m_rcPaint,  m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
		paintBkColor(hDC);
		paintBkImage(hDC);
		paintStatusImage(hDC);
		paintText(hDC);
		paintBorder(hDC);
	}
	else {
		paintBkColor(hDC);
		paintBkImage(hDC);
		paintStatusImage(hDC);
		paintText(hDC);
		paintBorder(hDC);
	}
}

void controlUI::paintBkColor(HDC hDC)
{
	if( m_dwBackColor != 0 ) {
		if( m_dwBackColor2 != 0 ) {
			if( m_dwBackColor3 != 0 ) {
				RECT rc = m_rcItem;
				rc.bottom = (rc.bottom + rc.top) / 2;
				renderEngine::drawGradient(hDC, rc, getAdjustColor(m_dwBackColor), getAdjustColor(m_dwBackColor2), true, 8);
				rc.top = rc.bottom;
				rc.bottom = m_rcItem.bottom;
				renderEngine::drawGradient(hDC, rc, getAdjustColor(m_dwBackColor2), getAdjustColor(m_dwBackColor3), true, 8);
			}
			else 
				renderEngine::drawGradient(hDC, m_rcItem, getAdjustColor(m_dwBackColor), getAdjustColor(m_dwBackColor2), true, 16);
		}
		else if( m_dwBackColor >= 0xFF000000 ) renderEngine::drawColor(hDC, m_rcPaint, getAdjustColor(m_dwBackColor));
		else renderEngine::drawColor(hDC, m_rcItem, getAdjustColor(m_dwBackColor));
	}
}

void controlUI::paintBkImage(HDC hDC)
{
	if( m_sBkImage.empty() ) return;
	if( !drawImage(hDC, m_sBkImage.c_str()) ) m_sBkImage.clear();
}

void controlUI::paintStatusImage(HDC hDC)
{
	return;
}

void controlUI::paintText(HDC hDC)
{
	return;
}

void controlUI::paintBorder(HDC hDC)
{
	if( m_nBorderSize > 0 )
	{
		if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 )
		{
			if (isFocused() && m_dwFocusBorderColor != 0)
				renderEngine::drawRoundRect(hDC, m_rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, getAdjustColor(m_dwFocusBorderColor));
			else
				renderEngine::drawRoundRect(hDC, m_rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, getAdjustColor(m_dwBorderColor));
		}
		else
		{
			if (isFocused() && m_dwFocusBorderColor != 0)
				renderEngine::drawRect(hDC, m_rcItem, m_nBorderSize, getAdjustColor(m_dwFocusBorderColor));
			else if (m_dwBorderColor != 0)
				renderEngine::drawRect(hDC, m_rcItem, m_nBorderSize, getAdjustColor(m_dwBorderColor));
		}
	}
}

void controlUI::doPostPaint(HDC hDC, const RECT& rcPaint)
{
	return;
}
}