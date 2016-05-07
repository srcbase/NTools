#include "StdAfx.h"
#include "dialogMain.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	dialogMain  类		2015-7-24
//	主框架
////////////////////////////////////////////////////////////////////////////////////////////////////////
template<> dialogMain* gtc::singleton<dialogMain>::m_pSingleton = NULL;
dialogMain& dialogMain::getSingleton()
{
	assert(m_pSingleton);
	return *m_pSingleton;
}
dialogMain* dialogMain::getSingletonPtr()
{
	return m_pSingleton;
}

dialogMain::dialogMain(void)
	: m_pTitleBar(nullptr), m_pToolBar(nullptr), m_pClientUC(nullptr), m_pTaskBar(nullptr)
	, m_labelCaption(nullptr), m_labelState(nullptr), m_buttonMinBox(nullptr), m_buttonClose(nullptr)
	, m_buttonSave(nullptr), m_buttonClear(nullptr)
	, m_web(nullptr)
{
	memset(&m_nid, 0, sizeof(m_nid));
}


dialogMain::~dialogMain(void)
{
}

LRESULT dialogMain::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam/*, BOOL &bHandled*/)
{
	LRESULT lRes = 0;
	bool bHandled = true;
	if (uMsg == wuc::menuUC::getSelfRegMsg())
	{
		onMenuCommand(wParam, lParam);
		//return 1;
	}
	switch (uMsg)
	{
	case WM_SGLW_TRAY:
		return onMessageTray(wParam, lParam);
		break;
	case WM_COPYDATA:
		{
			COPYDATASTRUCT *pcds = (COPYDATASTRUCT *)lParam;
			if (pcds && ((pcds->dwData >> 16) & 0xFFFF) == DBGER_LOG_TYPE)
			{
				gtc::BELogType lgtype = (gtc::BELogType)(pcds->dwData & 0xFFFF);
				addLog(lgtype, (PCWSTR)pcds->lpData);
			}
		}
		break;
	case WM_DESTROY:
		{
			SEND_LOG(L"dialogMain::handleMessage WM_DESTROY");
			RECT rcPos = {0};
			GetWindowRect(m_hWnd, &rcPos);
			gtc::app::writeProfile(L"MAIN", L"MAINPOS", rcPos);

			PostQuitMessage(0);
		}
		break;
	case WM_CREATE:
		lRes = onCreate();
		break;
	case WM_NCHITTEST:
		lRes = onNcHitTest(uMsg, wParam, lParam, bHandled);
		break;
	case WM_SIZE:
		//InvalidateRect(m_hWnd, NULL, TRUE);
		m_drawManager.getRoot()->needUpdate();
		bHandled = false;
		{

			SIZE szRoundCorner = m_drawManager.getRoundCorner();
			//szRoundCorner.cx += 1;
			//szRoundCorner.cy += 1;
#if defined(WIN32) && !defined(UNDER_CE)
			if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
				RECT rcWnd;
				::GetWindowRect(*this, &rcWnd);
				::OffsetRect(&rcWnd, -rcWnd.left, -rcWnd.top);
				rcWnd.right++; rcWnd.bottom++;
				HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
				::SetWindowRgn(*this, hRgn, TRUE);
				::DeleteObject(hRgn);
			}
#endif
		}

		break;
		//case WM_ENTERSIZEMOVE:
		//	m_drawManager.setLayerWindowTransparent(true);
		//	break;
		//case WM_EXITSIZEMOVE:
		//	m_drawManager.setLayerWindowTransparent(false);
		//	break;
	default:
		bHandled = false;
		break;
	}
	//if(!bHandled && !wuc::windowWnd::isOutMsg(uMsg))
	//	SEND_LOG(L"dialogMain::handleMessage msg:%s wp:%08X lp:%08X", wuc::windowWnd::stringMsg(uMsg).c_str(), wParam, lParam);

	//bool bHandled = bDone;
	if(bHandled) return lRes;
	if(m_drawManager.messageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return windowWnd::handleMessage(uMsg, wParam, lParam);
	//return windowWTL::handleMessage(uMsg, wParam, lParam, bHandled);
}
void dialogMain::onFinalMessage(HWND hWnd)
{
	SEND_LOG(L"dialogMain::onFinalMessage");
	__super::onFinalMessage(hWnd);
}
void dialogMain::notify(wuc::BSNotifyUC& msg)
{
	//SEND_LOG(L"dialogMain::notify type:<%s> sender:<%s>", msg.wsType.c_str(), msg.pSender->getName().c_str());
	if (msg.wsType == L"click")
	{
		SEND_LOG(L"dialogMain::notify click sender:<%s>", msg.pSender->getName().c_str());
		if (msg.pSender == m_buttonMinBox)
		{
			onClickMinbox();
		}
		else if (msg.pSender == m_buttonClose)
		{
			onClickClose();
		}
		else if (msg.pSender == m_buttonSave)
		{
			onClickSave();
		}
		else if (msg.pSender == m_buttonClear)
		{
			onClickClear();
		}
	}
	else if (msg.wsType == L"wke_urlchanged" && msg.pSender == m_web)
	{
	}
	else if (msg.wsType == L"wke_documentready" && msg.pSender == m_web)
	{

	}

	else if (msg.wsType == L"windowinit")
	{
		onInit();
	}
}

LRESULT dialogMain::onCreate()
{
	const int title_cy = 40;				// 标题高
	const int title_left_cx = 40;			// 
	const int tool_cy = 40;					// 
	const int task_cy = 30;					// 任务栏高
	const RECT client_inset = {4, 1, 4, 1};	// 客户区内缩
	const DWORD argb_title = 0xFFFFFFFF;	// 标题文本色
	const DWORD	argb_state_text = 0xFF00CC99;	// 状态栏文本色
	// 设置本窗口
	m_drawManager.init(m_hWnd);
	m_drawManager.setBackgroundTransparent(true);
	//m_drawManager.setLayerWindowTransparent(true);
	m_drawManager.setAttribute(L"roundcorner", L"5,5");
	m_drawManager.setMinInfo(0, 350);
	m_drawManager.setCaptionRect(title_cy);
	RECT rcSizeBox = {4, 4, 4, 4};
	m_drawManager.setSizeBox(rcSizeBox);
	// 加载控件
	wuc::verticalLayoutUC *pRoot = new wuc::verticalLayoutUC();
	assert(pRoot);
	m_drawManager.attachDialog(pRoot);
	m_drawManager.addNotifier(this);

	pRoot->setName(L"rootBk");

	//pRoot->setBkColor(0xff00ff00);		// 四个角隐隐约约是绿的
	pRoot->setAttribute(L"image", L"name='main_bk' corner='5,40,5,5'");
	pRoot->setBkImage(0,0,40,80);
	
	wuc::verticalLayoutUC *pver = nullptr;
	wuc::horizontalLayoutUC *phor = nullptr;
	SIZE sz = {0};
	// 标题栏
	m_pTitleBar = new wuc::horizontalLayoutUC();
	pRoot->add(m_pTitleBar);
	m_pTitleBar->setName(L"windowTitle");
	//m_pTitleBar->setBkColor(0xff00ff00);
	m_pTitleBar->setFixedHeight(title_cy);
	//m_pTitleBar->setAttribute(L"inset", L"2,1,2,1");		// m_szxyBorderRound.cx/2, m_szxyBorderRound.cy/2
	// 标题栏--> 再分区 --> 左
	pver = new wuc::verticalLayoutUC();
	m_pTitleBar->add(pver);
	//pver->setBkColor(0x8099CCFF);
	pver->setFixedWidth(title_left_cx);
	// 标题栏--> 再分区 --> 中
	pver = new wuc::verticalLayoutUC();
	//pver->setBkColor(0x809966CC);
	pver->setAttribute(L"inset", L"4,6,4,6");
	phor = new wuc::horizontalLayoutUC();
	pver->add(phor);
	m_labelCaption = new wuc::labelUC();
	//m_plabelCaption->setName(KPSGLW_NAME_LABEL_CAPTION);
	//m_plabelCaption->setBkColor(0xFFFFCCCC);
	m_labelCaption->setTextColor(argb_title);
	m_labelCaption->setFont(g_ifontMSYH18);
	m_labelCaption->setAttribute(L"align", L"center");
	m_labelCaption->setAttribute(L"endellipsis", L"true");
	//m_plabelCaption->setFixedHeight(30);
	phor->add(m_labelCaption);
	m_pTitleBar->add(pver);
	// 标题栏--> 再分区 --> 右
	sz.cx = 26;
	sz.cy = 26;
	pver = new wuc::verticalLayoutUC();
	//pver->setBkColor(0x803300FF);
	pver->setFixedWidth(sz.cx * 2);
	phor = new wuc::horizontalLayoutUC();
	phor->setFixedHeight(sz.cy);
	pver->add(phor);
	m_buttonMinBox = new wuc::buttonUC();
	//m_buttonMinBox->setName(KPSGLW_NAME_BUTTON_MINBOX);
	m_buttonMinBox->setAttribute(L"image", L"button_minbox");
	m_buttonMinBox->setAttribute(L"normalimage",	gtc::format::rect(sliceAreaCell(0, sz)).c_str());//	L"0,0,	26,26");
	m_buttonMinBox->setAttribute(L"hotimage",		gtc::format::rect(sliceAreaCell(1, sz)).c_str());//	L"26,0,	52,26");
	m_buttonMinBox->setAttribute(L"pushedimage",	gtc::format::rect(sliceAreaCell(2, sz)).c_str());//	L"52,0,	78,26");
	//m_pbtnMinbox->setBkColor(0xEF00FF00);
	m_buttonClose = new wuc::buttonUC();
	//m_buttonClose->setName(KPSGLW_NAME_BUTTON_CLOSE);
	m_buttonClose->setAttribute(L"image", L"button_close");
	m_buttonClose->setAttribute(L"normalimage",	gtc::format::rect(sliceAreaCell(0, sz)).c_str());//	L"0,0,	26,26");
	m_buttonClose->setAttribute(L"hotimage",		gtc::format::rect(sliceAreaCell(1, sz)).c_str());//	L"26,0,	52,26");
	m_buttonClose->setAttribute(L"pushedimage",	gtc::format::rect(sliceAreaCell(2, sz)).c_str());//	L"52,0,	78,26");
	//m_pbtnClose->setBkColor(0xEFFF0000);
	phor->add(m_buttonMinBox);
	phor->add(m_buttonClose);
	phor = new wuc::horizontalLayoutUC();
	pver->add(phor);
	m_pTitleBar->add(pver);
	// 工具栏
	m_pToolBar = new wuc::horizontalLayoutUC();
	pRoot->add(m_pToolBar);
	m_pToolBar->setName(L"windowTool");
	m_pToolBar->setFixedHeight(tool_cy);
	pver = new wuc::verticalLayoutUC();
	m_pToolBar->add(pver);
	pver->setFixedWidth(client_inset.left);
	pver = new wuc::verticalLayoutUC();
	m_pToolBar->add(pver);
	pver->setFixedWidth(tool_cy*1.5);
	pver->setInset(2,2,2,2);
	phor = new wuc::horizontalLayoutUC();
	pver->add(phor);
	m_buttonSave = new wuc::buttonUC();
	phor->add(m_buttonSave);
	m_buttonSave->setText(L"保存");
	m_buttonSave->setFont(g_ifontMSYH14);
	m_buttonSave->setBkColor(0xFFCC99FF);
	// 
	pver = new wuc::verticalLayoutUC();
	m_pToolBar->add(pver);
	pver->setFixedWidth(tool_cy*1.5);
	pver->setInset(2,2,2,2);
	phor = new wuc::horizontalLayoutUC();
	pver->add(phor);
	m_buttonClear = new wuc::buttonUC();
	phor->add(m_buttonClear);
	m_buttonClear->setText(L"清空");
	m_buttonClear->setFont(g_ifontMSYH14);
	m_buttonClear->setBkColor(0xFFCC99FF);

	// 客户区
	m_pClientUC = new wuc::horizontalLayoutUC();
	pRoot->add(m_pClientUC);
	m_pClientUC->setName(L"windowClient");
	m_pClientUC->setInset(client_inset);
	//m_pClientUC->setAttribute(L"inset", L"4,1,4,1");
	m_web = new wuc::wkeWebkitUC();
	m_pClientUC->add(m_web);
	m_web->setURL(L"www.ifeng.com");
	// 任务栏
	m_pTaskBar = new wuc::horizontalLayoutUC();
	pRoot->add(m_pTaskBar);
	//m_pTaskBar->setBkColor(0xff0000ff);
	m_pTaskBar->setName(L"windowTask");
	m_pTaskBar->setFixedHeight(task_cy);
	m_pTaskBar->setAttribute(L"inset", L"12,2,12,2");
	m_labelState = new wuc::labelUC();
	m_pTaskBar->add(m_labelState);
	m_labelState->setTextColor(argb_state_text);
	m_labelState->setFont(g_ifontMSYH14);
	// 任务栏 --> 按钮

	m_buttonClose->setToolTip(L"关闭");
	m_buttonClear->setToolTip(L"清空");
	return 0;
}
LRESULT dialogMain::onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
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

void dialogMain::setCaption(PCWSTR pTitle)
{
	assert(m_labelCaption);
	m_labelCaption->setText(pTitle);
}
void dialogMain::setStateText(PCWSTR pState)
{
	assert(m_labelState);
	m_labelState->setText(pState);
}

jsValue JS_CALL js_msgBox(jsExecState es)
{
	const wchar_t* text = jsToStringW(es, jsArg(es, 0));
	SEND_LOG(L"js_msgBox url:<%s>", text);
	gtc::app::exec(text);
	return jsUndefined();
}
jsValue JS_CALL js_msgBox2(jsExecState es)
{
	const wchar_t* text = jsToStringW(es, jsArg(es, 0));
	SEND_LOG(L"js_msgBox2 url:<%s>", text);
	gtc::app::exec(text);
	return jsUndefined();
}

void dialogMain::onInit()
{
	setCaption(L"调试日志");
	setStateText(L"就绪...");
	// 默认HTML
	onClickClear();

	m_web->buildJSFunction("newWindowOpen", js_msgBox, 1);
	m_web->buildJSFunction("clickTaobaoUrl", js_msgBox2, 1);
}

void dialogMain::onMenuCommand(WPARAM wParam, LPARAM lParam)
{
	SEND_LOG(L"NOTE dialogMain::onMenuCommand 正在执行菜单命令 cmdID:%d state:%08X", wParam, lParam);
	switch (wParam)
	{
	case DBGER_MENU_ID_EXIT:
		{
			//SEND_LOG(L"dialogMain::onMenuCommand 退出");
			close();
		}
		break;
	}
}
LRESULT dialogMain::onMessageTray(WPARAM wParam, LPARAM lParam)
{
	WORD msgFlag = LOWORD(lParam);
	switch(msgFlag)
	{
	case WM_LBUTTONDBLCLK:
		{
			if (isWindowVisible()) showWindow(false);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if (!isWindowVisible()) showWindow(true);
		}
		break;
	case WM_CONTEXTMENU:
	case WM_RBUTTONDOWN:
		{
			wuc::menuUC *pmenu = new wuc::menuUC();
			pmenu->applyAttributeList(L"Menu");
			pmenu->registerMessageWnd(m_hWnd);
			//pmenu->OnMenuCommand += gtc::makeDelegate(this, &dialogMain::onMenuCommand);
			//RECT rc = {0,0,35,35};
			//pmenu->setImage(L"name='menu_bk' corner='2,2,2,2'");
			//pmenu->setBkImage(rc);
			//pmenu->setItemTextColor(0xFF000000);
			//pmenu->setItemImage(L"menu_item_hot");
			//rc.right -= 1;
			//pmenu->setItemHotImage(rc);
			wuc::menuElementUC *pelem = pmenu->addMenu(DBGER_MENU_ID_EXIT, L"退出", pmenu->getNormalAttributeList(L"main_icon", &sliceAreaCell(3, 21, false, 4)).c_str());
			//pmenu->addSeparator();

			//wuc::menuElementUC::PtrArrayType vecSub;
			//vecSub.push_back(pmenu->createMenu(11, L"3aaaaaaaaaaaaaa3"));
			//vecSub.push_back(pmenu->createMenu(12, L"2bbbbbbbbbbb2"));
			//vecSub.push_back(pmenu->createMenu(13, L"1cc1"));
			//pelem = pmenu->addMenu(L"exit ...", vecSub);
			//pelem->add(pmenu->createMenu(17, L"1ttttt1"));
			//pelem->add(pmenu->createMenu(18, L"2trrrrrrrrrrrrrrrrrrr2"));
			POINT pt = {0};
			GetCursorPos(&pt);
			pmenu->trackPopupMenu(m_hWnd, pt.x, pt.y, wuc::BEMenuAlignBottom);
			//HMENU hMenu = m_drawManager.findMenu(L"menu_tray");
			//if(!hMenu) return FALSE;
			//HMENU hTrackMenu = GetSubMenu(hMenu, 0);
			//if (hTrackMenu)
			//{
			//	POINT pt = {0};
			//	GetCursorPos(&pt);
			//	UINT idCmd = ::TrackPopupMenu(hTrackMenu, TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD, pt.x, pt.y, 0, m_hWnd, nullptr);
			//	onMenuTray(idCmd);
			//}
		}
		break;
	default:
		break;
	}
	return TRUE;
}

void dialogMain::onClickMinbox()
{
	//sendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	showWindow(SW_HIDE);
}
// m_pbtnClose对应的click事件
void dialogMain::onClickClose()
{
	close();

}

void dialogMain::onClickSave()
{
	//gtc::file::browseFile()
}
void dialogMain::onClickClear()
{
	assert(m_web);
	m_web->setFile(L".\\logs.html");

}
bool dialogMain::create()
{
	RECT rcWork = {0};  
	SystemParametersInfo(SPI_GETWORKAREA, NULL, (PVOID)&rcWork, 0);  
	//SIZE sss = {GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN)};
	SIZE szWall = {rectWidth(rcWork), rectHeight(rcWork)};
	RECT rcPos = {0,0,500,400};
	bool bl = gtc::app::readProfileRect(L"MAIN", L"MAINPOS", rcPos);
	if (!bl)
	{
		::OffsetRect(&rcPos, (rectWidth(rcWork) - rectWidth(rcPos))/2, (rectHeight(rcWork) - rectHeight(rcPos))/2);
	}
	HWND hWnd = __super::create(NULL, rcPos, DBGER_WND_MAIN_CAPTION);
	if (!hWnd)
	{
		close();
	}
	else
	{
		// 低权限app往高权限app发送消息要注册
		//if(!gtc::app::addFilterWindowMessage(m_hWnd, gc_uiKPDesktopExit))
		//{
		//	SEND_LOG(L"ERROR dialogMain::create 过滤自定义消息<gc_uiKPDesktopExit> lasterr:%u", GetLastError());
		//}
		//if(!gtc::app::addFilterWindowMessage(m_hWnd, gc_uiKPDesktopRun))
		//{
		//	SEND_LOG(L"ERROR dialogMain::create 过滤自定义消息<gc_uiKPDesktopRun> lasterr:%u", GetLastError());
		//}
		//gtc::app::addFilterWindowMessage(m_hWnd, KPDESK_WMME_WALLPAPER);
		//gtc::app::addFilterWindowMessage(m_hWnd, KPDESK_WMME_DESK_CHANGENOTIFY);
		//gtc::app::addFilterWindowMessage(m_hWnd, KPDESK_WMME_ADDNODE);
		//gtc::app::addFilterWindowMessage(m_hWnd, KPDESK_WMME_DELETENODE);
		//gtc::app::addFilterWindowMessage(m_hWnd, KPDESK_WMME_UPDATENODE);


	}
	assert(hWnd);

	return hWnd != NULL;
}


bool dialogMain::createTray()
{
	NOTIFYICONDATA nid = {sizeof(NOTIFYICONDATA)};
	::ZeroMemory(&m_nid, sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
	m_nid.hWnd = m_hWnd;
	m_nid.uID = IDI_SMALL;// (UINT)this;
	m_nid.hIcon = LoadIcon(m_drawManager.getInstance(), MAKEINTRESOURCE(IDI_SMALL));
	m_nid.uCallbackMessage = WM_SGLW_TRAY;
	std::swprintf(m_nid.szTip, _countof(m_nid.szTip), L"调试日志输出");
	bool bRet = Shell_NotifyIcon(NIM_ADD, &m_nid) == TRUE;
	//if (!bRet)
	//{
	//	SEND_LOG(L"ERROR dialogMain::createTray 创建任务栏图标失败 #%u", GetLastError());
	//}

	return bRet;
}
void dialogMain::destroyTray()
{
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
}

void dialogMain::modifyTray()
{
	std::swprintf(m_nid.szTip, _countof(m_nid.szTip), L"调试日志输出");
	bool bRet = Shell_NotifyIcon(NIM_MODIFY, &m_nid) == TRUE;
	if (!bRet)
	{
		//SEND_LOG(L"ERROR dialogMain::modifyTray 修改任务栏图标失败 #%u", GetLastError());
	}
}


void dialogMain::addLog(gtc::BELogType logType, PCWSTR pInfo)
{
	static DWORD c_oldTick = GetTickCount();
	wchar_t pbuf[40] = {0};
	std::swprintf(pbuf, _countof(pbuf), L"%d", GetTickCount() - c_oldTick);
	c_oldTick = GetTickCount();
	//assert(m_web);
	if(!m_web) return;
	//m_web->runJS(L"addLog(log_flag.note, '2001-12-12 12:32:33', 'AAAAAAAAABBBBBBBBBB阿海ii')");
	gtc::datetime dtcur;
	std::wstring wsAdd;
	switch(logType)
	{
	case gtc::BELogTypeNormal:
		wsAdd = L"addLog(log_flag.normal, '";
		break;
	case gtc::BELogTypeNote:
		wsAdd = L"addLog(log_flag.note, '";
		break;
	case gtc::BELogTypeWarning:
		wsAdd = L"addLog(log_flag.warning, '";
		break;
	case gtc::BELogTypeError:
		wsAdd = L"addLog(log_flag.error, '";
		break;
	}
	wsAdd += dtcur.format() + L"', '";
	wsAdd += pbuf;
	wsAdd += L"', '";
	if(pInfo) wsAdd += pInfo;
	wsAdd += L"')";
	gtc::stringUtil::replaceAllRef(wsAdd, L"\x0A", L" ");
	m_web->runJS(wsAdd);
}