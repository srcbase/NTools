#include "wucContainer.h"
#include "wucBase.h"


namespace wuc {

containerUC::containerUC(void)
	: controlUC()
	, m_bMouseChildEnabled(true)
	, m_bAutoDestroy(true), m_bDelayedDestroy(true), m_bDelayedFree(false)
	, m_pVerticalScrollBar(nullptr), m_pHorizontalScrollBar(nullptr)
	, m_iChildPadding(0)
	, m_bScrollProcess(false)
	, m_bAutoHideScroll(false), m_bDriftScroll(false)
#if defined(GTC_BOOST_THREAD)

	, m_bReadLocked(false), m_bWriteLocked(false)
#endif
{
	::ZeroMemory(&m_rcInset, sizeof(m_rcInset));
	//m_swItems.setDescription(L"container_items");
}


containerUC::~containerUC(void)
{
	m_bDelayedDestroy = false;
	m_bDelayedFree = false;
	removeAll();
	if( m_pVerticalScrollBar ) delete m_pVerticalScrollBar;
	if( m_pHorizontalScrollBar ) delete m_pHorizontalScrollBar;
	//gtc::app::writeVSLogs(L"free containerUC <%s>", m_wsName.c_str());
}

PCWSTR containerUC::getClass() const
{
	return L"containerUC";
}

LPVOID containerUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"IContainer") == 0 ) return static_cast<IContainerUC*>(this);
	else if( _tcscmp(pstrName, L"Container") == 0 ) return static_cast<containerUC*>(this);
	return controlUC::getInterface(pstrName);
}

RECT containerUC::getInset() const
{
	return m_rcInset;
}
void containerUC::setInset(RECT rcInset)
{
	m_rcInset = rcInset;
	needUpdate();
}
void containerUC::setInset(LONG left, LONG top, LONG right, LONG bottom)
{
	RECT rcInset = {left, top, right, bottom};
	setInset(rcInset);
}
int containerUC::getChildPadding() const
{
	return m_iChildPadding;
}
void containerUC::setChildPadding(int iPadding)
{
	m_iChildPadding = iPadding;
	needUpdate();
}
bool containerUC::isMouseChildEnabled() const
{
	return m_bMouseChildEnabled;
}
void containerUC::setMouseChildEnabled(bool bEnable /*= true*/)
{
	m_bMouseChildEnabled = bEnable;
}
bool containerUC::isAutoDestroy() const
{
	return m_bAutoDestroy;
}
void containerUC::setAutoDestroy(bool bAuto)
{
	m_bAutoDestroy = bAuto;
}
bool containerUC::isDelayedFree() const
{
	return m_bDelayedFree;
}
void containerUC::setDelayedFree(bool bDelayed)
{
	m_bDelayedFree = bDelayed;
}
bool containerUC::isDelayedDestroy() const
{
	return m_bDelayedDestroy;
}
void containerUC::setDelayedDestroy(bool bDelayed)
{
	m_bDelayedDestroy = bDelayed;
}

void containerUC::setVisible(bool bVisible /*= true*/)
{
	if(m_bVisible == bVisible) return;
	controlUC::setVisible(bVisible);
	//m_swItems.waitToRead(L"containerUC::setVisible");
	READ_LOCK
	for (ControlArrayType::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		(*iter)->setInternVisible(isVisible());
	}
	//m_swItems.done(L"containerUC::setVisible");
}
void containerUC::setInternVisible(bool bVisible /*= true*/)
{
	controlUC::setInternVisible(bVisible);
	if(m_items.size() == 0) return;
	//m_swItems.waitToRead(L"containerUC::setInternVisible");
	READ_LOCK
	for (ControlArrayType::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		(*iter)->setInternVisible(isVisible());
	}
	//m_swItems.done(L"containerUC::setInternVisible");
}
void containerUC::setMouseEnabled(bool bEnabled /*= true*/)
{
	if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->setMouseEnabled(bEnabled);
	if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->setMouseEnabled(bEnabled);
	controlUC::setMouseEnabled(bEnabled);
}

void containerUC::setManager(drawManager *pManager, controlUC *pParent, bool bInit /*= true*/)
{
	//m_swItems.waitToRead(L"containerUC::setManager");
	READ_LOCK
	for (ControlArrayType::const_iterator iter = m_items.cbegin(); iter != m_items.cend(); ++iter )
	{
		(*iter)->setManager(pManager, this, bInit);
	}
	//m_swItems.done(L"containerUC::setManager");
	if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->setManager(pManager, this, bInit);
	if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->setManager(pManager, this, bInit);
	controlUC::setManager(pManager, pParent, bInit);
}

controlUC* containerUC::findControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	// Check if this guy is valid
	if( (uFlags & UCFIND_VISIBLE) != 0 && !isVisible() ) return NULL;
	if( (uFlags & UCFIND_ENABLED) != 0 && !isEnabled() ) return NULL;
	if( (uFlags & UCFIND_HITTEST) != 0 ) {
		if( !::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData))) ) return NULL;
		if( !m_bMouseChildEnabled ) {
			controlUC* pResult = NULL;
			if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->findControl(Proc, pData, uFlags);
			if( pResult == NULL && m_pHorizontalScrollBar != NULL ) pResult = m_pHorizontalScrollBar->findControl(Proc, pData, uFlags);
			if( pResult == NULL ) pResult = controlUC::findControl(Proc, pData, uFlags);
			return pResult;
		}
	}

	controlUC* pResult = NULL;
	if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->findControl(Proc, pData, uFlags);
	if( pResult == NULL && m_pHorizontalScrollBar != NULL ) pResult = m_pHorizontalScrollBar->findControl(Proc, pData, uFlags);
	if( pResult != NULL ) return pResult;

	if( (uFlags & UCFIND_ME_FIRST) != 0 ) {
		controlUC* pControl = controlUC::findControl(Proc, pData, uFlags);
		if( pControl != NULL ) return pControl;
	}
	RECT rc = m_rcItem;
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;
	if( m_pVerticalScrollBar && (m_bAutoHideScroll || m_pVerticalScrollBar->isVisible()) ) rc.right -= m_pVerticalScrollBar->getFixedWidth();
	if( m_pHorizontalScrollBar && (m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible()) ) rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();
	if( (uFlags & UCFIND_TOP_FIRST) != 0 ) {
		//m_swItems.waitToRead(L"containerUC::findControl");
		READ_LOCK	// 嵌套锁--〉可能会失败
		for( int it = m_items.size() - 1; it >= 0; it-- ) {
			controlUC* pControl = m_items[it]->findControl(Proc, pData, uFlags);
			if( pControl != NULL ) {
				if( (uFlags & UCFIND_HITTEST) != 0 && !pControl->isFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
					continue;
				else 
				{
					//m_swItems.done(L"containerUC::findControl");
					return pControl;
				}
			}
		}
		//m_swItems.done(L"containerUC::findControl");
	}
	else {
		//m_swItems.waitToRead(L"containerUC::findControl_2A");
		READ_LOCK
		for( std::size_t it = 0; it < m_items.size(); it++ ) {
			controlUC* pControl = m_items[it]->findControl(Proc, pData, uFlags);
			if( pControl != NULL ) {
				if( (uFlags & UCFIND_HITTEST) != 0 && !pControl->isFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
					continue;
				else 
				{
					//m_swItems.done(L"containerUC::findControl_2");
					return pControl;
				}
			} 
		}
		//m_swItems.done(L"containerUC::findControl_2");
	}

	if( pResult == NULL ) pResult = controlUC::findControl(Proc, pData, uFlags);
	return pResult;
}

bool containerUC::existsControl(controlUC *pControl)
{
	if(!pControl) return false;
	if(controlUC::existsControl(pControl)) return true;
	bool bRet = false;
	if (m_pHorizontalScrollBar)
	{
		bRet = m_pHorizontalScrollBar->existsControl(pControl);
		if(bRet) return true;
	}
	if (m_pVerticalScrollBar)
	{
		bRet = m_pVerticalScrollBar->existsControl(pControl);
		if(bRet) return true;
	}
	//m_swItems.waitToRead(L"containerUC::existsControl");
	READ_LOCK
	for( std::size_t it = 0; it < m_items.size(); it++ ) {
		bRet = m_items[it]->existsControl(pControl);
		if( bRet )
		{
			break;
		}
	}
	//m_swItems.done(L"containerUC::existsControl");
	return bRet;
}
void containerUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	// DAI
	if( _tcscmp(pstrName, _T("inset")) == 0 ) {
		RECT rcInset = { 0 };
		LPTSTR pstr = NULL;
		rcInset.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcInset.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcInset.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setInset(rcInset);
	}
	else if( _tcscmp(pstrName, _T("mousechild")) == 0 ) setMouseChildEnabled(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("vscrollbar")) == 0 ) {
		enableScrollBar(_tcscmp(pstrValue, _T("true")) == 0, getHorizontalScrollBar() != NULL);
	}
	else if( _tcscmp(pstrName, _T("vscrollbarstyle")) == 0 ) {
		enableScrollBar(true, getHorizontalScrollBar() != NULL);
		if( getVerticalScrollBar() ) getVerticalScrollBar()->applyAttributeList(pstrValue);
	}
	else if( _tcscmp(pstrName, _T("hscrollbar")) == 0 ) {
		enableScrollBar(getVerticalScrollBar() != NULL, _tcscmp(pstrValue, _T("true")) == 0);
	}
	else if( _tcscmp(pstrName, _T("hscrollbarstyle")) == 0 ) {
		enableScrollBar(getVerticalScrollBar() != NULL, true);
		if( getHorizontalScrollBar() ) getHorizontalScrollBar()->applyAttributeList(pstrValue);
	}
	else if( _tcscmp(pstrName, _T("childpadding")) == 0 ) setChildPadding(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, _T("autohidescroll")) == 0 ) setAutoHideScroll(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("driftscroll")) == 0 ) setDriftScroll(_tcscmp(pstrValue, _T("true")) == 0);
	else __super::setAttribute(pstrName, pstrValue);
}

void containerUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else __super::Event(event);
		return;
	}
	if (event.Type == BEUCEventMouseEnter)
	{
		if(m_bAutoHideScroll)
		{
			if(m_pVerticalScrollBar) m_pVerticalScrollBar->setVisible(true);
			if(m_pHorizontalScrollBar) m_pHorizontalScrollBar->setVisible(true);
			return;
		}
	}
	if (event.Type == BEUCEventMouseLeave)
	{
		if(m_bAutoHideScroll)
		{
			if(m_pVerticalScrollBar) m_pVerticalScrollBar->setVisible(false);
			if(m_pHorizontalScrollBar) m_pHorizontalScrollBar->setVisible(false);
			SEND_LOG(L"ERROR AAAAAAAAAAAAAAAAAAAAAAAA");
			return;
		}
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
	if( m_pVerticalScrollBar != NULL && (m_bAutoHideScroll || m_pVerticalScrollBar->isVisible()) && m_pVerticalScrollBar->isEnabled() )
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
			switch( LOWORD(event.wParam) ) {
			case SB_LINEUP:
				lineUp();
				return;
			case SB_LINEDOWN:
				//SEND_LOG(L"containerUC::Event BEUCEventScrollWheel_V SB_LINEDOWN");
				lineDown();
				return;
			}
		}
	}
	else if( m_pHorizontalScrollBar != NULL && (m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible()) && m_pHorizontalScrollBar->isEnabled() ) {
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
				SEND_LOG(L"containerUC::Event BEUCEventScrollWheel_H SB_LINEDOWN");
				lineRight();
				return;
			}
		}
	}
	__super::Event(event);
}

void containerUC::paint(HDC hdc, const RECT &rcPaint)
{
	RECT rcTemp = { 0 };
	if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;
	//if(rectWidth(rcPaint) == 332)
	//SEND_LOG(L"ERROR containerUC::paint class:<%s> name:<%s> rcPaint:<%s>", getClass(), getName().c_str(), gtc::format::rect(rcPaint, true).c_str());
	renderClip clip;
	renderClip::generateClip(hdc, rcTemp, clip);
	controlUC::paint(hdc, rcPaint);
	
	if( m_items.size() > 0 ) {
		RECT rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		if( m_pVerticalScrollBar 
			&& (/*m_bAutoHideScroll || 不能取消注释,会造成元素右侧被切掉m_pVerticalScrollBar->getFixedWidth()长度*/m_pVerticalScrollBar->isVisible())
			&& !m_bDriftScroll) rc.right -= m_pVerticalScrollBar->getFixedWidth();
		if( m_pHorizontalScrollBar 
			&& (/*m_bAutoHideScroll || */m_pHorizontalScrollBar->isVisible())
			&& !m_bDriftScroll) rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();

		if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
			//m_swItems.waitToRead(L"containerUC::paint");
			READ_LOCK
			for( std::size_t it = 0; it < m_items.size(); it++ ) {
				controlUC* pControl = m_items[it];
				if( !pControl->isVisible() ) continue;
				if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->getPosition()) ) continue;
				if( pControl ->isFloat() ) {
					if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->getPosition()) ) continue;
					pControl->paint(hdc, rcPaint);
				}
			}
			//m_swItems.done(L"containerUC::paint");
		}
		else {
			renderClip childClip;
			renderClip::generateClip(hdc, rcTemp, childClip);
			//m_swItems.waitToRead(L"containerUC::paint_2");
			READ_LOCK
			for( std::size_t it = 0; it < m_items.size(); it++ ) {
				controlUC* pControl = m_items[it];
				if( !pControl->isVisible() ) continue;
				if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->getPosition()) ) continue;
				if( pControl ->isFloat() ) {
					if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->getPosition()) ) continue;
					renderClip::useOldClipBegin(hdc, childClip);
					pControl->paint(hdc, rcPaint);
					renderClip::useOldClipEnd(hdc, childClip);
				}
				else {
					if( !::IntersectRect(&rcTemp, &rc, &pControl->getPosition()) ) continue;
					pControl->paint(hdc, rcPaint);
				}
			}
			//m_swItems.done(L"containerUC::paint_2");
		}
	}

	if( m_pVerticalScrollBar != NULL && (m_bAutoHideScroll || m_pVerticalScrollBar->isVisible()) ) {
		if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->getPosition()) ) {
			m_pVerticalScrollBar->paint(hdc, rcPaint);
		}
	}

	if( m_pHorizontalScrollBar != NULL && (m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible()) ) {
		if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->getPosition()) ) {
			m_pHorizontalScrollBar->paint(hdc, rcPaint);
		}
	}
}


controlUC* containerUC::getItemAt(int iIndex) const
{
	controlUC *pControl = nullptr;
	//m_swItems.waitToRead(L"containerUC::getItemAt");
	if( iIndex >= 0 && iIndex < int(m_items.size()) )
		pControl = m_items[iIndex];
	//m_swItems.done(L"containerUC::getItemAt");
	return pControl;
}

int containerUC::getItemIndex(controlUC* pControl) const 
{
	int iIndex = -1;
	//m_swItems.waitToRead(L"containerUC::getItemIndex");
	READ_LOCK
	for( int it = 0; it < int(m_items.size()); it++ ) {
		if( m_items[it] == pControl ) {
			iIndex = it;
			break;
		}
	}
	//m_swItems.done(L"containerUC::getItemIndex");
	return iIndex;
}

bool containerUC::setItemIndex(controlUC* pControl, int iIndex)
{
	bool bRet = false;
	//m_swItems.waitToWrite(L"containerUC::setItemIndex");
	WRITE_LOCK
	for (ControlArrayType::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		if (*iter == pControl)
		{
			needUpdate();
			m_items.erase(iter);
			m_items.insert(m_items.begin()+iIndex, pControl);
			bRet = true;
			break;
		}
	}
	//m_swItems.done(L"containerUC::setItemIndex");
	return bRet;
}

int containerUC::getCount() const
{
	return m_items.size();
}

bool containerUC::add(controlUC* pControl)
{
	if( pControl == NULL) return false;

	if( m_pManager != NULL ) m_pManager->initControls(pControl, this);
	if( isVisible() ) needUpdate();
	else pControl->setInternVisible(false);
	//return m_items.Add(pControl);  
	//m_swItems.waitToWrite(L"containerUC::add");
	WRITE_LOCK
	m_items.push_back(pControl);
	//m_swItems.done(L"containerUC::add");
	return true;
}

bool containerUC::addAt(controlUC* pControl, int iIndex)
{
	if( pControl == NULL) return false;

	if( m_pManager ) m_pManager->initControls(pControl, this);
	if( isVisible() ) needUpdate();
	else pControl->setInternVisible(false);
	//return m_items.InsertAt(iIndex, pControl);
	//m_swItems.waitToWrite(L"containerUC::addAt");
	WRITE_LOCK
	m_items.insert(m_items.begin()+iIndex, pControl);
	//m_swItems.done(L"containerUC::addAt");
	return true;
}

bool containerUC::remove(controlUC* pControl)
{
	if( pControl == NULL) return false;
	bool bRet = false;
	bool isDelayedCleanup = false;
	WRITE_LOCK
	ControlArrayType::iterator iter = std::find(m_items.begin(), m_items.end(), pControl);
	if (iter != m_items.end())
	{
		needUpdate();
		if(m_bDelayedFree)
		{
			if(m_pManager) m_pManager->addDelayedFree(pControl);
		}
		else if (m_bAutoDestroy)
		{
			if(m_bDelayedDestroy && m_pManager) {
				isDelayedCleanup = true;
				m_pManager->addDelayedCleanup(pControl);	// S:1
			}
			else delete pControl;
		}
		SEND_LOG(L"containerUC::remove %08X class:<%s> name:<%s>", pControl, pControl->getClass(), pControl->getName().c_str());
		//m_swItems.waitToWrite(L"containerUC::remove");
		m_items.erase(iter);	// S:2
		//m_swItems.done(L"containerUC::remove");
		if(isDelayedCleanup)
			::SendMessage(m_pManager->getPaintWindow(), WM_APP + 1, 0, 0L);	// 本来在addDelayedCleanup中执行，如果在S1
																			// 中释放了pControl，而在S2前执行了别的操作且
																			// 用到了m_items中的pControl则就崩溃了
		bRet = true;
	}
	//for (std::vector<controlUC *>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	//{
	//	if (*iter == pControl)
	//	{
	//		needUpdate();
	//		if (m_bAutoDestroy)
	//		{
	//			if(m_bDelayedDestroy && m_pManager) m_pManager->addDelayedCleanup(pControl);
	//			else delete pControl;
	//		}
	//		m_items.erase(iter);
	//		return true;
	//	}
	//}


	//for( int it = 0; it < m_items.GetSize(); it++ ) {
	//	if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
	//		NeedUpdate();
	//		if( m_bAutoDestroy ) {
	//			if( m_bDelayedDestroy && m_pManager ) m_pManager->AddDelayedCleanup(pControl);             
	//			else delete pControl;
	//		}
	//		return m_items.Remove(it);
	//	}
	//}
	return bRet;
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
	//m_swItems.waitToWrite(L"containerUC::removeAll");
	bool isDelayedCleanup = m_bDelayedDestroy && m_pManager;
	WRITE_LOCK
	for (ControlArrayType::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		if(m_bDelayedFree)
		{
			if(m_pManager) m_pManager->addDelayedFree(*iter);
		}
		else if (m_bAutoDestroy)
		{
			if(isDelayedCleanup) m_pManager->addDelayedCleanup(*iter);
			else delete *iter;
		}
	}
	//for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ ) {
	//	if( m_bDelayedDestroy && m_pManager ) m_pManager->AddDelayedCleanup(static_cast<CControlUI*>(m_items[it]));             
	//	else delete static_cast<CControlUI*>(m_items[it]);
	//}

	m_items.clear();
	if(isDelayedCleanup)
		::PostMessage(m_pManager->getPaintWindow(), WM_APP + 1, 0, 0L);
	//m_swItems.done(L"containerUC::removeAll");
	needUpdate();
}

controlUC* containerUC::front() const
{
	controlUC *pControl = nullptr;
	//m_swItems.waitToRead(L"containerUC::front");
	READ_LOCK
	if(m_items.size() > 0) pControl = m_items.front();
	//m_swItems.done(L"containerUC::front");
	return pControl;
}
controlUC* containerUC::back() const
{
	controlUC *pControl = nullptr;
	//m_swItems.waitToRead(L"containerUC::back");
	READ_LOCK
	if(m_items.size() > 0) pControl = m_items.back();
	//m_swItems.done(L"containerUC::back");
	return pControl;
}
#define CLAMP(x,a,b) (min(b,max(a,x)))
int containerUC::findSelectable(int iIndex, bool bForward /*= true*/) const
{
	// NOTE: This is actually a helper-function for the list/combo/ect controls
	//       that allow them to find the next enabled/available selectable item
	if( getCount() == 0 ) return -1;
	iIndex = CLAMP(iIndex, 0, getCount() - 1);
	if( bForward ) {
		for( int i = iIndex; i < getCount(); i++ ) {
			if( getItemAt(i)->getInterface(L"IListItem") != NULL 
				&& getItemAt(i)->isVisible()
				&& getItemAt(i)->isEnabled() ) return i;
		}
		return -1;
	}
	else {
		for( int i = iIndex; i >= 0; --i ) {
			if( getItemAt(i)->getInterface(L"IListItem") != NULL 
				&& getItemAt(i)->isVisible()
				&& getItemAt(i)->isEnabled() ) return i;
		}
		return findSelectable(0, true);
	}
}
//void containerUC::buildScrollBar(scrollBarUC *pScrollbarVertical, scrollBarUC *pScrollbarHorizontal /*= nullptr*/)
//{
//	m_pVerticalScrollBar = pScrollbarVertical;
//	m_pHorizontalScrollBar = pScrollbarHorizontal;
//}

void containerUC::enableScrollBar(bool bEnableVertical /*= true*/, bool bEnableHorizontal /*= false*/)
{
	//if (m_pVerticalScrollBar)
	//{
	//	m_pVerticalScrollBar->setEnabled(bEnableVertical);
	//}
	//if (m_pHorizontalScrollBar)
	//{
	//	m_pHorizontalScrollBar->setEnabled(bEnableHorizontal);
	//}
	if( bEnableVertical && !m_pVerticalScrollBar ) {
		m_pVerticalScrollBar = new scrollBarUC;
		m_pVerticalScrollBar->setOwner(this);
		m_pVerticalScrollBar->setManager(m_pManager, NULL, false);
		if ( m_pManager ) {
			//LPCTSTR pDefaultAttributes = m_pManager->getDefaultAttributeList(_T("VScrollBar"));
			//if( pDefaultAttributes ) {
			//	m_pVerticalScrollBar->applyAttributeList(pDefaultAttributes);
			//}
		}
	}
	else if( !bEnableVertical && m_pVerticalScrollBar ) {
		delete m_pVerticalScrollBar;
		m_pVerticalScrollBar = NULL;
	}

	if( bEnableHorizontal && !m_pHorizontalScrollBar ) {
		m_pHorizontalScrollBar = new scrollBarUC;
		m_pHorizontalScrollBar->setHorizontal(true);
		m_pHorizontalScrollBar->setOwner(this);
		m_pHorizontalScrollBar->setManager(m_pManager, NULL, false);
		if ( m_pManager ) {
			//LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("HScrollBar"));
			//if( pDefaultAttributes ) {
			//	m_pHorizontalScrollBar->applyAttributeList(pDefaultAttributes);
			//}
		}
	}
	else if( !bEnableHorizontal && m_pHorizontalScrollBar ) {
		delete m_pHorizontalScrollBar;
		m_pHorizontalScrollBar = NULL;
	}

	needUpdate();
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
	if( m_pVerticalScrollBar && (m_bAutoHideScroll || m_pVerticalScrollBar->isVisible()) ) sz.cy = m_pVerticalScrollBar->getScrollPos();
	if( m_pHorizontalScrollBar && (m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible()) ) sz.cx = m_pHorizontalScrollBar->getScrollPos();
	return sz;
}

SIZE containerUC::getScrollRange() const
{
	SIZE sz = {0, 0};
	if( m_pVerticalScrollBar && (m_bAutoHideScroll || m_pVerticalScrollBar->isVisible()) ) sz.cy = m_pVerticalScrollBar->getScrollRange();
	if( m_pHorizontalScrollBar && (m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible()) ) sz.cx = m_pHorizontalScrollBar->getScrollRange();
	return sz;
}

void containerUC::setScrollPos(SIZE szPos)
{
	int cx = 0;
	int cy = 0;
	if( m_pVerticalScrollBar && (m_bAutoHideScroll || m_pVerticalScrollBar->isVisible()) ) {
		int iLastScrollPos = m_pVerticalScrollBar->getScrollPos();
		m_pVerticalScrollBar->setScrollPos(szPos.cy);
		cy = m_pVerticalScrollBar->getScrollPos() - iLastScrollPos;
	}

	if( m_pHorizontalScrollBar && (m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible()) ) {
		int iLastScrollPos = m_pHorizontalScrollBar->getScrollPos();
		m_pHorizontalScrollBar->setScrollPos(szPos.cx);
		cx = m_pHorizontalScrollBar->getScrollPos() - iLastScrollPos;
	}

	if( cx == 0 && cy == 0 ) return;

	RECT rcPos;
	//m_swItems.waitToRead(L"containerUC::setScrollPos");
	READ_LOCK
	for (ControlArrayType::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
	{
		//SEND_LOG(L"scrollpos <%s> %d", gtc::format::rect(rcPos).c_str(), (*iter)->isVisible());
		if(!(*iter)->isVisible()) continue;
		if((*iter)->isFloat()) continue;
		rcPos = (*iter)->getPosition();
		rcPos.left -= cx;
		rcPos.right -= cx;
		rcPos.top -= cy;
		rcPos.bottom -= cy;
		(*iter)->setPosition(rcPos);
	}
	//m_swItems.done(L"containerUC::setScrollPos");
	invalidate();
}

void containerUC::setPosition(RECT pos)
{
	__super::setPosition(pos);
	if( m_items.empty() ) return;
	pos.left += m_rcInset.left;
	pos.top += m_rcInset.top;
	pos.right -= m_rcInset.right;
	pos.bottom -= m_rcInset.bottom;

	//m_swItems.waitToRead(L"containerUC::setPosition");
	READ_LOCK
	for( int it = 0; it < int(m_items.size()); it++ ) {
		if( !m_items[it]->isVisible() ) continue;
		if( m_items[it]->isFloat() ) {
			setFloatPos(it);
		}
		else {
			m_items[it]->setPosition(pos); // 所有非float子控件放大到整个客户区
		}
	}
	//m_swItems.done(L"containerUC::setPosition");
}

void containerUC::lineUp(int step /*= 0*/)
{
	int cyLine = 8;
	if(step <= 0)
	{
		if( m_pManager ) cyLine = m_pManager->getDefaultFontInfo()->tm.tmHeight + 8;
	}
	else cyLine = step;
	SIZE sz = getScrollPos();
	sz.cy -= cyLine;
	setScrollPos(sz);
}

void containerUC::lineDown(int step /*= 0*/)
{
	int cyLine = 8;
	if(step <= 0)
	{
		if( m_pManager ) cyLine = m_pManager->getDefaultFontInfo()->tm.tmHeight + 8;
	}
	else cyLine = step;
	SIZE sz = getScrollPos();
	sz.cy += cyLine;
	setScrollPos(sz);
}

void containerUC::pageUp()
{
	SIZE sz = getScrollPos();
	int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
	if( m_pHorizontalScrollBar && (m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible()) ) iOffset -= m_pHorizontalScrollBar->getFixedHeight();
	sz.cy -= iOffset;
	setScrollPos(sz);
}

void containerUC::pageDown()
{
	SIZE sz = getScrollPos();
	int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
	if( m_pHorizontalScrollBar && (m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible()) ) iOffset -= m_pHorizontalScrollBar->getFixedHeight();
	sz.cy += iOffset;
	setScrollPos(sz);
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
	if( m_pVerticalScrollBar && (m_bAutoHideScroll || m_pVerticalScrollBar->isVisible()) ) iOffset -= m_pVerticalScrollBar->getFixedWidth();
	sz.cx -= iOffset;
	setScrollPos(sz);
}

void containerUC::pageRight()
{
	SIZE sz = getScrollPos();
	int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
	if( m_pVerticalScrollBar && (m_bAutoHideScroll || m_pVerticalScrollBar->isVisible()) ) iOffset -= m_pVerticalScrollBar->getFixedWidth();
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
bool containerUC::isAutoHideScroll() const
{
	return m_bAutoHideScroll;
}
void containerUC::setAutoHideScroll(bool bAutoHide)
{
	m_bAutoHideScroll = bAutoHide;
}
bool containerUC::isDriftScroll() const
{
	return m_bDriftScroll;
}
void containerUC::setDriftScroll(bool bDrift /*= true*/)
{
	if(m_bDriftScroll == bDrift) return;
	m_bDriftScroll = bDrift;
	invalidate();
}
void containerUC::setFloatPos(int iIndex)
{
	// 因为controlUI::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
	if( iIndex < 0 || iIndex >= (int)m_items.size() ) return;

	//m_swItems.waitToRead(L"containerUC::setFloatPos");
	controlUC* pControl = m_items[iIndex];

	if( !pControl->isVisible() ) {
		//m_swItems.done(L"containerUC::setFloatPos");
		return;
	}
	if( !pControl->isFloat() ) {
		//m_swItems.done(L"containerUC::setFloatPos");
		return;
	}
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
		BSRelativePosUC tRelativePos = pControl->getRelativePos();
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
	pControl->setPosition(rcCtrl);
	//m_swItems.done(L"containerUC::setFloatPos");
}
void containerUC::processScrollBar(RECT rc, int cxRequired, int cyRequired)
{
	if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->getScrollRange() > 0/*(m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible())*/ ) {
		RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->getFixedHeight()};
		if(m_bDriftScroll) ::OffsetRect(&rcScrollBarPos, 0, -m_pHorizontalScrollBar->getFixedHeight());
		m_pHorizontalScrollBar->setPosition(rcScrollBarPos);
	}

	if( m_pVerticalScrollBar == NULL ) return;

	if( cyRequired > rc.bottom - rc.top && m_pVerticalScrollBar->getScrollRange() <= 0/*!m_pVerticalScrollBar->isVisible()*/ ) {
		m_pVerticalScrollBar->setVisible(true);
		m_pVerticalScrollBar->setScrollRange(cyRequired - (rc.bottom - rc.top));
		m_pVerticalScrollBar->setScrollPos(0);
		m_bScrollProcess = true;
		setPosition(m_rcItem);
		m_bScrollProcess = false;

		RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->getFixedWidth(), rc.bottom };
		if(m_bDriftScroll) ::OffsetRect(&rcScrollBarPos, -m_pVerticalScrollBar->getFixedWidth(), 0);
		m_pVerticalScrollBar->setPosition(rcScrollBarPos);

		return;
	}
	// No scrollbar required
	if(m_pVerticalScrollBar->getScrollRange() == 0 ) return;
	//if(!m_pVerticalScrollBar->isVisible() ) return;

	// Scroll not needed anymore?
	int cyScroll = cyRequired - (rc.bottom - rc.top);
	if( cyScroll <= 0 && !m_bScrollProcess) {
		m_pVerticalScrollBar->setVisible(false);
		m_pVerticalScrollBar->setScrollPos(0);
		m_pVerticalScrollBar->setScrollRange(0);
		//SEND_LOG(L"ERROR BBBBBBBBBBBBBBBBBBBBBBBBB");
		setPosition(m_rcItem);
	}
	else
	{
		RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->getFixedWidth(), rc.bottom };
		if(m_bDriftScroll) ::OffsetRect(&rcScrollBarPos, -m_pVerticalScrollBar->getFixedWidth(), 0);
		m_pVerticalScrollBar->setPosition(rcScrollBarPos);

		if( m_pVerticalScrollBar->getScrollRange() != cyScroll ) {
			int iScrollPos = m_pVerticalScrollBar->getScrollPos();
			m_pVerticalScrollBar->setScrollRange(::abs(cyScroll));
			if( m_pVerticalScrollBar->getScrollRange() == 0 ) {
				m_pVerticalScrollBar->setVisible(false);
				//SEND_LOG(L"ERROR CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
				m_pVerticalScrollBar->setScrollPos(0);
			}
			if( iScrollPos > m_pVerticalScrollBar->getScrollPos() ) {
				setPosition(m_rcItem);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

verticalLayoutUC::verticalLayoutUC()
	: m_iSepHeight(0), m_uButtonState(0), m_bImmMode(false)
{
	//TRACE_CLASS_BEGIN(this)
	ptLastMouse.x = ptLastMouse.y = 0;
	::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
}

PCWSTR verticalLayoutUC::getClass() const
{
	return L"verticalLayoutUI";
}

LPVOID verticalLayoutUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"VerticalLayout") == 0 ) return static_cast<verticalLayoutUC*>(this);
	return containerUC::getInterface(pstrName);
}

UINT verticalLayoutUC::getControlFlags() const
{
	if( isEnabled() && m_iSepHeight != 0 ) return UCFLAG_SETCURSOR;
	else return 0;
}

void verticalLayoutUC::setPosition(RECT rc)
{
	controlUC::setPosition(rc);
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
	//m_swItems.waitToRead(L"verticalLayoutUC::setPosition");
	{
		READ_LOCK
		for( std::size_t it1 = 0; it1 < m_items.size(); it1++ ) {
			controlUC* pControl = m_items[it1];
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

	}
	//m_swItems.done(L"verticalLayoutUC::setPosition");
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
	//m_swItems.waitToRead(L"verticalLayoutUC::setPosition_2");
	READ_LOCK
	for( std::size_t it2 = 0; it2 < m_items.size(); it2++ ) {
		controlUC* pControl = m_items[it2];
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
		if(m_pVerticalScrollBar && m_bDriftScroll)
			sz.cx += m_pVerticalScrollBar->getFixedWidth();
		if( sz.cx < pControl->getMinWidth() ) sz.cx = pControl->getMinWidth();
		if( sz.cx > pControl->getMaxWidth() ) sz.cx = pControl->getMaxWidth();

		RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
		pControl->setPosition(rcCtrl);

		iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
		cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
		szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
	}
	//m_swItems.done(L"verticalLayoutUC::setPosition_2");
	cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

	// Process the scrollbar
	processScrollBar(rc, 0, cyNeeded);
}


void verticalLayoutUC::setSepHeight(int iHeight)
{
	m_iSepHeight = iHeight;
}

int verticalLayoutUC::getSepHeight() const
{
	return m_iSepHeight;
}

void verticalLayoutUC::setSepImmMode(bool bImmediately)
{
	if( m_bImmMode == bImmediately ) return;
	if( (m_uButtonState & UCSTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
		//m_pManager->removePostPaint(this);
	}

	m_bImmMode = bImmediately;
}

bool verticalLayoutUC::isSepImmMode() const
{
	return m_bImmMode;
}

void verticalLayoutUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"sepheight") == 0 ) setSepHeight(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"sepimm") == 0 ) setSepImmMode(_tcscmp(pstrValue, L"true") == 0);
	else containerUC::setAttribute(pstrName, pstrValue);
}

void verticalLayoutUC::Event(BSEventUC& event)
{
	if( m_iSepHeight != 0 ) {
		if( event.Type == BEUCEventButtonDown && isEnabled() )
		{
			RECT rcSeparator = getThumbRect(false);
			if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
				m_uButtonState |= UCSTATE_CAPTURED;
				ptLastMouse = event.ptMouse;
				m_rcNewPos = m_rcItem;
				//if( !m_bImmMode && m_pManager ) m_pManager->addPostPaint(this);
				return;
			}
		}
		if( event.Type == BEUCEventButtonUp )
		{
			if( (m_uButtonState & UCSTATE_CAPTURED) != 0 ) {
				m_uButtonState &= ~UCSTATE_CAPTURED;
				m_rcItem = m_rcNewPos;
				//if( !m_bImmMode && m_pManager ) m_pManager->removePostPaint(this);
				needParentUpdate();
				return;
			}
		}
		if( event.Type == BEUCEventMouseMove )
		{
			if( (m_uButtonState & UCSTATE_CAPTURED) != 0 ) {
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

				RECT rcInvalidate = getThumbRect(true);
				m_rcNewPos = rc;
				m_szxyFixed.cy = m_rcNewPos.bottom - m_rcNewPos.top;

				if( m_bImmMode ) {
					m_rcItem = m_rcNewPos;
					needParentUpdate();
				}
				else {
					//rcInvalidate.join(getThumbRect(true));
					//rcInvalidate.join(getThumbRect(false));
					rcInvalidate = rectJoin(rcInvalidate, getThumbRect(true));
					rcInvalidate = rectJoin(rcInvalidate, getThumbRect(false));
					if( m_pManager ) m_pManager->invalidate(rcInvalidate);
				}
				return;
			}
		}
		if( event.Type == BEUCEventSetCursor )
		{
			RECT rcSeparator = getThumbRect(false);
			if( isEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
				return;
			}
		}
	}
	containerUC::Event(event);
}

RECT verticalLayoutUC::getThumbRect(bool bUseNew) const
{
	if( (m_uButtonState & UCSTATE_CAPTURED) != 0 && bUseNew) {
		if( m_iSepHeight >= 0 ) 
		{
			RECT rt = {m_rcNewPos.left, max(m_rcNewPos.bottom - m_iSepHeight, m_rcNewPos.top), 
				m_rcNewPos.right, m_rcNewPos.bottom};
			return rt;
		}
		else 
		{
			RECT rt = {m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.right, 
				min(m_rcNewPos.top - m_iSepHeight, m_rcNewPos.bottom)};
			return rt;
		}
	}
	else {
		if( m_iSepHeight >= 0 ) 
		{
			RECT rt = {m_rcItem.left, max(m_rcItem.bottom - m_iSepHeight, m_rcItem.top), m_rcItem.right, 
				m_rcItem.bottom};
			return rt;
		}
		else 
		{
			RECT rt = {m_rcItem.left, m_rcItem.top, m_rcItem.right, 
				min(m_rcItem.top - m_iSepHeight, m_rcItem.bottom)};
			return rt;
		}

	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

horizontalLayoutUC::horizontalLayoutUC() 
	: m_iSepWidth(0), m_uButtonState(0), m_bImmMode(false)
{
	//TRACE_CLASS_BEGIN(this)
	ptLastMouse.x = ptLastMouse.y = 0;
	::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
}

PCWSTR horizontalLayoutUC::getClass() const
{
	return L"horizontalLayoutUI";
}

LPVOID horizontalLayoutUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"HorizontalLayout") == 0 ) return static_cast<horizontalLayoutUC*>(this);
	return containerUC::getInterface(pstrName);
}

UINT horizontalLayoutUC::getControlFlags() const
{
	if( isEnabled() && m_iSepWidth != 0 ) return UCFLAG_SETCURSOR;
	else return 0;
}

void horizontalLayoutUC::setPosition(RECT rc)
{
	controlUC::setPosition(rc);
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
	//m_swItems.waitToRead(L"horizontalLayoutUC::setPosition");
	READ_LOCK
	for( std::size_t it1 = 0; it1 < m_items.size(); it1++ ) {
		controlUC* pControl = m_items[it1];
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
	//m_swItems.done(L"horizontalLayoutUC::setPosition");
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
	{

		//m_swItems.waitToRead(L"horizontalLayoutUC::setPosition_2");
		READ_LOCK
		for( std::size_t it2 = 0; it2 < m_items.size(); it2++ ) {
			controlUC* pControl = m_items[it2];
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
			pControl->setPosition(rcCtrl);
			iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
			szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
		}
	}

	//m_swItems.done(L"horizontalLayoutUC::setPosition_2");
	// Process the scrollbar
	processScrollBar(rc, 0, 0);
}

void horizontalLayoutUC::setSepWidth(int iWidth)
{
	m_iSepWidth = iWidth;
}

int horizontalLayoutUC::getSepWidth() const
{
	return m_iSepWidth;
}

void horizontalLayoutUC::setSepImmMode(bool bImmediately)
{
	if( m_bImmMode == bImmediately ) return;
	if( (m_uButtonState & UCSTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
		//m_pManager->removePostPaint(this);
	}

	m_bImmMode = bImmediately;
}

bool horizontalLayoutUC::isSepImmMode() const
{
	return m_bImmMode;
}

void horizontalLayoutUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"sepwidth") == 0 ) setSepWidth(_ttoi(pstrValue));
	else if( _tcscmp(pstrName, L"sepimm") == 0 ) setSepImmMode(_tcscmp(pstrValue, L"true") == 0);
	else __super::setAttribute(pstrName, pstrValue);
}

void horizontalLayoutUC::Event(BSEventUC& event)
{
	if( m_iSepWidth != 0 ) {
		if( event.Type == BEUCEventButtonDown && isEnabled() )
		{
			RECT rcSeparator = getThumbRect(false);
			if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
				m_uButtonState |= UCSTATE_CAPTURED;
				ptLastMouse = event.ptMouse;
				m_rcNewPos = m_rcItem;
				//if( !m_bImmMode && m_pManager ) m_pManager->addPostPaint(this);
				return;
			}
		}
		if( event.Type == BEUCEventButtonUp )
		{
			if( (m_uButtonState & UCSTATE_CAPTURED) != 0 ) {
				m_uButtonState &= ~UCSTATE_CAPTURED;
				m_rcItem = m_rcNewPos;
				//if( !m_bImmMode && m_pManager ) m_pManager->removePostPaint(this);
				needParentUpdate();
				return;
			}
		}
		if( event.Type == BEUCEventMouseMove )
		{
			if( (m_uButtonState & UCSTATE_CAPTURED) != 0 ) {
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

				RECT rcInvalidate = getThumbRect(true);
				m_rcNewPos = rc;
				m_szxyFixed.cx = m_rcNewPos.right - m_rcNewPos.left;

				if( m_bImmMode ) {
					m_rcItem = m_rcNewPos;
					needParentUpdate();
				}
				else {
					//rcInvalidate.join(getThumbRect(true));
					//rcInvalidate.join(getThumbRect(false));
					rcInvalidate = rectJoin(rcInvalidate, getThumbRect(true));
					rcInvalidate = rectJoin(rcInvalidate, getThumbRect(false));
					if( m_pManager ) m_pManager->invalidate(rcInvalidate);
				}
				return;
			}
		}
		if( event.Type == BEUCEventSetCursor )
		{
			RECT rcSeparator = getThumbRect(false);
			if( isEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
				return;
			}
		}
	}
	__super::Event(event);
}

RECT horizontalLayoutUC::getThumbRect(bool bUseNew) const
{
	if( (m_uButtonState & UCSTATE_CAPTURED) != 0 && bUseNew) {
		if( m_iSepWidth >= 0 )
		{
			RECT rt = {m_rcNewPos.right - m_iSepWidth, m_rcNewPos.top, m_rcNewPos.right, m_rcNewPos.bottom};
			return rt;
		}
		else
		{
			RECT rt = {m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.left - m_iSepWidth, m_rcNewPos.bottom};
			return rt;
		}
	}
	else {
		if( m_iSepWidth >= 0 )
		{
			RECT rt = {m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom};
			return rt;
		}
		else
		{
			RECT rt = {m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom};
			return rt;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	tabLayoutUC  类		2015-7-27
//	只有一个可见元素的容器
////////////////////////////////////////////////////////////////////////////////////////////////////////
tabLayoutUC::tabLayoutUC()
	: m_iCurSel(-1)
{

}

PCWSTR tabLayoutUC::getClass() const
{
	return L"tabLayoutUC";
}
LPVOID tabLayoutUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, _T("TabLayout")) == 0 ) return static_cast<tabLayoutUC*>(this);
	return __super::getInterface(pstrName);
}

bool tabLayoutUC::add(controlUC* pControl)
{
	bool ret = __super::add(pControl);
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
bool tabLayoutUC::addAt(controlUC* pControl, int iIndex)
{
	bool ret = __super::addAt(pControl, iIndex);
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
bool tabLayoutUC::remove(controlUC* pControl)
{
	if( pControl == NULL) return false;

	int index = getItemIndex(pControl);
	bool ret = __super::remove(pControl);
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
void tabLayoutUC::removeAll()
{
	m_iCurSel = -1;
	__super::removeAll();
	needParentUpdate();
}
int tabLayoutUC::getCurSel() const
{
	return m_iCurSel;
}
bool tabLayoutUC::selectItem(int iIndex)
{
	if( iIndex < 0 || iIndex >= (int)m_items.size() ) return false;
	if( iIndex == m_iCurSel ) return true;

	int iOldSel = m_iCurSel;
	m_iCurSel = iIndex;
	READ_LOCK
	for( int it = 0; it < (int)m_items.size(); it++ )
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

void tabLayoutUC::setPosition(RECT rc)
{
	controlUC::setPosition(rc);
	rc = m_rcItem;

	// Adjust for inset
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;
	READ_LOCK
	for( int it = 0; it < (int)m_items.size(); it++ ) {
		controlUC* pControl = m_items[it];
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
		pControl->setPosition(rcCtrl);
	}
}

void tabLayoutUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( std::wcscmp(pstrName, L"selectedid") == 0 ) selectItem(std::wcstol(pstrValue, NULL, 10));
	return __super::setAttribute(pstrName, pstrValue);
}



}