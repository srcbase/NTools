#include "wucCombo.h"
#include "windowWnd.h"
#include "wucBase.h"
namespace wuc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	comboWnd  类		2015-8-6
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class comboWnd : public windowWnd
{
public:
	void init(comboUC* pOwner);
	virtual PCWSTR getWindowClassName() const;
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void onFinalMessage(HWND hWnd);

	void ensureVisible(int iIndex);
	void scroll(int dx, int dy);

public:
	drawManager			m_pm;
	comboUC				*m_pOwner;
	verticalLayoutUC	*m_pLayout;
	int					m_iOldSel;
};


void comboWnd::init(comboUC* pOwner)
{
	m_pOwner = pOwner;
	m_pLayout = NULL;
	m_iOldSel = m_pOwner->getCurSel();

	// Position the popup window in absolute space
	SIZE szDrop = m_pOwner->getDropBoxSize();
	RECT rcOwner = pOwner->getPosition();
	RECT rc = rcOwner;
	rc.top = rc.bottom;
	rc.bottom = rc.top + szDrop.cy;
	if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;

	SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
	int cyFixed = 0;
	for( int it = 0; it < pOwner->getCount(); it++ ) {
		controlUC* pControl = static_cast<controlUC*>(pOwner->getItemAt(it));
		if( !pControl->isVisible() ) continue;
		SIZE sz = pControl->estimateSize(szAvailable);
		cyFixed += sz.cy;
	}
	cyFixed += 4; // CVerticalLayoutUI 默认的Inset 调整
	rc.bottom = rc.top + min(cyFixed, szDrop.cy);

	::MapWindowRect(pOwner->getManager()->getPaintWindow(), HWND_DESKTOP, &rc);

	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	RECT rcWork = oMonitor.rcWork;
	if( rc.bottom > rcWork.bottom ) {
		rc.left = rcOwner.left;
		rc.right = rcOwner.right;
		if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;
		rc.top = rcOwner.top - min(cyFixed, szDrop.cy);
		rc.bottom = rcOwner.top;
		::MapWindowRect(pOwner->getManager()->getPaintWindow(), HWND_DESKTOP, &rc);
	}

	create(pOwner->getManager()->getPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
	// HACK: Don't deselect the parent's caption
	HWND hWndParent = m_hWnd;
	while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
	::ShowWindow(m_hWnd, SW_SHOW);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

PCWSTR comboWnd::getWindowClassName() const
{
	return L"ComboWnd";
}

void comboWnd::onFinalMessage(HWND hWnd)
{
	m_pOwner->m_pWindow = NULL;
	m_pOwner->m_uButtonState &= ~ UCSTATE_PUSHED;
	m_pOwner->invalidate();
	delete this;
}

LRESULT comboWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( uMsg == WM_CREATE ) {
		m_pm.init(m_hWnd);
		// The trick is to add the items to the new container. Their owner gets
		// reassigned by this operation - which is why it is important to reassign
		// the items back to the righfull owner/manager when the window closes.
		m_pLayout = new verticalLayoutUC;
		//m_pm.useParentResource(m_pOwner->GetManager());
		m_pLayout->setManager(&m_pm, NULL, true);
		//LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayout"));
		//if( pDefaultAttributes ) {
		//	m_pLayout->ApplyAttributeList(pDefaultAttributes);
		//}
		m_pLayout->setInset(2, 2, 2, 2);
		m_pLayout->setBkColor(0xFEFFFFFF);
		m_pLayout->setBorderColor(0xFE85E4FF);
		m_pLayout->setBorderSize(2);
		m_pLayout->setAutoDestroy(false);
		m_pLayout->enableScrollBar();
		m_pLayout->applyAttributeList(m_pOwner->getDropBoxAttributeList());
		for( int i = 0; i < m_pOwner->getCount(); i++ ) {
			m_pLayout->add(static_cast<controlUC*>(m_pOwner->getItemAt(i)));
		}
		m_pm.attachDialog(m_pLayout);

		return 0;
	}
	else if( uMsg == drawManager::WUC_MSG_WINDOW_INIT)
	{
		assert(m_pOwner);
		//SEND_LOG(L"------> flag:%d size:%d,%d", wParam, HIWORD(lParam), LOWORD(lParam));
		//if(!m_pm.m_bFirstLayout)
		if(m_pOwner) ensureVisible(m_pOwner->getCurSel());
		//SIZE sz = {0, m_pOwner->getCurSel()*18};
		//m_pLayout->setScrollPos(sz);
	}
	else if( uMsg == WM_CLOSE ) {
		m_pOwner->setManager(m_pOwner->getManager(), m_pOwner->getParent(), false);
		m_pOwner->setPosition(m_pOwner->getPosition());
		m_pOwner->setFocus();
	}
	else if( uMsg == WM_LBUTTONUP ) {
		POINT pt = { 0 };
		::GetCursorPos(&pt);
		::ScreenToClient(m_pm.getPaintWindow(), &pt);
		controlUC* pControl = m_pm.findControl(pt);
		if( pControl && _tcscmp(pControl->getClass(), L"scrollBarUC") != 0 ) postMessage(WM_KILLFOCUS);
	}
	else if( uMsg == WM_KEYDOWN ) {
		switch( wParam ) {
		case VK_ESCAPE:
			m_pOwner->selectItem(m_iOldSel);
			ensureVisible(m_iOldSel);
			// FALL THROUGH...
		case VK_RETURN:
			postMessage(WM_KILLFOCUS);
			break;
		default:
			BSEventUC event;
			event.Type = BEUCEventKeyDown;
			event.chKey = (TCHAR)wParam;
			m_pOwner->Event(event);
			ensureVisible(m_pOwner->getCurSel());
			return 0;
		}
	}
	else if( uMsg == WM_MOUSEWHEEL ) {
		int zDelta = (int) (short) HIWORD(wParam);
		BSEventUC event;
		event.Type = BEUCEventScrollWheel;
		event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
		event.lParam = lParam;
		event.dwTimestamp = ::GetTickCount();
		m_pLayout->Event(event);
		//m_pOwner->Event(event);
		//ensureVisible(m_pOwner->getCurSel());
		return 0;
	}
	else if( uMsg == WM_KILLFOCUS ) {
		if( m_hWnd != (HWND) wParam ) postMessage(WM_CLOSE);
	}

	LRESULT lRes = 0;
	if( m_pm.messageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return windowWnd::handleMessage(uMsg, wParam, lParam);
}

void comboWnd::ensureVisible(int iIndex)
{
	if( m_pOwner->getCurSel() < 0 ) return;
	m_pLayout->findSelectable(m_pOwner->getCurSel(), false);
	RECT rcItem = m_pLayout->getItemAt(iIndex)->getPosition();
	RECT rcList = m_pLayout->getPosition();
	scrollBarUC* pHorizontalScrollBar = m_pLayout->getHorizontalScrollBar();
	if( pHorizontalScrollBar && pHorizontalScrollBar->isVisible() ) rcList.bottom -= pHorizontalScrollBar->getFixedHeight();
	int iPos = m_pLayout->getScrollPos().cy;
	if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
	int dx = 0;
	if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
	if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
	dx += rectHeight(rcList) - rectHeight(rcItem);		// 确保显示行在最上显示
	scroll(0, dx);
}

void comboWnd::scroll(int dx, int dy)
{
	if( dx == 0 && dy == 0 ) return;
	SIZE sz = m_pLayout->getScrollPos();
	sz.cx += dx;
	sz.cy += dy;
	m_pLayout->setScrollPos(sz);
	//m_pLayout->setScrollPos(CSize(sz.cx + dx, sz.cy + dy));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	comboUC  类		2015-8-6
//	对ComboWnd控件的封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
comboUC::comboUC()
	: m_pWindow(NULL)
	, m_iCurSel(-1), m_uButtonState(0)
{
	m_szDropBox.cx = 0;
	m_szDropBox.cy = 150;
	::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));

	m_listInfo.iColumns = 0;
	m_listInfo.iFont = -1;
	m_listInfo.uTextStyle = DT_VCENTER;
	m_listInfo.clrText = 0xFE000000;
	m_listInfo.clrBk = 0;
	m_listInfo.bAlternateBk = false;
	m_listInfo.clrSelectedText = 0xFE000000;
	m_listInfo.clrSelectedBk = 0xFEC1E3FF;
	m_listInfo.clrHotText = 0xFE000000;
	m_listInfo.clrHotBk = 0xFEE9F5FF;
	m_listInfo.clrDisabledText = 0xFECCCCCC;
	m_listInfo.clrDisabledBk = 0xFEFFFFFF;
	m_listInfo.clrLine = 0;
	m_listInfo.bShowHtml = false;
	m_listInfo.bMultiExpandable = false;
	::ZeroMemory(&m_listInfo.rcTextPadding, sizeof(m_listInfo.rcTextPadding));
	::ZeroMemory(&m_listInfo.rcColumn, sizeof(m_listInfo.rcColumn));
}
comboUC::~comboUC()
{

}

PCWSTR comboUC::getClass() const
{
	return L"comboUC";
}
LPVOID comboUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Combo") == 0 ) return static_cast<comboUC*>(this);
	if( _tcscmp(pstrName, L"IListOwner") == 0 ) return static_cast<IListOwnerUC*>(this);
	return containerUC::getInterface(pstrName);
}
UINT comboUC::getControlFlags() const
{
	return UCFLAG_TABSTOP;
}

void comboUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else containerUC::Event(event);
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
	if( event.Type == BEUCEventButtonDown )
	{
		if( isEnabled() ) {
			activate();
			m_uButtonState |= UCSTATE_PUSHED | UCSTATE_CAPTURED;
		}
		return;
	}
	if( event.Type == BEUCEventButtonUp )
	{
		if( (m_uButtonState & UCSTATE_CAPTURED) != 0 ) {
			m_uButtonState &= ~ UCSTATE_CAPTURED;
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventMouseMove )
	{
		return;
	}
	if( event.Type == BEUCEventKeyDown )
	{
		switch( event.chKey ) {
		case VK_F4:
			activate();
			return;
		case VK_UP:
			selectItem(findSelectable(m_iCurSel - 1, false));
			return;
		case VK_DOWN:
			selectItem(findSelectable(m_iCurSel + 1, true));
			return;
		case VK_PRIOR:
			selectItem(findSelectable(m_iCurSel - 1, false));
			return;
		case VK_NEXT:
			selectItem(findSelectable(m_iCurSel + 1, true));
			return;
		case VK_HOME:
			selectItem(findSelectable(0, false));
			return;
		case VK_END:
			selectItem(findSelectable(getCount() - 1, true));
			return;
		}
	}
	if( event.Type == BEUCEventScrollWheel )
	{
		bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
		selectItem(findSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
		return;
	}
	if( event.Type == BEUCEventContextMenu )
	{
		return;
	}
	if( event.Type == BEUCEventMouseEnter )
	{
		if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
			if( (m_uButtonState & UCSTATE_HOT) == 0  )
				m_uButtonState |= UCSTATE_HOT;
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventMouseLeave )
	{
		if( (m_uButtonState & UCSTATE_HOT) != 0 ) {
			m_uButtonState &= ~UCSTATE_HOT;
			invalidate();
		}
		return;
	}
	controlUC::Event(event);
}
SIZE comboUC::estimateSize(SIZE szAvailable)
{
	if( m_szxyFixed.cy == 0 )
	{
		SIZE sz = {m_szxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 12};
	}
	return controlUC::estimateSize(szAvailable);
}
void comboUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = NULL;
	if( _tcscmp(pstrName, L"textpadding") == 0 ) {
		RECT rcTextPadding = { 0 };
		rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setTextPadding(rcTextPadding);
	}
	else if( _tcscmp(pstrName, L"normalimage") == 0 )
	{
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setNormalImage(rcCell);
		//setNormalImage(pstrValue);
	}
	else if( _tcscmp(pstrName, L"hotimage") == 0 )
	{
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setHotImage(rcCell);
		//setHotImage(pstrValue);
	}
	else if( _tcscmp(pstrName, L"pushedimage") == 0 )
	{
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setPushedImage(rcCell);
		//SetPushedImage(pstrValue);
	}
	else if( _tcscmp(pstrName, L"focusedimage") == 0 )
	{
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setFocusedImage(rcCell);
		//SetFocusedImage(pstrValue);
	}
	else if( _tcscmp(pstrName, L"disabledimage") == 0 )
	{
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setDisabledImage(rcCell);
		//SetDisabledImage(pstrValue);
	}
	else if( _tcscmp(pstrName, L"dropbox") == 0 ) setDropBoxAttributeList(pstrValue);
	// 列表单元格属性
	else if( _tcscmp(pstrName, L"itemfont") == 0 ) setItemFont(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"itemlinecolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemLineColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemshowhtml") == 0 ) setItemShowHtml(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"alternatebk") == 0 ) setAlternateBk(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"itemalign") == 0 ) {
		if( _tcsstr(pstrValue, L"left") != NULL ) {
			m_listInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
			m_listInfo.uTextStyle |= DT_LEFT;
		}
		if( _tcsstr(pstrValue, L"center") != NULL ) {
			m_listInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
			m_listInfo.uTextStyle |= DT_CENTER;
		}
		if( _tcsstr(pstrValue, L"right") != NULL ) {
			m_listInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
			m_listInfo.uTextStyle |= DT_RIGHT;
		}
	}
	else if( _tcscmp(pstrName, L"itemendellipsis") == 0 ) {
		if( _tcscmp(pstrValue, L"true") == 0 ) m_listInfo.uTextStyle |= DT_END_ELLIPSIS;
		else m_listInfo.uTextStyle &= ~DT_END_ELLIPSIS;
	}
	else if( _tcscmp(pstrName, L"itemtextpadding") == 0 ) {
		RECT rcPadding = { 0 };
		rcPadding.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setItemTextPadding(rcPadding);
	}
	else if( _tcscmp(pstrName, L"itemtextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itembkcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemselectedtextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemSelectedTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemselectedbkcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemSelectedBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemhottextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemHotTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemhotbkcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemHotBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemcuttextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemCutTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemcutbkcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemCutBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemdisabledtextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemDisabledTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemdisabledbkcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemDisabledBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemimage") == 0 ) setItemImage(pstrValue);
	else if( _tcscmp(pstrName, L"itemnormalimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setItemNormalImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"itemselectedimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setItemSelectedImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"itemcutedimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setItemCutedImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"itemhotimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setItemHotImage(rcCell);
	}
	else if( _tcscmp(pstrName, L"itemdisabledimage") == 0 ) {
		RECT rcCell = { 0 };
		rcCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
		setItemDisabledImage(rcCell);
	}
	else containerUC::setAttribute(pstrName, pstrValue);
}
void comboUC::setPosition(RECT pos)
{
	// Put all elements out of sight
	RECT rcNull = { 0 };
	READ_LOCK
	for( int i = 0; i < (int)m_items.size(); i++ )
		m_items[i]->setPosition(rcNull);
	// Position this control
	controlUC::setPosition(pos);
}
void comboUC::paint(HDC hdc, const RECT &rcPaint)
{
	controlUC::paint(hdc, rcPaint);
}
void comboUC::paintText(HDC hdc)
{
	RECT rcText = m_rcItem;
	rcText.left += m_rcTextPadding.left;
	rcText.right -= m_rcTextPadding.right;
	rcText.top += m_rcTextPadding.top;
	rcText.bottom -= m_rcTextPadding.bottom;
	READ_LOCK
	if( m_iCurSel >= 0 ) {
		controlUC* pControl = m_items[m_iCurSel];
		IListItemUC* pElement = static_cast<IListItemUC*>(pControl->getInterface(L"IListItem"));
		if( pElement != NULL ) {
			pElement->drawItem(hdc, rcText);
		}
		else {
			RECT rcOldPos = pControl->getPosition();
			pControl->setPosition(rcText);
			pControl->paint(hdc, rcText);
			pControl->setPosition(rcOldPos);
		}
	}
}
void comboUC::paintStatusImage(HDC hdc)
{
	if( isFocused() ) m_uButtonState |= UCSTATE_FOCUSED;
	else m_uButtonState &= ~ UCSTATE_FOCUSED;
	if( !isEnabled() ) m_uButtonState |= UCSTATE_DISABLED;
	else m_uButtonState &= ~ UCSTATE_DISABLED;

	Gdiplus::Graphics grap(hdc);
	RECT rcSrc = {0};
	if( (m_uButtonState & UCSTATE_DISABLED) != 0 ) {
		if (getDisabledImage(rcSrc))
		{
			gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
			return;
		}
		//if( !m_sDisabledImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
		//	else return;
		//}
	}
	else if( (m_uButtonState & UCSTATE_PUSHED) != 0 ) {
		if (getPushedImage(rcSrc))
		{
			gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
			return;
		}
		//if( !m_sPushedImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)m_sPushedImage) ) m_sPushedImage.Empty();
		//	else return;
		//}
	}
	else if( (m_uButtonState & UCSTATE_HOT) != 0 ) {
		if (getHotImage(rcSrc))
		{
			gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
			return;
		}
		//if( !m_sHotImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) m_sHotImage.Empty();
		//	else return;
		//}
	}
	else if( (m_uButtonState & UCSTATE_FOCUSED) != 0 ) {
		if (getFocusedImage(rcSrc))
		{
			gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
			return;
		}
		//if( !m_sFocusedImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
		//	else return;
		//}
	}
	if (getNormalImage(rcSrc))
	{
		gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
	}
	//if( !m_sNormalImage.IsEmpty() ) {
	//	if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
	//	else return;
	//}
}

void comboUC::setEnabled(bool bEnable /*= true*/)
{
	containerUC::setEnabled(bEnable);
	if( !isEnabled() ) m_uButtonState = 0;
}
PCWSTR comboUC::getText() const
{
	if( m_iCurSel < 0 ) return L"";
	controlUC* pControl = m_items[m_iCurSel];
	return pControl->getText();
}
PCWSTR comboUC::getDropBoxAttributeList()
{
	return m_wsDropBoxAttributes.c_str();
}
void comboUC::setDropBoxAttributeList(PCWSTR pstrList)
{
	m_wsDropBoxAttributes = pstrList ? pstrList : L"";
}
SIZE comboUC::getDropBoxSize() const
{
	return m_szDropBox;
}
void comboUC::setDropBoxSize(SIZE szDropBox)
{
	m_szDropBox = szDropBox;
}
BSListItemInfo* comboUC::getListInfo()
{
	return &m_listInfo;
}
int comboUC::getCurSel() const
{
	return m_iCurSel;
}
bool comboUC::selectItem(int iIndex)
{
	if( m_pWindow != NULL ) m_pWindow->close();
	if( iIndex == m_iCurSel ) return true;
	int iOldSel = m_iCurSel;
	if( m_iCurSel >= 0 ) {
		controlUC* pControl = m_items[m_iCurSel];
		if( !pControl ) return false;
		IListItemUC* pListItem = static_cast<IListItemUC*>(pControl->getInterface(L"IListItem"));
		if( pListItem != NULL ) pListItem->select(false);
		m_iCurSel = -1;
	}
	if( iIndex < 0 ) return false;
	if( m_items.size() == 0 ) return false;
	if( iIndex >= (int)m_items.size() ) iIndex = m_items.size() - 1;
	controlUC* pControl = m_items[iIndex];
	if( !pControl || !pControl->isVisible() || !pControl->isEnabled() ) return false;
	IListItemUC* pListItem = static_cast<IListItemUC*>(pControl->getInterface(L"IListItem"));
	if( pListItem == NULL ) return false;
	m_iCurSel = iIndex;
	if( m_pWindow != NULL ) pControl->setFocus();
	pListItem->select(true);
	if( m_pManager != NULL ) m_pManager->sendNotify(this, L"itemselect", m_iCurSel, iOldSel);
	invalidate();

	return true;
}

bool comboUC::setItemIndex(controlUC* pControl, int iIndex)
{
	int iOrginIndex = getItemIndex(pControl);
	if( iOrginIndex == -1 ) return false;
	if( iOrginIndex == iIndex ) return true;

	IListItemUC* pSelectedListItem = NULL;
	if( m_iCurSel >= 0 ) pSelectedListItem = 
		static_cast<IListItemUC*>(getItemAt(m_iCurSel)->getInterface(L"IListItem"));
	if( !containerUC::setItemIndex(pControl, iIndex) ) return false;
	int iMinIndex = min(iOrginIndex, iIndex);
	int iMaxIndex = max(iOrginIndex, iIndex);
	for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
		controlUC* p = getItemAt(i);
		IListItemUC* pListItem = static_cast<IListItemUC*>(p->getInterface(L"IListItem"));
		if( pListItem != NULL ) {
			pListItem->setIndex(i);
		}
	}
	if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->getIndex();
	return true;
}
bool comboUC::add(controlUC* pControl)
{
	IListItemUC* pListItem = static_cast<IListItemUC*>(pControl->getInterface(L"IListItem"));
	if( pListItem != NULL ) 
	{
		pListItem->setOwner(this);
		pListItem->setIndex(m_items.size());
	}
	return containerUC::add(pControl);
}
bool comboUC::addAt(controlUC* pControl, int iIndex)
{
	if (!containerUC::addAt(pControl, iIndex)) return false;

	// The list items should know about us
	IListItemUC* pListItem = static_cast<IListItemUC*>(pControl->getInterface(L"IListItem"));
	if( pListItem != NULL ) {
		pListItem->setOwner(this);
		pListItem->setIndex(iIndex);
	}

	for(int i = iIndex + 1; i < getCount(); ++i) {
		controlUC* p = getItemAt(i);
		pListItem = static_cast<IListItemUC*>(p->getInterface(L"IListItem"));
		if( pListItem != NULL ) {
			pListItem->setIndex(i);
		}
	}
	if( m_iCurSel >= iIndex ) m_iCurSel += 1;
	return true;
}
bool comboUC::remove(controlUC* pControl)
{
	int iIndex = getItemIndex(pControl);
	if (iIndex == -1) return false;

	if (!containerUC::removeAt(iIndex)) return false;

	for(int i = iIndex; i < getCount(); ++i) {
		controlUC* p = getItemAt(i);
		IListItemUC* pListItem = static_cast<IListItemUC*>(p->getInterface(L"IListItem"));
		if( pListItem != NULL ) {
			pListItem->setIndex(i);
		}
	}

	if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
		int iSel = m_iCurSel;
		m_iCurSel = -1;
		selectItem(findSelectable(iSel, false));
	}
	else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
	return true;
}
bool comboUC::removeAt(int iIndex)
{
	if (!containerUC::removeAt(iIndex)) return false;

	for(int i = iIndex; i < getCount(); ++i) {
		controlUC* p = getItemAt(i);
		IListItemUC* pListItem = static_cast<IListItemUC*>(p->getInterface(L"IListItem"));
		if( pListItem != NULL ) pListItem->setIndex(i);
	}

	if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
		int iSel = m_iCurSel;
		m_iCurSel = -1;
		selectItem(findSelectable(iSel, false));
	}
	else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
	return true;
}
void comboUC::removeAll()
{
	m_iCurSel = -1;
	containerUC::removeAll();
}

bool comboUC::activate()
{
	if( !controlUC::activate() ) return false;
	if( m_pWindow ) return true;
	m_pWindow = new comboWnd();
	assert(m_pWindow);
	m_pWindow->init(this);
	if( m_pManager != NULL ) m_pManager->sendNotify(this, L"dropdown");
	invalidate();
	return true;
}

RECT comboUC::getTextPadding() const
{
	return m_rcTextPadding;
}
void comboUC::setTextPadding(RECT rc)
{
	m_rcTextPadding = rc;
	invalidate();
}
bool comboUC::getNormalImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_NORMAL, &rc);
}

void comboUC::setNormalImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_NORMAL, rcImgCell);
	invalidate();
}
void comboUC::setNormalImage(LONG left, LONG top, LONG right, LONG bottom)
{
	assert(m_pImage);
	RECT rc = {left, top, right, bottom};
	setNormalImage(rc);
}
bool comboUC::getHotImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_HOT, &rc);
}

void comboUC::setHotImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_HOT, rcImgCell);
	invalidate();
}
void comboUC::setHotImage(LONG left, LONG top, LONG right, LONG bottom)
{
	assert(m_pImage);
	RECT rc = {left, top, right, bottom};
	setHotImage(rc);
}
bool comboUC::getPushedImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_PUSHED, &rc);
}

void comboUC::setPushedImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_PUSHED, rcImgCell);
	invalidate();
}
void comboUC::setPushedImage(LONG left, LONG top, LONG right, LONG bottom)
{
	assert(m_pImage);
	RECT rc = {left, top, right, bottom};
	setPushedImage(rc);
}
bool comboUC::getFocusedImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_FOCUSED, &rc);
}

void comboUC::setFocusedImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_FOCUSED, rcImgCell);
	invalidate();
}
void comboUC::setFocusedImage(LONG left, LONG top, LONG right, LONG bottom)
{
	assert(m_pImage);
	RECT rc = {left, top, right, bottom};
	setFocusedImage(rc);
}
bool comboUC::getDisabledImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_DISABLED, &rc);
}

void comboUC::setDisabledImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_DISABLED, rcImgCell);
	invalidate();
}
void comboUC::setDisabledImage(LONG left, LONG top, LONG right, LONG bottom)
{
	assert(m_pImage);
	RECT rc = {left, top, right, bottom};
	setDisabledImage(rc);
}

int comboUC::getColumns() const
{
	return m_listInfo.iColumns;
}
int comboUC::getItemFont() const
{
	return m_listInfo.iFont;
}
void comboUC::setItemFont(int iFont)
{
	if(m_listInfo.iFont == iFont) return;
	m_listInfo.iFont = iFont;
	needUpdate();
}
UINT comboUC::getItemTextStyle() const
{
	return m_listInfo.uTextStyle;
}
void comboUC::setItemTextStyle(UINT style)
{
	if(m_listInfo.uTextStyle == style) return;
	m_listInfo.uTextStyle = style;
	needUpdate();
}
RECT comboUC::getItemTextPadding() const
{
	return m_listInfo.rcTextPadding;
}
void comboUC::setItemTextPadding(RECT rcPadding)
{
	if(m_listInfo.rcTextPadding == rcPadding) return;
	m_listInfo.rcTextPadding = rcPadding;
	needUpdate();
}
DWORD comboUC::getItemTextColor() const
{
	return m_listInfo.clrText;
}
void comboUC::setItemTextColor(DWORD clrText)
{
	if(m_listInfo.clrText == clrText) return;
	m_listInfo.clrText = clrText;
	invalidate();
}

DWORD comboUC::getItemBkColor() const
{
	return m_listInfo.clrBk;
}
void comboUC::setItemBkColor(DWORD clrBk)
{
	if(m_listInfo.clrBk == clrBk) return;
	m_listInfo.clrBk = clrBk;
	invalidate();
}

DWORD comboUC::getItemSelectedTextColor() const
{
	return m_listInfo.clrSelectedText;
}
void comboUC::setItemSelectedTextColor(DWORD clrText)
{
	if(m_listInfo.clrSelectedText == clrText) return;
	m_listInfo.clrSelectedText = clrText;
	invalidate();
}
DWORD comboUC::getItemSelectedBkColor() const
{
	return m_listInfo.clrSelectedBk;
}
void comboUC::setItemSelectedBkColor(DWORD clrBk)
{
	if(m_listInfo.clrSelectedBk == clrBk) return;
	m_listInfo.clrSelectedBk = clrBk;
	invalidate();
}
DWORD comboUC::getItemHotTextColor() const
{
	return m_listInfo.clrHotText;
}
void comboUC::setItemHotTextColor(DWORD clrText)
{
	if(m_listInfo.clrHotText == clrText) return;
	m_listInfo.clrHotText = clrText;
	invalidate();
}
DWORD comboUC::getItemHotBkColor() const
{
	return m_listInfo.clrHotBk;
}
void comboUC::setItemHotBkColor(DWORD clrBk)
{
	if(m_listInfo.clrHotBk == clrBk) return;
	m_listInfo.clrHotBk = clrBk;
	invalidate();
}
DWORD comboUC::getItemCutTextColor() const
{
	return m_listInfo.clrCutedText;
}
void comboUC::setItemCutTextColor(DWORD clrText)
{
	if(m_listInfo.clrCutedText == clrText) return;
	m_listInfo.clrCutedText = clrText;
	invalidate();
}
DWORD comboUC::getItemCutBkColor() const
{
	return m_listInfo.clrCutedBk;
}
void comboUC::setItemCutBkColor(DWORD clrBk)
{
	if(m_listInfo.clrCutedBk == clrBk) return;
	m_listInfo.clrCutedBk = clrBk;
	invalidate();
}
DWORD comboUC::getItemDisabledTextColor() const
{
	return m_listInfo.clrDisabledText;
}
void comboUC::setItemDisabledTextColor(DWORD clrText)
{
	if(m_listInfo.clrDisabledText == clrText) return;
	m_listInfo.clrDisabledText = clrText;
	invalidate();
}
DWORD comboUC::getItemDisabledBkColor() const
{
	return m_listInfo.clrDisabledBk;
}
void comboUC::setItemDisabledBkColor(DWORD clrBk)
{
	if(m_listInfo.clrDisabledBk == clrBk) return;
	m_listInfo.clrDisabledBk = clrBk;
	invalidate();
}
DWORD comboUC::getItemLineColor() const
{
	return m_listInfo.clrLine;
}
void comboUC::setItemLineColor(DWORD clrLine)
{
	if(m_listInfo.clrLine == clrLine) return;
	m_listInfo.clrLine = clrLine;
	invalidate();
}

bool comboUC::isItemShowHtml() const
{
	return m_listInfo.bShowHtml;
}
void comboUC::setItemShowHtml(bool bShowHtml /*= true*/)
{
	if(m_listInfo.bShowHtml == bShowHtml) return;
	m_listInfo.bShowHtml = bShowHtml;
	needUpdate();
}
bool comboUC::isAlternateBk() const
{
	return m_listInfo.bAlternateBk;
}
void comboUC::setAlternateBk(bool bAlternateBk)
{
	if(m_listInfo.bAlternateBk == bAlternateBk) return;
	m_listInfo.bAlternateBk = bAlternateBk;
	needUpdate();
}
gtc::image* comboUC::getItemImage() const
{
	return m_listInfo.pImage;
}
void comboUC::setItemImage(PCWSTR pwsImage)
{
	gtc::image *pimg = controlUC::analyzeImage(pwsImage);// drawManager::getResource().findImage(pwsImage);
	if(pimg == m_listInfo.pImage) return;
	m_listInfo.pImage = pimg;
	invalidate();
}
void comboUC::setItemNormalImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_listInfo.mapImage, UCIMAGE_STATE_NORMAL, rcImgCell, m_listInfo.pImage);
	invalidate();
}
bool comboUC::getItemNormalImage(RECT &rc) const
{
	return controlUC::existsImage(m_listInfo.mapImage, UCIMAGE_STATE_NORMAL, &rc);
}
bool comboUC::getItemSelectedImage(RECT &rc) const
{
	return controlUC::existsImage(m_listInfo.mapImage, UCIMAGE_STATE_SELECTED, &rc);
}
void comboUC::setItemSelectedImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_listInfo.mapImage, UCIMAGE_STATE_SELECTED, rcImgCell, m_listInfo.pImage);
	invalidate();
}
bool comboUC::getItemCutedImage(RECT &rc) const
{
	return controlUC::existsImage(m_listInfo.mapImage, UCIMAGE_STATE_CUTED, &rc);
}
void comboUC::setItemCutedImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_listInfo.mapImage, UCIMAGE_STATE_CUTED, rcImgCell, m_listInfo.pImage);
	invalidate();
}
bool comboUC::getItemHotImage(RECT &rc) const
{
	return controlUC::existsImage(m_listInfo.mapImage, UCIMAGE_STATE_HOT, &rc);
}
void comboUC::setItemHotImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_listInfo.mapImage, UCIMAGE_STATE_HOT, rcImgCell, m_listInfo.pImage);
	invalidate();
}
bool comboUC::getItemDisabledImage(RECT &rc) const
{
	return controlUC::existsImage(m_listInfo.mapImage, UCIMAGE_STATE_DISABLED, &rc);
}
void comboUC::setItemDisabledImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_listInfo.mapImage, UCIMAGE_STATE_DISABLED, rcImgCell, m_listInfo.pImage);
	invalidate();
}

}	// wuc
