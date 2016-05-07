#ifndef __WUCWEBBROWSER_H__
#define __WUCWEBBROWSER_H__
#pragma once
#include <ExDisp.h>
#include <ExDispid.h>
#include <mshtmhst.h>
#include <MsHTML.h>
#include "wucActiveX.h"
#include "drawManager.h"
namespace wuc {
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	IWebBrowserEventHandler  类		2015-8-11
//	
////////////////////////////////////////////////////////////////////////////////////////////////////////
class IWebBrowserEventHandler
{
public:
	virtual void beforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel ){}
	virtual void navigateError(IDispatch *pDisp,VARIANT * &url,VARIANT *&TargetFrameName,VARIANT *&StatusCode,VARIANT_BOOL *&Cancel){}
	virtual void navigateComplete2(IDispatch *pDisp,VARIANT *&url){}
	virtual void progressChange(LONG nProgress, LONG nProgressMax){}
	virtual void newWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl){}
	virtual void commandStateChange(long Command,VARIANT_BOOL Enable){}

	// interface IDocHostUIHandler
	virtual HRESULT STDMETHODCALLTYPE showContextMenu(
		/* [in] */ DWORD dwID,
		/* [in] */ POINT __RPC_FAR *ppt,
		/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
		/* [in] */ IDispatch __RPC_FAR *pdispReserved)
	{
		//return E_NOTIMPL;
		//返回 E_NOTIMPL 正常弹出系统右键菜单
		return S_OK;
		//返回S_OK 则可屏蔽系统右键菜单
	}

	virtual HRESULT STDMETHODCALLTYPE getHostInfo(
		/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo)
	{
		if (pInfo != NULL)
		{
			SEND_LOG(L"IWebBrowserEventHandler::getHostInfo 自定义属性");
			pInfo->cbSize = sizeof(DOCHOSTUIINFO);  
			pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;  
			pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME | 
				DOCHOSTUIFLAG_NO3DOUTERBORDER | DOCHOSTUIFLAG_DIALOG |
				DOCHOSTUIFLAG_DISABLE_HELP_MENU | DOCHOSTUIFLAG_SCROLL_NO;
			LPWSTR m_pZoom = L"BODY{Zoom:100%;}";
			pInfo->pchHostCss = (LPWSTR)::CoTaskMemAlloc((lstrlenW(m_pZoom)+1)*2);
			lstrcpyW(pInfo->pchHostCss, m_pZoom);
		}
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE showUI(
		/* [in] */ DWORD dwID,
		/* [in] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
		/* [in] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
		/* [in] */ IOleInPlaceFrame __RPC_FAR *pFrame,
		/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pDoc)
	{
		return S_FALSE;
	}

	virtual HRESULT STDMETHODCALLTYPE hideUI( void)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE updateUI( void)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE enableModeless(
		/* [in] */ BOOL fEnable)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE onDocWindowActivate(
		/* [in] */ BOOL fActivate)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE onFrameWindowActivate(
		/* [in] */ BOOL fActivate)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE resizeBorder(
		/* [in] */ LPCRECT prcBorder,
		/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
		/* [in] */ BOOL fRameWindow)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE translateAccelerator(
		/* [in] */ LPMSG lpMsg,
		/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
		/* [in] */ DWORD nCmdID)
	{
		return S_FALSE;
	}

	virtual HRESULT STDMETHODCALLTYPE getOptionKeyPath(
		/* [out] */ LPOLESTR __RPC_FAR *pchKey,
		/* [in] */ DWORD dw)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE getDropTarget(
		/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
		/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE getExternal(
		/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE translateUrl(
		/* [in] */ DWORD dwTranslate,
		/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
		/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE filterDataObject(
		/* [in] */ IDataObject __RPC_FAR *pDO,
		/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
	{
		return S_OK;
	}

	// 	virtual HRESULT STDMETHODCALLTYPE GetOverrideKeyPath( 
	// 		/* [annotation][out] */ 
	// 		__deref_out  LPOLESTR *pchKey,
	// 		/* [in] */ DWORD dw)
	// 	{
	// 		return E_NOTIMPL;
	// 	}

	// IDownloadManager
	virtual HRESULT STDMETHODCALLTYPE download( 
		/* [in] */ IMoniker *pmk,
		/* [in] */ IBindCtx *pbc,
		/* [in] */ DWORD dwBindVerb,
		/* [in] */ LONG grfBINDF,
		/* [in] */ BINDINFO *pBindInfo,
		/* [in] */ LPCOLESTR pszHeaders,
		/* [in] */ LPCOLESTR pszRedir,
		/* [in] */ UINT uiCP)
	{
		return S_OK;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	webBrowserUC  类		2015-8-11
//	对IE浏览器封装
////////////////////////////////////////////////////////////////////////////////////////////////////////
class webBrowserUC : public activeXUC, public ITranslateAccelerator
	, public IDocHostUIHandler, public IServiceProvider, public IOleCommandTarget, public IDispatch
{
public:
	webBrowserUC();
	virtual ~webBrowserUC();

	virtual PCWSTR getClass() const;
	virtual LPVOID getInterface(PCWSTR pstrName);
	virtual void Event(BSEventUC &event);
	virtual void setAttribute(PCWSTR pstrName, PCWSTR pstrValue);
	virtual void setPosition(RECT pos);
	virtual void setScrollPos(SIZE szPos);

public:
	void setHomePage(PCWSTR lpszUrl);
	std::wstring getHomePage() const;

	void setAutoNavigation(bool bAuto = true);
	bool isAutoNavigation() const;

	void setWebBrowserEventHandler(IWebBrowserEventHandler* pEventHandler, bool bFreeOnRelease = true);
	void navigate(PCWSTR lpszUrl);
	void navigate(const std::string &strStreamData);
	void refresh();
	void refresh2(int Level);
	void goBack();
	void goForward();
	void navigateHomePage();
	void navigateUrl(PCWSTR lpszUrl);
	void navigateBlank();
	virtual bool doCreateControl();
	IWebBrowser2* getWebBrowser2(void);
	IHTMLDocument2* getHtmlDocument(); 
	IHTMLWindow2* getHtmlWindow();
	IHTMLElement* getHtmlElement();
	static DISPID findId(IDispatch *pObj, LPOLESTR pName);
	static HRESULT invokeMethod(IDispatch *pObj, LPOLESTR pMehtod, VARIANT *pVarResult, VARIANT *ps, int cArgs);
	static HRESULT getProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue);
	static HRESULT setProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue);

public:
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-8-13 得到当前网页数据 pwsHtml = 得到Html格式，pwsText = 仅内容
	bool getDocumentData(std::wstring *pwsHtml, std::wstring *pwsText = NULL, std::wstring *pwsUrl = NULL);
	//////////////////////////////////////////////////////////////////////////////////////////
	//	2015-8-20 刷新滚动条
	void initScrollBar();
public:
	// IUnknown
	STDMETHOD_(ULONG,AddRef)();
	STDMETHOD_(ULONG,Release)();
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

	// IDispatch
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( __RPC__out UINT *pctinfo );
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo );
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId);
	virtual HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );

	// IDocHostUIHandler
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit);
	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO* pInfo);
	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc);
	STDMETHOD(HideUI)();
	STDMETHOD(UpdateUI)();
	STDMETHOD(EnableModeless)(BOOL fEnable);
	STDMETHOD(OnDocWindowActivate)(BOOL fActivate);
	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);
	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow, BOOL fFrameWindow);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID);	//浏览器消息过滤
	STDMETHOD(GetOptionKeyPath)(LPOLESTR* pchKey, DWORD dwReserved);
	STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget);
	STDMETHOD(GetExternal)(IDispatch** ppDispatch);
	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut);
	STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet);

	// IServiceProvider
	STDMETHOD(QueryService)(REFGUID guidService, REFIID riid, void** ppvObject);

	// IOleCommandTarget
	virtual HRESULT STDMETHODCALLTYPE QueryStatus( __RPC__in_opt const GUID *pguidCmdGroup, ULONG cCmds, __RPC__inout_ecount_full(cCmds ) OLECMD prgCmds[ ], __RPC__inout_opt OLECMDTEXT *pCmdText);
	virtual HRESULT STDMETHODCALLTYPE Exec( __RPC__in_opt const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, __RPC__in_opt VARIANT *pvaIn, __RPC__inout_opt VARIANT *pvaOut );

	// IDownloadManager
	STDMETHOD(Download)( 
		/* [in] */ IMoniker *pmk,
		/* [in] */ IBindCtx *pbc,
		/* [in] */ DWORD dwBindVerb,
		/* [in] */ LONG grfBINDF,
		/* [in] */ BINDINFO *pBindInfo,
		/* [in] */ LPCOLESTR pszHeaders,
		/* [in] */ LPCOLESTR pszRedir,
		/* [in] */ UINT uiCP);

	// ITranslateAccelerator
	// Duilib消息分发给WebBrowser
	virtual LRESULT translateAccelerator( MSG *pMsg );
protected:
	
	virtual void releaseControl();
	HRESULT registerEventHandler(BOOL inAdvise);
	// IWebBrowserEventHandler
	virtual void beforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel );
	virtual void navigateError(IDispatch *pDisp,VARIANT * &url,VARIANT *&TargetFrameName,VARIANT *&StatusCode,VARIANT_BOOL *&Cancel);
	virtual void navigateComplete2(IDispatch *pDisp,VARIANT *&url);
	virtual void progressChange(LONG nProgress, LONG nProgressMax);
	virtual void newWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);
	virtual void commandStateChange(long Command,VARIANT_BOOL Enable);

protected:
	IWebBrowser2			*m_pWebBrowser2; //浏览器指针
	//IHTMLWindow2			*m_pHtmlWnd2;
	LONG					m_dwRef;
	DWORD					m_dwCookie;
	std::wstring			m_wsHomePage;	// 默认页面
	bool					m_bAutoNavi;	// 是否启动时打开默认页面
	IWebBrowserEventHandler *m_pWebBrowserEventHandler;	//浏览器事件处理
	bool					m_bFreeOnReleaseWBEH;	// 是否为本对象内释放m_pWebBrowserEventHandler
	bool					m_bInitScrollbar;
};


}	// wuc

#endif	// __WUCWEBBROWSER_H__