#include "StdAfx.h"
#include "UCcontrol.h"

namespace wuc {


controlUC::controlUC() 
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
	m_dwBackColor(0xFFFFFFFF),
	m_dwBorderColor(0),
	m_dwFocusBorderColor(0),
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
	::ZeroMemory(&m_tRelativePos, sizeof(BSRelativePosUC));
}

controlUC::~controlUC()
{
	if( m_pManager != NULL ) m_pManager->reapObjects(this);
}

std::wstring controlUC::getName() const
{
	return m_wsName;
}

void controlUC::setName(PCWSTR pstrName)
{
	m_wsName = pstrName;
}

LPVOID controlUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Control") == 0 ) return this;
	return NULL;
}

PCWSTR controlUC::getClass() const
{
	return L"ControlUI";
}

UINT controlUC::getControlFlags() const
{
	return 0;
}

bool controlUC::activate()
{
	if( !isVisible() ) return false;
	if( !isEnabled() ) return false;
	return true;
}

paintManagerUC* controlUC::getManager() const
{
	return m_pManager;
}

void controlUC::setManager(paintManagerUC* pManager, controlUC* pParent, bool bInit)
{
	m_pManager = pManager;
	m_pParent = pParent;
	if( bInit && m_pParent ) init();
}

controlUC* controlUC::getParent() const
{
	return m_pParent;
}

std::wstring controlUC::getText() const
{
	return m_wsText;
}

void controlUC::setText(PCWSTR pstrText)
{
	if( m_wsText == pstrText ) return;

	m_wsText = pstrText;
	invalidate();
}

DWORD controlUC::getBkColor() const
{
	return m_dwBackColor;
}

void controlUC::setBkColor(DWORD dwBackColor)
{
	if( m_dwBackColor == dwBackColor ) return;

	m_dwBackColor = dwBackColor;
	invalidate();
}
void controlUC::setBkImage(PCWSTR pkeyImage)
{
	gtc::image *pImage = m_pManager->findImage(pkeyImage);
	RECT rc = {0};
	if (pImage)
	{
		rc.right = pImage->getWidth();
		rc.bottom = pImage->getHeight();
	}
	setBkImage(pkeyImage, rc);
}

void controlUC::setBkImage(PCWSTR pkeyImage, const RECT &rcScr)
{
	m_bsBackImage.wsKeyImage = pkeyImage;
	m_bsBackImage.rcScource = rcScr;
	invalidate();
}

DWORD controlUC::getBorderColor() const
{
	return m_dwBorderColor;
}

void controlUC::setBorderColor(DWORD dwBorderColor)
{
	if( m_dwBorderColor == dwBorderColor ) return;

	m_dwBorderColor = dwBorderColor;
	invalidate();
}

DWORD controlUC::getFocusBorderColor() const
{
	return m_dwFocusBorderColor;
}

void controlUC::setFocusBorderColor(DWORD dwBorderColor)
{
	if( m_dwFocusBorderColor == dwBorderColor ) return;

	m_dwFocusBorderColor = dwBorderColor;
	invalidate();
}

int controlUC::getBorderSize() const
{
	return m_nBorderSize;
}

void controlUC::setBorderSize(int nSize)
{
	if( m_nBorderSize == nSize ) return;

	m_nBorderSize = nSize;
	invalidate();
}

SIZE controlUC::getBorderRound() const
{
	return m_cxyBorderRound;
}

void controlUC::setBorderRound(SIZE cxyRound)
{
	m_cxyBorderRound = cxyRound;
	invalidate();
}

bool controlUC::drawImage(HDC hDC, PCWSTR pStrImage, PCWSTR pStrModify)
{
	//return renderEngine::drawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, pStrImage, pStrModify);
	return true;
}

const RECT& controlUC::getPos() const
{
	return m_rcItem;
}

void controlUC::setPos(RECT rc)
{
	if( rc.right < rc.left ) rc.right = rc.left;
	if( rc.bottom < rc.top ) rc.bottom = rc.top;

	gtc::rect invalidateRc = m_rcItem;
	if(invalidateRc.isEmptyArea())invalidateRc = rc;
	m_rcItem = rc;
	if( m_pManager == NULL ) return;

	if( m_bFloat ) {
		controlUC* pParent = getParent();
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
		//if( onSize ) onSize(this);
		m_bSetPos = false;
	}

	m_bUpdateNeeded = false;

	// NOTE: SetPos() is usually called during the WM_PAINT cycle where all controls are
	//       being laid out. Calling UpdateLayout() again would be wrong. Refreshing the
	//       window won't hurt (if we're already inside WM_PAINT we'll just validate it out).
	invalidateRc.join(m_rcItem);

	controlUC* pParent = this;
	RECT rcTemp;
	RECT rcParent;
	while( pParent = pParent->getParent() )
	{
		//rcTemp = invalidateRc;
		rcParent = pParent->getPos();
		//if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
		//{
		//	return;
		//}
		if(!invalidateRc.intersect(rcParent)) return;
	}
	m_pManager->invalidate(rcTemp = invalidateRc);
}

int controlUC::getWidth() const
{
	return m_rcItem.right - m_rcItem.left;
}

int controlUC::getHeight() const
{
	return m_rcItem.bottom - m_rcItem.top;
}

int controlUC::getX() const
{
	return m_rcItem.left;
}

int controlUC::getY() const
{
	return m_rcItem.top;
}

RECT controlUC::getPadding() const
{
	return m_rcPadding;
}

void controlUC::setPadding(RECT rcPadding)
{
	m_rcPadding = rcPadding;
	needParentUpdate();
}

SIZE controlUC::getFixedXY() const
{
	return m_cXY;
}

void controlUC::setFixedXY(SIZE szXY)
{
	m_cXY.cx = szXY.cx;
	m_cXY.cy = szXY.cy;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUC::getFixedWidth() const
{
	return m_cxyFixed.cx;
}

void controlUC::setFixedWidth(int cx)
{
	if( cx < 0 ) return; 
	m_cxyFixed.cx = cx;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUC::getFixedHeight() const
{
	return m_cxyFixed.cy;
}

void controlUC::setFixedHeight(int cy)
{
	if( cy < 0 ) return; 
	m_cxyFixed.cy = cy;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUC::getMinWidth() const
{
	return m_cxyMin.cx;
}

void controlUC::setMinWidth(int cx)
{
	if( m_cxyMin.cx == cx ) return;

	if( cx < 0 ) return; 
	m_cxyMin.cx = cx;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUC::getMaxWidth() const
{
	return m_cxyMax.cx;
}

void controlUC::setMaxWidth(int cx)
{
	if( m_cxyMax.cx == cx ) return;

	if( cx < 0 ) return; 
	m_cxyMax.cx = cx;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUC::getMinHeight() const
{
	return m_cxyMin.cy;
}

void controlUC::setMinHeight(int cy)
{
	if( m_cxyMin.cy == cy ) return;

	if( cy < 0 ) return; 
	m_cxyMin.cy = cy;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

int controlUC::getMaxHeight() const
{
	return m_cxyMax.cy;
}

void controlUC::setMaxHeight(int cy)
{
	if( m_cxyMax.cy == cy ) return;

	if( cy < 0 ) return; 
	m_cxyMax.cy = cy;
	if( !m_bFloat ) needParentUpdate();
	else needUpdate();
}

void controlUC::setRelativePos(SIZE szMove,SIZE szZoom)
{
	m_tRelativePos.bRelative = TRUE;
	m_tRelativePos.nMoveXPercent = szMove.cx;
	m_tRelativePos.nMoveYPercent = szMove.cy;
	m_tRelativePos.nZoomXPercent = szZoom.cx;
	m_tRelativePos.nZoomYPercent = szZoom.cy;
}

void controlUC::setRelativeParentSize(SIZE sz)
{
	m_tRelativePos.szParent = sz;
}

BSRelativePosUC controlUC::getRelativePos() const
{
	return m_tRelativePos;
}

bool controlUC::isRelativePos() const
{
	return m_tRelativePos.bRelative;
}

std::wstring controlUC::getToolTip() const
{
	return m_wsToolTip;
}

void controlUC::setToolTip(PCWSTR pstrText)
{
	m_wsToolTip = pstrText;
}


wchar_t controlUC::getShortcut() const
{
	return m_chShortcut;
}

void controlUC::setShortcut(TCHAR ch)
{
	m_chShortcut = ch;
}

bool controlUC::isContextMenuUsed() const
{
	return m_bMenuUsed;
}

void controlUC::setContextMenuUsed(bool bMenuUsed)
{
	m_bMenuUsed = bMenuUsed;
}

const std::wstring& controlUC::getUserData()
{
	return m_wsUserData;
}

void controlUC::setUserData(PCWSTR pstrText)
{
	m_wsUserData = pstrText;
}

UINT_PTR controlUC::getTag() const
{
	return m_pTag;
}

void controlUC::setTag(UINT_PTR pTag)
{
	m_pTag = pTag;
}

bool controlUC::isVisible() const
{
	return m_bVisible && m_bInternVisible;
}

void controlUC::setVisible(bool bVisible)
{
	if( m_bVisible == bVisible ) return;

	bool v = isVisible();
	m_bVisible = bVisible;
	if( m_bFocused ) m_bFocused = false;
	if( isVisible() != v ) {
		needParentUpdate();
	}
}

void controlUC::setInternVisible(bool bVisible)
{
	m_bInternVisible = bVisible;
}

bool controlUC::isEnabled() const
{
	return m_bEnabled;
}

void controlUC::setEnabled(bool bEnabled)
{
	if( m_bEnabled == bEnabled ) return;

	m_bEnabled = bEnabled;
	invalidate();
}

bool controlUC::isMouseEnabled() const
{
	return m_bMouseEnabled;
}

void controlUC::setMouseEnabled(bool bEnabled)
{
	m_bMouseEnabled = bEnabled;
}

bool controlUC::isFocused() const
{
	return m_bFocused;
}

void controlUC::setFocus()
{
	if( m_pManager != NULL ) m_pManager->setFocus(this);
}

bool controlUC::isFloat() const
{
	return m_bFloat;
}

void controlUC::setFloat(bool bFloat)
{
	if( m_bFloat == bFloat ) return;

	m_bFloat = bFloat;
	needParentUpdate();
}

controlUC* controlUC::findControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if( (uFlags & UCFIND_VISIBLE) != 0 && !isVisible() ) return NULL;
	if( (uFlags & UCFIND_ENABLED) != 0 && !isEnabled() ) return NULL;
	if( (uFlags & UCFIND_HITTEST) != 0 && (!m_bMouseEnabled || !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) ) return NULL;
	return Proc(this, pData);
}

void controlUC::invalidate()
{
	if( !isVisible() ) return;

	RECT invalidateRc = m_rcItem;

	controlUC* pParent = this;
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

bool controlUC::isUpdateNeeded() const
{
	return m_bUpdateNeeded;
}

void controlUC::needUpdate()
{
	if( !isVisible() ) return;
	m_bUpdateNeeded = true;
	invalidate();

	if( m_pManager != NULL ) m_pManager->needUpdate();
}

void controlUC::needParentUpdate()
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



void controlUC::init()
{
}

void controlUC::Event(BSEventUC& event)
{
	if( event.Type == BEUCEventSetCursor )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return;
	}
	if( event.Type == BEUCEventSetFocus ) 
	{
		m_bFocused = true;
		invalidate();
		return;
	}
	if( event.Type == BEUCEventKillFocus ) 
	{
		m_bFocused = false;
		invalidate();
		return;
	}
	if( event.Type == BEUCEventTimer )
	{
		m_pManager->sendNotify(this, L"timer", event.wParam, event.lParam);
		return;
	}
	if( event.Type == BEUCEventContextMenu )
	{
		if( isContextMenuUsed() ) {
			m_pManager->sendNotify(this, L"menu", event.wParam, event.lParam);
			return;
		}
	}
	if( m_pParent != NULL ) m_pParent->Event(event);
}

void controlUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
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
	else if( _tcscmp(pstrName, L"bordersize") == 0 ) setBorderSize(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"borderround") == 0 ) {
		SIZE cxyRound = { 0 };
		PWSTR pstr = NULL;
		cxyRound.cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);     
		setBorderRound(cxyRound);
	}
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

controlUC* controlUC::applyAttributeList(PCWSTR pstrList)
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

SIZE controlUC::estimateSize(SIZE szAvailable)
{
	return m_cxyFixed;
}

void controlUC::paint(HDC hDC, const RECT& rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

	// »æÖÆÑ­Ðò£º±³¾°ÑÕÉ«->±³¾°Í¼->×´Ì¬Í¼->ÎÄ±¾->±ß¿ò
	if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ) {
		//renderClip roundClip;
		//renderClip::generateRoundClip(hDC, m_rcPaint,  m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
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

void controlUC::paintBkColor(HDC hDC)
{
	Gdiplus::Graphics grap(hDC);
	if( m_dwBackColor != 0 ) {
		gtc::drawer::fillRectangle(grap, m_dwBackColor, m_rcItem);
	}
}

void controlUC::paintBkImage(HDC hDC)
{
	assert(hDC);
	gtc::image *pImage = nullptr;
	if (!m_bsBackImage.wsKeyImage.empty() && (pImage = m_pManager->findImage(m_bsBackImage.wsKeyImage)))
	{
		Gdiplus::Graphics grap(hDC);
		gtc::drawer::drawImage(grap, *pImage, m_rcItem, m_bsBackImage.rcScource);
		//gtc::drawer::drawString(grap, L"12431afasfja;", m_rcItem, m_pManager->getDefaultFontInfo()->hFont, BEAlignModeLeftTop, 0xFFFFFFFF);
	}
}

void controlUC::paintStatusImage(HDC hDC)
{
	return;
}

void controlUC::paintText(HDC hDC)
{
	return;
}

void controlUC::paintBorder(HDC hDC)
{
	//if( m_nBorderSize > 0 )
	//{
	//	if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 )
	//	{
	//		if (isFocused() && m_dwFocusBorderColor != 0)
	//			renderEngine::drawRoundRect(hDC, m_rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, getAdjustColor(m_dwFocusBorderColor));
	//		else
	//			renderEngine::drawRoundRect(hDC, m_rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, getAdjustColor(m_dwBorderColor));
	//	}
	//	else
	//	{
	//		if (isFocused() && m_dwFocusBorderColor != 0)
	//			renderEngine::drawRect(hDC, m_rcItem, m_nBorderSize, getAdjustColor(m_dwFocusBorderColor));
	//		else if (m_dwBorderColor != 0)
	//			renderEngine::drawRect(hDC, m_rcItem, m_nBorderSize, getAdjustColor(m_dwBorderColor));
	//	}
	//}
}

void controlUC::doPostPaint(HDC hDC, const RECT& rcPaint)
{
	return;
}
}