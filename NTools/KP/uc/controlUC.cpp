#include "controlUC.h"

namespace gtc{
controlUC::controlUC(void)
	: m_pwndManager(nullptr), m_wsName(L""), m_pParent(nullptr)
	, m_wsText(L"")
	, m_bVisible(true), m_bFocused(false), m_bEnabled(true), m_bMouseEnabled(true)
	, m_bFloat(true)
	, m_dwBackColor(0x0), m_dwBorderColor(0x0), m_dwFocusBorderColor(0x0)
	, m_bUpdateNeeded(true)
	, m_bMenuUsed(false), m_chShortcut(L'\0')
	, m_nBorderSize(1)
{
	m_cxyMin.cx = m_cxyMin.cy = 0;
	m_cxyMax.cx = m_cxyMax.cy = 9999;
	m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

	::ZeroMemory(&m_rcItem, sizeof(RECT));
}


controlUC::~controlUC(void)
{
}

const std::wstring& controlUC::getName() const
{
	return m_wsName;
}
void controlUC::setName(PCWSTR pstrName)
{
	m_wsName = pstrName?pstrName:L"";
}
PCWSTR controlUC::getClass() const
{
	return L"controlUC";
}

LPVOID controlUC::getInterface(PCWSTR pstrName)
{
	if(std::wcscmp(pstrName, L"Control") == 0) return static_cast<controlUC *>(this);
	return nullptr;
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
		setPosition(rcPos);
	}
	else if( _tcscmp(pstrName, L"bkcolor") == 0 ) {
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
	else if( _tcscmp(pstrName, L"minwidth") == 0 ) setMinWidth(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"minheight") == 0 ) setMinHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"maxwidth") == 0 ) setMaxWidth(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"maxheight") == 0 ) setMaxHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"name") == 0 ) setName(pstrValue);
	else if( _tcscmp(pstrName, L"text") == 0 ) setText(pstrValue);
	else if( _tcscmp(pstrName, L"tooltip") == 0 ) setToolTip(pstrValue);
	else if( _tcscmp(pstrName, L"enabled") == 0 ) setEnabled(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"mouse") == 0 ) setMouseEnabled(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"visible") == 0 ) setVisible(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"shortcut") == 0 ) setShortcut(pstrValue[0]);
	else if( _tcscmp(pstrName, L"menu") == 0 ) setContextMenuUsed(_tcscmp(pstrValue, L"true") == 0);
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
	 needUpdate();
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
	needUpdate();
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
	needUpdate();
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
	needUpdate();
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

bool controlUC::activate()
{
	if( !isVisible() ) return false;
	if( !isEnabled() ) return false;
	return true;
}

windowUC* controlUC::getManager()
{
	return m_pwndManager;
}

void controlUC::setManager(windowUC *pManager, controlUC *pParent)
{
	m_pwndManager = pManager;
	m_pParent = pParent;
}

controlUC* controlUC::getParent() const
{
	return m_pParent;
}

// 文本相关
PCWSTR controlUC::getText() const
{
	return m_wsText.c_str();
}
void controlUC::setText(PCWSTR pstrText)
{
	if(m_wsText.compare(pstrText) == 0) return;
	m_wsText = pstrText;
	invalidate();
}
// 一些重要的属性
bool controlUC::isVisible() const
{
	return m_bVisible;
}
void controlUC::setVisible(bool bVisible /*= true*/)
{
	if(m_bVisible == bVisible) return;
	bool isv = isVisible();
	m_bVisible = bVisible;
	if( m_bFocused ) m_bFocused = false;
	if( isVisible() != isv ) {
		needParentUpdate();
	}
}
bool controlUC::isEnabled() const
{
	return m_bEnabled;
}
void controlUC::setEnabled(bool bEnabled /*= true*/)
{
	if( m_bEnabled == bEnabled ) return;

	m_bEnabled = bEnabled;
	invalidate();
}

bool controlUC::isMouseEnabled() const
{
	return m_bMouseEnabled;
}

void controlUC::setMouseEnabled(bool bEnable /*= true*/)
{
	m_bMouseEnabled = bEnable;
}

bool controlUC::isFocused() const
{
	return m_bFocused;
}
void controlUC::setFocus(bool bFocus /*= true*/)
{
	if(m_bFocused == bFocus) return;
	m_bFocused = bFocus;
	invalidate();
}
bool controlUC::isFloat() const
{
	return m_bFloat;
}
void controlUC::setFloat(bool bFloat /*= true*/)
{
	if( m_bFloat == bFloat ) return;

	m_bFloat = bFloat;
	needParentUpdate();
}
RECT controlUC::getPosition() const
{
	return m_rcItem;
}

void controlUC::setPosition(RECT &pos)
{
	if( pos.right < pos.left ) pos.right = pos.left;
	if( pos.bottom < pos.top ) pos.bottom = pos.top;

	RECT invalidateRc = m_rcItem;
	if( ::IsRectEmpty(&invalidateRc) ) invalidateRc = pos;

	m_rcItem = pos;
	if( m_rcItem.left < invalidateRc.left ) invalidateRc.left = m_rcItem.left;
	if( m_rcItem.top < invalidateRc.top ) invalidateRc.top = m_rcItem.top;
	if( m_rcItem.right > invalidateRc.right ) invalidateRc.right = m_rcItem.right;
	if( m_rcItem.bottom > invalidateRc.bottom ) invalidateRc.bottom = m_rcItem.bottom;

	controlUC* pParent = this;
	RECT rcTemp;
	RECT rcParent;
	while( pParent = pParent->getParent() )
	{
		rcTemp = invalidateRc;
		rcParent = pParent->getPosition();
		if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
		{
			return;
		}
	}
	if(m_pwndManager) m_pwndManager->invalidate(invalidateRc);
}

bool controlUC::ptIn(const POINT &pt)
{
	RECT rc = getPosition();
	//m_pwndManager->clientToScreen(&rc);
	return ::PtInRect(&rc, pt) == TRUE;
}

void controlUC::clientToScreen(RECT *prect)
{
	assert(m_pwndManager);
	m_pwndManager->clientToScreen(prect);
}

void controlUC::clientToScreen(POINT *ppt)
{
	assert(m_pwndManager);
	//POINT pt = *ppt;
	m_pwndManager->clientToScreen(ppt);
	//SEND_LOG(L"windowUC::clientToScreen old<%s> <%s> <%s>", gtc::format::point(pt).c_str(), 
	//	gtc::format::point(*ppt).c_str(), 
	//	gtc::format::rect(m_pwndManager->getWindowPos()).c_str());

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
		rcParent = pParent->getPosition();
		if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
		{
			return;
		}
	}

	if( m_pwndManager != NULL ) m_pwndManager->invalidate(invalidateRc);
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

	if( m_pwndManager != NULL ) m_pwndManager->needUpdate();
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

	if( m_pwndManager != NULL ) m_pwndManager->needUpdate();
}


void controlUC::paint(HDC hdc)
{
	//SEND_LOG(L"controlUC::paint");
	paintBkColor(hdc);
	paintBkImage(hdc);
	paintStatusImage(hdc);
	paintText(hdc);
	paintBorder(hdc);
}

void controlUC::paintBkColor(HDC hdc)
{

}
void controlUC::paintBkImage(HDC hdc)
{

}
void controlUC::paintStatusImage(HDC hdc)
{

}
void controlUC::paintText(HDC hdc)
{

}
void controlUC::paintBorder(HDC hdc)
{

}
void controlUC::Event(BSEventUC &event)
{
	bool done = true;
	switch(event.Type)
	{
	case BEUCEventSetCursor:
		//::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		break;
	case BEUCEventSetFocus:
		m_bFocused = true;
		invalidate();
		break;
	case BEUCEventKillFocus:
		m_bFocused = false;
		invalidate();
		break;
	default:
		done = false;
		break;
	}
	// 
	if(done) return;
	if(m_pParent) m_pParent->Event(event);
}
bool controlUC::isContextMenuUsed() const
{
	return m_bMenuUsed;
}
void controlUC::setContextMenuUsed(bool bMenuUsed)
{
	m_bMenuUsed = bMenuUsed;
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
}