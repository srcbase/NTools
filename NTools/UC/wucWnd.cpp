#include "wucWnd.h"

namespace wuc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	wucWnd  类		2015-3-20
//	对立于任何wuc控件的基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
wucWnd::wucWnd()
	: m_pOwner(nullptr)
{
}
wucWnd::~wucWnd()
{

}
LRESULT wucWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	bool bDone = false;
	//if(!bDone && !wuc::windowWnd::isOutMsg(uMsg))
	//	SEND_LOG(L"wucWnd::handleMessage msg:%s", wuc::windowWnd::stringMsg(uMsg).c_str() );

	switch(uMsg)
	{
	//case WM_CREATE:
	case WM_SHOWWINDOW:
		{
			if(wParam == TRUE && m_pOwner &&m_pOwner->getCallback()) m_pOwner->getCallback()->onxInit(this);
		}
		break;
	case WM_SETFOCUS:
		{
			if(m_pOwner && m_pOwner->getCallback()) m_pOwner->getCallback()->onxSetFocus(this);
		}
		break;
	case WM_KILLFOCUS:
		{
			if(m_pOwner && m_pOwner->getCallback()) {
				bool bExit = true;
				m_pOwner->getCallback()->onxKillFocus(this, bExit);
				if(bExit) {
					lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
					bDone = true;
					postMessage(WM_CLOSE);
				}
			}
			else
			{
				bDone = true;
				postMessage(WM_CLOSE);
			}
		}
		break;
	case WM_DESTROY:
		{
			if(m_pOwner && m_pOwner->getCallback()) {
				bool bExit = true;
				m_pOwner->getCallback()->onxExit(this, bExit);
				if(!bExit) bDone = true;
			}
			else bDone = true;
		}
		break;
	}

	//if(!bDone && !wuc::windowWnd::isOutMsg(uMsg))
	//	SEND_LOG(L"wucWnd::handleMessage bdone:%d msg:%s", bDone, wuc::windowWnd::stringMsg(uMsg).c_str() );


	if(bDone) return lRes;
	return windowWnd::handleMessage(uMsg, wParam, lParam);
}
void wucWnd::onFinalMessage(HWND hWnd) 
{
	if(m_pOwner && m_pOwner->getCallback()) m_pOwner->getCallback()->onxFinal(this);
}
bool wucWnd::modifyStyle(DWORD dwRemove, DWORD dwAdd)
{
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if(dwStyle == dwNewStyle)
		return FALSE;

	return ::SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle) != 0;
}
bool wucWnd::create(IWndX *pOwner, HWND hParent, RECT rcPos, DWORD dwStyle, DWORD dwExStyle /*= 0*/)
{
	assert(pOwner);
	if(!pOwner) return false;
	if((dwStyle & WS_CHILD) == WS_CHILD) 
	{
		assert(hParent);
		if(!hParent) return false;
	}
	m_pOwner = pOwner;
	bool bRet = __super::create(hParent, NULL, dwStyle, dwExStyle, rcPos) != NULL;
	//if(bRet) sendMessage(WM_SETFOCUS);
	//setCapture();	// 不能用哦，否则要2次点击才能失去焦点
	return bRet;
}

IWndX* wucWnd::getOwner() const
{
	return m_pOwner;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	editWnd类		2015-2-11
//	WC_EDIT控件的子类化
////////////////////////////////////////////////////////////////////////////////////////////////////////
class editWnd : public wucWnd
{
public:
	editWnd();
	~editWnd();
	void init(editUC* pOwner);
	RECT calPos();

	virtual PCWSTR getWindowClassName() const;
	virtual PCWSTR getSuperClassName() const;
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void onFinalMessage(HWND hWnd);
public:
	//LRESULT onKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT onEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
	editUC* m_pEdit;
	HBRUSH m_hBkBrush;
};


editWnd::editWnd() : m_pEdit(NULL), m_hBkBrush(NULL)
{
	SEND_LOG(L"editWnd 创建 ......");
}
editWnd::~editWnd()
{
	SEND_LOG(L"editWnd 释放 ......");

}
void editWnd::init(editUC* pOwner)
{
	m_pEdit = pOwner;
	assert(m_pEdit);
	//assert(m_pEdit->getCallback());	// 如果不需要对输入字符进行控制的话可以不实现IEditWndXCallback接口

	RECT rcPos = calPos();
	UINT txtStyle = m_pEdit->getTextStyle();
	//UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;
	UINT uStyle = WS_POPUP | ES_AUTOHSCROLL | WS_VISIBLE;
	RECT rcWnd={0};
	//::GetWindowRect(m_pEdit->getManager()->getPaintWindow(), &rcWnd);
	//rcPos.left += rcWnd.left;
	//rcPos.right += rcWnd.left;
	//rcPos.top += rcWnd.top;
	//rcPos.bottom += rcWnd.top;
	//::GetClientRect(m_pEdit->getManager()->getPaintWindow(), &rcWnd);
	POINT pt = {0};
	::ClientToScreen(m_pEdit->getManager()->getPaintWindow(), &pt);
	::OffsetRect(&rcPos, pt.x+2, pt.y);
	if(/*!m_pEdit->getImage() && */m_pEdit->supportBorder())
		uStyle |= WS_BORDER;
	if(txtStyle & DT_CENTER) uStyle |= ES_CENTER;
	else if(txtStyle & DT_LEFT) uStyle |= ES_LEFT;
	else if(txtStyle & DT_RIGHT) uStyle |= ES_RIGHT;
	if( m_pEdit->isPasswordMode() ) uStyle |= ES_PASSWORD;
	if(m_pEdit->isMultiLine()) uStyle |= ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL;
	else uStyle |= ES_AUTOHSCROLL;

	//if(!m_pOwner->getImage()) uStyle |= WS_BORDER;
	create(m_pEdit, /*m_pEdit->getManager()->isLayerWindowTransparent() ? NULL : */m_pEdit->getManager()->getPaintWindow(), rcPos, uStyle/*, m_pEdit->getManager()->isLayerWindowTransparent()?WS_EX_TOOLWINDOW:0*/);
	//HWND hParent = m_pOwner->getManager()->getPaintWindow();
	//m_hWnd = ::CreateWindowEx(0, L"EDIT", nullptr, uStyle, rcPos.left, rcPos.top, rectWidth(rcPos), rectHeight(rcPos)
	//	, hParent, NULL, (HINSTANCE)GetWindowLongPtr(hParent,GWLP_HINSTANCE), this);
	//create(NULL, NULL, uStyle, 0, rcPos);
	SetWindowFont(m_hWnd, m_pEdit->getManager()->getFont(m_pEdit->getFont()), TRUE);
	Edit_LimitText(m_hWnd, m_pEdit->getMaxChar());
	if( m_pEdit->isPasswordMode() ) Edit_SetPasswordChar(m_hWnd, m_pEdit->getPasswordChar());
	Edit_SetText(m_hWnd, m_pEdit->getText());
	Edit_SetModify(m_hWnd, FALSE);
	sendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
	Edit_Enable(m_hWnd, m_pEdit->isEnabled() == true);
	Edit_SetReadOnly(m_hWnd, m_pEdit->isReadOnly() == true);

	//modifyStyle(WS_POPUP, WS_CHILD);
	//SetParent(*this, m_pEdit->getManager()->getPaintWindow());
	//this->modifyStyle(WS_CHILD, WS_POPUP);
	//SetParent(*this, m_pEdit->getManager()->getPaintWindow());
	//m_pEdit->getManager()->clientToScreen(rcPos);
	//SetWindowPos(*this, m_pEdit->getManager()->getPaintWindow(),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	//Styls
	//LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
	//styleValue |= pOwner->GetWindowStyls();
	//::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	::SetFocus(m_hWnd);
	//::SetCapture(m_hWnd);
}

RECT editWnd::calPos()
{
	RECT rcPos = m_pEdit->getPosition();
	RECT rcInset = m_pEdit->getTextPadding();
	rcPos.left += rcInset.left;
	rcPos.top += rcInset.top;
	rcPos.right -= rcInset.right;
	rcPos.bottom -= rcInset.bottom;
	if (m_pEdit && !m_pEdit->isMultiLine())
	{
		LONG lEditHeight = m_pEdit->getManager()->findFont(m_pEdit->getFont())->tm.tmHeight/*+4*/;
		if (m_pEdit->supportBorder())
		{
			lEditHeight += 4;
		}
		//LONG lEditHeight = m_pEdit->getManager()->getDefaultFontInfo()->tm.tmHeight+4;
		if( lEditHeight < rectHeight(rcPos) /*rcPos.GetHeight()*/ ) {
			rcPos.top += (rectHeight(rcPos) /*rcPos.GetHeight()*/ - lEditHeight) / 2;
			rcPos.bottom = rcPos.top + lEditHeight;
		}
	}
	return rcPos;
}

PCWSTR editWnd::getWindowClassName() const
{
	return L"editWnd";
}
PCWSTR editWnd::getSuperClassName() const
{
	return WC_EDIT;
}


LRESULT editWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	bool bHandled = false;

	/*if( uMsg == WM_KILLFOCUS )
	{
	lRes = onKillFocus(uMsg, wParam, lParam, bHandled);
	}
	else */if( uMsg == OCM_COMMAND ) {
		bHandled = true;
		if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) {
			lRes = onEditChanged(uMsg, wParam, lParam, bHandled);
		}
		else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) {
			RECT rcClient;
			::GetClientRect(m_hWnd, &rcClient);
			//SEND_LOG(L"NOTE editWnd::handleMessage 测试刷新区域 vvvvvvvvv");
			::InvalidateRect(m_hWnd, &rcClient, FALSE);
		}
	}
	else if (uMsg == WM_CHAR)
	{
		IEditWndXCallback *pEdit = static_cast<IEditWndXCallback *>(m_pEdit->getCallback());
		if (pEdit)
		{
			//if(m_pEdit->getManager()->isLayerWindowTransparent())
			//{
			//	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
			//	wchar_t *pstr = new wchar_t[cchLen];

			//	//PWSTR pstr = static_cast<PWSTR>(_alloca(cchLen * sizeof(wchar_t)));
			//	assert(pstr);
			//	if( pstr == NULL ) return 0;
			//	::GetWindowText(m_hWnd, pstr, cchLen);
			//	m_pEdit->m_wsText = pstr;

			//	int nChar = wParam;
			//	int iSelBegin =  LOWORD(Edit_GetSel(m_hWnd));
			//	int iSels = HIWORD(Edit_GetSel(m_hWnd));
			//	if (!std::iscntrl(nChar) || !std::iswcntrl(nChar))
			//	{
			//		m_pEdit->m_wsText.insert(m_pEdit->m_wsText.begin()+iSelBegin, nChar);
			//	}
			//	else
			//	{
			//		if (nChar == VK_BACK)
			//		{
			//			if(--iSelBegin < 0) iSelBegin = 0;
			//			m_pEdit->m_wsText.erase(iSelBegin,iSels);
			//		}
			//		else if (nChar == VK_DELETE)
			//		{
			//			m_pEdit->m_wsText.erase(iSelBegin,iSels);
			//		}
			//	}
			//	delete pstr;

			//}
			pEdit->onxChar(this, (UINT)wParam, (UINT)lParam, 1, bHandled);
		}
	}
	else if( uMsg == WM_KEYDOWN ) {
		if (wchar_t(wParam) == VK_RETURN)
		{
			if (m_pEdit->getCallback())
			{
				IEditWndXCallback *pEdit = static_cast<IEditWndXCallback *>(m_pEdit->getCallback());
				if (pEdit)
				{
					bool bExit = true;
					pEdit->onxEditReturn(this, bExit);
					if (bExit)
					{
						postMessage(WM_CLOSE);
						return 0;
					}
				}
			}
			bHandled = true;
			m_pEdit->getManager()->sendNotify(m_pEdit, L"return");
		}
	}
	else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) {
		bHandled = true;
		if( m_pEdit->getNativeEditBkColor() == 0xFEFFFFFF )
		{
			if( m_hBkBrush == NULL ) {
				m_hBkBrush = ::CreateSolidBrush(RGB(0x0FF, 0x0FF, 0x0FF));
			}
			return (LRESULT)m_hBkBrush;
		}
		::SetBkMode((HDC)wParam, TRANSPARENT);
		DWORD dwTextColor = m_pEdit->getTextColor();
		DWORD clrColor = m_pEdit->getNativeEditBkColor();
		::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
		if( m_hBkBrush == NULL ) {
			m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
		}
		return (LRESULT)m_hBkBrush;
	}
	else bHandled = false;

	//if(!wuc::windowWnd::isOutMsg(uMsg))
	//	SEND_LOG(L"editWnd::handleMessage wnd:%08X msg:%s", m_hWnd, wuc::windowWnd::stringMsg(uMsg).c_str() );
	//bHandled = false;
	if( bHandled ) return lRes;
	return __super::handleMessage(uMsg, wParam, lParam);
}

void editWnd::onFinalMessage(HWND hWnd)
{
	// Clear reference and die
	SEND_LOG(L"editWnd::onFinalMessage hWnd:%08X", hWnd);
	if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
	m_pEdit->m_pWindow = NULL;
	m_pEdit->setFocus(false);
	if(m_pEdit->getCallback()) m_pEdit->getCallback()->onxFinal(this);
	m_pEdit->invalidate();
	delete this;
}

//LRESULT editWnd::onKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
//{
//	LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
//	if(m_pEdit->getCallback())
//	{
//		bool bExit = false;
//		m_pEdit->getCallback()->onxKillFocus(this, bExit);
//		if (bExit)
//		{
//			postMessage(WM_CLOSE);
//		}
//	}
//	return lRes;
//}

LRESULT editWnd::onEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, bool& /*bHandled*/)
{
	SEND_LOG(L"editWnd::onEditChanged begin");
	if( m_pOwner == NULL ) return 0;
	// Copy text back
	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
	PWSTR pstr = static_cast<PWSTR>(_alloca(cchLen * sizeof(wchar_t)));
	assert(pstr);
	if( pstr == NULL ) return 0;
	::GetWindowText(m_hWnd, pstr, cchLen);
	m_pEdit->m_wsText = pstr;
	m_pEdit->getManager()->sendNotify(m_pEdit, L"textchanged");
	if(m_pEdit->getCallback())
	{
		IEditWndXCallback *pEdit = static_cast<IEditWndXCallback *>(m_pEdit->getCallback());
		if (pEdit)
		{
			pEdit->onxEditChanged(this);
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	editUC类		2015-2-11
//	普通文本编辑控件
////////////////////////////////////////////////////////////////////////////////////////////////////////
editUC::editUC()
	: m_pWindow(NULL), m_pOwner(NULL)
	, m_uMaxChar(255), m_bReadOnly(false)
	, m_bBorder(false)
	, m_bPasswordMode(false), m_cPasswordChar(L'*'), m_bMultiLine(false)
	, m_uButtonState(0)
	, m_dwEditbkColor(0xFEFFFFFF)
	, m_dwPromptTextColor(0xFECECECE)
{
	SEND_LOG(L"editUC init ...");
}
editUC::~editUC()
{
	SEND_LOG(L"editUC free ...");
}
PCWSTR editUC::getClass() const
{
	return L"editUC";
}
LPVOID editUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"Edit") == 0 ) return static_cast<editUC*>(this);
	if( _tcscmp(pstrName, L"IWnd") == 0 ) return static_cast<IWndX*>(this);
	return __super::getInterface(pstrName);
}
UINT editUC::getControlFlags() const
{
	if(!isEnabled()) return controlUC::getControlFlags();
	return UCFLAG_SETCURSOR | UCFLAG_TABSTOP;
}

void editUC::setEnabled(bool bEnable /*= true*/)
{
	controlUC::setEnabled(bEnable);
	if( !isEnabled() ) {
		m_uButtonState = 0;
	}
}
void editUC::setText(PCWSTR pstrText)
{
	m_wsText = pstrText ? pstrText : L"";
	//__try
	//{

	if( m_pWindow != NULL ) Edit_SetText(*m_pWindow, m_wsText.c_str());
	invalidate();
	//}
	//__except(gtc::app::sehFilterDefault(GetExceptionInformation(), L"editUC::setText"))
	//{
	//	
	//	SEND_LOG(L"editUC::setText SEH异常 name:<%u>", GetLastError());
	//}
}

void editUC::setPosition(RECT pos)
{
	controlUC::setPosition(pos);
	if( m_pWindow != NULL ) {
		RECT rcPos = m_pWindow->calPos();
		::SetWindowPos(m_pWindow->getHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
			rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);        
	}
}
void editUC::setVisible(bool bVisible /*= true*/)
{
	controlUC::setVisible(bVisible);
	if( !isVisible() && m_pWindow != NULL ) m_pManager->setFocus(NULL);
}
void editUC::setInternVisible(bool bVisible /*= true*/)
{
	if( !isVisible() && m_pWindow != NULL ) m_pManager->setFocus(NULL);
}

SIZE editUC::estimateSize(SIZE szAvailable)
{
	if( m_szxyFixed.cy == 0 )
	{
		SIZE sz = {m_szxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 6};
		return sz;
	}
	return controlUC::estimateSize(szAvailable);
}
void editUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else labelUC::Event(event);
		return;
	}
	if( event.Type == BEUCEventSetCursor && isEnabled() )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
		//SEND_LOG(L"editUC::Event  BEUCEventSetCursor");
		return;
	}
	if( event.Type == BEUCEventWindowSize )
	{
		if( m_pWindow != NULL ) m_pManager->setFocusNeeded(this);
	}
	if( event.Type == BEUCEventScrollWheel )
	{
		if( m_pWindow != NULL ) return;
	}
	if( event.Type == BEUCEventSetFocus && isEnabled() ) 
	{
		if( m_pWindow ) return;
		m_pWindow = new editWnd();
		assert(m_pWindow);
		//setFocus(true);
		//getManager()->releaseCapture();
		m_pWindow->init(this);
		invalidate();
	}
	if( event.Type == BEUCEventKillFocus && isEnabled() ) 
	{
		//setFocus(false);
		//if(m_pWindow)
		//{
		//	m_pWindow->sendMessage(WM_CLOSE);
		//}
		invalidate();
	}
	if( event.Type == BEUCEventButtonDown || event.Type == BEUCEventDBLClick || event.Type == BEUCEventRButtonDown) 
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
					SEND_LOG(L"editUC::Event BEUCEventButtonDown range:0-%d", nSize);
				}
			}
			else if( m_pWindow != NULL )
			{
#if 1
				int nSize = GetWindowTextLength(*m_pWindow);
				if( nSize == 0 )
					nSize = 1;
				Edit_SetSel(*m_pWindow, 0, nSize);
				SEND_LOG(L"editUC::Event BEUCEventButtonDown 2 range:0-%d", nSize);
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
	if( event.Type == BEUCEventMouseMove ) 
	{
		return;
	}
	if( event.Type == BEUCEventButtonUp ) 
	{
		return;
	}
	if( event.Type == BEUCEventContextMenu )
	{
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
	labelUC::Event(event);
}
void editUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = NULL;
	if( _tcscmp(pstrName, L"readonly") == 0 ) setReadOnly(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"password") == 0 ) setPasswordMode(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"multiline") == 0 ) setMultiLine(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"maxchar") == 0 ) setMaxChar(_ttoi(pstrValue));
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
	else if( _tcscmp(pstrName, L"prompttextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setPromptTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"nativebkcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setNativeEditBkColor(clrColor);
	}
	else __super::setAttribute(pstrName, pstrValue);
}

void editUC::paintStatusImage(HDC hdc)
{
	if(!hdc) return;
	//if(m_pManager && m_pWindow) return;
	if( isFocused() ) m_uButtonState |= UCSTATE_FOCUSED;
	else m_uButtonState &= ~ UCSTATE_FOCUSED;
	if( !isEnabled() ) m_uButtonState |= UCSTATE_DISABLED;
	else m_uButtonState &= ~ UCSTATE_DISABLED;

	RECT rcSrc = {0};
	Gdiplus::Graphics grap(hdc);
	if( (m_uButtonState & UCSTATE_DISABLED) != 0 ) {
		if (getDisabledImage(rcSrc))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc)) return;
		}
		//if( !m_sDisabledImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
		//	else return;
		//}
	}
	else if( (m_uButtonState & UCSTATE_FOCUSED) != 0 ) {
		if (getFocusedImage(rcSrc))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc)) return;
		}
		//if( !m_sFocusedImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
		//	else return;
		//}
	}
	else if( (m_uButtonState & UCSTATE_HOT) != 0 ) {
		if (getHotImage(rcSrc))
		{
			if(gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc)) return;
		}
		//if( !m_sHotImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) m_sHotImage.Empty();
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
void editUC::paintText(HDC hdc)
{
	if(!hdc) return;
	if(m_pWindow && ::IsWindowVisible(*m_pWindow))
	{
		return;
	}
	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->getDefaultFontColor();
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->getDefaultDisabledColor();

	if( m_wsText.empty() && m_wsPromptInfo.empty() ) return;
	Gdiplus::Graphics grap(hdc);
	DWORD clrText = m_dwTextColor;
	std::wstring wsText = m_wsText;
	if( m_bPasswordMode ) {
		wsText.clear();
		PCWSTR p = m_wsText.data();
		while( *p != L'\0' ) {
			wsText += m_cPasswordChar;
			p = ::CharNext(p);
		}
	}
	if(wsText.empty()) 
	{
		wsText = m_wsPromptInfo;
		clrText = m_dwPromptTextColor;
		
	}
	RECT rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;
	if( isEnabled() ) {
		gtc::drawer::drawString(grap, wsText.c_str(), rc, m_pManager->getResource().getFont(m_iFont), (BEAlignMode)(DT_SINGLELINE | m_uiTextStyle), clrText);
		//CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwTextColor, \
		//	m_iFont, DT_SINGLELINE | m_uiTextStyle);
	}
	else {
		gtc::drawer::drawString(grap, wsText.c_str(), rc, m_pManager->getResource().getFont(m_iFont), (BEAlignMode)(DT_SINGLELINE | m_uiTextStyle), m_dwDisabledTextColor);
		//CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, \
		//	m_iFont, DT_SINGLELINE | m_uTextStyle);
	}
}

//void editUC::setOwner(IEditWndXCallback *pOwner)
//{
//	assert(pOwner);
//	m_pOwner = pOwner;
//}
//IEditWndXCallback* editUC::getOwner() const
//{
//	return m_pOwner;
//}
void editUC::setMaxChar(UINT uMax)
{
	m_uMaxChar = uMax;
	if( m_pWindow != NULL ) Edit_LimitText(*m_pWindow, m_uMaxChar);
}
UINT editUC::getMaxChar()
{
	return m_uMaxChar;
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
	if( m_cPasswordChar == cPasswordChar ) return;
	m_cPasswordChar = cPasswordChar;
	if( m_pWindow != NULL ) Edit_SetPasswordChar(*m_pWindow, m_cPasswordChar);
	invalidate();
}
wchar_t editUC::getPasswordChar() const
{
	return m_cPasswordChar;
}

void editUC::setMultiLine(bool bMultiLine)
{
	if(m_bMultiLine == bMultiLine) return;
	m_bMultiLine = bMultiLine;
	if(m_pWindow) Edit_FmtLines(*m_pWindow, m_bMultiLine);
	invalidate();
}

bool editUC::isMultiLine() const
{
	return m_bMultiLine;
}

void editUC::setBorder(bool bBorder)
{
	if(m_bBorder == bBorder) return;
	m_bBorder = bBorder;
	if(m_pWindow && !m_pImage)
	{
		LONG_PTR style = GetWindowLongPtr(*m_pWindow, GWL_STYLE);
		if(m_bBorder) style |= WS_BORDER;
		else style &= ~WS_BORDER;
		SetWindowLongPtr(*m_pWindow, GWL_STYLE, style);
	}
	invalidate();
}
bool editUC::supportBorder() const
{
	return m_bBorder;
}
bool editUC::getNormalImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_NORMAL, &rc);
}

void editUC::setNormalImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_NORMAL, rcImgCell);
	invalidate();
}

bool editUC::getHotImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_HOT, &rc);
}

void editUC::setHotImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_HOT, rcImgCell);
	invalidate();
}

bool editUC::getFocusedImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_FOCUSED, &rc);
}

void editUC::setFocusedImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_FOCUSED, rcImgCell);
	invalidate();
}

bool editUC::getDisabledImage(RECT &rc)
{
	return existsImage(UCIMAGE_STATE_DISABLED, &rc);
}

void editUC::setDisabledImage(RECT rcImgCell)
{
	setCellImage(UCIMAGE_STATE_DISABLED, rcImgCell);
	invalidate();
}

void editUC::setNativeEditBkColor(DWORD dwBkColor)
{
	m_dwEditbkColor = dwBkColor;
}
DWORD editUC::getNativeEditBkColor() const
{
	return m_dwEditbkColor;
}
void editUC::setPromptInfo(PCWSTR pPrompt)
{
	if(pPrompt && m_wsPromptInfo == pPrompt) return;
	if(!pPrompt && m_wsPromptInfo.empty()) return;
	m_wsPromptInfo = pPrompt ? pPrompt : L"";
}
PCWSTR editUC::getPromptInfo() const
{
	return m_wsPromptInfo.c_str();
}
void editUC::setPromptTextColor(DWORD dwColor)
{
	if(m_dwPromptTextColor == dwColor) return;
	m_dwPromptTextColor = dwColor;
	invalidate();
}
DWORD editUC::getPromptTextColor() const
{
	return m_dwPromptTextColor;
}
IWndXCallback* editUC::getCallback() const
{
	return m_pOwner;
}
void editUC::setCallback(IWndXCallback *pCallback)
{
	IEditWndXCallback *pEdit = static_cast<IEditWndXCallback *>(pCallback);
	assert(pEdit);
	m_pOwner = pEdit;
}
// 得到子类化控件（即：本接口的实现者）
controlUC* editUC::getSelf()
{
	return this;
}
void editUC::setSelRange(int iStart, int iEnd)
{
	SEND_LOG(L"editUC::setSelRange range1:%d-%d", iStart, iEnd);
	if(!m_pWindow) return;
	int nSize = GetWindowTextLength(*m_pWindow);
	if(iStart < 0) iStart = 0;
	if(iEnd <= 0 || iEnd > nSize) iEnd = nSize;
	if(iStart > iEnd) return;

	SEND_LOG(L"editUC::setSelRange range2:%d-%d", iStart, iEnd);
	Edit_SetSel(*m_pWindow, iStart, iEnd);
}

}	// end wuc