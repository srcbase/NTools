#include "wucWebBrowser.h"
#include <mshtml.h>
#include "downloadmgr.h"
#include "wucBase.h"
namespace wuc {
//////////////////////////////////////////////////////////////////////////////////////////
//	2015-8-13 
static std::wstring toStringIEDispid(DWORD dispID) 
{
	switch (dispID)
	{
	case DISPID_BEFORENAVIGATE				:	return L"id:BeforeNavigate_100 desc:this is sent before navigation to give a chance to abort";
	case DISPID_NAVIGATECOMPLETE			:	return L"id:NavigateComplete_101 desc:in async, this is sent when we have enough to show";
	//case DISPID_STATUSTEXTCHANGE			:	return L"id:StatusTextChange_102";
	case DISPID_QUIT						:	return L"id:Quit_103";
	case DISPID_DOWNLOADCOMPLETE			:	return L"id:DownloadComplete_104";
	//case DISPID_COMMANDSTATECHANGE			:	return L"id:CommandStateChange_105";
	case DISPID_DOWNLOADBEGIN				:	return L"id:DownloadBegin_106";
	case DISPID_NEWWINDOW					:	return L"id:NewWindow_107 desc:sent when a new window should be created";
	case DISPID_PROGRESSCHANGE				:	return L"id:ProgressChange_108 desc:sent when download progress is updated";
	case DISPID_WINDOWMOVE					:	return L"id:WindowMove_109 desc:sent when main window has been moved";
	case DISPID_WINDOWRESIZE				:	return L"id:WindowResize_110 desc:sent when main window has been sized";
	case DISPID_WINDOWACTIVATE				:	return L"id:WindowActivate_111 desc:sent when main window has been activated";
	case DISPID_PROPERTYCHANGE				:	return L"id:PropertyChange_112 desc:sent when the PutProperty method is called";
	case DISPID_TITLECHANGE					:	return L"id:TitleChange_113 desc:sent when the document title changes";
	case DISPID_TITLEICONCHANGE				:	return L"id:TitleIconChange_114 desc:sent when the top level window icon may have changed.";

	case DISPID_FRAMEBEFORENAVIGATE			:	return L"id:FrameBeforeNavigate_200";
	case DISPID_FRAMENAVIGATECOMPLETE		:	return L"id:FrameNavigateComplete_201";
	case DISPID_FRAMENEWWINDOW				:	return L"id:FrameNewWindow_204";

	case DISPID_BEFORENAVIGATE2				:	return L"id:BeforeNavigate2_250 desc:hyperlink clicked on";
	case DISPID_NEWWINDOW2					:	return L"id:NewWindow2_251";
	case DISPID_NAVIGATECOMPLETE2			:	return L"id:NavigateComplete2_252 desc:UIActivate new document";
	case DISPID_ONQUIT						:	return L"id:onQuit_253";
	case DISPID_ONVISIBLE					:	return L"id:onVisible_254 desc:sent when the window goes visible/hidden";
	case DISPID_ONTOOLBAR					:	return L"id:onToolbar_255 desc:sent when the toolbar should be shown/hidden";
	case DISPID_ONMENUBAR					:	return L"id:onMenubar_256 desc:sent when the menubar should be shown/hidden";
	case DISPID_ONSTATUSBAR					:	return L"id:onStatusbar_257 desc:sent when the statusbar should be shown/hidden";
	case DISPID_ONFULLSCREEN				:	return L"id:onFullScreen_258 desc:sent when kiosk mode should be on/off";
	case DISPID_DOCUMENTCOMPLETE			:	return L"id:DocumentComplete_259 desc:new document goes ReadyState_Complete";
	case DISPID_ONTHEATERMODE				:	return L"id:onTheaterMode_260 desc:sent when theater mode should be on/off";
	case DISPID_ONADDRESSBAR				:	return L"id:onAddressbar_261 desc:sent when the address bar should be shown/hidden";
	case DISPID_WINDOWSETRESIZABLE			:	return L"id:WindowSetResizable_262 desc:sent to set the style of the host window frame";
	case DISPID_WINDOWCLOSING				:	return L"id:WindowClosing_263 desc:sent before script window.close closes the window";
	case DISPID_WINDOWSETLEFT				:	return L"id:WindowSetLeft_264 desc:sent when the put_left method is called on the WebOC";
	case DISPID_WINDOWSETTOP				:	return L"id:WindowSetTop_265 desc:sent when the put_top method is called on the WebOC";
	case DISPID_WINDOWSETWIDTH				:	return L"id:WindowSetWidth_266 desc:sent when the put_width method is called on the WebOC";
	case DISPID_WINDOWSETHEIGHT				:	return L"id:WindowSetHeight_267 desc:sent when the put_height method is called on the WebOC ";
	case DISPID_CLIENTTOHOSTWINDOW			:	return L"id:ClientToHostWindow_268 desc:sent during window.open to request conversion of dimensions";
	case DISPID_SETSECURELOCKICON			:	return L"id:SetSecureLockIcon_269 desc:sent to suggest the appropriate security icon to show";
	case DISPID_FILEDOWNLOAD				:	return L"id:FileDownload_270 desc:Fired to indicate the File Download dialog is opening";
	case DISPID_NAVIGATEERROR				:	return L"id:NavigateError_271 desc:Fired to indicate the a binding error has occured";
	case DISPID_PRIVACYIMPACTEDSTATECHANGE	:	return L"id:PrivacyImpactedStateChange_272 desc:Fired when the user's browsing experience is impacted";
	case DISPID_NEWWINDOW3					:	return L"id:NewWindow3_273";
	case DISPID_VIEWUPDATE					:	return L"id:ViewUpdate_281 desc:Fired when the contents of a shell browser window change";
	case DISPID_SETPHISHINGFILTERSTATUS		:	return L"id:SetPhishingFilterStatus_282 desc:Fired by the Phishing Filter API to signal what state the analysis is in";
	case DISPID_WINDOWSTATECHANGED			:	return L"id:WindowStateChanged_283 desc:Fired to indicate that the browser window's visibility or enabled state has changed";
	case DISPID_NEWPROCESS					:	return L"id:NewProcess_284 desc:Fired when a navigation must be redirected due to Protected Mode";
	case DISPID_THIRDPARTYURLBLOCKED		:	return L"id:ThirdPartyURLBlocked_285 desc:Fired when a third-party url is blocked due to Privacy Advisor";
	case DISPID_REDIRECTXDOMAINBLOCKED		:	return L"id:RedirectxDomainBlocked_286 desc:Fired when a x-domain redirect is blocked due to browser nav constant";

		// Printing events
	case DISPID_PRINTTEMPLATEINSTANTIATION	:	return L"id:PrintTemplateInstantiation_225 desc:Fired to indicate that a print template is instantiated";
	case DISPID_PRINTTEMPLATETEARDOWN		:	return L"id:PrintTemplateTeardown_226 desc:Fired to indicate that a print templete is completely gone";
	case DISPID_UPDATEPAGESTATUS			:	return L"id:UpdatePageStatus_227 desc:Fired to indicate that the spooling status has changed";


	default:
		{
			wchar_t pbuf[40] = {0};
			std::swprintf(pbuf, _countof(pbuf), L"id:%u", dispID);
			return pbuf;
		}
		break;
	}
	return L"";
}
#define WEB_LOG	SEND_LOG
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	webBrowserUC  类		2015-8-11
//	对IE浏览器封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
webBrowserUC::webBrowserUC()
	: m_pWebBrowser2(NULL)
	//, m_pHtmlWnd2(NULL)
	, m_pWebBrowserEventHandler(NULL)
	, m_bAutoNavi(false)
	, m_dwRef(0)
	, m_dwCookie(0)
	, m_bFreeOnReleaseWBEH(false)
	, m_bInitScrollbar(false)
{
	m_clsid = CLSID_WebBrowser;
}
webBrowserUC::~webBrowserUC()
{
	releaseControl();
	//if(m_pHtmlWnd2)
	//{
	//	m_pHtmlWnd2->Release();
	//}
	if(m_pWebBrowser2) m_pWebBrowser2->Release();
	while(m_dwRef != 0)
		Release();
	
	if(m_bFreeOnReleaseWBEH && m_pWebBrowserEventHandler) delete m_pWebBrowserEventHandler;
}
PCWSTR webBrowserUC::getClass() const
{
	return L"webBrowserUC";
}
LPVOID webBrowserUC::getInterface(PCWSTR pstrName)
{
	if( _tcscmp(pstrName, L"WebBrowser") == 0 ) return static_cast<webBrowserUC*>(this);
	return activeXUC::getInterface(pstrName);
}

void webBrowserUC::Event(BSEventUC &event)
{
	if( !isMouseEnabled() && event.Type > BEUCEventMouseBegin && event.Type < BEUCEventMouseEnd ) {
		if( m_pParent != NULL ) m_pParent->Event(event);
		else __super::Event(event);
		return;
	}

	if( event.Type == BEUCEventScrollWheel )
	{
		switch( LOWORD(event.wParam) ) {
		case SB_LINEUP:
			lineUp(50);
			return;
		case SB_LINEDOWN:
			//SEND_LOG(L"containerUC::Event BEUCEventScrollWheel_V SB_LINEDOWN");
			lineDown(50);
			return;
		}
	}
	__super::Event(event);
}

void webBrowserUC::setPosition(RECT pos)
{
	__super::setPosition(pos);
	//if (m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible())
	//{
		SEND_LOG(L"ERROR webBrowserUC::setPosition");
	//RECT rc = {0};
	//GetWindowRect(m_hwndHost, &rc);
	//rc.right -= 8;
	////	//setBkColor(0xFF00FF00);
	//::MoveWindow(m_hwndHost, rc.left, rc.top, rectWidth(rc), rectHeight(rc), true);
	//	::SetWindowPos(m_hwndHost, NULL, 0, 0, rectWidth(rc), rectHeight(rc), SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
	//IHTMLWindow2 *pwin2 = getHtmlWindow();
	//if (pwin2)
	//{
	//	HRESULT hr = pwin2->resizeTo(rectWidth(rc), rectHeight(rc));
	//	DWORD dw = GetLastError();
	//	pwin2->Release();
	//}
	//	//rc.left = rc.right;
	//	//rc.right = rc.left + 8;
	//	//m_pManager->screenToClient(rc);
	//	//m_pVerticalScrollBar->setPosition(rc);
	//	////m_pVerticalScrollBar->invalidate();
	//}
}
void webBrowserUC::setScrollPos(SIZE szPos)
{
	//__super::setScrollPos(szPos);
	int cx = 0;
	int cy = 0;
	SIZE szOldPos = getScrollPos();

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

	IHTMLWindow2 *pWin2 = getHtmlWindow();
	if (pWin2)
	{
		pWin2->scrollBy(cx, cy);
	}
}

void webBrowserUC::releaseControl()
{
	assert(m_pManager);
	m_bCreated=false;
	getManager()->removeTranslateAccelerator(this);
	registerEventHandler(FALSE);
}
HRESULT webBrowserUC::registerEventHandler(BOOL inAdvise)
{
	IWebBrowser2 *pWebBrowser = NULL;
	IConnectionPointContainer *pCPC = NULL;
	IConnectionPoint *pCP = NULL;
	HRESULT hr = S_OK;
	try
	{
		hr = getControl(IID_IWebBrowser2, (void**)&pWebBrowser);
		if (FAILED(hr))
			throw L"调用getControl失败！";
		hr = pWebBrowser->QueryInterface(IID_IConnectionPointContainer,(void **)&pCPC);
		if (FAILED(hr))
			throw L"调用QueryInterface失败！";
		hr = pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2,&pCP);
		if (FAILED(hr))
			throw L"调用FindConnectionPoint失败！";

		if (inAdvise)
		{
			hr = pCP->Advise((IDispatch*)this, &m_dwCookie);
		}
		else
		{
			hr = pCP->Unadvise(m_dwCookie);
		}
	}
	catch (PCWSTR pinfo)
	{
		if(SUCCEEDED(hr)) hr = S_FALSE;
		SEND_LOG(L"ERROR webBrowserUC::registerEventHandler %s", pinfo);
	}
	catch (gtc::baseException &e)
	{
		if(SUCCEEDED(hr)) hr = S_FALSE;
		SEND_LOG(L"ERROR webBrowserUC::registerEventHandler %s", e.description().c_str());
	#if !defined(_DEBUG)
		WRITE_LOG(L"throw webBrowserUC::registerEventHandler %s", e.description().c_str());
	#endif
	}
	catch (...)
	{
		if(SUCCEEDED(hr)) hr = S_FALSE;
		SEND_LASTERROR();
	}
	// release
	if(pCP) pCP->Release();
	if(pCPC) pCPC->Release();
	if(pWebBrowser) pWebBrowser->Release();
	
	return hr; 
}
void webBrowserUC::setAttribute(PCWSTR pstrName, PCWSTR pstrValue)
{
	if (_tcscmp(pstrName, L"homepage") == 0) m_wsHomePage = pstrValue ? pstrValue :L"";
	else if (_tcscmp(pstrName, L"autonavi")==0) m_bAutoNavi = (_tcscmp(pstrValue, L"true") == 0);
	else
		activeXUC::setAttribute(pstrName, pstrValue);
}

void webBrowserUC::beforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel )
{
	
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->beforeNavigate2(pDisp,url,Flags,TargetFrameName,PostData,Headers,Cancel);
	}
}
void webBrowserUC::navigateError(IDispatch *pDisp,VARIANT * &url,VARIANT *&TargetFrameName,VARIANT *&StatusCode,VARIANT_BOOL *&Cancel)
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->navigateError(pDisp,url,TargetFrameName,StatusCode,Cancel);
	}
}
void webBrowserUC::navigateComplete2(IDispatch *pDisp,VARIANT *&url)
{
	IDispatch *spDoc = NULL;   
	ICustomDoc *spCustomDoc = NULL;
	m_pWebBrowser2->get_Document(&spDoc);   

	if (spDoc)
	{   
		spDoc->QueryInterface(IID_ICustomDoc, (void**)&spCustomDoc);
		//CComQIPtr<ICustomDoc, &IID_ICustomDoc> spCustomDoc(spDoc);   
		if (spCustomDoc)   
			spCustomDoc->SetUIHandler(this);   
	}

	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->navigateComplete2(pDisp,url);
	}

	// release
	if(spCustomDoc) spCustomDoc->Release();
	if(spDoc) spDoc->Release();
}
void webBrowserUC::progressChange(LONG nProgress, LONG nProgressMax)
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->progressChange(nProgress,nProgressMax);
	}
}
void webBrowserUC::newWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl)
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->newWindow3(pDisp,Cancel,dwFlags,bstrUrlContext,bstrUrl);
	}
}
void webBrowserUC::commandStateChange(long Command,VARIANT_BOOL Enable)
{
	if (m_pWebBrowserEventHandler)
	{
		m_pWebBrowserEventHandler->commandStateChange(Command,Enable);
	}
}

void webBrowserUC::setHomePage(PCWSTR lpszUrl)
{
	m_wsHomePage = lpszUrl ? lpszUrl : L"";
}
std::wstring webBrowserUC::getHomePage() const
{
	return m_wsHomePage;
}

void webBrowserUC::setAutoNavigation(bool bAuto /*= true*/)
{
	if(m_bAutoNavi == bAuto) return;
	m_bAutoNavi = bAuto;
}
bool webBrowserUC::isAutoNavigation() const
{
	return m_bAutoNavi;
}

void webBrowserUC::setWebBrowserEventHandler(IWebBrowserEventHandler* pEventHandler, bool bFreeOnRelease /*= true*/)
{
	if(pEventHandler && pEventHandler != m_pWebBrowserEventHandler)
		m_pWebBrowserEventHandler = pEventHandler;
	m_bFreeOnReleaseWBEH = bFreeOnRelease;
}

void webBrowserUC::navigateBlank()
{
	assert(m_pWebBrowser2);
	if(!m_pWebBrowser2) return;
	VARIANT varEmpty;
	::VariantInit(&varEmpty);
	VARIANT url;
	::VariantInit(&url);
	url.vt = VT_BSTR;
	url.bstrVal = ::SysAllocString(L"about:blank");
	HRESULT hr = m_pWebBrowser2->Navigate2(&url, &varEmpty, &varEmpty, &varEmpty, &varEmpty);

	::VariantClear(&url);

	IDispatch *spDoc = NULL;   
	ICustomDoc *spCustomDoc = NULL;
	m_pWebBrowser2->get_Document(&spDoc);   

	if (spDoc)
	{   
		spDoc->QueryInterface(IID_ICustomDoc, (void**)&spCustomDoc);
		//CComQIPtr<ICustomDoc, &IID_ICustomDoc> spCustomDoc(spDoc);   
		if (spCustomDoc)   
			spCustomDoc->SetUIHandler(this);   
	}

	// release
	if(spCustomDoc) spCustomDoc->Release();
	if(spDoc) spDoc->Release();
}
void webBrowserUC::navigate(PCWSTR lpszUrl)
{
	if (lpszUrl == NULL)
		return;
	IDispatch *pDisp = NULL;
	if (m_pWebBrowser2)
	{
		if(FAILED(m_pWebBrowser2->get_Document(&pDisp)) || !pDisp)
		{
			navigateBlank();
			if(FAILED(m_pWebBrowser2->get_Document(&pDisp)) || !pDisp) throw(NULL);
		}
		//CVariant url;
		//url.vt=VT_BSTR;
		//url.bstrVal=T2BSTR(lpszUrl);
		VARIANT varEmpty;
		::VariantInit(&varEmpty);
		VARIANT url;
		::VariantInit(&url);
		url.vt = VT_BSTR;
		url.bstrVal = ::SysAllocString(lpszUrl);
		HRESULT hr = m_pWebBrowser2->Navigate2(&url, &varEmpty, &varEmpty, &varEmpty, &varEmpty);

		::VariantClear(&url);
	}
	if(pDisp) pDisp->Release();
}

void webBrowserUC::navigate(const std::string &strStreamData)
{
	IDispatch *pDisp = NULL;
	IPersistStreamInit *pPersistStream = NULL;
	IStream *pStream = NULL;
	HGLOBAL hg = NULL;
	assert(m_pWebBrowser2);
	// 显示一个空白网页 
	//navigate(L"about:blank");
	if(strStreamData.length() == 0) return;
	try
	{
		// 
		if(FAILED(m_pWebBrowser2->get_Document(&pDisp)) || !pDisp)
		{
			navigateBlank();
			if(FAILED(m_pWebBrowser2->get_Document(&pDisp)) || !pDisp) throw(NULL);
		}
		if(FAILED(pDisp->QueryInterface(IID_PPV_ARGS(&pPersistStream))) || !pPersistStream) throw(NULL);
		hg = gtc::app::createGlobalHandle(NULL, strStreamData.length(), GPTR);
		//std::strncpy((char *)hg, strStreamData.data(), strStreamData.length());
		memcpy_s(hg, strStreamData.length(), strStreamData.data(), strStreamData.length());
		if(FAILED(CreateStreamOnHGlobal(hg, TRUE, &pStream)) || !pStream)throw(NULL);
		pPersistStream->InitNew();
		//LARGE_INTEGER lg = {0};
		//pStream->Seek(lg, 0, NULL);
		pPersistStream->Load(pStream);
		//refresh();

	}
	catch (gtc::baseException &e)
	{
		SEND_LOG(L"ERROR webBrowserUC::navigate %s", e.description().c_str());
	#if !defined(_DEBUG)
		WRITE_LOG(L"throw webBrowserUC::navigate %s", e.description().c_str());
	#endif
	}
	catch (...)
	{
		SEND_LASTERROR();
	}
	// release
	if(pStream) pStream->Release();
	if(pPersistStream) pPersistStream->Release();
	if(pDisp) pDisp->Release();
	//if(hg) GlobalFree(hg);
}
void webBrowserUC::refresh()
{
	if(m_pWebBrowser2) m_pWebBrowser2->Refresh();
}
void webBrowserUC::refresh2(int Level)
{
	VARIANT varLevel;
	::VariantInit(&varLevel);
	varLevel.vt = VT_I4;
	varLevel.intVal = Level;
	if(m_pWebBrowser2) m_pWebBrowser2->Refresh2(&varLevel);
}
void webBrowserUC::goBack()
{
	if(m_pWebBrowser2) m_pWebBrowser2->GoBack();
}
void webBrowserUC::goForward()
{
	if(m_pWebBrowser2) m_pWebBrowser2->GoForward();
}
void webBrowserUC::navigateHomePage()
{
	if(!m_wsHomePage.empty()) navigateUrl(m_wsHomePage.c_str());
}
void webBrowserUC::navigateUrl(PCWSTR lpszUrl)
{
	if (m_pWebBrowser2 && lpszUrl)
	{
		BSTR bsUrl = ::SysAllocString(lpszUrl);
		m_pWebBrowser2->Navigate(bsUrl, NULL, NULL, NULL, NULL);
		if(bsUrl) ::SysFreeString(bsUrl);
	}
}
bool webBrowserUC::doCreateControl()
{
	if (!activeXUC::doCreateControl())
		return false;
	getManager()->addTranslateAccelerator(this);
	getControl(IID_IWebBrowser2,(LPVOID*)&m_pWebBrowser2);
	if ( m_bAutoNavi && !m_wsHomePage.empty())
	{
		this->navigate(m_wsHomePage.c_str());
	}
	registerEventHandler(TRUE);
	return true;
}
IWebBrowser2* webBrowserUC::getWebBrowser2(void)
{
	return m_pWebBrowser2;
}

IHTMLDocument2* webBrowserUC::getHtmlDocument()
{
	assert(m_pWebBrowser2);
	if(!m_pWebBrowser2) return NULL;
	IDispatch *pDisp = NULL;
	IHTMLDocument2 *pDoc2 = NULL;
	try
	{
		if(FAILED(m_pWebBrowser2->get_Document(&pDisp)) || !pDisp) throw(NULL);
		//assert(pDisp);
		//if(!pDisp) throw(L"");
		if(FAILED(pDisp->QueryInterface(IID_PPV_ARGS(&pDoc2)))) throw(NULL);
	}
	catch (...)
	{
		if(pDoc2) pDoc2->Release();
		pDoc2 = NULL;
	}
	// 
	if(pDisp) pDisp->Release();
	return pDoc2;
}

IHTMLWindow2* webBrowserUC::getHtmlWindow()
{
	assert(m_pWebBrowser2);
	if(!m_pWebBrowser2) return NULL;
	IHTMLWindow2 *pWnd2 = NULL;
	IHTMLDocument2 *pDoc2 = getHtmlDocument();
	if (pDoc2)
	{
		pDoc2->get_parentWindow(&pWnd2);
		pDoc2->Release();
	}
	return pWnd2;
}

IHTMLElement* webBrowserUC::getHtmlElement()
{
	assert(m_pWebBrowser2);
	if(!m_pWebBrowser2) return NULL;
	IHTMLElement *pElem = NULL;
	IHTMLDocument2 *pDoc2 = getHtmlDocument();
	if (pDoc2)
	{
		pDoc2->get_body(&pElem);
		pDoc2->Release();
	}
	return pElem;
}
/*static*/ DISPID webBrowserUC::findId(IDispatch *pObj, LPOLESTR pName)
{
	DISPID id = 0;
	if(FAILED(pObj->GetIDsOfNames(IID_NULL,&pName,1,LOCALE_SYSTEM_DEFAULT,&id))) id = -1;
	return id;
}
/*static*/ HRESULT webBrowserUC::invokeMethod(IDispatch *pObj, LPOLESTR pMehtod, VARIANT *pVarResult, VARIANT *ps, int cArgs)
{
	DISPID dispid = findId(pObj, pMehtod);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS dispparams;
	dispparams.cArgs = cArgs;
	dispparams.rgvarg = ps;
	dispparams.cNamedArgs = 0;
	dispparams.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispparams, pVarResult, NULL, NULL);
}
/*static*/ HRESULT webBrowserUC::getProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue)
{
	DISPID dispid = findId(pObj, pName);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS ps;
	ps.cArgs = 0;
	ps.rgvarg = NULL;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &ps, pValue, NULL, NULL);
}
/*static*/ HRESULT webBrowserUC::setProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue)
{
	DISPID dispid = findId(pObj, pName);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS ps;
	ps.cArgs = 1;
	ps.rgvarg = pValue;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &ps, NULL, NULL, NULL);
}

bool webBrowserUC::getDocumentData(std::wstring *pwsHtml, std::wstring *pwsText /*= NULL*/, std::wstring *pwsUrl /*= NULL*/)
{
	if(pwsHtml) pwsHtml->clear();
	if(pwsText) pwsText->clear();
	if(!pwsHtml && !pwsText && !pwsUrl) return false;
	assert(m_pWebBrowser2);
	if(!m_pWebBrowser2) return false;
	bool bret = false;
	BSTR bs = NULL;
	IDispatch *pDisp = NULL;
	IHTMLDocument3 *pDoc = NULL;
	IHTMLElement *pElement = NULL;
	try
	{
		if(pwsUrl && SUCCEEDED(m_pWebBrowser2->get_LocationURL(&bs)) && bs)
		{
			*pwsUrl = bs;
			::SysFreeString(bs);
			bs = NULL;
		}
		

		if(FAILED(m_pWebBrowser2->get_Document(&pDisp)) || !pDisp) throw(NULL);
		if(FAILED(pDisp->QueryInterface(IID_PPV_ARGS(&pDoc))) || !pDoc) throw(NULL);
		if(FAILED(pDoc->get_documentElement(&pElement)) || !pElement) throw(NULL);
		pDoc->get_baseUrl(&bs);
		pDoc->get_dir(&bs);
		if(pwsHtml)
		{
			pElement->get_innerHTML(&bs);
			if (SUCCEEDED(pElement->get_outerHTML(&bs)) && bs)
			{
				*pwsHtml = bs;
				::SysFreeString(bs);
				bs = NULL;
			}
		}
		if(pwsText)
		{
			pElement->get_innerText(&bs);
			if (SUCCEEDED(pElement->get_outerText(&bs)) && bs)
			{
				*pwsText = bs;
				::SysFreeString(bs);
				bs = NULL;
			}
		}
	}
	catch (gtc::baseException &e)
	{
		SEND_LOG(L"ERROR webBrowserUC::getDocumentData %s", e.description().c_str());
	#if !defined(_DEBUG)
		WRITE_LOG(L"throw webBrowserUC::getDocumentData %s", e.description().c_str());
	#endif
		bret = false;
	}
	catch (...)
	{
		bret = false;
		SEND_LASTERROR();
	}
	// release
	if(pElement) pElement->Release();
	if(pDoc) pDoc->Release();
	if(pDisp) pDisp->Release();
	return bret;
}

void webBrowserUC::initScrollBar()
{
	IHTMLDocument2 *pDoc2 = getHtmlDocument();
	if(!pDoc2) return;
	IHTMLWindow2 *pWin2 = NULL;
	IHTMLElement *pElem1 = NULL;
	IHTMLElement2 *pElem2 = NULL;

	SIZE szScroll = {0};
	pDoc2->get_parentWindow(&pWin2);
	pDoc2->get_body(&pElem1);
	if(pElem1) pElem1->QueryInterface(IID_PPV_ARGS(&pElem2));
	if (pElem2 && pWin2)
	{
		pElem2->get_scrollHeight(&szScroll.cy);
		pElem2->get_scrollWidth(&szScroll.cx);

	}

	RECT rc = m_rcItem;
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;
	if(m_pVerticalScrollBar && szScroll.cy > rectHeight(rc)) rc.right -= m_pVerticalScrollBar->getFixedWidth();
	if(m_pHorizontalScrollBar && szScroll.cx > rectWidth(rc)) rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();
	//if(m_pVerticalScrollBar && m_pVerticalScrollBar->isVisible()) rc.right -= m_pVerticalScrollBar->getFixedWidth();
	//if(m_pHorizontalScrollBar && m_pHorizontalScrollBar->isVisible()) rc.bottom -= m_pHorizontalScrollBar->getFixedHeight();
	processScrollBar(rc, szScroll.cx, szScroll.cy);

	if(pElem1) pElem1->Release();
	if(pElem2) pElem2->Release();
	if(pWin2) pWin2->Release();
	if(pDoc2) pDoc2->Release();
}
STDMETHODIMP webBrowserUC::QueryInterface( REFIID riid, LPVOID *ppvObject )
{
	*ppvObject = NULL;

	if( riid == IID_IDocHostUIHandler)
		*ppvObject = static_cast<IDocHostUIHandler*>(this);
	else if( riid == IID_IDispatch)
		*ppvObject = static_cast<IDispatch*>(this);
	else if( riid == IID_IServiceProvider)
		*ppvObject = static_cast<IServiceProvider*>(this);
	else if (riid == IID_IOleCommandTarget)
		*ppvObject = static_cast<IOleCommandTarget*>(this);

	if( *ppvObject != NULL )
		AddRef();
	return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

STDMETHODIMP_(ULONG) webBrowserUC::AddRef()
{
	InterlockedIncrement(&m_dwRef); 
	return m_dwRef;
}

STDMETHODIMP_(ULONG) webBrowserUC::Release()
{
	ULONG ulRefCount = InterlockedDecrement(&m_dwRef);
	return ulRefCount; 
}


STDMETHODIMP webBrowserUC::GetTypeInfoCount( UINT *iTInfo )
{
	WEB_LOG(L"NOTE webBrowserUC::GetTypeInfoCount");
	*iTInfo = 0;
	return E_NOTIMPL;
}

STDMETHODIMP webBrowserUC::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo )
{
	WEB_LOG(L"NOTE webBrowserUC::GetTypeInfo");
	return E_NOTIMPL;
}

STDMETHODIMP webBrowserUC::GetIDsOfNames( REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid,DISPID *rgDispId )
{
	WEB_LOG(L"NOTE webBrowserUC::GetIDsOfNames");
	return E_NOTIMPL;
}



// IDocHostUIHandler
STDMETHODIMP webBrowserUC::ShowContextMenu( DWORD dwID, POINT* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit )
{
	WEB_LOG(L"NOTE webBrowserUC::ShowContextMenu");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->showContextMenu(dwID,pptPosition,pCommandTarget,pDispatchObjectHit);
	}
	return S_FALSE;
}

STDMETHODIMP webBrowserUC::GetHostInfo( DOCHOSTUIINFO* pInfo )
{
	WEB_LOG(L"NOTE webBrowserUC::GetHostInfo");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->getHostInfo(pInfo);
	}
	return E_NOTIMPL;
}

STDMETHODIMP webBrowserUC::ShowUI( DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc )
{
	WEB_LOG(L"NOTE webBrowserUC::ShowUI");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->showUI(dwID,pActiveObject,pCommandTarget,pFrame,pDoc);
	}
	return S_OK;
}

STDMETHODIMP webBrowserUC::HideUI()
{
	WEB_LOG(L"NOTE webBrowserUC::HideUI");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->hideUI();
	}

	return S_OK;
}

STDMETHODIMP webBrowserUC::UpdateUI()
{
	//WEB_LOG(L"NOTE webBrowserUC::UpdateUI");
	if(!m_bInitScrollbar) 
	{
		WEB_LOG(L"NOTE webBrowserUC::UpdateUI 2");
		m_bInitScrollbar = true;
		initScrollBar();
	}
	//{
	//	IDispatch *pDisp = NULL;
	//	IHTMLDocument2 *pHtml = NULL;
	//	IHTMLElement *pEl = NULL; 
	//	IHTMLBodyElement *pBodyEl = NULL;
	//	IHTMLStyle *phtmlStyle = NULL;
	//	IHTMLWindow2 *pWin2 = NULL;
	//	if (SUCCEEDED(m_pWebBrowser2->get_Document(&pDisp)))
	//	{
	//		pDisp->QueryInterface(IID_PPV_ARGS(&pHtml));
	//		if(pHtml && SUCCEEDED(pHtml->get_body(&pEl))) 
	//		{
	//			pHtml->get_parentWindow(&pWin2);
	//			if(SUCCEEDED(pEl->QueryInterface(IID_IHTMLBodyElement, (void**)&pBodyEl))) 
	//			{ 
	//				//VARIANT var;
	//				//var.vt = VT_BSTR;
	//				//var.bstrVal = L"#FF0000";
	//				////pBodyEl->get_bgColor(&var);
	//				//pBodyEl->put_bgColor(var);
	//				pBodyEl->put_scroll(L"no");    //去滚动条(网页中滚动条设置为no 那么不管IE窗口与网页是否匹配 都不会显示滚动条)
	//			} 
	//			pEl->get_style(&phtmlStyle); 
	//			if(phtmlStyle) 
	//			{ 
	//				//phtmlStyle->put_overflow(L"scroll");
	//				phtmlStyle->put_border(L"none");    //去除边网页边框
	//			} 
	//		}
	//		if(pWin2)
	//		{
	//			long height = 0;
	//			BSTR bs = NULL;
	//			IHTMLElement2 *pE2 = NULL;

	//			/*IHTMLElementCollection *pElemAll = NULL;
	//			pHtml->get_all(&pElemAll);
	//			long len = 0;
	//			if(pElemAll) pElemAll->get_length(&len);

	//			for (long idx = 0; idx < len; ++idx)
	//			{
	//			VARIANT varIdx, var;
	//			::VariantInit(&varIdx);
	//			::VariantInit(&var);
	//			IDispatch *pDisp = NULL;
	//			pElemAll->item(var, varIdx, &pDisp);
	//			if(!pDisp) continue;
	//			pDisp->QueryInterface(IID_PPV_ARGS(&pE2));
	//			if(!pE2) continue;
	//			long h = 0;
	//			pE2->get_scrollHeight(&h);
	//			pE2->put_scrollTop(h);
	//			if(h > height) height = h;
	//			pE2->Release();
	//			pDisp->Release();
	//			pE2 = NULL;
	//			}
	//			pWin2->scrollTo(0, height);*/



	//			pEl->get_outerHTML(&bs);
	//			pEl->QueryInterface(IID_PPV_ARGS(&pE2));
	//			//long sy = 300;
	//			SIZE sz = {0, 300}, hm = {0};
	//			if(pE2) pE2->get_scrollHeight(&sz.cy);
	//			//PixelToHiMetric(&sz, &hm);
	//			pWin2->scrollTo(0, sz.cy);

	//			if(pE2) pE2->Release();

	//			pWin2->Release();
	//			::SysFreeString(bs);
	//		}
	//	}
	//	// release
	//	if(phtmlStyle) phtmlStyle->Release();
	//	if(pBodyEl) pBodyEl->Release();
	//	if(pEl) pEl->Release();
	//	if(pHtml) pHtml->Release();
	//	if(pDisp) pDisp->Release();
	//}	
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->updateUI();
	}
	return S_OK;
}

STDMETHODIMP webBrowserUC::EnableModeless( BOOL fEnable )
{
	WEB_LOG(L"NOTE webBrowserUC::EnableModeless");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->enableModeless(fEnable);
	}
	return E_NOTIMPL;
}

STDMETHODIMP webBrowserUC::OnDocWindowActivate( BOOL fActivate )
{
	WEB_LOG(L"NOTE webBrowserUC::OnDocWindowActivate");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->onDocWindowActivate(fActivate);
	}
	return E_NOTIMPL;
}

STDMETHODIMP webBrowserUC::OnFrameWindowActivate( BOOL fActivate )
{
	WEB_LOG(L"NOTE webBrowserUC::OnFrameWindowActivate");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->onFrameWindowActivate(fActivate);
	}
	return E_NOTIMPL;
}

STDMETHODIMP webBrowserUC::ResizeBorder( LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow, BOOL fFrameWindow )
{
	WEB_LOG(L"NOTE webBrowserUC::ResizeBorder");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->resizeBorder(prcBorder,pUIWindow,fFrameWindow);
	}
	return E_NOTIMPL;
}

STDMETHODIMP webBrowserUC::TranslateAccelerator( LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID )
{
	WEB_LOG(L"NOTE webBrowserUC::TranslateAccelerator");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->translateAccelerator(lpMsg,pguidCmdGroup,nCmdID);
	}
	return S_FALSE;
}

LRESULT webBrowserUC::translateAccelerator( MSG *pMsg )
{
	if(pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST)
		return S_FALSE;

	if( m_pWebBrowser2 == NULL )
		return E_NOTIMPL;

	// 当前Web窗口不是焦点,不处理加速键
	BOOL bIsChild = FALSE;
	HWND hTempWnd = NULL;
	HWND hWndFocus = ::GetFocus();

	hTempWnd = hWndFocus;
	while(hTempWnd != NULL)
	{
		if(hTempWnd == m_hwndHost)
		{
			bIsChild = TRUE;
			break;
		}
		hTempWnd = ::GetParent(hTempWnd);
	}
	if(!bIsChild)
		return S_FALSE;

	IOleInPlaceActiveObject *pObj = NULL;
	if (FAILED(m_pWebBrowser2->QueryInterface(IID_IOleInPlaceActiveObject, (LPVOID *)&pObj)))
		return S_FALSE;

	HRESULT hResult = pObj->TranslateAccelerator(pMsg);
	pObj->Release();
	return hResult;
}

STDMETHODIMP webBrowserUC::GetOptionKeyPath( LPOLESTR* pchKey, DWORD dwReserved )
{
	WEB_LOG(L"NOTE webBrowserUC::GetOptionKeyPath");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->getOptionKeyPath(pchKey,dwReserved);
	}
	return E_NOTIMPL;
}

STDMETHODIMP webBrowserUC::GetDropTarget( IDropTarget* pDropTarget, IDropTarget** ppDropTarget )
{
	WEB_LOG(L"NOTE webBrowserUC::GetDropTarget");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->getDropTarget(pDropTarget,ppDropTarget);
	}
	return S_FALSE;	// 使用系统拖拽
}

STDMETHODIMP webBrowserUC::GetExternal( IDispatch** ppDispatch )
{
	WEB_LOG(L"NOTE webBrowserUC::GetExternal");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->getExternal(ppDispatch);
	}
	return S_FALSE;
}

STDMETHODIMP webBrowserUC::TranslateUrl( DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut )
{
	WEB_LOG(L"NOTE webBrowserUC::TranslateUrl");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->translateUrl(dwTranslate,pchURLIn,ppchURLOut);
	}
	else
	{
		*ppchURLOut = 0;
		return E_NOTIMPL;
	}
}

STDMETHODIMP webBrowserUC::FilterDataObject( IDataObject* pDO, IDataObject** ppDORet )
{
	WEB_LOG(L"NOTE webBrowserUC::FilterDataObject");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->filterDataObject(pDO,ppDORet);
	}
	else
	{
		*ppDORet = 0;
		return E_NOTIMPL;
	}
}

STDMETHODIMP webBrowserUC::QueryService( REFGUID guidService, REFIID riid, void** ppvObject )
{
	WEB_LOG(L"NOTE webBrowserUC::QueryService");
	HRESULT hr = E_NOINTERFACE;
	*ppvObject = NULL;

	if ( guidService == SID_SDownloadManager && riid == IID_IDownloadManager)
	{
		*ppvObject = this;
		return S_OK;
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE webBrowserUC::QueryStatus( __RPC__in_opt const GUID *pguidCmdGroup, ULONG cCmds, __RPC__inout_ecount_full(cCmds ) OLECMD prgCmds[ ], __RPC__inout_opt OLECMDTEXT *pCmdText )
{
	WEB_LOG(L"NOTE webBrowserUC::QueryStatus");
	HRESULT hr = OLECMDERR_E_NOTSUPPORTED;
	return hr;
}

HRESULT STDMETHODCALLTYPE webBrowserUC::Exec( __RPC__in_opt const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, __RPC__in_opt VARIANT *pvaIn, __RPC__inout_opt VARIANT *pvaOut )
{
	WEB_LOG(L"NOTE webBrowserUC::Exec");
	HRESULT hr = S_OK;
	SEND_LOG(L"WARNING webBrowserUC::Exec cmdID:%u cmdExecOpt:%u", nCmdID, nCmdexecopt);
	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
	{

		switch (nCmdID) 
		{

		case OLECMDID_SHOWSCRIPTERROR:
			{
				IHTMLDocument2*             pDoc = NULL;
				IHTMLWindow2*               pWindow = NULL;
				IHTMLEventObj*              pEventObj = NULL;
				BSTR                        rgwszNames[5] = 
				{ 
					SysAllocString(L"errorLine"),
					SysAllocString(L"errorCharacter"),
					SysAllocString(L"errorCode"),
					SysAllocString(L"errorMessage"),
					SysAllocString(L"errorUrl")
				};
				DISPID                      rgDispIDs[5];
				VARIANT                     rgvaEventInfo[5];
				DISPPARAMS                  params;
				BOOL                        fContinueRunningScripts = true;
				int                         i;

				params.cArgs = 0;
				params.cNamedArgs = 0;

				// Get the document that is currently being viewed.
				hr = pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **) &pDoc);    
				// Get document.parentWindow.
				hr = pDoc->get_parentWindow(&pWindow);
				pDoc->Release();
				// Get the window.event object.
				hr = pWindow->get_event(&pEventObj);
				// Get the error info from the window.event object.
				for (i = 0; i < 5; i++) 
				{  
					// Get the property's dispID.
					hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszNames[i], 1, 
						LOCALE_SYSTEM_DEFAULT, &rgDispIDs[i]);
					// Get the value of the property.
					hr = pEventObj->Invoke(rgDispIDs[i], IID_NULL,
						LOCALE_SYSTEM_DEFAULT,
						DISPATCH_PROPERTYGET, &params, &rgvaEventInfo[i],
						NULL, NULL);
					SysFreeString(rgwszNames[i]);
				}

				// At this point, you would normally alert the user with 
				// the information about the error, which is now contained
				// in rgvaEventInfo[]. Or, you could just exit silently.

				(*pvaOut).vt = VT_BOOL;
				if (fContinueRunningScripts)
				{
					// Continue running scripts on the page.
					(*pvaOut).boolVal = VARIANT_TRUE;
				}
				else
				{
					// Stop running scripts on the page.
					(*pvaOut).boolVal = VARIANT_FALSE;   
				}
				if(pWindow) pWindow->Release();
				if(pEventObj) pEventObj->Release();
				break;
			}
		default:
			hr = OLECMDERR_E_NOTSUPPORTED;
			break;
		}
	}
	else
	{
		//IOleCommandTarget *pTarget = NULL;
		//IHTMLDocument2 *pDoc = getHtmlDocument();
		//if (pDoc)
		//{
		//	pDoc->QueryInterface(IID_PPV_ARGS(&pTarget));
		//	if(pTarget) 
		//		hr = pTarget->Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut);
		//	else 
		//		hr = E_NOINTERFACE;
		//}
		//else
		//	hr = OLECMDERR_E_UNKNOWNGROUP;
		//if(pTarget) pTarget->Release();
		//if(pDoc) pDoc->Release();

		hr = OLECMDERR_E_UNKNOWNGROUP;
	}
	return (hr);
}

//////////////////////////////////////////////////////////////////////////////////////////
//	2015-8-10 单位转换	
static void PixelToHiMetric(const SIZEL* lpSizeInPix, LPSIZEL lpSizeInHiMetric)
{
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
	int nPixelsPerInchX;    // Pixels per logical inch along width
	int nPixelsPerInchY;    // Pixels per logical inch along height
	HDC hDCScreen = ::GetDC(NULL);
	nPixelsPerInchX = ::GetDeviceCaps(hDCScreen, LOGPIXELSX);
	nPixelsPerInchY = ::GetDeviceCaps(hDCScreen, LOGPIXELSY);
	::ReleaseDC(NULL, hDCScreen);
	lpSizeInHiMetric->cx = MAP_PIX_TO_LOGHIM(lpSizeInPix->cx, nPixelsPerInchX);
	lpSizeInHiMetric->cy = MAP_PIX_TO_LOGHIM(lpSizeInPix->cy, nPixelsPerInchY);
}
STDMETHODIMP webBrowserUC::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid,WORD wFlags, DISPPARAMS* pDispParams,VARIANT* pVarResult, EXCEPINFO* pExcepInfo,UINT* puArgErr )
{
	if ((riid != IID_NULL))
		return E_INVALIDARG;
	//if(!(dispIdMember == DISPID_COMMANDSTATECHANGE || dispIdMember == DISPID_STATUSTEXTCHANGE))
	//	SEND_LOG(L"WARNING webBrowserUC::Invoke  lcid:%u flag:%u dispID:<%s>", dispIdMember, lcid, toStringIEDispid(dispIdMember).c_str());
	switch(dispIdMember)
	{
	case DISPID_BEFORENAVIGATE2:
		beforeNavigate2(
			pDispParams->rgvarg[6].pdispVal,
			pDispParams->rgvarg[5].pvarVal,
			pDispParams->rgvarg[4].pvarVal,
			pDispParams->rgvarg[3].pvarVal,
			pDispParams->rgvarg[2].pvarVal,
			pDispParams->rgvarg[1].pvarVal,
			pDispParams->rgvarg[0].pboolVal);
		break;
	case DISPID_COMMANDSTATECHANGE:
		commandStateChange(
			pDispParams->rgvarg[1].lVal,
			pDispParams->rgvarg[0].boolVal);
		break;
	case DISPID_NAVIGATECOMPLETE2:
		navigateComplete2(
			pDispParams->rgvarg[1].pdispVal,
			pDispParams->rgvarg[0].pvarVal);
		break;
	case DISPID_NAVIGATEERROR:
		navigateError(
			pDispParams->rgvarg[4].pdispVal,
			pDispParams->rgvarg[3].pvarVal,
			pDispParams->rgvarg[2].pvarVal,
			pDispParams->rgvarg[1].pvarVal,
			pDispParams->rgvarg[0].pboolVal);
		break;
	//case DISPID_STATUSTEXTCHANGE:
	//	break;
		//  	case DISPID_NEWWINDOW2:
		//  		break;
	case DISPID_NEWWINDOW3:
		newWindow3(
			pDispParams->rgvarg[4].ppdispVal,
			pDispParams->rgvarg[3].pboolVal,
			pDispParams->rgvarg[2].uintVal,
			pDispParams->rgvarg[1].bstrVal,
			pDispParams->rgvarg[0].bstrVal);
		break;
	case DISPID_DOWNLOADCOMPLETE:
		{
			SEND_LOG(L"ERROR DISPID_DOWNLOADCOMPLETE");
			m_bInitScrollbar = false;
			if(m_pManager) m_pManager->sendNotify(this, L"downloadcomplete");

			IHTMLElement *pElem1 = getHtmlElement();
			if(!pElem1) break;
			IHTMLBodyElement *pBodyElem = NULL;
			IHTMLStyle *pStyle = NULL;
			if (SUCCEEDED(pElem1->QueryInterface(IID_PPV_ARGS(&pBodyElem))))
			{
				if(m_pVerticalScrollBar) pBodyElem->put_scroll(L"no");    // 去滚动条(网页中滚动条设置为no 那么不管IE窗口与网页是否匹配 都不会显示滚动条)
			}
			if (SUCCEEDED(pElem1->get_style(&pStyle)))
			{
				pStyle->put_border(L"none");    //去除边网页边框
			}
			// release
			if(pStyle) pStyle->Release();
			if(pBodyElem) pBodyElem->Release();
			if(pElem1) pElem1->Release();
		}
		break;
	case DISPID_DOCUMENTCOMPLETE:
		{
			SEND_LOG(L"ERROR DISPID_DOCUMENTCOMPLETE");
			m_bInitScrollbar = false;
			if(m_pManager) m_pManager->sendNotify(this, L"webdocumentcomplete");

			//IHTMLElement *pElem1 = getHtmlElement();
			//if(!pElem1) break;
			//IHTMLBodyElement *pBodyElem = NULL;
			//IHTMLStyle *pStyle = NULL;
			//if (SUCCEEDED(pElem1->QueryInterface(IID_PPV_ARGS(&pBodyElem))))
			//{
			//	if(m_pVerticalScrollBar) pBodyElem->put_scroll(L"no");    // 去滚动条(网页中滚动条设置为no 那么不管IE窗口与网页是否匹配 都不会显示滚动条)
			//}
			//if (SUCCEEDED(pElem1->get_style(&pStyle)))
			//{
			//	pStyle->put_border(L"none");    //去除边网页边框
			//}
			//// release
			//if(pStyle) pStyle->Release();
			//if(pBodyElem) pBodyElem->Release();
			//if(pElem1) pElem1->Release();
		}
		break;
		// 	case DISPID_PROPERTYCHANGE:
		// 		if (pDispParams->cArgs>0 && pDispParams->rgvarg[0].vt == VT_BSTR) {
		// 			TRACE(_T("PropertyChange(%s)\n"), pDispParams->rgvarg[0].bstrVal);
		// 		}
		// 		break;
	default:
		return DISP_E_MEMBERNOTFOUND;
	}
	return S_OK;
}

// IDownloadManager
STDMETHODIMP webBrowserUC::Download( /* [in] */ IMoniker *pmk, /* [in] */ IBindCtx *pbc, /* [in] */ DWORD dwBindVerb, /* [in] */ LONG grfBINDF, /* [in] */ BINDINFO *pBindInfo, /* [in] */ LPCOLESTR pszHeaders, /* [in] */ LPCOLESTR pszRedir, /* [in] */ UINT uiCP )
{
	WEB_LOG(L"NOTE webBrowserUC::Download");
	if (m_pWebBrowserEventHandler)
	{
		return m_pWebBrowserEventHandler->download(pmk,pbc,dwBindVerb,grfBINDF,pBindInfo,pszHeaders,pszRedir,uiCP);
	}
	return S_OK;
}
}	// wuc