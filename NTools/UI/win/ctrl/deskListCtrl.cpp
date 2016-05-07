#include "deskListCtrl.h"
#include "AC/sysupport.h"
#include "KP/kpdeskListView.h"
namespace gtc {
inline double calculateDelay(double state) {
	return pow(state, 2);
}
deskListCtrl::deskListCtrl() 
	: m_pmapNodes(nullptr), m_hWnd(nullptr)
	, m_uButtonState(0), m_dwDelayDeltaY(0), m_dwDelayNum(0), m_dwDelayLeft(0)
{
	//for(int i = 0; i < 500; ++i) 
	//{
	//	wui::dialogBuilder builder;
	//	wui::containerUI* pDesk = static_cast<wui::containerUI*>(builder.create(L"kpsk\\deskctrl.xml", (UINT)0));
	//	if( pDesk ) {
	//		this->add(pDesk);
	//		//TCHAR indexBuffer[16];
	//		//CStdString strIndexString = _T("- ");
	//		//strIndexString += _itot(i+1, indexBuffer, 10);
	//		//strIndexString += _T(" -");
	//		//pDesk->GetItemAt(3)->SetText(strIndexString);
	//	}
	//	else {
	//		this->removeAll();
	//		return;
	//	}
	//}
}

void deskListCtrl::doEvent(wui::TEventUI& event) 
{
	if( !isMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		SEND_LOG(L"mm %d", event.Type);
		if( m_pParent != NULL ) m_pParent->doEvent(event);
		else wui::tileLayoutUI::doEvent(event);
		return;
	}
	if( event.Type == UIEVENT_TIMER && event.wParam == SCROLL_TIMERID )
	{
		SEND_LOG(L"UIEVENT_TIMER %d", event.Type);

		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			POINT pt = m_pManager->getMousePos();
			LONG cy = (pt.y - m_ptLastMouse.y);
			m_ptLastMouse = pt;
			SIZE sz = getScrollPos();
			sz.cy -= cy;
			setScrollPos(sz);
			return;
		}
		else if( m_dwDelayLeft > 0 ) {
			--m_dwDelayLeft;
			SIZE sz = getScrollPos();
			LONG lDeltaY =  (LONG)(calculateDelay((double)m_dwDelayLeft / m_dwDelayNum) * m_dwDelayDeltaY);
			if( (lDeltaY > 0 && sz.cy != 0)  || (lDeltaY < 0 && sz.cy != getScrollRange().cy ) ) {
				sz.cy -= lDeltaY;
				setScrollPos(sz);
				return;
			}
		}
		
		m_dwDelayDeltaY = 0;
		m_dwDelayNum = 0;
		m_dwDelayLeft = 0;
		m_pManager->killTimer(this, SCROLL_TIMERID);
		return;
	}
	if( event.Type == UIEVENT_BUTTONDOWN && isEnabled() )
	{
		SEND_LOG(L"UIEVENT_BUTTONDOWN %d", event.Type);

		m_uButtonState |= UISTATE_CAPTURED;
		m_ptLastMouse = event.ptMouse;
		m_dwDelayDeltaY = 0;
		m_dwDelayNum = 0;
		m_dwDelayLeft = 0;
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		m_pManager->setTimer(this, SCROLL_TIMERID, 50U);
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		SEND_LOG(L"UIEVENT_BUTTONUP %d", event.Type);

		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			m_uButtonState &= ~UISTATE_CAPTURED;
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			if( m_ptLastMouse.y != event.ptMouse.y ) {
				m_dwDelayDeltaY = (event.ptMouse.y - m_ptLastMouse.y);
				if( m_dwDelayDeltaY > 120 ) m_dwDelayDeltaY = 120;
				else if( m_dwDelayDeltaY < -120 ) m_dwDelayDeltaY = -120;
				m_dwDelayNum = (DWORD)sqrt((double)abs(m_dwDelayDeltaY)) * 5;
				m_dwDelayLeft = m_dwDelayNum;
			}
			else 
				m_pManager->killTimer(this, SCROLL_TIMERID);
		}
		return;
	}
	if( event.Type == UIEVENT_SCROLLWHEEL )
	{
		SEND_LOG(L"UIEVENT_SCROLLWHEEL %d", event.Type);

		LONG lDeltaY = 0;
		if( m_dwDelayNum > 0 ) lDeltaY =  (LONG)(calculateDelay((double)m_dwDelayLeft / m_dwDelayNum) * m_dwDelayDeltaY);
		switch( LOWORD(event.wParam) ) {
		case SB_LINEUP:
			if( m_dwDelayDeltaY >= 0 ) m_dwDelayDeltaY = lDeltaY + 8;
			else m_dwDelayDeltaY = lDeltaY + 12;
			break;
		case SB_LINEDOWN:
			if( m_dwDelayDeltaY <= 0 ) m_dwDelayDeltaY = lDeltaY - 8;
			else m_dwDelayDeltaY = lDeltaY - 12;
			break;
		}
		if( m_dwDelayDeltaY > 100 ) m_dwDelayDeltaY = 100;
		else if( m_dwDelayDeltaY < -100 ) m_dwDelayDeltaY = -100;
		m_dwDelayNum = (DWORD)sqrt((double)abs(m_dwDelayDeltaY)) * 5;
		m_dwDelayLeft = m_dwDelayNum;
		m_pManager->setTimer(this, SCROLL_TIMERID, 50U);
		return;
	}
	tileLayoutUI::doEvent(event);
}

void deskListCtrl::setPos(RECT rc)
{
	if( getCount() > 0 ) {
		int iDeskWidth = getItemAt(0)->getFixedWidth();
		int column = (rc.right - rc.left) / iDeskWidth;
		if( column < 1 ) column = 1;
		setColumns(column);
	}

	tileLayoutUI::setPos(rc);
}


void deskListCtrl::redrawNodes()
{
	if(!m_pmapNodes || m_hWnd == nullptr || m_pmapNodes->size() <= 0) return;
	if(getCount() != m_pmapNodes->size()) return;
	int nodeIdx = 0;
	const RECT rtClient = getPos();
	RECT rt;
	POINT pt;

	//SEND_LOG(L"client:%d %d %d %d", rtClient.left, rtClient.top, rtClient.right, rtClient.bottom);
	for (gtc::idlNode::IDINodeMap::const_iterator iter = m_pmapNodes->cbegin(); iter != m_pmapNodes->cend();++nodeIdx, ++iter)
	{
		rt = getItemAt(nodeIdx)->getPos();
		pt.x = rt.left;
		pt.y = rt.top;
		rt.top += 6;
		rt.bottom -= 6;
		if(::IntersectRect(&rt, &rtClient, &rt))
		{	// 显示
			::ClientToScreen(m_hWnd, &pt);
			kpdeskListView::moveto(iter->first, pt);

			//SEND_LOG(L"节点%d %s:%d %d %d %d", nodeIdx, iter->first.c_str(), rt.left, rt.top, rt.right, rt.bottom);
		}
		else
		{	// 将节点移到桌面外
			pt.x = pt.y = -999;
			kpdeskListView::moveto(iter->first, pt);
		}
	}
}


bool deskListCtrl::getNodePOS(const std::wstring &nodeName, RECT &rt, bool isScreen) {
	int nodeIdx = 0;
	for (gtc::idlNode::IDINodeMap::const_iterator iter = m_pmapNodes->cbegin(); iter != m_pmapNodes->cend();++nodeIdx, ++iter)
	{
		if (iter->first == nodeName)
		{
			rt = getItemAt(nodeIdx)->getPos();
			if (isScreen)
			{
				POINT pt;
				pt.x = rt.left;
				pt.y = rt.top;
				::ClientToScreen(m_hWnd, &pt);
				rt.left = pt.x;
				rt.top = pt.y;
				pt.x = rt.right;
				pt.y = rt.bottom;
				::ClientToScreen(m_hWnd, &pt);
				rt.right = pt.x;
				rt.bottom = pt.y;
			}
			return true;
		}
	}
	return false;
}

}