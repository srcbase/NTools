#include "StdAfx.h"
#include "containerUI.h"

namespace wui {


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

containerUI::containerUI() 
	: m_iChildPadding(0),
	m_bAutoDestroy(true),
	m_bDelayedDestroy(true),
	m_bMouseChildEnabled(true),
	m_pVerticalScrollBar(NULL),
	m_pHorizontalScrollBar(NULL),
	m_bScrollProcess(false)
{
	//TRACE_CLASS_BEGIN(this)
	::ZeroMemory(&m_rcInset, sizeof(m_rcInset));
	
}

containerUI::~containerUI()
{
	//TRACE_CLASS_END(this)
	m_bDelayedDestroy = false;
	removeAll();
	if( m_pVerticalScrollBar ) delete m_pVerticalScrollBar;
	if( m_pHorizontalScrollBar ) delete m_pHorizontalScrollBar;
}

PCWSTR containerUI::getClass() const
{
	return L"ContainerUI";
}

LPVOID containerUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"IContainer") == 0 ) return static_cast<IContainerUI*>(this);
	else if( _tcscmp(pstrName, L"Container") == 0 ) return static_cast<containerUI*>(this);
	return controlUI::getInterface(pstrName);
}

controlUI* containerUI::getItemAt(int iIndex) const
{
	if( iIndex < 0 || iIndex >= m_items.size() ) return NULL;
	return m_items[iIndex];
}

int containerUI::getItemIndex(controlUI* pControl) const
{
	for( int it = 0; it < m_items.size(); it++ ) {
		if( m_items[it] == pControl ) {
			return it;
		}
	}
	return -1;
}

bool containerUI::setItemIndex(controlUI* pControl, int iIndex)
{
	//for( int it = 0; it < m_items.GetSize(); it++ ) {
	//	if( static_cast<controlUI*>(m_items[it]) == pControl ) {
	//		NeedUpdate();            
	//		m_items.Remove(it);
	//		return m_items.InsertAt(iIndex, pControl);
	//	}
	//}
	for (std::vector<controlUI *>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
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

int containerUI::getCount() const
{
	return m_items.size();
}

bool containerUI::add(controlUI* pControl)
{
	if( pControl == NULL) return false;

	if( m_pManager != NULL ) m_pManager->initControls(pControl, this);
	if( isVisible() ) needUpdate();
	else pControl->setInternVisible(false);
	//return m_items.Add(pControl);  
	m_items.push_back(pControl);
	return true;
}

bool containerUI::addAt(controlUI* pControl, int iIndex)
{
	if( pControl == NULL) return false;

	if( m_pManager != NULL ) m_pManager->initControls(pControl, this);
	if( isVisible() ) needUpdate();
	else pControl->setInternVisible(false);
	//return m_items.InsertAt(iIndex, pControl);
	m_items.insert(m_items.begin()+iIndex, pControl);
	return true;
}

bool containerUI::remove(controlUI* pControl)
{
	if( pControl == NULL) return false;

	//for( int it = 0; it < m_items.GetSize(); it++ ) {
	//	if( static_cast<controlUI*>(m_items[it]) == pControl ) {
	//		NeedUpdate();
	//		if( m_bAutoDestroy ) {
	//			if( m_bDelayedDestroy && m_pManager ) m_pManager->AddDelayedCleanup(pControl);             
	//			else delete pControl;
	//		}
	//		return m_items.Remove(it);
	//	}
	//}
	for (std::vector<controlUI *>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		if (*iter == pControl)
		{
			needUpdate();
			if (m_bAutoDestroy)
			{
				if (m_bDelayedDestroy && m_pManager)
				{
					m_pManager->addDelayedCleanup(pControl);
				}
				else
					delete pControl;
			}
			m_items.erase(iter);
			return true;
		}
	}
	return false;
}

bool containerUI::removeAt(int iIndex)
{
	controlUI* pControl = getItemAt(iIndex);
	if (pControl != NULL) {
		return containerUI::remove(pControl);
	}

	return false;
}

void containerUI::removeAll()
{
	//for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ ) {
	//	if( m_bDelayedDestroy && m_pManager ) m_pManager->AddDelayedCleanup(static_cast<controlUI*>(m_items[it]));             
	//	else delete static_cast<controlUI*>(m_items[it]);
	//}

	for (std::vector<controlUI *>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		if (m_bDelayedDestroy && m_pManager)
		{
			m_pManager->addDelayedCleanup(*iter);
		}
		else
			delete *iter;
	}
	m_items.clear();
	needUpdate();
}

bool containerUI::isAutoDestroy() const
{
	return m_bAutoDestroy;
}

void containerUI::setAutoDestroy(bool bAuto)
{
	m_bAutoDestroy = bAuto;
}

bool containerUI::isDelayedDestroy() const
{
	return m_bDelayedDestroy;
}

void containerUI::setDelayedDestroy(bool bDelayed)
{
	m_bDelayedDestroy = bDelayed;
}

RECT containerUI::getInset() const
{
	return m_rcInset;
}

void containerUI::setInset(RECT rcInset)
{
	m_rcInset = rcInset;
	needUpdate();
}

int containerUI::getChildPadding() const
{
	return m_iChildPadding;
}

void containerUI::setChildPadding(int iPadding)
{
	m_iChildPadding = iPadding;
	needUpdate();
}

bool containerUI::isMouseChildEnabled() const
{
	return m_bMouseChildEnabled;
}

void containerUI::setMouseChildEnabled(bool bEnable)
{
	m_bMouseChildEnabled = bEnable;
}

void containerUI::setVisible(bool bVisible)
{
	if( m_bVisible == bVisible ) return;
	controlUI::setVisible(bVisible);
	//for( int it = 0; it < m_items.GetSize(); it++ ) {
	//	static_cast<controlUI*>(m_items[it])->SetInternVisible(IsVisible());
	//}
	for (std::vector<controlUI *>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		(*iter)->setInternVisible(isVisible());
	}
}

void containerUI::setInternVisible(bool bVisible)
{
	controlUI::setInternVisible(bVisible);
	if( m_items.empty() ) return;
	//for( int it = 0; it < m_items.GetSize(); it++ ) {
	//	static_cast<controlUI*>(m_items[it])->SetInternVisible(IsVisible());
	//}
	for (std::vector<controlUI *>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		(*iter)->setInternVisible(isVisible());
	}
}

void containerUI::setMouseEnabled(bool bEnabled)
{
	if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->setMouseEnabled(bEnabled);
	if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->setMouseEnabled(bEnabled);
	controlUI::setMouseEnabled(bEnabled);
}

void containerUI::doEvent(TEventUI& event)
{
	if( !isMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->doEvent(event);
		else controlUI::doEvent(event);
		return;
	}

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
	if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->isVisible() && m_pVerticalScrollBar->isEnabled() )
	{
		if( event.Type == UIEVENT_KEYDOWN ) 
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
		else if( event.Type == UIEVENT_SCROLLWHEEL )
		{
			switch( LOWORD(event.wParam) ) {
			case SB_LINEUP:
				lineUp();
				return;
			case SB_LINEDOWN:
				lineDown();
				return;
			}
		}
	}
	else if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->isVisible() && m_pHorizontalScrollBar->isEnabled() ) {
		if( event.Type == UIEVENT_KEYDOWN ) 
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
		else if( event.Type == UIEVENT_SCROLLWHEEL )
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
	controlUI::doEvent(event);
}

SIZE containerUI::getScrollPos() const
{
	SIZE sz = {0, 0};
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) sz.cy = m_pVerticalScrollBar->getScrollPos();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) sz.cx = m_pHorizontalScrollBar->getScrollPos();
	return sz;
}

SIZE containerUI::getScrollRange() const
{
	SIZE sz = {0, 0};
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) sz.cy = m_pVerticalScrollBar->getScrollRange();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) sz.cx = m_pHorizontalScrollBar->getScrollRange();
	return sz;
}

void containerUI::setScrollPos(SIZE szPos)
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
	//for( int it2 = 0; it2 < m_items.size(); it2++ ) {
	//	controlUI* pControl = static_cast<controlUI*>(m_items[it2]);
	//	if( !pControl->isVisible() ) continue;
	//	if( pControl->isFloat() ) continue;

	//	rcPos = pControl->getPos();
	//	rcPos.left -= cx;
	//	rcPos.right -= cx;
	//	rcPos.top -= cy;
	//	rcPos.bottom -= cy;
	//	pControl->setPos(rcPos);
	//}

	for (std::vector<controlUI *>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		if(!(*iter)->isVisible()) continue;
		if((*iter)->isFloat()) continue;
		rcPos = (*iter)->getPos();
		rcPos.left -= cx;
		rcPos.right -= cx;
		rcPos.top -= cy;
		rcPos.bottom -= cy;
		(*iter)->setPos(rcPos);
	}
	invalidate();
}

void containerUI::lineUp()
{
	int cyLine = 8;
	if( m_pManager ) cyLine = m_pManager->getDefaultFontInfo()->tm.tmHeight + 8;

	SIZE sz = getScrollPos();
	sz.cy -= cyLine;
	setScrollPos(sz);
}

void containerUI::lineDown()
{
	int cyLine = 8;
	if( m_pManager ) cyLine = m_pManager->getDefaultFontInfo()->tm.tmHeight + 8;

	SIZE sz = getScrollPos();
	sz.cy += cyLine;
	setScrollPos(sz);
}

void containerUI::pageUp()
{
	SIZE sz = getScrollPos();
	int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) iOffset -= m_pHorizontalScrollBar->getFixedHeight();
	sz.cy -= iOffset;
	setScrollPos(sz);
}

void containerUI::pageDown()
{
	SIZE sz = getScrollPos();
	int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) iOffset -= m_pHorizontalScrollBar->getFixedHeight();
	sz.cy += iOffset;
	setScrollPos(sz);
}

void containerUI::homeUp()
{
	SIZE sz = getScrollPos();
	sz.cy = 0;
	setScrollPos(sz);
}

void containerUI::endDown()
{
	SIZE sz = getScrollPos();
	sz.cy = getScrollRange().cy;
	setScrollPos(sz);
}

void containerUI::lineLeft()
{
	SIZE sz = getScrollPos();
	sz.cx -= 8;
	setScrollPos(sz);
}

void containerUI::lineRight()
{
	SIZE sz = getScrollPos();
	sz.cx += 8;
	setScrollPos(sz);
}

void containerUI::pageLeft()
{
	SIZE sz = getScrollPos();
	int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) iOffset -= m_pVerticalScrollBar->getFixedWidth();
	sz.cx -= iOffset;
	setScrollPos(sz);
}

void containerUI::pageRight()
{
	SIZE sz = getScrollPos();
	int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) iOffset -= m_pVerticalScrollBar->getFixedWidth();
	sz.cx += iOffset;
	setScrollPos(sz);
}

void containerUI::homeLeft()
{
	SIZE sz = getScrollPos();
	sz.cx = 0;
	setScrollPos(sz);
}

void containerUI::endRight()
{
	SIZE sz = getScrollPos();
	sz.cx = getScrollRange().cx;
	setScrollPos(sz);
}

void containerUI::enableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
{
	if( bEnableVertical && !m_pVerticalScrollBar ) {
		m_pVerticalScrollBar = new scrollBarUI;
		m_pVerticalScrollBar->setOwner(this);
		m_pVerticalScrollBar->setManager(m_pManager, NULL, false);
		if ( m_pManager ) {
			PCWSTR pDefaultAttributes = m_pManager->getDefaultAttributeList(L"VScrollBar");
			if( pDefaultAttributes ) {
				m_pVerticalScrollBar->applyAttributeList(pDefaultAttributes);
			}
		}
	}
	else if( !bEnableVertical && m_pVerticalScrollBar ) {
		delete m_pVerticalScrollBar;
		m_pVerticalScrollBar = NULL;
	}

	if( bEnableHorizontal && !m_pHorizontalScrollBar ) {
		m_pHorizontalScrollBar = new scrollBarUI;
		m_pHorizontalScrollBar->setHorizontal(true);
		m_pHorizontalScrollBar->setOwner(this);
		m_pHorizontalScrollBar->setManager(m_pManager, NULL, false);
		if ( m_pManager ) {
			PCWSTR pDefaultAttributes = m_pManager->getDefaultAttributeList(L"HScrollBar");
			if( pDefaultAttributes ) {
				m_pHorizontalScrollBar->applyAttributeList(pDefaultAttributes);
			}
		}
	}
	else if( !bEnableHorizontal && m_pHorizontalScrollBar ) {
		delete m_pHorizontalScrollBar;
		m_pHorizontalScrollBar = NULL;
	}

	needUpdate();
}

scrollBarUI* containerUI::getVerticalScrollBar() const
{
	return m_pVerticalScrollBar;
}

scrollBarUI* containerUI::getHorizontalScrollBar() const
{
	return m_pHorizontalScrollBar;
}

int containerUI::findSelectable(int iIndex, bool bForward /*= true*/) const
{
	// NOTE: This is actually a helper-function for the list/combo/ect controls
	//       that allow them to find the next enabled/available selectable item
	if( getCount() == 0 ) return -1;
	iIndex = CLAMP(iIndex, 0, getCount() - 1);
	if( bForward ) {
		for( int i = iIndex; i < getCount(); i++ ) {
			if( getItemAt(i)->getInterface(L"ListItem") != NULL 
				&& getItemAt(i)->isVisible()
				&& getItemAt(i)->isEnabled() ) return i;
		}
		return -1;
	}
	else {
		for( int i = iIndex; i >= 0; --i ) {
			if( getItemAt(i)->getInterface(L"ListItem") != NULL 
				&& getItemAt(i)->isVisible()
				&& getItemAt(i)->isEnabled() ) return i;
		}
		return findSelectable(0, true);
	}
}

void containerUI::setPos(RECT rc)
{
	controlUI::setPos(rc);
	if( m_items.empty() ) return;
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	for( int it = 0; it < m_items.size(); it++ ) {
		if( !m_items[it]->isVisible() ) continue;
		if( m_items[it]->isFloat() ) {
			setFloatPos(it);
		}
		else {
			m_items[it]->setPos(rc); // 所有非float子控件放大到整个客户区
		}
	}

}

void containerUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"inset") == 0 ) {
		RECT rcInset = { 0 };
		PWSTR pstr = NULL;
		rcInset.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcInset.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcInset.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setInset(rcInset);
	}
	else if( _tcscmp(pstrName, L"mousechild") == 0 ) setMouseChildEnabled(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"vscrollbar") == 0 ) {
		enableScrollBar(_tcscmp(pstrValue, L"true") == 0, getHorizontalScrollBar() != NULL);
	}
	else if( _tcscmp(pstrName, L"vscrollbarstyle") == 0 ) {
		enableScrollBar(true, getHorizontalScrollBar() != NULL);
		if( getVerticalScrollBar() ) getVerticalScrollBar()->applyAttributeList(pstrValue);
	}
	else if( _tcscmp(pstrName, L"hscrollbar") == 0 ) {
		enableScrollBar(getVerticalScrollBar() != NULL, _tcscmp(pstrValue, L"true") == 0);
	}
	else if( _tcscmp(pstrName, L"hscrollbarstyle") == 0 ) {
		enableScrollBar(getVerticalScrollBar() != NULL, true);
		if( getHorizontalScrollBar() ) getHorizontalScrollBar()->applyAttributeList(pstrValue);
	}
	else if( _tcscmp(pstrName, L"childpadding") == 0 ) setChildPadding(_ttoi(pstrValue));
	else controlUI::setAttribute(pstrName, pstrValue);
}

void containerUI::setManager(paintManagerUI* pManager, controlUI* pParent, bool bInit)
{
	//for( int it = 0; it < m_items.GetSize(); it++ ) {
	//	static_cast<controlUI*>(m_items[it])->SetManager(pManager, this, bInit);
	//}
	for (std::vector<controlUI *>::const_iterator iter = m_items.cbegin(); iter != m_items.cend(); ++iter )
	{
		(*iter)->setManager(pManager, this, bInit);
	}
	if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->setManager(pManager, this, bInit);
	if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->setManager(pManager, this, bInit);
	controlUI::setManager(pManager, pParent, bInit);
}

controlUI* containerUI::findControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	// Check if this guy is valid
	if( (uFlags & UIFIND_VISIBLE) != 0 && !isVisible() ) return NULL;
	if( (uFlags & UIFIND_ENABLED) != 0 && !isEnabled() ) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 ) {
		if( !::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData))) ) return NULL;
		if( !m_bMouseChildEnabled ) {
			controlUI* pResult = NULL;
			if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->findControl(Proc, pData, uFlags);
			if( pResult == NULL && m_pHorizontalScrollBar != NULL ) pResult = m_pHorizontalScrollBar->findControl(Proc, pData, uFlags);
			if( pResult == NULL ) pResult = controlUI::findControl(Proc, pData, uFlags);
			return pResult;
		}
	}

	controlUI* pResult = NULL;
	if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->findControl(Proc, pData, uFlags);
	if( pResult == NULL && m_pHorizontalScrollBar != NULL ) pResult = m_pHorizontalScrollBar->findControl(Proc, pData, uFlags);
	if( pResult != NULL ) return pResult;

	if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
		controlUI* pControl = controlUI::findControl(Proc, pData, uFlags);
		if( pControl != NULL ) return pControl;
	}
	RECT rc = m_rcItem;
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) rc.right -= m_pVerticalScrollBar->getFixedWidth();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();
	if( (uFlags & UIFIND_TOP_FIRST) != 0 ) {
		for( int it = m_items.size() - 1; it >= 0; it-- ) {
			controlUI* pControl = static_cast<controlUI*>(m_items[it])->findControl(Proc, pData, uFlags);
			if( pControl != NULL ) {
				if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->isFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
					continue;
				else 
					return pControl;
			}            
		}
	}
	else {
		for( int it = 0; it < m_items.size(); it++ ) {
			controlUI* pControl = static_cast<controlUI*>(m_items[it])->findControl(Proc, pData, uFlags);
			if( pControl != NULL ) {
				if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->isFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
					continue;
				else 
					return pControl;
			} 
		}
	}

	if( pResult == NULL ) pResult = controlUI::findControl(Proc, pData, uFlags);
	return pResult;
}

void containerUI::doPaint(HDC hDC, const RECT& rcPaint)
{
	RECT rcTemp = { 0 };
	if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

	renderClip clip;
	renderClip::generateClip(hDC, rcTemp, clip);
	controlUI::doPaint(hDC, rcPaint);

	if( m_items.size() > 0 ) {
		RECT rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) rc.right -= m_pVerticalScrollBar->getFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();

		if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
			for( int it = 0; it < m_items.size(); it++ ) {
				controlUI* pControl = static_cast<controlUI*>(m_items[it]);
				if( !pControl->isVisible() ) continue;
				if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->getPos()) ) continue;
				if( pControl ->isFloat() ) {
					if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->getPos()) ) continue;
					pControl->doPaint(hDC, rcPaint);
				}
			}
		}
		else {
			renderClip childClip;
			renderClip::generateClip(hDC, rcTemp, childClip);
			for( int it = 0; it < m_items.size(); it++ ) {
				controlUI* pControl = static_cast<controlUI*>(m_items[it]);
				if( !pControl->isVisible() ) continue;
				if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->getPos()) ) continue;
				if( pControl ->isFloat() ) {
					if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->getPos()) ) continue;
					renderClip::useOldClipBegin(hDC, childClip);
					pControl->doPaint(hDC, rcPaint);
					renderClip::useOldClipEnd(hDC, childClip);
				}
				else {
					if( !::IntersectRect(&rcTemp, &rc, &pControl->getPos()) ) continue;
					pControl->doPaint(hDC, rcPaint);
				}
			}
		}
	}

	if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->isVisible() ) {
		if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->getPos()) ) {
			m_pVerticalScrollBar->doPaint(hDC, rcPaint);
		}
	}

	if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->isVisible() ) {
		if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->getPos()) ) {
			m_pHorizontalScrollBar->doPaint(hDC, rcPaint);
		}
	}
}

void containerUI::setFloatPos(int iIndex)
{
	// 因为controlUI::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
	if( iIndex < 0 || iIndex >= m_items.size() ) return;

	controlUI* pControl = m_items[iIndex];

	if( !pControl->isVisible() ) return;
	if( !pControl->isFloat() ) return;

	SIZE szXY = pControl->getFixedXY();
	SIZE sz = {pControl->getFixedWidth(), pControl->getFixedHeight()};
	RECT rcCtrl = { 0 };
	if( szXY.cx >= 0 ) {
		rcCtrl.left = m_rcItem.left + szXY.cx;
		rcCtrl.right = m_rcItem.left + szXY.cx + sz.cx;
	}
	else {
		rcCtrl.left = m_rcItem.right + szXY.cx - sz.cx;
		rcCtrl.right = m_rcItem.right + szXY.cx;
	}
	if( szXY.cy >= 0 ) {
		rcCtrl.top = m_rcItem.top + szXY.cy;
		rcCtrl.bottom = m_rcItem.top + szXY.cy + sz.cy;
	}
	else {
		rcCtrl.top = m_rcItem.bottom + szXY.cy - sz.cy;
		rcCtrl.bottom = m_rcItem.bottom + szXY.cy;
	}
	if( pControl->isRelativePos() )
	{
		TRelativePosUI tRelativePos = pControl->getRelativePos();
		SIZE szParent = {m_rcItem.right-m_rcItem.left,m_rcItem.bottom-m_rcItem.top};
		if(tRelativePos.szParent.cx != 0)
		{
			int nIncrementX = szParent.cx-tRelativePos.szParent.cx;
			int nIncrementY = szParent.cy-tRelativePos.szParent.cy;
			rcCtrl.left += (nIncrementX*tRelativePos.nMoveXPercent/100);
			rcCtrl.top += (nIncrementY*tRelativePos.nMoveYPercent/100);
			rcCtrl.right = rcCtrl.left+sz.cx+(nIncrementX*tRelativePos.nZoomXPercent/100);
			rcCtrl.bottom = rcCtrl.top+sz.cy+(nIncrementY*tRelativePos.nZoomYPercent/100);
		}
		pControl->setRelativeParentSize(szParent);
	}
	pControl->setPos(rcCtrl);
}

void containerUI::processScrollBar(RECT rc, int cxRequired, int cyRequired)
{
	if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->isVisible() ) {
		RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->getFixedHeight()};
		m_pHorizontalScrollBar->setPos(rcScrollBarPos);
	}

	if( m_pVerticalScrollBar == NULL ) return;

	if( cyRequired > rc.bottom - rc.top && !m_pVerticalScrollBar->isVisible() ) {
		m_pVerticalScrollBar->setVisible(true);
		m_pVerticalScrollBar->setScrollRange(cyRequired - (rc.bottom - rc.top));
		m_pVerticalScrollBar->setScrollPos(0);
		m_bScrollProcess = true;
		setPos(m_rcItem);
		m_bScrollProcess = false;
		return;
	}
	// No scrollbar required
	if( !m_pVerticalScrollBar->isVisible() ) return;

	// Scroll not needed anymore?
	int cyScroll = cyRequired - (rc.bottom - rc.top);
	if( cyScroll <= 0 && !m_bScrollProcess) {
		m_pVerticalScrollBar->setVisible(false);
		m_pVerticalScrollBar->setScrollPos(0);
		m_pVerticalScrollBar->setScrollRange(0);
		setPos(m_rcItem);
	}
	else
	{
		RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->getFixedWidth(), rc.bottom };
		m_pVerticalScrollBar->setPos(rcScrollBarPos);

		if( m_pVerticalScrollBar->getScrollRange() != cyScroll ) {
			int iScrollPos = m_pVerticalScrollBar->getScrollPos();
			m_pVerticalScrollBar->setScrollRange(::abs(cyScroll));
			if( m_pVerticalScrollBar->getScrollRange() == 0 ) {
				m_pVerticalScrollBar->setVisible(false);
				m_pVerticalScrollBar->setScrollPos(0);
			}
			if( iScrollPos > m_pVerticalScrollBar->getScrollPos() ) {
				setPos(m_rcItem);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

verticalLayoutUI::verticalLayoutUI() : m_iSepHeight(0), m_uButtonState(0), m_bImmMode(false)
{
	//TRACE_CLASS_BEGIN(this)
	ptLastMouse.x = ptLastMouse.y = 0;
	::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
}

PCWSTR verticalLayoutUI::getClass() const
{
	return L"VerticalLayoutUI";
}

LPVOID verticalLayoutUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"VerticalLayout") == 0 ) return static_cast<verticalLayoutUI*>(this);
	return containerUI::getInterface(pstrName);
}

UINT verticalLayoutUI::getControlFlags() const
{
	if( isEnabled() && m_iSepHeight != 0 ) return UIFLAG_SETCURSOR;
	else return 0;
}

void verticalLayoutUI::setPos(RECT rc)
{
	controlUI::setPos(rc);
	rc = m_rcItem;

	// Adjust for inset
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) rc.right -= m_pVerticalScrollBar->getFixedWidth();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();

	if( m_items.size() == 0) {
		processScrollBar(rc, 0, 0);
		return;
	}

	// Determine the minimum size
	SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) 
		szAvailable.cx += m_pHorizontalScrollBar->getScrollRange();

	int nAdjustables = 0;
	int cyFixed = 0;
	int nEstimateNum = 0;
	for( int it1 = 0; it1 < m_items.size(); it1++ ) {
		controlUI* pControl = m_items[it1];
		if( !pControl->isVisible() ) continue;
		if( pControl->isFloat() ) continue;
		SIZE sz = pControl->estimateSize(szAvailable);
		if( sz.cy == 0 ) {
			nAdjustables++;
		}
		else {
			if( sz.cy < pControl->getMinHeight() ) sz.cy = pControl->getMinHeight();
			if( sz.cy > pControl->getMaxHeight() ) sz.cy = pControl->getMaxHeight();
		}
		cyFixed += sz.cy + pControl->getPadding().top + pControl->getPadding().bottom;
		nEstimateNum++;
	}
	cyFixed += (nEstimateNum - 1) * m_iChildPadding;

	// Place elements
	int cyNeeded = 0;
	int cyExpand = 0;
	if( nAdjustables > 0 ) cyExpand = max(0, (szAvailable.cy - cyFixed) / nAdjustables);
	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosY = rc.top;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) {
		iPosY -= m_pVerticalScrollBar->getScrollPos();
	}
	int iPosX = rc.left;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) {
		iPosX -= m_pHorizontalScrollBar->getScrollPos();
	}
	int iAdjustable = 0;
	int cyFixedRemaining = cyFixed;
	for( int it2 = 0; it2 < m_items.size(); it2++ ) {
		controlUI* pControl = m_items[it2];
		if( !pControl->isVisible() ) continue;
		if( pControl->isFloat() ) {
			setFloatPos(it2);
			continue;
		}

		RECT rcPadding = pControl->getPadding();
		szRemaining.cy -= rcPadding.top;
		SIZE sz = pControl->estimateSize(szRemaining);
		if( sz.cy == 0 ) {
			iAdjustable++;
			sz.cy = cyExpand;
			// Distribute remaining to last element (usually round-off left-overs)
			if( iAdjustable == nAdjustables ) {
				sz.cy = max(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
			} 
			if( sz.cy < pControl->getMinHeight() ) sz.cy = pControl->getMinHeight();
			if( sz.cy > pControl->getMaxHeight() ) sz.cy = pControl->getMaxHeight();
		}
		else {
			if( sz.cy < pControl->getMinHeight() ) sz.cy = pControl->getMinHeight();
			if( sz.cy > pControl->getMaxHeight() ) sz.cy = pControl->getMaxHeight();
			cyFixedRemaining -= sz.cy;
		}

		sz.cx = pControl->getFixedWidth();
		if( sz.cx == 0 ) sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
		if( sz.cx < 0 ) sz.cx = 0;
		if( sz.cx < pControl->getMinWidth() ) sz.cx = pControl->getMinWidth();
		if( sz.cx > pControl->getMaxWidth() ) sz.cx = pControl->getMaxWidth();

		RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
		pControl->setPos(rcCtrl);

		iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
		cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
		szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
	}
	cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

	// Process the scrollbar
	processScrollBar(rc, 0, cyNeeded);
}

void verticalLayoutUI::doPostPaint(HDC hDC, const RECT& rcPaint)
{
	if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) {
		RECT rcSeparator = getThumbRect(true);
		renderEngine::drawColor(hDC, rcSeparator, 0xAA000000);
	}
}

void verticalLayoutUI::setSepHeight(int iHeight)
{
	m_iSepHeight = iHeight;
}

int verticalLayoutUI::getSepHeight() const
{
	return m_iSepHeight;
}

void verticalLayoutUI::setSepImmMode(bool bImmediately)
{
	if( m_bImmMode == bImmediately ) return;
	if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
		m_pManager->removePostPaint(this);
	}

	m_bImmMode = bImmediately;
}

bool verticalLayoutUI::isSepImmMode() const
{
	return m_bImmMode;
}

void verticalLayoutUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"sepheight") == 0 ) setSepHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"sepimm") == 0 ) setSepImmMode(_tcscmp(pstrValue, L"true") == 0);
	else containerUI::setAttribute(pstrName, pstrValue);
}

void verticalLayoutUI::doEvent(TEventUI& event)
{
	if( m_iSepHeight != 0 ) {
		if( event.Type == UIEVENT_BUTTONDOWN && isEnabled() )
		{
			RECT rcSeparator = getThumbRect(false);
			if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
				m_uButtonState |= UISTATE_CAPTURED;
				ptLastMouse = event.ptMouse;
				m_rcNewPos = m_rcItem;
				if( !m_bImmMode && m_pManager ) m_pManager->addPostPaint(this);
				return;
			}
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				m_uButtonState &= ~UISTATE_CAPTURED;
				m_rcItem = m_rcNewPos;
				if( !m_bImmMode && m_pManager ) m_pManager->removePostPaint(this);
				needParentUpdate();
				return;
			}
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				LONG cy = event.ptMouse.y - ptLastMouse.y;
				ptLastMouse = event.ptMouse;
				RECT rc = m_rcNewPos;
				if( m_iSepHeight >= 0 ) {
					if( cy > 0 && event.ptMouse.y < m_rcNewPos.bottom + m_iSepHeight ) return;
					if( cy < 0 && event.ptMouse.y > m_rcNewPos.bottom ) return;
					rc.bottom += cy;
					if( rc.bottom - rc.top <= getMinHeight() ) {
						if( m_rcNewPos.bottom - m_rcNewPos.top <= getMinHeight() ) return;
						rc.bottom = rc.top + getMinHeight();
					}
					if( rc.bottom - rc.top >= getMaxHeight() ) {
						if( m_rcNewPos.bottom - m_rcNewPos.top >= getMaxHeight() ) return;
						rc.bottom = rc.top + getMaxHeight();
					}
				}
				else {
					if( cy > 0 && event.ptMouse.y < m_rcNewPos.top ) return;
					if( cy < 0 && event.ptMouse.y > m_rcNewPos.top + m_iSepHeight ) return;
					rc.top += cy;
					if( rc.bottom - rc.top <= getMinHeight() ) {
						if( m_rcNewPos.bottom - m_rcNewPos.top <= getMinHeight() ) return;
						rc.top = rc.bottom - getMinHeight();
					}
					if( rc.bottom - rc.top >= getMaxHeight() ) {
						if( m_rcNewPos.bottom - m_rcNewPos.top >= getMaxHeight() ) return;
						rc.top = rc.bottom - getMaxHeight();
					}
				}

				rect rcInvalidate = getThumbRect(true);
				m_rcNewPos = rc;
				m_cxyFixed.cy = m_rcNewPos.bottom - m_rcNewPos.top;

				if( m_bImmMode ) {
					m_rcItem = m_rcNewPos;
					needParentUpdate();
				}
				else {
					rcInvalidate.join(getThumbRect(true));
					rcInvalidate.join(getThumbRect(false));
					if( m_pManager ) m_pManager->invalidate(rcInvalidate);
				}
				return;
			}
		}
		if( event.Type == UIEVENT_SETCURSOR )
		{
			RECT rcSeparator = getThumbRect(false);
			if( isEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
				return;
			}
		}
	}
	containerUI::doEvent(event);
}

RECT verticalLayoutUI::getThumbRect(bool bUseNew) const
{
	if( (m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew) {
		if( m_iSepHeight >= 0 ) 
			return rect(m_rcNewPos.left, max(m_rcNewPos.bottom - m_iSepHeight, m_rcNewPos.top), 
			m_rcNewPos.right, m_rcNewPos.bottom);
		else 
			return rect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.right, 
			min(m_rcNewPos.top - m_iSepHeight, m_rcNewPos.bottom));
	}
	else {
		if( m_iSepHeight >= 0 ) 
			return rect(m_rcItem.left, max(m_rcItem.bottom - m_iSepHeight, m_rcItem.top), m_rcItem.right, 
			m_rcItem.bottom);
		else 
			return rect(m_rcItem.left, m_rcItem.top, m_rcItem.right, 
			min(m_rcItem.top - m_iSepHeight, m_rcItem.bottom));

	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

horizontalLayoutUI::horizontalLayoutUI() : m_iSepWidth(0), m_uButtonState(0), m_bImmMode(false)
{
	//TRACE_CLASS_BEGIN(this)
	ptLastMouse.x = ptLastMouse.y = 0;
	::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
}

PCWSTR horizontalLayoutUI::getClass() const
{
	return L"HorizontalLayoutUI";
}

LPVOID horizontalLayoutUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"HorizontalLayout") == 0 ) return static_cast<horizontalLayoutUI*>(this);
	return containerUI::getInterface(pstrName);
}

UINT horizontalLayoutUI::getControlFlags() const
{
	if( isEnabled() && m_iSepWidth != 0 ) return UIFLAG_SETCURSOR;
	else return 0;
}

void horizontalLayoutUI::setPos(RECT rc)
{
	controlUI::setPos(rc);
	rc = m_rcItem;

	// Adjust for inset
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	if( m_items.size() == 0) {
		processScrollBar(rc, 0, 0);
		return;
	}

	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) rc.right -= m_pVerticalScrollBar->getFixedWidth();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();

	// Determine the width of elements that are sizeable
	SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) 
		szAvailable.cx += m_pHorizontalScrollBar->getScrollRange();

	int nAdjustables = 0;
	int cxFixed = 0;
	int nEstimateNum = 0;
	for( int it1 = 0; it1 < m_items.size(); it1++ ) {
		controlUI* pControl = static_cast<controlUI*>(m_items[it1]);
		if( !pControl->isVisible() ) continue;
		if( pControl->isFloat() ) continue;
		SIZE sz = pControl->estimateSize(szAvailable);
		if( sz.cx == 0 ) {
			nAdjustables++;
		}
		else {
			if( sz.cx < pControl->getMinWidth() ) sz.cx = pControl->getMinWidth();
			if( sz.cx > pControl->getMaxWidth() ) sz.cx = pControl->getMaxWidth();
		}
		cxFixed += sz.cx +  pControl->getPadding().left + pControl->getPadding().right;
		nEstimateNum++;
	}
	cxFixed += (nEstimateNum - 1) * m_iChildPadding;

	int cxExpand = 0;
	if( nAdjustables > 0 ) cxExpand = max(0, (szAvailable.cx - cxFixed) / nAdjustables);
	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosX = rc.left;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) {
		iPosX -= m_pHorizontalScrollBar->getScrollPos();
	}
	int iAdjustable = 0;
	int cxFixedRemaining = cxFixed;
	for( int it2 = 0; it2 < m_items.size(); it2++ ) {
		controlUI* pControl = static_cast<controlUI*>(m_items[it2]);
		if( !pControl->isVisible() ) continue;
		if( pControl->isFloat() ) {
			setFloatPos(it2);
			continue;
		}
		RECT rcPadding = pControl->getPadding();
		szRemaining.cx -= rcPadding.left;
		SIZE sz = pControl->estimateSize(szRemaining);
		if( sz.cx == 0 ) {
			iAdjustable++;
			sz.cx = cxExpand;
			// Distribute remaining to last element (usually round-off left-overs)
			if( iAdjustable == nAdjustables ) {
				sz.cx = max(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
			}
			if( sz.cx < pControl->getMinWidth() ) sz.cx = pControl->getMinWidth();
			if( sz.cx > pControl->getMaxWidth() ) sz.cx = pControl->getMaxWidth();
		}
		else {
			if( sz.cx < pControl->getMinWidth() ) sz.cx = pControl->getMinWidth();
			if( sz.cx > pControl->getMaxWidth() ) sz.cx = pControl->getMaxWidth();

			cxFixedRemaining -= sz.cx;
		}

		sz.cy = pControl->getFixedHeight();
		if( sz.cy == 0 ) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
		if( sz.cy < 0 ) sz.cy = 0;
		if( sz.cy < pControl->getMinHeight() ) sz.cy = pControl->getMinHeight();
		if( sz.cy > pControl->getMaxHeight() ) sz.cy = pControl->getMaxHeight();

		RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.cy};
		pControl->setPos(rcCtrl);
		iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
		szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
	}

	// Process the scrollbar
	processScrollBar(rc, 0, 0);
}

void horizontalLayoutUI::doPostPaint(HDC hDC, const RECT& rcPaint)
{
	if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) {
		RECT rcSeparator = getThumbRect(true);
		renderEngine::drawColor(hDC, rcSeparator, 0xAA000000);
	}
}

void horizontalLayoutUI::setSepWidth(int iWidth)
{
	m_iSepWidth = iWidth;
}

int horizontalLayoutUI::getSepWidth() const
{
	return m_iSepWidth;
}

void horizontalLayoutUI::setSepImmMode(bool bImmediately)
{
	if( m_bImmMode == bImmediately ) return;
	if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
		m_pManager->removePostPaint(this);
	}

	m_bImmMode = bImmediately;
}

bool horizontalLayoutUI::isSepImmMode() const
{
	return m_bImmMode;
}

void horizontalLayoutUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"sepwidth") == 0 ) setSepWidth(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"sepimm") == 0 ) setSepImmMode(_tcscmp(pstrValue, L"true") == 0);
	else containerUI::setAttribute(pstrName, pstrValue);
}

void horizontalLayoutUI::doEvent(TEventUI& event)
{
	if( m_iSepWidth != 0 ) {
		if( event.Type == UIEVENT_BUTTONDOWN && isEnabled() )
		{
			RECT rcSeparator = getThumbRect(false);
			if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
				m_uButtonState |= UISTATE_CAPTURED;
				ptLastMouse = event.ptMouse;
				m_rcNewPos = m_rcItem;
				if( !m_bImmMode && m_pManager ) m_pManager->addPostPaint(this);
				return;
			}
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				m_uButtonState &= ~UISTATE_CAPTURED;
				m_rcItem = m_rcNewPos;
				if( !m_bImmMode && m_pManager ) m_pManager->removePostPaint(this);
				needParentUpdate();
				return;
			}
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				LONG cx = event.ptMouse.x - ptLastMouse.x;
				ptLastMouse = event.ptMouse;
				RECT rc = m_rcNewPos;
				if( m_iSepWidth >= 0 ) {
					if( cx > 0 && event.ptMouse.x < m_rcNewPos.right - m_iSepWidth ) return;
					if( cx < 0 && event.ptMouse.x > m_rcNewPos.right ) return;
					rc.right += cx;
					if( rc.right - rc.left <= getMinWidth() ) {
						if( m_rcNewPos.right - m_rcNewPos.left <= getMinWidth() ) return;
						rc.right = rc.left + getMinWidth();
					}
					if( rc.right - rc.left >= getMaxWidth() ) {
						if( m_rcNewPos.right - m_rcNewPos.left >= getMaxWidth() ) return;
						rc.right = rc.left + getMaxWidth();
					}
				}
				else {
					if( cx > 0 && event.ptMouse.x < m_rcNewPos.left ) return;
					if( cx < 0 && event.ptMouse.x > m_rcNewPos.left - m_iSepWidth ) return;
					rc.left += cx;
					if( rc.right - rc.left <= getMinWidth() ) {
						if( m_rcNewPos.right - m_rcNewPos.left <= getMinWidth() ) return;
						rc.left = rc.right - getMinWidth();
					}
					if( rc.right - rc.left >= getMaxWidth() ) {
						if( m_rcNewPos.right - m_rcNewPos.left >= getMaxWidth() ) return;
						rc.left = rc.right - getMaxWidth();
					}
				}

				rect rcInvalidate = getThumbRect(true);
				m_rcNewPos = rc;
				m_cxyFixed.cx = m_rcNewPos.right - m_rcNewPos.left;

				if( m_bImmMode ) {
					m_rcItem = m_rcNewPos;
					needParentUpdate();
				}
				else {
					rcInvalidate.join(getThumbRect(true));
					rcInvalidate.join(getThumbRect(false));
					if( m_pManager ) m_pManager->invalidate(rcInvalidate);
				}
				return;
			}
		}
		if( event.Type == UIEVENT_SETCURSOR )
		{
			RECT rcSeparator = getThumbRect(false);
			if( isEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
				return;
			}
		}
	}
	containerUI::doEvent(event);
}

RECT horizontalLayoutUI::getThumbRect(bool bUseNew) const
{
	if( (m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew) {
		if( m_iSepWidth >= 0 ) return rect(m_rcNewPos.right - m_iSepWidth, m_rcNewPos.top, m_rcNewPos.right, m_rcNewPos.bottom);
		else return rect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.left - m_iSepWidth, m_rcNewPos.bottom);
	}
	else {
		if( m_iSepWidth >= 0 ) return rect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
		else return rect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

tileLayoutUI::tileLayoutUI() : m_nColumns(2)
{
	//TRACE_CLASS_BEGIN(this)
}

PCWSTR tileLayoutUI::getClass() const
{
	return L"TileLayoutUI";
}

LPVOID tileLayoutUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"TileLayout") == 0 ) return static_cast<tileLayoutUI*>(this);
	return containerUI::getInterface(pstrName);
}

int tileLayoutUI::getColumns() const
{
	return m_nColumns;
}

void tileLayoutUI::setColumns(int nCols)
{
	if( nCols <= 0 ) return;
	m_nColumns = nCols;
	needUpdate();
}

void tileLayoutUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"columns") == 0 ) setColumns(_ttoi(pstrValue));
	else containerUI::setAttribute(pstrName, pstrValue);
}

void tileLayoutUI::setPos(RECT rc)
{
	controlUI::setPos(rc);
	rc = m_rcItem;

	// Adjust for inset
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	if( m_items.size() == 0) {
		processScrollBar(rc, 0, 0);
		return;
	}

	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) rc.right -= m_pVerticalScrollBar->getFixedWidth();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();

	// Position the elements
	int cyNeeded = 0;
	int cxWidth = (rc.right - rc.left) / m_nColumns;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) 
		cxWidth = (rc.right - rc.left + m_pHorizontalScrollBar->getScrollRange() ) / m_nColumns; ;

	int cyHeight = 0;
	int iCount = 0;
	POINT ptTile = { rc.left, rc.top };
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) {
		ptTile.y -= m_pVerticalScrollBar->getScrollPos();
	}
	int iPosX = rc.left;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) {
		iPosX -= m_pHorizontalScrollBar->getScrollPos();
		ptTile.x = iPosX;
	}
	for( int it1 = 0; it1 < m_items.size(); it1++ ) {
		controlUI* pControl = static_cast<controlUI*>(m_items[it1]);
		if( !pControl->isVisible() ) continue;
		if( pControl->isFloat() ) {
			setFloatPos(it1);
			continue;
		}

		// Determine size
		RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
		if( (iCount % m_nColumns) == 0 )
		{
			int iIndex = iCount;
			for( int it2 = it1; it2 < m_items.size(); it2++ ) {
				controlUI* pLineControl = static_cast<controlUI*>(m_items[it2]);
				if( !pLineControl->isVisible() ) continue;
				if( pLineControl->isFloat() ) continue;

				RECT rcPadding = pLineControl->getPadding();
				SIZE szAvailable = { rcTile.right - rcTile.left - rcPadding.left - rcPadding.right, 9999 };
				if( iIndex == iCount || (iIndex + 1) % m_nColumns == 0 ) {
					szAvailable.cx -= m_iChildPadding / 2;
				}
				else {
					szAvailable.cx -= m_iChildPadding;
				}

				if( szAvailable.cx < pControl->getMinWidth() ) szAvailable.cx = pControl->getMinWidth();
				if( szAvailable.cx > pControl->getMaxWidth() ) szAvailable.cx = pControl->getMaxWidth();

				SIZE szTile = pLineControl->estimateSize(szAvailable);
				if( szTile.cx < pControl->getMinWidth() ) szTile.cx = pControl->getMinWidth();
				if( szTile.cx > pControl->getMaxWidth() ) szTile.cx = pControl->getMaxWidth();
				if( szTile.cy < pControl->getMinHeight() ) szTile.cy = pControl->getMinHeight();
				if( szTile.cy > pControl->getMaxHeight() ) szTile.cy = pControl->getMaxHeight();

				cyHeight = max(cyHeight, szTile.cy + rcPadding.top + rcPadding.bottom);
				if( (++iIndex % m_nColumns) == 0) break;
			}
		}

		RECT rcPadding = pControl->getPadding();

		rcTile.left += rcPadding.left + m_iChildPadding / 2;
		rcTile.right -= rcPadding.right + m_iChildPadding / 2;
		if( (iCount % m_nColumns) == 0 ) {
			rcTile.left -= m_iChildPadding / 2;
		}

		if( ( (iCount + 1) % m_nColumns) == 0 ) {
			rcTile.right += m_iChildPadding / 2;
		}

		// Set position
		rcTile.top = ptTile.y + rcPadding.top;
		rcTile.bottom = ptTile.y + cyHeight;

		SIZE szAvailable = { rcTile.right - rcTile.left, rcTile.bottom - rcTile.top };
		SIZE szTile = pControl->estimateSize(szAvailable);
		if( szTile.cx == 0 ) szTile.cx = szAvailable.cx;
		if( szTile.cy == 0 ) szTile.cy = szAvailable.cy;
		if( szTile.cx < pControl->getMinWidth() ) szTile.cx = pControl->getMinWidth();
		if( szTile.cx > pControl->getMaxWidth() ) szTile.cx = pControl->getMaxWidth();
		if( szTile.cy < pControl->getMinHeight() ) szTile.cy = pControl->getMinHeight();
		if( szTile.cy > pControl->getMaxHeight() ) szTile.cy = pControl->getMaxHeight();
		RECT rcPos = {(rcTile.left + rcTile.right - szTile.cx) / 2, (rcTile.top + rcTile.bottom - szTile.cy) / 2,
			(rcTile.left + rcTile.right - szTile.cx) / 2 + szTile.cx, (rcTile.top + rcTile.bottom - szTile.cy) / 2 + szTile.cy};
		pControl->setPos(rcPos);

		if( (++iCount % m_nColumns) == 0 ) {
			ptTile.x = iPosX;
			ptTile.y += cyHeight + m_iChildPadding;
			cyHeight = 0;
		}
		else {
			ptTile.x += cxWidth;
		}
		cyNeeded = rcTile.bottom - rc.top;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) cyNeeded += m_pVerticalScrollBar->getScrollPos();
	}

	// Process the scrollbar
	processScrollBar(rc, 0, cyNeeded);
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

dialogLayoutUI::dialogLayoutUI() : m_aModes(sizeof(STRETCHMODE))
{
	//TRACE_CLASS_BEGIN(this)
}

PCWSTR dialogLayoutUI::getClass() const
{
	return L"DialogLayoutUI";
}

LPVOID dialogLayoutUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"DialogLayout") == 0 ) return static_cast<dialogLayoutUI*>(this);
	return containerUI::getInterface(pstrName);
}

void dialogLayoutUI::setStretchMode(controlUI* pControl, UINT uMode)
{
	STRETCHMODE mode;
	mode.pControl = pControl;
	mode.uMode = uMode;
	RECT rcPos = { pControl->getFixedXY().cx, pControl->getFixedXY().cy, pControl->getFixedXY().cx + pControl->getFixedWidth(), 
		pControl->getFixedXY().cy + pControl->getFixedHeight() };
	mode.rcItem = rcPos;
	//m_aModes.Add(&mode);
	m_aModes.push_back(mode);
}

SIZE dialogLayoutUI::estimateSize(SIZE szAvailable)
{
	RECT rcDialog = recalcArea();
	SIZE rc = { rcDialog.right - rcDialog.left, rcDialog.bottom - rcDialog.top };
	if( m_pVerticalScrollBar != NULL ) {
		if( rc.cx > szAvailable.cx ) rc.cx = szAvailable.cx;
		if( rc.cy > szAvailable.cy ) rc.cy = szAvailable.cy;
	}
	return rc;
}

void dialogLayoutUI::setPos(RECT rc)
{
	controlUI::setPos(rc);
	rc = m_rcItem;

	if( m_items.size() == 0) return;

	RECT rcDialog = recalcArea();

	// Adjust for inset
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) {
		rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();
		RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->getFixedHeight()};
		m_pHorizontalScrollBar->setPos(rcScrollBarPos);
	}

	if( m_pVerticalScrollBar != NULL ) {
		bool bScrollBar = m_pVerticalScrollBar->isVisible();
		if( bScrollBar ) rc.right -= m_pVerticalScrollBar->getFixedWidth();
		processScrollBar(rc, 0, rcDialog.bottom - rcDialog.top);
		if( bScrollBar != m_pVerticalScrollBar->isVisible() ) {
			if( bScrollBar ) rc.right += m_pVerticalScrollBar->getFixedWidth();
			else rc.right -= m_pVerticalScrollBar->getFixedWidth();
		}
	}

	// Determine how "scaled" the dialog is compared to the original size
	int cxDiff = (rc.right - rc.left) - (rcDialog.right - rcDialog.left);
	int cyDiff = (rc.bottom - rc.top) - (rcDialog.bottom - rcDialog.top);
	if( cxDiff < 0 ) cxDiff = 0;
	if( cyDiff < 0 ) cyDiff = 0;
	// Stretch each control
	// Controls are coupled in "groups", which determine a scaling factor.
	// A "line" indicator is used to apply the same scaling to a new group of controls.
	int nCount, cxStretch, cyStretch, cxMove, cyMove;
	for( int i = 0; i < m_aModes.size(); i++ ) {
		STRETCHMODE* pItem = &m_aModes[i];
		if( !pItem->pControl->isVisible() ) continue;
		if( pItem->pControl->isFloat() ) {
			setFloatPos(i);
			continue;
		}

		if( i == 0 || (pItem->uMode & UISTRETCH_NEWGROUP) != 0 ) {
			nCount = 0;
			for( int j = i + 1; j < m_aModes.size(); j++ ) {
				STRETCHMODE* pNext = &m_aModes[j];
				if( (pNext->uMode & (UISTRETCH_NEWGROUP | UISTRETCH_NEWLINE)) != 0 ) break;
				if( (pNext->uMode & (UISTRETCH_SIZE_X | UISTRETCH_SIZE_Y)) != 0 ) nCount++;
			}
			if( nCount == 0 ) nCount = 1;
			cxStretch = cxDiff / nCount;
			cyStretch = cyDiff / nCount;
			cxMove = 0;
			cyMove = 0;
		}
		if( (pItem->uMode & UISTRETCH_NEWLINE) != 0 ) {
			cxMove = 0;
			cyMove = 0;
		}
		RECT rcPos = pItem->rcItem;
		int iScrollPosY = 0;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible() ) iScrollPosY = m_pVerticalScrollBar->getScrollPos();
		int iScrollPosX = 0;
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible() ) iScrollPosX = m_pHorizontalScrollBar->getScrollPos();

		::OffsetRect(&rcPos, rc.left - iScrollPosX, rc.top - iScrollPosY);

		if( (pItem->uMode & UISTRETCH_MOVE_X) != 0 ) ::OffsetRect(&rcPos, cxMove, 0);
		if( (pItem->uMode & UISTRETCH_MOVE_Y) != 0 ) ::OffsetRect(&rcPos, 0, cyMove);
		if( (pItem->uMode & UISTRETCH_SIZE_X) != 0 ) rcPos.right += cxStretch;
		if( (pItem->uMode & UISTRETCH_SIZE_Y) != 0 ) rcPos.bottom += cyStretch;
		if( (pItem->uMode & (UISTRETCH_SIZE_X | UISTRETCH_SIZE_Y)) != 0 ) {
			cxMove += cxStretch;
			cyMove += cyStretch;
		}
		if( rcPos.right - rcPos.left > pItem->pControl->getMaxWidth() ) rcPos.right = rc.left + pItem->pControl->getMaxWidth();
		if( rcPos.right - rcPos.left < pItem->pControl->getMinWidth() ) rcPos.right = rc.left + pItem->pControl->getMinWidth();
		if( rcPos.bottom - rcPos.top > pItem->pControl->getMaxHeight() ) rcPos.bottom = rc.top + pItem->pControl->getMaxHeight();
		if( rcPos.bottom - rcPos.top < pItem->pControl->getMinHeight() ) rcPos.bottom = rc.top + pItem->pControl->getMinHeight();

		pItem->pControl->setPos(rcPos);
	}
}

RECT dialogLayoutUI::recalcArea()
{
	//CStdValArray aTempModes( sizeof(STRETCHMODE), m_items.GetSize() );
	std::vector<STRETCHMODE> aTempModes;
	for( int it = 0; it < m_items.size(); it++ ) {      
		controlUI* pControl = static_cast<controlUI*>(m_items[it]);

		bool bFound = false;
		int i = 0;
		for( ; !bFound && i < m_aModes.size(); i++ ) {
			if( (m_aModes[i]).pControl == pControl ) {
				bFound = true;
				break;
			}
		}

		if( bFound ) {
			RECT rcPos = { pControl->getFixedXY().cx, pControl->getFixedXY().cy, pControl->getFixedXY().cx + pControl->getFixedWidth(), 
				pControl->getFixedXY().cy + pControl->getFixedHeight() };
			//static_cast<STRETCHMODE*>(m_aModes[i])->rcItem = rcPos;
			//aTempModes.Add(m_aModes[i]);
			m_aModes[i].rcItem = rcPos;
			aTempModes.push_back(m_aModes[i]);
		}
		else {
			STRETCHMODE mode;
			mode.pControl = pControl;
			mode.uMode = UISTRETCH_NEWGROUP;
			RECT rcPos = { pControl->getFixedXY().cx, pControl->getFixedXY().cy, pControl->getFixedXY().cx + pControl->getFixedWidth(), 
				pControl->getFixedXY().cy + pControl->getFixedHeight() };
			mode.rcItem = rcPos;
			//aTempModes.Add(&mode);
			aTempModes.push_back(mode);
		}
	}

	//LPBYTE bTempBuffer[sizeof(CStdValArray)];
	//::CopyMemory( (LPVOID)&bTempBuffer, (LPCVOID)&m_aModes, sizeof(CStdValArray) );
	//::CopyMemory( (LPVOID)&m_aModes, (LPCVOID)&aTempModes, sizeof(CStdValArray) );
	//::CopyMemory( (LPVOID)&aTempModes, (LPCVOID)&bTempBuffer, sizeof(CStdValArray) );
	std::swap(m_aModes, aTempModes);

	// Figure out the actual size of the dialog so we can add proper scrollbars later
	rect rcDialog(9999, 9999, 0,0);
	for( int i = 0; i < m_aModes.size(); i++ ) {
		STRETCHMODE* pItem = &m_aModes[i];
		if( !pItem->pControl->isVisible() ) continue;
		if( pItem->pControl->isFloat() ) continue;
		const RECT& rcPos = m_aModes[i].rcItem;
		rcDialog.join(rcPos);
	}
	for( int j = 0; j < m_aModes.size(); j++ ) {
		RECT& rcPos = m_aModes[j].rcItem;
		::OffsetRect(&rcPos, -rcDialog.left, -rcDialog.top);
	}

	return rcDialog;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

tabLayoutUI::tabLayoutUI() : m_iCurSel(-1)
{
	//TRACE_CLASS_BEGIN(this)
}

PCWSTR tabLayoutUI::getClass() const
{
	return L"TabLayoutUI";
}

LPVOID tabLayoutUI::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"TabLayout") == 0 ) return static_cast<tabLayoutUI*>(this);
	return containerUI::getInterface(pstrName);
}

bool tabLayoutUI::add(controlUI* pControl)
{
	bool ret = containerUI::add(pControl);
	if( !ret ) return ret;

	if(m_iCurSel == -1 && pControl->isVisible())
	{
		m_iCurSel = getItemIndex(pControl);
	}
	else
	{
		pControl->setVisible(false);
	}

	return ret;
}

bool tabLayoutUI::addAt(controlUI* pControl, int iIndex)
{
	bool ret = containerUI::addAt(pControl, iIndex);
	if( !ret ) return ret;

	if(m_iCurSel == -1 && pControl->isVisible())
	{
		m_iCurSel = getItemIndex(pControl);
	}
	else if( m_iCurSel != -1 && iIndex <= m_iCurSel )
	{
		m_iCurSel += 1;
	}
	else
	{
		pControl->setVisible(false);
	}

	return ret;
}

bool tabLayoutUI::remove(controlUI* pControl)
{
	if( pControl == NULL) return false;

	int index = getItemIndex(pControl);
	bool ret = containerUI::remove(pControl);
	if( !ret ) return false;

	if( m_iCurSel == index)
	{
		if( getCount() > 0 ) getItemAt(0)->setVisible(true);
		needParentUpdate();
	}
	else if( m_iCurSel > index )
	{
		m_iCurSel -= 1;
	}

	return ret;
}

void tabLayoutUI::removeAll()
{
	m_iCurSel = -1;
	containerUI::removeAll();
	needParentUpdate();
}

int tabLayoutUI::getCurSel() const
{
	return m_iCurSel;
}

bool tabLayoutUI::selectItem(int iIndex)
{
	if( iIndex < 0 || iIndex >= m_items.size() ) return false;
	if( iIndex == m_iCurSel ) return true;

	int iOldSel = m_iCurSel;
	m_iCurSel = iIndex;
	for( int it = 0; it < m_items.size(); it++ )
	{
		if( it == iIndex ) {
			getItemAt(it)->setVisible(true);
			getItemAt(it)->setFocus();
		}
		else getItemAt(it)->setVisible(false);
	}
	needParentUpdate();

	if( m_pManager != NULL ) {
		m_pManager->setNextTabControl();
		m_pManager->sendNotify(this, L"tabselect", m_iCurSel, iOldSel);
	}
	return true;
}

void tabLayoutUI::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"selectedid") == 0 ) selectItem(_ttoi(pstrValue));
	return containerUI::setAttribute(pstrName, pstrValue);
}

void tabLayoutUI::setPos(RECT rc)
{
	controlUI::setPos(rc);
	rc = m_rcItem;

	// Adjust for inset
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	for( int it = 0; it < m_items.size(); it++ ) {
		controlUI* pControl = static_cast<controlUI*>(m_items[it]);
		if( !pControl->isVisible() ) continue;
		if( pControl->isFloat() ) {
			setFloatPos(it);
			continue;
		}

		if( it != m_iCurSel ) continue;

		RECT rcPadding = pControl->getPadding();
		rc.left += rcPadding.left;
		rc.top += rcPadding.top;
		rc.right -= rcPadding.right;
		rc.bottom -= rcPadding.bottom;

		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

		SIZE sz = pControl->estimateSize(szAvailable);
		if( sz.cx == 0 ) {
			sz.cx = max(0, szAvailable.cx);
		}
		if( sz.cx < pControl->getMinWidth() ) sz.cx = pControl->getMinWidth();
		if( sz.cx > pControl->getMaxWidth() ) sz.cx = pControl->getMaxWidth();

		if(sz.cy == 0) {
			sz.cy = max(0, szAvailable.cy);
		}
		if( sz.cy < pControl->getMinHeight() ) sz.cy = pControl->getMinHeight();
		if( sz.cy > pControl->getMaxHeight() ) sz.cy = pControl->getMaxHeight();

		RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy};
		pControl->setPos(rcCtrl);
	}
}


}