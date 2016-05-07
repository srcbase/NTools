// testwinUI.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "testwinUI.h"
//#include "AC/sysupport.h"
class CFrameWindowWnd : public windowWnd, public INotifyUI
{
public:
	CFrameWindowWnd() { };
	PCWSTR getWindowClassName() const { return L"UIMainFrame"; };
	UINT getClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	void onFinalMessage(HWND /*hWnd*/) { delete this; };

	void init() { }

	bool onHChanged(void* param) {
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if( pMsg->sType == L"valuechanged" ) {
			short H, S, L;
			paintManagerUI::getHSL(&H, &S, &L);
			paintManagerUI::setHSL(true, (static_cast<sliderUI*>(pMsg->pSender))->getValue(), S, L);
		}
		return true;
	}

	bool onSChanged(void* param) {
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if( pMsg->sType == L"valuechanged" ) {
			short H, S, L;
			paintManagerUI::getHSL(&H, &S, &L);
			paintManagerUI::setHSL(true, H, (static_cast<sliderUI*>(pMsg->pSender))->getValue(), L);
		}
		return true;
	}

	bool onLChanged(void* param) {
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if( pMsg->sType == L"valuechanged" ) {
			short H, S, L;
			paintManagerUI::getHSL(&H, &S, &L);
			paintManagerUI::setHSL(true, H, S, (static_cast<sliderUI*>(pMsg->pSender))->getValue());
		}
		return true;
	}

	bool onAlphaChanged(void* param) {
		TNotifyUI* pMsg = (TNotifyUI*)param;
		if( pMsg->sType == L"valuechanged" ) {
			m_pm.setTransparent((static_cast<sliderUI*>(pMsg->pSender))->getValue());
		}
		return true;
	}

	void onPrepare() 
	{
		sliderUI* pSilder = static_cast<sliderUI*>(m_pm.findControl(L"alpha_controlor"));
		if( pSilder ) pSilder->onNotify += makeDelegate(this, &CFrameWindowWnd::onAlphaChanged);
		pSilder = static_cast<sliderUI*>(m_pm.findControl(L"h_controlor"));
		if( pSilder ) pSilder->onNotify += makeDelegate(this, &CFrameWindowWnd::onHChanged);
		pSilder = static_cast<sliderUI*>(m_pm.findControl(L"s_controlor"));
		if( pSilder ) pSilder->onNotify += makeDelegate(this, &CFrameWindowWnd::onSChanged);
		pSilder = static_cast<sliderUI*>(m_pm.findControl(L"l_controlor"));
		if( pSilder ) pSilder->onNotify += makeDelegate(this, &CFrameWindowWnd::onLChanged);
	}

	void notify(TNotifyUI& msg)
	{
		if( msg.sType == L"windowinit" ) onPrepare();
		else if( msg.sType == L"click" ) {
			if( msg.pSender->getName() == L"insertimagebtn" ) {
				// DAI
				//CRichEditUI* pRich = static_cast<CRichEditUI*>(m_pm.FindControl(L"testrichedit")));
				//if( pRich ) {
				//	pRich->RemoveAll();
				//}
			}
			else if( msg.pSender->getName() == L"changeskinbtn" ) {
				if( paintManagerUI::getResourcePath() == paintManagerUI::getInstancePath() )
					paintManagerUI::setResourcePath((paintManagerUI::getInstancePath() + L"skin\\FlashRes").c_str());
				else
					paintManagerUI::setResourcePath(paintManagerUI::getInstancePath().c_str());
				paintManagerUI::reloadSkin();
			}
		}
	}

	LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if( uMsg == WM_CREATE ) {
			m_pm.init(m_hWnd);
			dialogBuilder builder;
			controlUI* pRoot = builder.create(L"test1.xml", (UINT)0, NULL, &m_pm);
			assert(pRoot && "Failed to parse XML");
			m_pm.attachDialog(pRoot);
			m_pm.addNotifier(this);
			init();
			return 0;
		}
		else if( uMsg == WM_DESTROY ) {
			::PostQuitMessage(0L);
		}
		else if( uMsg == WM_ERASEBKGND ) {
			return 1;
		}
		LRESULT lRes = 0;
		if( m_pm.messageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return windowWnd::handleMessage(uMsg, wParam, lParam);
	}

public:
	paintManagerUI m_pm;
};

class CDeskWnd : public wui::windowWnd, public wui::INotifyUI
{
	virtual PCWSTR getWindowClassName() const { return L"WUIDeskWnd"; }
	LRESULT onNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lrst = S_OK;

		return lrst;
	}
	virtual void    notify(TNotifyUI& msg) 
	{
		if (msg.sType == L"click")
		{
			if (msg.pSender->getName() == L"btnHello")
			{
				MessageBox(NULL, L"hello world", L"hello", 0);
			}
		}
	}

	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch(uMsg)
		{
		case WM_CREATE:
			{
				m_paintManager.init(m_hWnd);	// 准备绘图环境

				dialogBuilder builder;
				controlUI* pRoot = builder.create(L"duilib.xml", (UINT)0, NULL, &m_paintManager);   // duilib.xml需要放到exe目录下
				assert(pRoot && L"Failed to parse XML");

				m_paintManager.attachDialog(pRoot);
				m_paintManager.addNotifier(this);   // 添加控件等消息响应，这样消息就会传达到duilib的消息循环，我们可以在Notify函数里做消息处理

				//wui::controlUI *pWnd = new wui::buttonUI();
				//pWnd->setText(L"Hello World");   // 设置文字
				//pWnd->setBkColor(0xFF00FF00);       // 设置背景色
				//pWnd->setName(L"btnHello");

				//m_paintManager.attachDialog(pWnd);	// initControls pControl->setManager pControl->findControl * pManager->m_mNameHash
				//m_paintManager.addNotifier(this);
				return lRes;
			}
			break;
		case WM_DESTROY:
			{
				::PostQuitMessage(0L);
			}
			break;
		case WM_ERASEBKGND:
			return 1;
			break;
		case WM_NCACTIVATE: // 以下3个消息WM_NCACTIVATE、WM_NCCALCSIZE、WM_NCPAINT用于屏蔽系统标题栏
			{
				if( !::IsIconic(m_hWnd) ) 
				{
					return (wParam == 0) ? TRUE : FALSE;
				}
			}
			break;
		case WM_NCCALCSIZE:
			return 0;
			break;
		case WM_NCPAINT:
			return 0;
			break;
		case WM_NCHITTEST:
			lRes = onNcHitTest(uMsg, wParam, lParam, bHandled);
			//MessageBox(NULL, L"WM_NCHITTEST", L"msg", 0);
			break;
		case WM_GETMINMAXINFO:
			//MessageBox(NULL, L"WM_GETMINMAXINFO", L"msg", 0);
			break;
		case WM_SIZE:
			//MessageBox(NULL, L"WM_SIZE", L"msg", 0);
			break;
		case WM_SYSCOMMAND:
			//MessageBox(NULL, L"WM_SYSCOMMAND", L"msg", 0);
			break;
		}
		

		if( m_paintManager.messageHandler(uMsg, wParam, lParam, lRes) ) 
		{
			return lRes;
		}

		return __super::handleMessage(uMsg, wParam, lParam);
	}

protected:
	paintManagerUI m_paintManager;

};

class CDuiFrameWnd : public wui::windowImplBase
{
public:
	virtual PCWSTR getWindowClassName() const { return L"WUIDeskWnd"; }
	virtual std::wstring getSkinFile()                {   return L"duilib.xml";  }
	//virtual std::wstring getSkinFolder()              {   return L"D:\\NTools\\UI\\win\\bin\\";  }
	virtual void    notify(TNotifyUI& msg) 
	{
		if (msg.sType == L"click")
		{
			if (msg.pSender->getName() == L"btnHello")
			{
				MessageBox(NULL, L"hello world", L"hello", 0);
			}
			else if (msg.pSender->getName() == L"closebtn")
			{
				PostQuitMessage(0);
			}
		}
	}
};

//IID_IShellFolder
#define EXPAND_SHLGUID(name, l, w1, w2) EXPAND_GUID(name, l, w1, w2, 0xC0,0,0,0,0,0,0,0x46)
#define EXPAND_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	EXTERN_C const GUID DECLSPEC_SELECTANY name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

EXPAND_SHLGUID(EXIID_IShellFolder, 0x000214E6L, 0, 0);

static int PrintDesktop()
{
	LPMALLOC pMalloc;
	LPITEMIDLIST pidlProgFiles = NULL;
	LPITEMIDLIST pidlItems = NULL;
	IShellFolder *psfFirstFolder = NULL;
	IShellFolder *psfDeskTop = NULL;
	IShellFolder *psfProgFiles = NULL;
	LPENUMIDLIST ppenum = NULL;
	ULONG celtFetched;
	HRESULT hr;
	STRRET strDispName;
	TCHAR pszDisplayName[MAX_PATH];
	ULONG uAttr;

	CoInitialize( NULL );
	hr = SHGetMalloc(&pMalloc);

	hr = SHGetDesktopFolder(&psfDeskTop);

	//hr = psfDeskTop->QueryInterface(EXIID_IShellFolder,(LPVOID *) &psfProgFiles);

	//psfDeskTop->Release();
	psfProgFiles = psfDeskTop;
	hr = psfProgFiles->EnumObjects(NULL,SHCONTF_FOLDERS | SHCONTF_NONFOLDERS , &ppenum);
	std::wostringstream out;
	while( hr = ppenum->Next(1,&pidlItems, &celtFetched) == S_OK && (celtFetched) == 1)
	{
		psfProgFiles->GetDisplayNameOf(pidlItems, SHGDN_INFOLDER, &strDispName);
		StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);
		printf("%s\n",pszDisplayName);
		out << pszDisplayName << L"  " << std::endl;
		
		if(!psfFirstFolder)
		{
			uAttr = SFGAO_FOLDER;
			psfProgFiles->GetAttributesOf(1, (LPCITEMIDLIST *) &pidlItems, &uAttr);
			if(uAttr & SFGAO_FOLDER)
			{
				//Here Can Do Enum Sub Folder.
				//hr = psfProgFiles->BindToObject(pidlItems, NULL, IID_IShellFolder, (LPVOID *) &psfFirstFolder);
				//psfFirstFolder->Release();
				//psfFirstFolder = NULL;
			}
		}
		pMalloc->Free(pidlItems);
	}
	printf("\n",pszDisplayName);
	//gtc::app::message(out.str().c_str());
	pMalloc->Free(pidlProgFiles);
	pMalloc->Release();
	psfProgFiles->Release();

	CoUninitialize();
	return 0;
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	PrintDesktop();
	return 0;
	/*HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;
	paintManagerUI::setInstance(hInstance);
	paintManagerUI::setResourcePath(paintManagerUI::getInstancePath().c_str());

	//CDuiFrameWnd duiFrame;
	//duiFrame.create(NULL, L"测试", UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	//duiFrame.centerWindow();
	//duiFrame.showModal();



	kpdeskWnd* pFrame = new kpdeskWnd();
	if( pFrame == NULL ) return 0;
	pFrame->create(NULL, L"测试", pFrame->getWindowStyle(), pFrame->getWindowExStyle());
	pFrame->centerWindow();
	pFrame->showWindow(true);
	paintManagerUI::messageLoop();

	::CoUninitialize();
	return 0;*/
}
