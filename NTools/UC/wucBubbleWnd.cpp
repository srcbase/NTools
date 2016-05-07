#include "wucBubbleWnd.h"
#include "wucContainer.h"
namespace wuc {

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	bubbleWnd  类		2015-8-10
//	气泡式窗口基类
//	关闭方式：关闭按钮、失去焦点、倒计时三种，其中“关闭按钮”和“失去焦点”互斥
////////////////////////////////////////////////////////////////////////////////////////////////////////
bubbleWnd::bubbleWnd()
	: m_pOwner(nullptr)
	, m_pRoot(nullptr)/*, m_buttonClose(nullptr)*/
	, m_uiTimeElapse(0)
	, m_bExitAtLostfocus(true)
{

}
bubbleWnd::~bubbleWnd()
{
	SEND_LOG(L"NOTE bubbleWnd free ... ");
}
//buttonUC* bubbleWnd::getButtonClose() const
//{
//	return m_buttonClose;
//}
verticalLayoutUC* bubbleWnd::getRoot() const
{
	return m_pRoot;
}
LRESULT bubbleWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	bool bHandled = false;
	if (uMsg == WM_CREATE)
	{
		m_drawManager.init(m_hWnd);
		//m_drawManager.setBackgroundTransparent(true);
		//m_drawManager.setLayerWindowTransparent(true);
		m_pRoot = new verticalLayoutUC();
		//m_pRoot->setBkColor(0xFF6600CC);
		m_pRoot->setName(L"root");
		m_drawManager.attachDialog(m_pRoot);
		m_drawManager.addNotifier(this);

		//m_buttonClose = new buttonUC();
		//m_buttonClose->setFloat(true);
		//m_buttonClose->setVisible(false);

		
		onInitDialog();
		// 
		if (m_uiTimeElapse > 0)
		{
			SetTimer(m_hWnd, BUBBLE_WND_CLOSE_TIMEID, m_uiTimeElapse, NULL);
		}
	}
	else if (uMsg == WM_CLOSE)
	{
		onClose();
		if(m_pOwner) m_pOwner->setFocus();
		//m_pOwner->getWindowOwner()->setFocus();
	}
	else if (uMsg == WM_KILLFOCUS)
	{
		if (m_bExitAtLostfocus)
		{
			postMessage(WM_CLOSE);
		}
	}
	else if (uMsg == WM_TIMER)
	{
		if (wParam == BUBBLE_WND_CLOSE_TIMEID)
		{
			postMessage(WM_CLOSE);
		}
	}
	else if (uMsg == WM_WINDOWPOSCHANGED)
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
	else if (uMsg == WM_NCHITTEST)
	{
		if (!isEmpty(m_drawManager.getCaptionRect()))
		{
			lRes = onNcHitTest(uMsg, wParam, lParam, bHandled);
			bHandled = true;
		}
	}
	else if (uMsg == WM_SETFOCUS)
	{
		SEND_LOG(L"NOTE bubbleWnd::handleMessage WM_SETFOCUS");
	}
	else if (uMsg == WM_KILLFOCUS)
	{
		SEND_LOG(L"NOTE bubbleWnd::handleMessage WM_KILLFOCUS");
	}
	//if(!wuc::windowWnd::isOutMsg(uMsg))
	//	SEND_LOG(L"bubbleWnd::handleMessage msg:%s wp:%08X lp:%08X", wuc::windowWnd::stringMsg(uMsg).c_str(), wParam, lParam);
	if(bHandled) return lRes;
	if( m_drawManager.messageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return windowWnd::handleMessage(uMsg, wParam, lParam);
}
void bubbleWnd::onFinalMessage(HWND hWnd)
{
	SEND_LOG(L"bubbleWnd::onFinalMessage hwnd:%08X", hWnd);
	delete this;
}
void bubbleWnd::notify(wuc::BSNotifyUC& msg)
{
	if(!msg.pSender) return;
	if (msg.wsType == L"click")
	{
		if (msg.pSender->getName() == UC_DIALOG_BUTTON_CLOSE)
		{
			postMessage(WM_CLOSE);
		}
	}
}

void bubbleWnd::init(controlUC *pOwner, RECT rcPos, UINT timeElapse /*= 0*/)
{
	assert(pOwner);
	m_pOwner = pOwner;
	m_uiTimeElapse = timeElapse;
	create(m_pOwner->getManager()->getPaintWindow(), NULL, getWindowStyle(), getWindowExStyle(), rcPos);
	// HACK: Don't deselect the parent's caption
	HWND hWndParent = m_hWnd;
	while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
	::ShowWindow(m_hWnd, SW_SHOW);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

void bubbleWnd::onInitDialog()
{

}
void bubbleWnd::onClose()
{

}

LRESULT bubbleWnd::onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	m_drawManager.screenToClient(pt);
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	if( !::IsZoomed(*this) ) {
		RECT rcSizeBox = m_drawManager.getSizeBox();
		if( pt.y < rcClient.top + rcSizeBox.top ) {
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
			return HTTOP;
		}
		else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}
		if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
		if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
	}
	RECT rcCaption = m_drawManager.getCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			wuc::controlUC* pControl = m_drawManager.findControl(pt);
			if( pControl && _tcscmp(pControl->getClass(), _T("buttonUC")) != 0 )
			{
				return HTCAPTION;
			}
	}
	return HTCLIENT;
}
}	// wuc