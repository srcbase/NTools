#include "StdAfx.h"
#include "kpdeskDialog.h"


kpdeskDialog::kpdeskDialog(void)
{
}


kpdeskDialog::~kpdeskDialog(void)
{
}

void kpdeskDialog::notify(wuc::BSNotifyUC& msg)
{
	std::wstring senderName;
	if(msg.pSender) senderName = msg.pSender->getName();
	if (msg.wsType == L"click" && msg.pSender)
	{
		if (senderName == L"btnOK")
		{
			setAnswerState(wuc::BEAnswerStateOK);
			close();
			return;
		}
		else if (senderName == L"btnCancel" || senderName == L"btnClose")
		{
			setAnswerState(wuc::BEAnswerStateCancel);
			close();
			return;
		}
	}
}
LRESULT kpdeskDialog::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	bool bHandled = true;
	switch(uMsg)
	{
	case WM_CREATE:
		lRes = onCreate(uMsg, wParam, lParam, bHandled);
		break;
	case WM_NCHITTEST:
		lRes = onNcHitTest(uMsg, wParam, lParam, bHandled);
		break;
	default:
		bHandled = false;
		break;
	}
	if(bHandled) return lRes;
	if(m_drawManager.messageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return windowWnd::handleMessage(uMsg, wParam, lParam);
}
void kpdeskDialog::onFinalMessage(HWND hWnd)
{

}

LRESULT kpdeskDialog::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	//LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	//styleValue &= ~WS_CAPTION;
	//::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_drawManager.init(m_hWnd);
	// 初始化窗口属性
	m_drawManager.setAttribute(L"caption", L"0,0,0,40");	// 标题区
	m_drawManager.setAttribute(L"size", L"449,300");		// 窗口大小
	//m_drawManager.setAttribute(L"alpha", L"123");
	m_drawManager.setLayerWindowTransparent(true);
	m_drawManager.setBackgroupImage(L"dialog_bk_delete");
	// 加载控件
	wuc::verticalLayoutUC *pRoot = new wuc::verticalLayoutUC();
	assert(pRoot);

	// “取消”按钮
	wuc::buttonUC *pbutton = new wuc::buttonUC();
	pbutton->setName(L"btnCancel");
	pbutton->setAttribute(L"pos", L"225,261,0,0");
	pbutton->setAttribute(L"height", L"39");
	pbutton->setAttribute(L"width", L"224");
	pbutton->setAttribute(L"image", L"cancel");
	pbutton->setAttribute(L"normalimage", L"0");
	pbutton->setAttribute(L"hotimage", L"1");
	pbutton->setAttribute(L"pushedimage", L"2");
	pRoot->add(pbutton);
	// “确定”按钮
	pbutton = new wuc::buttonUC();
	pbutton->setName(L"btnOK");
	pbutton->setAttribute(L"pos", L"0,261,0,0");
	pbutton->setAttribute(L"height", L"39");
	pbutton->setAttribute(L"width", L"224");
	pbutton->setAttribute(L"image", L"ok");
	pbutton->setAttribute(L"normalimage", L"0");
	pbutton->setAttribute(L"hotimage", L"1");
	pbutton->setAttribute(L"pushedimage", L"2");
	pRoot->add(pbutton);
	// “关闭”按钮
	pbutton = new wuc::buttonUC();
	pbutton->setName(L"btnClose");
	pbutton->setAttribute(L"pos", L"414,14,0,0");
	pbutton->setAttribute(L"height", L"11");
	pbutton->setAttribute(L"width", L"11");
	pbutton->setAttribute(L"image", L"close");
	pbutton->setAttribute(L"normalimage", L"0");
	pbutton->setAttribute(L"hotimage", L"1");
	pbutton->setAttribute(L"pushedimage", L"2");
	pRoot->add(pbutton);

	m_drawManager.attachDialog(pRoot);
	m_drawManager.addNotifier(this);

	return 0;
}

LRESULT kpdeskDialog::onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	m_drawManager.screenToClient(pt);
	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_drawManager.getCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			wuc::controlUC* pControl = m_drawManager.findControl(pt);
			if( pControl && _tcscmp(pControl->getClass(), _T("buttonUC")) != 0 )
				return HTCAPTION;
	}
	return HTCLIENT;
}