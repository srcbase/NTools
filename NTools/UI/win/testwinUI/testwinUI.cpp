// testwinUI.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "testwinUI.h"
#include "AC/sysupport.h"
//#include "KP/KPDeskInfo.h"
//#include "AC/resourceManager.h"
#include "kpdeskNotify.h"
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")

static HWND gc_hList = nullptr;

//
//class CDuiFrameWnd : public wui::windowImplBase
//{
//public:
//	virtual PCWSTR getWindowClassName() const { return L"WUIDeskWnd"; }
//	virtual std::wstring getSkinFile()                {   return L"duilib.xml";  }
//	//virtual std::wstring getSkinFolder()              {   return L"D:\\NTools\\UI\\win\\bin\\";  }
//	virtual void    notify(TNotifyUI& msg) 
//	{
//		if (msg.sType == L"click")
//		{
//			if (msg.pSender->getName() == L"btnHello")
//			{
//				MessageBox(NULL, L"hello world", L"hello", 0);
//			}
//			else if (msg.pSender->getName() == L"closebtn")
//			{
//				PostQuitMessage(0);
//			}
//		}
//	}
//};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	gtc::app::init();
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	

	paintManagerUI::setInstance(hInstance);
	paintManagerUI::setResourcePath(paintManagerUI::getInstancePath().c_str());

	//CDuiFrameWnd duiFrame;
	//duiFrame.create(NULL, L"²âÊÔ", UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	//duiFrame.centerWindow();
	//duiFrame.showModal();



	kpdeskNotify* pFrame = new kpdeskNotify();
	if( pFrame == NULL ) return 0;
	pFrame->create(NULL, L"²âÊÔ", pFrame->getWindowStyle(), pFrame->getWindowExStyle());
	pFrame->centerWindow();
	pFrame->showModal();
	//pFrame->showWindow(true);
	//paintManagerUI::messageLoop();

	::CoUninitialize();
	return 0;
}
