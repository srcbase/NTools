#include "containerUC.h"
#include "baseUC.h"

namespace gtc {

containerUC::containerUC(void)
	: controlUC()
{
	m_rcInset.top = UC_CONTAINER_SPACE_TOP;
	m_rcInset.bottom = UC_CONTAINER_SPACE_BOTTOM;
	m_rcInset.left = UC_CONTAINER_SPACE_LEFT;
	m_rcInset.right = UC_CONTAINER_SPACE_RIGHT;
	//m_szSpace.cx = 4;
	//m_szSpace.cy = 2;
}


containerUC::~containerUC(void)
{
	removeAll();
}

PCWSTR containerUC::getClass() const
{
	return L"containerUC";
}


void containerUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else __super::Event(event);
		return;
	}

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
	if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->isVisible() && m_pVerticalScrollBar->isEnabled() )
	{
		if( event.Type == BEUCEventKeyDown ) 
		{
			switch( event.chKey ) {
			case VK_DOWN:
				lineDown();
				return;
			case VK_UP:
				lineUp();
				return;
			case VK_NEXT:
				pageDown();
				return;
			case VK_PRIOR:
				pageUp();
				return;
			case VK_HOME:
				homeUp();
				return;
			case VK_END:
				endDown();
				return;
			}
		}
		else if( event.Type == BEUCEventScrollWheel )
		{
			//SEND_LOG(L"BEUCEventScrollWheel mmmm %d %d", LOWORD(event.wParam), HIWORD(event.wParam));
			switch( LOWORD(event.wParam) ) {
			case SB_LINEUP:
				lineUp(HIWORD(event.wParam));
				return;
			case SB_LINEDOWN:
				lineDown(HIWORD(event.wParam));
				return;
			}
		}
	}
	else if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->isVisible() && m_pHorizontalScrollBar->isEnabled() ) {
		if( event.Type == BEUCEventKeyDown ) 
		{
			switch( event.chKey ) {
			case VK_DOWN:
				lineRight();
				return;
			case VK_UP:
				lineLeft();
				return;
			case VK_NEXT:
				pageRight();
				return;
			case VK_PRIOR:
				pageLeft();
				return;
			case VK_HOME:
				homeLeft();
				return;
			case VK_END:
				endRight();
				return;
			}
		}
		else if( event.Type == BEUCEventScrollWheel )
		{
			switch( LOWORD(event.wParam) ) {
			case SB_LINEUP:
				lineLeft();
				return;
			case SB_LINEDOWN:
				lineRight();
				return;
			}
		}
	}
	__super::Event(event);
}

void containerUC::paint(HDC hdc)
{

}


controlUC* containerUC::getItemAt(int iIndex) const
{
	if( iIndex < 0 || iIndex >= int(m_items.size()) ) return NULL;
	return m_items[iIndex];
}

int containerUC::getItemIndex(controlUC* pControl) const 
{
	for( int it = 0; it < int(m_items.size()); it++ ) {
		if( m_items[it] == pControl ) {
			return it;
		}
	}
	return -1;
}

bool containerUC::setItemIndex(controlUC* pControl, int iIndex)
{
	for (std::vector<controlUC *>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		if (*iter == pControl)
		{
			needUpdate();
			m_items.erase(iter);
			m_items.insert(m_items.begin()+iIndex, pControl);
			return true;
		}
	}
	return false;
}

int containerUC::getCount() const
{
	return m_items.size();
}

bool containerUC::add(controlUC* pControl)
{
	if( pControl == NULL) return false;

	if( m_pwndManager != NULL ) m_pwndManager->initControl(pControl, this);
	//if( isVisible() ) needUpdate();
	//else pControl->setInternVisible(false);
	//return m_items.Add(pControl);  
	m_items.push_back(pControl);
	return true;
}

bool containerUC::addAt(controlUC* pControl, int iIndex)
{
	if( pControl == NULL) return false;

	if( m_pwndManager ) m_pwndManager->initControl(pControl, this);
	//if( isVisible() ) needUpdate();
	//else pControl->setInternVisible(false);
	//return m_items.InsertAt(iIndex, pControl);
	m_items.insert(m_items.begin()+iIndex, pControl);
	return true;
}

bool containerUC::remove(controlUC* pControl)
{
	if( pControl == NULL) return false;
	for (std::vector<controlUC *>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		if (*iter == pControl)
		{
			//needUpdate();
			//if (m_bAutoDestroy)
			//{
			//	if (m_bDelayedDestroy && m_pManager)
			//	{
			//		m_pManager->addDelayedCleanup(pControl);
			//	}
			//	else
			//		delete pControl;
			//}
			// 取消控件在窗口的注册
			if (m_pwndManager) m_pwndManager->removeControl(pControl);

			m_items.erase(iter);
			delete pControl;
			return true;
		}
	}
	return false;
}

bool containerUC::removeAt(int iIndex) 
{
	controlUC* pControl = getItemAt(iIndex);
	if (pControl != NULL) {
		return remove(pControl);
	}

	return false;
}

void containerUC::removeAll()
{
	for (std::vector<controlUC *>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		//if (m_bDelayedDestroy && m_pManager)
		//{
		//	m_pManager->addDelayedCleanup(*iter);
		//}
		//else
			delete *iter;
	}
	m_items.clear();
	//needUpdate();
}

void containerUC::buildScrollBar(scrollBarUC *pScrollbarVertical, scrollBarUC *pScrollbarHorizontal /*= nullptr*/)
{
	m_pVerticalScrollBar = pScrollbarVertical;
	m_pHorizontalScrollBar = pScrollbarHorizontal;
}

void containerUC::enableScrollBar(bool bEnableVertical /*= true*/, bool bEnableHorizontal /*= false*/)
{
	if (m_pVerticalScrollBar)
	{
		m_pVerticalScrollBar->setEnabled(bEnableVertical);
	}
	if (m_pHorizontalScrollBar)
	{
		m_pHorizontalScrollBar->setEnabled(bEnableHorizontal);
	}
}

scrollBarUC* containerUC::getVerticalScrollBar() const
{
	return m_pVerticalScrollBar;
}

scrollBarUC* containerUC::getHorizontalScrollBar() const
{
	return m_pHorizontalScrollBar;
}

SIZE containerUC::getScrollPos() const
{
	SIZE sz = {0, 0};
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) sz.cy = m_pVerticalScrollBar->getScrollPos();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) sz.cx = m_pHorizontalScrollBar->getScrollPos();
	return sz;
}

SIZE containerUC::getScrollRange() const
{
	SIZE sz = {0, 0};
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) sz.cy = m_pVerticalScrollBar->getScrollRange();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) sz.cx = m_pHorizontalScrollBar->getScrollRange();
	return sz;
}

void containerUC::setScrollPos(SIZE szPos)
{
	int cx = 0;
	int cy = 0;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) {
		int iLastScrollPos = m_pVerticalScrollBar->getScrollPos();
		m_pVerticalScrollBar->setScrollPos(szPos.cy);
		cy = m_pVerticalScrollBar->getScrollPos() - iLastScrollPos;
	}

	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) {
		int iLastScrollPos = m_pHorizontalScrollBar->getScrollPos();
		m_pHorizontalScrollBar->setScrollPos(szPos.cx);
		cx = m_pHorizontalScrollBar->getScrollPos() - iLastScrollPos;
	}

	if( cx == 0 && cy == 0 ) return;
	
	RECT rcPos;
	for (std::vector<controlUC *>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		//SEND_LOG(L"scrollpos <%s> %d", gtc::format::rect(rcPos).c_str(), (*iter)->isVisible());
		if(!(*iter)->isVisible()) continue;
		//if((*iter)->isFloat()) continue;
		rcPos = (*iter)->getPosition();
		rcPos.left -= cx;
		rcPos.right -= cx;
		rcPos.top -= cy;
		rcPos.bottom -= cy;
		(*iter)->setPosition(rcPos);
	}
	invalidate();
}

void containerUC::setFloatPos(int iIndex)
{
	
}
void containerUC::setPosition(RECT &pos)
{
	__super::setPosition(pos);
	if( m_items.empty() ) return;
	pos.left += m_rcInset.left;
	pos.top += m_rcInset.top;
	pos.right -= m_rcInset.right;
	pos.bottom -= m_rcInset.bottom;

	for( int it = 0; it < int(m_items.size()); it++ ) {
		if( !m_items[it]->isVisible() ) continue;
		if( m_items[it]->isFloat() ) {
			setFloatPos(it);
		}
		else {
			m_items[it]->setPosition(pos); // 所有非float子控件放大到整个客户区
		}
	}
}

void containerUC::lineUp(int step /*= 0*/)
{
	SIZE sz = getScrollPos();
	if (step <= 0)
	{
		int cyLine = 8;
		if( m_pwndManager ) cyLine = m_pwndManager->getDefaultFont()->tm.tmHeight + 8;

		sz.cy -= cyLine;
	}
	else
		sz.cy -= step;
	setScrollPos(sz);
}

void containerUC::lineDown(int step /*= 0*/)
{
	SIZE sz = getScrollPos();
	if (step <= 0)
	{
		int cyLine = 8;
		if( m_pwndManager ) cyLine = m_pwndManager->getDefaultFont()->tm.tmHeight + 8;

		sz.cy += cyLine;
	}
	else
		sz.cy += step;
	setScrollPos(sz);
}

void containerUC::pageUp()
{
	SIZE sz = getScrollPos();
	int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) iOffset -= m_pHorizontalScrollBar->getFixedHeight();
	sz.cy -= iOffset;
	setScrollPos(sz);
}

void containerUC::pageDown()
{
	SIZE sz = getScrollPos();
	//SEND_LOG(L"pageDown %d,%d", sz.cx, sz.cy);
	int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) iOffset -= m_pHorizontalScrollBar->getFixedHeight();
	sz.cy += iOffset;
	setScrollPos(sz);
	//SEND_LOG(L"pageDown %d,%d", sz.cx, sz.cy);
}

void containerUC::homeUp()
{
	SIZE sz = getScrollPos();
	sz.cy = 0;
	setScrollPos(sz);
}

void containerUC::endDown()
{
	SIZE sz = getScrollPos();
	sz.cy = getScrollRange().cy;
	setScrollPos(sz);
}

void containerUC::lineLeft()
{
	SIZE sz = getScrollPos();
	sz.cx -= 8;
	setScrollPos(sz);
}

void containerUC::lineRight()
{
	SIZE sz = getScrollPos();
	sz.cx += 8;
	setScrollPos(sz);
}

void containerUC::pageLeft()
{
	SIZE sz = getScrollPos();
	int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) iOffset -= m_pVerticalScrollBar->getFixedWidth();
	sz.cx -= iOffset;
	setScrollPos(sz);
}

void containerUC::pageRight()
{
	SIZE sz = getScrollPos();
	int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) iOffset -= m_pVerticalScrollBar->getFixedWidth();
	sz.cx += iOffset;
	setScrollPos(sz);
}

void containerUC::homeLeft()
{
	SIZE sz = getScrollPos();
	sz.cx = 0;
	setScrollPos(sz);
}

void containerUC::endRight()
{
	SIZE sz = getScrollPos();
	sz.cx = getScrollRange().cx;
	setScrollPos(sz);
}

SIZE containerUC::getMinSize() const
{
	SIZE sz = {m_rcInset.left+m_rcInset.right, m_rcInset.top+m_rcInset.bottom};
	return sz;
}
}