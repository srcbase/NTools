#include "StdAfx.h"
#include "kpdeskNotify.h"


kpdeskNotify::kpdeskNotify(void)
{
}


kpdeskNotify::~kpdeskNotify(void)
{
}

void kpdeskNotify::notify(wuc::BSNotifyUC& msg)
{
	std::wstring senderName;
	if(msg.pSender) senderName = msg.pSender->getName();
	if (msg.wsType == L"click" && msg.pSender)
	{
		if (senderName == L"btnIKnow")
		{
			setAnswerState(wuc::BEAnswerStateOK);
			close();
			return;
		}
		else if (senderName == L"btnNoDisplay")
		{
			if (wuc::buttonUC *pbutton = static_cast<wuc::buttonUC *>(msg.pSender))
			{

				//pbutton->setAttribute(L"normalimage", L"0");
			}
			
		}
		else if (senderName == L"btnClose")
		{
			setAnswerState(wuc::BEAnswerStateCancel);
			close();
			return;
		}
	}
	else if (msg.wsType == L"windowinit")
	{
		wuc::animateFrameUC *pAnimate = static_cast<wuc::animateFrameUC *>(m_drawManager.findControl(L"animateFrame"));
		if(pAnimate) pAnimate->playStart();
	}
}
LRESULT kpdeskNotify::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
	case WM_CLOSE:
		{
			bHandled = false;
			wuc::animateFrameUC *pAnimate = static_cast<wuc::animateFrameUC *>(m_drawManager.findControl(L"animateFrame"));
			if(pAnimate) pAnimate->playStop();
		}
		break;
	default:
		bHandled = false;
		break;
	}
	if(bHandled) return lRes;
	if(m_drawManager.messageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return windowWnd::handleMessage(uMsg, wParam, lParam);
}
void kpdeskNotify::onFinalMessage(HWND hWnd)
{

}

LRESULT kpdeskNotify::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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
	m_drawManager.setBackgroupImage(L"dialog_bk_notify");
	// 加载控件
	wuc::verticalLayoutUC *pRoot = new wuc::verticalLayoutUC();
	assert(pRoot);

	// “我知道了”按钮
	wuc::buttonUC *pbutton = new wuc::buttonUC();
	pbutton->setName(L"btnIKnow");
	pbutton->setAttribute(L"pos", L"345,261,0,0");
	pbutton->setAttribute(L"height", L"39");
	pbutton->setAttribute(L"width", L"104");
	pbutton->setAttribute(L"image", L"iknow");
	pbutton->setAttribute(L"normalimage", L"0");
	pbutton->setAttribute(L"hotimage", L"1");
	pbutton->setAttribute(L"pushedimage", L"2");
	pRoot->add(pbutton);
	// “不再显示”按钮  此处要美工重新修正
	pbutton = new wuc::buttonUC();
	pbutton->setName(L"btnNoDisplay");
	pbutton->setAttribute(L"ascheckbox", L"true");
	pbutton->setAttribute(L"pos", L"15,270,0,0");
	pbutton->setAttribute(L"height", L"20");
	pbutton->setAttribute(L"width", L"20");
	//pbutton->setAttribute(L"height", L"20");
	//pbutton->setAttribute(L"width", L"100");
	pbutton->setAttribute(L"image", L"unselected");
	pbutton->setAttribute(L"selectedimage", L"selected");
	pbutton->setAttribute(L"normalimage", L"0");
	//pbutton->setAttribute(L"hotimage", L"1");
	//pbutton->setAttribute(L"pushedimage", L"2");
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
	// “帧”动画
	wuc::animateFrameUC *pAnimate = new wuc::animateFrameUC();
	pAnimate->setName(L"animateFrame");
	pAnimate->setAttribute(L"pos", L"25,70,0,0");
	pAnimate->setAttribute(L"height", L"175");
	pAnimate->setAttribute(L"width", L"175");
	pAnimate->setAttribute(L"frameImage", L"animate");
	pAnimate->setAttribute(L"frameElapse", L"400");
	pRoot->add(pAnimate);

	m_drawManager.attachDialog(pRoot);
	m_drawManager.addNotifier(this);

	return 0;
}

LRESULT kpdeskNotify::onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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