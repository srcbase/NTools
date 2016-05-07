#include "wucList.h"
#include "wucBase.h"
#if defined(WUC_SUPPORT_THEME)
#include "AC/theme.h"

#endif
namespace wuc {


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listElementUC	类	2015-1-27
//	单个节点元素
////////////////////////////////////////////////////////////////////////////////////////////////////////
listElementUC::listElementUC(void)
	: m_pOwner(nullptr)
	, m_uButtonState(UCSTATE_NONE)
	, m_iIndex(-1), m_bSelected(false)
{
	//setBkColor(0xff0000ff);
}
listElementUC::~listElementUC(void)
{

}

PCWSTR listElementUC::getClass() const
{
	return L"listElementUC";
}
LPVOID listElementUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"IListItem") == 0 ) return static_cast<IListItemUC*>(this);
	if( _tcscmp(pstrName, _T("ListElement")) == 0 ) return static_cast<listElementUC*>(this);
	return controlUC::getInterface(pstrName);
}
UINT listElementUC::getControlFlags() const
{
	return UCFLAG_WANTRETURN;
}

int listElementUC::getIndex() const
{
	return m_iIndex;
}
void listElementUC::setIndex(int iIndex)
{
	if(m_iIndex == iIndex) return;
	m_iIndex = iIndex;
}

IListOwnerUC* listElementUC::getOwner()
{
	return m_pOwner;
}
void listElementUC::setOwner(controlUC* pOwner)
{
	m_pOwner = NULL;
	if(pOwner)
		m_pOwner = static_cast<IListOwnerUC *>(pOwner->getInterface(L"IListOwner"));
}
bool listElementUC::isSelected() const
{
	return m_bSelected;
}
bool listElementUC::select(bool bSelect /*= true*/)
{
	if( !isEnabled() ) return false;
	if( bSelect == m_bSelected ) return true;
	m_bSelected = bSelect;
	if( bSelect && m_pOwner != NULL ) m_pOwner->selectItem(m_iIndex);
	invalidate();

	return true;
}
bool listElementUC::isExpanded() const
{
	return false;
}
bool listElementUC::expand(bool bExpand /*= true*/)
{
	return false;
}
bool listElementUC::isCuted() const
{
	return (m_uButtonState & UCSTATE_CUTED) == UCSTATE_CUTED;

}
bool listElementUC::cut(bool bCut /*= true*/)
{
	if(!isEnabled()) return false;
	if (bCut)
	{
		if((m_uButtonState & UCSTATE_CUTED) == UCSTATE_CUTED) return true;
		m_uButtonState |= UCSTATE_CUTED;
		invalidate();
	}
	else
	{
		if((m_uButtonState & UCSTATE_CUTED) == 0) return true;
		m_uButtonState &= ~UCSTATE_CUTED;
		invalidate();
	}
	return true;
}
bool listElementUC::isHot() const
{
	return (m_uButtonState & UCSTATE_HOT) == UCSTATE_HOT;

}
bool listElementUC::hot(bool bHot /*= true*/)
{
	if(!isEnabled()) return false;
	if (bHot)
	{
		if((m_uButtonState & UCSTATE_HOT) == UCSTATE_HOT) return true;
		m_uButtonState |= UCSTATE_HOT;
		invalidate();
	}
	else
	{
		if((m_uButtonState & UCSTATE_HOT) == 0) return true;
		m_uButtonState &= ~UCSTATE_HOT;
		invalidate();
	}
	return true;
}

void listElementUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("selected")) == 0 ) select(_tcscmp(pstrValue, L"true") == 0);
	else __super::setAttribute(pstrName, pstrValue);
}
void listElementUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pOwner != NULL ) m_pOwner->Event(event);
		else controlUC::Event(event);
		return;
	}
	switch(event.Type)
	{
	case BEUCEventDBLClick:
		{
			if (isEnabled())
			{
				activate();
				invalidate();
			}
			return;
		}
		break;
	case BEUCEventKeyDown:
		{
			if (isEnabled() && event.chKey == VK_RETURN)
			{
				activate();
				invalidate();
				return;
			}
		}
		break;
	default:
		break;
	}
	if(m_pOwner) m_pOwner->Event(event);
	else controlUC::Event(event);
}
void listElementUC::paint(HDC hdc, const RECT &rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	drawItemBk(hdc, m_rcItem);
	drawItem(hdc, m_rcItem);
}
//void listElementUC::drawItem(HDC hdc)
//{
//
//}
void listElementUC::drawItemBk(HDC hdc, const RECT& rcItem)
{
	assert(m_pOwner);
	if( m_pOwner == NULL ) return;
	BSListItemInfo* pInfo = m_pOwner->getListInfo();
	DWORD iBackColor = 0;
	if( m_iIndex % 2 == 0 ) iBackColor = pInfo->clrBk;
	if( (m_uButtonState & UCSTATE_HOT) != 0 ) {
		iBackColor = pInfo->clrHotBk;
	}
	if( isSelected() ) {
		iBackColor = pInfo->clrSelectedBk;
	}
	if( !isEnabled() ) {
		iBackColor = pInfo->clrDisabledBk;
	}
	Gdiplus::Graphics grap(hdc);
	if ( iBackColor != 0 ) {
		gtc::drawer::fillRectangle(grap, iBackColor, m_rcItem);
		//CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
	}
	RECT rcSrc;
	if( !isEnabled() ) {
		if (pInfo->pImage && existsImage(pInfo->mapImage, UCIMAGE_STATE_DISABLED, &rcSrc))
		{
			if(gtc::drawer::drawImage(grap, pInfo->pImage, m_rcItem, rcSrc)) return;
		}
		//if( !pInfo->sDisabledImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)pInfo->sDisabledImage) ) pInfo->sDisabledImage.Empty();
		//	else return;
		//}
	}
	if( isSelected() ) {
		if (pInfo->pImage && existsImage(pInfo->mapImage, UCIMAGE_STATE_SELECTED, &rcSrc))
		{
			if(gtc::drawer::drawImage(grap, pInfo->pImage, m_rcItem, rcSrc)) return;
		}
		//if( !pInfo->sSelectedImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)pInfo->sSelectedImage) ) pInfo->sSelectedImage.Empty();
		//	else return;
		//}
	}
	if( (m_uButtonState & UCSTATE_HOT) != 0 ) {
		if (pInfo->pImage && existsImage(pInfo->mapImage, UCIMAGE_STATE_HOT, &rcSrc))
		{
			if(gtc::drawer::drawImage(grap, pInfo->pImage, m_rcItem, rcSrc)) return;
		}
		//if( !pInfo->sHotImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)pInfo->sHotImage) ) pInfo->sHotImage.Empty();
		//	else return;
		//}
	}
	

	if (getBkImage(rcSrc))
	{
		if (m_iIndex % 2 == 0)
		{
			gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
		}
	}
	else
	{
		if (pInfo->pImage && existsImage(pInfo->mapImage, UCIMAGE_STATE_BACKGROUND, &rcSrc))
		{
			if(gtc::drawer::drawImage(grap, pInfo->pImage, m_rcItem, rcSrc)) return;
		}
	}
	if (pInfo->clrLine != 0)
	{
		RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
		gtc::drawer::drawBorder(grap, gtc::BSPen(gtc::BEPenStyleSolid, 1, pInfo->clrLine), rcLine);
	}
	//if( !m_sBkImage.IsEmpty() ) {
	//	if( m_iIndex % 2 == 0 ) {
	//		if( !DrawImage(hDC, (LPCTSTR)m_sBkImage) ) m_sBkImage.Empty();
	//	}
	//}

	//if( m_sBkImage.IsEmpty() ) {
	//	if( !pInfo->sBkImage.IsEmpty() ) {
	//		if( !DrawImage(hDC, (LPCTSTR)pInfo->sBkImage) ) pInfo->sBkImage.Empty();
	//		else return;
	//	}
	//}

	//if ( pInfo->dwLineColor != 0 ) {
	//	RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
	//	CRenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
	//}
}

void listElementUC::setVisible(bool bVisible /*= true*/)
{
	controlUC::setVisible(bVisible);
	if( !isVisible() && isSelected())
	{
		select(false);
		if( m_pOwner != NULL ) m_pOwner->selectItem(-1);
	}
}
void listElementUC::setEnabled(bool bEnable /*= true*/)
{
	controlUC::setEnabled(bEnable);
	if( !isEnabled() ) {
		m_uButtonState = 0;
	}
}
void listElementUC::invalidate()
{
	if( !isVisible() ) return;

	if( getParent() ) {
		containerUC* pParentContainer = static_cast<containerUC*>(getParent()->getInterface(L"Container"));
		if( pParentContainer ) {
			RECT rc = pParentContainer->getPosition();
			RECT rcInset = pParentContainer->getInset();
			rc.left += rcInset.left;
			rc.top += rcInset.top;
			rc.right -= rcInset.right;
			rc.bottom -= rcInset.bottom;
			scrollBarUC* pVerticalScrollBar = pParentContainer->getVerticalScrollBar();
			if( pVerticalScrollBar && pVerticalScrollBar->isVisible() ) rc.right -= pVerticalScrollBar->getFixedWidth();
			scrollBarUC* pHorizontalScrollBar = pParentContainer->getHorizontalScrollBar();
			if( pHorizontalScrollBar && pHorizontalScrollBar->isVisible() ) rc.bottom -= pHorizontalScrollBar->getFixedHeight();

			RECT invalidateRc = m_rcItem;
			if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
			{
				return;
			}

			controlUC* pParent = getParent();
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

			if( m_pManager != NULL ) m_pManager->invalidate(invalidateRc);
		}
		else {
			controlUC::invalidate();
		}
	}
	else {
		controlUC::invalidate();
	}
}
bool listElementUC::activate()
{
	if( !controlUC::activate() ) return false;
	if( m_pManager != NULL ) m_pManager->sendNotify(this, L"itemactivate");
	return true;
}

//void listElementUC::drawBk(HDC hdc, const RECT& rcItem)
//{
//	// 如：绘制奇偶行交替显示色；选中状态等
//}

RECT listElementUC::getSubItemPosition(int iSubItem)
{
	assert(m_pOwner);
	BSListItemInfo *pListItem = m_pOwner->getListInfo();
	assert(pListItem);
	assert(iSubItem >= 0 && iSubItem < pListItem->iColumns);

	RECT rcSubItem = {pListItem->rcColumn[iSubItem].left, m_rcItem.top, pListItem->rcColumn[iSubItem].right, m_rcItem.bottom};
	rcSubItem.left += pListItem->rcTextPadding.left;
	rcSubItem.top += pListItem->rcTextPadding.top;
	rcSubItem.right -= pListItem->rcTextPadding.right;
	rcSubItem.bottom -= pListItem->rcTextPadding.bottom;

	return rcSubItem; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listLabelElementUC  类		2015-8-7
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
listLabelElementUC::listLabelElementUC()
{

}
listLabelElementUC::~listLabelElementUC()
{

}

PCWSTR listLabelElementUC::getClass() const
{
	return L"listLabelElementUC";
}
LPVOID listLabelElementUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"ListLabelElement") == 0 ) return static_cast<listLabelElementUC*>(this);
	return listElementUC::getInterface(pstrName);
}

void listLabelElementUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pOwner != NULL ) m_pOwner->Event(event);
		else __super::Event(event);
		return;
	}
	switch(event.Type)
	{
	case BEUCEventButtonDown:
	case BEUCEventRButtonDown:
		{
			if( isEnabled() ) {
				m_pManager->sendNotify(this, L"itemclick");
				select();
				invalidate();
			}
			return;
		}
		break;
	case BEUCEventMouseMove:
	case BEUCEventButtonUp:
		return;
		break;
	case BEUCEventMouseEnter:
		{
			if( isEnabled() ) {
				m_uButtonState |= UCSTATE_HOT;
				invalidate();
			}
		}
		break;
	case BEUCEventMouseLeave:
		{
			if( (m_uButtonState & UCSTATE_HOT) != 0 ) {
				m_uButtonState &= ~UCSTATE_HOT;
				invalidate();
			}
		}
		break;
	default:
		break;
	}
	listElementUC::Event(event);
}
SIZE listLabelElementUC::estimateSize(SIZE szAvailable)
{
	SIZE szXY = {0};
	if( m_pOwner == NULL ) return szXY;

	BSListItemInfo* pInfo = m_pOwner->getListInfo();
	szXY = m_szxyFixed;
	if( szXY.cy == 0 && m_pManager != NULL ) {
		szXY.cy = m_pManager->findFont(pInfo->iFont)->tm.tmHeight + 8;
		szXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
	}

	if( szXY.cx == 0 && m_pManager != NULL ) {
		RECT rcText = { 0, 0, 0, szXY.cy };
		//if( pInfo->bShowHtml ) {
		//	int nLinks = 0;
		//	CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, NULL, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
		//}
		//else {
		//	CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, pInfo->nFont, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
		//}
		Gdiplus::Graphics grap(m_pManager->getPaintDC());
		gtc::rect rc = gtc::drawer::measureString(grap, m_wsText.c_str(), rcText, m_pManager->getFont(pInfo->iFont), pInfo->uTextStyle|DT_SINGLELINE);
		
		szXY.cx = rc.width + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;        
	}

	return szXY;
}
// IListItemUC
void listLabelElementUC::drawItem(HDC hdc, const RECT& rcItem)
{
	if( m_wsText.empty() ) return;

	if( m_pOwner == NULL ) return;
	BSListItemInfo* pInfo = m_pOwner->getListInfo();
	DWORD clrText = pInfo->clrText;
	if( (m_uButtonState & UCSTATE_HOT) != 0 ) {
		clrText = pInfo->clrHotText;
	}
	if( isSelected() ) {
		clrText = pInfo->clrSelectedText;
	}
	if( !isEnabled() ) {
		clrText = pInfo->clrDisabledText;
	}
	int nLinks = 0;
	RECT rcText = rcItem;
	rcText.left += pInfo->rcTextPadding.left;
	rcText.right -= pInfo->rcTextPadding.right;
	rcText.top += pInfo->rcTextPadding.top;
	rcText.bottom -= pInfo->rcTextPadding.bottom;

	Gdiplus::Graphics grap(hdc);
	gtc::drawer::drawString(grap, m_wsText.c_str(), rcText, m_pManager->getFont(pInfo->iFont), pInfo->uTextStyle|DT_SINGLELINE, clrText);
	//if( pInfo->bShowHtml )
	//	CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, iTextColor, \
	//	NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle);
	//else
	//	CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, iTextColor, \
	//	pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listContainerElementUC  类		2015-7-30
//	带有子控件的列表元素基类
////////////////////////////////////////////////////////////////////////////////////////////////////////
listContainerElementUC::listContainerElementUC(void)
	: m_iIndex(-1)
	, m_pOwner(nullptr)
	, m_bSelected(false), m_uiButtonState(0)
{

}
listContainerElementUC::~listContainerElementUC(void)
{

}

PCWSTR listContainerElementUC::getClass() const
{
	return L"listContainerElementUC";
}
LPVOID listContainerElementUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"IListItem") == 0 ) return static_cast<IListItemUC*>(this);
	if( _tcscmp(pstrName, L"ListContainerElement") == 0 ) return static_cast<listContainerElementUC*>(this);
	return __super::getInterface(pstrName);
}
UINT listContainerElementUC::getControlFlags() const
{
	return UCFLAG_WANTRETURN;
}

void listContainerElementUC::setVisible(bool bVisible /*= true*/)
{
	__super::setVisible(bVisible);
	if (!isVisible() && m_bSelected)
	{
		m_bSelected = false;
		if(m_pOwner) m_pOwner->selectItem(-1);
	}
	//CContainerUI::SetVisible(bVisible);
	//if( !IsVisible() && m_bSelected)
	//{
	//	m_bSelected = false;
	//	if( m_pOwner != NULL ) m_pOwner->SelectItem(-1);
	//}
}
void listContainerElementUC::setEnabled(bool bEnable /*= true*/)
{
	controlUC::setEnabled(bEnable);
	if(!isEnabled()) m_uiButtonState = 0;
	//CControlUI::SetEnabled(bEnable);
	//if( !IsEnabled() ) {
	//	m_uButtonState = 0;
	//}
}
bool listContainerElementUC::activate()
{
	if(!containerUC::activate()) return false;
	if(m_pManager) m_pManager->sendNotify(this, L"itemactivate");
	return true;
	//if( !CContainerUI::Activate() ) return false;
	//if( m_pManager != NULL ) m_pManager->SendNotify(this, _T("itemactivate"));
	//return true;
}
void listContainerElementUC::invalidate()
{
	if( !isVisible() ) return;

	if( getParent() ) {
		containerUC* pParentContainer = static_cast<containerUC*>(getParent()->getInterface(L"Container"));
		if( pParentContainer ) {
			RECT rc = pParentContainer->getPosition();
			RECT rcInset = pParentContainer->getInset();
			rc.left += rcInset.left;
			rc.top += rcInset.top;
			rc.right -= rcInset.right;
			rc.bottom -= rcInset.bottom;
			scrollBarUC* pVerticalScrollBar = pParentContainer->getVerticalScrollBar();
			if( pVerticalScrollBar && pVerticalScrollBar->isVisible() ) rc.right -= pVerticalScrollBar->getFixedWidth();
			scrollBarUC* pHorizontalScrollBar = pParentContainer->getHorizontalScrollBar();
			if( pHorizontalScrollBar && pHorizontalScrollBar->isVisible() ) rc.bottom -= pHorizontalScrollBar->getFixedHeight();

			RECT invalidateRc = m_rcItem;
			if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
			{
				return;
			}

			controlUC* pParent = getParent();
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

			if( m_pManager != NULL ) m_pManager->invalidate(invalidateRc);
		}
		else {
			containerUC::invalidate();
		}
	}
	else {
		containerUC::invalidate();
	}
}

void listContainerElementUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if( _tcscmp(pstrName, L"selected") == 0 ) select();
	else __super::setAttribute(pstrName, pstrValue);
}
void listContainerElementUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pOwner != NULL ) m_pOwner->Event(event);
		else containerUC::Event(event);
		return;
	}

	if( event.Type == BEUCEventDBLClick )
	{
		if( isEnabled() ) {
			activate();
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventKeyDown && isEnabled() )
	{
		if( event.chKey == VK_RETURN ) {
			activate();
			invalidate();
			return;
		}
	}
	if( event.Type == BEUCEventButtonDown || event.Type == BEUCEventRButtonDown )
	{
		if( isEnabled() ){
			m_pManager->sendNotify(this, L"itemclick", NULL, (LPARAM)m_iIndex);
			if( ::PtInRect(&m_rcItem, event.ptMouse)) {
				m_uiButtonState |= UCSTATE_PUSHED | UCSTATE_CAPTURED;
			}
			//select();
			activate();
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventButtonUp ) 
	{
		if( (m_uiButtonState & UCSTATE_CAPTURED) != 0 ) {
			if( ::PtInRect(&m_rcItem, event.ptMouse) ) select();//activate();
			m_uiButtonState &= ~(UCSTATE_PUSHED | UCSTATE_CAPTURED);
			invalidate();
		}
		return;
	}
	if( event.Type == BEUCEventMouseMove )
	{
		if( (m_uiButtonState & UCSTATE_CAPTURED) != 0 ) {
			if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uiButtonState |= UCSTATE_PUSHED;
			else m_uiButtonState &= ~UCSTATE_PUSHED;
			invalidate();

		}
		return;
	}
	if( event.Type == BEUCEventMouseEnter )
	{
		if( isEnabled() ) {
			m_uiButtonState |= UCSTATE_HOT;
			invalidate();
		}
		//return;
	}
	if( event.Type == BEUCEventMouseLeave )
	{
		if( (m_uiButtonState & UCSTATE_HOT) != 0 ) {
			m_uiButtonState &= ~UCSTATE_HOT;
			invalidate();
		}
		//return;
	}

	// An important twist: The list-item will send the event not to its immediate
	// parent but to the "attached" list. A list may actually embed several components
	// in its path to the item, but key-presses etc. needs to go to the actual list.
	if( m_pOwner != NULL ) m_pOwner->Event(event); else controlUC::Event(event);
}
void listContainerElementUC::paint(HDC hdc, const RECT &rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	drawItemBk(hdc, m_rcItem);
	drawItem(hdc, m_rcItem);
	__super::paint(hdc, rcPaint);
}
void listContainerElementUC::drawItem(HDC hdc, const RECT& rcItem)
{

}
void listContainerElementUC::drawItemBk(HDC hdc, const RECT& rcItem)
{
	assert(m_pOwner);
	if( m_pOwner == NULL ) return;
	BSListItemInfo* pInfo = m_pOwner->getListInfo();
	DWORD iBackColor = 0;
	if( m_iIndex % 2 == 0 ) iBackColor = pInfo->clrBk;

	if( (m_uiButtonState & UCSTATE_HOT) != 0 ) {
		iBackColor = pInfo->clrHotBk;
	}
	if( isSelected() ) {
		iBackColor = pInfo->clrSelectedBk;
	}
	if( !isEnabled() ) {
		iBackColor = pInfo->clrDisabledBk;
	}
	Gdiplus::Graphics grap(hdc);
	if ( iBackColor != 0 ) {
		gtc::drawer::fillRectangle(grap, iBackColor, m_rcItem);
		//CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
	}
	RECT rcSrc;
	if( !isEnabled() ) {
		if (pInfo->pImage && existsImage(pInfo->mapImage, UCIMAGE_STATE_DISABLED, &rcSrc))
		{
			if(gtc::drawer::drawImage(grap, pInfo->pImage, m_rcItem, rcSrc)) return;
		}
		//if( !pInfo->sDisabledImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)pInfo->sDisabledImage) ) pInfo->sDisabledImage.Empty();
		//	else return;
		//}
	}
	if( isSelected() ) {
		if (pInfo->pImage && existsImage(pInfo->mapImage, UCIMAGE_STATE_SELECTED, &rcSrc))
		{
			if(gtc::drawer::drawImage(grap, pInfo->pImage, m_rcItem, rcSrc)) return;
		}
		//if( !pInfo->sSelectedImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)pInfo->sSelectedImage) ) pInfo->sSelectedImage.Empty();
		//	else return;
		//}
	}
	if( (m_uiButtonState & UCSTATE_HOT) != 0 ) {
		if (pInfo->pImage && existsImage(pInfo->mapImage, UCIMAGE_STATE_HOT, &rcSrc))
		{
			if(gtc::drawer::drawImage(grap, pInfo->pImage, m_rcItem, rcSrc)) return;
		}
		//if( !pInfo->sHotImage.IsEmpty() ) {
		//	if( !DrawImage(hDC, (LPCTSTR)pInfo->sHotImage) ) pInfo->sHotImage.Empty();
		//	else return;
		//}
	}
	if (getBkImage(rcSrc))
	{
		if (m_iIndex % 2 == 0)
		{
			gtc::drawer::drawImage(grap, m_pImage, m_rcItem, rcSrc);
		}
	}
	else
	{
		if (pInfo->pImage && existsImage(pInfo->mapImage, UCIMAGE_STATE_BACKGROUND, &rcSrc))
		{
			if(gtc::drawer::drawImage(grap, pInfo->pImage, m_rcItem, rcSrc)) return;
		}
	}
	if (pInfo->clrLine != 0)
	{
		RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
		gtc::drawer::drawBorder(grap, gtc::BSPen(gtc::BEPenStyleSolid, 1, pInfo->clrLine), rcLine);
	}
	/*if( !m_sBkImage.IsEmpty() ) {
		if( m_iIndex % 2 == 0 ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sBkImage) ) m_sBkImage.Empty();
		}
	}

	if( m_sBkImage.IsEmpty() ) {
		if( !pInfo->sBkImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)pInfo->sBkImage) ) pInfo->sBkImage.Empty();
			else return;
		}
	}

	if ( pInfo->dwLineColor != 0 ) {
		RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
		CRenderEngine::DrawLine(hDC, rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
	}*/
}
int listContainerElementUC::getIndex() const
{
	return m_iIndex;
}
void listContainerElementUC::setIndex(int iIndex)
{
	m_iIndex = iIndex;
}
IListOwnerUC* listContainerElementUC::getOwner()
{
	return m_pOwner;
}
void listContainerElementUC::setOwner(controlUC* pOwner)
{
	m_pOwner = static_cast<IListOwnerUC*>(pOwner->getInterface(_T("IListOwner")));
}
bool listContainerElementUC::isSelected() const
{
	return m_bSelected;
}
bool listContainerElementUC::select(bool bSelect /*= true*/)
{
	if( !isEnabled() ) return false;
	if( bSelect == m_bSelected ) return true;
	m_bSelected = bSelect;
	if( bSelect && m_pOwner != NULL ) m_pOwner->selectItem(m_iIndex);

	if(bSelect) m_uiButtonState |= UCSTATE_SELECTED;
	else m_uiButtonState &= ~UCSTATE_SELECTED;

	invalidate();

	return true;
}
bool listContainerElementUC::isCuted() const
{
	return (m_uiButtonState & UCSTATE_CUTED) == UCSTATE_CUTED;
}
bool listContainerElementUC::cut(bool bCut /*= true*/)
{
	if(!isEnabled()) return false;
	if (bCut)
	{
		if((m_uiButtonState & UCSTATE_CUTED) == UCSTATE_CUTED) return true;
		m_uiButtonState |= UCSTATE_CUTED;
		invalidate();
	}
	else
	{
		if((m_uiButtonState & UCSTATE_CUTED) == 0) return true;
		m_uiButtonState &= ~UCSTATE_CUTED;
		invalidate();
	}
	return true;
}
bool listContainerElementUC::isHot() const
{
	return (m_uiButtonState & UCSTATE_HOT) == UCSTATE_HOT;
}
bool listContainerElementUC::hot(bool bHot /*= true*/)
{
	if(!isEnabled()) return false;
	if (bHot)
	{
		if((m_uiButtonState & UCSTATE_HOT) == UCSTATE_HOT) return true;
		m_uiButtonState |= UCSTATE_HOT;
		invalidate();
	}
	else
	{
		if((m_uiButtonState & UCSTATE_HOT) == 0) return true;
		m_uiButtonState &= ~UCSTATE_HOT;
		invalidate();
	}
	return true;
}
bool listContainerElementUC::isExpanded() const
{
	return false;
}
bool listContainerElementUC::expand(bool bExpand /*= true*/)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listEmptyElementUC  类		2015-8-3
//	为了满足效果图设计了空行
////////////////////////////////////////////////////////////////////////////////////////////////////////
listEmptyElementUC::listEmptyElementUC()
{

}
listEmptyElementUC::~listEmptyElementUC()
{

}

PCWSTR listEmptyElementUC::getClass() const
{
	return L"listEmptyElementUC";
}
LPVOID listEmptyElementUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"ListEmptyElement") == 0 ) return static_cast<listEmptyElementUC*>(this);
	return __super::getInterface(pstrName);
}
UINT listEmptyElementUC::getControlFlags() const
{
	return UCFLAG_TABSTOP | (isEnabled() ? UCFLAG_SETCURSOR : 0);
}

void listEmptyElementUC::drawItem(HDC hdc, const RECT& rcItem)
{

}
void listEmptyElementUC::drawItemBk(HDC hdc, const RECT& rcItem)
{
	if(!hdc || m_dwBackColor == 0x0) return;
	Gdiplus::Graphics grap(hdc);
	RECT rcSrc = {0};
	if (m_pImage && getBkImage(rcSrc))
	{
		gtc::drawer::drawImage(grap, m_pImage, rcItem, rcSrc);
	}
	else
	{
		gtc::drawer::fillRectangle(grap, m_dwBackColor, rcItem);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listUC	类	2015-1-22
//	文件节点显示类
////////////////////////////////////////////////////////////////////////////////////////////////////////

listUC::listUC(void)
	: m_pCallback(NULL)
	, m_iCurSel(-1), m_iExpandedItem(-1)
	, m_bScrollSelect(false)
{
	m_pList = new listBodyUC(this);
	m_pHeader = new listHeaderUC();
	add(m_pHeader);
	__super::add(m_pList);
	//m_pHeader->setBkColor(0xFF00CCFF);
	//memset(&m_bsInfo, 0, sizeof(m_bsInfo));	// 不能这样用哦，里面有std::map
	
	m_bsInfo.iColumns = 0;				// 当前列数
	memset(m_bsInfo.rcColumn, 0, sizeof(m_bsInfo.rcColumn));	// 
	m_bsInfo.iFont = -1;					// 
	memset(&m_bsInfo.rcTextPadding, 0, sizeof(m_bsInfo.rcTextPadding));	// 文本留边
	//m_bsInfo.rcPadding.left = m_bsInfo.rcPadding.right = m_bsInfo.rcPadding.top = m_bsInfo.rcPadding.bottom = 1;
	m_bsInfo.clrText = 0xFEFFFFFF;				// 常规显示时的文本色
	m_bsInfo.clrBk = 0x0;					//		的背景色
	m_bsInfo.pImage = drawManager::getResource().findImage(L"folder_item_bk");				//		的背景图
	//m_bsInfo.nAlpha = 0x0FF;			// 
	m_bsInfo.clrSelectedText = 0xFEFFFFFF;		// 被选中时的文本色
	m_bsInfo.clrSelectedBk = 0x8099D9EA;		//		的背景色
	//m_bsInfo.nSelectedAlpha = 0x0FF;
	m_bsInfo.clrHotText = 0xFEFFFFFF;			// 处于悬停时的文本色
	m_bsInfo.clrHotBk = 0x3099D9EA;				//		的背景色
	//m_bsInfo.nHotAlpha = 0x0FF;
	m_bsInfo.clrCutedText = 0xFEFFFFFF;		// 处于剪切时的文本色
	m_bsInfo.clrCutedBk = 0x4099D9EA;			//		的背景色
	//m_bsInfo.nCutedAlpha = 0x7F;				// 
	m_bsInfo.clrDisabledText = 0xFECCCCCC;		// 处于无效或禁用时的文本色
	m_bsInfo.clrDisabledBk = 0xFEFFFFFF;		//		的背景色
	//m_bsInfo.nDisabledAlpha = 0x0FF;
	m_bsInfo.bMultiExpandable = false;
	m_bsInfo.clrLine = 0x0;					// 网格线的颜色，=0时不用绘制网格线
	m_bsInfo.mapImage.clear();
}


listUC::~listUC(void)
{
}

PCWSTR listUC::getClass() const
{
	return L"listUC";
}
LPVOID listUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"List") == 0 ) return static_cast<listUC*>(this);
	if( _tcscmp(pstrName, L"IList") == 0 ) return static_cast<IListUC*>(this);
	if( _tcscmp(pstrName, L"IListOwner") == 0 ) return static_cast<IListOwnerUC*>(this);
	return __super::getInterface(pstrName);
}
UINT listUC::getControlFlags() const
{
	return UCFLAG_TABSTOP;
}
void listUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = NULL;
	// 列表头及其单元格属性
	if( _tcscmp(pstrName, L"header") == 0 ) getHeader()->setVisible(_tcscmp(pstrValue, L"hidden") != 0);
	//else if( _tcscmp(pstrName, L"headertextcolor") == 0 ) {
	//	if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
	//	DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
	//	getHeader()->setTextColor(clrColor);
	//}
	//else if( _tcscmp(pstrName, L"headerbkcolor") == 0 ) {
	//	if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
	//	DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
	//	getHeader()->setBkColor(clrColor);
	//}
	//else if( _tcscmp(pstrName, L"headerfont") == 0 ) getHeader()->setFont(_tcstol(pstrValue, NULL, 10));
	//else if( _tcscmp(pstrName, L"headershadowtext") == 0 ) getHeader()->setShadowText(_tcscmp(pstrValue, L"true") == 0);
	//else if( _tcscmp(pstrName, L"headerimage") == 0 ) getHeader()->setImage(pstrValue);
	//else if( _tcscmp(pstrName, L"headernormalimage") == 0 ) {
	//	RECT rcImgCell = { 0 };
	//	rcImgCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
	//	rcImgCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
	//	rcImgCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
	//	rcImgCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
	//	getHeader()->setNormalImage(rcImgCell);
	//}
	//else if( _tcscmp(pstrName, L"headerhotimage") == 0 ) {
	//	RECT rcImgCell = { 0 };
	//	rcImgCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
	//	rcImgCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
	//	rcImgCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
	//	rcImgCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
	//	getHeader()->setHotImage(rcImgCell);
	//}
	//else if( _tcscmp(pstrName, L"headerpushedimage") == 0 ) {
	//	RECT rcImgCell = { 0 };
	//	rcImgCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
	//	rcImgCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
	//	rcImgCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
	//	rcImgCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
	//	getHeader()->setPushedImage(rcImgCell);
	//}
	//else if( _tcscmp(pstrName, L"headeralign") == 0 ) {
	//	UINT txtStyle = getHeader()->getTextStyle();
	//	if( _tcsstr(pstrValue, L"left") != NULL ) {
	//		txtStyle &= ~(DT_CENTER | DT_RIGHT);
	//		txtStyle |= DT_LEFT;
	//	}
	//	if( _tcsstr(pstrValue, L"center") != NULL ) {
	//		txtStyle &= ~(DT_LEFT | DT_RIGHT);
	//		txtStyle |= DT_CENTER;
	//	}
	//	if( _tcsstr(pstrValue, L"right") != NULL ) {
	//		txtStyle &= ~(DT_LEFT | DT_CENTER);
	//		txtStyle |= DT_RIGHT;
	//	}
	//	getHeader()->setTextStyle(txtStyle);
	//}
	// 列表单元格属性
	else if( _tcscmp(pstrName, L"itemfont") == 0 ) m_bsInfo.iFont = _ttoi(pstrValue);
	else if( _tcscmp(pstrName, L"itemlinecolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemLineColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemshowhtml") == 0 ) setItemShowHtml(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"alternatebk") == 0 ) setAlternateBk(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"itemalign") == 0 ) {
		if( _tcsstr(pstrValue, L"left") != NULL ) {
			m_bsInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
			m_bsInfo.uTextStyle |= DT_LEFT;
		}
		if( _tcsstr(pstrValue, L"center") != NULL ) {
			m_bsInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
			m_bsInfo.uTextStyle |= DT_CENTER;
		}
		if( _tcsstr(pstrValue, L"right") != NULL ) {
			m_bsInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
			m_bsInfo.uTextStyle |= DT_RIGHT;
		}
	}
	else if( _tcscmp(pstrName, L"itemendellipsis") == 0 ) {
		if( _tcscmp(pstrValue, L"true") == 0 ) m_bsInfo.uTextStyle |= DT_END_ELLIPSIS;
		else m_bsInfo.uTextStyle &= ~DT_END_ELLIPSIS;
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
	else __super::setAttribute(pstrName, pstrValue);

}
void listUC::setPosition(RECT pos)
{
	__super::setPosition(pos);
	if( m_pHeader == NULL ) return;
	// Determine general list information and the size of header columns
	m_bsInfo.iColumns = min(m_pHeader->getCount(), UCLIST_MAX_COLUMNS);
	// The header/columns may or may not be visible at runtime. In either case
	// we should determine the correct dimensions...
////#if defined(_DEBUG)
////	if(m_bsInfo.iColumns == 17)
////	{
////		int s = 0;
////		s = 1;
////	}
////
////#endif
	if( !m_pHeader->isVisible() ) {
		for( int it = 0; it < m_pHeader->getCount(); it++ ) {
			static_cast<controlUC*>(m_pHeader->getItemAt(it))->setInternVisible(true);
		}
		RECT rc = {pos.left, 0, pos.right, 0};
		m_pHeader->setPosition(rc);
	}
	int iOffset = m_pList->getScrollPos().cx;
	for( int i = 0; i < m_bsInfo.iColumns; i++ ) {
		controlUC* pControl = static_cast<controlUC*>(m_pHeader->getItemAt(i));
		if( !pControl->isVisible() ) continue;
		if( pControl->isFloat() ) continue;

		RECT rcPos = pControl->getPosition();
		if( iOffset > 0 ) {
			rcPos.left -= iOffset;
			rcPos.right -= iOffset;
			pControl->setPosition(rcPos);
		}
		m_bsInfo.rcColumn[i] = pControl->getPosition();
	}
	if( !m_pHeader->isVisible() ) {
		for( int it = 0; it < m_pHeader->getCount(); it++ ) {
			static_cast<controlUC*>(m_pHeader->getItemAt(it))->setInternVisible(false);
		}
	}
}

RECT listUC::getSubItemPosition(int iItem, int iSubItem)
{
	RECT rcSubItem = {0};
	assert(iItem >= 0 && iItem < m_pList->getCount());
	assert(iSubItem >= 0 && iSubItem < m_pHeader->getCount());
	assert(m_pHeader->getCount() == m_bsInfo.iColumns);
	if(iItem < 0 || iSubItem < 0 || iItem >= m_pList->getCount() || iSubItem >= m_pHeader->getCount()) return rcSubItem;
	listElementUC *pElement = static_cast<listElementUC *>(m_pList->getItemAt(iItem));
	if (pElement)
	{
		return pElement->getSubItemPosition(iSubItem);
	}
	return rcSubItem;
}
//void listUC::Event(BSEventUC &event)
//{
//	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
//		if( m_pParent != NULL ) m_pParent->Event(event);
//		else verticalLayoutUC::Event(event);
//		return;
//	}
//
//	switch(event.Type)
//	{
//	case BEUCEventSetFocus:
//		{
//			setFocus(true);
//			invalidate();
//		}
//		break;
//	case BEUCEventKillFocus:
//		{
//			setFocus(false);
//			invalidate();
//		}
//		break;
//	case BEUCEventKeyDown:
//		{
//
//		}
//		break;
//	case BEUCEventScrollWheel:
//		{
//
//		}
//		break;
//	default:
//		break;
//	}
//	verticalLayoutUC::Event(event);
//}

BSListItemInfo* listUC::getListInfo()
{
	return &m_bsInfo;
}
listHeaderUC* listUC::getHeader() const
{
	return m_pHeader;
}
containerUC* listUC::getList() const
{
	return m_pList;
}
int listUC::getCurSel() const
{
	return m_iCurSel;
}
bool listUC::getScrollSelect()
{
	return m_bScrollSelect;
}
void listUC::setScrollSelect(bool bScrollSelect)
{
	m_bScrollSelect = bScrollSelect;
}
bool listUC::selectItem(int iIndex)
{
	if( iIndex == m_iCurSel ) return true;

	int iOldSel = m_iCurSel;
	// We should first unselect the currently selected item
	if( m_iCurSel >= 0 ) {
		controlUC* pControl = getItemAt(m_iCurSel);
		if( pControl != NULL) {
			IListItemUC* pListItem = static_cast<IListItemUC*>(pControl->getInterface(_T("IListItem")));
			if( pListItem != NULL ) pListItem->select(false);
		}

		m_iCurSel = -1;
	}
	if( iIndex < 0 ) return false;

	controlUC* pControl = getItemAt(iIndex);
	if( pControl == NULL ) return false;
	if( !pControl->isVisible() ) return false;
	if( !pControl->isEnabled() ) return false;

	IListItemUC* pListItem = static_cast<IListItemUC*>(pControl->getInterface(_T("IListItem")));
	if( pListItem == NULL ) return false;
	m_iCurSel = iIndex;
	if( !pListItem->select(true) ) {
		m_iCurSel = -1;
		return false;
	}
	ensureVisible(m_iCurSel);
	//pControl->setFocus();
	if( m_pManager != NULL ) {
		m_pManager->sendNotify(this, L"itemselect", m_iCurSel, iOldSel);
	}

	return true;
}

IListCallbackUC* listUC::getItemCallback() const
{
	return m_pCallback;
}
void listUC::setItemCallback(IListCallbackUC* pCallback)
{
	if(m_pCallback == pCallback) return;
	m_pCallback = pCallback;
	// ... dai
}

bool listUC::expandItem(int iIndex, bool bExpand /*= true*/)
{
	if( m_iExpandedItem >= 0 && !m_bsInfo.bMultiExpandable) {
		controlUC* pControl = getItemAt(m_iExpandedItem);
		if( pControl != NULL ) {
			IListItemUC* pItem = static_cast<IListItemUC*>(pControl->getInterface(L"IListItem"));
			if( pItem != NULL ) pItem->expand(false);
		}
		m_iExpandedItem = -1;
	}
	if( bExpand ) {
		controlUC* pControl = getItemAt(iIndex);
		if( pControl == NULL ) return false;
		if( !pControl->isVisible() ) return false;
		IListItemUC* pItem = static_cast<IListItemUC*>(pControl->getInterface(L"IListItem"));
		if( pItem == NULL ) return false;
		m_iExpandedItem = iIndex;
		if( !pItem->expand(true) ) {
			m_iExpandedItem = -1;
			return false;
		}
	}
	needUpdate();
	return true;
}

void listUC::setMultiExpanding(bool bMultiExpandable)
{
	m_bsInfo.bMultiExpandable = bMultiExpandable;
}
int listUC::getExpandedItem() const
{
	return m_iExpandedItem;
}
//void listUC::doEvent(BSEventUC& event)
//{
//	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
//		if( m_pParent != NULL ) m_pParent->Event(event);
//		else __super::Event(event);
//		return;
//	}
//
//	if( event.Type == BEUCEventSetFocus ) 
//	{
//		m_bFocused = true;
//		return;
//	}
//	if( event.Type == BEUCEventKillFocus ) 
//	{
//		m_bFocused = false;
//		return;
//	}
//
//	switch( event.Type ) {
//	case BEUCEventKeyDown:
//		switch( event.chKey ) {
//		case VK_UP:
//			selectItem(findSelectable(m_iCurSel - 1, false));
//			return;
//		case VK_DOWN:
//			selectItem(findSelectable(m_iCurSel + 1, true));
//			return;
//		case VK_PRIOR:
//			pageUp();
//			return;
//		case VK_NEXT:
//			pageDown();
//			return;
//		case VK_HOME:
//			selectItem(findSelectable(0, false));
//			return;
//		case VK_END:
//			selectItem(findSelectable(getCount() - 1, true));
//			return;
//		case VK_RETURN:
//			if( m_iCurSel != -1 ) getItemAt(m_iCurSel)->activate();
//			return;
//		}
//		break;
//	case BEUCEventScrollWheel:
//		{
//			switch( LOWORD(event.wParam) ) {
//			case SB_LINEUP:
//				if( m_bScrollSelect ) selectItem(findSelectable(m_iCurSel - 1, false));
//				else lineUp();
//				return;
//			case SB_LINEDOWN:
//				if( m_bScrollSelect ) selectItem(findSelectable(m_iCurSel + 1, true));
//				else lineDown();
//				return;
//			}
//		}
//		break;
//	}
//	__super::Event(event);
//}
void listUC::Event(BSEventUC& event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else horizontalLayoutUC::Event(event);
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

	switch( event.Type ) {
	case BEUCEventKeyDown:
		switch( event.chKey ) {
		case VK_UP:
			selectItem(findSelectable(m_iCurSel - 1, false));
			return;
		case VK_DOWN:
			selectItem(findSelectable(m_iCurSel + 1, true));
			return;
		case VK_PRIOR:
			pageUp();
			return;
		case VK_NEXT:
			pageDown();
			return;
		case VK_HOME:
			selectItem(findSelectable(0, false));
			return;
		case VK_END:
			selectItem(findSelectable(getCount() - 1, true));
			return;
		case VK_RETURN:
			if( m_iCurSel != -1 ) getItemAt(m_iCurSel)->activate();
			return;
		}
		break;
	case BEUCEventScrollWheel:
		{
			switch( LOWORD(event.wParam) ) {
			case SB_LINEUP:
				if( m_bScrollSelect ) selectItem(findSelectable(m_iCurSel - 1, false));
				else
				{
					int step = 0;
					if (m_pCallback)
					{
						step = m_pCallback->getScrollLineStep();
					}
					else
					{
						if(m_pList->getCount() > 0)
							step = rectHeight(m_pList->getItemAt(0)->getPosition());
					}
					lineUp(step);
				}
				return;
			case SB_LINEDOWN:
				//SEND_LOG(L"SB_LINEDOWN pt:<%d, %d>", event.ptMouse.x, event.ptMouse.y);
				if( m_bScrollSelect ) selectItem(findSelectable(m_iCurSel + 1, true));
				else
				{
					int step = 0;
					if (m_pCallback)
					{
						step = m_pCallback->getScrollLineStep();
					}
					else
					{
						if(m_pList->getCount() > 0)
							step = rectHeight(m_pList->getItemAt(0)->getPosition());
					}
					lineDown(step);
				}
				return;
			}
		}
		break;
	}
	horizontalLayoutUC::Event(event);
}
int listUC::getChildPadding() const
{
	return m_pList->getChildPadding();
}
void listUC::setChildPadding(int iPadding)
{
	m_pList->setChildPadding(iPadding);
}
controlUC* listUC::getItemAt(int iIndex) const
{
	return m_pList->getItemAt(iIndex);
}
int listUC::getItemIndex(controlUC* pControl) const 
{
	if( pControl->getInterface(L"ListHeader") != NULL ) return __super::getItemIndex(pControl);
	// We also need to recognize header sub-items
	if( _tcsstr(pControl->getClass(), _T("listHeaderItemUC")) != NULL ) return m_pHeader->getItemIndex(pControl);

	return m_pList->getItemIndex(pControl);
}
bool listUC::setItemIndex(controlUC* pControl, int iIndex)
{
	if( pControl->getInterface(_T("ListHeader")) != NULL ) return __super::setItemIndex(pControl, iIndex);
	// We also need to recognize header sub-items
	if( _tcsstr(pControl->getClass(), _T("listHeaderItemUC")) != NULL ) return m_pHeader->setItemIndex(pControl, iIndex);

	int iOrginIndex = m_pList->getItemIndex(pControl);
	if( iOrginIndex == -1 ) return false;
	if( iOrginIndex == iIndex ) return true;

	IListItemUC* pSelectedListItem = NULL;
	if( m_iCurSel >= 0 ) pSelectedListItem = 
		static_cast<IListItemUC*>(getItemAt(m_iCurSel)->getInterface(L"IListItem"));
	if( !m_pList->setItemIndex(pControl, iIndex) ) return false;
	int iMinIndex = min(iOrginIndex, iIndex);
	int iMaxIndex = max(iOrginIndex, iIndex);
	for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
		controlUC* p = m_pList->getItemAt(i);
		IListItemUC* pListItem = static_cast<IListItemUC*>(p->getInterface(L"IListItem"));
		if( pListItem != NULL ) {
			pListItem->setIndex(i);
		}
	}
	if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->getIndex();
	return true;
}
int listUC::getCount() const
{
	return m_pList->getCount();
}
bool listUC::add(controlUC* pControl)
{
	// Override the Add() method so we can add items specifically to
	// the intended widgets. Headers are assumed to be
	// answer the correct interface so we can add multiple list headers.
	if( pControl->getInterface(L"ListHeader") != NULL ) {
		if( m_pHeader != pControl && m_pHeader->getCount() == 0 ) {
			__super::remove(m_pHeader);
			m_pHeader = static_cast<listHeaderUC*>(pControl);
		}
		m_bsInfo.iColumns = min(m_pHeader->getCount(), UCLIST_MAX_COLUMNS);
		return __super::addAt(pControl, 0);
	}
	// We also need to recognize header sub-items
	if( _tcsstr(pControl->getClass(), L"listHeaderItemUC") != NULL ) {
		listHeaderItemUC *pheaderItem = static_cast<listHeaderItemUC*>(pControl);
		pheaderItem->setOwner(m_pHeader);
		bool ret = m_pHeader->add(pheaderItem);
		m_bsInfo.iColumns = min(m_pHeader->getCount(), UCLIST_MAX_COLUMNS);
		//// Determine general list information and the size of header columns
		//m_bsInfo.iColumns = min(m_pHeader->getCount(), UCLIST_MAX_COLUMNS);
		//// The header/columns may or may not be visible at runtime. In either case
		//// we should determine the correct dimensions...
		//RECT pos = m_rcItem;
		//if( !m_pHeader->isVisible() ) {
		//	for( int it = 0; it < m_pHeader->getCount(); it++ ) {
		//		static_cast<controlUC*>(m_pHeader->getItemAt(it))->setInternVisible(true);
		//	}
		//	RECT rc = {pos.left, 0, pos.right, 0};
		//	m_pHeader->setPosition(rc);
		//}
		//int iOffset = m_pList->getScrollPos().cx;
		//for( int i = 0; i < m_bsInfo.iColumns; i++ ) {
		//	controlUC* pControl = static_cast<controlUC*>(m_pHeader->getItemAt(i));
		//	if( !pControl->isVisible() ) continue;
		//	if( pControl->isFloat() ) continue;

		//	RECT rcPos = pControl->getPosition();
		//	if( iOffset > 0 ) {
		//		rcPos.left -= iOffset;
		//		rcPos.right -= iOffset;
		//		pControl->setPosition(rcPos);
		//	}
		//	m_bsInfo.rcColumn[i] = pControl->getPosition();
		//}
		//if( !m_pHeader->isVisible() ) {
		//	for( int it = 0; it < m_pHeader->getCount(); it++ ) {
		//		static_cast<controlUC*>(m_pHeader->getItemAt(it))->setInternVisible(false);
		//	}
		//}
////#if defined(_DEBUG)
////		if (m_bsInfo.iColumns == 17)
////		{
////			int s = 0;
////			s = 1;
////		}
////#endif
		return ret;
	}
	// The list items should know about us
	IListItemUC* pListItem = static_cast<IListItemUC*>(pControl->getInterface(L"IListItem"));
	if( pListItem != NULL ) {
		pListItem->setOwner(this);
		pListItem->setIndex(getCount());
	}
	return m_pList->add(pControl);
}
bool listUC::addAt(controlUC* pControl, int iIndex)
{
	// Override the AddAt() method so we can add items specifically to
	// the intended widgets. Headers and are assumed to be
	// answer the correct interface so we can add multiple list headers.
	if( pControl->getInterface(L"ListHeader") != NULL ) {
		if( m_pHeader != pControl && m_pHeader->getCount() == 0 ) {
			__super::remove(m_pHeader);
			m_pHeader = static_cast<listHeaderUC*>(pControl);
		}
		m_bsInfo.iColumns = min(m_pHeader->getCount(), UCLIST_MAX_COLUMNS);
		return __super::addAt(pControl, 0);
	}
	// We also need to recognize header sub-items
	if( _tcsstr(pControl->getClass(), L"ListHeaderItemUI") != NULL ) {
		listHeaderItemUC *pheaderItem = static_cast<listHeaderItemUC*>(pControl);
		pheaderItem->setOwner(m_pHeader);
		bool ret = m_pHeader->addAt(pheaderItem, iIndex);
		m_bsInfo.iColumns = min(m_pHeader->getCount(), UCLIST_MAX_COLUMNS);
		return ret;
	}
	if (!m_pList->addAt(pControl, iIndex)) return false;

	// The list items should know about us
	IListItemUC* pListItem = static_cast<IListItemUC*>(pControl->getInterface(L"IListItem"));
	if( pListItem != NULL ) {
		pListItem->setOwner(this);
		pListItem->setIndex(iIndex);
	}

	for(int i = iIndex + 1; i < m_pList->getCount(); ++i) {
		controlUC* p = m_pList->getItemAt(i);
		pListItem = static_cast<IListItemUC*>(p->getInterface(L"IListItem"));
		if( pListItem != NULL ) {
			pListItem->setIndex(i);
		}
	}
	if( m_iCurSel >= iIndex ) m_iCurSel += 1;
	return true;
}
bool listUC::remove(controlUC* pControl)
{
	if( pControl->getInterface(L"ListHeader") != NULL ) return __super::remove(pControl);
	// We also need to recognize header sub-items
	if( _tcsstr(pControl->getClass(), L"listHeaderItemUC") != NULL )
	{
		bool bRet = m_pHeader->remove(pControl);
		m_bsInfo.iColumns = min(m_pHeader->getCount(), UCLIST_MAX_COLUMNS);
		return bRet;
	}
	int iIndex = m_pList->getItemIndex(pControl);
	if (iIndex == -1) return false;

	if (!m_pList->removeAt(iIndex)) return false;

	for(int i = iIndex; i < m_pList->getCount(); ++i) {
		controlUC* p = m_pList->getItemAt(i);
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

bool listUC::removeAt(int iIndex) 
{
	if (!m_pList->removeAt(iIndex)) return false;

	for(int i = iIndex; i < m_pList->getCount(); ++i) {
		controlUC* p = m_pList->getItemAt(i);
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

void listUC::removeAll()
{
	m_iCurSel = -1;
	m_iExpandedItem = -1;
	m_pList->removeAll();
}

void listUC::ensureVisible(int iIndex)
{
	if( m_iCurSel < 0 ) return;
	RECT rcItem = m_pList->getItemAt(iIndex)->getPosition();
	RECT rcList = m_pList->getPosition();
	RECT rcListInset = m_pList->getInset();

	rcList.left += rcListInset.left;
	rcList.top += rcListInset.top;
	rcList.right -= rcListInset.right;
	rcList.bottom -= rcListInset.bottom;

	scrollBarUC* pHorizontalScrollBar = m_pList->getHorizontalScrollBar();
	if( pHorizontalScrollBar && pHorizontalScrollBar->isVisible() ) rcList.bottom -= pHorizontalScrollBar->getFixedHeight();

	int iPos = m_pList->getScrollPos().cy;
	if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
	int dx = 0;
	if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
	if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
	scroll(0, dx);
}
void listUC::scroll(int dx, int dy)
{
	if( dx == 0 && dy == 0 ) return;
	SIZE sz = m_pList->getScrollPos();
	sz.cx += dx;
	sz.cy += dy;
	m_pList->setScrollPos(sz/*CSize(sz.cx + dx, sz.cy + dy)*/);
}
SIZE listUC::getScrollPos() const
{
	return m_pList->getScrollPos();
}
SIZE listUC::getScrollRange() const
{
	return m_pList->getScrollRange();
}
void listUC::setScrollPos(SIZE szPos)
{
	m_pList->setScrollPos(szPos);
}
void listUC::lineUp(int step /*= 0*/)
{
	m_pList->lineUp(step);
}
void listUC::lineDown(int step /*= 0*/)
{
	m_pList->lineDown(step);
}
void listUC::pageUp()
{
	m_pList->pageUp();
}
void listUC::pageDown()
{
	m_pList->pageDown();
}
void listUC::homeUp()
{
	m_pList->homeUp();
}
void listUC::endDown()
{
	m_pList->endDown();
}
void listUC::lineLeft()
{
	m_pList->lineLeft();
}
void listUC::lineRight()
{
	m_pList->lineRight();
}
void listUC::pageLeft()
{
	m_pList->pageLeft();
}
void listUC::pageRight()
{
	m_pList->pageRight();
}
void listUC::homeLeft()
{
	m_pList->homeLeft();
}
void listUC::endRight()
{
	m_pList->endRight();
}
//void listUC::buildScrollBar(scrollBarUC *pScrollbarVertical, scrollBarUC *pScrollbarHorizontal /*= nullptr*/)
//{
//	m_pList->buildScrollBar(pScrollbarVertical, pScrollbarHorizontal);
//}
void listUC::enableScrollBar(bool bEnableVertical /*= true*/, bool bEnableHorizontal /*= false*/)
{
	m_pList->enableScrollBar(bEnableVertical, bEnableHorizontal);
}
scrollBarUC* listUC::getVerticalScrollBar() const
{
	return m_pList->getVerticalScrollBar();
}
scrollBarUC* listUC::getHorizontalScrollBar() const
{
	return m_pList->getHorizontalScrollBar();
}

int listUC::getColumns() const
{
	return m_bsInfo.iColumns;
}
int listUC::getItemFont() const
{
	return m_bsInfo.iFont;
}
void listUC::setItemFont(int iFont)
{
	if(m_bsInfo.iFont == iFont) return;
	m_bsInfo.iFont = iFont;
	needUpdate();
}
UINT listUC::getItemTextStyle() const
{
	return m_bsInfo.uTextStyle;
}
void listUC::setItemTextStyle(UINT style)
{
	if(m_bsInfo.uTextStyle == style) return;
	m_bsInfo.uTextStyle = style;
	needUpdate();
}
RECT listUC::getItemTextPadding() const
{
	return m_bsInfo.rcTextPadding;
}
void listUC::setItemTextPadding(RECT rcPadding)
{
	if(m_bsInfo.rcTextPadding == rcPadding) return;
	m_bsInfo.rcTextPadding = rcPadding;
	needUpdate();
}
DWORD listUC::getItemTextColor() const
{
	return m_bsInfo.clrText;
}
void listUC::setItemTextColor(DWORD clrText)
{
	if(m_bsInfo.clrText == clrText) return;
	m_bsInfo.clrText = clrText;
	invalidate();
}

DWORD listUC::getItemBkColor() const
{
	return m_bsInfo.clrBk;
}
void listUC::setItemBkColor(DWORD clrBk)
{
	if(m_bsInfo.clrBk == clrBk) return;
	m_bsInfo.clrBk = clrBk;
	invalidate();
}

DWORD listUC::getItemSelectedTextColor() const
{
	return m_bsInfo.clrSelectedText;
}
void listUC::setItemSelectedTextColor(DWORD clrText)
{
	if(m_bsInfo.clrSelectedText == clrText) return;
	m_bsInfo.clrSelectedText = clrText;
	invalidate();
}
DWORD listUC::getItemSelectedBkColor() const
{
	return m_bsInfo.clrSelectedBk;
}
void listUC::setItemSelectedBkColor(DWORD clrBk)
{
	if(m_bsInfo.clrSelectedBk == clrBk) return;
	m_bsInfo.clrSelectedBk = clrBk;
	invalidate();
}
DWORD listUC::getItemHotTextColor() const
{
	return m_bsInfo.clrHotText;
}
void listUC::setItemHotTextColor(DWORD clrText)
{
	if(m_bsInfo.clrHotText == clrText) return;
	m_bsInfo.clrHotText = clrText;
	invalidate();
}
DWORD listUC::getItemHotBkColor() const
{
	return m_bsInfo.clrHotBk;
}
void listUC::setItemHotBkColor(DWORD clrBk)
{
	if(m_bsInfo.clrHotBk == clrBk) return;
	m_bsInfo.clrHotBk = clrBk;
	invalidate();
}
DWORD listUC::getItemCutTextColor() const
{
	return m_bsInfo.clrCutedText;
}
void listUC::setItemCutTextColor(DWORD clrText)
{
	if(m_bsInfo.clrCutedText == clrText) return;
	m_bsInfo.clrCutedText = clrText;
	invalidate();
}
DWORD listUC::getItemCutBkColor() const
{
	return m_bsInfo.clrCutedBk;
}
void listUC::setItemCutBkColor(DWORD clrBk)
{
	if(m_bsInfo.clrCutedBk == clrBk) return;
	m_bsInfo.clrCutedBk = clrBk;
	invalidate();
}
DWORD listUC::getItemDisabledTextColor() const
{
	return m_bsInfo.clrDisabledText;
}
void listUC::setItemDisabledTextColor(DWORD clrText)
{
	if(m_bsInfo.clrDisabledText == clrText) return;
	m_bsInfo.clrDisabledText = clrText;
	invalidate();
}
DWORD listUC::getItemDisabledBkColor() const
{
	return m_bsInfo.clrDisabledBk;
}
void listUC::setItemDisabledBkColor(DWORD clrBk)
{
	if(m_bsInfo.clrDisabledBk == clrBk) return;
	m_bsInfo.clrDisabledBk = clrBk;
	invalidate();
}
DWORD listUC::getItemLineColor() const
{
	return m_bsInfo.clrLine;
}
void listUC::setItemLineColor(DWORD clrLine)
{
	if(m_bsInfo.clrLine == clrLine) return;
	m_bsInfo.clrLine = clrLine;
	invalidate();
}

bool listUC::isItemShowHtml() const
{
	return m_bsInfo.bShowHtml;
}
void listUC::setItemShowHtml(bool bShowHtml /*= true*/)
{
	if(m_bsInfo.bShowHtml == bShowHtml) return;
	m_bsInfo.bShowHtml = bShowHtml;
	needUpdate();
}
bool listUC::isAlternateBk() const
{
	return m_bsInfo.bAlternateBk;
}
void listUC::setAlternateBk(bool bAlternateBk)
{
	if(m_bsInfo.bAlternateBk == bAlternateBk) return;
	m_bsInfo.bAlternateBk = bAlternateBk;
	needUpdate();
}
gtc::image* listUC::getItemImage() const
{
	return m_bsInfo.pImage;
}
void listUC::setItemImage(PCWSTR pwsImage)
{
	gtc::image *pimg = controlUC::analyzeImage(pwsImage);// drawManager::getResource().findImage(pwsImage);
	if(pimg == m_bsInfo.pImage) return;
	m_bsInfo.pImage = pimg;
	invalidate();
}
void listUC::setItemNormalImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_bsInfo.mapImage, UCIMAGE_STATE_NORMAL, rcImgCell, m_bsInfo.pImage);
	invalidate();
}
bool listUC::getItemNormalImage(RECT &rc) const
{
	return controlUC::existsImage(m_bsInfo.mapImage, UCIMAGE_STATE_NORMAL, &rc);
}
bool listUC::getItemSelectedImage(RECT &rc) const
{
	return controlUC::existsImage(m_bsInfo.mapImage, UCIMAGE_STATE_SELECTED, &rc);
}
void listUC::setItemSelectedImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_bsInfo.mapImage, UCIMAGE_STATE_SELECTED, rcImgCell, m_bsInfo.pImage);
	invalidate();
}
bool listUC::getItemCutedImage(RECT &rc) const
{
	return controlUC::existsImage(m_bsInfo.mapImage, UCIMAGE_STATE_CUTED, &rc);
}
void listUC::setItemCutedImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_bsInfo.mapImage, UCIMAGE_STATE_CUTED, rcImgCell, m_bsInfo.pImage);
	invalidate();
}
bool listUC::getItemHotImage(RECT &rc) const
{
	return controlUC::existsImage(m_bsInfo.mapImage, UCIMAGE_STATE_HOT, &rc);
}
void listUC::setItemHotImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_bsInfo.mapImage, UCIMAGE_STATE_HOT, rcImgCell, m_bsInfo.pImage);
	invalidate();
}
bool listUC::getItemDisabledImage(RECT &rc) const
{
	return controlUC::existsImage(m_bsInfo.mapImage, UCIMAGE_STATE_DISABLED, &rc);
}
void listUC::setItemDisabledImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_bsInfo.mapImage, UCIMAGE_STATE_DISABLED, rcImgCell, m_bsInfo.pImage);
	invalidate();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listBodyUC 类	2015-1-27
//	存放节点的容器类
////////////////////////////////////////////////////////////////////////////////////////////////////////
listBodyUC::listBodyUC(listUC *pOwner)
	: m_pOwner(pOwner)
{
	assert(m_pOwner);
	//RECT rc = {0};
	//setInset(rc);
	//setBkColor(0xFF663399);
}

listBodyUC::~listBodyUC()
{

}

void listBodyUC::setPosition(RECT pos)
{
	//SEND_LOG(L"listBodyUC::setPosition begin ...");
	controlUC::setPosition(pos);
	RECT rc = m_rcItem;

	// Adjust for inset
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;
	if( m_pVerticalScrollBar 
		&& m_pVerticalScrollBar->getScrollRange() > 0/*(m_bAutoHideScroll || m_pVerticalScrollBar->isVisible())*/
		&& !m_bDriftScroll) 
		rc.right -= m_pVerticalScrollBar->getFixedWidth();
	if( m_pHorizontalScrollBar 
		&& m_pHorizontalScrollBar->getScrollRange() > 0/*m_pHorizontalScrollBar->isVisible()*/
		&& !m_bDriftScroll) 
		rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();

	// Determine the minimum size
	SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->getScrollRange() > 0 /*(m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible())*/ ) 
		szAvailable.cx += m_pHorizontalScrollBar->getScrollRange();

	int cxNeeded = 0;
	int nAdjustables = 0;
	int cyFixed = 0;
	int nEstimateNum = 0;
	int items = 0;
	//m_swItems.waitToRead(L"listBodyUC::setPosition");
	{

		READ_LOCK
		items = m_items.size();
		for( int it1 = 0; it1 < items; it1++ ) {
			controlUC* pControl = m_items[it1];//static_cast<controlUC*>(m_items[it1]);
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

			RECT rcPadding = pControl->getPadding();
			sz.cx = max(sz.cx, 0);
			if( sz.cx < pControl->getMinWidth() ) sz.cx = pControl->getMinWidth();
			if( sz.cx > pControl->getMaxWidth() ) sz.cx = pControl->getMaxWidth();
			cxNeeded = max(cxNeeded, sz.cx);
			nEstimateNum++;
		}
	}
	//m_swItems.done(L"listBodyUC::setPosition");
	cyFixed += (nEstimateNum - 1) * m_iChildPadding;

	if( m_pOwner ) {
		listHeaderUC* pHeader = m_pOwner->getHeader();
		if( pHeader != NULL && pHeader->getCount() > 0 ) {
			SIZE sz = {rc.right - rc.left, rc.bottom - rc.top};
			cxNeeded = max(0, pHeader->estimateSize(sz).cx);
		}
	}

	// Place elements
	int cyNeeded = 0;
	int cyExpand = 0;
	if( nAdjustables > 0 ) cyExpand = max(0, (szAvailable.cy - cyFixed) / nAdjustables);
	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosY = rc.top;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->getScrollRange() > 0 /*(m_bAutoHideScroll || m_pVerticalScrollBar->isVisible())*/ ) {
		iPosY -= m_pVerticalScrollBar->getScrollPos();
	}
	int iPosX = rc.left;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->getScrollRange() > 0 /*(m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible())*/ ) {
		iPosX -= m_pHorizontalScrollBar->getScrollPos();
	}
	int iAdjustable = 0;
	int cyFixedRemaining = cyFixed;
	//m_swItems.waitToRead(L"listBodyUC::setPosition_2");
	{

		READ_LOCK
		//items = m_items.size();
		for( int it2 = 0; it2 < (int)m_items.size(); it2++ ) {
			controlUC* pControl = m_items[it2];//static_cast<controlUC*>(m_items[it2]);
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
			sz.cx = max(cxNeeded, szAvailable.cx - rcPadding.left - rcPadding.right);
			//if(m_pVerticalScrollBar && m_bDriftScroll)
			//	sz.cx += m_pVerticalScrollBar->getFixedWidth();
			if( sz.cx < pControl->getMinWidth() ) sz.cx = pControl->getMinWidth();
			if( sz.cx > pControl->getMaxWidth() ) sz.cx = pControl->getMaxWidth();

			RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
		
			//SEND_LOG(L"listBodyUC::setPosition element:%d pos:<%s>", it2, gtc::format::rect(rcCtrl, true).c_str());
			pControl->setPosition(rcCtrl);

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}
	}

	//m_swItems.done(L"listBodyUC::setPosition_2");
	cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

	if( m_pHorizontalScrollBar != NULL ) {
		if( cxNeeded > rc.right - rc.left ) {
			if( (m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible()) ) {
				m_pHorizontalScrollBar->setScrollRange(cxNeeded - (rc.right - rc.left));
			}
			else {
				m_pHorizontalScrollBar->setVisible(true);
				m_pHorizontalScrollBar->setScrollRange(cxNeeded - (rc.right - rc.left));
				m_pHorizontalScrollBar->setScrollPos(0);
				rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();
			}
		}
		else {
			if( (m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible()) ) {
				m_pHorizontalScrollBar->setVisible(false);
				m_pHorizontalScrollBar->setScrollRange(0);
				m_pHorizontalScrollBar->setScrollPos(0);
				rc.bottom += m_pHorizontalScrollBar->getFixedHeight();
			}
		}
	}
	//if (rectWidth(rc) == 332)
	//{
	//	SEND_LOG(L"------------------1--------%08X rows:%d cyNeeded:%d", this, getCount(), cyNeeded);
	//}
	//SEND_LOG(L"------------------1--------%08X rows:%d cyNeeded:%d", this, getCount(), cyNeeded);
	//if (getCount() == 5)
	//{
	//	int s;
	//	s = 0;
	//}
	// Process the scrollbar
	//if (m_pHorizontalScrollBar || m_pVerticalScrollBar)
	//{
		//int oldPos = m_pVerticalScrollBar->getScrollPos();
		SIZE szOldRange = getScrollRange();
		processScrollBar(rc, cxNeeded, cyNeeded);
		SIZE szNewRange = getScrollRange();
		//SEND_LOG(L"------------------2--------%08X rows:%d old:<%d,%d> new:<%d,%d>", this, getCount(), szOldRange.cx, szOldRange.cy, szNewRange.cx, szNewRange.cy);
		if(szOldRange != szNewRange && m_pOwner && m_pOwner->getItemCallback())
		{
			m_pOwner->getItemCallback()->scrollRangeChanged(szOldRange, szNewRange);
		}
	////	m_pVerticalScrollBar->setScrollPos(oldPos);
	////}
	//if (m_pVerticalScrollBar)
	//{
	//	m_pVerticalScrollBar->setScrollPos(oldPosV);
	//	m_pVerticalScrollBar->setVisible(oldShowV);
	//}

	//SEND_LOG(L"listBodyUC::setPosition end ...");

}
void listBodyUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pOwner != NULL ) m_pOwner->Event(event);
		else verticalLayoutUC::Event(event);
		return;
	}
	if(m_pOwner) m_pOwner->Event(event);
	else verticalLayoutUC::Event(event);
}
void listBodyUC::setScrollPos(SIZE szPos)
{
	int cx = 0;
	int cy = 0;
	SIZE szOldPos = getScrollPos();

	if( m_pVerticalScrollBar && m_pVerticalScrollBar->getScrollRange() > 0 ) {
	//if( m_pVerticalScrollBar && (m_bAutoHideScroll || m_pVerticalScrollBar->isVisible()) ) {
		int iLastScrollPos = m_pVerticalScrollBar->getScrollPos();
		m_pVerticalScrollBar->setScrollPos(szPos.cy);
		cy = m_pVerticalScrollBar->getScrollPos() - iLastScrollPos;
	}

	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->getScrollRange() > 0 ) {
	//if( m_pHorizontalScrollBar && (m_bAutoHideScroll || m_pHorizontalScrollBar->isVisible()) ) {
		int iLastScrollPos = m_pHorizontalScrollBar->getScrollPos();
		m_pHorizontalScrollBar->setScrollPos(szPos.cx);
		cx = m_pHorizontalScrollBar->getScrollPos() - iLastScrollPos;
	}

	if( cx == 0 && cy == 0 ) return;

	RECT rcPos;
	//m_swItems.waitToRead(L"listBodyUC::setScrollPos");
	READ_LOCK
	for( int it2 = 0; it2 < (int)m_items.size(); it2++ ) {
		controlUC* pControl = static_cast<controlUC*>(m_items[it2]);
		if( !pControl->isVisible() ) continue;
		if( pControl->isFloat() ) continue;

		rcPos = pControl->getPosition();
		rcPos.left -= cx;
		rcPos.right -= cx;
		rcPos.top -= cy;
		rcPos.bottom -= cy;
		pControl->setPosition(rcPos);
	}
	//m_swItems.done(L"listBodyUC::setScrollPos");
	invalidate();

	if( cx != 0 && m_pOwner ) {
		listHeaderUC* pHeader = m_pOwner->getHeader();
		if( pHeader == NULL ) return;
		BSListItemInfo* pInfo = m_pOwner->getListInfo();
		pInfo->iColumns = min(pHeader->getCount(), UCLIST_MAX_COLUMNS);

		if( !pHeader->isVisible() ) {
			for( int it = 0; it < pHeader->getCount(); it++ ) {
				static_cast<controlUC*>(pHeader->getItemAt(it))->setInternVisible(true);
			}
		}
		for( int i = 0; i < pInfo->iColumns; i++ ) {
			controlUC* pControl = static_cast<controlUC*>(pHeader->getItemAt(i));
			if( !pControl->isVisible() ) continue;
			if( pControl->isFloat() ) continue;

			RECT rcPos = pControl->getPosition();
			rcPos.left -= cx;
			rcPos.right -= cx;
			pControl->setPosition(rcPos);
			pInfo->rcColumn[i] = pControl->getPosition();
		}
		if( !pHeader->isVisible() ) {
			for( int it = 0; it < pHeader->getCount(); it++ ) {
				static_cast<controlUC*>(pHeader->getItemAt(it))->setInternVisible(false);
			}
		}
	}
	//SEND_LOG(L"listUC::setScrollPos %d", szPos.cy);
	if (szOldPos != szPos && m_pOwner && m_pOwner->getItemCallback())
	{
		m_pOwner->getItemCallback()->scrollPosChanged(szOldPos, szPos);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listHeaderUC	类	2015-1-27
//	列表控件的头；本对象中的属性影响listHeaderItemUC单元格
////////////////////////////////////////////////////////////////////////////////////////////////////////
listHeaderUC::listHeaderUC()
	: m_bDragable(false)
	, m_pimgSlider(NULL)
{
	m_bsInfo.bShadowText = false;
	m_bsInfo.uiTextStyle = DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
	m_bsInfo.iFont = -1;
	memset(&m_bsInfo.rcTextPadding, 0, sizeof(m_bsInfo.rcTextPadding));
	m_bsInfo.clrBk = 0x0FF999999;
	m_bsInfo.clrText = 0x0FFFFFFCC;
	m_bsInfo.clrHotText = 0x0FFFFFF99;
	m_bsInfo.clrHotBk = 0x0FF999999;
	m_bsInfo.clrPushedText = 0x0FFFFFF99;
	m_bsInfo.clrPushedBk = 0x0FF669999;
	m_bsInfo.pImage = NULL;
}
listHeaderUC::~listHeaderUC()
{

}
PCWSTR listHeaderUC::getClass() const
{
	return L"listHeaderUC";
}
LPVOID listHeaderUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"ListHeader") == 0 ) return this;
	return __super::getInterface(pstrName);
}

SIZE listHeaderUC::estimateSize(SIZE szAvailable)
{
	SIZE cXY = {0, m_szxyFixed.cy};
	if( cXY.cy == 0 && m_pManager != NULL ) {
		//m_swItems.waitToRead(L"listHeaderUC::estimateSize");
		READ_LOCK
		for( int it = 0; it < (int)m_items.size(); it++ ) {
			cXY.cy = max(cXY.cy,static_cast<controlUC*>(m_items[it])->estimateSize(szAvailable).cy);
		}
		//m_swItems.done(L"listHeaderUC::estimateSize");
		int nMin = m_pManager->getDefaultFontInfo()->tm.tmHeight + 6;
		cXY.cy = max(cXY.cy,nMin);
	}
	//m_swItems.waitToRead(L"listHeaderUC::estimateSize_2");
	READ_LOCK
	for( int it = 0; it < (int)m_items.size(); it++ ) {
		cXY.cx +=  static_cast<controlUC*>(m_items[it])->estimateSize(szAvailable).cx;
	}
	//m_swItems.done(L"listHeaderUC::estimateSize_2");
	return cXY;
}
bool listHeaderUC::isDragable() const
{
	return m_bDragable;
}
void listHeaderUC::setDragable(bool bDragable)
{
	m_bDragable = bDragable;
}
gtc::image* listHeaderUC::getSliderImage()
{
	return m_pimgSlider;
}
void listHeaderUC::setSliderImage(PCWSTR pImage)
{
	m_pimgSlider = drawManager::getResource().findImage(pImage);
}

BSHeaderItemInfo* listHeaderUC::getHeaderInfo()
{
	return &m_bsInfo;
}
void listHeaderUC::setItemTextStyle(UINT uStyle)
{
	if(m_bsInfo.uiTextStyle == uStyle) return;
	m_bsInfo.uiTextStyle = uStyle;
	invalidate();
}
UINT listHeaderUC::getItemTextStyle() const
{
	return m_bsInfo.uiTextStyle;
}
void listHeaderUC::setItemTextColor(DWORD dwTextColor)
{
	if(m_bsInfo.clrText == dwTextColor) return;
	m_bsInfo.clrText = dwTextColor;
	invalidate();
}
DWORD listHeaderUC::getItemTextColor() const
{
	return m_bsInfo.clrText;
}
void listHeaderUC::setItemBkColor(DWORD clrBk)
{
	if(m_bsInfo.clrBk == clrBk) return;
	m_bsInfo.clrBk = clrBk;
	invalidate();
}
DWORD listHeaderUC::getItemBkColor() const
{
	return m_bsInfo.clrBk;
}
void listHeaderUC::setItemFont(int index)
{
	if(m_bsInfo.iFont == index) return;
	m_bsInfo.iFont = index;
	invalidate();
}
int listHeaderUC::getItemFont() const
{
	return m_bsInfo.iFont;
}
bool listHeaderUC::isItemShadowText() const
{
	return m_bsInfo.bShadowText;
}
void listHeaderUC::setItemShadowText(bool bShadow /*= true*/)
{
	if(m_bsInfo.bShadowText == bShadow) return;
	m_bsInfo.bShadowText = bShadow;
	invalidate();
}


void listHeaderUC::setItemTextPadding(RECT rc)
{
	if(m_bsInfo.rcTextPadding == rc) return;
	m_bsInfo.rcTextPadding = rc;
	invalidate();
}
RECT listHeaderUC::getItemTextPadding() const
{
	return m_bsInfo.rcTextPadding;
}
gtc::image* listHeaderUC::getItemImage() const
{
	return m_bsInfo.pImage;
}
void listHeaderUC::setItemImage(PCWSTR pImage)
{
	m_bsInfo.pImage = drawManager::getResource().findImage(pImage);
}
bool listHeaderUC::getItemNormalImage(RECT &rc)
{
	return controlUC::existsImage(m_bsInfo.mapImage, UCIMAGE_STATE_NORMAL, &rc);
}
void listHeaderUC::setItemNormalImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_bsInfo.mapImage, UCIMAGE_STATE_NORMAL, rcImgCell);
	invalidate();
}
bool listHeaderUC::getItemHotImage(RECT &rc)
{
	return controlUC::existsImage(m_bsInfo.mapImage, UCIMAGE_STATE_HOT, &rc);
}
void listHeaderUC::setItemHotImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_bsInfo.mapImage, UCIMAGE_STATE_HOT, rcImgCell);
	invalidate();

}
bool listHeaderUC::getItemPushedImage(RECT &rc)
{
	return controlUC::existsImage(m_bsInfo.mapImage, UCIMAGE_STATE_PUSHED, &rc);
}
void listHeaderUC::setItemPushedImage(RECT rcImgCell)
{
	controlUC::setCellImage(m_bsInfo.mapImage, UCIMAGE_STATE_PUSHED, rcImgCell);
	invalidate();

}

void listHeaderUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	PWSTR pstr = NULL;
	if( _tcscmp(pstrName, L"itemtextcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itembkcolor") == 0 ) {
		if( *pstrValue == L'#') pstrValue = ::CharNext(pstrValue);
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		setItemBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, L"itemfont") == 0 ) setItemFont(_tcstol(pstrValue, NULL, 10));
	else if( _tcscmp(pstrName, L"itemshadowtext") == 0 ) setItemShadowText(_tcscmp(pstrValue, L"true") == 0);
	else if( _tcscmp(pstrName, L"itemimage") == 0 ) setItemImage(pstrValue);
	else if( _tcscmp(pstrName, L"itemnormalimage") == 0 ) {
		RECT rcImgCell = { 0 };
		rcImgCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcImgCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcImgCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcImgCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setItemNormalImage(rcImgCell);
	}
	else if( _tcscmp(pstrName, L"itemhotimage") == 0 ) {
		RECT rcImgCell = { 0 };
		rcImgCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcImgCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcImgCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcImgCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setItemHotImage(rcImgCell);
	}
	else if( _tcscmp(pstrName, L"itempushedimage") == 0 ) {
		RECT rcImgCell = { 0 };
		rcImgCell.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
		rcImgCell.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
		rcImgCell.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
		rcImgCell.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
		setItemPushedImage(rcImgCell);
	}
	else if( _tcscmp(pstrName, L"itemalign") == 0 ) {
		if( _tcsstr(pstrValue, L"left") != NULL ) {
			m_bsInfo.uiTextStyle &= ~(DT_CENTER | DT_RIGHT);
			m_bsInfo.uiTextStyle |= DT_LEFT;
		}
		if( _tcsstr(pstrValue, L"center") != NULL ) {
			m_bsInfo.uiTextStyle &= ~(DT_LEFT | DT_RIGHT);
			m_bsInfo.uiTextStyle |= DT_CENTER;
		}
		if( _tcsstr(pstrValue, L"right") != NULL ) {
			m_bsInfo.uiTextStyle &= ~(DT_LEFT | DT_CENTER);
			m_bsInfo.uiTextStyle |= DT_RIGHT;
		}
	}
	else __super::setAttribute(pstrName, pstrValue);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//	listHeaderItemUC	类	2015-1-27
//	列表控件的头元素单元格；本对象将影响其对应列的listElementUC集合属性
////////////////////////////////////////////////////////////////////////////////////////////////////////
listHeaderItemUC::listHeaderItemUC()
	: m_pOwner(NULL)
	, m_uiButtonState(UCSTATE_NONE)
	, m_iSepWidth(4)
{
	//assert(m_pOwner);
	memset(&m_ptLastMouse, 0, sizeof(m_ptLastMouse));
}
listHeaderItemUC::~listHeaderItemUC()
{

}

PCWSTR listHeaderItemUC::getClass() const
{
	return L"listHeaderItemUC";
}
LPVOID listHeaderItemUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, _T("ListHeaderItem")) == 0 ) return this;
	return controlUC::getInterface(pstrName);
}
UINT listHeaderItemUC::getControlFlags() const
{
	if( isEnabled() && m_iSepWidth != 0 ) return UCFLAG_SETCURSOR;
	else return 0;
}
listHeaderUC* listHeaderItemUC::getOwner() const
{
	return m_pOwner;
}
void listHeaderItemUC::setOwner(listHeaderUC *pHeader)
{
	assert(pHeader);
	m_pOwner = pHeader;
}
int listHeaderItemUC::getSepWidth() const
{
	return m_iSepWidth;
}
void listHeaderItemUC::setSepWidth(int iWidth)
{
	m_iSepWidth = iWidth;
}

RECT listHeaderItemUC::getThumbRect() const
{
	if( m_iSepWidth >= 0 )
	{
		RECT rc = {m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom};
		return rc;
	}
	RECT rc = {m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom};
	return rc;
}

void listHeaderItemUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else __super::Event(event);
		return;
	}
	switch (event.Type)
	{
	case BEUCEventSetFocus:
	case BEUCEventKillFocus:
		invalidate();
		//return;
		break;
	case BEUCEventButtonDown:
	case BEUCEventDBLClick:
		{
			if( !isEnabled() ) return;
			RECT rcSeparator = getThumbRect();
			if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
				if( m_pOwner->isDragable() ) {
					m_uiButtonState |= UCSTATE_CAPTURED;
					m_ptLastMouse = event.ptMouse;
				}
			}
			else {
				m_uiButtonState |= UCSTATE_PUSHED;
				m_pManager->sendNotify(this, L"headerclick");
				invalidate();
			}
			return;
		}
		break;
	case BEUCEventButtonUp:
		{
			if( (m_uiButtonState & UCSTATE_CAPTURED) != 0 ) {
				m_uiButtonState &= ~UCSTATE_CAPTURED;
				if( getParent() ) 
					getParent()->needParentUpdate();
			}
			else if( (m_uiButtonState & UCSTATE_PUSHED) != 0 ) {
				m_uiButtonState &= ~UCSTATE_PUSHED;
				invalidate();
			}
			return;
		}
		break;
	case BEUCEventMouseMove:
		{
			if( (m_uiButtonState & UCSTATE_CAPTURED) != 0 ) {
				RECT rc = m_rcItem;
				if( m_iSepWidth >= 0 ) {
					rc.right -= m_ptLastMouse.x - event.ptMouse.x;
				}
				else {
					rc.left -= m_ptLastMouse.x - event.ptMouse.x;
				}

				if( rc.right - rc.left > getMinWidth() ) {
					m_szxyFixed.cx = rc.right - rc.left;
					m_ptLastMouse = event.ptMouse;
					if( getParent() ) 
						getParent()->needParentUpdate();
				}
			}
			return;
		}
		break;
	case BEUCEventSetCursor:
		{
			RECT rcSeparator = getThumbRect();
			if( isEnabled() && m_pOwner->isDragable() && ::PtInRect(&rcSeparator, event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
				return;
			}
		}
		break;
	case BEUCEventMouseEnter:
		{
			if( isEnabled() ) {
				m_uiButtonState |= UCSTATE_HOT;
				invalidate();
			}
			return;
		}
		break;
	case BEUCEventMouseLeave:
		{
			if( isEnabled() ) {
				m_uiButtonState &= ~UCSTATE_HOT;
				invalidate();
			}
			return;
		}
		break;
	default:
		break;
	}
}
SIZE listHeaderItemUC::estimateSize(SIZE szAvailable)
{
	if( m_szxyFixed.cy == 0 ) {
		SIZE sz = {m_szxyFixed.cx, m_pManager->getDefaultFontInfo()->tm.tmHeight + 14};
		return sz;
	}
	return controlUC::estimateSize(szAvailable);
}
void listHeaderItemUC::paintStatusImage(HDC hdc)
{

}
void listHeaderItemUC::paintText(HDC hdc)
{
#if !defined(_DEBUG)
	if( m_wsText.empty() || !hdc) return;

#endif
	DWORD clrText = m_pOwner->getItemTextColor();
	if( clrText == 0 ) clrText = m_pManager->getDefaultFontColor();
	RECT rcTextPadding = m_pOwner->getItemTextPadding();
	RECT rcText = m_rcItem;
	rcText.left += rcTextPadding.left;
	rcText.top += rcTextPadding.top;
	rcText.right -= rcTextPadding.right;
	rcText.bottom -= rcTextPadding.bottom;

	int nLinks = 0;
	//clrText = 0xFFFF0099;
	//m_wsText = L"abc汉字123";
	Gdiplus::Graphics grap(hdc);
	RECT rc = rcText;
	::InflateRect(&rc, -1,-1);
	gtc::drawer::drawBorder(grap, gtc::BSPen(gtc::BEPenStyleSolid, 1, 0xFECCFF33), rc);

	if( m_pOwner->isItemShadowText() )
	{
#if defined(WUC_SUPPORT_THEME)
		acm::theme themeShadow;
		if (themeShadow.open(m_pManager->getPaintWindow(), L"TextStyle"))
		{
			bool bTheme = themeShadow.drawShadowText(hdc, m_wsText.c_str(), &rcText, clrText);
			themeShadow.close();
		}

#endif
	}
	else
	{
		gtc::drawer::drawString(grap, m_wsText.c_str(), rcText, m_pManager->getDefaultFontInfo()->hFont, (BEAlignMode)m_pOwner->getItemTextStyle(), clrText);
		//CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, m_dwTextColor, \
		//m_iFont, DT_SINGLELINE | m_uTextStyle);
	}
}
}	// end wuc