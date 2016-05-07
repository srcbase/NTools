#include "dialogWnd.h"
#include "UC/wucBase.h"
namespace wuc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	dialogWnd  类		2015-5-25
//	对话框封装类
////////////////////////////////////////////////////////////////////////////////////////////////////////
dialogWnd::dialogWnd()
{

}
dialogWnd::~dialogWnd()
{

}

void dialogWnd::notify(wuc::BSNotifyUC& msg)
{
	if (msg.wsType == L"click" && msg.pSender)
	{
		if (msg.pSender->getName() == UC_DIALOG_BUTTON_OK)
		{
			close(wuc::BEAnswerStateOK);
			return;
		}
		else if (msg.pSender->getName() == UC_DIALOG_BUTTON_CANCEL)
		{
			close(wuc::BEAnswerStateCancel);
			return;
		}
		else if (msg.pSender->getName() == UC_DIALOG_BUTTON_CLOSE)
		{
			close(wuc::BEAnswerStateClose);
			return;
		}
		else if (msg.pSender->getName() == UC_DIALOG_BUTTON_MIN)
		{
			//::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, NULL);
			::ShowWindow(m_hWnd, SW_MINIMIZE);
		}
	}
}
LRESULT dialogWnd::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	bool bHandled = false;

	switch(uMsg) 
	{
	case WM_CREATE:
		{
			onInitDialog();
			bHandled = true;
		}
		break;
	case WM_NCHITTEST:
		{
			lRes = onNcHitTest(uMsg, wParam, lParam, bHandled);
			bHandled = true;
		}
		break;
	}
	if(bHandled) return lRes;
	if(m_drawManager.messageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return windowWnd::handleMessage(uMsg, wParam, lParam);
}
void dialogWnd::onFinalMessage(HWND hWnd)
{
}
LRESULT dialogWnd::onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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
void dialogWnd::onInitDialog()
{
	// 
	m_drawManager.init(m_hWnd);
	// 加载控件
	wuc::verticalLayoutUC *pRoot = new wuc::verticalLayoutUC();
	assert(pRoot);
	pRoot->setName(L"root");
	m_drawManager.attachDialog(pRoot);
	m_drawManager.addNotifier(this);
}
BEAnswerState dialogWnd::showModal(HWND hWndParent /*= ::GetActiveWindow()*/, const RECT *prcParent /*= NULL*/)
{
	RECT rc = {0};
	if(prcParent) rc = *prcParent;

	HWND hWnd = create(hWndParent, rc);
	assert(hWnd);
	return __super::showModal(&rc);
}
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-5-11 
verticalLayoutUC* dialogWnd::getRoot()
{
	verticalLayoutUC *pRoot = static_cast<verticalLayoutUC *>(m_drawManager.getRoot());
	assert(pRoot);
	return pRoot;
}
//////////////////////////////////////////////////////////////////////////
//	2015-5-10	设定对话框大小
void dialogWnd::setWindowSize(SIZE sz) {
	m_drawManager.setInitSize(sz.cx, sz.cy);
	if (m_hWnd)
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, sz.cx, sz.cy, /*窗口大小*/
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}
void dialogWnd::setWindowSize(int cx, int cy) {
	SIZE sz = {cx, cy};
	setWindowSize(sz);
}
//////////////////////////////////////////////////////////////////////////
//	2015-5-10	设定窗口是否支持透明
void dialogWnd::setBackgroundTransparent(bool bTrans /*= true*/) {
	m_drawManager.setBackgroundTransparent(bTrans);
}

void dialogWnd::setLayerWindowTransparent(bool bTrans /*= true*/) {
	m_drawManager.setLayerWindowTransparent(bTrans);
}
//////////////////////////////////////////////////////////////////////////
//	2015-5-10	设定标题区
void dialogWnd::setCaption(RECT rcCaption) {
	m_drawManager.setCaptionRect(rcCaption);
}
void dialogWnd::setCaption(int height) {
	RECT rc = {0,0,0,height};
	setCaption(rc);
}
//////////////////////////////////////////////////////////////////////////
//	2015-5-10	
void dialogWnd::setBackgroupImage(PCWSTR pbkImage) {
	//m_drawManager.setBackgroupImage(pbkImage);
	containerUC *pRoot = getRoot();
	pRoot->setImage(pbkImage);
	gtc::image *pimgBk = pRoot->getImage();// m_drawManager.findImage(pbkImage);
	if(!pimgBk) return ;
	RECT rc = {0,0,pimgBk->getWidth()-1, pimgBk->getHeight()-1};
	pRoot->setAttribute(L"bkimage", gtc::format::rect(rc).c_str());
}
//////////////////////////////////////////////////////////////////////////
//	2015-5-10	添加控件
void dialogWnd::addControl(controlUC *pControl) {
	assert(pControl);
	containerUC *pRoot = getRoot();
	pRoot->add(pControl);
}
//////////////////////////////////////////////////////////////////////////
//	2015-5-10	按控件名称找控件
controlUC* dialogWnd::findControl(PCWSTR pName) const {
	assert(pName);
	controlUC *pControl = m_drawManager.findControl(pName);
	//if (!pControl && m_pforeDialog)
	//{
	//	return m_pforeDialog->findControl(pName);
	//}
	return pControl;
}
}